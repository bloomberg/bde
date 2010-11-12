// bdem_choicearrayimp.t.cpp                  -*-C++-*-

#include <bdem_choicearrayimp.h>

#include <bdem_functiontemplates.h>
#include <bdem_properties.h>

#include <bdema_sequentialallocator.h>
#include <bdesb_fixedmeminstreambuf.h>
#include <bdesb_memoutstreambuf.h>
#include <bdetu_unset.h>
#include <bdeu_printmethods.h>

#include <bdex_byteinstream.h>
#include <bdex_byteoutstream.h>
#include <bdex_testinstream.h>
#include <bdex_testinstreamexception.h>
#include <bdex_testoutstream.h>

#include <bslalg_typetraits.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslmf_isconvertible.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// The bdem_ChoiceArrayImp serves as the underlying implementation of
// bdem_ChoiceArray.  ChoiceArrayImp in turn delegates it's method calls to
// a DescriptorCatalog, holding the catalog of types that can be chosen,
// and a vector of ChoiceHeaders that holds the actual array members.
// Both delegated classes are defined in the ChoiceHeader component that
// is at a lower level and has already been tested.
//
// The test plan for this class follows the standard design for tests of a
// value semantic component.
//  o [ 1] Breathing test
//  o [ 2] Verify helper methods (populateCatalog)
//  o [ 3] Basic constructors- Tests the functionality of the basic
//                    constructors.  2 variants are truly required, a 3rd is
//                    tested in the same case because it overlaps strongly
//                    with the other 2.
//  o [ 4] Basic modifiers & accessors - Tests the basic modifier and accessor
//                    methods.  insertNullItems,insertNullItems,operator[] are
//                    the key methods.
//  o [ 5] Verify helper methods (populateData) - these helper methods
//                    require accessors and modifiers tested in test case 4.
//  o [ 6] Output operations - operator<<, print()
//  o [ 7] Equality operators - operator==, operator!=
//  o [ 8] Copy constructors
//  o [ 9] Assignment - operator=
//  o [10] Bdex streaming - input and output objects to streams
//  o [11-14] Other manipulators - Any manipulators not tested already
//                   (makeSelection,reset,insertItem,remove)
//  o [15] Choice Descriptor - test the properties of the static Descriptor
//                   object pointed to by the test helper methods
//  o [16] Usage Example
// Note that the private function 'bdexMinorVersion' is tested indirectly.
//-----------------------------------------------------------------------------
//                         ==========================
//                         class bdem_ChoiceArrayImp
//                         ==========================
// CLASS METHODS
// [  ] template <class STREAM>
//      void streamInChoiceArray(
//                  void                                  *obj,
//                  STREAM&                                stream,
//                  int                                    version,
//                  const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
//                  const bdem_Descriptor                 *const attrLookup[]);
// [  ] template <class STREAM>
//      static
//      void streamOutChoiceArray(
//                     const void                             *obj,
//                     STREAM&                                 stream,
//                     int                                     version,
//                     const bdem_DescriptorStreamOut<STREAM>
//                     *strmAttrLookup);
//
// CREATORS
// [ 3] bdem_ChoiceArrayImp(bslma_Allocator *basicAlloc = 0);
// [ 3] bdem_ChoiceArrayImp(bslma_Allocator::AllocationStrategy,
//                       bslma_Allocator *);
// [ 3] bdem_ChoiceArrayImpl(const bdem_ElemType::Type[], int, const
//                        bdem_Descriptor *const *,
//                        bdem_Allocator::AllocationStrategy,
//                        bdem_Allocator *);
// [ 3] ~bdem_ChoiceArrayImpl();
// [ 8] bdem_ChoiceArrayImp(const bdem_ChoiceArrayImp& original,
//                     bslma_Allocator *);
// [ 8] bdem_ChoiceArrayImp(const bdem_ChoiceArrayImp& original,
//                     bdem_AllocationStrategy,
//                     bslma_Allocator *)
//
// MANIPULATORS
// [ 4] void insertNullItems(int dstIndex, int elemCount);
// [ 4] void insertNullItems(int dstIndex, int elemCount);
// [ 4] bdem_ChoiceHeader& operator[] (int index);
// [ 9] bdem_ChoiceArrayImp& operator=(const bdem_ChoiceArrayImp& rhs);
// [10] bdex_InStream& streamIn(bdex_InStream& stream);
// [10] bdex_InStream& streamIn(bdex_InStream& stream, int version);
// [11] bdem_ElemRef makeSelection(int index, int selection);
// [12] void clear();
// [12] void reset();
// [12] void reset(const bdem_ElemType::Type [], int,
//                 const bdem_Descriptor *const);
// [13] void insertItem(int, const bdem_ChoiceHeader&);
// [14] void removeItem(int);
// [14] void removeItems(int,int)
// ACCESSORS
// [ 3] int numSelections() const;
// [ 3] bdem_ElemType::Type selectionType(int selection) const;
// [ 4] const bdem_ChoiceHeader& operator[] (int index) const;
// [ 4] int length() const;
// [ 6] bsl::ostream& print(bsl::ostream& stream,
//                       int           level = 0,
//                       int           spacesPerLevel = 4) const;
// [ 6] operator<<(ostream&, const bdem_ChoiceArrayImp&);
// [ 7] operator==(const bdem_ChoiceArrayImp&, const bdem_ChoiceArrayImp&);
// [ 7] operator!=(const bdem_ChoiceArrayImp&, const bdem_ChoiceArrayImp&);
// [10] bdex_OutStream& streamOut(bdex_OutStream& stream) const;
// [10] bdex_OutStream& streamOut(bdex_OutStream& stream, int) const;
// [15] static const bdem_Descriptor bdem_ChoiceArrayImp::d_choiceArrayAttr;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] populateCatalog
// [ 5] populateData
// [17] USAGE
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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                                      // P(X) w/o '\n'
#define L_ __LINE__                                   // current Line number
#define T_ bsl::cout << "\t" << bsl::flush;      // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdem_ChoiceArrayImp      CAI;

typedef bdem_Properties          Prop;
typedef bdem_Descriptor          Desc;
typedef bdem_ElemType            EType;
typedef bdem_ElemRef             ERef;
typedef bdem_ConstElemRef        CERef;
typedef bdem_ElemType            EType;
typedef bdem_AggregateOption     AggOption;

typedef bsl::vector<EType::Type> Catalog;

typedef bsls_PlatformUtil::Int64 Int64;

typedef bdet_Datetime            Datetime;
typedef bdet_Date                Date;
typedef bdet_Time                Time;
typedef bdet_DatetimeTz          DatetimeTz;
typedef bdet_DateTz              DateTz;
typedef bdet_TimeTz              TimeTz;

typedef bdex_TestInStream        In;
typedef bdex_TestOutStream       Out;

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

#define READ(OBJ, STREAM) {\
OBJ.bdexStreamIn(STREAM, VERSION, inLookup.lookupTable(),DESCRIPTORS);\
}

//=============================================================================
//                           GLOBAL DATA FOR TESTING
//-----------------------------------------------------------------------------

        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Create Three Distinct Exemplars For Each Element Type
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const bool             A22 = true;
const bool             B22 = false;
const bool             N22 = bdetu_Unset<bool>::unsetValue();

const char             A00 = 'A';
const char             B00 = 'B';
const char             N00 = bdetu_Unset<char>::unsetValue();

const short            A01 = -1;
const short            B01 = -2;
const short            N01 = bdetu_Unset<short>::unsetValue();

const int              A02 = 10;
const int              B02 = 20;
const int              N02 = bdetu_Unset<int>::unsetValue();

const bsls_PlatformUtil::Int64
                       A03 = -100;
const bsls_PlatformUtil::Int64
                       B03 = -200;
const bsls_PlatformUtil::Int64
                       N03 =
                           bdetu_Unset<bsls_PlatformUtil::Int64>::unsetValue();

const float            A04 = -1.5;
const float            B04 = -2.5;
const float            N04 = bdetu_Unset<float>::unsetValue();

const double           A05 = 10.5;
const double           B05 = 20.5;
const double           N05 = bdetu_Unset<double>::unsetValue();

const bsl::string      A06 = "one";
const bsl::string      B06 = "two";
const bsl::string      N06 = bdetu_Unset<bsl::string>::unsetValue();

// Note: bdet_Datetime X07 implemented in terms of X08 and X09.

const bdet_Date        A08(2000,  1, 1);
const bdet_Date        B08(9999, 12,31);
const bdet_Date        N08 = bdetu_Unset<bdet_Date>::unsetValue();

const bdet_Time        A09(0, 1, 2, 3);
const bdet_Time        B09(4, 5, 6, 789);
const bdet_Time        N09 = bdetu_Unset<bdet_Time>::unsetValue();

const bdet_Datetime    A07(A08, A09);
const bdet_Datetime    B07(B08, B09);
const bdet_Datetime    N07 = bdetu_Unset<bdet_Datetime>::unsetValue();

const bdet_DateTz        A24(A08, -5);
const bdet_DateTz        B24(B08, -4);
const bdet_DateTz        N24 = bdetu_Unset<bdet_DateTz>::unsetValue();

const bdet_TimeTz        A25(A09, -5);
const bdet_TimeTz        B25(B09, -5);
const bdet_TimeTz        N25 = bdetu_Unset<bdet_TimeTz>::unsetValue();

const bdet_DatetimeTz    A23(A07, -5);
const bdet_DatetimeTz    B23(B07, -5);
const bdet_DatetimeTz    N23 = bdetu_Unset<bdet_DatetimeTz>::unsetValue();

static bsl::vector<bool>                        fA26() {
       bsl::vector<bool> t;          t.push_back(A22); return t; }
static bsl::vector<bool>                        fB26() {
       bsl::vector<bool> t;          t.push_back(B22); return t; }
const  bsl::vector<bool>                         A26 = fA26();
const  bsl::vector<bool>                         B26 = fB26();
const  bsl::vector<bool>                         N26;

static bsl::vector<char>                        fA10() {
       bsl::vector<char> t;          t.push_back(A00); return t; }
static bsl::vector<char>                        fB10() {
       bsl::vector<char> t;          t.push_back(B00); return t; }
const  bsl::vector<char>                         A10 = fA10();
const  bsl::vector<char>                         B10 = fB10();
const  bsl::vector<char>                         N10;

static bsl::vector<short>                       fA11() {
       bsl::vector<short> t;         t.push_back(A01); return t; }
static bsl::vector<short>                       fB11() {
       bsl::vector<short> t;         t.push_back(B01); return t; }
const  bsl::vector<short>                        A11 = fA11();
const  bsl::vector<short>                        B11 = fB11();
const  bsl::vector<short>                        N11;

static bsl::vector<int>                         fA12() {
       bsl::vector<int> t;           t.push_back(A02); return t; }
static bsl::vector<int>                         fB12() {
       bsl::vector<int> t;           t.push_back(B02); return t; }
const  bsl::vector<int>                          A12 = fA12();
const  bsl::vector<int>                          B12 = fB12();
const  bsl::vector<int>                          N12;

static bsl::vector<bsls_PlatformUtil::Int64>    fA13() {
       bsl::vector<bsls_PlatformUtil::Int64> t;
                                     t.push_back(A03); return t; }
static bsl::vector<bsls_PlatformUtil::Int64>    fB13() {
       bsl::vector<bsls_PlatformUtil::Int64> t;
                                     t.push_back(B03); return t; }
const  bsl::vector<bsls_PlatformUtil::Int64>     A13 = fA13();
const  bsl::vector<bsls_PlatformUtil::Int64>     B13 = fB13();
const  bsl::vector<bsls_PlatformUtil::Int64>     N13;

static bsl::vector<float>                       fA14() {
       bsl::vector<float> t;         t.push_back(A04); return t; }
static bsl::vector<float>                       fB14() {
       bsl::vector<float> t;         t.push_back(B04); return t; }
const  bsl::vector<float>                        A14 = fA14();
const  bsl::vector<float>                        B14 = fB14();
const  bsl::vector<float>                        N14;

static bsl::vector<double>                      fA15() {
       bsl::vector<double> t;        t.push_back(A05); return t; }
static bsl::vector<double>                      fB15() {
       bsl::vector<double> t;        t.push_back(B05); return t; }
const  bsl::vector<double>                       A15 = fA15();
const  bsl::vector<double>                       B15 = fB15();
const  bsl::vector<double>                       N15;

static bsl::vector<bsl::string>                 fA16() {
       bsl::vector<bsl::string> t;   t.push_back(A06); return t; }
static bsl::vector<bsl::string>                 fB16() {
       bsl::vector<bsl::string> t;   t.push_back(B06); return t; }
const  bsl::vector<bsl::string>                  A16 = fA16();
const  bsl::vector<bsl::string>                  B16 = fB16();
const  bsl::vector<bsl::string>                  N16;

static bsl::vector<bdet_Datetime>               fA17() {
       bsl::vector<bdet_Datetime> t; t.push_back(A07); return t; }
static bsl::vector<bdet_Datetime>               fB17() {
       bsl::vector<bdet_Datetime> t; t.push_back(B07); return t; }
const  bsl::vector<bdet_Datetime>                A17 = fA17();
const  bsl::vector<bdet_Datetime>                B17 = fB17();
const  bsl::vector<bdet_Datetime>                N17;

static bsl::vector<bdet_Date>                   fA18() {
       bsl::vector<bdet_Date> t;     t.push_back(A08); return t; }
static bsl::vector<bdet_Date>                   fB18() {
       bsl::vector<bdet_Date> t;     t.push_back(B08); return t; }
const  bsl::vector<bdet_Date>                    A18 = fA18();
const  bsl::vector<bdet_Date>                    B18 = fB18();
const  bsl::vector<bdet_Date>                    N18;

static bsl::vector<bdet_Time>                   fA19() {
       bsl::vector<bdet_Time> t;     t.push_back(A09); return t; }
static bsl::vector<bdet_Time>                   fB19() {
       bsl::vector<bdet_Time> t;     t.push_back(B09); return t; }
const  bsl::vector<bdet_Time>                    A19 = fA19();
const  bsl::vector<bdet_Time>                    B19 = fB19();
const  bsl::vector<bdet_Time>                    N19;

static bsl::vector<bdet_DatetimeTz>               fA27() {
       bsl::vector<bdet_DatetimeTz> t; t.push_back(A23); return t; }
static bsl::vector<bdet_DatetimeTz>               fB27() {
       bsl::vector<bdet_DatetimeTz> t; t.push_back(B23); return t; }
const  bsl::vector<bdet_DatetimeTz>                A27 = fA27();
const  bsl::vector<bdet_DatetimeTz>                B27 = fB27();
const  bsl::vector<bdet_DatetimeTz>                N27;

static bsl::vector<bdet_DateTz>                   fA28() {
       bsl::vector<bdet_DateTz> t;     t.push_back(A24); return t; }
static bsl::vector<bdet_DateTz>                   fB28() {
       bsl::vector<bdet_DateTz> t;     t.push_back(B24); return t; }
const  bsl::vector<bdet_DateTz>                    A28 = fA28();
const  bsl::vector<bdet_DateTz>                    B28 = fB28();
const  bsl::vector<bdet_DateTz>                    N28;

static bsl::vector<bdet_TimeTz>                   fA29() {
       bsl::vector<bdet_TimeTz> t;     t.push_back(A25); return t; }
static bsl::vector<bdet_TimeTz>                   fB29() {
       bsl::vector<bdet_TimeTz> t;     t.push_back(B25); return t; }
const  bsl::vector<bdet_TimeTz>                    A29 = fA29();
const  bsl::vector<bdet_TimeTz>                    B29 = fB29();
const  bsl::vector<bdet_TimeTz>                    N29;

        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Empty Dummy descriptors for list, table, choice and choiceArray
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const bdem_Descriptor listAttr = {
    bdem_ElemType::BDEM_LIST,                                // type
    0,                                                  // size
    0,                                                  // alignment
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

const bdem_Descriptor tableAttr = {
    bdem_ElemType::BDEM_TABLE,                               // type
    0,                                                  // size
    0,                                                  // alignment
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

const bdem_Descriptor choiceAttr = {
    bdem_ElemType::BDEM_CHOICE,                              // type
    0,                                                  // size
    0,                                                  // alignment
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

const bdem_Descriptor choiceArrayAttr = {
    bdem_ElemType::BDEM_CHOICE_ARRAY,                        // type
    0,                                                  // size
    0,                                                  // alignment
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static const EType::Type cElemTypes[] = {
    EType::BDEM_CHAR,            //  0
    EType::BDEM_SHORT,           //  1
    EType::BDEM_INT,             //  2
    EType::BDEM_INT64,           //  3
    EType::BDEM_FLOAT,           //  4
    EType::BDEM_DOUBLE,          //  5
    EType::BDEM_STRING,          //  6
    EType::BDEM_DATETIME,        //  7
    EType::BDEM_DATE,            //  8
    EType::BDEM_TIME,            //  9
    EType::BDEM_CHAR_ARRAY,      // 10
    EType::BDEM_SHORT_ARRAY,     // 11
    EType::BDEM_INT_ARRAY,       // 12
    EType::BDEM_INT64_ARRAY,     // 13
    EType::BDEM_FLOAT_ARRAY,     // 14
    EType::BDEM_DOUBLE_ARRAY,    // 15
    EType::BDEM_STRING_ARRAY,    // 16
    EType::BDEM_DATETIME_ARRAY,  // 17
    EType::BDEM_DATE_ARRAY,      // 18
    EType::BDEM_TIME_ARRAY,      // 19
    EType::BDEM_LIST,            // 20
    EType::BDEM_TABLE,           // 21
    EType::BDEM_BOOL,            // 22
    EType::BDEM_DATETIMETZ,      // 23
    EType::BDEM_DATETZ,          // 24
    EType::BDEM_TIMETZ,          // 25
    EType::BDEM_BOOL_ARRAY,      // 26
    EType::BDEM_DATETIMETZ_ARRAY,// 27
    EType::BDEM_DATETZ_ARRAY,    // 28
    EType::BDEM_TIMETZ_ARRAY,    // 29
    EType::BDEM_CHOICE,          // 30
    EType::BDEM_CHOICE_ARRAY,    // 31
};

const int NUM_CTYPES = sizeof cElemTypes / sizeof *cElemTypes;

static char SPECIFICATIONS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
    // This string stores the valid specification values used for constructing
    // a TypesCatalog.  Each character specifies an element type and the order
    // for character to element type mapping is similar to that in
    // bdem_elemtype.h.  Thus, 'A' corresponds to 'CHAR', 'B'- 'SHORT',
    // ... 'Z' - 'TIMETZ', 'a' - 'BOOL_ARRAY', ... 'f' - 'CHOICE_ARRAY'.
    // Additionally for this test 'g' - 'SmallString', 'h' -
    // 'SmallStringAlloc', 'i' - 'LargeString' and 'j' - 'LargeStringAlloc'.
const int SPEC_LEN = sizeof SPECIFICATIONS - 1;

static const Desc *DESCRIPTORS[] =
    // This array stores the descriptors corresponding to each element
    // type specified in the spec string.
{
    &Prop::d_charAttr,
    &Prop::d_shortAttr,
    &Prop::d_intAttr,
    &Prop::d_int64Attr,
    &Prop::d_floatAttr,
    &Prop::d_doubleAttr,
    &Prop::d_stringAttr,
    &Prop::d_datetimeAttr,
    &Prop::d_dateAttr,
    &Prop::d_timeAttr,
    &Prop::d_charArrayAttr,
    &Prop::d_shortArrayAttr,
    &Prop::d_intArrayAttr,
    &Prop::d_int64ArrayAttr,
    &Prop::d_floatArrayAttr,
    &Prop::d_doubleArrayAttr,
    &Prop::d_stringArrayAttr,
    &Prop::d_datetimeArrayAttr,
    &Prop::d_dateArrayAttr,
    &Prop::d_timeArrayAttr,
    &listAttr,
    &tableAttr,
    &Prop::d_boolAttr,
    &Prop::d_datetimeTzAttr,
    &Prop::d_dateTzAttr,
    &Prop::d_timeTzAttr,
    &Prop::d_boolArrayAttr,
    &Prop::d_datetimeTzArrayAttr,
    &Prop::d_dateTzArrayAttr,
    &Prop::d_timeTzArrayAttr,
    &choiceAttr,
    &choiceArrayAttr,
};
const int NUM_DESCS = sizeof DESCRIPTORS / sizeof *DESCRIPTORS;

static const AggOption::AllocationStrategy PASSTH =
                                                  AggOption::BDEM_PASS_THROUGH;

                        // ===========================
                        // Dummy streaming descriptors
                        // ===========================

                        // -------------------------
                        // struct streamInAttrLookup
                        // -------------------------

template <class STREAM>
struct streamInAttrLookup {
    static const bdem_DescriptorStreamIn<STREAM>* lookupTable();
};

template <class STREAM>
const bdem_DescriptorStreamIn<STREAM>*
streamInAttrLookup<STREAM>::lookupTable()
{
    static const bdem_DescriptorStreamIn<STREAM>
        table[] =
    {
        { &bdem_FunctionTemplates::streamInFundamental<char,STREAM> },
        { &bdem_FunctionTemplates::streamInFundamental<short,STREAM> },
        { &bdem_FunctionTemplates::streamInFundamental<int,STREAM> },
        { &bdem_FunctionTemplates::
                        streamInFundamental<bsls_PlatformUtil::Int64,STREAM> },
        { &bdem_FunctionTemplates::streamInFundamental<float,STREAM> },
        { &bdem_FunctionTemplates::streamInFundamental<double,STREAM> },
        { &bdem_FunctionTemplates::streamIn<bsl::string,STREAM> },
        { &bdem_FunctionTemplates::streamIn<bdet_Datetime,STREAM> },
        { &bdem_FunctionTemplates::streamIn<bdet_Date,STREAM> },
        { &bdem_FunctionTemplates::streamIn<bdet_Time,STREAM> },
        { &bdem_FunctionTemplates::streamInArray<char,STREAM> },
        { &bdem_FunctionTemplates::streamInArray<short,STREAM> },
        { &bdem_FunctionTemplates::streamInArray<int,STREAM> },
        { &bdem_FunctionTemplates::
                              streamInArray<bsls_PlatformUtil::Int64,STREAM> },
        { &bdem_FunctionTemplates::streamInArray<float,STREAM> },
        { &bdem_FunctionTemplates::streamInArray<double,STREAM> },
        { &bdem_FunctionTemplates::streamInArray<bsl::string,STREAM> },
        { &bdem_FunctionTemplates::streamInArray<bdet_Datetime,STREAM>},
        { &bdem_FunctionTemplates::streamInArray<bdet_Date,STREAM> },
        { &bdem_FunctionTemplates::streamInArray<bdet_Time, STREAM> },
        { 0 },
        { 0 },

        { &bdem_FunctionTemplates::streamInFundamental<bool,STREAM> },
        { &bdem_FunctionTemplates::streamIn<bdet_DatetimeTz,STREAM> },
        { &bdem_FunctionTemplates::streamIn<bdet_DateTz,STREAM> },
        { &bdem_FunctionTemplates::streamIn<bdet_TimeTz,STREAM> },
        { &bdem_FunctionTemplates::streamInArray<bool,STREAM> },
        { &bdem_FunctionTemplates::streamInArray<bdet_DatetimeTz,STREAM>},
        { &bdem_FunctionTemplates::streamInArray<bdet_DateTz,STREAM> },
        { &bdem_FunctionTemplates::streamInArray<bdet_TimeTz, STREAM> },

        { 0 },
        { 0 },
    };

    return table;
}

                        // --------------------------
                        // struct streamOutAttrLookup
                        // --------------------------

template <class STREAM>
struct streamOutAttrLookup {
    static const bdem_DescriptorStreamOut<STREAM>* lookupTable();
};

template <class STREAM>
const bdem_DescriptorStreamOut<STREAM>*
streamOutAttrLookup<STREAM>::lookupTable()
{
    static const bdem_DescriptorStreamOut<STREAM>
        table[] =
    {
        { &bdem_FunctionTemplates::streamOutFundamental<char,STREAM> },
        { &bdem_FunctionTemplates::streamOutFundamental<short,STREAM> },
        { &bdem_FunctionTemplates::streamOutFundamental<int,STREAM> },
        { &bdem_FunctionTemplates::
                      streamOutFundamental<bsls_PlatformUtil::Int64, STREAM> },
        { &bdem_FunctionTemplates::streamOutFundamental<float,STREAM> },
        { &bdem_FunctionTemplates::streamOutFundamental<double,STREAM> },
        { &bdem_FunctionTemplates::streamOut<bsl::string,STREAM> },
        { &bdem_FunctionTemplates::streamOut<bdet_Datetime,STREAM> },
        { &bdem_FunctionTemplates::streamOut<bdet_Date,STREAM> },
        { &bdem_FunctionTemplates::streamOut<bdet_Time,STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<char, STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<short, STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<int, STREAM> },
        { &bdem_FunctionTemplates::
                            streamOutArray<bsls_PlatformUtil::Int64, STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<float, STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<double, STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bsl::string, STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_Datetime, STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_Date, STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_Time, STREAM> },
        { 0 },
        { 0 },

        { &bdem_FunctionTemplates::streamOutFundamental<bool,STREAM> },
        { &bdem_FunctionTemplates::streamOut<bdet_DatetimeTz,STREAM> },
        { &bdem_FunctionTemplates::streamOut<bdet_DateTz,STREAM> },
        { &bdem_FunctionTemplates::streamOut<bdet_TimeTz,STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bool, STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_DatetimeTz, STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_DateTz, STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_TimeTz, STREAM> },

        // TBD uncomment when bdem_Choice, bdem_ChoiceArray is ready
        { 0 },
        { 0 },
    };

    return table;
}

static const void *VALUES_A[] =
    // This array stores a set of values corresponding to each element
    // type specified in the spec string.
{
    &A00,
    &A01,
    &A02,
    &A03,
    &A04,
    &A05,
    &A06,
    &A07,
    &A08,
    &A09,
    &A10,
    &A11,
    &A12,
    &A13,
    &A14,
    &A15,
    &A16,
    &A17,
    &A18,
    &A19,
    0, // Empty Value for bdem_List
    0, // Empty Value for bdem_Table
    &A22,
    &A23,
    &A24,
    &A25,
    &A26,
    &A27,
    &A28,
    &A29,
    0, // Empty Value for bdem_Choice
    0, // Empty Value for bdem_ChoiceArray
};
const int NUM_VALUESA = sizeof VALUES_A / sizeof *VALUES_A;

static const void *VALUES_B[] =
    // This array stores a second set of values corresponding to each element
    // type specified in the spec string.
{
    &B00,
    &B01,
    &B02,
    &B03,
    &B04,
    &B05,
    &B06,
    &B07,
    &B08,
    &B09,
    &B10,
    &B11,
    &B12,
    &B13,
    &B14,
    &B15,
    &B16,
    &B17,
    &B18,
    &B19,
    0, // Empty Value for bdem_List
    0, // Empty Value for bdem_Table
    &B22,
    &B23,
    &B24,
    &B25,
    &B26,
    &B27,
    &B28,
    &B29,
    0, // Empty Value for bdem_Choice
    0, // Empty Value for bdem_ChoiceArray
};
const int NUM_VALUESB = sizeof VALUES_B / sizeof *VALUES_B;

static const void *VALUES_N[] =
    // This array stores unset values corresponding to each element
    // type specified in the spec string.
{
    &N00,
    &N01,
    &N02,
    &N03,
    &N04,
    &N05,
    &N06,
    &N07,
    &N08,
    &N09,
    &N10,
    &N11,
    &N12,
    &N13,
    &N14,
    &N15,
    &N16,
    &N17,
    &N18,
    &N19,
    0, // Empty Value for bdem_List
    0, // Empty Value for bdem_Table
    &N22,
    &N23,
    &N24,
    &N25,
    &N26,
    &N27,
    &N28,
    &N29,
    0, // Empty Value for bdem_Choice
    0, // Empty Value for bdem_ChoiceArray
};
const int NUM_VALUESN = sizeof VALUES_N / sizeof *VALUES_N;

struct tempStruct {
    static int assertion1[SPEC_LEN == NUM_DESCS];
    static int assertion2[SPEC_LEN == NUM_VALUESA];
    static int assertion3[SPEC_LEN == NUM_VALUESB];
    static int assertion4[SPEC_LEN == NUM_VALUESN];
    static int assertion5[SPEC_LEN == NUM_CTYPES];
};

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static int compare(const void *p, const void *q, char spec)
    // Compare the specified 'p' and 'q' void pointers by casting them to the
    // data type corresponding to the specified 'spec' value.  Return true if
    // the two values are equal and false otherwise.
{
    switch (spec) {
      case 'A': return *(char *) p == *(char *) q;
      case 'B': return *(short *) p == *(short *) q;
      case 'C': return *(int *) p == *(int *) q;
      case 'D': return *(Int64 *) p == *(Int64 *) q;
      case 'E': return *(float *) p == *(float *) q;
      case 'F': return *(double *) p == *(double *) q;
      case 'G': return *(bsl::string *) p == *(bsl::string *) q;
      case 'H': return *(Datetime *) p == *(Datetime *) q;
      case 'I': return *(Date *) p == *(Date *) q;
      case 'J': return *(Time *) p == *(Time *) q;
      case 'K': return *(bsl::vector<char> *) p == *(bsl::vector<char> *) q;
      case 'L': return *(bsl::vector<short> *) p == *(bsl::vector<short> *) q;
      case 'M': return *(bsl::vector<int> *) p == *(bsl::vector<int> *) q;
      case 'N': return *(bsl::vector<Int64> *) p == *(bsl::vector<Int64> *) q;
      case 'O': return *(bsl::vector<float> *) p == *(bsl::vector<float> *) q;
      case 'P': return
                     *(bsl::vector<double> *) p == *(bsl::vector<double> *) q;
      case 'Q': return *(bsl::vector<bsl::string> *) p
                                            == *(bsl::vector<bsl::string> *) q;
      case 'R': return *(bsl::vector<Datetime> *) p
                                            == *(bsl::vector<Datetime> *) q;
      case 'S': return *(bsl::vector<Date> *) p == *(bsl::vector<Date> *) q;
      case 'T': return *(bsl::vector<Time> *) p == *(bsl::vector<Time> *) q;
      case 'U': ASSERT(0); return 1; // Comparing bdem_List
      case 'V': ASSERT(0); return 1; // Comparing bdem_Table
      case 'W': return *(bool *) p == *(bool *) q;
      case 'X': return *(DatetimeTz *) p == *(DatetimeTz *) q;
      case 'Y': return *(DateTz *) p == *(DateTz *) q;
      case 'Z': return *(TimeTz *) p == *(TimeTz *) q;
      case 'a': return *(bsl::vector<bool> *) p == *(bsl::vector<bool> *) q;
      case 'b': return *(bsl::vector<DatetimeTz> *) p
                                             == *(bsl::vector<DatetimeTz> *) q;
      case 'c': return *(bsl::vector<DateTz> *) p
                                             == *(bsl::vector<DateTz> *) q;
      case 'd': return *(bsl::vector<TimeTz> *) p
                                             == *(bsl::vector<TimeTz> *) q;
      case 'e': ASSERT(0); return 1; // Comparing bdem_Choice
      case 'f': ASSERT(0); return 1; // Comparing bdem_ChoiceArray
      default: ASSERT(0); return 0;
    }
}

static void assign(void *lhs, const void *rhs, char spec)
    // Assign to the specified 'lhs' the value of the specified 'rhs'
    // according to the specified 'spec'.
{
    switch (spec) {
      case 'A': *(char   *) lhs = *(char   *) rhs; break;
      case 'B': *(short  *) lhs = *(short  *) rhs; break;
      case 'C': *(int    *) lhs = *(int    *) rhs; break;
      case 'D': *(Int64  *) lhs = *(Int64  *) rhs; break;
      case 'E': *(float  *) lhs = *(float  *) rhs; break;
      case 'F': *(double *) lhs = *(double *) rhs; break;
      case 'G': *(bsl::string *) lhs = *(bsl::string *) rhs; break;
      case 'H': *(Datetime *) lhs = *(Datetime *) rhs; break;
      case 'I': *(Date *) lhs = *(Date *) rhs; break;
      case 'J': *(Time *) lhs = *(Time *) rhs; break;
      case 'K': *(bsl::vector<char>   *) lhs =
                                            *(bsl::vector<char> *) rhs; break;
      case 'L': *(bsl::vector<short>  *) lhs =
                                           *(bsl::vector<short> *) rhs; break;
      case 'M': *(bsl::vector<int>    *) lhs =
                                             *(bsl::vector<int> *) rhs; break;
      case 'N': *(bsl::vector<Int64>  *) lhs =
                                           *(bsl::vector<Int64> *) rhs; break;
      case 'O': *(bsl::vector<float>  *) lhs =
                                           *(bsl::vector<float> *) rhs; break;
      case 'P': *(bsl::vector<double> *) lhs =
                                          *(bsl::vector<double> *) rhs; break;
      case 'Q': *(bsl::vector<bsl::string> *) lhs =
                                     *(bsl::vector<bsl::string> *) rhs; break;
      case 'R': *(bsl::vector<Datetime> *) lhs =
                                        *(bsl::vector<Datetime> *) rhs; break;
      case 'S': *(bsl::vector<Date> *) lhs =
                                            *(bsl::vector<Date> *) rhs; break;
      case 'T': *(bsl::vector<Time> *) lhs =
                                            *(bsl::vector<Time> *) rhs; break;
      case 'U': ASSERT(0); return; // Assigning bdem_List
      case 'V': ASSERT(0); return; // Assigning bdem_Table
      case 'W': *(bool *) lhs = *(bool *) rhs; break;
      case 'X': *(DatetimeTz *) lhs = *(DatetimeTz *) rhs; break;
      case 'Y': *(DateTz *) lhs = *(DateTz *) rhs; break;
      case 'Z': *(TimeTz *) lhs = *(TimeTz *) rhs; break;
      case 'a': *(bsl::vector<bool> *) lhs = *(bsl::vector<bool> *) rhs; break;
      case 'b': *(bsl::vector<DatetimeTz> *) lhs =
                                       *(bsl::vector<DatetimeTz> *) rhs; break;
      case 'c': *(bsl::vector<DateTz> *) lhs =
                                          *(bsl::vector<DateTz> *) rhs; break;
      case 'd': *(bsl::vector<TimeTz> *) lhs =
                                          *(bsl::vector<TimeTz> *) rhs; break;
      case 'e': ASSERT(0); return; // Comparing bdem_Choice
      case 'f': ASSERT(0); return; // Comparing bdem_ChoiceArray
      default: ASSERT(0); return;
    }
}

static EType::Type getElemType(char spec)
    // Return the element type corresponding to the specified
    // 'spec' value.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".
{
    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return (EType::Type) index;
}

static const Desc *getDescriptor(char spec)
    // Return the non-modifiable descriptor corresponding to the specified
    // 'spec' value.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".
{
    int index = getElemType(spec);
    return DESCRIPTORS[index];
}

static const void *getValueA(char spec)
    // Return the 'A' value corresponding to the specified 'spec'.  Valid
    // input consists of uppercase letters where the index of each letter is
    // in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".
{
    int index = getElemType(spec);
    return VALUES_A[index];
}

static const void *getValueB(char spec)
    // Return the 'B' value corresponding to the specified 'spec'.
{
    int index = getElemType(spec);
    return VALUES_B[index];
}

static const void *getValueN(char spec)
    // Return the 'N' value corresponding to the specified 'spec'.
{
    int index = getElemType(spec);
    return VALUES_N[index];
}

static void populateCatalog(Catalog *catalog, const char *spec)
    // Populate the specified descriptor 'catalog' according to the specified
    // 'spec' string.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".  Note
    // that the order of characters in the spec string is similar to their
    // bdem_ElemType values.  Thus 'A' - CHAR, 'B' - SHORT .. 'Z' - TIMETZ, 'a'
    // - BOOL_ARRAY .. 'f' - CHOICE_ARRAY.
{

    ASSERT(catalog);
    ASSERT(spec);

    catalog->clear();
    for (const char *s = spec; *s; ++s) {
        if (' ' == *s || '\t' == *s || '\n' == *s) continue; // ignore WS
        catalog->push_back(getElemType(*s));
    }
}

static void populateData(CAI *X, const void **VALUES)
    // populates array X with dummy data.  Uses the size of the catalog
    // to determine the size of the array to populate.  For each element i
    // it selects the type at index i from the catalog and looks up a value for
    // that type from the VALUES array.
    // The VALUES array must be an array of values indexed by the types
    // (e.g. VALUES_A)
    // The catalog must also not contain any aggregates such as table
    // or List since we cannot create objects for them
    // Uses operations: insertNullItems, numSelections, selectionType, []
    // (they must be tested prior to use)
{
    const int size = X->numSelections();
    int spaceNeeded = size - X->length();
    if (spaceNeeded < 0)
        spaceNeeded = 0;

    X->insertNullItems(0,spaceNeeded);
    for (int i=0; i < size; ++i) {
        EType::Type type = X->selectionType(i);
        const void *VALUE = VALUES[(int)type];
        X->theModifiableItem(i).makeSelection(i,VALUE);
    }
}

static void write(const Catalog *catalog)
    // Send the catalog to cout
{
    cout << "Types [";
    Catalog::const_iterator i= catalog->begin();
    while (i!=catalog->end())
    {
        if (i!=catalog->begin())
        {
            cout << ", ";
        }
        cout << *i;
        ++i;
    }
    cout << "]";
}

static bool isNull(const CAI& cai, int index)
    //
{
    return cai.theItem(index).isSelectionNull();
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

const bdem_Descriptor *const MY_DESCRIPTORS[] = {

    // Element attribute structures for fundamental and basic types:
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

    // Element attribute structures for array types:
    &bdem_Properties::d_charArrayAttr,
    &bdem_Properties::d_shortArrayAttr,
    &bdem_Properties::d_intArrayAttr,
    &bdem_Properties::d_int64ArrayAttr,
    &bdem_Properties::d_floatArrayAttr,
    &bdem_Properties::d_doubleArrayAttr,
    &bdem_Properties::d_stringArrayAttr,
    &bdem_Properties::d_datetimeArrayAttr,
    &bdem_Properties::d_dateArrayAttr,
    &bdem_Properties::d_timeArrayAttr,

    // Element attribute structures for list and table types
    (const bdem_Descriptor *) 0,
    (const bdem_Descriptor *) 0,

    // New bdem data types
    &bdem_Properties::d_boolAttr,
    &bdem_Properties::d_datetimeTzAttr,
    &bdem_Properties::d_dateTzAttr,
    &bdem_Properties::d_timeTzAttr,
    &bdem_Properties::d_boolArrayAttr,
    &bdem_Properties::d_datetimeTzArrayAttr,
    &bdem_Properties::d_dateTzArrayAttr,
    &bdem_Properties::d_timeTzArrayAttr,

    (const bdem_Descriptor *) 0,
    (const bdem_Descriptor *) 0
};

///Usage
///-----
// The class bdem_ChoiceArrayImp class can be used to store arrays of choice
// data, where the type of the value in each element can be selected from a
// catalog of types held by the object.
//
// Each item in a choice array imp uses the same catalog as the choice array
// imp itself and can specify its chosen selection type and selection value
// independently.
//
// In the following example we use an array of data representing the results of
// some computation we wish to display.
//
//   [ 12  "No Data"  23  52  "Dived by zero"]
//
// Here each element is either an integer or a string describing why data is
// not available for that element.
//
// The example will show how to populate the choice array imp with this data.
// The following function will return a choice array imp representing the
// above data:
//..
 bdem_ChoiceArrayImp computeResults() {
//..
// First, we create the types catalog:
//..
       const bdem_ElemType::Type CHOICE_TYPES[] = {
           bdem_ElemType::BDEM_INT,     // result value
           bdem_ElemType::BDEM_STRING,  // input error
       };
       const int NUM_CHOICES =  sizeof CHOICE_TYPES / sizeof *CHOICE_TYPES;
       enum { RESULT_VAL, ERROR };
//..
// We are now ready to construct our ChoiceArrayImp using a pre-specified
// descriptor array and the above specified types catalog.
//..
       bdem_ChoiceArrayImp result(CHOICE_TYPES,
                                  NUM_CHOICES,
                                  MY_DESCRIPTORS,
                                  bdem_AggregateOption::BDEM_PASS_THROUGH);
//..
// Now insert our sample data into the array imp:
//     [ 12  "No Data"  23  52  "Dived by zero"]
//..
       const int VAL0 = 12;
       const int VAL1 = 23;
       const int VAL2 = 52;
       bsl::string noData   = "No data.";
       bsl::string divError = "Divided by zero.";
//..
// The normal usage of 'bdem_ChoiceArray' is create 5 elements, and then
// assign the corresponding items to the desired value.
//..
       result.insertNullItems(0, 5);
       result.makeSelection(0, RESULT_VAL).theModifiableInt() = VAL0;
       result.makeSelection(1, ERROR).theModifiableString()   = noData;
       result.makeSelection(2, RESULT_VAL).theModifiableInt() = VAL1;
       result.makeSelection(3, RESULT_VAL).theModifiableInt() = VAL2;
       result.makeSelection(4, ERROR).theModifiableString()   = divError;
//..
// We can now return the choice array imp:
//..
       return result;
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

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    bslma_TestAllocator defaultAlloc;
    bslma_DefaultAllocatorGuard allocGuard(&defaultAlloc);

    // Declare an array of test specifications used for many of the tests.
    const struct TestRow {
        int         d_line;
        const char *d_catalogSpec;  // specification to create the catalog
    } DATA[] = {
        // Line     DescriptorSpec
        // ====     ==============
        { L_,       "" },
        { L_,       "A" },
        { L_,       "B" },
        { L_,       "C" },
        { L_,       "D" },
        { L_,       "E" },
        { L_,       "F" },
        { L_,       "G" },
        { L_,       "H" },
        { L_,       "I" },
        { L_,       "J" },
        { L_,       "K" },
        { L_,       "L" },
        { L_,       "M" },
        { L_,       "N" },
        { L_,       "O" },
        { L_,       "P" },
        { L_,       "Q" },
        { L_,       "R" },
        { L_,       "S" },
        { L_,       "T" },
        //{ L_,       "U" },
        //{ L_,       "V" },
        { L_,       "W" },
        { L_,       "X" },
        { L_,       "Y" },
        { L_,       "Z" },
        { L_,       "a" },
        { L_,       "b" },
        { L_,       "c" },
        { L_,       "d" },
        //{ L_,       "e" },
        //{ L_,       "f" },

        // Testing various interesting combinations
        { L_,       "AA" },
        { L_,       "AC" },
        { L_,       "CD" },
        { L_,       "CE" },
        { L_,       "FG" },
        { L_,       "GG" },
        { L_,       "GH" },
        { L_,       "MN" },
        { L_,       "OP" },
        { L_,       "PQ" },
        { L_,       "KQ" },
        { L_,       "ST" },
        { L_,       "WX" },
        { L_,       "YZ" },
        { L_,       "ab" },
        { L_,       "cd" },

        { L_,       "HIJ" },
        { L_,       "KLM" },
        { L_,       "RST" },
        { L_,       "YZa" },

        { L_,       "BLRW" },
        { L_,       "DGNQ" },
        { L_,       "QRST" },

        { L_,       "JHKHSK" },
        { L_,       "RISXLSW" },
        { L_,       "MXKZOLPR" },
        { L_,       "GGGGGGGGG" },
        { L_,       "QQQQQQQQQQ" },
        { L_,       "abcdABCDEFG" },
        { L_,       "FDLALAabADSF" },

        { L_,       "GSRWSASDEFDSA" },
        { L_,       "PKJHALKGabASDA" },
        { L_,       "XZSAEWRPOIJLKaAS" },
        { L_,       "GALKacKASJDKSWEIO" },

        { L_,       "ABCDEFGHIJKLMNOPQRSTWXYZabcd" },
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    switch (test) { case 0:  // Zero is always the leading case.
      case 17: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating how one might use a choice imp
        //
        // Concerns:
        //
        // Plan:
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force implementation technique
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "USAGE EXAMPLE" << bsl::endl
                               << "=============" << bsl::endl;

        bdem_ChoiceArrayImp mX = computeResults();
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING BSLMA ALLOCATOR MODEL AND ALLOCATOR TRAITS
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
        if (verbose) bsl::cout << "\nTesting allocator traits"
                               << "\n========================" << bsl::endl;

        typedef bdem_ChoiceArrayImp Obj;

        ASSERT((0 == bslmf_IsConvertible<bslma_Allocator*, Obj>::VALUE));
        ASSERT((1 ==
             bslalg_HasTrait<Obj, bslalg_TypeTraitUsesBslmaAllocator>::VALUE));

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'd_choiceArrayAttr' STRUCT
        //
        // Concerns:
        //   The 'd_choiceArrayAttr' struct is initialized such that:
        //   1. 'd_elemEnum == 31'
        //   2. 'd_size == sizeof(bdem_ChoiceArrayImp)'
        //   3. 'd_alignment == bsls_AlignmentFromType<bdem_ChoiceArrayImp>::VALUE'
        //   4. Function 'unsetConstruct' calls the default constructor.
        //   5. Function 'copyConstruct' calls the copy constructor.
        //   6. Function 'destroy' calls the destructor.
        //   7. Function 'assign' calls the assignment operator.
        //   8. Function 'move' performs a bit-wise move.
        //   9. Function 'makeUnset' calls 'reset'.
        //   10. Function 'isUnset' returns true if object has a void
        //       selection.
        //   11. Function 'areEqual' returns the result of operator==.
        //   12. Function 'print' calls the 'bdem_ChoiceArrayImp::print'.
        //
        // Plan:
        //   Test the data variables for the stated condition.  Use ad-hoc
        //   data to test each of the function pointers once or twice to
        //   convince ourselves that the correct pass-through operation is
        //   occurring.
        //
        // Testing:
        //   static const bdem_Descriptor d_choiceArrayAttr;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting d_choiceArrayAttr Struct"
                               << "\n================================"
                               << bsl::endl;
        const Desc &d = CAI::d_choiceArrayAttr;
        {
            // TEST 1. d_elemENUM
            if (veryVerbose) cout << "Test d_elemEnum" << endl;
            ASSERT(d.d_elemEnum == 31);
            ASSERT(d.d_elemEnum == EType::BDEM_CHOICE_ARRAY);
        }
        {
            // TEST 2. d_size
            if (veryVerbose) cout << "Test d_size" << endl;
            ASSERT(d.d_size == sizeof(bdem_ChoiceArrayImp));
        }
        {
            // TEST 3. d_alignment
            if (veryVerbose) cout << "Test d_alignment" << endl;
            ASSERT(d.d_alignment ==
                   bsls_AlignmentFromType<bdem_ChoiceArrayImp>::VALUE);
        }
        {
            // TEST 4. unsetConstruct
            bslma_TestAllocator tAlloc(veryVeryVerbose);
            if (veryVerbose) cout << "Test unset construction" << endl;
            CAI mX; const CAI& X = mX;
            void *dataPtr = tAlloc.allocate(d.d_size);
            d.unsetConstruct(dataPtr, PASSTH, &tAlloc);
            CAI &mY = *((CAI *)dataPtr); const CAI &Y = mY;
            ASSERT(X == Y);
            // explicitly call the destructor to free the memory
            mY.~CAI();
            tAlloc.deallocate(dataPtr);
        }

        {
            bslma_TestAllocator tAlloc(veryVeryVerbose);
            const bslma_DefaultAllocatorGuard dag1(&tAlloc);
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_catalogSpec;
                const int   LEN  = bsl::strlen(SPEC);

                Catalog cat;
                populateCatalog(&cat, SPEC);

                if (veryVerbose) {
                    cout << "Testing descriptor methods with spec '"
                         << SPEC << "'" << endl;
                }

                EType::Type *catSt = (cat.size() > 0)
                                   ? &cat.front()
                                   : NULL;
                // create and populate mX
                CAI mX(catSt, cat.size(), DESCRIPTORS, PASSTH, &tAlloc);
                const CAI& X = mX;
                populateData(&mX, VALUES_A);

                {
                    // TEST 5. copyConstruct
                    bslma_TestAllocator copyAlloc(veryVeryVerbose);
                    if (veryVerbose) {
                        cout << "Testing copy construction" << endl;
                    }

                    // construct a object Y using descriptor.copyConstruct
                    void *dataPtr = copyAlloc.allocate(d.d_size);
                    d.copyConstruct(dataPtr, &X, PASSTH, &copyAlloc);
                    CAI &mY = *((CAI *)dataPtr); const CAI &Y = mY;

                    ASSERT(X == Y);

                    const int numItems = X.length();
                    for (int j = 0; j < numItems; ++j) {
                        ASSERT(X.theItem(j).isSelectionNull()
                                            == Y.theItem(j).isSelectionNull());
                    }

                    // explicitly call the destructor to free the memory
                    mY.~CAI();
                    copyAlloc.deallocate(dataPtr);
                }
                {

                    // TEST 6. destroy
                    bslma_TestAllocator dstyAlloc(veryVeryVerbose);
                    if (veryVerbose) cout << "Testing desctructors" << endl;

                    // construct a object Y from X
                    // use placement new forcing us to explicitly destroy
                    // the memory
                    void *dataPtr = dstyAlloc.allocate(d.d_size);
                    CAI *temp = new (dataPtr) CAI(X, PASSTH, &dstyAlloc);
                    CAI &mY = *((CAI *)dataPtr); const CAI &Y = mY;

                    ASSERT(X == Y);

                    // explicitly call the destructor to free the memory
                    // will cause a problem in the test allocator if it
                    // doesn't work
                    d.destroy(&mY);
                    dstyAlloc.deallocate(dataPtr);
                }
                {
                    // TEST 7. assign
                    if (veryVerbose) cout << "Testing Assignments" << endl;
                    CAI mCntrl(X); const CAI &CNTRL = mCntrl;
                    CAI mY; const CAI &Y = mY;

                    // ensure the arrays are different for the base case
                    if (LEN == 0)  mCntrl.insertNullItems(0, 1);

                    ASSERT(CNTRL != Y);
                    d.assign(&mY, &CNTRL);
                    LOOP2_ASSERT(CNTRL, Y, CNTRL == Y);
                    const int numItems = X.length();
                    for (int j = 0; j < numItems; ++j) {
                        ASSERT(X.theItem(j).isSelectionNull()
                                            == Y.theItem(j).isSelectionNull());
                    }
                }
                {
                    // TEST 8. move (bitwise copy)
                    bslma_TestAllocator moveAlloc(veryVeryVerbose);
                    if (veryVerbose) {
                        cout << "Testing move" << endl;
                    }
                    CAI mCntrl(X); const CAI& CNTRL = mCntrl;
                    // ensure the arrays are different for the base case
                    if (LEN == 0)  mCntrl.insertNullItems(0, 1);

                    CAI mZ(CNTRL); const CAI& Z = mZ;
                    // use placement new - because we are testing
                    // a bitwise copy function, we don't want to call
                    // a destructor twice on the same object
                    void *dataPtr = moveAlloc.allocate(d.d_size);

                    memset(dataPtr, 0, d.d_size);

                    ASSERT(CNTRL == Z);
                    d.move(dataPtr, &mZ);

                    CAI &mY = *((CAI *)dataPtr); const CAI& Y = mY;
                    ASSERT(CNTRL == Y);
                    ASSERT(Z == Y);
                    const int numItems = Y.length();
                    for (int j = 0; j < numItems; ++j) {
                        ASSERT(Y.theItem(j).isSelectionNull()
                                            == Z.theItem(j).isSelectionNull());
                    }

                    // Don't call the destructor for Y.  It will already be
                    // called on the bitwise identical object Z.
                    moveAlloc.deallocate(dataPtr);
                }
                {
                    // TEST 9. makeUnset
                    if (veryVerbose) cout << "Testing makeUnset" << endl;
                    CAI mY(X);    const CAI& Y = mY;
                    if (LEN != 0) {
                        ASSERT(0 < Y.numSelections());
                        ASSERT(0 < Y.length());
                    }

                    d.makeUnset(&mY);

                    ASSERT(0 == Y.numSelections());
                    ASSERT(0 == Y.length());
                }
                {
                    // TEST 10. isUnset
                    if (veryVerbose) cout << "Testing isUnset" << endl;
                    CAI mY(X);  const CAI& Y = mY;
                    bool isUnset = (LEN == 0);

                    ASSERT(isUnset == d.isUnset(&mY));
                }
                {
                    // TEST 10. areEqual
                    if (veryVerbose) cout << "Testing areEqual" << endl;
                    CAI mZ(X); const CAI& Z = mZ;
                    CAI mY(X); const CAI& Y = mY;
                    mZ.insertNullItems(0, 1);
                    ASSERT(d.areEqual(&X, &Y));
                    const int numItems = X.length();
                    for (int j = 0; j < numItems; ++j) {
                        ASSERT(X.theItem(j).isSelectionNull()
                                            == Y.theItem(j).isSelectionNull());
                    }
                    ASSERT(!d.areEqual(&X, &Z));
                }
                {
                    CAI mY(X);    const CAI &Y = mY;
                    bsl::ostringstream streamX;
                    bsl::ostringstream streamY;

                    X.print(streamX,1,4);
                    d.print(&mY,streamY,1,4);

                    ASSERT(streamX.str() == streamY.str());

                }
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'removeItem' METHODS:
        //
        // Concerns:
        //      That removeItem removes items at the correct index.  That it
        //      removes the correct number of items.  That there are no other
        //      side effects to the operation
        //
        // Plan:
        //   1.Specify a set S of (unique) objects with substantial and varied
        //     differences in value.  For each element in the set create
        //     a test array A and a control X.  Iterate over the indices of the
        //     array A and and iterate over the number of possible elements
        //     to remove at that index.  Call
        //     removeItems(index,numItemsToRemove) on A and then validate the
        //     resulting array A against the control X.
        //   2.Specify a set S of (unique) objects with substantial and varied
        //     differences in value.  For each element in the set create an
        //     array A and a control X.  Iterate over the indices of the array
        //     A.  Call removeItem(index) on A and validate the resulting array
        //     agains the control X.
        //
        // Testing:
        //   void removeItem(int);
        //   void removeItems(int, int)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting removeItem METHODS"
                               << "\n==========================" << bsl::endl;
        {
            if (verbose) {
                cout << "\tTest removeItem(int, int) for "
                        " ChoiceArrays.  Based on data from a table test specs"
                     << endl;
            }

            // don't use aggregate types in the test spec
            const char *testSpec = "ABCDEFGHIJKLMNOPQRSTWXYZabcd";
            const int   specLen  = strlen(testSpec);

            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Catalog cat;
            populateCatalog(&cat, testSpec);

            // iterate over the sample test data
            for (int i = 0; i < NUM_DATA; ++i) {
                const char *SPEC = DATA[i].d_catalogSpec;
                const int   LEN  = bsl::strlen(SPEC);

                if (veryVerbose) {
                    cout << "Testing removeItems(int, int) for "
                            "array based on spec '" << SPEC << "'" << endl;
                }

                // iterate over the index for removal
                for (int index = 0; index < LEN; ++index) {
                    // iterate over how many elements to remove
                    for (int num = 0; num < LEN - index; ++num) {

                        CAI mA(&cat.front(), cat.size(), DESCRIPTORS,
                               PASSTH, &testAllocator);
                        const CAI& A = mA;

                        // pre-populate our test array X
                        mA.insertNullItems(0, LEN);
                        for (int j = 0; j < LEN; ++j) {
                            const EType::Type type = getElemType(testSpec[j]);
                            const void *valueA     = getValueA(testSpec[j]);
                            mA.theModifiableItem(j).makeSelection(j, valueA);
                        }

                        CAI mB(mA, PASSTH, &testAllocator);
                        const CAI& B = mB;

                        // create a control copy Y
                        CAI mX(mA, PASSTH, &testAllocator);
                        const CAI& X = mX;

                        // remove the elements
                        mA.removeItems(index, num);

                        // validate the elements have been removed
                        LOOP2_ASSERT(i, index, A.length() == LEN - num);

                        // verify we haven't modified any of the previous
                        // elements
                        for (int j = 0; j < index - 1; ++j) {
                            LOOP3_ASSERT(i, index, j,
                                         A.theItem(j) == X.theItem(j));
                            LOOP3_ASSERT(i, index, j,
                                              A.theItem(j).isSelectionNull() ==
                                               X.theItem(j).isSelectionNull());
                        }

                        // verify the subsequent elements
                        for (int j = index; j < LEN - num; ++j) {
                            LOOP3_ASSERT(i, index, j,
                                         A.theItem(j) == X.theItem(j + num));
                            LOOP3_ASSERT(i, index, j,
                                         A.theItem(j).isSelectionNull()
                                      == X.theItem(j + num).isSelectionNull());
                        }
                    }
                }
            }
        }

        {
            if (verbose) {
                cout << "\tTest removeItem(int) for "
                        " ChoiceArrays.  Based on data from a table test specs"
                     << endl;
            }
            // don't use aggregate types in the test spec
            const char *testSpec = "ABCDEFGHIJKLMNOPQRSTWXYZabcd";
            const int   specLen  = strlen(testSpec);

            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Catalog cat;
            populateCatalog(&cat,testSpec);

            // iterate over the sample test data
            for (int i = 0; i < NUM_DATA; ++i) {
                const char *SPEC = DATA[i].d_catalogSpec;
                const int   LEN  = bsl::strlen(SPEC);

                if (veryVerbose) {
                    cout << "Testing removeItem(int) for "
                            "array based on spec '" << SPEC << "'" << endl;
                }

                // iterate over the index for removal
                for (int index = 0; index < LEN; ++index) {
                    if (LEN == 0) {
                        continue;
                    }

                    CAI mA(&cat.front(), cat.size(), DESCRIPTORS,
                           PASSTH, &testAllocator);
                    const CAI& A = mA;

                    // pre-populate our test array X
                    mA.insertNullItems(0, LEN);
                    for (int j = 0; j < LEN; ++j) {
                        const EType::Type type = getElemType(testSpec[j]);
                        const void *valueA     = getValueA(testSpec[j]);
                        mA.theModifiableItem(j).makeSelection(j, valueA);
                    }

                    CAI mB(mA,PASSTH,&testAllocator);
                    const CAI &B = mB;

                    // create a control copy Y
                    CAI mX(mA,PASSTH,&testAllocator);
                    const CAI& X = mX;

                    // remove the element
                    mA.removeItem(index);

                    // validate the elements have been removed
                    LOOP2_ASSERT(i, index, A.length() == LEN - 1);

                    // verify we haven't modified any of the previous elements
                    for (int j = 0; j < index - 1; ++j) {
                        LOOP3_ASSERT(i, index, j,
                                     A.theItem(j) == X.theItem(j));
                        LOOP3_ASSERT(i, index, j,
                                     A.theItem(j).isSelectionNull()
                                            == X.theItem(j).isSelectionNull());
                    }

                    // verify the subsequent elements
                    for (int j = index; j < LEN - 1; ++j) {
                        LOOP3_ASSERT(i, index, j,
                                     A.theItem(j) == X.theItem(j + 1));
                        LOOP3_ASSERT(i, index, j,
                                     A.theItem(j).isSelectionNull()
                                        == X.theItem(j + 1).isSelectionNull());
                    }
                }
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'insertItem' METHODS:
        //
        // Concerns:
        //   That insertItem properly inserts a new element in the array
        //   at the correct index.  That it sets the new elements value
        //   correctly.  That there are no other side effects to the operation.
        //   That insertItem() works with aliasing.
        //
        // Plan:
        //   1.Specify a set S of (unique) objects with substantial and varied
        //     differences in value.  For each element in the set iterate
        //     through all the indices.  For each index: insert a value,
        //     verify the correct value was inserted at the correct index.
        //   2.Specify a set S of (unique) objects with substantial and varied
        //     differences in value.  For each element in the set iterate
        //     through all the indices.  For each index test for aliasing:
        //     insert a value with a reference to the beginning of the array,
        //     insert a value with a reference to the end of the array,
        //     insert a value with a reference to the middle of the array.
        //     in each case verify the correct value was inserted at the
        //     correct index.
        //
        // Testing:
        //   void insertItem(int, const bdem_ChoiceHeader&);
        // --------------------------------------------------------------------

          static const struct TestRow {
              int         d_line;
              const char *d_catalogSpec;  // Specification to create catalog
          } DATA[] = {
              // Line     DescriptorSpec
              // ====     ==============
              { L_,       "" },
              { L_,       "A" },
              { L_,       "D" },
              { L_,       "AA" },
              { L_,       "GALKacK" },
          };
          const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) bsl::cout << "\nTesting insertItem METHOD"
                               << "\n=========================" << bsl::endl;
        {
            if (verbose) {
                cout << "\tTest insert(i,bdem_ChoiceHeader) for choice arrays"
                        " based on data from a table test specs"
                     << endl;
            }
            // don't use aggregate types in the test spec
            const char *testSpec = "ABCDEFGHIJKLMNOPQRSTWXYZabcd";
            const int   specLen  = strlen(testSpec);

            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Catalog cat;
            populateCatalog(&cat, testSpec);

            // we need this dummy to help create choice headers without
            // cumbersome code to create a DescriptorCatalog for the header
            CAI dummy(&cat.front(), cat.size(), DESCRIPTORS,
                      PASSTH, &testAllocator);
            const CAI& DUMMY = dummy;
            dummy.insertNullItems(0, 1);

            // iterate over element over our selection of test data
            for (int i = 0; i < NUM_DATA; ++i) {
                const char *SPEC = DATA[i].d_catalogSpec;
                const int   LEN  = bsl::strlen(SPEC);

                if (veryVerbose) {
                    cout << "Testing insertItem(i, Obj) for "
                         << "array based on spec '" << SPEC << "'" << endl;
                }
                BEGIN_BSLMA_EXCEPTION_TEST {
                    // iterate over the index for insertion
                    // notice index=LEN+1 appends to the array
                    for (int index = 0; index < LEN + 1; ++index) {
                        CAI mX(&cat.front(), cat.size(), DESCRIPTORS,
                               PASSTH, &testAllocator);
                        const CAI& X = mX;

                        // pre-populate our test array X
                        mX.insertNullItems(0, LEN);
                        for (int j = 0; j < LEN; ++j) {
                            const EType::Type  type = getElemType(testSpec[j]);
                            const void        *valueA = getValueA(testSpec[j]);
                            mX.theModifiableItem(j).makeSelection(j, valueA);
                        }

                        // create a control copy Y
                        CAI mY(mX, PASSTH, &testAllocator);
                        const CAI& Y = mY;

                        // insert an element at the current index, use a
                        // different type
                        const int selector = (index + 5) % specLen;

                        const EType::Type type =
                                               getElemType(testSpec[selector]);
                        const void     *valueB = VALUES_B[(int)type];

                        // use our dummy to initialize the choice header
                        bdem_ChoiceHeader hdr(DUMMY.theItem(0));
                        const bdem_ChoiceHeader& HDR = hdr;
                        hdr.makeSelection(selector, valueB);

                        mX.insertItem(index, HDR);

                        // VERIFY THE INSERTED ITEM

                        // validate the index has been added
                        LOOP2_ASSERT(i, index, X.length() == LEN + 1);
                        LOOP2_ASSERT(i, index, X != Y);

                        // verify we haven't modified any of the
                        // previous elements
                        for (int j = 0; j < index; ++j) {
                            LOOP3_ASSERT(i, index, j,
                                         X.theItem(j) == Y.theItem(j));
                            LOOP3_ASSERT(i, index, j,
                                         X.theItem(j).isSelectionNull()
                                            == Y.theItem(j).isSelectionNull());
                        }

                        // verify our new element
                        LOOP2_ASSERT(i, index, X.theItem(index) == HDR);
                        LOOP2_ASSERT(i, index,
                                     X.theItem(index).isSelectionNull()
                                                     == HDR.isSelectionNull());

                        // verify the subsequent elements
                        for (int j = index + 1; j < LEN + 1; ++j) {
                            LOOP3_ASSERT(i, index, j,
                                         X.theItem(j) == Y.theItem(j - 1));
                            LOOP3_ASSERT(i, index, j,
                                         X.theItem(j).isSelectionNull()
                                        == Y.theItem(j - 1).isSelectionNull());
                        }
                    }
                } END_BSLMA_EXCEPTION_TEST
            }
        }
        {
            if (verbose) {
                cout << "\tTest insert(i, bdem_ChoiceHeader) for aliasing "
                     << "issues" << endl;
            }
            // don't use aggregate types in the test spec
            const char *testSpec = "ABCDEFGHIJKLMNOPQRSTWXYZabcd";
            const int   specLen  = strlen(testSpec);

            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Catalog cat;
            populateCatalog(&cat, testSpec);

            // iterate over element over our selection of test data
            for (int i = 0; i < NUM_DATA; ++i) {
                const char *SPEC = DATA[i].d_catalogSpec;
                const int   LEN  = bsl::strlen(SPEC);

                if (veryVerbose) {
                    cout << "Testing insertItem(i, Obj) for "
                         << "array based on spec '" << SPEC << "'" << endl;
                }
                BEGIN_BSLMA_EXCEPTION_TEST {
                    // iterate over the index for insertion
                    // notice index=LEN+1 appends to the array
                    for (int index = 0; index <= LEN; ++index) {
                        CAI mA(&cat.front(), cat.size(), DESCRIPTORS,
                               PASSTH, &testAllocator);
                        const CAI& A = mA;

                        // pre-populate our test array X
                        mA.insertNullItems(0, LEN + 1);
                        LOOP_ASSERT(index, LEN + 1 == A.length());
                        for (int j = 0; j < LEN; ++j) {
                            const EType::Type type = getElemType(testSpec[j]);
                            const void *valueA     = getValueA(testSpec[j]);
                            mA.theModifiableItem(j).makeSelection(j, valueA);
                        }

                        CAI mB(mA, PASSTH, &testAllocator); const CAI& B = mB;
                        CAI mC(mA, PASSTH, &testAllocator); const CAI& C = mC;

                        // create a control copy Y
                        CAI mX(mA, PASSTH, &testAllocator); const CAI& X = mX;
                        mA.insertItem(index, A.theItem(0));
                        mB.insertItem(index, B.theItem(B.length() - 1));
                        mC.insertItem(index, C.theItem((C.length()- 1) / 2));

                        // VERIFY THE INSERTED ITEM
                        // validate the index has been added
                        LOOP2_ASSERT(i, index, X.length() + 1 == A.length());
                        LOOP2_ASSERT(i, index, X.length() + 1 == B.length());
                        LOOP2_ASSERT(i, index, X.length() + 1 == C.length());
                        LOOP2_ASSERT(i, index, X != A);
                        LOOP2_ASSERT(i, index, X != B);
                        LOOP2_ASSERT(i, index, X != C);

                        // verify we haven't modified any of the
                        // previous elements
                        for (int j = 0; j < index; ++j) {
                            LOOP5_ASSERT(i, index, j, X.theItem(j),
                                         A.theItem(j),
                                         X.theItem(j) == A.theItem(j));
                            LOOP3_ASSERT(i, index, j,
                                         X.theItem(j) == B.theItem(j));
                            LOOP3_ASSERT(i, index, j,
                                         X.theItem(j) == C.theItem(j));
                        }

                        // verify our new element
                        LOOP4_ASSERT(i, index, X.theItem(0), A.theItem(index),
                                     X.theItem(0) == A.theItem(index));
                        LOOP2_ASSERT(i, index,
                                X.theItem(X.length() - 1) == B.theItem(index));
                        LOOP2_ASSERT(i, index,
                          X.theItem((X.length() - 1) / 2) == C.theItem(index));

                        // verify the subsequent elements
                        for (int j = index + 1; j < LEN + 1; ++j) {
                            LOOP3_ASSERT(i, index, j,
                                         X.theItem(j - 1) == A.theItem(j));
                            LOOP3_ASSERT(i, index, j,
                                         X.theItem(j - 1) == B.theItem(j));
                            LOOP3_ASSERT(i, index, j,
                                         X.theItem(j - 1) == C.theItem(j));
                        }
                    }
                } END_BSLMA_EXCEPTION_TEST
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'reset' FUNCTION:
        //
        // Concerns:
        //     Verify that when reset the object has a state identical
        //     to its state after construction
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all objects, mX1
        //   and mX2 using the set S.  Default-construct an object, mY,
        //   and construct an object mZ with a catalog taken from set S.
        //   Use the two flavors of reset on mX1 and mX2 and verify
        //   that they are equal to mY and mZ.
        // Testing:
        //   void clear();
        //   void reset();
        //   void reset(const bdem_ElemType::Type [], int,
        //              const bdem_Descriptor *const);
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTesting reset Functions"
                         "\n=======================" << bsl::endl;
        }
        const Desc **D = DESCRIPTORS;
        {

            bslma_TestAllocator testAllocator(veryVeryVerbose);
            bslma_TestAllocator &tAlloc = testAllocator;
            // iterate U over catalogs in S
            for (int i = 0; i < NUM_DATA; ++i) {
                const char *SPEC  = DATA[i].d_catalogSpec;

                BEGIN_BSLMA_EXCEPTION_TEST {
                    if (veryVerbose) { P(SPEC); }

                    Catalog cat;
                    populateCatalog(&cat,SPEC);
                    EType::Type *catSt = (cat.size()>0) ? &cat.front()
                        : NULL;

                    // create and populate mX1 and mX2
                    CAI mX1(catSt,cat.size(),D,PASSTH,&tAlloc);
                    const CAI &X1 = mX1;
                    populateData(&mX1,VALUES_A);
                    CAI mX2(mX1,PASSTH,&tAlloc); const CAI &X2 = mX2;
                    CAI mX3(mX1,PASSTH,&tAlloc); const CAI &X3 = mX3;

                    // create the control arrays
                    CAI mY;                                    const CAI &Y=mY;
                    CAI mZ(catSt,cat.size(),D,PASSTH,&tAlloc); const CAI &Z=mZ;

                    // reset
                    mX1.reset();
                    mX2.clear();
                    mX3.reset(catSt,cat.size(),D);

                    // compare
                    LOOP_ASSERT(i, X1==Y);
                    LOOP_ASSERT(i, X2==Y);
                    LOOP_ASSERT(i, X3==Z);
                }END_BSLMA_EXCEPTION_TEST
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'makeSelection' FUNCTIONS:
        //
        // Concerns:
        //   The 'makeSelection' functions have the same concerns as
        //   assignment.  Any value must be assignable to an object having any
        //   initial value without affecting the rhs operand value.  Also, any
        //   object must be settable to itself.
        //
        // Plan:
        //   1. Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct two choice arrays based on
        //   each element of the set S.  Use the makeSelection method to
        //   set the values of one and previously tested methods (operator[])
        //   to set the value of the other.  Verify the values of the final
        //   arrays are equal.
        //   2. Then test aliasing by copy constructing a
        //   control w from each u in S, use 'makeSelection' to assign u to
        //   itself, and verifying that w == u.  For each transformation,
        //   verify that no memory is leaked and that the correct number
        //   constructors and destructors are called.
        //
        // Testing:
        //   bdem_ElemRef makeSelection(int index, int selection);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting makeSelection Functions"
                                  "\n==============================="
                               << bsl::endl;

        // makeSelection for choice arrays on the set of test data
        // specified in the DATA table
        {
            if (veryVerbose) {
                cout << "\tTest basic makeSelection functionality for choice"
                        " arrays.  Compare two"
                        "array constructed using the DATA table of specs"
                     << endl;
            }
            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Catalog cat;
            populateCatalog(&cat,SPECIFICATIONS);

            // we've already tested that the allocation strategy and allocator
            // is passed to the delegated types and is works correctly for
            // insertNullItem and operator[]
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                BEGIN_BSLMA_EXCEPTION_TEST {
                    if (veryVerbose) {
                        cout << "testing makeSelection based on spec '"
                             << SPEC << "'" << endl;
                    }

                    CAI mX(&cat.front(), cat.size(), DESCRIPTORS,
                           PASSTH, &testAllocator);
                    CAI mY(&cat.front(), cat.size(), DESCRIPTORS,
                           PASSTH, &testAllocator);

                    const CAI& X = mX;
                    const CAI& Y = mY;

                    LOOP_ASSERT(i, 0 == X.length());

                    mX.insertNullItems(0, LEN);
                    mY.insertNullItems(0, LEN);
                    for (int j = 0; j < LEN; ++j) {
                        const EType::Type  type      = getElemType(SPEC[j]);
                        const Desc        *DESC      = getDescriptor(SPEC[j]);
                        const void        *valueA    = getValueA(SPEC[j]);
                        const int          selection = (int)type;

                        // X is the control; use tested methods to set the
                        // value.
                        mX.theModifiableItem(j).makeSelection(selection,
                                                              valueA);
                        ERef Ref = mY.makeSelection(j, selection);
                        LOOP2_ASSERT(i, j, Ref.isNull());
                        Ref.replaceValue(bdem_ConstElemRef(valueA,DESC));
                        LOOP2_ASSERT(i, j, !Ref.isNull());
                    }
                    LOOP_ASSERT(i, X == Y);
                } END_BSLMA_EXCEPTION_TEST
            }
        }
        {
            if (veryVerbose) {
                cout << "\tTest for aliasing issues with makeSelection"
                     << endl;
            }
            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Catalog cat;
            populateCatalog(&cat, SPECIFICATIONS);

            // we've already tested that the allocation strategy and allocator
            // is passed to the delegated types and is works correctly for
            // insertNullItem and operator[]
            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_catalogSpec;
                const int LEN  = bsl::strlen(SPEC);

                if (veryVerbose) {
                    cout << "testing makeSelection aliasing based on spec '"
                         << SPEC << "'" << endl;
                }

                CAI mU(&cat.front(), cat.size(), DESCRIPTORS,
                       PASSTH, &testAllocator);
                const CAI& U = mU;
                // construct our sample data using tested methods
                mU.insertNullItems(0, LEN);
                for (int j = 0; j < LEN; ++j) {
                    const EType::Type  type   = getElemType(SPEC[j]);
                    const void        *valueA = getValueA(SPEC[j]);
                    // U is the control, use tested methods to set the value
                    mU.theModifiableItem(j).makeSelection(type, valueA);
                }

                CAI mW(U, PASSTH, &testAllocator);
                const CAI& W = mW;

                for (int j = 0; j < LEN; ++j) {
                    const EType::Type  type      = getElemType(SPEC[j]);
                    const Desc        *DESC      = getDescriptor(SPEC[j]);
                    const void        *valueA    = getValueA(SPEC[j]);
                    const int          selection = (int)type;

                    ERef Ref = mU.makeSelection(j, selection);
                    LOOP2_ASSERT(i, j, Ref.isNull());
                    Ref.replaceValue(bdem_ConstElemRef(valueA,DESC));
                    LOOP2_ASSERT(i, j, !Ref.isNull());
                }
                LOOP_ASSERT(i, W == U);
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //   The 'bdex' streaming concerns for this component are standard.  We
        //   first probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality.  We then thoroughly test streaming functionality
        //   relying on the bdex functions which forward appropriate calls
        //   to the member functions of this component.  We next step through
        //   the sequence of possible stream states (valid, empty, invalid,
        //   incomplete, and corrupted), appropriately selecting data sets as
        //   described below.  In all cases, exception neutrality is confirmed
        //   using the specially instrumented 'bdex_TestInStream' and a pair of
        //   standard macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
        //   'END_BDEX_EXCEPTION_TEST', which configure the
        //   'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   1.PRELIMINARY MEMBER FUNCTION TEST
        //     First test 'maxSupportedBdexVersion' explicitly, and then
        //     perform a trivial direct test of the 'outStream' and 'inStream'
        //     methods (the rest of the testing will use the stream operators).
        //
        //   2.VALID STREAMS
        //     For the set S of globally-defined test values, use all
        //     combinations (u, v) in the cross product S X S, stream the
        //     value of v into (a temporary copy of) u and assert u == v.
        //
        //   3. EMPTY AND INVALID STREAMS
        //     For each u in S, create a copy and attempt to stream into it
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   4. INCOMPLETE DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct an input stream and attempt to read into objects
        //     initialized with distinct values.  Verify values of objects
        //     that are either successfully modified or left entirely
        //     unmodified, and that the stream became invalid immediately after
        //     the first incomplete read.  Finally ensure that each object
        //     streamed into is in some valid state by assigning it a distinct
        //     new value and testing for equality.
        //
        // Testing:
        //   int maxSupportedBdexVersion() const;
        //   bdex_InStream& streamIn(bdex_InStream& stream);
        //   bdex_InStream& streamIn(bdex_InStream& stream, int version);
        //   bdex_OutStream& streamOut(bdex_OutStream& stream) const;
        //   bdex_OutStream& streamOut(bdex_OutStream& stream, int) const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Streaming Functionality"
                               << "\n==============================="
                               << endl;

        const Desc               **D = DESCRIPTORS;
        streamInAttrLookup<In>     inLookup;
        streamOutAttrLookup<Out>   outLookup;

        bslma_TestAllocator tAlloc(veryVeryVerbose);
        const bslma_DefaultAllocatorGuard dag1(&tAlloc);

        for (int v = 1; v < 4; ++v) {

            const int VERSION = v;

            // iterate U over catalogs in S
            for (int uI = 0; uI < NUM_DATA; ++uI) {
                const char *SPECU  = DATA[uI].d_catalogSpec;

                Catalog catU;
                populateCatalog(&catU, SPECU);
                EType::Type *catUSt = (catU.size() > 0)
                    ? &catU.front()
                    : NULL;

                CAI mU(catUSt, catU.size(), D, PASSTH, &tAlloc);
                const CAI &U = mU;
                populateData(&mU, VALUES_A);

                Out os;
                U.bdexStreamOut(os, VERSION, outLookup.lookupTable());

                if (veryVerbose) { P(SPECU); P(VERSION); P(U); }

                // TEST 2.VALID STREAMS (case 1)
                // Stream a constructed obj to an empty obj
                {
                    CAI mA(&tAlloc);    const CAI& A = mA;

                    if (catU.size() > 0)
                    {
                        LOOP_ASSERT(uI, U != A);
                    }

                    ASSERT(os);

                    In testInStream(os.data(), os.length());
                    BEGIN_BDEX_EXCEPTION_TEST {
                        testInStream.reload(os.data(),os.length());
                        testInStream.setSuppressVersionCheck(1);
                        ASSERT(testInStream);
                        mA.bdexStreamIn(testInStream, VERSION,
                                        inLookup.lookupTable(),DESCRIPTORS);
                    } END_BDEX_EXCEPTION_TEST
                          LOOP3_ASSERT(uI, U, A, U == A);
                }

                // TEST 3. EMPTY AND INVALID STREAMS
                // Stream from an empty and invalid stream
                {
                    CAI mA(U, &tAlloc); const CAI& A = mA;

                    {
                        In testInStream;  // Empty stream

                        testInStream.setSuppressVersionCheck(1);

                        BEGIN_BDEX_EXCEPTION_TEST {
                            testInStream.reload(NULL,0);
                            mA.bdexStreamIn(testInStream, VERSION,
                                            inLookup.lookupTable(),
                                            DESCRIPTORS);

                            ASSERT(!testInStream);
                        } END_BDEX_EXCEPTION_TEST
                              ASSERT(A == U);
                    }
                    {
                        In testInStream(os.data(), os.length());

                        BEGIN_BDEX_EXCEPTION_TEST {
                            testInStream.setSuppressVersionCheck(1);
                            testInStream.reload(os.data(),os.length());
                            testInStream.invalidate();  // Invalid stream
                            mA.bdexStreamIn(testInStream, VERSION,
                                            inLookup.lookupTable(),
                                            DESCRIPTORS);
                        } END_BDEX_EXCEPTION_TEST
                              LOOP2_ASSERT(A, U, A == U);
                        ASSERT(!testInStream);
                    }
                }

                for (int vI = 0; vI < NUM_DATA; ++vI) {
                    const char *SPECV  = DATA[vI].d_catalogSpec;

                    Catalog catV;
                    populateCatalog(&catV,SPECV);
                    EType::Type *catVSt = (catV.size()>0) ? &catV.front()
                                                          : NULL;

                    CAI mV(catVSt,catV.size(),D,PASSTH,&tAlloc);
                    const CAI &V = mV;

                    populateData(&mV,VALUES_A);

                    // TEST 2.VALID STREAMS (case 2)
                    // Stream a constructed U into a temp copy of V
                    {
                        CAI tmpV(&tAlloc); const CAI& TMPV = tmpV;

                        In testInStream(os.data(), os.length());

                        testInStream.setSuppressVersionCheck(1);
                        BEGIN_BDEX_EXCEPTION_TEST {
                            testInStream.reload(os.data(),os.length());
                            tmpV.bdexStreamIn(testInStream, VERSION,
                                              inLookup.lookupTable(),
                                              DESCRIPTORS);
                            LOOP2_ASSERT(uI,vI, U == TMPV);
                        } END_BDEX_EXCEPTION_TEST
                              }
                }
            }

            // TEST 4. INCOMPLETE DATA
            // Streaming in from an incomplete stream
            {
                bslma_TestAllocator tAlloc(veryVeryVerbose);
                const bslma_DefaultAllocatorGuard dag1(&tAlloc);

                Catalog cat;
                populateCatalog(&cat, "A");
                                    // DATA.theItem(NUM_DATA-1).d_catalogSpec);
                CAI mA(&cat.front(),cat.size(),D,PASSTH,&tAlloc);
                const CAI& A = mA;
                CAI mB(&cat.front(),cat.size(),D,PASSTH,&tAlloc);
                const CAI& B = mB;
                CAI mC(&cat.front(),cat.size(),D,PASSTH,&tAlloc);
                const CAI& C = mC;

                populateData(&mA,VALUES_A);
                populateData(&mB,VALUES_B);
                populateData(&mC,VALUES_N);
                ASSERT(A != B);
                ASSERT(A != C);
                ASSERT(B != C);

                Out os;
                A.bdexStreamOut(os, VERSION, outLookup.lookupTable());

                const int LOD1 = os.length();
                B.bdexStreamOut(os, VERSION, outLookup.lookupTable());
                const int LOD2 = os.length();
                C.bdexStreamOut(os, VERSION, outLookup.lookupTable());

                const int LOD = os.length();
                const char* const OD = os.data();

                for (int bytes = 0; bytes < LOD; ++bytes) {
                    // create an in stream of the first i bytes of the output
                    In testInStream(OD,bytes);
                    In &in = testInStream;

                    CAI mX;    const CAI& X = mX;
                    CAI mY(A); const CAI& Y = mY;
                    mY.removeItems(0,A.length());

                    BEGIN_BDEX_EXCEPTION_TEST {
                        testInStream.reload(OD,bytes);

                        LOOP_ASSERT(bytes,testInStream);
                        LOOP_ASSERT(bytes,!bytes == testInStream.isEmpty());

                        CAI t1(X), t2(X), t3(X);

                        if (bytes < LOD1) {
                            READ( t1, in);  LOOP_ASSERT(bytes, !in);
                            if (0 == bytes) { LOOP_ASSERT(bytes, X == t1);}
                            READ( t2, in);  LOOP_ASSERT(bytes, !in);
                            LOOP_ASSERT(bytes, X == t2);
                            READ( t3, in);  LOOP_ASSERT(bytes, !in);
                            LOOP_ASSERT(bytes, X == t3);
                        }
                        else if (bytes < LOD2) {
                            READ( t1, in);  LOOP_ASSERT(bytes,  in);
                            LOOP_ASSERT(bytes, A == t1);
                            READ( t2, in);  LOOP_ASSERT(bytes, !in);
                            if (LOD1 == bytes)  { LOOP_ASSERT(bytes, X == t2);}
                            READ( t3, in);  LOOP_ASSERT(bytes, !in);
                            LOOP_ASSERT(bytes, X == t3);
                        }
                        else {
                            READ( t1, in);  LOOP_ASSERT(bytes,  in);
                            LOOP_ASSERT(bytes, A == t1);
                            READ( t2, in);  LOOP_ASSERT(bytes,  in);
                            LOOP_ASSERT(bytes, B == t2);
                            READ( t3, in);  LOOP_ASSERT(bytes, !in);
                            if (LOD2 == bytes) { LOOP_ASSERT(bytes, X == t3);}
                        }

                        CAI blank(A); const CAI &BLANK = blank;
                        // not tested, but for our purposes its fine
                        blank.reset();

                        LOOP_ASSERT(bytes, C != BLANK);
                        t1 = BLANK;        LOOP_ASSERT(bytes, BLANK == t1);

                        LOOP_ASSERT(bytes, C != BLANK);
                        t2 = BLANK;        LOOP_ASSERT(bytes, BLANK == t2);

                        LOOP_ASSERT(bytes, C != BLANK);
                        t3 = BLANK;        LOOP_ASSERT(bytes, BLANK == t3);

                    } END_BDEX_EXCEPTION_TEST
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.  Assigning a value to an object with the
        //   same value type should use assignment, whereas assigning a value
        //   to an object with a different value type should use destruction
        //   and copy construction.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   bdem_ChoiceArrayImp& operator=(const bdem_ChoiceArrayImp& rhs);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Assignment Operator"
                               << "\n===========================" << bsl::endl;

        if (verbose) {
            cout << "\tTesting basic assignment" << endl;
        }

        for (int uI = 0; uI < NUM_DATA; ++uI) {
            // for each test in the DATA table
            const char *SPECU      = DATA[uI].d_catalogSpec;

            bslma_TestAllocator tAlloc(veryVeryVerbose);
            const bslma_DefaultAllocatorGuard dag1(&tAlloc);
            for (int vI = 0; vI < NUM_DATA; ++vI) {
                const char *SPECV = DATA[vI].d_catalogSpec;
                if (veryVerbose) {
                    bsl::cout << "\tTesting table spec '" << SPECU << "'"
                              << " and '" << SPECV << "'" << bsl::endl;
                }

                Catalog catU;
                populateCatalog(&catU, SPECU);
                EType::Type *catUSt = (catU.size() > 0)
                                    ? &catU.front()
                                    : NULL;
                Catalog catV;
                populateCatalog(&catV, SPECV);
                EType::Type *catVSt = (catV.size() > 0)
                                    ? &catV.front()
                                    : NULL;

                const Desc **D = DESCRIPTORS;

                CAI mV(catVSt, catV.size(), D, PASSTH, &tAlloc);
                const CAI &V = mV;

                CAI mU(catUSt, catU.size(), D, PASSTH, &tAlloc);
                const CAI &U = mU;

                populateData(&mU, VALUES_A);
                populateData(&mV, VALUES_A);

                if (V.length() > 0) {
                    mV.theModifiableItem(0).clearNullnessBit();
                    mV.theModifiableItem((V.length() - 1) / 2)
                                                           .clearNullnessBit();
                    mV.theModifiableItem(V.length() - 1).clearNullnessBit();
                }

                CAI mW(mV); const CAI &W = mW;

                // sanity check
                LOOP2_ASSERT(uI, vI, (vI == uI) == (U == W));
                LOOP2_ASSERT(uI, vI, (vI == uI) == (U == V));

                // PERFORM ASSIGNMENT
                mU = V;

                LOOP2_ASSERT(uI, vI, (W == V) && (W == U));

                // modify V, and ensure no change to U
                if (catV.size() > 0) {
                    const void *VALUE = VALUES_B[catV[0]];
                    mV.theModifiableItem(0).makeSelection(0,VALUE);

                    LOOP2_ASSERT(uI, vI, U == W);
                    LOOP2_ASSERT(uI, vI, U != V);
                }
            }
        }

        if (verbose) {
            cout << "\tTesting aliasing" << endl;
        }

        for (int uI = 0; uI < NUM_DATA; ++uI) {
            // for each test in the DATA table

            const int   LINE      = DATA[uI].d_line;
            const char *SPEC      = DATA[uI].d_catalogSpec;
            const int   LEN       = bsl::strlen(SPEC);

            bslma_TestAllocator tAlloc(veryVeryVerbose);

            if (veryVerbose) {
                bsl::cout << "\tTesting table spec '" << SPEC << "'"
                          << bsl::endl;
            }

            Catalog cat;
            populateCatalog(&cat,SPEC);

            EType::Type *catSt = (cat.size() > 0)
                               ? &cat.front()
                               : NULL;

            CAI mU(catSt, cat.size(),
                   DESCRIPTORS, PASSTH,&tAlloc);
            const CAI &U = mU;
            populateData(&mU,VALUES_A);

            if (U.length() > 0) {
                mU.theModifiableItem(0).clearNullnessBit();
                mU.theModifiableItem((U.length() - 1) / 2).clearNullnessBit();
                mU.theModifiableItem(U.length() - 1).clearNullnessBit();
            }

            CAI mW(U); const CAI &W = mW;

            // PERFORM ASSIGNMENT
            mU = U;

            LOOP_ASSERT(uI, W == U);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.  For values that require dynamic allocation, each
        //   copy must allocate storage independently from its own allocator.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects A and B, with different
        //   allocators, using tested methods.  Then copy construct objects
        //   X, Y, and Z using different constructors from A using a
        //   third allocator, and use the equality operator to
        //   assert that A,B,X,Y and Z have the same value.  Modify X,Y, and Z
        //   by changing one attribute and verify that A and B still have the
        //   same value but that X,Y, and Z are different from both.
        //   Repeat the entire process, but change different attributes of X,Y
        //   and Z   in the last step.
        //
        // Testing:
        //   bdem_ChoiceArrayImp(const bdem_ChoiceArrayImp&  original,
        //                       bslma_Allocator            *basicAllocator);
        //   bdem_ChoiceArrayImp(const bdem_ChoiceArrayImp&  original,
        //                       bdem_AllocationStrategy     allocMode,
        //                       bslma_Allocator            *basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Copy Constructor"
                               << "\n========================" << bsl::endl;

        for (int testI = 0; testI < NUM_DATA; ++testI) {
            // for each test in the DATA table

            const int   LINE = DATA[testI].d_line;
            const char *SPEC = DATA[testI].d_catalogSpec;
            const int   LEN  = bsl::strlen(SPEC);

            if (veryVerbose) {
                bsl::cout << "\tTesting table spec '" << SPEC << "'"
                          << bsl::endl;
            }

            bslma_TestAllocator tAlloc1(veryVeryVerbose);
            bslma_TestAllocator tAlloc2(veryVeryVerbose);
            bslma_TestAllocator tAlloc3(veryVeryVerbose);
            bslma_TestAllocator tAlloc4(veryVeryVerbose);
            bslma_TestAllocator tAlloc5(veryVeryVerbose);

            bdema_SequentialAllocator  sAlloc2(&tAlloc2);
            bdema_SequentialAllocator  sAlloc4(&tAlloc4);
            bdema_SequentialAllocator  sAlloc5(&tAlloc5);

            Catalog cat;
            populateCatalog(&cat,SPEC);

            // create arrays with the two catalogs
            // with two types of allocators
            const EType::Type *catSt = (cat.size() > 0)
                                      ? &cat.front()
                                      : NULL;

            const Desc **D = DESCRIPTORS;
            CAI mA(catSt, cat.size(), D, AggOption::BDEM_WRITE_MANY, &tAlloc1);
            CAI mB(catSt, cat.size(), D, AggOption::BDEM_SUBORDINATE,&sAlloc2);
            const CAI& A = mA;
            const CAI& B = mB;

            // populate our test arrays
            populateData(&mA,VALUES_A);
            populateData(&mB,VALUES_A);

            LOOP_ASSERT(testI, A == B);

            for (int modeI = 0; modeI < 4; ++modeI) {
                AggOption::AllocationStrategy mode =
                                         (AggOption::AllocationStrategy) modeI;

                bslma_Allocator *alloc4 = NULL;
                bslma_Allocator *alloc5 = NULL;
                if  (mode & AggOption::BDEM_NODESTRUCT_FLAG) {
                    alloc4 = &sAlloc4;
                    alloc5 = &sAlloc5;
                }
                else {
                    alloc4 = &tAlloc4;
                    alloc5 = &tAlloc5;
                }

                // Set a default alloc guard
                // and allocate an array with each constructor
                // ensures the default allocator is specified correctly
                const bslma_DefaultAllocatorGuard dag1(&tAlloc3);

                CAI mX1(A);                const CAI &X1 = mX1;
                CAI mY1(A,alloc4);         const CAI &Y1 = mY1;
                CAI mZ1(A,mode,alloc5);    const CAI &Z1 = mZ1;

                // This tests that the mode and allocator used in copy
                // construction by looking into the sub objects
                // and investigating their state.
                if (cat.size()>0) {
                    LOOP2_ASSERT(testI, modeI,
                                 &tAlloc3 == X1.theItem(0).allocator());
                    LOOP2_ASSERT(testI, modeI,
                                 alloc4==Y1.theItem(0).allocator());
                    if (!(mode & AggOption::BDEM_OWN_ALLOCATOR_FLAG) ) {
                        LOOP2_ASSERT(testI,modeI, alloc5 ==
                                     Z1.theItem(0).allocator());
                    }
                    else {
                        LOOP2_ASSERT(testI,modeI, alloc5 !=
                                     Z1.theItem(0).allocator());

                    }
                }

                LOOP2_ASSERT(testI,modeI, A==X1);
                LOOP2_ASSERT(testI,modeI, A==Y1);
                LOOP2_ASSERT(testI,modeI, A==Z1);
                LOOP2_ASSERT(testI,modeI, B==X1);
                LOOP2_ASSERT(testI,modeI, B==Y1);
                LOOP2_ASSERT(testI,modeI, B==Z1);

                // perturb by modifying a value
                if (cat.size() > 0) {
                    const void *VALUE = VALUES_B[cat[0]];
                    mX1.theModifiableItem(0).makeSelection(0,VALUE);
                    mY1.theModifiableItem(0).makeSelection(0,VALUE);
                    mZ1.theModifiableItem(0).makeSelection(0,VALUE);

                    LOOP2_ASSERT(testI,modeI, A==B);
                    LOOP2_ASSERT(testI,modeI, A!=X1);
                    LOOP2_ASSERT(testI,modeI, A!=Y1);
                    LOOP2_ASSERT(testI,modeI, A!=Z1);
                    LOOP2_ASSERT(testI,modeI, B!=X1);
                    LOOP2_ASSERT(testI,modeI, B!=Y1);
                    LOOP2_ASSERT(testI,modeI, B!=Z1);
                }

                // perturb by adding an element
                CAI mX2(A);                const CAI &X2 = mX2;
                CAI mY2(A,alloc4);         const CAI &Y2 = mY2;
                CAI mZ2(A,mode,alloc5);    const CAI &Z2 = mZ2;

                mX2.insertNullItems(0,1);
                mY2.insertNullItems(0,1);
                mZ2.insertNullItems(0,1);

                LOOP2_ASSERT(testI,modeI, A==B);
                LOOP2_ASSERT(testI,modeI, A!=X2);
                LOOP2_ASSERT(testI,modeI, A!=Y2);
                LOOP2_ASSERT(testI,modeI, A!=Z2);
                LOOP2_ASSERT(testI,modeI, B!=X2);
                LOOP2_ASSERT(testI,modeI, B!=Y2);
                LOOP2_ASSERT(testI,modeI, B!=Z2);

                // perturb by resetting the catalog
                CAI mX3(A);                const CAI &X3 = mX3;
                CAI mY3(A,alloc4);         const CAI &Y3 = mY3;
                CAI mZ3(A,mode,alloc5);    const CAI &Z3 = mZ3;

                // if the catalog is size 0, the result of reset
                // on the second catalog should be equal to the original
                const bool ISEQUAL = (cat.size()==0);
                mX3.reset(); // Note: method not tested
                mY3.reset();
                mZ3.reset();

                // validate that reset had the desired result
                LOOP2_ASSERT(testI,modeI, 0 == mX3.numSelections());
                LOOP2_ASSERT(testI,modeI, 0 == mY3.numSelections());
                LOOP2_ASSERT(testI,modeI, 0 == mZ3.numSelections());

                LOOP2_ASSERT(testI,modeI, A==B);
                LOOP2_ASSERT(testI,modeI, ISEQUAL==(A==X3));
                LOOP2_ASSERT(testI,modeI, ISEQUAL==(A==Y3));
                LOOP2_ASSERT(testI,modeI, ISEQUAL==(A==Z3));
                LOOP2_ASSERT(testI,modeI, ISEQUAL==(B==X3));
                LOOP2_ASSERT(testI,modeI, ISEQUAL==(B==Y3));
                LOOP2_ASSERT(testI,modeI, ISEQUAL==(B==Z3));
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //
        // Concerns:
        //   Any subtle variation in value must be detected by the equality
        //   operators.  The allocator used to construct the object is not
        //   part of the value and does not affect the results of the equality
        //   tests.
        //
        // Plan:
        //  1 Specify a set S of unique object values.  For each object value
        //    iterate over the catalog of types, the array of elements, and
        //    the allocator.  On each iteration perturbate the values (or not).
        //   Verify the correctness of 'operator==' and 'operator!='
        //
        // Testing:
        //  operator==(const bdem_ChoiceArrayImp&, const bdem_ChoiceArrayImp&);
        //  operator!=(const bdem_ChoiceArrayImp&, const bdem_ChoiceArrayImp&);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Equality Operators"
                               << "\n==========================" << bsl::endl;

        if (veryVerbose) {
            bsl::cout << "\tTesting equality choice array"
                      << bsl::endl;
        }

        bslma_TestAllocator       testAllocator1(veryVeryVerbose);
        bslma_TestAllocator       tempAlloc(veryVeryVerbose);
        bdema_SequentialAllocator testAllocator2(&tempAlloc);

        // create a structure holding our variations of allocators
        // and allocation strategy (to lower the number of loop
        // nestings used)
        const struct {
            const AggOption::AllocationStrategy strat1;
            const AggOption::AllocationStrategy strat2;
            bslma_Allocator *alloc1;
            bslma_Allocator *alloc2;
        } ALLOC[] = {
            {
                PASSTH,
                PASSTH,
                &testAllocator1,
                &testAllocator1
            },
            {
                PASSTH,
                PASSTH,
                &testAllocator1,
                &testAllocator2
            },
            {
                AggOption::BDEM_WRITE_ONCE,
                AggOption::BDEM_PASS_THROUGH,
                &testAllocator1,
                &testAllocator1
            },
            {
                AggOption::BDEM_WRITE_ONCE,
                AggOption::BDEM_PASS_THROUGH,
                &testAllocator1,
                &testAllocator2,
            },
        };
        const int NUM_ALLOC = sizeof ALLOC/sizeof *ALLOC;

        // iterate over allocation strategy, test specification,
        // catalog perturbation, and index perturbation.

        for (int allocI = 0; allocI < NUM_ALLOC; ++allocI) {
            // for each combination of allocators
            const AggOption::AllocationStrategy strat1 = ALLOC[allocI].strat1;
            const AggOption::AllocationStrategy strat2 = ALLOC[allocI].strat2;
            bslma_Allocator *alloc1 = ALLOC[allocI].alloc1;
            bslma_Allocator *alloc2 = ALLOC[allocI].alloc2;

            for (int testI = 0; testI < NUM_DATA; ++testI) {
                // for each test in the DATA table
                const int   LINE = DATA[testI].d_line;
                const char *SPEC = DATA[testI].d_catalogSpec;
                const int   LEN  = bsl::strlen(SPEC);

                // iterate over catalog positions to perturbate
                // (-1 indicates no perturbation)
                for (int catIndex = -1; catIndex < LEN; ++catIndex) {
                    // create two catalogs, one perturbed
                    Catalog catA,catB;
                    populateCatalog(&catA, SPEC);
                    populateCatalog(&catB, SPEC);

                    if (catIndex >= 0) {
                        // perturbate the catalog types
                        catB[catIndex] = (EType::Type)
                               ((catB[catIndex] + 1) % EType::BDEM_TIME_ARRAY);
                    }

                    // create arrays with the two catalogs
                    // with two types of allocators
                    const EType::Type *catAStart = (catA.size() > 0)
                                                 ? &catA.front() : NULL;

                    const EType::Type *catBStart = (catB.size() > 0)
                                                 ? &catB.front() : NULL;

                    const Desc **D = DESCRIPTORS;

                    LOOP3_ASSERT(allocI, testI, catIndex,
                                 catA.size() == catB.size());

                    // iterate over the array index to perturbate
                    //-2 indicates an additional element in on array
                    //-1 indicates no perturbation
                    for (int arrI = -2; arrI < LEN; ++arrI) {

                        CAI caA1(catAStart, catA.size(), D, strat1, alloc1);
                        CAI caA2(catAStart, catA.size(), D, strat2, alloc2);
                        CAI caB1(catBStart, catB.size(), D, strat1, alloc1);
                        CAI caB2(catBStart, catB.size(), D, strat2, alloc2);

                        const CAI &CAA1 = caA1;
                        const CAI &CAA2 = caA2;
                        const CAI &CAB1 = caB1;
                        const CAI &CAB2 = caB2;

                        // prepopulate the arrays using valueA()
                        populateData(&caA1, VALUES_A);
                        populateData(&caA2, VALUES_A);
                        populateData(&caB1, VALUES_A);
                        populateData(&caB2, VALUES_A);

                        // perturb B using valueB() values
                        if (arrI >= 0) {
                            const void *valueB = VALUES_B[catB[arrI]];

                            caB1.theModifiableItem(arrI).makeSelection(arrI,
                                                                       valueB);
                            caB2.theModifiableItem(arrI).makeSelection(arrI,
                                                                       valueB);
                        }

                        if (arrI == -2)
                        {
                            caB1.insertNullItems(caB1.length(), 1);
                            caB2.insertNullItems(caB2.length(), 1);
                        }

                        // these should aways be true (the allocator
                        // and alloc strategy don't affect equality)
                        LOOP4_ASSERT(allocI, testI, catIndex, arrI,
                                     CAA1 == CAA2);
                        LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                     CAB1 == CAB2);

                        // if neither the catalog nor array was perturbed
                        // then all of the choice arrays should be equal
                        if ((arrI==-1) && (catIndex==-1)) {

                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         CAA1 == CAB1);
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         CAA1 == CAB2);
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         CAA2 == CAB1);
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         CAA2 == CAB2);
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         !(CAA1 != CAB1));
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         !(CAA1 != CAB2));
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         !(CAA2 != CAB1));
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         !(CAA2 != CAB2));

                        }
                        // if either the array or catalog was perturbed
                        // the A arrays should be different from the B arrays
                        else {
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         CAA1 != CAB1);
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         CAA1 != CAB2);
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         CAA2 != CAB1);
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         CAA2 != CAB2);
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         !(CAA1 == CAB1));
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         !(CAA1 == CAB2));
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         !(CAA2 == CAB1));
                            LOOP4_ASSERT(allocI,testI,catIndex, arrI,
                                         !(CAA2 == CAB2));
                        }
                    }
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING print() FUNCTION AND OUTPUT (<<) OPERATOR:
        //
        // Concerns:
        //   The print() function is correctly forwarded to the appropriate
        //   print function for the type stored in the choice object.  The
        //   '<<' operator prints the output on one line.
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& stream,
        //                       int           level = 0,
        //                       int           spacesPerLevel = 4) const;
        //   operator<<(ostream&, const bdem_ChoiceArrayImp&);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "Testing Output Functions" << bsl::endl
                               << "========================" << bsl::endl;
        {
            if (veryVerbose) {
                bsl::cout << "\tTesting empty choice array" << bsl::endl;
            }

            const char *EXP_P1 = "    {\n"
                                 "        Selection Types: [\n"
                                 "        ]\n"
                                 "    }\n";
            const char *EXP_P2 = "{\n"
                                 "        Selection Types: [\n"
                                 "        ]\n"
                                 "    }\n";
            const char *EXP_P3 = "    { Selection Types: [ ] }";
            const char *EXP_P4 = "{ Selection Types: [ ] }";
            const char *EXP_OP = "{ Selection Types: [ ] }";

            CAI mX; const CAI& X = mX;

            bsl::ostringstream os1, os2, os3, os4, os5;
            X.print(os1, 1, 4);
            X.print(os2, -1, 4);
            X.print(os3, 1, -4);
            X.print(os4, -1, -4);
            os5 << X;

            LOOP2_ASSERT(EXP_P1,
                         os1.str(),
                         0 == bsl::strcmp(EXP_P1, os1.str().c_str()));
            LOOP2_ASSERT(EXP_P2,
                         os2.str(),
                         0 == bsl::strcmp(EXP_P2, os2.str().c_str()));
            LOOP2_ASSERT(EXP_P3,
                         os3.str(),
                         0 == bsl::strcmp(EXP_P3, os3.str().c_str()));
            LOOP2_ASSERT(EXP_P4,
                         os4.str(),
                         0 == bsl::strcmp(EXP_P4, os4.str().c_str()));
            LOOP2_ASSERT(EXP_OP,
                         os5.str(),
                         0 == bsl::strcmp(EXP_OP, os5.str().c_str()));
        }

        {
            const int MAX_PRINT_TESTS = 4;
            const struct {
                    int         d_line;       // Line number
                    const char  *d_spec;       // Type Specification string
                    char        d_valueSpec;  // Spec specifying the value
                                              // for that
                    // type.  Can be one of 'A', 'B' or 'N'
                    // corresponding to the A, B or N value
                    // for that type.
                    struct {
                            int         d_level;      // Level to print at
                            int         d_spacesPerLevel; // spaces per level
                                                          // to print at
                            const char *d_printExpOutput; // Expected o/p from
                                                          // print
                    } PRINT_TESTS[MAX_PRINT_TESTS];
                    const char *d_outputOpExpOutput; // Expected o/p from
                                                     // operator<<
            } DATA[] = {
                {
                    L_,
                    "A",
                    'A',
                    {
                        {
                            1,
                            4,
                            "    {\n"
                            "        Selection Types: [\n"
                            "            CHAR\n"
                            "        ]\n"
                            "        Item 0: {\n"
                            "            CHAR A\n"
                            "        }\n"
                            "    }\n",
                        },
                        {
                            -1,
                            4,
                            "{\n"
                            "        Selection Types: [\n"
                            "            CHAR\n"
                            "        ]\n"
                            "        Item 0: {\n"
                            "            CHAR A\n"
                            "        }\n"
                            "    }\n",
                        },
                        {
                            1,
                            -4,
                            "    { Selection Types: [ CHAR ] "
                            "Item 0: { CHAR A } }",
                        },
                        {
                            -1,
                            -4,
                            "{ Selection Types: [ CHAR ] Item 0: { CHAR A } }",
                        },
                    },
                    "{ Selection Types: [ CHAR ] Item 0: { CHAR A } }"
                },
                {
                    L_,
                    "C",
                    'A',
                    {
                        {
                            1,
                            4,
                            "    {\n"
                            "        Selection Types: [\n"
                            "            INT\n"
                            "        ]\n"
                            "        Item 0: {\n"
                            "            INT 10\n"
                            "        }\n"
                            "    }\n",
                        },
                        {
                            -1,
                            4,
                            "{\n"
                            "        Selection Types: [\n"
                            "            INT\n"
                            "        ]\n"
                            "        Item 0: {\n"
                            "            INT 10\n"
                            "        }\n"
                            "    }\n",
                        },
                        {
                            1,
                            -4,
                            "    { Selection Types: [ INT ] Item 0: { INT 10 }"
                            " }",
                        },
                        {
                            -1,
                            -4,
                            "{ Selection Types: [ INT ] Item 0: { INT 10 } }",
                        },
                    },
                    "{ Selection Types: [ INT ] Item 0: { INT 10 } }"
                },
                {
                    L_,
                    "AC",
                    'A',
                    {
                        {
                            1,
                            4,
                            "    {\n"
                            "        Selection Types: [\n"
                            "            CHAR\n"
                            "            INT\n"
                            "        ]\n"
                            "        Item 0: {\n"
                            "            CHAR A\n"
                            "        }\n"
                            "        Item 1: {\n"
                            "            INT 10\n"
                            "        }\n"
                            "    }\n",
                        },
                        {
                            -1,
                            4,
                            "{\n"
                            "        Selection Types: [\n"
                            "            CHAR\n"
                            "            INT\n"
                            "        ]\n"
                            "        Item 0: {\n"
                            "            CHAR A\n"
                            "        }\n"
                            "        Item 1: {\n"
                            "            INT 10\n"
                            "        }\n"
                            "    }\n",
                        },
                        {
                            1,
                            -4,
                            "    { Selection Types: [ CHAR INT ] "
                            "Item 0: { CHAR A } Item 1: { INT 10 } }",
                        },
                        {
                            -1,
                            -4,
                            "{ Selection Types: [ CHAR INT ] "
                            "Item 0: { CHAR A } Item 1: { INT 10 } }",
                        },
                    },
                    "{ Selection Types: [ CHAR INT ] "
                    "Item 0: { CHAR A } Item 1: { INT 10 } }"
                },
                {
                    L_,
                    "AMG",
                    'A',
                    {
                        {
                            1,
                            4,
                            "    {\n"
                            "        Selection Types: [\n"
                            "            CHAR\n"
                            "            INT_ARRAY\n"
                            "            STRING\n"
                            "        ]\n"
                            "        Item 0: {\n"
                            "            CHAR A\n"
                            "        }\n"
                            "        Item 1: {\n"
                            "            INT_ARRAY [\n"
                            "                10\n"
                            "            ]\n"
                            "        }\n"
                            "        Item 2: {\n"
                            "            STRING one\n"
                            "        }\n"
                            "    }\n",
                        },
                        {
                            -1,
                            4,
                            "{\n"
                            "        Selection Types: [\n"
                            "            CHAR\n"
                            "            INT_ARRAY\n"
                            "            STRING\n"
                            "        ]\n"
                            "        Item 0: {\n"
                            "            CHAR A\n"
                            "        }\n"
                            "        Item 1: {\n"
                            "            INT_ARRAY [\n"
                            "                10\n"
                            "            ]\n"
                            "        }\n"
                            "        Item 2: {\n"
                            "            STRING one\n"
                            "        }\n"
                            "    }\n",
                        },
                        {
                            1,
                            -4,
                            "    { Selection Types: [ CHAR INT_ARRAY STRING ] "
                            "Item 0: { CHAR A } "
                            "Item 1: { INT_ARRAY [ 10 ] } "
                            "Item 2: { STRING one } }",
                        },
                        {
                            -1,
                            -4,
                            "{ Selection Types: [ CHAR INT_ARRAY STRING ] "
                            "Item 0: { CHAR A } "
                            "Item 1: { INT_ARRAY [ 10 ] } "
                            "Item 2: { STRING one } }",
                        },
                    },
                    "{ Selection Types: [ CHAR INT_ARRAY STRING ] "
                    "Item 0: { CHAR A } "
                    "Item 1: { INT_ARRAY [ 10 ] } "
                    "Item 2: { STRING one } }"
                },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE     = DATA[i].d_line;
                const char *SPEC     = DATA[i].d_spec;
                const char  VAL_SPEC = DATA[i].d_valueSpec;
                const char *OP_EXP   = DATA[i].d_outputOpExpOutput;

                bslma_TestAllocator alloc1(veryVeryVerbose);
                Catalog cat;
                populateCatalog(&cat,SPEC);
                const EType::Type *catPtr = cat.size() > 0 ? &cat.front()
                                                           : NULL;
                CAI mX(catPtr, cat.size(), DESCRIPTORS,
                       PASSTH,&alloc1);
                const CAI& X = mX;
                mX.insertNullItems(0,cat.size());

                // Set Element j to a catalog value of j
                for (int j = 0; j < cat.size(); ++j) {
                    const char SPEC_V = SPEC[j];
                    const Desc *DESC  = getDescriptor(SPEC_V);
                    EType::Type TYPE  = getElemType(SPEC_V);

                    const void *VALUE;
                    if ('A' == VAL_SPEC) {
                        VALUE = getValueA(SPEC_V);
                    }
                    else if ('B' == VAL_SPEC) {
                        VALUE = getValueB(SPEC_V);
                    }
                    else if ('N' == VAL_SPEC) {
                        VALUE = getValueN(SPEC_V);
                    }
                    else {
                        ASSERT(0);
                    }
                    mX.theModifiableItem(j).makeSelection(j, VALUE);
                }

                bsl::ostringstream os;
                os << X;
                LOOP2_ASSERT(OP_EXP,
                             os.str(),
                             0 == bsl::strcmp(OP_EXP, os.str().c_str()));

                for (int j = 0; j < MAX_PRINT_TESTS; ++j) {
                    const int   LEVEL     = DATA[i].PRINT_TESTS[j].d_level;
                    const int   SPL       =
                        DATA[i].PRINT_TESTS[j].d_spacesPerLevel;
                    const char *PRINT_EXP =
                        DATA[i].PRINT_TESTS[j].d_printExpOutput;

                    bsl::ostringstream os1;
                    X.print(os1, LEVEL, SPL);
                    LOOP2_ASSERT(PRINT_EXP,
                                 os1.str(),
                                 0 == bsl::strcmp(PRINT_EXP,
                                                  os1.str().c_str()));
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING HELPER FUNCTIONS populateData
        //   Ensure that the helper functions to create a
        //   an array of data from a specification string works correctly.
        //
        // Concerns:
        //   - Given valid input the generator function 'populateData'
        //     creates the correct array definition.
        // Plan:
        //   1. Test an empty spec string
        //   2. Test all single element spec strings
        //   3. Test a long spec string containing at least 1
        //      of each element.
        //
        // Tactics:
        //   - Brute force implementation
        //
        // Testing:
        //   void populateData(bdem_ChoiceArrayImp *, const char *);
        //
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << endl
                      <<"TESTING HELPER FUNCTION populateData" << endl
                      <<"====================================" << endl;
        }
        {
            // data cant be populated with aggregate types (List, Table, etc)
            // so only allow non-aggregate values in the spec
            const char *SPEC = "ABCDEFGHIJKLMNOPQRSTWQXYZabcd";
            const int LEN = strlen(SPEC);

            if (verbose) bsl::cout << "TESTING 'populateCatalog'" << endl;
            {
                if (veryVerbose) {
                    bsl::cout << "Testing with empty string" << endl;
                }
                // create and populate mX
                bslma_TestAllocator tAlloc(veryVeryVerbose);
                CAI mX(NULL,0,DESCRIPTORS,PASSTH,&tAlloc);
                const CAI &X = mX;
                populateData(&mX,VALUES_A);

                ASSERT(0 == X.length());
                ASSERT(0 == X.numSelections());
            }
            {
                if (veryVerbose) bsl::cout << "Testing all types" << endl;
                char spec [] = "%";
                for (int i = 0; i < LEN; ++i) {
                    spec[0] = SPEC[i];
                    Catalog c;
                    populateCatalog(&c,spec);
                    EType::Type *catSt = (c.size()>0) ? &c.front()
                        : NULL;
                    // create and populate mX
                    bslma_TestAllocator tAlloc(veryVeryVerbose);
                    CAI mX(catSt,c.size(),DESCRIPTORS,PASSTH,&tAlloc);
                    const CAI &X = mX;
                    populateData(&mX,VALUES_A);

                    ASSERT(1 == X.length());
                    ASSERT(0 == X.theItem(0).selector());
                    ASSERT(compare(
                               getValueA(spec[0]),
                               X.theItem(0).selectionPointer(),
                               spec[0]));
                }
            }
            {
                if (veryVerbose) {
                    bsl::cout << "Testing a large invocation touching"
                                 "all tokens multiple times" << endl;
                }
                // create a spec string and a matching vector of ETypes
                string spec;
                vector<EType::Type> indexVec;
                // cover all the types
                for (int i = 0; i < LEN; ++i) {
                    spec += SPEC[i];
                    indexVec.push_back(cElemTypes[i]);
                }
                // add a bunch of random types for good measure
                for (int i = 0; i < 4 * LEN; ++i) {
                    int r = rand() % LEN;
                    spec += SPEC[r];
                    indexVec.push_back(cElemTypes[r]);
                }

                Catalog c;
                populateCatalog(&c,spec.data());
                ASSERT (c.size() == spec.length());
                ASSERT (c.size() == indexVec.size());

                EType::Type *catSt = (c.size()>0) ? &c.front()
                    : NULL;
                // create and populate mX
                bslma_TestAllocator tAlloc(veryVeryVerbose);
                CAI mX(catSt,c.size(),DESCRIPTORS,PASSTH,&tAlloc);
                const CAI &X = mX;

                // ensure that we only allocate the space in the array
                // that we need
                mX.insertNullItems(0,5);
                populateData(&mX,VALUES_A);
                ASSERT(c.size()==mX.length());

                for (int i = 0; i < c.size(); ++i) {
                    LOOP_ASSERT(i,i==X.theItem(i).selector());
                    LOOP_ASSERT(i,
                                compare(getValueA(spec[i]),
                                        X.theItem(i).selectionPointer(),
                                        spec[i]));
                }
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS AND PRIMARY ACCESSORS:
        //   Verify the most basic functionality of
        //   'bdem_ChoiceArrayImpl'.  Note the ChoiceArrayImpl delegates a
        //   majority of it's functionality.  We only need to ensure the
        //   delegation happens correctly.
        //
        // Concerns:
        //   We are able to create a 'bdem_ChoiceArrayImp' object and put it
        //   into any legal state.  We are not concerned in this
        //   test about trying to move the object from one state to another.
        //
        // Plan:
        //   1. Create an empty choice array, verify length and
        //      insertNullItems(0,0);
        //   2. For each type T, create an empty choice array.  Insert 1 unset
        //      item.  Verify index 0 now points to an unset choice element.
        //      Set the choice at index 0 to be an object O of type T.
        //      verify index 0 now points to a choice element that holds O.
        //   3. For each data point D in the array of sample data, walk through
        //      the elements of D inserting an unset item, verifying that
        //      a unset choice has been added to the array, set the new choice
        //      , verify that the new choice and all previous choice values
        //      are correct.
        //   4. For each data point D create a catalog then create an
        //      array using that Catalog.  Assure that items added to the
        //      array share the same catalog as the array
        //   5. Create a choice array with data values set to an arbitrary
        //      value.  Attempt inserting unset values to each of the points
        //      in the array.  Test with varying numbers of inserted items.
        //      Ensure that the correct number of unset choice header elements
        //      are added to the correct position in the choice array imp
        //
        // Testing:
        //   int length() const;
        //   bdem_ChoiceHeader& theModifiableItem(int index);
        //   const bdem_ChoiceHeader& theItem(int index) const;
        //   void insertNullItems(int dstIndex, int elemCount);
        //   void insertNullItems(int dstIndex, int elemCount);
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTESTING PRIMARY MANIPULATORS AND ACCESSORS"
                      << bsl::endl
                      << "============================================"
                      << bsl::endl;
        }

        // Test empty choice array
        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);
            if (verbose) cout << "\tTest empty choice arrays." <<endl;
            CAI array1(&testAllocator);

            ASSERT(array1.length() == 0);
        }

        // Test single element choice array
        if (verbose) {
            cout << "\tTest length one choice arrays." <<endl;
        }

        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Catalog cat;
            populateCatalog(&cat, SPECIFICATIONS);
            for (int i = 0; i < 4; ++i) {
                AggOption::AllocationStrategy mode =
                                             (AggOption::AllocationStrategy) i;

#if !defined(BSLS_PLATFORM__CMP_MSVC)
                BEGIN_BSLMA_EXCEPTION_TEST {
#endif
                for (int j = 0; j < NUM_CTYPES; ++j) {

                    // Loop through all types skipping the aggregate
                    // types which are at a higher level
                    if (EType::isAggregateType(cElemTypes[j]))
                        continue;

                    if (veryVeryVerbose) {
                        cout << "\tTesting insert and index functions for "
                                "index 0 for " << SPECIFICATIONS[cElemTypes[j]]
                             << endl;
                    }

                    // set up our test allocator based on the alloc strategy
                    bdema_SequentialAllocator  seqAlloc(&testAllocator);
                    bslma_Allocator           *alloc= NULL;

                    if  (!(mode & AggOption::BDEM_NODESTRUCT_FLAG)) {
                        alloc = &testAllocator;
                    }
                    else {
                        alloc = &seqAlloc;
                    }

                    CAI mX(&cat.front(), cat.size(), DESCRIPTORS, mode, alloc);
                    const CAI& X = mX;

                    ASSERT(X.length() == 0);

                    mX.insertNullItems(0, 1);

                    // Verify that we've added an unset choice
                    ASSERT(X.length() == 1);
                    ASSERT(X.theItem(0).selector() == -1);
                    ASSERT(X.theItem(0).isSelectionNull());

                    // Set the choice  and verify it's modified value
                    const char  type   = SPECIFICATIONS[j];
                    const void *valueA = getValueA(type);
                    mX.theModifiableItem(0).makeSelection(j, valueA);
                    ASSERT(X.theItem(0).selector() == j);
                    ASSERT(!X.theItem(0).isSelectionNull());
                    ASSERT(compare(X.theItem(0).selectionPointer(),
                                   valueA,
                                   type));
                    ASSERT(compare(mX.theModifiableItem(0).selectionPointer(),
                                   valueA,
                                   type));

                    const void *valueB = getValueB(type);
                    mX.theModifiableItem(0).makeSelection(j,valueB);
                    ASSERT(X.theItem(0).selector() == j);
                    ASSERT(!X.theItem(0).isSelectionNull());
                    ASSERT(compare(X.theItem(0).selectionPointer(),
                                   valueB,
                                   type));
                    ASSERT(compare(mX.theModifiableItem(0).selectionPointer(),
                                   valueB,
                                   type));

                    const void *valueN = getValueN(type);
                    mX.theModifiableItem(0).makeSelection(j,valueN);
                    ASSERT(X.theItem(0).selector() == j);
                    ASSERT(!X.theItem(0).isSelectionNull());
                    ASSERT(compare(X.theItem(0).selectionPointer(),
                                   valueN,
                                   type));
                    ASSERT(compare(mX.theModifiableItem(0).selectionPointer(),
                                   valueN,
                                   type));

                    CAI mY(&cat.front(), cat.size(), DESCRIPTORS, mode, alloc);
                    const CAI& Y = mY;
                    ASSERT(Y.length() == 0);

                    mY.insertNullItems(0,1);

                    // Verify that we've added an null choice
                    ASSERT(Y.length() == 1);
                    ASSERT(Y.theItem(0).selector() == -1);
                    ASSERT(Y.theItem(0).isSelectionNull());

                    // Set the choice  and verify it's modified value
                    mY.theModifiableItem(0).makeSelection(j, valueA);
                    ASSERT(Y.theItem(0).selector() == j);
                    ASSERT(!Y.theItem(0).isSelectionNull());
                    ASSERT(compare(Y.theItem(0).selectionPointer(),
                                   valueA,
                                   type));
                    ASSERT(compare(mY.theItem(0).selectionPointer(),
                                   valueA,
                                   type));
                    ASSERT(!isNull(Y, 0));
                }
#if !defined(BSLS_PLATFORM__CMP_MSVC)
                } END_BSLMA_EXCEPTION_TEST
#endif
            }
        }

        // Test insert(i, 1) and index functions for choice arrays whose data
        // is based on the DATA table of specs
        {
            if (verbose) {
                cout << "\tTest insert(i,1) and index fns for choice arrays"
                        " based on data from a table test specs" << endl;
            }

            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Catalog cat;
            populateCatalog(&cat,SPECIFICATIONS);

            // we've already tested that the allocation strategy and allocator
            // is passed to the delegated types and is works correctly for
            // insertNullItem and index functions
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                if (veryVerbose) {
                    cout << "Testing insertNullItem(i,1) and index fns for "
                            "array based on spec '" << SPEC << "'" << endl;
                }

                CAI mX(&cat.front(), cat.size(), DESCRIPTORS,
                       PASSTH, &testAllocator);
                const CAI& X = mX;

                LOOP_ASSERT(i, 0 == X.length());

                for (int j = 0; j < LEN; ++j) {
                    mX.insertNullItems(j, 1);

                    // verify a unset choice has been added
                    LOOP2_ASSERT(i, j, j + 1 == X.length());
                    LOOP2_ASSERT(i, j, -1    == X.theItem(j).selector());

                    // set the new element to a value and verify that value
                    // is set
                    char type = SPEC[j];
                    int selection = getElemType(type);

                    const void *valueA = getValueA(type);
                    mX.theModifiableItem(j).makeSelection(selection, valueA);
                    LOOP2_ASSERT(i, j, selection == X.theItem(j).selector());
                    LOOP2_ASSERT(i, j, compare(X.theItem(j).selectionPointer(),
                                               valueA,
                                               type));
                    LOOP2_ASSERT(i, j, !X.theItem(j).isSelectionNull());

                    const void *valueB = getValueB(type);
                    mX.theModifiableItem(j).makeSelection(selection, valueB);
                    LOOP2_ASSERT(i,j,selection == X.theItem(j).selector());
                    LOOP2_ASSERT(i,j, compare(X.theItem(j).selectionPointer(),
                                              valueB,
                                              type));
                    LOOP2_ASSERT(i, j, !X.theItem(j).isSelectionNull());

                    const void *valueN = getValueN(type);
                    mX.theModifiableItem(j).makeSelection(selection, valueN);
                    LOOP2_ASSERT(i, j, selection == X.theItem(j).selector());
                    LOOP2_ASSERT(i, j, compare(X.theItem(j).selectionPointer(),
                                               valueN,
                                               type));
                    LOOP2_ASSERT(i, j, !X.theItem(j).isSelectionNull());

                    // verify that none of the previous choices were affected
                    for (int k = 0; k < j; ++k) {
                        const char type = SPEC[k];
                        const int selection = getElemType(type);
                        // all previous entries should have been left with
                        // valueN for that type
                        const void *valueN = getValueN(type);
                        LOOP3_ASSERT(i, j, k, selection
                                                   == X.theItem(k).selector());
                        LOOP3_ASSERT(i, j, k,
                                     compare(X.theItem(k).selectionPointer(),
                                             valueN,
                                             type));
                        LOOP3_ASSERT(i, j, k,
                            compare(mX.theModifiableItem(k).selectionPointer(),
                                    valueN,
                                    type));
                        LOOP3_ASSERT(i, j, k, !X.theItem(k).isSelectionNull());
                    }
                }

                CAI mY(&cat.front(), cat.size(), DESCRIPTORS,
                       PASSTH, &testAllocator);
                const CAI& Y = mY;

                LOOP_ASSERT(i, 0 == Y.length());

                for (int j = 0; j < LEN; ++j) {
                    mY.insertNullItems(j, 1);

                    // verify a unset choice has been added
                    LOOP2_ASSERT(i, j, j + 1 == Y.length());
                    LOOP2_ASSERT(i, j, -1    == Y.theItem(j).selector());
                    LOOP2_ASSERT(i, j, Y.theItem(j).isSelectionNull());

                    // set the new element to a value and verify that value
                    // is set
                    char type = SPEC[j];
                    int selection = getElemType(type);

                    const void *valueA = getValueA(type);
                    mY.theModifiableItem(j).makeSelection(selection,valueA);
                    LOOP2_ASSERT(i, j, selection == Y.theItem(j).selector());
                    LOOP2_ASSERT(i, j, compare(Y.theItem(j).selectionPointer(),
                                               valueA,
                                               type));
                    LOOP2_ASSERT(i, j, !Y.theItem(j).isSelectionNull());

                    const void *valueN = getValueN(type);
                    mY.makeSelection(j,selection).makeNull();
                    LOOP2_ASSERT(i,j,selection == Y.theItem(j).selector());
                    LOOP2_ASSERT(i,j,
                                 compare(Y.theItem(j).selectionPointer(),
                                         valueN,
                                         type));
                    LOOP2_ASSERT(i, j, Y.theItem(j).isSelectionNull());

                    // verify that none of the previous choices were affected
                    for (int k = 0; k < j; ++k) {
                        const char type = SPEC[k];
                        const int selection = getElemType(type);
                        // all previous entries should have been left with
                        // valueN for that type
                        const void *valueN = getValueN(type);
                        LOOP3_ASSERT(i, j, k, selection
                                                   == Y.theItem(k).selector());
                        LOOP3_ASSERT(i, j, k,
                                     compare(Y.theItem(k).selectionPointer(),
                                             valueN,
                                             type));
                        LOOP3_ASSERT(i, j, k,
                                     compare(mY.theItem(k).selectionPointer(),
                                             valueN,
                                             type));
                        LOOP3_ASSERT(i, j, k, Y.theItem(j).isSelectionNull());
                    }
                }
            }
        }
        // Test that the elements in a choice array share the same
        // type catalog with the array itself over catalogs defined
        // from the DATA table of test specs
        {
            if (verbose) {
                cout << "\tTest the catalogs for elements of a choice array "
                        "using catalogs defined from a table of test specs"
                     << endl;
            }
            const int NUM_TEST_ELEMS = 4;

            bslma_TestAllocator testAllocator(veryVeryVerbose);

            // we've already tested that the allocation strategy and allocator
            // is passed to the delegated types and is works correctly for
            // insertNullItem and index functions
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_catalogSpec;
                const int   LEN  = bsl::strlen(SPEC);

                if (veryVerbose) {
                    cout << "Testing the element catalogs for a type "
                            "catalog based on spec '" << SPEC << "'" << endl;
                }

                Catalog cat;
                populateCatalog(&cat, SPEC);
                const EType::Type *catPtr = cat.size() ? &cat.front() : NULL;

                CAI mX(catPtr, cat.size(), DESCRIPTORS,
                       PASSTH, &testAllocator);
                const CAI& X = mX;

                LOOP_ASSERT(i, 0 == X.length());
                mX.insertNullItems(0, NUM_TEST_ELEMS);
                LOOP_ASSERT(i, NUM_TEST_ELEMS == X.length());
                for (int j = 0; j < NUM_TEST_ELEMS; ++j) {
                    LOOP2_ASSERT(i, j, LEN == X.theItem(j).numSelections());
                    LOOP2_ASSERT(i, j, X.theItem(j).isSelectionNull());
                    LOOP2_ASSERT(i, j,
                            X.numSelections() == X.theItem(j).numSelections());

                    for (int selI = 0;
                         selI < X.theItem(j).numSelections();
                         ++selI) {
                        // verify the selection type via the descriptor
                        const Desc *desc =
                                        X.theItem(j).selectionDescriptor(selI);
                        const Desc *expected = getDescriptor(SPEC[selI]);

                        LOOP3_ASSERT(i, j, selI, expected == desc);
                    }
                }
                CAI mY(catPtr, cat.size(), DESCRIPTORS,
                       PASSTH, &testAllocator);
                const CAI& Y = mY;

                LOOP_ASSERT(i, 0 == Y.length());
                mY.insertNullItems(0, NUM_TEST_ELEMS);
                LOOP_ASSERT(i, NUM_TEST_ELEMS == Y.length());
                for (int j = 0; j < NUM_TEST_ELEMS; ++j) {
                    LOOP2_ASSERT(i, j, LEN == Y.theItem(j).numSelections());
                    LOOP2_ASSERT(i, j, Y.theItem(j).isSelectionNull());
                    LOOP2_ASSERT(i, j,
                            Y.numSelections() == Y.theItem(j).numSelections());

                    for (int selI = 0;
                         selI < Y.theItem(j).numSelections();
                         ++selI){
                        // verify the selection type via the descriptor
                        const Desc *desc =
                                        Y.theItem(j).selectionDescriptor(selI);
                        const Desc *expected = getDescriptor(SPEC[selI]);

                        LOOP3_ASSERT(i,j,selI,expected == desc);
                    }
                }
            }
        }

        // Test insertNullItem in various points with various lengths
        // in the array
        {
            if (verbose) {
                cout << "\tTest insertNullItems at all positions with "
                        "various lengths in a populated array"
                     << endl;
            }

            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Catalog cat;
            populateCatalog(&cat, SPECIFICATIONS);

            const int TEST_SIZE = 20;
            // iterate over where to add the spaces
            for (int indexToAdd = 0; indexToAdd < TEST_SIZE; ++indexToAdd) {
                // iterate over the number of spaces to add
                for (int numSpaces = 0; numSpaces < 4; ++numSpaces) {

                    if (veryVerbose) {
                        cout << "\tInserting " << numSpaces << " spaces at "
                             << "position " << indexToAdd << " in the array"
                             << endl;
                    }

                    CAI mX(&cat.front(), cat.size(), DESCRIPTORS,
                           PASSTH, &testAllocator);
                    const CAI& X = mX;

                    // We're not concerned with the actual dummy data
                    const char  type      = 'A';
                    const void *valueN    = getValueN(type);
                    const int   selection = getElemType(type);

                    // we've only tested insertNullItem(i, 1) up to this point
                    int i;
                    for (i = 0; i < TEST_SIZE; ++i) {
                        mX.insertNullItems(i, 1);
                        mX.theModifiableItem(i).makeSelection(selection,
                                                              valueN);
                    }
                    ASSERT(TEST_SIZE == X.length());
                    for (i = 0; i< TEST_SIZE; ++i) {
                        LOOP2_ASSERT(indexToAdd, i,
                                     selection == X.theItem(i).selector());
                        LOOP2_ASSERT(indexToAdd,i,
                                     compare(X.theItem(i).selectionPointer(),
                                             valueN,type));
                        mX.theModifiableItem(i).makeSelection(selection,
                                                              valueN);
                    }

                    mX.insertNullItems(indexToAdd,numSpaces);
                    ASSERT(X.length() == numSpaces + TEST_SIZE);
                    // verify that previous elements haven't been affected
                    for (i = 0; i < indexToAdd; ++i) {
                        LOOP2_ASSERT(indexToAdd,i,selection ==
                                     X.theItem(i).selector());
                        LOOP2_ASSERT(indexToAdd,i,
                                     compare(X.theItem(i).selectionPointer(),
                                             valueN,type));
                    }

                    // verify we've added unset items
                    for (; i < indexToAdd+numSpaces; ++i) {
                        LOOP2_ASSERT(indexToAdd,i,-1 ==
                                     X.theItem(i).selector());
                    }

                    // verify that subsequent elements haven't been affected
                    for (; i < TEST_SIZE + numSpaces; ++i) {
                        LOOP2_ASSERT(indexToAdd,i,selection ==
                                     X.theItem(i).selector());
                        LOOP2_ASSERT(indexToAdd,i,
                                     compare(X.theItem(i).selectionPointer(),
                                             valueN,type));
                    }
                    CAI mY(&cat.front(), cat.size(), DESCRIPTORS,
                           PASSTH, &testAllocator);
                    const CAI& Y = mY;

                    // we've only tested insertNullItem(i,1) up to this point
                    for (i = 0; i < TEST_SIZE; ++i) {
                        mY.insertNullItems(i,1);
                        mY.theModifiableItem(i).makeSelection(selection,
                                                              valueN);
                    }
                    ASSERT(TEST_SIZE == Y.length());
                    for (i = 0; i<TEST_SIZE; ++i) {
                        LOOP2_ASSERT(indexToAdd,i,
                                     selection == Y.theItem(i).selector());
                        LOOP2_ASSERT(indexToAdd,i,
                                     compare(Y.theItem(i).selectionPointer(),
                                             valueN,type));
                        LOOP2_ASSERT(indexToAdd,i, !isNull(Y, i));
                    }

                    mY.insertNullItems(indexToAdd,numSpaces);
                    ASSERT(Y.length()==numSpaces + TEST_SIZE);
                    // verify that previous elements haven't been affected
                    for (i = 0; i < indexToAdd; ++i) {
                        LOOP2_ASSERT(indexToAdd,i,selection ==
                                     Y.theItem(i).selector());
                        LOOP2_ASSERT(indexToAdd,i,
                                     compare(Y.theItem(i).selectionPointer(),
                                             valueN,type));
                        LOOP2_ASSERT(indexToAdd,i, !isNull(Y, i));
                    }

                    // verify we've added null items
                    for (; i < indexToAdd+numSpaces; ++i) {
                        LOOP2_ASSERT(indexToAdd,i,-1 ==
                                     Y.theItem(i).selector());
                        LOOP2_ASSERT(indexToAdd,i, isNull(Y, i));
                    }

                    // verify that subsequent elements haven't been affected
                    for (; i < TEST_SIZE+numSpaces; ++i) {
                        LOOP2_ASSERT(indexToAdd,i,selection ==
                                     Y.theItem(i).selector());
                        LOOP2_ASSERT(indexToAdd,i,
                                     compare(Y.theItem(i).selectionPointer(),
                                             valueN,type));
                        LOOP2_ASSERT(indexToAdd,i, !isNull(Y, i));
                    }
                }
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY CONSTRUCTORS (BOOTSTRAP) / PRIMARY ACCESSORS
        //   Ensure that the primary constructors are "wired-up" and defaults
        //   properly.  We cannot test values of the allocator and allocation
        //   strategy directly, so we must test for their impact.
        //
        // Concerns:
        //   - That the default arguments are passed through to delegated
        //     objects correct
        //   - That specified constructor parameters are passed through to the
        //     delegate objects correctly
        //   - That the catalog of types is installed and returned properly
        //     by the catalog accessors.
        // Plan:
        //   1. To ensure that the constructors take their allocator by
        //      default from 'bslma_Default::allocator'.  This must be
        //      done for each of the 3 constructor variants under test
        //   2. Ensure the default AllocationStrategy is specified as
        //      'BDEM_PASS_THROUGH', or if an AllocationStrategy is specified
        //      it holds the correct value.  This must be done for all the
        //      constructor variants under test
        //   3. Create a test with empty and default type catalog's.  Ensure
        //      The selectionTypes and Descriptors are installed in the
        //      choice array imp.
        //   4. Create tests using a list of interesting catalog's.  Ensure
        //      the selectionTypes and Descriptors are installed correctly in
        //      the choice array imp
        //
        // Tactics:
        //   - Brute force implementation
        //
        // Testing:
        //   bdem_ChoiceArrayImp(bslma_Allocator *basicAlloc = 0);
        //   bdem_ChoiceArrayImp(bslma_Allocator::AllocationStrategy,
        //                       bslma_Allocator *);
        //   bdem_ChoiceArrayImpl(const bdem_ElemType::Type[], int, const
        //                        bdem_Descriptor *const *,
        //                        bdem_Allocator::AllocationStrategy,
        //                        bdem_Allocator *);
        //   int numSelections() const;
        //   bdem_ElemType::Type selectionType(int selection) const;
        //   ~bdem_ChoiceArrayImpl();
        //
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << endl
                      << "PRIMARY CONSTRUCTOR" << endl
                      << "===================" << endl;
        }
        if (verbose) cout <<
            "\nEnsure bdem_Default::allocator() is used by default." << endl;
        {

            // These allocator must be installed as the
            // default in order to pass this test.
            // One allocator for each flavor of constructor.
            bslma_TestAllocator alloc1A, alloc1B, alloc1C;
            EType::Type dummyCat[] = { EType::BDEM_INT };
            const int dummyCatSize = sizeof dummyCat/sizeof *dummyCat;

            if (verbose)
                cout << "\tInstall test allocator 'da' as default." << endl;

            {
                // Setting a default alloc guard
                // and allocating an array with each constructor
                // ensures the default allocator is specified correctly.
                const bslma_DefaultAllocatorGuard dag1(&alloc1A);
                ASSERT(0 == alloc1A.numBlocksTotal());
                CAI array1A;          // using default alloc1A
                const int NBTA = alloc1A.numBlocksTotal();

                const bslma_DefaultAllocatorGuard dag2(&alloc1B);
                ASSERT(0 == alloc1B.numBlocksTotal());
                CAI array1B(PASSTH);  // using default alloc1B
                const int NBTB = alloc1B.numBlocksTotal();

                const bslma_DefaultAllocatorGuard dag3(&alloc1C);
                ASSERT(0 == alloc1C.numBlocksTotal());
                // using default alloc1C
                CAI array1C(dummyCat,dummyCatSize,DESCRIPTORS, PASSTH);
                const int NBTC = alloc1C.numBlocksTotal();

                ASSERT(NBTA > 0);
                ASSERT(NBTB > 0);
                ASSERT(NBTC > 0);

                // Specify alloc guard explicitly to constructors.
                // Ensure that the previous default allocators aren't affected.
                bslma_TestAllocator alloc2A,alloc2B,alloc2C;
                ASSERT(0 == alloc2A.numBlocksTotal());
                ASSERT(0 == alloc2B.numBlocksTotal());
                ASSERT(0 == alloc2C.numBlocksTotal());

                CAI array2A(&alloc2A);
                CAI array2B(PASSTH, &alloc2B);
                CAI array2C(dummyCat, dummyCatSize, DESCRIPTORS,
                            PASSTH, &alloc2C);

                ASSERT(NBTA == alloc1A.numBlocksTotal());
                ASSERT(NBTA == alloc2A.numBlocksTotal());

                ASSERT(NBTB == alloc1B.numBlocksTotal());
                ASSERT(NBTB == alloc2B.numBlocksTotal());

                ASSERT(NBTC == alloc1C.numBlocksTotal());
                ASSERT(NBTC == alloc2C.numBlocksTotal());

                // Installing this other allocator should have no effect on
                // subsequent use of pre-existing list objects.
                bslma_TestAllocator alloc3A(veryVeryVerbose),
                                    alloc3B(veryVeryVerbose),
                                    alloc3C(veryVeryVerbose);

                if (verbose)
                    cout << "\tInstall test allocator 'oa' as default."
                         << endl;
                {
                    const bslma_DefaultAllocatorGuard oag1(&alloc3A);
                    ASSERT(0 == alloc3A.numBlocksTotal());
                    CAI array3A;

                    const bslma_DefaultAllocatorGuard oag2(&alloc3B);
                    ASSERT(0 == alloc3B.numBlocksTotal());
                    CAI array3B(PASSTH);

                    const bslma_DefaultAllocatorGuard oag3(&alloc3C);
                    ASSERT(0 == alloc3C.numBlocksTotal());
                    CAI array3C(dummyCat,dummyCatSize, DESCRIPTORS, PASSTH);

                    ASSERT(NBTA == alloc3A.numBlocksTotal());
                    ASSERT(NBTB == alloc3B.numBlocksTotal());
                    ASSERT(NBTC == alloc3C.numBlocksTotal());

                    ASSERT(NBTA == alloc1A.numBlocksTotal());
                    ASSERT(NBTA == alloc2A.numBlocksTotal());
                    ASSERT(NBTB == alloc1B.numBlocksTotal());
                    ASSERT(NBTB == alloc2B.numBlocksTotal());
                    ASSERT(NBTC == alloc1C.numBlocksTotal());
                    ASSERT(NBTC == alloc2C.numBlocksTotal());

                    // inserting on alloc1
                    array1A.insertNullItems(0,1000); // Note: method not yet
                                                     // tested.
                    array1B.insertNullItems(0,1000); // Note: method not yet
                                                     // tested.
                    array1C.insertNullItems(0,1000); // Note: method not yet
                                                     // tested.

                    ASSERT(NBTA <  alloc1A.numBlocksTotal());
                    ASSERT(NBTB <  alloc1B.numBlocksTotal());
                    ASSERT(NBTC <  alloc1C.numBlocksTotal());
                    ASSERT(NBTA == alloc2A.numBlocksTotal());
                    ASSERT(NBTB == alloc2B.numBlocksTotal());
                    ASSERT(NBTC == alloc2C.numBlocksTotal());
                    ASSERT(NBTA == alloc3A.numBlocksTotal());
                    ASSERT(NBTB == alloc3B.numBlocksTotal());
                    ASSERT(NBTC == alloc3C.numBlocksTotal());

                    // inserting on alloc2
                    array2A.insertNullItems(0,1000);  // Note: method not yet
                                                      // tested.
                    array2B.insertNullItems(0,1000);  // Note: method not yet
                                                      // tested.
                    array2C.insertNullItems(0,1000);  // Note: method not yet
                                                      // tested.

                    ASSERT(NBTA <  alloc1A.numBlocksTotal());
                    ASSERT(NBTB <  alloc1B.numBlocksTotal());
                    ASSERT(NBTC <  alloc1C.numBlocksTotal());
                    ASSERT(NBTA <  alloc2A.numBlocksTotal());
                    ASSERT(NBTB <  alloc2B.numBlocksTotal());
                    ASSERT(NBTC <  alloc2C.numBlocksTotal());
                    ASSERT(NBTA == alloc3A.numBlocksTotal());
                    ASSERT(NBTB == alloc3B.numBlocksTotal());
                    ASSERT(NBTC == alloc3C.numBlocksTotal());

                    // inserting on alloc3
                    array3A.insertNullItems(0,1000);  // Note: method not
                                                      // yet tested.
                    array3B.insertNullItems(0,1000);  // Note: method not
                                                      // yet tested.
                    array3C.insertNullItems(0,1000);  // Note: method not
                                                      // yet tested.

                    ASSERT(NBTA < alloc1A.numBlocksTotal());
                    ASSERT(NBTB < alloc1B.numBlocksTotal());
                    ASSERT(NBTC < alloc1C.numBlocksTotal());
                    ASSERT(NBTA < alloc2A.numBlocksTotal());
                    ASSERT(NBTB < alloc2B.numBlocksTotal());
                    ASSERT(NBTC < alloc2C.numBlocksTotal());
                    ASSERT(NBTA < alloc3A.numBlocksTotal());
                    ASSERT(NBTB < alloc3B.numBlocksTotal());
                    ASSERT(NBTC < alloc3C.numBlocksTotal());

                    ASSERT(alloc2A.numBlocksTotal() ==
                           alloc1A.numBlocksTotal());
                    ASSERT(alloc2B.numBlocksTotal() ==
                           alloc1B.numBlocksTotal());
                    ASSERT(alloc2C.numBlocksTotal() ==
                           alloc1C.numBlocksTotal());
                    ASSERT(alloc2A.numBlocksTotal() ==
                           alloc3A.numBlocksTotal());
                    ASSERT(alloc2B.numBlocksTotal() ==
                           alloc3B.numBlocksTotal());
                    ASSERT(alloc2C.numBlocksTotal() ==
                           alloc3C.numBlocksTotal());
                }

                if (verbose) cout <<
                    "\tUn-install test allocator 'alloc3' as the default."
                                  << endl;
            }
            if (verbose) cout <<
                    "\tUn-install test allocator 'alloc1' as the default."
                              << endl;
        }

        if (verbose) cout << "\nEnsure the mode is passed through correctly "
                             " and the default mode is BDEM_PASS_THROUGH"
                          << endl;

        {
            bslma_TestAllocator aX1, aA1, aB1, aC1, aD1;
            bslma_TestAllocator aX2, aA2, aB2, aC2, aD2;

            const int N0X1 = aX1.numBlocksInUse(), M0X1 = aX1.numBytesInUse();
            const int N0A1 = aA1.numBlocksInUse(), M0A1 = aA1.numBytesInUse();
            const int N0B1 = aB1.numBlocksInUse(), M0B1 = aB1.numBytesInUse();
            const int N0C1 = aC1.numBlocksInUse(), M0C1 = aC1.numBytesInUse();
            const int N0D1 = aD1.numBlocksInUse(), M0D1 = aD1.numBytesInUse();

            const int N0A2 = aA2.numBlocksInUse(), M0A2 = aA2.numBytesInUse();
            const int N0B2 = aB2.numBlocksInUse(), M0B2 = aB2.numBytesInUse();
            const int N0C2 = aC2.numBlocksInUse(), M0C2 = aC2.numBytesInUse();
            const int N0D2 = aD2.numBlocksInUse(), M0D2 = aD2.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N0X1); P_(N0A1); P_(N0B1); P_(N0C1); P(N0D1);
                T_; T_; P_(M0X1); P_(M0A1); P_(M0B1); P_(M0C1); P(M0D1);

                T_; T_; P_(N0A2); P_(N0B2); P_(N0C2); P(N0D2);
                T_; T_; P_(M0A2); P_(M0B2); P_(M0C2); P(M0D2);
            }

            if (verbose) {
                cout << "\tCreate five different choice arrays for"
                        "EACH of the two constructors that take an"
                        "Agg option" << endl;
            }

            // USING THE FIRST CONSTRUCTOR
            CAI cArrX1(                        &aX1);
            CAI cArrA1(AggOption::BDEM_PASS_THROUGH,&aA1);
            CAI cArrB1(AggOption::BDEM_WRITE_ONCE,  &aB1);
            CAI cArrC1(AggOption::BDEM_WRITE_MANY,  &aC1);
            // CAI cArrD1(AggOption::BDEM_SUBORDINATE,&aD1);
            CAI cArrD1(AggOption::BDEM_WRITE_MANY,  &aD1); // dummy

            EType::Type cat[] = { EType::BDEM_INT };
            const int catS = sizeof cat/sizeof *cat;

            // USING THE SECOND CONSTRUCTOR
            CAI cArrA2(cat,catS,DESCRIPTORS,
                       AggOption::BDEM_PASS_THROUGH,&aA2);
            CAI cArrB2(cat,catS,DESCRIPTORS,
                       AggOption::BDEM_WRITE_ONCE,  &aB2);
            CAI cArrC2(cat,catS,DESCRIPTORS,
                       AggOption::BDEM_WRITE_MANY,  &aC2);
            // CAI cArrD2(cat,catS,DESCRIPTORS,
            //            AggOption::BDEM_SUBORDINATE,&aD2);

            CAI cArrD2(cat,catS,DESCRIPTORS, AggOption::BDEM_WRITE_MANY, &aD2);
                                                                       // dummy

            // As it turns out, we would get a memory leak if BDEM_SUBORDINATE
            // were the default, and so this case is addressed as well
            // because the test allocator would report the leak and assert.
            const int N1X1 = aX1.numBlocksInUse(), M1X1 = aX1.numBytesInUse();
            const int N1A1 = aA1.numBlocksInUse(), M1A1 = aA1.numBytesInUse();
            const int N1B1 = aB1.numBlocksInUse(), M1B1 = aB1.numBytesInUse();
            const int N1C1 = aC1.numBlocksInUse(), M1C1 = aC1.numBytesInUse();
            const int N1D1 = aD1.numBlocksInUse(), M1D1 = aD1.numBytesInUse();

            const int N1A2 = aA1.numBlocksInUse(), M1A2 = aA1.numBytesInUse();
            const int N1B2 = aB1.numBlocksInUse(), M1B2 = aB1.numBytesInUse();
            const int N1C2 = aC1.numBlocksInUse(), M1C2 = aC1.numBytesInUse();
            const int N1D2 = aD1.numBlocksInUse(), M1D2 = aD1.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N1X1); P_(N1A1); P_(N1B1); P_(N1C1); P(N1D1);
                T_; T_; P_(M1X1); P_(M1A1); P_(M1B1); P_(M1C1); P(M1D1);

                T_; T_; P_(N1A2); P_(N1B2); P_(N1C2); P(N1D2);
                T_; T_; P_(M1A2); P_(M1B2); P_(M1C2); P(M1D2);
            }

            if (verbose) cout << "\tinsert unset elements." << endl;

            cArrX1.insertNullItems(0,100);
            cArrA1.insertNullItems(0,100);
            cArrB1.insertNullItems(0,100);
            cArrC1.insertNullItems(0,100);
            cArrD1.insertNullItems(0,100);

            cArrA2.insertNullItems(0,100);
            cArrB2.insertNullItems(0,100);
            cArrC2.insertNullItems(0,100);
            cArrD2.insertNullItems(0,100);

            const int N2X1 = aX1.numBlocksInUse(), M2X1 = aX1.numBytesInUse();
            const int N2A1 = aA1.numBlocksInUse(), M2A1 = aA1.numBytesInUse();
            const int N2B1 = aB1.numBlocksInUse(), M2B1 = aB1.numBytesInUse();
            const int N2C1 = aC1.numBlocksInUse(), M2C1 = aC1.numBytesInUse();
            const int N2D1 = aD1.numBlocksInUse(), M2D1 = aD1.numBytesInUse();

            const int N2A2 = aA2.numBlocksInUse(), M2A2 = aA2.numBytesInUse();
            const int N2B2 = aB2.numBlocksInUse(), M2B2 = aB2.numBytesInUse();
            const int N2C2 = aC2.numBlocksInUse(), M2C2 = aC2.numBytesInUse();
            const int N2D2 = aD2.numBlocksInUse(), M2D2 = aD2.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N2X1); P_(N2A1); P_(N2B1); P_(N2C1); P(N2D1);
                T_; T_; P_(M2X1); P_(M2A1); P_(M2B1); P_(M2C1); P(M2D1);

                T_; T_; P_(N2A2); P_(N2B2); P_(N2C2); P(N2D2);
                T_; T_; P_(M2A2); P_(M2B2); P_(M2C2); P(M2D2);
            }

            if (verbose) cout << "\tRemove elements." << endl;

            cArrX1.removeItems(0,100);
            cArrA1.removeItems(0,100);
            cArrB1.removeItems(0,100);
            cArrC1.removeItems(0,100);
            cArrD1.removeItems(0,100);

            cArrA2.removeItems(0,100);
            cArrB2.removeItems(0,100);
            cArrC2.removeItems(0,100);
            cArrD2.removeItems(0,100);

            const int N3X1 = aX1.numBlocksInUse(), M3X1 = aX1.numBytesInUse();
            const int N3A1 = aA1.numBlocksInUse(), M3A1 = aA1.numBytesInUse();
            const int N3B1 = aB1.numBlocksInUse(), M3B1 = aB1.numBytesInUse();
            const int N3C1 = aC1.numBlocksInUse(), M3C1 = aC1.numBytesInUse();
            const int N3D1 = aD1.numBlocksInUse(), M3D1 = aD1.numBytesInUse();

            const int N3A2 = aA2.numBlocksInUse(), M3A2 = aA2.numBytesInUse();
            const int N3B2 = aB2.numBlocksInUse(), M3B2 = aB2.numBytesInUse();
            const int N3C2 = aC2.numBlocksInUse(), M3C2 = aC2.numBytesInUse();
            const int N3D2 = aD2.numBlocksInUse(), M3D2 = aD2.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N3X1); P_(N3A1); P_(N3B1); P_(N3C1); P(N3D1);
                T_; T_; P_(M3X1); P_(M3A1); P_(M3B1); P_(M3C1); P(M3D1);

                T_; T_;  P_(N3A2); P_(N3B2); P_(N3C2); P(N3D2);
                T_; T_;  P_(M3A2); P_(M3B2); P_(M3C2); P(M3D2);
            }

            if (verbose) cout << "\tAppend the empty elements again." << endl;

            cArrX1.insertNullItems(0,100);
            cArrA1.insertNullItems(0,100);
            cArrB1.insertNullItems(0,100);
            cArrC1.insertNullItems(0,100);
            cArrD1.insertNullItems(0,100);

            cArrA2.insertNullItems(0,100);
            cArrB2.insertNullItems(0,100);
            cArrC2.insertNullItems(0,100);
            cArrD2.insertNullItems(0,100);

            const int N4X1 = aX1.numBlocksInUse(), M4X1 = aX1.numBytesInUse();
            const int N4A1 = aA1.numBlocksInUse(), M4A1 = aA1.numBytesInUse();
            const int N4B1 = aB1.numBlocksInUse(), M4B1 = aB1.numBytesInUse();
            const int N4C1 = aC1.numBlocksInUse(), M4C1 = aC1.numBytesInUse();
            const int N4D1 = aD1.numBlocksInUse(), M4D1 = aD1.numBytesInUse();

            const int N4A2 = aA2.numBlocksInUse(), M4A2 = aA2.numBytesInUse();
            const int N4B2 = aB2.numBlocksInUse(), M4B2 = aB2.numBytesInUse();
            const int N4C2 = aC2.numBlocksInUse(), M4C2 = aC2.numBytesInUse();
            const int N4D2 = aD2.numBlocksInUse(), M4D2 = aD2.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N4X1); P_(N4A1); P_(N4B1); P_(N4C1); P(N4D1);
                T_; T_; P_(M4X1); P_(M4A1); P_(M4B1); P_(M4C1); P(M4D1);

                T_; T_; P_(N4A2); P_(N4B2); P_(N4C2); P(N4D2);
                T_; T_; P_(M4A2); P_(M4B2); P_(M4C2); P(M4D2);
            }

            if (verbose) cout << "\tRemove the elements again." << endl;

            cArrX1.removeItems(0,100);
            cArrA1.removeItems(0,100);
            cArrB1.removeItems(0,100);
            cArrC1.removeItems(0,100);
            cArrD1.removeItems(0,100);

            cArrA2.removeItems(0,100);
            cArrB2.removeItems(0,100);
            cArrC2.removeItems(0,100);
            cArrD2.removeItems(0,100);

            const int N5X1 = aX1.numBlocksInUse(), M5X1 = aX1.numBytesInUse();
            const int N5A1 = aA1.numBlocksInUse(), M5A1 = aA1.numBytesInUse();
            const int N5B1 = aB1.numBlocksInUse(), M5B1 = aB1.numBytesInUse();
            const int N5C1 = aC1.numBlocksInUse(), M5C1 = aC1.numBytesInUse();
            const int N5D1 = aD1.numBlocksInUse(), M5D1 = aD1.numBytesInUse();

            const int N5A2 = aA2.numBlocksInUse(), M5A2 = aA2.numBytesInUse();
            const int N5B2 = aB2.numBlocksInUse(), M5B2 = aB2.numBytesInUse();
            const int N5C2 = aC2.numBlocksInUse(), M5C2 = aC2.numBytesInUse();
            const int N5D2 = aD2.numBlocksInUse(), M5D2 = aD2.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N5X1); P_(N5A1); P_(N5B1); P_(N5C1); P(N5D1);
                T_; T_; P_(M5X1); P_(M5A1); P_(M5B1); P_(M5C1); P(M5D1);

                T_; T_;  P_(N5A2); P_(N5B2); P_(N5C2); P(N5D2);
                T_; T_;  P_(M5A2); P_(M5B2); P_(M5C2); P(M5D2);
            }

            if (verbose) cout << "\tEnsure BDEM_PASS_THROUGH's the default."
                              << endl;

            ASSERT(N5A1 == N5X1);         ASSERT(M5A1 == M5X1);
            ASSERT(N5B1 != N5X1);         ASSERT(M5B1 != M5X1);
            ASSERT(N5C1 != N5X1);         ASSERT(M5C1 != M5X1);
            ASSERT(N5D1 != N5X1);         ASSERT(M5D1 != M5X1);
        }
        if (verbose) cout << "\nThe catalog is passed through correctly"
                             "and defaults to the empty catalog" << endl;

        // Test defaults and empty catalog
        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);
            if (verbose) {
                cout << "\tTest empty catalog and defaults." << endl;
            }
            for (int i = 0; i < 4; ++i) {
                AggOption::AllocationStrategy mode =
                      (i == 0) ? AggOption::BDEM_PASS_THROUGH
                    : (i == 1) ? AggOption::BDEM_WRITE_MANY
                    : (i == 2) ? AggOption::BDEM_WRITE_ONCE
                    :            AggOption::BDEM_SUBORDINATE;

#if !defined(BSLS_PLATFORM__CMP_MSVC) || !defined(BDE_BUILD_TARGET_OPT)
                BEGIN_BSLMA_EXCEPTION_TEST {
#endif
                  // set up our test allocator based on the alloc strategy
                  bdema_SequentialAllocator seqAlloc(&testAllocator);
                  bslma_Allocator *alloc= NULL;

                  if (!(mode & AggOption::BDEM_NODESTRUCT_FLAG)) {
                      alloc = &testAllocator;
                  }
                  else {
                      alloc = &seqAlloc;
                  }

                  EType::Type *cat = NULL;
                  const int catS = 0;
                  const Desc **desc = NULL;

                  // PERFORM THE TEST
                  CAI array1(alloc);
                  CAI array2(mode, alloc);
                  CAI array3(cat,catS,desc,mode, alloc);

                  LOOP_ASSERT(i,0 == array1.numSelections());
                  LOOP_ASSERT(i,0 == array2.numSelections());
                  LOOP_ASSERT(i,0 == array3.numSelections());
#if !defined(BSLS_PLATFORM__CMP_MSVC) || !defined(BDE_BUILD_TARGET_OPT)
                } END_BSLMA_EXCEPTION_TEST
#endif
            }
        }
        // Test selection types for interesting catalog types
        // Verify the selection types and descriptors are installed
        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);
            if (verbose) {
                cout << "\tTest interesting catalog types to ensure "
                     << "selectionTypes and Descriptors are installed"
                     << endl;
            }

            for (int i = 0; i < 4; ++i) {
                AggOption::AllocationStrategy mode =
                      (i == 0) ? AggOption::BDEM_PASS_THROUGH
                    : (i == 1) ? AggOption::BDEM_WRITE_MANY
                    : (i == 2) ? AggOption::BDEM_WRITE_ONCE
                    :            AggOption::BDEM_SUBORDINATE;

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int   LINE = DATA[j].d_line;
                    const char *SPEC = DATA[j].d_catalogSpec;
                    const int   LEN  = bsl::strlen(SPEC);
                    Catalog cat;
                    populateCatalog(&cat,SPEC);

#if !defined(BSLS_PLATFORM__CMP_MSVC) || !defined(BDE_BUILD_TARGET_OPT)
                    BEGIN_BSLMA_EXCEPTION_TEST {
#endif
                      // set up our test allocator based on the alloc
                      // strategy
                      bdema_SequentialAllocator seqAlloc(&testAllocator);
                      bslma_Allocator *alloc= NULL;

                      if (!(mode & AggOption::BDEM_NODESTRUCT_FLAG)) {
                          alloc = &testAllocator;
                      }
                      else {
                          alloc = &seqAlloc;
                      }

                      if (veryVeryVerbose) {
                          cout << "Testing Selection Types and Descriptors "
                               << "for Catalog [" << j << "]:";
                          write(&cat);
                          cout << endl;
                      }
                      EType::Type *begin = 0 == cat.size() ? 0 : &cat[0];

                      CAI mX(begin, cat.size(), DESCRIPTORS, mode, alloc);
                      const CAI& X = mX;

                      // Note: Untested methods.  Need to get the choice
                      // header to inspect the set descriptors.
                      mX.insertNullItems(0,1);
                      const bdem_ChoiceHeader &CH = X.theItem(0);

                      LOOP2_ASSERT(i,j, cat.size() == CH.numSelections());
                      LOOP2_ASSERT(i,j,cat.size()  == X.numSelections());
                      for (int k = 0; k < cat.size(); ++k) {
                          const char   S    = SPEC[k];
                          EType::Type  TYPE = getElemType(S);
                          const Desc  *DESC = getDescriptor(S);

                          LOOP3_ASSERT(i, j, k, TYPE == X.selectionType(k));
                          LOOP3_ASSERT(i, j, k, DESC ==
                                       CH.selectionDescriptor(k));
                      }
#if !defined(BSLS_PLATFORM__CMP_MSVC) || !defined(BDE_BUILD_TARGET_OPT)
                    } END_BSLMA_EXCEPTION_TEST
#endif
                }
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING HELPER FUNCTIONS populateCatalog
        //   Ensure that the helper functions to create a catalog, or create
        //   an array of data from a specification string works correctly.
        //
        // Concerns:
        //   - Given valid input the generator function 'populateCatalog'
        //     creates the correct catalog definition.
        //   - test getValueA,getValueB,getValueN
        // Plan:
        //   1. Test an empty spec string
        //   2. Test all single element spec strings
        //   3. Test a long spec string containing at least 1
        //      of each element.
        //
        // Tactics:
        //   - Brute force implementation
        //
        // Testing:
        //   void populateCatalog(Catalog *, const char *);
        //
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << endl
                      <<"TESTING HELPER FUNCTION populateCatalog" << endl
                      <<"=======================================" << endl;
        }
        {
            if (verbose) bsl::cout << "TESTING 'populateCatalog'" << endl;
            {
                if (veryVerbose) {
                    bsl::cout << "Testing with empty string" << endl;
                }
                Catalog c;
                populateCatalog(&c,"");
                ASSERT(0 == c.size());
                if (veryVeryVerbose) {
                    cout << "Catalog: ";
                    write(&c);
                }
            }
            {
                if (veryVerbose) bsl::cout << "Testing all types" << endl;
                char spec [] = "%";
                for (int i = 0; i < SPEC_LEN; ++i) {
                    spec[0] = SPECIFICATIONS[i];
                    Catalog c;
                    populateCatalog(&c,spec);
                    ASSERT(1 == c.size());
                    ASSERT(cElemTypes[i] == c[0]);
                    if (veryVeryVerbose) {
                        cout << "Catalog: ";
                        write(&c);
                    }
                }
            }
            {
                if (veryVerbose) {
                    bsl::cout << "Testing a large invocation touching"
                                 "all tokens multiple times" << endl;
                }

                // create a specification string
                // and a matching vector of ETypes
                string spec;
                vector<EType::Type> indexVec;
                // cover all the types
                for (int i = 0; i < SPEC_LEN; ++i) {
                    spec += SPECIFICATIONS[i];
                    indexVec.push_back(cElemTypes[i]);
                }
                // add a bunch of random types for good measure
                for (int i = 0; i < 4 * SPEC_LEN; ++i) {
                    int r = rand() % SPEC_LEN;
                    spec += SPECIFICATIONS[r];
                    indexVec.push_back(cElemTypes[r]);
                }

                Catalog c;
                populateCatalog(&c,spec.data());
                ASSERT (c.size() == indexVec.size());

                for (int i = 0; i < c.size(); ++i) {
                    LOOP_ASSERT(i,indexVec[i] == c[i]);
                }
                if (veryVeryVerbose) {
                    cout << "Catalog: ";
                    write(&c);
                }
            }
        }

        if (verbose) bsl::cout << "\nTesting 'getValueA'." << bsl::endl;
        {
            ASSERT(A00   == *(char *) getValueA(SPECIFICATIONS[0]));
            ASSERT(A01   == *(short *) getValueA(SPECIFICATIONS[1]));
            ASSERT(A02   == *(int *) getValueA(SPECIFICATIONS[2]));
            ASSERT(A03   == *(Int64 *) getValueA(SPECIFICATIONS[3]));
            ASSERT(A04   == *(float *) getValueA(SPECIFICATIONS[4]));
            ASSERT(A05   == *(double *) getValueA(SPECIFICATIONS[5]));
            ASSERT(A06   == *(bsl::string *) getValueA(SPECIFICATIONS[6]));
            ASSERT(A07   == *(Datetime *) getValueA(SPECIFICATIONS[7]));
            ASSERT(A08   == *(Date *) getValueA(SPECIFICATIONS[8]));
            ASSERT(A09   == *(Time *) getValueA(SPECIFICATIONS[9]));
            ASSERT(A10   ==
                   *(bsl::vector<char> *) getValueA(SPECIFICATIONS[10]));
            ASSERT(A11   ==
                   *(bsl::vector<short> *) getValueA(SPECIFICATIONS[11]));
            ASSERT(A12   ==
                   *(bsl::vector<int> *) getValueA(SPECIFICATIONS[12]));
            ASSERT(A13   ==
                   *(bsl::vector<Int64> *) getValueA(SPECIFICATIONS[13]));
            ASSERT(A14   ==
                   *(bsl::vector<float> *) getValueA(SPECIFICATIONS[14]));
            ASSERT(A15   ==
                   *(bsl::vector<double> *) getValueA(SPECIFICATIONS[15]));
            ASSERT(A16   ==
                  *(bsl::vector<bsl::string> *) getValueA(SPECIFICATIONS[16]));
            ASSERT(A17   ==
                   *(bsl::vector<Datetime> *) getValueA(SPECIFICATIONS[17]));
            ASSERT(A18   ==
                   *(bsl::vector<Date> *) getValueA(SPECIFICATIONS[18]));
            ASSERT(A19   ==
                   *(bsl::vector<Time> *) getValueA(SPECIFICATIONS[19]));
            ASSERT(A22   ==
                   *(bool *) getValueA(SPECIFICATIONS[22]));
            ASSERT(A23   ==
                   *(DatetimeTz *) getValueA(SPECIFICATIONS[23]));
            ASSERT(A24   ==
                   *(DateTz *) getValueA(SPECIFICATIONS[24]));
            ASSERT(A25   ==
                   *(TimeTz *) getValueA(SPECIFICATIONS[25]));
            ASSERT(A26   ==
                   *(bsl::vector<bool> *) getValueA(SPECIFICATIONS[26]));
            ASSERT(A27   ==
                   *(bsl::vector<DatetimeTz> *) getValueA(SPECIFICATIONS[27]));
            ASSERT(A28   ==
                   *(bsl::vector<DateTz> *) getValueA(SPECIFICATIONS[28]));
            ASSERT(A29   ==
                   *(bsl::vector<TimeTz> *) getValueA(SPECIFICATIONS[29]));
        }

        if (verbose) bsl::cout << "\nTesting 'getValueB'." << bsl::endl;
        {
            ASSERT(B00   == *(char *) getValueB(SPECIFICATIONS[0]));
            ASSERT(B01   == *(short *) getValueB(SPECIFICATIONS[1]));
            ASSERT(B02   == *(int *) getValueB(SPECIFICATIONS[2]));
            ASSERT(B03   == *(Int64 *) getValueB(SPECIFICATIONS[3]));
            ASSERT(B04   == *(float *) getValueB(SPECIFICATIONS[4]));
            ASSERT(B05   == *(double *) getValueB(SPECIFICATIONS[5]));
            ASSERT(B06   == *(bsl::string *) getValueB(SPECIFICATIONS[6]));
            ASSERT(B07   == *(Datetime *) getValueB(SPECIFICATIONS[7]));
            ASSERT(B08   == *(Date *) getValueB(SPECIFICATIONS[8]));
            ASSERT(B09   == *(Time *) getValueB(SPECIFICATIONS[9]));
            ASSERT(B10   ==
                   *(bsl::vector<char> *) getValueB(SPECIFICATIONS[10]));
            ASSERT(B11   ==
                   *(bsl::vector<short> *) getValueB(SPECIFICATIONS[11]));
            ASSERT(B12   ==
                   *(bsl::vector<int> *) getValueB(SPECIFICATIONS[12]));
            ASSERT(B13   ==
                   *(bsl::vector<Int64> *) getValueB(SPECIFICATIONS[13]));
            ASSERT(B14   ==
                   *(bsl::vector<float> *) getValueB(SPECIFICATIONS[14]));
            ASSERT(B15   ==
                   *(bsl::vector<double> *) getValueB(SPECIFICATIONS[15]));
            ASSERT(B16   ==
                  *(bsl::vector<bsl::string> *) getValueB(SPECIFICATIONS[16]));
            ASSERT(B17   ==
                   *(bsl::vector<Datetime> *) getValueB(SPECIFICATIONS[17]));
            ASSERT(B18   ==
                   *(bsl::vector<Date> *) getValueB(SPECIFICATIONS[18]));
            ASSERT(B19   ==
                   *(bsl::vector<Time> *) getValueB(SPECIFICATIONS[19]));
            ASSERT(B22   == *(bool *) getValueB(SPECIFICATIONS[22]));
            ASSERT(B23   == *(DatetimeTz *) getValueB(SPECIFICATIONS[23]));
            ASSERT(B24   == *(DateTz *) getValueB(SPECIFICATIONS[24]));
            ASSERT(B25   == *(TimeTz *) getValueB(SPECIFICATIONS[25]));
            ASSERT(B26   ==
                   *(bsl::vector<bool> *) getValueB(SPECIFICATIONS[26]));
            ASSERT(B27   ==
                   *(bsl::vector<DatetimeTz> *) getValueB(SPECIFICATIONS[27]));
            ASSERT(B28   ==
                   *(bsl::vector<DateTz> *) getValueB(SPECIFICATIONS[28]));
            ASSERT(B29   ==
                   *(bsl::vector<TimeTz> *) getValueB(SPECIFICATIONS[29]));
        }

        if (verbose) bsl::cout << "\nTesting 'getValueN'." << bsl::endl;
        {
            ASSERT(N00       == *(char *) getValueN(SPECIFICATIONS[0]));
            ASSERT(N01       == *(short *) getValueN(SPECIFICATIONS[1]));
            ASSERT(N02       == *(int *) getValueN(SPECIFICATIONS[2]));
            ASSERT(N03       == *(Int64 *) getValueN(SPECIFICATIONS[3]));
            ASSERT(N04       == *(float *) getValueN(SPECIFICATIONS[4]));
            ASSERT(N05       == *(double *) getValueN(SPECIFICATIONS[5]));
            ASSERT(N06       == *(bsl::string *) getValueN(SPECIFICATIONS[6]));
            ASSERT(N07       == *(Datetime *) getValueN(SPECIFICATIONS[7]));
            ASSERT(N08       == *(Date *) getValueN(SPECIFICATIONS[8]));
            ASSERT(N09       == *(Time *) getValueN(SPECIFICATIONS[9]));
            ASSERT(N10       ==
                   *(bsl::vector<char> *) getValueN(SPECIFICATIONS[10]));
            ASSERT(N11       ==
                   *(bsl::vector<short> *) getValueN(SPECIFICATIONS[11]));
            ASSERT(N12       ==
                   *(bsl::vector<int> *) getValueN(SPECIFICATIONS[12]));
            ASSERT(N13       ==
                   *(bsl::vector<Int64> *) getValueN(SPECIFICATIONS[13]));
            ASSERT(N14       ==
                   *(bsl::vector<float> *) getValueN(SPECIFICATIONS[14]));
            ASSERT(N15       ==
                   *(bsl::vector<double> *) getValueN(SPECIFICATIONS[15]));
            ASSERT(N16       ==
                  *(bsl::vector<bsl::string> *) getValueN(SPECIFICATIONS[16]));
            ASSERT(N17       ==
                   *(bsl::vector<Datetime> *) getValueN(SPECIFICATIONS[17]));
            ASSERT(N18       ==
                   *(bsl::vector<Date> *) getValueN(SPECIFICATIONS[18]));
            ASSERT(N19       ==
                   *(bsl::vector<Time> *) getValueN(SPECIFICATIONS[19]));
            ASSERT(N22       == *(bool *) getValueN(SPECIFICATIONS[22]));
            ASSERT(N23       == *(DatetimeTz *) getValueN(SPECIFICATIONS[23]));
            ASSERT(N24       == *(DateTz *) getValueN(SPECIFICATIONS[24]));
            ASSERT(N25       == *(TimeTz *) getValueN(SPECIFICATIONS[25]));
            ASSERT(N26       ==
                   *(bsl::vector<bool> *) getValueN(SPECIFICATIONS[26]));
            ASSERT(N27       ==
                   *(bsl::vector<DatetimeTz> *) getValueN(SPECIFICATIONS[27]));
            ASSERT(N28       ==
                   *(bsl::vector<DateTz> *) getValueN(SPECIFICATIONS[28]));
            ASSERT(N29       ==
                   *(bsl::vector<TimeTz> *) getValueN(SPECIFICATIONS[29]));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   After each of these steps, check that all attributes of the
        //   bdem_ChoiceArrayImp object are as expected, check that memory is
        //   being used as expected, and (if in verbose mode), print the value
        //   of the created or modified object.
        //   1. Create an object, mX, using the default constructor.
        //   2. Modify mX so that it holds an int.
        //   3. Create an object, mY, that holds an unset string.
        //   3. Modify the value in mY so that it holds "hello
        //      world".
        //   4. Copy-construct mZ from mY.
        //   5. Assign mY = mX.
        //   6. Construct mA holding a double.
        //   7. Stream each choice array into a test stream.
        //   8. Read all choice arrays from the test stream into a vector of
        //      choice array imps.
        //   9. Iterate over a set of "interesting" specs
        //      o iterate over the spec
        //          a. insert an unset item, then set the new element from the
        //             specs type at the index
        //          b. Access the item and use the returned ChoiceHeader
        //             to set another unset item
        //          c. Insert unset elements to the beginning of the array
        //             insert unset items to the end of the array
        //      o reset the array
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << endl
                               << "BREATHING TEST" << endl
                               << "==============" << endl;

        {
            if (verbose) {
                bsl::cout << "\tDefault construct CAI mX" << bsl::endl;
            }
            bslma_TestAllocator alloc(veryVeryVerbose);
            CAI mX(&alloc); const CAI& X = mX;
            ASSERT(0 == X.numSelections());
            ASSERT(0 == X.length());

            if (veryVerbose) {
                bsl::cout << "\tX:" << bsl::endl;
                X.print(bsl::cout, 1, 4);
            }

            const int INT_VAL = 100;
            if (verbose) {
                bsl::cout << "\tModify mX to hold int with Value: "
                          << INT_VAL << bsl::endl;
            }

            // create mX with INT as the only selection type
            EType::Type catalog1[] = { EType::BDEM_INT };
            mX.reset(catalog1, 1, DESCRIPTORS);
            ASSERT(1 == X.numSelections());
            ASSERT(0 == X.length());
            ASSERT(EType::BDEM_INT == X.selectionType(0));

            // add an unset element to mX
            mX.insertNullItems(0, 1);
            ASSERT(1  == X.numSelections());
            ASSERT(1  == X.length());
            ASSERT(-1 == X.selector(0));
            ASSERT(EType::BDEM_INT == X.selectionType(0));

            // select INT for the first element of mX
            mX.makeSelection(0, 0).theModifiableInt() = INT_VAL;
            ASSERT(1 == X.numSelections());
            ASSERT(1 == X.length());
            ASSERT(0 == X.selector(0));
            ASSERT(EType::BDEM_INT == X.selectionType(0));
            ASSERT(INT_VAL == *(const int *) X.theItem(0).selectionPointer());
            if (veryVerbose) {
                bsl::cout << "\tX:" << bsl::endl;
                X.print(bsl::cout, 1, 4);
                bsl::cout << "\tConstruct CAI mY holding a string"
                          << bsl::endl;
                bsl::cout << "\tX:" << bsl::endl;
                X.print(bsl::cout, 1, 4);
                bsl::cout << "\tConstruct CAI mY holding a string"
                          << bsl::endl;
            }

            // create a second object mY
            EType::Type catalog2[] = { EType::BDEM_STRING };
            CAI mY(catalog2,
                   1,
                   DESCRIPTORS,
                   PASSTH,
                   &alloc);
            const CAI& Y = mY;

            // add an unset choice element to mY
            mY.insertNullItems(0, 1);
            ASSERT(1  == Y.numSelections());
            ASSERT(1  == Y.length());
            ASSERT(-1 == Y.selector(0));
            ASSERT(EType::BDEM_STRING == Y.selectionType(0));
            ASSERT(X != Y);
            ASSERT(!(X == Y));
            if (veryVerbose) {
                bsl::cout << "\tY:" << bsl::endl;
                Y.print(bsl::cout, 1, 4);
                bsl::cout << "\tModify mY to have value "
                          << "\"Hello World\"" << bsl::endl;
            }

            const bsl::string STR_VAL = "Hello World";
            mY.makeSelection(0, 0).theModifiableString() = STR_VAL;
            ASSERT(1 == Y.numSelections());
            ASSERT(1 == Y.length());
            ASSERT(0 == Y.selector(0));
            ASSERT(EType::BDEM_STRING == Y.selectionType(0));
            ASSERT(STR_VAL ==
                   *(const bsl::string *) Y.theItem(0).selectionPointer());
            if (veryVerbose) {
                bsl::cout << "\tY:" << bsl::endl;
                Y.print(bsl::cout, 1, 4);
                bsl::cout << "\tCopy Construct mZ from mY" << bsl::endl;
            }
            CAI mZ(mY, &alloc); const CAI& Z = mZ;
            ASSERT(1 == Z.numSelections());
            ASSERT(1 == Z.length());
            ASSERT(0 == Z.selector(0));
            ASSERT(EType::BDEM_STRING == Z.selectionType(0));
            ASSERT(STR_VAL ==
                       *(const bsl::string *) Z.theItem(0).selectionPointer());
            ASSERT(1 == Y.numSelections());
            ASSERT(1 == Y.length());
            ASSERT(0 == Y.selector(0));
            ASSERT(EType::BDEM_STRING == Y.selectionType(0));
            ASSERT(STR_VAL ==
                   *(const bsl::string *) Y.theItem(0).selectionPointer());
            ASSERT(X != Y);
            ASSERT(!(X == Y));
            ASSERT(Z == Y);
            ASSERT(!(Z != Y));
            if (veryVerbose) {
                bsl::cout << "\tX:" << bsl::endl;
                X.print(bsl::cout, 1, 4);
                bsl::cout << "\tY:" << bsl::endl;
                Y.print(bsl::cout, 1, 4);
                bsl::cout << "\tZ:" << bsl::endl;
                Z.print(bsl::cout, 1, 4);
                bsl::cout << "\tAssign mX to mY" << bsl::endl;
            }
            mY = mX;
            ASSERT(1 == X.numSelections());
            ASSERT(1 == X.length());
            ASSERT(0 == X.selector(0));
            ASSERT(EType::BDEM_INT == X.selectionType(0));
            ASSERT(INT_VAL == *(const int *) X.theItem(0).selectionPointer());
            ASSERT(1 == Y.numSelections());
            ASSERT(1 == Y.length());
            ASSERT(0 == Y.selector(0));
            ASSERT(EType::BDEM_INT == Y.selectionType(0));
            ASSERT(INT_VAL == *(const int *) Y.theItem(0).selectionPointer());
            ASSERT(X == Y);
            ASSERT(!(X != Y));
            ASSERT(Z != Y);
            ASSERT(!(Z == Y));
            if (veryVerbose) {
                bsl::cout << "\tX:" << bsl::endl;
                X.print(bsl::cout, 1, 4);
                bsl::cout << "\tY:" << bsl::endl;
                Y.print(bsl::cout, 1, 4);
                bsl::cout << "\tZ:" << bsl::endl;
                Z.print(bsl::cout, 1, 4);
                bsl::cout << "\tConstruct CAI mA holding a double"
                          << bsl::endl;
            }
            EType::Type catalog3[] = { EType::BDEM_DOUBLE };
            bslma_TestAllocator ta(verbose);
            CAI mA(catalog3,
                   1,
                   DESCRIPTORS,
                   PASSTH,
                   &ta);
            const CAI& A = mA;
            mA.insertNullItems(0, 1);
            const double DBL_VAL = 123.5;
            mA.makeSelection(0, 0).theModifiableDouble() = DBL_VAL;
            ASSERT(1 == A.numSelections());
            ASSERT(1 == A.length());
            ASSERT(0 == A.selector(0));
            ASSERT(EType::BDEM_DOUBLE == A.selectionType(0));
            ASSERT(DBL_VAL ==
                            *(const double *) A.theItem(0).selectionPointer());
            if (veryVerbose) {
                bsl::cout << "\tA:" << bsl::endl;
                A.print(bsl::cout, 1, 4);
                bsl::cout << "\tStream out each choice array" << bsl::endl;
            }
            bdex_TestOutStream os(&alloc);
            X.bdexStreamOut(os,
                            1,
                       streamOutAttrLookup<bdex_TestOutStream>::lookupTable());
            Y.bdexStreamOut(os,
                            1,
                       streamOutAttrLookup<bdex_TestOutStream>::lookupTable());
            Z.bdexStreamOut(os,
                            1,
                       streamOutAttrLookup<bdex_TestOutStream>::lookupTable());
            A.bdexStreamOut(os,
                            1,
                       streamOutAttrLookup<bdex_TestOutStream>::lookupTable());
            ASSERT(os);

            bdex_TestInStream is(os.data(), os.length());
            bslma_TestAllocator va(verbose);
            bsl::vector<CAI> objVec(4, &va);
            objVec[0].bdexStreamIn(
                         is,
                         1,
                         streamInAttrLookup<bdex_TestInStream>::lookupTable(),
                         DESCRIPTORS);
            objVec[1].bdexStreamIn(
                         is,
                         1,
                         streamInAttrLookup<bdex_TestInStream>::lookupTable(),
                         DESCRIPTORS);
            objVec[2].bdexStreamIn(
                         is,
                         1,
                         streamInAttrLookup<bdex_TestInStream>::lookupTable(),
                         DESCRIPTORS);
            objVec[3].bdexStreamIn(
                         is,
                         1,
                         streamInAttrLookup<bdex_TestInStream>::lookupTable(),
                         DESCRIPTORS);
            LOOP2_ASSERT(X, objVec[0], X == objVec[0]);
            ASSERT(!(X != objVec[0]));
            ASSERT(Y == objVec[1]);
            ASSERT(!(Y != objVec[1]));
            ASSERT(Z == objVec[2]);
            ASSERT(!(Z != objVec[2]));
            ASSERT(A == objVec[3]);
            ASSERT(!(A != objVec[3]));
            if (veryVerbose) {
                bsl::cout << "\tX:" << bsl::endl;
                X.print(bsl::cout, 1, 4);
                bsl::cout << "\tY:" << bsl::endl;
                Y.print(bsl::cout, 1, 4);
                bsl::cout << "\tZ:" << bsl::endl;
                Z.print(bsl::cout, 1, 4);
                bsl::cout << "\tA:" << bsl::endl;
                A.print(bsl::cout, 1, 4);
            }
        }

        if (verbose) bsl::cout << "\n\tTesting default construction"
                               << "\n\t============================"
                               << bsl::endl;
        {
            bslma_TestAllocator t(veryVeryVerbose);

            CAI mX(&t); const CAI& X = mX;
            ASSERT(0 == X.numSelections());
            ASSERT(0 == X.length());
        }

        if (verbose) bsl::cout << "\n\tTesting different specs"
                                  "\n\t======================="
                               << bsl::endl;

        {
            for (int j = NUM_DATA - 1; j < NUM_DATA; ++j) {
                const int   LINE = DATA[j].d_line;
                const char *SPEC = DATA[j].d_catalogSpec;
                const int   LEN  = bsl::strlen(SPEC);

                bslma_TestAllocator testAllocator(veryVeryVerbose);
                bslma_TestAllocator &alloc=testAllocator;
                Catalog catalog;
                populateCatalog(&catalog, SPEC);

              BEGIN_BSLMA_EXCEPTION_TEST { // removed for performance

                EType::Type *begin = 0 == catalog.size() ? 0 : &catalog[0];

                CAI mX(begin, catalog.size(), DESCRIPTORS, PASSTH, &alloc);
                const CAI& X = mX;

                LOOP_ASSERT(j, catalog.size() == X.numSelections());
                LOOP_ASSERT(j, 0              == X.length());

                for (int k = 0; k < LEN; ++k) {

                    const char   S    = SPEC[k];
                    const Desc  *DESC = getDescriptor(S);
                    EType::Type  TYPE = getElemType(S);

                    const void *VAL_A  = getValueA(S);
                    const void *VAL_B  = getValueB(S);
                    const void *VAL_N  = getValueN(S);

                    if (veryVerbose) { P_(j) P_(k) P_(SPEC) P(S) }

                    // Insert unset items
                    mX.insertNullItems(mX.length(),1);
                    LOOP_ASSERT(S, catalog.size() == X.numSelections());
                    LOOP_ASSERT(S, k + 1          == X.length());
                    LOOP_ASSERT(S, TYPE           == X.selectionType(k));
                    // set a value
                    mX.theModifiableItem(k).makeSelection(k,VAL_A);
                    const bdem_ChoiceHeader mI = mX.theModifiableItem(k);
                    LOOP_ASSERT(S, catalog.size() == X.numSelections());
                    LOOP_ASSERT(S, k + 1          == X.length());
                    LOOP_ASSERT(S, TYPE           == X.selectionType(k));

                    LOOP_ASSERT(S, compare(VAL_A,
                                mX.theModifiableItem(k).selectionPointer(),S));
                    LOOP_ASSERT(S, compare(VAL_A,
                                           X.theItem(k).selectionPointer(),S));
                    LOOP_ASSERT(S, !compare(VAL_B,
                                mX.theModifiableItem(k).selectionPointer(),S));
                    LOOP_ASSERT(S, !compare(VAL_B,
                                           X.theItem(k).selectionPointer(),S));

                    // append items
                    mX.insertItem(mX.length(),mI);
                    LOOP_ASSERT(S, catalog.size() == X.numSelections());
                    LOOP_ASSERT(S, k + 2          == X.length());
                    LOOP_ASSERT(S, TYPE           == X.selectionType(k));

                    LOOP_ASSERT(S, compare(VAL_A,
                                mX.theModifiableItem(k).selectionPointer(),S));
                    LOOP_ASSERT(S, compare(VAL_A,
                                           X.theItem(k).selectionPointer(),S));
                    LOOP_ASSERT(S, compare(VAL_A,
                              mX.theModifiableItem(k+1).selectionPointer(),S));
                    LOOP_ASSERT(S, compare(VAL_A,
                                         X.theItem(k+1).selectionPointer(),S));

                    // set their value
                    mX.theModifiableItem(k+1).makeSelection(k,VAL_B);
                    bdem_ChoiceHeader mJ = mX.theModifiableItem(k + 1);
                    LOOP_ASSERT(S, catalog.size() == X.numSelections());
                    LOOP_ASSERT(S, k + 2          == X.length());
                    LOOP_ASSERT(S, TYPE           == X.selectionType(k));

                    LOOP_ASSERT(S, compare(VAL_A,
                                mX.theModifiableItem(k).selectionPointer(),S));
                    LOOP_ASSERT(S, compare(VAL_A,
                                           X.theItem(k).selectionPointer(),S));
                    LOOP_ASSERT(S, compare(VAL_B,
                              mX.theModifiableItem(k+1).selectionPointer(),S));
                    LOOP_ASSERT(S, compare(VAL_B,
                                         X.theItem(k+1).selectionPointer(),S));
                    {
                        // insert at index 0 and then remove
                        const int NUM_ITEMS = 2;
                        mX.insertNullItems(0, NUM_ITEMS);
                        LOOP_ASSERT(S, catalog.size() == X.numSelections());
                        LOOP_ASSERT(S,k + 4         == X.length());
                        LOOP_ASSERT(S,TYPE          == X.selectionType(k));

                        LOOP_ASSERT(S,
                                    -1 == mX.theModifiableItem(0).selector());
                        LOOP_ASSERT(S, -1 ==  X.theItem(0).selector());
                        LOOP_ASSERT(S,
                                    -1 == mX.theModifiableItem(1).selector());
                        LOOP_ASSERT(S, -1 ==  X.theItem(1).selector());

                        LOOP_ASSERT(S, compare(VAL_A,
                              mX.theModifiableItem(k+2).selectionPointer(),S));
                        LOOP_ASSERT(S, compare(VAL_A,
                                         X.theItem(k+2).selectionPointer(),S));
                        LOOP_ASSERT(S, compare(VAL_B,
                              mX.theModifiableItem(k+3).selectionPointer(),S));
                        LOOP_ASSERT(S, compare(VAL_B,
                                         X.theItem(k+3).selectionPointer(),S));

                        mX.removeItems(0, NUM_ITEMS);

                        LOOP_ASSERT(S, catalog.size()== X.numSelections());
                        LOOP_ASSERT(S, k + 2         == X.length());
                        LOOP_ASSERT(S, TYPE          == X.selectionType(k));

                        LOOP_ASSERT(S, compare(VAL_A,
                                mX.theModifiableItem(k).selectionPointer(),S));
                        LOOP_ASSERT(S, compare(VAL_A,
                                           X.theItem(k).selectionPointer(),S));
                        LOOP_ASSERT(S, compare(VAL_B,
                              mX.theModifiableItem(k+1).selectionPointer(),S));
                        LOOP_ASSERT(S, compare(VAL_B,
                                         X.theItem(k+1).selectionPointer(),S));
                    }

                    {
                        // insert at the end of the array and then remove
                        const int NUM_ITEMS = 2;
                        mX.insertNullItems(X.length(), NUM_ITEMS);
                        LOOP_ASSERT(S, catalog.size() == X.numSelections());
                        LOOP_ASSERT(S, k + 4          == X.length());
                        LOOP_ASSERT(S, TYPE           == X.selectionType(k));

                        LOOP_ASSERT(S, compare(VAL_A,
                                mX.theModifiableItem(k).selectionPointer(),S));
                        LOOP_ASSERT(S, compare(VAL_A,
                                           X.theItem(k).selectionPointer(),S));
                        LOOP_ASSERT(S, compare(VAL_B,
                              mX.theModifiableItem(k+1).selectionPointer(),S));
                        LOOP_ASSERT(S, compare(VAL_B,
                                         X.theItem(k+1).selectionPointer(),S));

                        LOOP_ASSERT(S,
                                   -1 == mX.theModifiableItem(k+2).selector());
                        LOOP_ASSERT(S, -1 ==  X.theItem(k+2).selector());
                        LOOP_ASSERT(S,
                                   -1 == mX.theModifiableItem(k+3).selector());
                        LOOP_ASSERT(S, -1 ==  X.theItem(k+3).selector());

                        mX.removeItems(X.length() - 2, NUM_ITEMS);
                        LOOP_ASSERT(S, catalog.size() == X.numSelections());
                        LOOP_ASSERT(S, k + 2          == X.length());
                        LOOP_ASSERT(S, TYPE           == X.selectionType(k));

                        LOOP_ASSERT(S, compare(VAL_A,
                                mX.theModifiableItem(k).selectionPointer(),S));
                        LOOP_ASSERT(S, compare(VAL_A,
                                           X.theItem(k).selectionPointer(),S));
                        LOOP_ASSERT(S, compare(VAL_B,
                              mX.theModifiableItem(k+1).selectionPointer(),S));
                        LOOP_ASSERT(S, compare(VAL_B,
                                         X.theItem(k+1).selectionPointer(),S));
                    }

                    mX.removeItem(k + 1);
                    LOOP_ASSERT(S, catalog.size() == X.numSelections());
                    LOOP_ASSERT(S, k + 1          == X.length());
                    LOOP_ASSERT(S, TYPE           == X.selectionType(k));
                    LOOP_ASSERT(S, compare(VAL_A,
                                mX.theModifiableItem(k).selectionPointer(),S));
                    LOOP_ASSERT(S, compare(VAL_A,
                                           X.theItem(k).selectionPointer(),S));

                    if (verbose) {
                        bsl::cout << "bdex Streaming" << bsl::endl;
                    }
                    {
                        bdex_TestOutStream os;
                        streamInAttrLookup<In>   inLookup;
                        streamOutAttrLookup<Out> outLookup;

                        X.bdexStreamOut(os, 1, outLookup.lookupTable());

                        bdex_TestInStream is(os.data(), os.length());
                        bslma_TestAllocator ta(veryVeryVerbose);
                        CAI mY(PASSTH, &ta);
                        const CAI& Y = mY;

                        LOOP_ASSERT(S,   X != Y);
                        LOOP_ASSERT(S, !(X == Y));

                        mY.bdexStreamIn(is, 1, inLookup.lookupTable(),
                                        DESCRIPTORS);

                        LOOP_ASSERT(S,   X == Y);
                        LOOP_ASSERT(S, !(X != Y));
                    }

                    if (verbose) {
                        bsl::cout << "Assignment operator" << bsl::endl;
                    }
                    {
                        bslma_TestAllocator ta(veryVeryVerbose);
                        CAI mY(PASSTH, &ta); const CAI& Y = mY;

                        LOOP_ASSERT(S,   X != Y);
                        LOOP_ASSERT(S, !(X == Y));

                        mY = X;

                        LOOP_ASSERT(S,   X == Y);
                        LOOP_ASSERT(S, !(X != Y));
                    }

                    if (verbose) {
                        bsl::cout << "Copy constructor" << bsl::endl;
                    }
                    {
                        bslma_TestAllocator ta(verbose);
                        CAI mY(X, PASSTH, &ta); const CAI& Y = mY;

                        LOOP_ASSERT(S,   X == Y);
                        LOOP_ASSERT(S, !(X != Y));
                    }
                    {
                        if (veryVerbose) { bsl::cout << X << bsl::endl; }
                    }
                }

                mX.removeItems(0,mX.length());
                LOOP_ASSERT(j, catalog.size() == X.numSelections());
                LOOP_ASSERT(j, 0 == X.length());

                mX.reset();
                LOOP_ASSERT(j, 0 == X.numSelections());
                LOOP_ASSERT(j, 0 == X.length());

              } END_BSLMA_EXCEPTION_TEST
            }
        }

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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
