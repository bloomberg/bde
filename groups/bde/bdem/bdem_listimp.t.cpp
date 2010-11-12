// bdem_listimp.t.cpp                                                 -*-C++-*-

#include <bdem_listimp.h>

#include <bdem_elemtype.h>
#include <bdem_functiontemplates.h>
#include <bdem_properties.h>

#include <bdetu_unset.h>
#include <bdex_testinstream.h>                // for testing only
#include <bdex_testoutstream.h>               // for testing only
#include <bdex_testinstreamexception.h>       // for testing only

#include <bslma_testallocator.h>              // for testing only
#include <bslma_testallocatorexception.h>     // for testing only
#include <bslma_bufferallocator.h>              // for testing only
#include <bslmf_isconvertible.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bdetu_unset.h>
#include <bdex_testinstream.h>                // for testing only
#include <bdex_testoutstream.h>               // for testing only
#include <bdex_testinstreamexception.h>       // for testing only

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_c_ctype.h>                            // isspace()

#include <bsl_iostream.h>
#include <bsl_strstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The majority of methods in 'bdem_ListImp' perform simple delegation by
// calling (usually) a single method via the (private) 'bdem_RowData' data
// member.  For these methods there is no need for full-blown tests; rather, it
// is sufficient to verify that each method is "hooked-up" properly.
//
// A number of methods do require non-trivial testing; these include
// 'insertString', 'insertList', some of the streaming methods, and the
// constructors.  The latter are trivial in their implementation, but they do
// require non-trivial testing to confirm that the
// 'bdem_AggregateOption::AllocationStrategy' is correctly passed through.
//
// Testing proceeds by exercising the simple delegation methods first.
// Although we could do this within one test case, we split it into three so as
// to emphasize the hierarchy of the methods involved.  I.e., we first test the
// primary manipulator and accessors, followed by the equality operators,
// followed by the rest of the delegation methods.  After this we move on to
// the non-trivial test cases.
//
// Note that in the majority of the tests we primarily use two types (int and
// bsl::string) to demonstrate that type-based arguments are handled correctly.
// For these types we also rely on a small number of file-scope const variables
// to use in "set/test" contexts.
//
// There is no requirement for a 'gg' function.  One trivial helper function
// is supplied.
//
// We use the following abbreviations: (for documentation purposes only)
//  'D'       for 'const bdem_Descriptor'
//  'ET'      for 'const bdem_ElemType::Type'
//  'AS'      for 'bdem_AggregateOption::AllocationStrategy'
//  'SO'      for 'bdem_DescriptorStreamOut<STREAM>'
//  'SI'      for 'bdem_DescriptorStreamIn<STREAM>'
//-----------------------------------------------------------------------------
// [ 6] bdem_ListImp(AS, bslma_Allocator);
// [ 6] bdem_ListImp(ET, int, D *const [], AS, bslma_Allocator);
// [ 6] bdem_ListImp(const bdem_RowData&, AS, bslma_Allocator);
// [ 6] bdem_ListImp(const bdem_ListImp&, AS, bslma_Allocator);
// [ 2] void *insertElement(int, const void *, D *);
// [ 2] int length() const;
// [ 2] ET elemType(int) const;
// [ 2] const bdem_RowData& row() const;
// [ 2] bdem_RowData& row();
// [ 2] ~bdem_ListImp();
// [ 3] bool operator==(const bdem_ListImp&, const bdem_ListImp&);
// [ 3] bool operator!=(const bdem_ListImp&, const bdem_ListImp&);
// [ 4] void *insertElementRaw(int, const bdem_Descriptor *);
// [ 4] void insertElements(int, bdem_RowData&, int, int);
// [ 4] void *insertNullElement(int, const bdem_Descriptor *);
// [ 4] void insertNullElements(int, ET typ[], int num, D *const al[]);
// [ 4] void swap(int, int);
// [ 4] void removeElement(int);
// [ 4] void removeElements(int, int);
// [ 4] void removeAll();
// [ 4] bsl::ostream& print(bsl::ostream&, int, int) const;
// [ 4] bdem_ListImp& operator=(const bdem_RowData&);
// [ 4] bdem_ListImp& operator=(const bdem_ListImp&);
// [ 4] void replaceElement(int,const bdem_ConstElemRef&);
// [ 4] void resetElement(int, D *);
// [ 4] void reset(ET [], , D *const);
// [ 5] bsl::string& insertString(int, const char *);
// [ 5] bdem_ListImp& insertList(int, const bdem_RowData&);
// [ 7] bdexStreamOutImp(STREAM&, const SO *);
// [ 7] streamOutList(void *, STREAM&, int const SO *);
// [ 7] bdexStreamInImp(STREAM&, const SI *, const D *const[]);
// [ 7] bdexStreamInImp(STREAM&, int, const SI *, const D *const[]);
// [ 7] streamInList(void *, STREAM&,int , const SI *, const D *const[]);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] BOOTSTRAP: bdem_ListImp(AS, bslma_Allocator *);
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
enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

typedef bsls_Types::Int64  Int64;

typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

typedef bdem_ListImp       Obj;

typedef bdem_ElemType      ET;
typedef bdem_RowData       RowData;

static
const   bdem_AggregateOption::AllocationStrategy PT =
        bdem_AggregateOption::BDEM_PASS_THROUGH;

static
const   bdem_AggregateOption::AllocationStrategy WM =
        bdem_AggregateOption::BDEM_WRITE_MANY;

static
const bdem_Descriptor *EDA[] =
{
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

static
const ET::Type ETA[] =
{
    ET::BDEM_CHAR,
    ET::BDEM_SHORT,
    ET::BDEM_INT,
    ET::BDEM_INT64,
    ET::BDEM_FLOAT,
    ET::BDEM_DOUBLE,
    ET::BDEM_STRING,
    ET::BDEM_DATETIME,
    ET::BDEM_DATE,
    ET::BDEM_TIME,
    ET::BDEM_CHAR_ARRAY,
    ET::BDEM_SHORT_ARRAY,
    ET::BDEM_INT_ARRAY,
    ET::BDEM_INT64_ARRAY,
    ET::BDEM_FLOAT_ARRAY,
    ET::BDEM_DOUBLE_ARRAY,
    ET::BDEM_STRING_ARRAY,
    ET::BDEM_DATETIME_ARRAY,
    ET::BDEM_DATE_ARRAY,
    ET::BDEM_TIME_ARRAY
};

static
bdem_DescriptorStreamIn<In> STREAMIN_TABLE[] = {
    &bdem_FunctionTemplates::streamInFundamental<char,In>,
    &bdem_FunctionTemplates::streamInFundamental<short,In>,
    &bdem_FunctionTemplates::streamInFundamental<int,In>,
    &bdem_FunctionTemplates::
    streamInFundamental<bsls_Types::Int64,In>,
    &bdem_FunctionTemplates::streamInFundamental<float,In>,
    &bdem_FunctionTemplates::streamInFundamental<double,In>,
    &bdem_FunctionTemplates::streamInFundamental<bsl::string,In>,
    &bdem_FunctionTemplates::streamIn<bdet_Datetime,In>,
    &bdem_FunctionTemplates::streamIn<bdet_Date,In>,
    &bdem_FunctionTemplates::streamIn<bdet_Time,In>,
    &bdem_FunctionTemplates::streamInArray<char,In>,
    &bdem_FunctionTemplates::streamInArray<short,In>,
    &bdem_FunctionTemplates::streamInArray<int,In>,
    &bdem_FunctionTemplates::streamInArray<bsls_Types::Int64,In>,
    &bdem_FunctionTemplates::streamInArray<float,In>,
    &bdem_FunctionTemplates::streamInArray<double,In>,
    &bdem_FunctionTemplates::streamInArray<bsl::string,In>,
    &bdem_FunctionTemplates::streamInArray<bdet_Datetime,In>,
    &bdem_FunctionTemplates::streamInArray<bdet_Date,In>,
    &bdem_FunctionTemplates::streamInArray<bdet_Time, In>
};

static
bdem_DescriptorStreamOut<Out> STREAMOUT_TABLE[] = {
    &bdem_FunctionTemplates::streamOutFundamental<char,Out>,
    &bdem_FunctionTemplates::streamOutFundamental<short,Out>,
    &bdem_FunctionTemplates::streamOutFundamental<int,Out>,
    &bdem_FunctionTemplates::
    streamOutFundamental<bsls_Types::Int64,Out>,
    &bdem_FunctionTemplates::streamOutFundamental<float,Out>,
    &bdem_FunctionTemplates::streamOutFundamental<double,Out>,
    &bdem_FunctionTemplates::streamOutFundamental<bsl::string,Out>,
    &bdem_FunctionTemplates::streamOut<bdet_Datetime,Out>,
    &bdem_FunctionTemplates::streamOut<bdet_Date,Out>,
    &bdem_FunctionTemplates::streamOut<bdet_Time,Out>,
    &bdem_FunctionTemplates::streamOutArray<char,Out>,
    &bdem_FunctionTemplates::streamOutArray<short,Out>,
    &bdem_FunctionTemplates::streamOutArray<int,Out>,
    &bdem_FunctionTemplates::streamOutArray<bsls_Types::Int64,Out>,
    &bdem_FunctionTemplates::streamOutArray<float,Out>,
    &bdem_FunctionTemplates::streamOutArray<double,Out>,
    &bdem_FunctionTemplates::streamOutArray<bsl::string,Out>,
    &bdem_FunctionTemplates::streamOutArray<bdet_Datetime,Out>,
    &bdem_FunctionTemplates::streamOutArray<bdet_Date,Out>,
    &bdem_FunctionTemplates::streamOutArray<bdet_Time, Out>
};

static const char        CHAR_VALUE1   = '3';
static const char        CHAR_VALUE2   = '4';
static const short       SHORT_VALUE1  = 3;
static const short       SHORT_VALUE2  = 4;
static const int         INT_VALUE1    = 3;
static const int         INT_VALUE2    = 4;
static const bsl::string STRING_VALUE1 = "TEST";
static const bsl::string STRING_VALUE2 = "test";

static const int         UNSET_INT    = bdetu_Unset<int>::unsetValue();
static const bsl::string UNSET_STRING = bdetu_Unset<bsl::string>::unsetValue();

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

bool isEqual(const ET::Type elemType,
             const void    *value1,
             const void    *value2)
    // Determine if two 'bdem' values pointed to by the 'void *' pointers
    // 'val1' and 'val2' are equal; the appropriate casts are made based on
    // the value of 'elemType'.
{
    bool rv = false;
    switch (elemType) {
      case ET::BDEM_INT: {
        rv = *(int *)value1 == *(int *)value2;
      } break;
      case ET::BDEM_STRING: {
        rv = *(bsl::string *)value1 == *(bsl::string *)value2;
      } break;
      case ET::BDEM_CHAR:
      case ET::BDEM_SHORT:
      case ET::BDEM_INT64:
      case ET::BDEM_FLOAT:
      case ET::BDEM_DOUBLE:
      case ET::BDEM_DATETIME:
      case ET::BDEM_DATE:
      case ET::BDEM_TIME:
      case ET::BDEM_CHAR_ARRAY:
      case ET::BDEM_SHORT_ARRAY:
      case ET::BDEM_INT_ARRAY:
      case ET::BDEM_INT64_ARRAY:
      case ET::BDEM_FLOAT_ARRAY:
      case ET::BDEM_DOUBLE_ARRAY:
      case ET::BDEM_STRING_ARRAY:
      case ET::BDEM_DATETIME_ARRAY:
      case ET::BDEM_DATE_ARRAY:
      case ET::BDEM_TIME_ARRAY:
      case ET::BDEM_LIST:
      case ET::BDEM_TABLE:
      case ET::BDEM_BOOL:
      case ET::BDEM_DATETIMETZ:
      case ET::BDEM_DATETZ:
      case ET::BDEM_TIMETZ:
      case ET::BDEM_BOOL_ARRAY:
      case ET::BDEM_DATETIMETZ_ARRAY:
      case ET::BDEM_DATETZ_ARRAY:
      case ET::BDEM_TIMETZ_ARRAY:
      case ET::BDEM_CHOICE:
      case ET::BDEM_CHOICE_ARRAY: {
      } break;
    }
    return rv;
}

int replaceLoop(Obj &o, bslma_TestAllocator &a, int veryVerbose) {

    int hwm = a.numBytesInUse();

    for (int index = 0; index < 64; index++) {
        if (index % 2) {  // INT64
            bsls_Types::Int64 index64 = index;
            bdem_ConstElemRef cer(&index64, EDA[3]);
            o.replaceElement(0, cer);
        }
        else {            // INT
            bdem_ConstElemRef cer(&index, EDA[2]);
            o.replaceElement(0, cer);
        }

        if (a.numBytesInUse() > hwm) {
            hwm = a.numBytesInUse();
        }

        if (veryVerbose) {
            cout << "high water mark: "
                 << hwm
                 << " number of elements: "
                 << o.size()
                 << endl;
        }
    }

    ASSERT(2 == o.size());
    return hwm;
}

void testConstructorAllocationPT(Obj *list, bslma_TestAllocator& testAlloc) {
    // Determine if the allocator the specified 'list' was created with is used
    // correctly.  This method assumes that 'list' uses
    // 'bdem_AggregateOption::BDEM_PASS_THROUGH' as its
    // 'bdem_AggregateOption::AllocationStrategy' and tests that
    // 'numBytesInUse()' gets decremented after a 'remove()' and 'compact()'.

    bslma_Allocator *alloc = &testAlloc;

    int numBytes = testAlloc.numBytesInUse();
    {
        // Create a vector of doubles that uses a different allocator instance
        // then the one that was used to create 'list'.
        bslma_TestAllocator testAlloc2(veryVeryVerbose);
        bslma_Allocator *alloc2 = &testAlloc2;
        LOOP_ASSERT(testAlloc2.numBytesInUse(),
                    0 == testAlloc2.numBytesInUse());
        bsl::vector<double> da(alloc2);

        int numBytes2 = testAlloc2.numBytesInUse();

        // Insert the elements from the vector into the list.  Assert that the
        // 'numBytesInUse()' of the allocator instance used to create the
        // vector did not increase.  Assert that the 'numBytesInUse()' of the
        // allocator instance used to create 'list' increased.
        list->insertElement(0, &da, EDA[ET::BDEM_DOUBLE_ARRAY]);
        LOOP2_ASSERT(numBytes2, testAlloc2.numBytesInUse(),
                     numBytes2 == testAlloc2.numBytesInUse());
        LOOP2_ASSERT(numBytes, testAlloc.numBytesInUse(),
                     numBytes < testAlloc.numBytesInUse());
        numBytes = testAlloc.numBytesInUse();
    }

    // Get the row definition from the list so we can obtain pointer to the
    // double array inserted above.
    bdem_RowData& rh = list->rowData();
    bsl::vector<double> *elemVector =
        (bsl::vector<double> *)rh.elemRef(0).data();

    // Trigger an allocation by calling reserve on the double array; capacity +
    // 1 triggers allocation.  Assert the 'numBytesInUse()' by the allocator
    // instance used to create 'list' increased.
    elemVector->reserve(elemVector->capacity() + 1);
    LOOP2_ASSERT(numBytes, testAlloc.numBytesInUse(),
                 numBytes < testAlloc.numBytesInUse());

    numBytes = testAlloc.numBytesInUse();

    // Remove the double array from 'list' and compact 'list'.  Assert the
    // allocator instance used to create 'list' decreased.
    list->removeElement(0);
    list->compact();
    LOOP2_ASSERT(numBytes, testAlloc.numBytesInUse(),
                 numBytes > testAlloc.numBytesInUse());
}

void testConstructorAllocationWM(Obj *list, bslma_TestAllocator& testAlloc) {
    // Determine if the allocator the specified 'list' was created with is used
    // correctly.  This method assumes that 'list' uses
    // 'bdem_AggregateOption::WRITE_MANY' as its
    // 'bdem_AggregateOption::AllocationStrategy' and tests that
    // 'numBytesInUse()' does not gets decremented after a 'remove()' and
    // 'compact()'.

    bslma_Allocator *alloc = &testAlloc;

    bsl::vector<double> da;
    list->insertElement(0, &da, EDA[ET::BDEM_DOUBLE_ARRAY]);
    bdem_RowData& rh = list->rowData();
    ((bsl::vector<double> *)rh.elemRef(0).data())->reserve(100);
    int numBytes = testAlloc.numBytesInUse();

    // Remove the double array from 'list' and compact 'list'.  Assert the
    // allocator instance used to create 'list' did not decrease.
    list->removeElement(0);
    list->compact();
    // TBD:
//     LOOP2_ASSERT(numBytes, testAlloc.numBytesInUse(),
//                  numBytes == testAlloc.numBytesInUse());
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
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
        if (verbose) cout << endl << "Testing allocator traits"
                          << endl << "========================" << endl;

        typedef bdem_ListImp Obj;

        ASSERT((0 == bslmf_IsConvertible<bslma_Allocator*, Obj>::VALUE));
        ASSERT((1 ==
             bslalg_HasTrait<Obj, bslalg_TypeTraitUsesBslmaAllocator>::VALUE));

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'replaceElement' ALLOCATION OVERHEAD
        //
        // Concern:
        //   That the replacement of an element by one of bigger footprint
        //   increases the size of the 'bdem_ListImp' in a non-stationary way
        //
        // Plan:
        //   Create a list object and repeatedly replace its first element
        //   by one of a larger footprint, then by the original element.
        //   Verify that doing this several times does not increase the
        //   maximum number of bytes used by the allocator beyond what it
        //   does in a single time.
        //
        // Testing:
        //   void replaceElement(int dstIndex,
        //                       const bdem_ConstElemRef& srcElem);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'replaceElement' (highwatermark)"
                          << endl << "========================================"
                          << endl;

        bslma_TestAllocator allocator;
        Obj mX(PT, &allocator);
        mX.insertElement(0, &INT_VALUE1, EDA[2]);
        // mX.insertElement(1, &INT_VALUE1, EDA[3]);
        const bsls_Types::Int64 INT64_VALUE1 = INT_VALUE1;
        mX.insertElement(1, &INT64_VALUE1, EDA[3]);

        if (veryVerbose)
          cout << "highWaterMark 1-----------------" <<endl;

        const int highWaterMark = replaceLoop(mX, allocator, veryVerbose);

        if (veryVerbose)
          cout << "highWaterMark 2-----------------" <<endl;

        int highWaterMark2 = 0;

        for (int j = 0; j < 10; j++) {
            highWaterMark2 = replaceLoop(mX, allocator, veryVerbose);
        }

        // TBD:
//         ASSERT(highWaterMark >= highWaterMark2);

        if (verbose) cout << "\nEnd of 'replaceElement' Test." << endl;

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'replaceElement'
        //
        // Concerns:
        //   That the replacement of an element by one of same footprint
        //   does not increase the size of the 'bdem_ListImp'.
        //
        // Plan:
        //   Create a list object and replace its first element by one of same
        //   footprint.  Verify that doing this several times does not increase
        //   the number of bytes used by the allocator.
        //
        // Testing:
        //   void replaceElement(int dstIndex,
        //                       const bdem_ConstElemRef& srcElem);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'replaceElement'"
                          << endl << "=======================" << endl;

        bslma_TestAllocator allocator;
        Obj mX(PT, &allocator);
        mX.insertElement(0, &INT_VALUE1, EDA[2]);

        const int BEFORE_SIZE = allocator.numBytesInUse();

        for (int index = 0; index < 16; index++) {
          bdem_ConstElemRef cer(&index, EDA[2]);
          mX.replaceElement(0, cer);

          if (veryVerbose) {
            cout << "Expected size: " << BEFORE_SIZE << endl
                 << "Actual size:   " << allocator.numBytesInUse() << endl;
          }

          ASSERT( BEFORE_SIZE == allocator.numBytesInUse() );
        }

        if (verbose) cout << "\nEnd of 'replaceElement' Test." << endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'reserveMemory'
        //
        // Concerns:
        //   That 'reserveMemory' allocates enough memory as requested.
        //
        // Plan:
        //   Using a test allocator, for both aggregate options
        //   'BDEM_WRITE_ONCE' or 'BDEM_WRITE_MANY' reserve extra memory and
        //   check the allocator got passed the request properly.
        //
        // Testing:
        //   void reserveMemory(int numBytes);
        // --------------------------------------------------------------------
        if (verbose) cout << endl << "Testing 'reserveMemory'"
                          << endl << "=======================" << endl;

        const int SIZE = 4000;

        if (verbose) cout << "\nUsing 'BDEM_WRITE_ONCE'" << endl;
        {
            bslma_TestAllocator allocator;
            bdem_ListImp mX(bdem_AggregateOption::BDEM_WRITE_ONCE, &allocator);

            const int BEFORE_SIZE = allocator.numBytesInUse();

            mX.reserveMemory(SIZE);

            LOOP2_ASSERT(allocator.numBytesInUse(), BEFORE_SIZE,
                         SIZE <= allocator.numBytesInUse() - BEFORE_SIZE);
        }

        if (verbose) cout << "\nUsing 'BDEM_WRITE_MANY'" << endl;
        {
            bslma_TestAllocator allocator;
            bdem_ListImp mX(WM, &allocator);

            const int BEFORE_SIZE = allocator.numBytesInUse();

            mX.reserveMemory(SIZE);

            LOOP2_ASSERT(allocator.numBytesInUse(), BEFORE_SIZE,
                         SIZE <= allocator.numBytesInUse() - BEFORE_SIZE);
        }

        if (verbose) cout << "\nEnd of 'reserveMemory' Test." << endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING CTORS WITH INITIAL MEMORY
        //
        // Concerns:
        //   That constructors with initial memory requests allocate enough
        //   memory as requested.
        //
        // Plan:
        //   Using a test allocator, construct various objects with both
        //   aggregate options 'BDEM_WRITE_ONCE' and 'WRITE_ANY', and check
        //   that enough bytes were indeed requested to the allocator.
        //
        // Testing:
        //   InitialMemory argument to constructors
        // --------------------------------------------------------------------
        if (verbose) cout << endl << "TESTING CTORS WITH INITIAL MEMORY"
                          << endl << "================================="
                          << endl;

        const int SIZE = 4000;  // initial memory

        if (verbose) cout << "\nWith allocMode only." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'." << endl;
            {
                bslma_TestAllocator allocator;

                bdem_ListImp mX(bdem_AggregateOption::BDEM_WRITE_ONCE,
                                bdem_ListImp::InitialMemory(SIZE),
                                &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'." << endl;
            {
                bslma_TestAllocator allocator;

                bdem_ListImp mX(WM,
                                bdem_ListImp::InitialMemory(SIZE),
                                &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
        }

        if (verbose) cout << "\nWith element types." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'." << endl;
            {
                bslma_TestAllocator allocator;

                const ET::Type CHAR = ET::BDEM_CHAR;

                bdem_ListImp mX(&CHAR,
                                1,
                                EDA,
                                bdem_AggregateOption::BDEM_WRITE_ONCE,
                                bdem_ListImp::InitialMemory(SIZE),
                                &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'." << endl;
            {
                bslma_TestAllocator allocator;

                const ET::Type CHAR = ET::BDEM_CHAR;

                bdem_ListImp mX(&CHAR,
                                1,
                                EDA,
                                WM,
                                bdem_ListImp::InitialMemory(SIZE),
                                &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
        }

        if (verbose) cout << "\nWith row header." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'." << endl;
            {
                bslma_TestAllocator allocator;
                bdem_RowLayout rowLayout(&allocator);

                const int SIZE = 128;
                char memory[SIZE];
                bslma_BufferAllocator ba(memory, SIZE);
                bdem_RowData rowData(&rowLayout, PT, &ba);

                const int startSize = allocator.numBytesInUse();

                bdem_ListImp mX(rowData,
                                bdem_AggregateOption::BDEM_WRITE_ONCE,
                                bdem_ListImp::InitialMemory(SIZE),
                                &allocator);

                LOOP2_ASSERT(allocator.numBytesInUse(), startSize,
                             SIZE <= allocator.numBytesInUse() - startSize);
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'." << endl;
            {
                bslma_TestAllocator allocator;
                bdem_RowLayout rowLayout(&allocator);

                const int SIZE = 128;
                char memory[SIZE];
                bslma_BufferAllocator ba(memory, SIZE);
                bdem_RowData rowData(&rowLayout, WM, &ba);

                const int startSize = allocator.numBytesInUse();

                bdem_ListImp mX(rowData,
                                WM,
                                bdem_ListImp::InitialMemory(SIZE),
                                &allocator);

                LOOP2_ASSERT(allocator.numBytesInUse(), startSize,
                             SIZE <= allocator.numBytesInUse() - startSize);
            }
        }

        if (verbose) cout << "\nWith copy ctor." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'." << endl;
            {
                bslma_TestAllocator allocator;
                bdem_ListImp original(bdem_AggregateOption::BDEM_PASS_THROUGH,
                                      &allocator);

                const int startSize = allocator.numBytesInUse();

                bdem_ListImp mX(original,
                                bdem_AggregateOption::BDEM_WRITE_ONCE,
                                bdem_ListImp::InitialMemory(SIZE),
                                &allocator);

                LOOP2_ASSERT(allocator.numBytesInUse(), startSize,
                             SIZE <= allocator.numBytesInUse() - startSize);
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'." << endl;
            {
                bslma_TestAllocator allocator;
                bdem_ListImp original(bdem_AggregateOption::BDEM_PASS_THROUGH,
                                      &allocator);

                const int startSize = allocator.numBytesInUse();

                bdem_ListImp mX(original,
                                WM,
                                bdem_ListImp::InitialMemory(SIZE),
                                &allocator);

                LOOP2_ASSERT(allocator.numBytesInUse(), startSize,
                             SIZE <= allocator.numBytesInUse() - startSize);
            }
        }

        if (verbose) cout << "\nEnd of 'InitialMemory' Test." << endl;
      } break;
      case 7: {
        //--------------------------------------------------------------------
        // TESTING STREAMING
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   bdexStreamInImp(STREAM&, int, const SI *, const D *const[]);
        //   bdexStreamOutImp(STREAM&, const SO *);
        //   streamOutList(void *, STREAM&, int const SO *);
        //   streamInList(void *, STREAM&,int , const SI *, const D *const[]);
        //--------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Streaming" << endl
                                  << "=================" << endl;

        const int NUM_TESTS = 5;

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- backward-compatibility test"
                          << endl;

        static const struct {
            int         d_line;
            int         d_length;
            const char *d_input;
        } DATA[] = {
          // Line Len Input
          // ==== === =====
            { L_, 24, "\xe1\x1\xe0\x2\xe0\x6\xe0\x2"
                      "\xe0\x4\xe1\x0\x0\x0\x4T"
                      "EST\xe6\x0\x0\x0\x3" },
            { L_, 44, "\xe1\x1\xe0\x4\xe0\x6\xe0\x2"
                      "\xe0\x6\xe0\x2\xe0\x4\xe1\x0"
                      "\x0\x0\x4TEST\xe6"
                      "\x0\x0\x0\x3\xe0\x4\xe1\x0"
                      "\x0\x0\x4TEST\xe6"
                      "\x0\x0\x0\x3" },
            { L_, 64, "\xe1\x1\xe0\x6\xe0\x6\xe0\x2"
                      "\xe0\x6\xe0\x2\xe0\x6\xe0\x2"
                      "\xe0\x4\xe1\x0\x0\x0\x4T"
                      "EST\xe6\x0\x0\x0\x3"
                      "\xe0\x4\xe1\x0\x0\x0\x4T"
                      "EST\xe6\x0\x0\x0\x3"
                      "\xe0\x4\xe1\x0\x0\x0\x4T"
                      "EST\xe6\x0\x0\x0\x3" },
            { L_, 84, "\xe1\x1\xe0\x8\xe0\x6\xe0\x2"
                      "\xe0\x6\xe0\x2\xe0\x6\xe0\x2"
                      "\xe0\x6\xe0\x2\xe0\x4\xe1\x0"
                      "\x0\x0\x4TEST\xe6"
                      "\x0\x0\x0\x3\xe0\x4\xe1\x0"
                      "\x0\x0\x4TEST\xe6"
                      "\x0\x0\x0\x3\xe0\x4\xe1\x0"
                      "\x0\x0\x4TEST\xe6"
                      "\x0\x0\x0\x3\xe0\x4\xe1\x0"
                      "\x0\x0\x4TEST\xe6"
                      "\x0\x0\x0\x3" },
            { L_, 104, "\xe1\x1\xe0\xa\xe0\x6\xe0\x2"
                       "\xe0\x6\xe0\x2\xe0\x6\xe0\x2"
                       "\xe0\x6\xe0\x2\xe0\x6\xe0\x2"
                       "\xe0\x4\xe1\x0\x0\x0\x4T"\
                       "EST\xe6\x0\x0\x0\x3"
                       "\xe0\x4\xe1\x0\x0\x0\x4T"
                       "EST\xe6\x0\x0\x0\x3"
                       "\xe0\x4\xe1\x0\x0\x0\x4T"
                       "EST\xe6\x0\x0\x0\x3"
                       "\xe0\x4\xe1\x0\x0\x0\x4T"
                       "EST\xe6\x0\x0\x0\x3"
                       "\xe0\x4\xe1\x0\x0\x0\x4T"
                       "EST\xe6\x0\x0\x0\x3" }
        };
        static const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);
        ASSERT(NUM_DATA == NUM_TESTS);

        {
        for (int i = 0; i < NUM_TESTS; ++i) {
            const int LINE = DATA[i].d_line;
            const int LENGTH = DATA[i].d_length;
            const char *const INPUT = DATA[i].d_input;

            Obj ref(PT);
            for (int j = 0; j <= i; ++j) {
                ref.insertElement(0, &INT_VALUE1, EDA[2]);
                ref.insertElement(0, &STRING_VALUE1, EDA[6]);
            }
            Obj list(PT);
            list.insertElement(0, &INT_VALUE2, EDA[2]);
            list.insertElement(0, &STRING_VALUE2, EDA[6]);
            ASSERT(list != ref);

            int VERSION = 1;

            In in(INPUT, LENGTH);
            In &testInStream = in;
            LOOP_ASSERT(i, in);
            LOOP_ASSERT(i, !in.isEmpty());

            int ver;

            BEGIN_BDEX_EXCEPTION_TEST {
                in.reset();
                in.getVersion(ver);
                LOOP_ASSERT(i, VERSION == ver);

                list.bdexStreamInImp(in, 1, STREAMIN_TABLE, EDA);
            } END_BDEX_EXCEPTION_TEST

            LOOP2_ASSERT(LINE, i, ref == list);
            LOOP2_ASSERT(LINE, i, in);
            LOOP2_ASSERT(LINE, i, in.isEmpty());
        }
        }

        if (verbose) cout << "\t- ok data" << endl;

        {
            // version 1 and 2 no nulls
            for (int v = 1; v <= 2; ++v) {
                for (int i = 0; i < NUM_TESTS; ++i) {
                    Obj ref(PT);
                    for (int j = 0; j <= i; ++j) {
                        ref.insertElement(0, &INT_VALUE1, EDA[2]);
                        ref.insertElement(0, &STRING_VALUE1, EDA[6]);
                    }
                    Obj list(PT);
                    list.insertElement(0, &INT_VALUE2, EDA[2]);
                    list.insertElement(0, &STRING_VALUE2, EDA[6]);
                    ASSERT(list != ref);

                    Out out;
                    int VERSION = v;
                    out.putVersion(VERSION);
                    ref.bdexStreamOutImp(out, VERSION, STREAMOUT_TABLE);
                    const char *const OD  = out.data();
                    const int LOD = out.length();
                    In in(OD, LOD);
                    In &testInStream = in;
                    LOOP_ASSERT(i, in);
                    LOOP_ASSERT(i, !in.isEmpty());

                    int ver;

                    BEGIN_BDEX_EXCEPTION_TEST {
                        in.reset();
                        in.getVersion(ver);
                        LOOP_ASSERT(i, VERSION == ver);

                        list.bdexStreamInImp(in, VERSION, STREAMIN_TABLE, EDA);
                    } END_BDEX_EXCEPTION_TEST

                    LOOP_ASSERT(i, ref == list);
                    LOOP_ASSERT(i, in);
                    LOOP_ASSERT(i, in.isEmpty());
                }
            }

            // version 3 with nulls
            for (int i = 0; i < NUM_TESTS; ++i) {
                Obj ref(PT);
                for (int j = 0; j <= i; ++j) {
                    ref.insertNullElement(0, EDA[2]);
                    ref.insertNullElement(0, EDA[6]);
                }
                Obj list(PT);
                list.insertNullElement(0, EDA[6]);
                list.insertNullElement(0, EDA[2]);
                ASSERT(list != ref);

                Out out;
                int VERSION = 3;
                out.putVersion(VERSION);
                ref.bdexStreamOutImp(out, VERSION, STREAMOUT_TABLE);
                const char *const OD  = out.data();
                const int LOD = out.length();
                In in(OD, LOD);
                In &testInStream = in;
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, !in.isEmpty());

                int ver;

                BEGIN_BDEX_EXCEPTION_TEST {
                  in.reset();
                  in.getVersion(ver);
                  LOOP_ASSERT(i, VERSION == ver);

                  list.bdexStreamInImp(in, VERSION, STREAMIN_TABLE, EDA);
                } END_BDEX_EXCEPTION_TEST

                LOOP_ASSERT(i, ref == list);
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- empty and invalid streams" << endl;

        // version 1 and 2 no nulls
        for (int v = 1; v <= 2; ++v) {
            Obj ref(PT);
            ref.insertElement(0, &INT_VALUE1, EDA[2]);
            ref.insertElement(0, &STRING_VALUE1, EDA[6]);
            Obj refCopy(PT);
            refCopy = ref;

            Out out;
            const char VERSION = v;
            const char *const OD  = out.data();
            const int LOD = out.length();
            ASSERT(0 == LOD);

            In in(OD, LOD);
            In &testInStream = in;
            ASSERT(in);
            ASSERT(in.isEmpty());

            BEGIN_BDEX_EXCEPTION_TEST {
              in.reset();
              ASSERT(in);

              refCopy.bdexStreamInImp(in, VERSION, STREAMIN_TABLE, EDA);
              ASSERT(refCopy == ref);
              ASSERT(!in);

            } END_BDEX_EXCEPTION_TEST
        }

        // version 3 with nulls
        {
            Obj ref(PT);
            ref.insertElement(0, &INT_VALUE1, EDA[2]);
            ref.insertNullElement(0, EDA[6]);
            Obj refCopy(PT);
            refCopy = ref;

            Out out;
            const char VERSION = 3;
            const char *const OD  = out.data();
            const int LOD = out.length();
            ASSERT(0 == LOD);

            In in(OD, LOD);
            In &testInStream = in;
            ASSERT(in);
            ASSERT(in.isEmpty());

            BEGIN_BDEX_EXCEPTION_TEST {
              in.reset();
              ASSERT(in);

              refCopy.bdexStreamInImp(in, VERSION, STREAMIN_TABLE, EDA);
              ASSERT(refCopy == ref);
              ASSERT(!in);

            } END_BDEX_EXCEPTION_TEST
        }

        // --------------------------------------------------------------------

        if (verbose) cout <<
             "\t- incomplete, but otherwise valid, data" << endl;

        // version 1 and 2 no nulls
        for (int v = 1; v <= 2; ++v) {
            Obj ref1(PT);
            ref1.insertElement(0, &INT_VALUE1, EDA[2]);
            Obj ref2(PT);
            ref2.insertElement(0, &STRING_VALUE1, EDA[6]);
            Out out;
            const char VERSION = v;
            ref1.bdexStreamOutImp(out, VERSION, STREAMOUT_TABLE);
            const int LOD1 = out.length();
            ref2.bdexStreamOutImp(out, VERSION, STREAMOUT_TABLE);

            const char *const OD = out.data();

            {
                In in(OD, LOD1 - 1);
                In &testInStream = in;
                BEGIN_BDEX_EXCEPTION_TEST {
                    Obj list(PT);
                    in.reset();
                    ASSERT(in);
                    ASSERT(!in.isEmpty());
                    list.bdexStreamInImp(in, VERSION, STREAMIN_TABLE, EDA);
                    ASSERT(!in);
                    ASSERT(!in.isEmpty());
                } END_BDEX_EXCEPTION_TEST
            }

            {
                In in(OD, LOD1 + 1);
                In &testInStream = in;
                BEGIN_BDEX_EXCEPTION_TEST {
                    Obj list(PT);
                    in.reset();
                    ASSERT(in);
                    ASSERT(!in.isEmpty());
                    list.bdexStreamInImp(in, VERSION, STREAMIN_TABLE, EDA);
                    ASSERT(list == ref1);
                    ASSERT(in);
                    ASSERT(!in.isEmpty());
                    list.bdexStreamInImp(in, VERSION, STREAMIN_TABLE, EDA);
                    ASSERT(!in);
                    ASSERT(!in.isEmpty());
                } END_BDEX_EXCEPTION_TEST
            }
        }

        // version 3 with nulls
        {
            Obj ref1(PT);
            ref1.insertElement(0, &INT_VALUE1, EDA[2]);
            Obj ref2(PT);
            ref2.insertNullElement(0, EDA[6]);
            Out out;
            const char VERSION = 3;
            ref1.bdexStreamOutImp(out, VERSION, STREAMOUT_TABLE);
            const int LOD1 = out.length();
            ref2.bdexStreamOutImp(out, VERSION, STREAMOUT_TABLE);

            const char *const OD = out.data();

            {
                In in(OD, LOD1 - 1);
                In &testInStream = in;
                BEGIN_BDEX_EXCEPTION_TEST {
                    Obj list(PT);
                    in.reset();
                    ASSERT(in);
                    ASSERT(!in.isEmpty());
                    list.bdexStreamInImp(in, VERSION, STREAMIN_TABLE, EDA);
                    ASSERT(!in);
                    ASSERT(!in.isEmpty());
                } END_BDEX_EXCEPTION_TEST
            }

            {
                In in(OD, LOD1 + 1);
                In &testInStream = in;
                BEGIN_BDEX_EXCEPTION_TEST {
                    Obj list(PT);
                    in.reset();
                    ASSERT(in);
                    ASSERT(!in.isEmpty());
                    list.bdexStreamInImp(in, VERSION, STREAMIN_TABLE, EDA);
                    ASSERT(list == ref1);
                    ASSERT(in);
                    ASSERT(!in.isEmpty());
                    list.bdexStreamInImp(in, VERSION, STREAMIN_TABLE, EDA);
                    ASSERT(!in);
                    ASSERT(!in.isEmpty());
                } END_BDEX_EXCEPTION_TEST
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- bad version number" << endl;

        {
            Obj ref(PT);
            ref.insertElement(0, &INT_VALUE1, EDA[2]);
            ref.insertNullElement(0, EDA[6]);

            for (int ver = 0; ver != 99; ver = 99) {

                {
                    Obj list(PT);
                    list = ref;
                    Out out;
                    list.bdexStreamOutImp(out, ver, STREAMOUT_TABLE);
                    ASSERT(0 == out.length());
                    ASSERT(list == ref);
                }

                {
                    Obj list(PT);
                    Out out;
                    int v = 1;
                    out.putVersion(1);
                    ref.bdexStreamOutImp(out, 1, STREAMOUT_TABLE);
                    const char *const OD  = out.data();
                    const int LOD = out.length();
                    ASSERT(0 != LOD);

                    In in(OD, LOD);
                    ASSERT(in);
                    ASSERT(!in.isEmpty());

                    in.reset();
                    in.getVersion(v);
                    ASSERT(1 == v);

                    list.bdexStreamInImp(in, ver, STREAMIN_TABLE, EDA);
                    ASSERT(list != ref);
                    ASSERT(!in);
                    ASSERT(!in.isEmpty());
                }

                {
                    Obj list(PT);
                    Out out;
                    out.putVersion(0);
                    ref.bdexStreamOutImp(out, 1, STREAMOUT_TABLE);
                    const char *const OD  = out.data();
                    const int LOD = out.length();

                    In in(OD, LOD);
                    ASSERT(in);
                    ASSERT(!in.isEmpty());

                    in.reset();
                    in.setQuiet(!veryVerbose);
                    in.setSuppressVersionCheck(1);
                    // TBD this was streamIn
                    int version;
                    in.getVersion(version);
                    ASSERT(0 == version);
                    list.bdexStreamInImp(in, version, STREAMIN_TABLE, EDA);
                    ASSERT(list != ref);
                    ASSERT(!in);
                    ASSERT(!in.isEmpty());
                }
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- wrapper functions" << endl;

        // version 1 and 2 no nulls
        {
            for (int v = 1; v <= 2; ++v) {
                const char VERSION = v;

                for (int i = 0; i < NUM_TESTS; ++i) {
                    Obj ref(PT);
                    for (int j = 0; j <= i; ++j) {
                        ref.insertElement(0, &INT_VALUE1, EDA[2]);
                        ref.insertElement(0, &STRING_VALUE1, EDA[6]);
                        ref.insertElement(0, &INT_VALUE1, EDA[2]);
                    }

                    {
                        Obj list(PT);
                        list.insertElement(0, &INT_VALUE2, EDA[2]);
                        list.insertElement(0, &STRING_VALUE2, EDA[6]);
                        list.insertElement(0, &INT_VALUE2, EDA[2]);
                        LOOP_ASSERT(i, list != ref);

                        Out out;
                        // TBD this was streamOut...
                        out.putVersion(VERSION);
                        ref.bdexStreamOutImp(out, VERSION, STREAMOUT_TABLE);
                        const char *const OD  = out.data();
                        const int         LOD = out.length();
                        In in(OD, LOD);
                        In &testInStream = in;
                        LOOP_ASSERT(i, in);
                        LOOP_ASSERT(i, !in.isEmpty());

                        BEGIN_BDEX_EXCEPTION_TEST {
                            in.reset();
                            // TBD this was streamIn
                            int version;
                            in.getVersion(version);
                            ASSERT(VERSION == version);
                            list.bdexStreamInImp(in, version,
                                              STREAMIN_TABLE, EDA);
                        } END_BDEX_EXCEPTION_TEST

                        LOOP_ASSERT(i, ref == list);
                        LOOP_ASSERT(i, in);
                        LOOP_ASSERT(i, in.isEmpty());
                    }

                    {
                        Obj list(PT);
                        list.insertElement(0, &INT_VALUE2, EDA[2]);
                        list.insertElement(0, &STRING_VALUE2, EDA[6]);
                        LOOP_ASSERT(i, list != ref);

                        Out out;
                        bdem_ListImp::streamOutList(&ref, out, VERSION,
                                                    STREAMOUT_TABLE);
                        const char *const OD  = out.data();
                        const int         LOD = out.length();
                        In in(OD, LOD);
                        in.setSuppressVersionCheck(1);
                        bdem_ListImp::streamInList(&list, in, VERSION,
                                                   STREAMIN_TABLE, EDA);
                        LOOP_ASSERT(i, list == ref);
                    }
                }
            }

            // version 3 with nulls
            {
                const char VERSION = 3;
                for (int i = 0; i < NUM_TESTS; ++i) {
                    Obj ref(PT);
                    for (int j = 0; j <= i; ++j) {
                        ref.insertElement(0, &INT_VALUE1, EDA[2]);
                        ref.insertNullElement(0, EDA[6]);
                    }

                    {
                        Obj list(PT);
                        list.insertNullElement(0, EDA[2]);
                        list.insertElement(0, &STRING_VALUE2, EDA[6]);
                        LOOP_ASSERT(i, list != ref);

                        Out out;
                        // TBD this was streamOut...
                        out.putVersion(VERSION);
                        ref.bdexStreamOutImp(out, VERSION, STREAMOUT_TABLE);
                        const char *const OD  = out.data();
                        const int         LOD = out.length();
                        In in(OD, LOD);
                        In &testInStream = in;
                        LOOP_ASSERT(i, in);
                        LOOP_ASSERT(i, !in.isEmpty());

                        BEGIN_BDEX_EXCEPTION_TEST {
                            in.reset();
                            // TBD this was streamIn
                            int version;
                            in.getVersion(version);
                            ASSERT(VERSION == version);
                            list.bdexStreamInImp(in, version,
                                              STREAMIN_TABLE, EDA);
                        } END_BDEX_EXCEPTION_TEST

                        LOOP_ASSERT(i, ref == list);
                        LOOP_ASSERT(i, in);
                        LOOP_ASSERT(i, in.isEmpty());
                    }

                    {
                        Obj list(PT);
                        list.insertElement(0, &INT_VALUE2, EDA[2]);
                        list.insertElement(0, &STRING_VALUE2, EDA[6]);
                        LOOP_ASSERT(i, list != ref);

                        Out out;
                        bdem_ListImp::streamOutList(&ref, out, VERSION,
                                                    STREAMOUT_TABLE);
                        const char *const OD  = out.data();
                        const int         LOD = out.length();
                        In in(OD, LOD);
                        in.setSuppressVersionCheck(1);
                        bdem_ListImp::streamInList(&list, in, VERSION,
                                                   STREAMIN_TABLE, EDA);
                        LOOP_ASSERT(i, list == ref);
                    }
                }
            }
        }
      } break;
      case 6: {
        //-------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   bdem_ListImp(AS, ALLOC);
        //   bdem_ListImp(AS, IM, ALLOC);
        //   bdem_ListImp(ET, NE, ALT, AS, ALLOC);
        //   bdem_ListImp(ET, NE, ALT, AS, IM, ALLOC);
        //   bdem_ListImp(RH, AS, ALLOC);
        //   bdem_ListImp(RH, AS, IM, ALLOC);
        //   bdem_ListImp(LI, AS, ALLOC);
        //   bdem_ListImp(LI, AS, IM, ALLOC);
        //--------------------------------------------------------------------

        if (verbose) cout << endl << "Testing creators"
                          << endl <<  "================" << endl;

        if (verbose)
            cout << "bdem_ListImp(AS, ALLOC)" << endl <<
                    "-----------------------" << endl;

        if (verbose) cout << "\ttesting BDEM_PASS_THROUGH" << endl;
        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());

            Obj *list = new Obj(PT, &testAlloc);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 <  testAlloc.numBytesInUse());

            testConstructorAllocationPT(list, testAlloc);

            delete list;
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());
        }

        if (verbose) cout << "\ttesting BDEM_WRITE_MANY" << endl;
        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());

            Obj *list = new Obj(WM, &testAlloc);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 <  testAlloc.numBytesInUse());

            testConstructorAllocationWM(list, testAlloc);

            delete list;
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());
        }

        if (verbose)
            cout << "bdem_ListImp(AS, IM, ALLOC)" << endl <<
            "-----------------------------------" << endl;

        if (verbose) cout << "\ttesting BDEM_PASS_THROUGH" << endl;
        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());

            Obj *list = new Obj(PT, Obj::InitialMemory(50), &testAlloc);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 <  testAlloc.numBytesInUse());

            testConstructorAllocationPT(list, testAlloc);

            delete list;
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());
        }

        if (verbose) cout << "\ttesting BDEM_WRITE_MANY" << endl;
        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());

            Obj *list = new Obj(WM, Obj::InitialMemory(50), &testAlloc);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 <  testAlloc.numBytesInUse());

            testConstructorAllocationWM(list, testAlloc);

            delete list;
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());
        }

        if (verbose)
            cout << "bdem_ListImp(ET, NE, ALT, AS, ALLOC)" << endl <<
                    "------------------------------------" << endl;

        if (verbose) cout << "\ttesting BDEM_PASS_THROUGH" << endl;
        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());

            Obj *list = new Obj(ETA, 2, EDA, PT, &testAlloc);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 <  testAlloc.numBytesInUse());

            testConstructorAllocationPT(list, testAlloc);

            delete list;
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());
        }

        if (verbose) cout << "\ttesting BDEM_WRITE_MANY" << endl;
        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());

            Obj *list = new Obj(ETA, 2, EDA, WM, &testAlloc);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 <  testAlloc.numBytesInUse());

            testConstructorAllocationWM(list, testAlloc);

            delete list;
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());
        }

        if (verbose)
            cout << "bdem_ListImp(ET, NE, ALT, AS, IM, ALLOC)" << endl <<
                    "----------------------------------------" << endl;

        if (verbose) cout << "\ttesting BDEM_PASS_THROUGH" << endl;
        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());

            Obj *list =
                new Obj(ETA, 2, EDA, PT, Obj::InitialMemory(2), &testAlloc);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 <  testAlloc.numBytesInUse());

            testConstructorAllocationPT(list, testAlloc);

            delete list;
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());
        }

        if (verbose) cout << "\ttesting BDEM_WRITE_MANY" << endl;
        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());

            Obj *list =
                new Obj(ETA, 2, EDA, WM, Obj::InitialMemory(2), &testAlloc);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 <  testAlloc.numBytesInUse());

            testConstructorAllocationWM(list, testAlloc);

            delete list;
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());
        }

        if (verbose)
            cout << "bdem_ListImp(RH, AS, ALLOC)" << endl <<
                    "---------------------------" << endl;

        if (verbose) cout << "\ttesting BDEM_PASS_THROUGH" << endl;
        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());

            bdem_RowLayout rowLayout(&testAlloc);
            const int SIZE = 128;
            char memory[SIZE];
            bslma_BufferAllocator ba(memory, SIZE);
            RowData rh(&rowLayout, PT, &ba);
            Obj *list = new Obj(rh, PT, &testAlloc);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 <  testAlloc.numBytesInUse());

            testConstructorAllocationPT(list, testAlloc);

            delete list;
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());
        }

        if (verbose) cout << "\ttesting BDEM_WRITE_MANY" << endl;
        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());

            bdem_RowLayout rowLayout(&testAlloc);
            const int SIZE = 128;
            char memory[SIZE];
            bslma_BufferAllocator ba(memory, SIZE);
            RowData rh(&rowLayout, WM, &ba);
            Obj *list = new Obj(rh, WM, &testAlloc);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 <  testAlloc.numBytesInUse());

            testConstructorAllocationWM(list, testAlloc);

            delete list;
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());
        }

        if (verbose)
            cout << "bdem_ListImp(RH, AS, IM, ALLOC)" << endl <<
                    "-------------------------------" << endl;

        if (verbose) cout << "\ttesting BDEM_PASS_THROUGH" << endl;
        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());

            bslma_TestAllocator alloc(veryVeryVerbose);
            bdem_RowLayout rowLayout(&alloc);
            const int SIZE = 128;
            char memory[SIZE];
            bslma_BufferAllocator ba(memory, SIZE);
            RowData rh(&rowLayout, PT, &ba);
            Obj *list = new Obj(rh, PT, Obj::InitialMemory(2), &testAlloc);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 <  testAlloc.numBytesInUse());

            testConstructorAllocationPT(list, testAlloc);

            delete list;
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());
        }

        if (verbose) cout << "\ttesting BDEM_WRITE_MANY" << endl;
        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());

            bdem_RowLayout rowLayout(&testAlloc);
            const int SIZE = 128;
            char memory[SIZE];
            bslma_BufferAllocator ba(memory, SIZE);
            RowData rh(&rowLayout, WM, &ba);
            Obj *list = new Obj(rh, WM, Obj::InitialMemory(2), &testAlloc);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 <  testAlloc.numBytesInUse());

            testConstructorAllocationWM(list, testAlloc);

            delete list;
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());
        }

        if (verbose)
            cout << "bdem_ListImp(LI, AS, ALLOC)" << endl <<
                    "---------------------------" << endl;

        if (verbose) cout << "\ttesting BDEM_PASS_THROUGH" << endl;
        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());

            bslma_TestAllocator alloc(veryVeryVerbose);
            Obj li(PT, &alloc);
            Obj *list = new Obj(li, PT, &testAlloc);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 <  testAlloc.numBytesInUse());

            testConstructorAllocationPT(list, testAlloc);

            delete list;
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());
        }

        if (verbose) cout << "\ttesting BDEM_WRITE_MANY" << endl;
        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());

            bslma_TestAllocator alloc(veryVeryVerbose);
            Obj li(WM, &alloc);
            Obj *list = new Obj(li, WM, &testAlloc);
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 <  testAlloc.numBytesInUse());

            testConstructorAllocationWM(list, testAlloc);

            delete list;
            LOOP_ASSERT(testAlloc.numBytesInUse(),
                        0 == testAlloc.numBytesInUse());
        }
      } break;
      case 5: {
        //-------------------------------------------------------------------
        // TESTING
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //--------------------------------------------------------------------

        if (verbose) cout << "" << endl <<
                             "==========================" << endl;

      } break;
      case 4: {
        //--------------------------------------------------------------------
        // TESTING
        //   This test verifies that the "delegation functions" are "hooked up"
        //   properly.  Note that no attempt is made at checking for exception
        //   neutrality.
        //
        // Concerns:
        //   Each method under test calls the appropriate 'delegated' method,
        //   and all arguments are correctly processed as expected.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   bdem_ListImp& operator=(const bdem_RowData& rhs);
        //   bdem_ListImp& operator=(const bdem_ListImp& rhs);
        //   void insertElements(int dI, RH& row, int src, int num);
        //   void *insertElementRaw(int dI, D *elemAttr);
        //   void *insertNullElement(int dI, const D *elemAttr);
        //   void insertNullElements(int d, ET t[], int n, D *const luTbl[]);
        //   void swap(int index1, int index2);
        //   void replaceElement(int dI, const bdem_ConstElemRef& srcElem);
        //   void resetElement(int dI, cDesc *elemAttr);
        //   void reset(ET elemTypes[], int numElems, D *const lookupTbl[]);
        //   void removeElement(int index);
        //   void removeElements(int startIndex, int numElements);
        //   void removeAll();
        //   bsl::ostream& print(bsl::ostream& s, int level, int spcs) const;
        //--------------------------------------------------------------------

        if (verbose) cout << endl << "Testing delegation"
                          << endl << "==================" << endl;

        //--------------------------------------------------------------------

        if (verbose) cout << "\t- assignment operators" << endl;

        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            bslma_Allocator *alloc = &testAlloc;

            Obj ref0(PT, alloc);
            ref0.insertElement(0, &INT_VALUE1, EDA[2]);
            Obj ref1(PT, alloc);
            ref1.insertElement(0, &INT_VALUE1,    EDA[2]);
            ref1.insertElement(1, &STRING_VALUE1, EDA[6]);
            ASSERT(ref0 != ref1);

            Obj list(PT, alloc);
            list = ref0;
            ASSERT(list == ref0);
            ASSERT(list != ref1);

            list = ref1;
            ASSERT(list != ref0);
            ASSERT(list == ref1);

            list = ref0.rowData();
            ASSERT(list == ref0);
            ASSERT(list != ref1);

            list = ref1.rowData();
            ASSERT(list != ref0);
            ASSERT(list == ref1);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- insertElements" << endl;

        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            bslma_Allocator *alloc = &testAlloc;

            Obj ref0(PT, alloc);
            ref0.insertElement(0, &STRING_VALUE1, EDA[6]);
            ref0.insertElement(1, &INT_VALUE1,    EDA[2]);
            Obj ref1(PT, alloc);
            ref1.insertElement(0, &STRING_VALUE1, EDA[6]);
            ref1.insertElement(1, &STRING_VALUE1, EDA[6]);
            ref1.insertElement(2, &INT_VALUE1,    EDA[2]);
            ref1.insertElement(3, &INT_VALUE1,    EDA[2]);
            Obj ref2(PT, alloc);
            ref2.insertElement(0, &STRING_VALUE1, EDA[6]);
            ref2.insertElement(1, &STRING_VALUE1, EDA[6]);
            ref2.insertElement(2, &INT_VALUE1,    EDA[2]);
            ref2.insertElement(3, &INT_VALUE1,    EDA[2]);
            ref2.insertElement(4, &INT_VALUE1,    EDA[2]);

            Obj list1(PT, alloc);
            list1.insertElement(0, &STRING_VALUE1, EDA[6]);
            list1.insertElement(1, &INT_VALUE1,    EDA[2]);
            Obj list2(PT, alloc);
            list2.insertElements(0, list1.rowData(), 0, 2);
            ASSERT(list2 == ref0);
            ASSERT(list2 != ref1);
            ASSERT(list2 != ref2);

            list2.insertElements(1, list1.rowData(), 0, 2);
            ASSERT(list2 != ref0);
            ASSERT(list2 == ref1);
            ASSERT(list2 != ref2);

            list2.insertElements(2, list1.rowData(), 1, 1);
            ASSERT(list2 != ref0);
            ASSERT(list2 != ref1);
            ASSERT(list2 == ref2);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- insertNullElement" << endl;

        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            bslma_Allocator *alloc = &testAlloc;

            void *p;
            Obj list(PT, alloc);

            p = list.insertNullElement(0, EDA[2]);
            ASSERT(1      == list.length());
            ASSERT(ETA[2] == list.elemType(0));
            ASSERT(isEqual(ETA[2], &UNSET_INT,
                           list.rowData().elemRef(0).data()));

            *(int *)p = INT_VALUE1;
            ASSERT(isEqual(ETA[2], &INT_VALUE1,
                           list.rowData().elemRef(0).data()));

            p = list.insertNullElement(0, EDA[6]);
            ASSERT(2      == list.length());
            ASSERT(ETA[6] == list.elemType(0));
            ASSERT(ETA[2] == list.elemType(1));
            ASSERT(isEqual(ETA[6], &UNSET_STRING,
                           list.rowData().elemRef(0).data()));

            *(bsl::string *)p = STRING_VALUE1;
            ASSERT(isEqual(ETA[6], &STRING_VALUE1,
                           list.rowData().elemRef(0).data()));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- insertNullElements" << endl;

        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            bslma_Allocator *alloc = &testAlloc;

            Obj list(PT, alloc);
            const ET::Type  etas[] = { ETA[6], ETA[2], ETA[6] };

            // short, int, short
            list.insertNullElements(0, etas, 3, EDA);
            ASSERT(3      == list.length());
            ASSERT(ETA[6] == list.elemType(0));
            ASSERT(ETA[2] == list.elemType(1));
            ASSERT(ETA[6] == list.elemType(2));
            ASSERT(isEqual(ETA[6], &UNSET_STRING,
                           list.rowData().elemRef(0).data()));
            ASSERT(isEqual(ETA[2], &UNSET_INT,
                           list.rowData().elemRef(1).data()));
            ASSERT(isEqual(ETA[6], &UNSET_STRING,
                           list.rowData().elemRef(2).data()));

            // short, short, int, short, int, short
            list.insertNullElements(1, etas, 3, EDA);
            ASSERT(6      == list.length());
            ASSERT(ETA[6] == list.elemType(0));
            ASSERT(ETA[6] == list.elemType(1));
            ASSERT(ETA[2] == list.elemType(2));
            ASSERT(ETA[6] == list.elemType(3));
            ASSERT(ETA[2] == list.elemType(4));
            ASSERT(ETA[6] == list.elemType(5));
            ASSERT(isEqual(ETA[6], &UNSET_STRING,
                           list.rowData().elemRef(0).data()));
            ASSERT(isEqual(ETA[6], &UNSET_STRING,
                           list.rowData().elemRef(1).data()));
            ASSERT(isEqual(ETA[2], &UNSET_INT,
                           list.rowData().elemRef(2).data()));
            ASSERT(isEqual(ETA[6], &UNSET_STRING,
                           list.rowData().elemRef(3).data()));
            ASSERT(isEqual(ETA[2], &UNSET_INT,
                           list.rowData().elemRef(4).data()));
            ASSERT(isEqual(ETA[6], &UNSET_STRING,
                           list.rowData().elemRef(5).data()));
        }

        if (verbose) cout << "\t- insertNullElement" << endl;

        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            bslma_Allocator *alloc = &testAlloc;

            void *p;
            Obj list(PT, alloc); const Obj& L = list;

            p = list.insertNullElement(0, EDA[2]);
            ASSERT(1      == list.length());
            ASSERT(ETA[2] == list.elemType(0));
            ASSERT(isEqual(ETA[2], &UNSET_INT, L.rowData().elemRef(0).data()));
            ASSERT(L.rowData().elemRef(0).isNull());

            p = list.insertNullElement(0, EDA[6]);
            ASSERT(2      == list.length());
            ASSERT(ETA[6] == list.elemType(0));
            ASSERT(ETA[2] == list.elemType(1));
            ASSERT(isEqual(ETA[6], &UNSET_STRING,
                           L.rowData().elemRef(0).data()));
            ASSERT(L.rowData().elemRef(0).isNull());
            ASSERT(L.rowData().elemRef(1).isNull());

            list.rowData().elemRef(0).theModifiableString() =
                                                                STRING_VALUE1;
            ASSERT(isEqual(ETA[6], &STRING_VALUE1,
                           L.rowData().elemRef(0).data()));
            ASSERT(L.rowData().elemRef(0).isNonNull());
            ASSERT(L.rowData().elemRef(1).isNull());

            // Insert a null element at an index other then 0
            p = list.insertNullElement(2, EDA[2]);
            ASSERT(3      == list.length());
            ASSERT(ETA[6] == list.elemType(0));
            ASSERT(ETA[2] == list.elemType(1));
            ASSERT(ETA[2] == list.elemType(2));
            ASSERT(isEqual(ETA[2], &UNSET_INT, L.rowData().elemRef(2).data()));
            ASSERT(L.rowData().elemRef(0).isNonNull());
            ASSERT(L.rowData().elemRef(1).isNull());
            ASSERT(L.rowData().elemRef(2).isNull());

            list.rowData().elemRef(2).theModifiableInt() = INT_VALUE2;
            ASSERT(isEqual(ETA[2], &INT_VALUE2,
                           L.rowData().elemRef(2).data()));
            ASSERT(L.rowData().elemRef(0).isNonNull());
            ASSERT(L.rowData().elemRef(1).isNull());
            ASSERT(L.rowData().elemRef(2).isNonNull());

            // Insert a null element before and after a non-null element
            p = list.insertNullElement(2, EDA[2]);
            ASSERT(4      == list.length());
            ASSERT(ETA[6] == list.elemType(0));
            ASSERT(ETA[2] == list.elemType(1));
            ASSERT(ETA[2] == list.elemType(2));
            ASSERT(ETA[2] == list.elemType(3));
            ASSERT(isEqual(ETA[2], &UNSET_INT, L.rowData().elemRef(2).data()));
            ASSERT(L.rowData().elemRef(0).isNonNull());
            ASSERT(L.rowData().elemRef(1).isNull());
            ASSERT(L.rowData().elemRef(2).isNull());
            ASSERT(L.rowData().elemRef(3).isNonNull());

            p = list.insertNullElement(4, EDA[2]);
            ASSERT(5      == list.length());
            ASSERT(ETA[6] == list.elemType(0));
            ASSERT(ETA[2] == list.elemType(1));
            ASSERT(ETA[2] == list.elemType(2));
            ASSERT(ETA[2] == list.elemType(3));
            ASSERT(ETA[2] == list.elemType(4));
            ASSERT(isEqual(ETA[2], &UNSET_INT,
                           L.rowData().elemRef(4).data()));
            ASSERT(L.rowData().elemRef(0).isNonNull());
            ASSERT(L.rowData().elemRef(1).isNull());
            ASSERT(L.rowData().elemRef(2).isNull());
            ASSERT(L.rowData().elemRef(3).isNonNull());
            ASSERT(L.rowData().elemRef(4).isNull());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- insertNullElements" << endl;

        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            bslma_Allocator *alloc = &testAlloc;

            Obj list(PT, alloc); const Obj& L = list;
            const ET::Type  etas[] = { ETA[6], ETA[2], ETA[6] };

            // short, int, short
            list.insertNullElements(0, etas, 3, EDA);
            ASSERT(3      == list.length());
            ASSERT(ETA[6] == list.elemType(0));
            ASSERT(ETA[2] == list.elemType(1));
            ASSERT(ETA[6] == list.elemType(2));
            ASSERT(isEqual(ETA[6], &UNSET_STRING,
                           L.rowData().elemRef(0).data()));
            ASSERT(isEqual(ETA[2], &UNSET_INT,
                           L.rowData().elemRef(1).data()));
            ASSERT(isEqual(ETA[6], &UNSET_STRING,
                           L.rowData().elemRef(2).data()));
            ASSERT(L.rowData().elemRef(0).isNull());
            ASSERT(L.rowData().elemRef(1).isNull());
            ASSERT(L.rowData().elemRef(2).isNull());

            // short, short, int, short, int, short
            list.insertNullElements(1, etas, 3, EDA);
            ASSERT(6      == list.length());
            ASSERT(ETA[6] == list.elemType(0));
            ASSERT(ETA[6] == list.elemType(1));
            ASSERT(ETA[2] == list.elemType(2));
            ASSERT(ETA[6] == list.elemType(3));
            ASSERT(ETA[2] == list.elemType(4));
            ASSERT(ETA[6] == list.elemType(5));
            ASSERT(isEqual(ETA[6], &UNSET_STRING,
                           L.rowData().elemRef(0).data()));
            ASSERT(isEqual(ETA[6], &UNSET_STRING,
                           L.rowData().elemRef(1).data()));
            ASSERT(isEqual(ETA[2], &UNSET_INT,
                           L.rowData().elemRef(2).data()));
            ASSERT(isEqual(ETA[6], &UNSET_STRING,
                           L.rowData().elemRef(3).data()));
            ASSERT(isEqual(ETA[2], &UNSET_INT,
                           L.rowData().elemRef(4).data()));
            ASSERT(isEqual(ETA[6], &UNSET_STRING,
                           L.rowData().elemRef(5).data()));
            ASSERT(L.rowData().elemRef(0).isNull());
            ASSERT(L.rowData().elemRef(1).isNull());
            ASSERT(L.rowData().elemRef(2).isNull());
            ASSERT(L.rowData().elemRef(3).isNull());
            ASSERT(L.rowData().elemRef(4).isNull());
            ASSERT(L.rowData().elemRef(5).isNull());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- swap" << endl;

        {
            {
                bslma_TestAllocator testAlloc(veryVeryVerbose);
                bslma_Allocator *alloc = &testAlloc;

                // one element - swap element with itself
                Obj ref(PT, alloc);
                ref.insertElement(0, &STRING_VALUE1, EDA[6]);
                Obj list(PT, alloc);
                list.insertElement(0, &STRING_VALUE1, EDA[6]);
                ASSERT(list == ref);

                list.swap(0, 0);
                ASSERT(list == ref);
            }

            {
                bslma_TestAllocator testAlloc(veryVeryVerbose);
                bslma_Allocator *alloc = &testAlloc;

                // two elements
                Obj ref(PT, alloc);
                ref.insertElement(0, &STRING_VALUE1, EDA[6]);
                ref.insertElement(1, &INT_VALUE1,   EDA[2]);
                Obj list(PT, alloc);
                list.insertElement(0, &INT_VALUE1,   EDA[2]);
                list.insertElement(1, &STRING_VALUE1, EDA[6]);
                ASSERT(list != ref);

                list.swap(0, 1);
                ASSERT(list == ref);

                list.swap(1, 0);
                ASSERT(list != ref);

                list.swap(1, 0);
                ASSERT(list == ref);
            }

            {
                bslma_TestAllocator testAlloc(veryVeryVerbose);
                bslma_Allocator *alloc = &testAlloc;

                // three elements
                Obj ref(PT, alloc);
                ref.insertElement(0, &STRING_VALUE1, EDA[6]);
                ref.insertElement(1, &INT_VALUE1,   EDA[2]);
                ref.insertElement(2, &STRING_VALUE2, EDA[6]);
                Obj list(PT, alloc);
                list.insertElement(0, &INT_VALUE1,   EDA[2]);
                list.insertElement(1, &STRING_VALUE2, EDA[6]);
                list.insertElement(2, &STRING_VALUE1, EDA[6]);
                ASSERT(list != ref);

                list.swap(1, 2);
                ASSERT(list != ref);

                list.swap(0, 1);
                ASSERT(list == ref);
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- replaceElement" << endl;

        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            bslma_Allocator *alloc = &testAlloc;

            Obj ref0(PT, alloc);
            ref0.insertElement(0, &STRING_VALUE2, EDA[6]);
            ref0.insertElement(1, &STRING_VALUE1, EDA[6]);
            ref0.insertElement(2, &STRING_VALUE1, EDA[6]);
            Obj ref1(PT, alloc);
            ref1.insertElement(0, &STRING_VALUE2, EDA[6]);
            ref1.insertElement(1, &STRING_VALUE1, EDA[6]);
            ref1.insertElement(2, &STRING_VALUE2, EDA[6]);
            Obj ref2(PT, alloc);
            ref2.insertElement(0, &STRING_VALUE2, EDA[6]);
            ref2.insertElement(1, &STRING_VALUE1, EDA[6]);
            ref2.insertElement(2, &INT_VALUE1,    EDA[2]);

            Obj list(PT, alloc);
            list.insertElement(0, &STRING_VALUE1, EDA[6]);
            list.insertElement(1, &STRING_VALUE1, EDA[6]);
            list.insertElement(2, &STRING_VALUE1, EDA[6]);
            ASSERT(list != ref0);
            ASSERT(list != ref1);
            ASSERT(list != ref2);

            if (veryVerbose) {
                bsl::cout << "BEFORE replace:" << bsl::endl;
                list.print(bsl::cout, 1); ref0.print(bsl::cout, 1);
                ref1.print(bsl::cout, 1); ref2.print(bsl::cout, 1);
            }

            bdem_ConstElemRef cer1(&STRING_VALUE2, EDA[6]);
            list.replaceElement(0, cer1);

            if (veryVerbose) {
                bsl::cout << "AFTER replace 0:" << bsl::endl;
                list.print(bsl::cout, 1); ref0.print(bsl::cout, 1);
                ref1.print(bsl::cout, 1); ref2.print(bsl::cout, 1);
            }

            ASSERT(list == ref0);
            ASSERT(list != ref1);
            ASSERT(list != ref2);

            list.replaceElement(2, cer1);

            if (veryVerbose) {
                bsl::cout << "AFTER replace 2:" << bsl::endl;
                list.print(bsl::cout, 1); ref0.print(bsl::cout, 1);
                ref1.print(bsl::cout, 1); ref2.print(bsl::cout, 1);
            }

            ASSERT(list != ref0);
            ASSERT(list == ref1);
            ASSERT(list != ref2);

            bdem_ConstElemRef cer2(&INT_VALUE1, EDA[2]);
            list.replaceElement(2, cer2);

            if (veryVerbose) {
                bsl::cout << "AFTER replace 2:" << bsl::endl;
                list.print(bsl::cout, 1); ref0.print(bsl::cout, 1);
                ref1.print(bsl::cout, 1); ref2.print(bsl::cout, 1);
            }

            ASSERT(list != ref0);
            ASSERT(list != ref1);
            ASSERT(list == ref2);

            {

            //L.rowData().elemRef(0).makeNull();
            Obj ref0(PT, alloc);
            ref0.insertNullElement(0, EDA[6]);
            ref0.insertElement(1, &STRING_VALUE1, EDA[6]);
            ref0.insertElement(2, &STRING_VALUE1, EDA[6]);
            Obj ref1(PT, alloc);
            ref1.insertNullElement(0, EDA[6]);
            ref1.insertElement(1, &STRING_VALUE1, EDA[6]);
            ref1.insertNullElement(2, EDA[6]);
            Obj ref2(PT, alloc);
            ref2.insertNullElement(0, EDA[6]);
            ref2.insertElement(1, &STRING_VALUE1, EDA[6]);
            ref2.insertNullElement(2, EDA[2]);

            Obj list(PT, alloc);
            list.insertElement(0, &STRING_VALUE1, EDA[6]);
            list.insertElement(1, &STRING_VALUE1, EDA[6]);
            list.insertElement(2, &STRING_VALUE1, EDA[6]);
            ASSERT(list != ref0);
            ASSERT(list != ref1);
            ASSERT(list != ref2);

            if (veryVerbose) {
                bsl::cout << "BEFORE replace:" << bsl::endl;
                bsl::cout << "list = "; list.print(bsl::cout, 1);
                bsl::cout << "ref0 = "; ref0.print(bsl::cout, 1);
                bsl::cout << "ref1 = "; ref1.print(bsl::cout, 1);
                bsl::cout << "ref2 = "; ref2.print(bsl::cout, 1);
            }

            int nullBits = 0;
            bsl::string s2 = STRING_VALUE2;

            bdem_ElemRef er1(&s2, EDA[6], &nullBits, 0);
            er1.makeNull();
            list.replaceElement(0, er1);

            if (veryVerbose) {
                bsl::cout << "AFTER replace 0:" << bsl::endl;
                bsl::cout << "list = "; list.print(bsl::cout, 1);
                bsl::cout << "ref0 = "; ref0.print(bsl::cout, 1);
                bsl::cout << "ref1 = "; ref1.print(bsl::cout, 1);
                bsl::cout << "ref2 = "; ref2.print(bsl::cout, 1);
            }

            ASSERT(list == ref0);
            ASSERT(list != ref1);
            ASSERT(list != ref2);

            list.replaceElement(2, er1);

            if (veryVerbose) {
                bsl::cout << "AFTER replace 2:" << bsl::endl;
                bsl::cout << "list = "; list.print(bsl::cout, 1);
                bsl::cout << "ref0 = "; ref0.print(bsl::cout, 1);
                bsl::cout << "ref1 = "; ref1.print(bsl::cout, 1);
                bsl::cout << "ref2 = "; ref2.print(bsl::cout, 1);
            }

            ASSERT(list != ref0);
            ASSERT(list == ref1);
            ASSERT(list != ref2);

            int i1 = 999;
            bdem_ElemRef er2(&i1, EDA[2], &nullBits, 1);
            er2.makeNull();
            list.replaceElement(2, er2);

            if (veryVerbose) {
                bsl::cout << "AFTER replace 2:" << bsl::endl;
                bsl::cout << "list = "; list.print(bsl::cout, 1);
                bsl::cout << "ref0 = "; ref0.print(bsl::cout, 1);
                bsl::cout << "ref1 = "; ref1.print(bsl::cout, 1);
                bsl::cout << "ref2 = "; ref2.print(bsl::cout, 1);
            }

            ASSERT(list != ref0);
            ASSERT(list != ref1);
            ASSERT(list == ref2);
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- resetElement" << endl;

        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            bslma_Allocator *alloc = &testAlloc;

            Obj ref0(PT, alloc);
            ref0.insertNullElement(0, EDA[2]);
            ref0.insertElement(1, &STRING_VALUE1, EDA[6]);
            ref0.insertElement(2, &STRING_VALUE1, EDA[6]);
            Obj ref1(PT, alloc);
            ref1.insertNullElement(0, EDA[2]);
            ref1.insertElement(1, &STRING_VALUE1, EDA[6]);
            ref1.insertNullElement(2, EDA[2]);
            Obj ref2(PT, alloc);
            ref2.insertNullElement(0, EDA[2]);
            ref2.insertNullElement(1, EDA[6]);
            ref2.insertNullElement(2, EDA[2]);

            // resetElement
            Obj list(PT, alloc);
            list.insertElement(0, &STRING_VALUE1, EDA[6]);
            list.insertElement(1, &STRING_VALUE1, EDA[6]);
            list.insertElement(2, &STRING_VALUE1, EDA[6]);
            ASSERT(list != ref0);
            ASSERT(list != ref1);
            ASSERT(list != ref2);

            list.resetElement(0, EDA[6]); // reset, but to different type
            ASSERT(list != ref0);
            ASSERT(list != ref1);
            ASSERT(list != ref2);

            list.resetElement(0, EDA[2]);
            ASSERT(list == ref0);
            ASSERT(list != ref1);
            ASSERT(list != ref2);

            list.resetElement(2, EDA[2]);
            ASSERT(list != ref0);
            ASSERT(list == ref1);
            ASSERT(list != ref2);

            list.resetElement(1, EDA[6]);
            ASSERT(list != ref0);
            ASSERT(list != ref1);
            ASSERT(list == ref2);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- reset" << endl;

        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            bslma_Allocator *alloc = &testAlloc;

            // reset
            Obj ref0(PT, alloc);
            ref0.insertNullElement(0, EDA[2]);
            Obj ref1(PT, alloc);
            ref1.insertNullElement(0, EDA[2]);
            ref1.insertNullElement(1, EDA[6]);

            const ET::Type etas[] = { ETA[2], ETA[6] };
            Obj list(PT, alloc);
            list.insertElement(0, &STRING_VALUE1, EDA[6]);
            list.insertElement(1, &STRING_VALUE1, EDA[6]);
            list.insertElement(2, &STRING_VALUE1, EDA[6]);
            ASSERT(list != ref0);
            ASSERT(list != ref1);

            list.reset(etas, 1, EDA);
            ASSERT(list == ref0);
            ASSERT(list != ref1);

            list.reset(etas, 2, EDA);
            ASSERT(list != ref0);
            ASSERT(list == ref1);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- remove*" << endl;

        {
            bslma_TestAllocator testAlloc(veryVeryVerbose);
            bslma_Allocator *alloc = &testAlloc;

            Obj ref0(PT, alloc);
            Obj ref1(PT, alloc);
            ref1.insertElement(0, &STRING_VALUE1, EDA[6]);
            Obj ref2(PT, alloc);
            ref2.insertElement(0, &INT_VALUE1, EDA[2]);
            Obj ref3(PT, alloc);
            ref3.insertElement(0, &STRING_VALUE1, EDA[6]);
            ref3.insertElement(1,   &INT_VALUE1, EDA[2]);

            {
                // removeElement
                Obj list(PT, alloc);
                list.insertElement(0, &STRING_VALUE1, EDA[6]);
                list.insertElement(1, &INT_VALUE1, EDA[2]);

                list.removeElement(0);
                ASSERT(list == ref2);

                list.removeElement(0);
                ASSERT(list == ref0);

                list.insertElement(0, &STRING_VALUE1, EDA[6]);
                list.insertElement(1, &INT_VALUE1, EDA[2]);
                list.removeElement(1);
                ASSERT(list == ref1);

                list.removeElement(0);
                ASSERT(list == ref0);
            }

            {
                // removeElements
                Obj list(PT, alloc);
                list.insertElement(0, &STRING_VALUE1, EDA[6]);
                list.insertElement(1, &INT_VALUE1, EDA[2]);

                list.removeElements(0, 1);
                ASSERT(list == ref2);

                list.removeElements(0, 1);
                ASSERT(list == ref0);

                list.insertElement(0, &STRING_VALUE1, EDA[6]);
                list.insertElement(1, &INT_VALUE1,   EDA[2]);
                list.removeElements(1, 1);
                ASSERT(list == ref1);

                list.removeElements(0, 1);
                ASSERT(list == ref0);

                list.insertElement(0, &STRING_VALUE1, EDA[6]);
                list.insertElement(1, &INT_VALUE1,   EDA[2]);
                list.removeElements(0, 2);
                ASSERT(list == ref0);

                list.insertElement(0, &STRING_VALUE1, EDA[6]);
                list.insertElement(1, &INT_VALUE1,   EDA[2]);
                list.insertElement(2, &STRING_VALUE1, EDA[6]);

                list.removeElements(0, 2);
                ASSERT(list == ref1);

                list.removeElements(0, 1);
                ASSERT(list == ref0);

                list.insertElement(0, &STRING_VALUE1, EDA[6]);
                list.insertElement(1, &INT_VALUE1,   EDA[2]);
                list.insertElement(2, &STRING_VALUE1, EDA[6]);

                list.removeElements(1, 2);
                ASSERT(list == ref1);
            }

            {
                // removeAll
                Obj list(PT, alloc);
                list.insertElement(0, &STRING_VALUE1, EDA[6]);

                list.removeAll();
                ASSERT(list == ref0);

                list.insertElement(0, &STRING_VALUE1, EDA[6]);
                list.insertElement(1, &INT_VALUE1, EDA[2]);
                list.removeAll();
                ASSERT(list == ref0);
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- print" << endl;

        {
            const char ref0[] =  "{"                            "\n"
                                 "    INT 3"                    "\n"
                                 "}"                            "\n";

            const char ref1[] =  "    {"                        "\n"
                                 "        INT 3"                "\n"
                                 "    }"                        "\n";

            const char ref2[] =  "{"                            "\n"
                                 "  INT 3"                      "\n"
                                 "}"                            "\n";

            const char ref3[] =  "{"                            "\n"
                                 "  INT NULL"                   "\n"
                                 "  INT 3"                      "\n"
                                 "}"                            "\n";

            bslma_TestAllocator testAlloc(veryVeryVerbose);
            bslma_Allocator *alloc = &testAlloc;

            Obj list(PT, alloc);
            list.insertElement(0, &INT_VALUE1, EDA[2]);
            char buf[100];
            {
                ostrstream out(buf, 100);
                list.print(out, 0, 4) << ends;
                ASSERT(strlen(buf) == strlen(ref0));
                ASSERT(0 == memcmp(buf, ref0, strlen(ref0)));
            }

            {
                ostrstream out(buf, 100);
                list.print(out, 1, 4) << ends;
                ASSERT(strlen(buf) == strlen(ref1));
                ASSERT(0 == memcmp(buf, ref1, strlen(ref1)));
            }

            {
                ostrstream out(buf, 100);
                list.print(out, 0, 2) << ends;
                ASSERT(strlen(buf) == strlen(ref2));
                ASSERT(0 == memcmp(buf, ref2, strlen(ref2)));
            }

            list.insertNullElement(0, EDA[2]);
            {
                ostrstream out(buf, 100);
                list.print(out, 0, 2) << ends;
                ASSERT(strlen(buf) == strlen(ref3));
                ASSERT(0 == memcmp(buf, ref3, strlen(ref3)));
            }
        }
      } break;
      case 3: {
        //--------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //
        // Concerns:
        // Plan:
        // Testing:
        //   bool operator==(const bdem_ListImp& lhs, const bdem_ListImp& rhs);
        //   bool operator!=(const bdem_ListImp& lhs, const bdem_ListImp& rhs);
        //--------------------------------------------------------------------

          if (verbose) cout << endl << "Testing equality operators"
                            << endl << "==========================" << endl;

          Obj list1(PT);
          Obj list2(PT);
          ASSERT(list1 == list1);   ASSERT(!(list1 != list1));
          ASSERT(list2 == list2);   ASSERT(!(list2 != list2));
          ASSERT(list1 == list2);   ASSERT(!(list1 != list2));
          ASSERT(list2 == list1);   ASSERT(!(list2 != list1));

          list1.insertElement(0, &STRING_VALUE1, EDA[6]);
          ASSERT(list1 == list1);   ASSERT(!(list1 != list1));
          ASSERT(list1 != list2);   ASSERT(!(list1 == list2));
          ASSERT(list2 != list1);   ASSERT(!(list2 == list1));

          list2.insertElement(0, &STRING_VALUE1, EDA[6]);
          ASSERT(list2 == list2);   ASSERT(!(list2 != list2));
          ASSERT(list1 == list2);   ASSERT(!(list1 != list2));
          ASSERT(list2 == list1);   ASSERT(!(list2 != list1));

          list1.insertElement(1, &INT_VALUE1, EDA[2]);
          ASSERT(list1 == list1);   ASSERT(!(list1 != list1));
          ASSERT(list1 != list2);   ASSERT(!(list1 == list2));
          ASSERT(list2 != list1);   ASSERT(!(list2 == list1));

          list2.insertElement(1, &STRING_VALUE1, EDA[6]);
          ASSERT(list1 == list1);   ASSERT(!(list1 != list1));
          ASSERT(list1 != list2);   ASSERT(!(list1 == list2));
         ASSERT(list2 != list1);   ASSERT(!(list2 == list1));
      } break;
      case 2: {
        //--------------------------------------------------------------------
        // PRIMARY CONSTRUCTOR(BOOTSTRAP)/MANIPULATOR, DIRECT ACCESSORS
        //   The methods under test are all trivial, for the most part
        //   performing simple delegation.  It is sufficient to verify that
        //   each method is "hooked-up" properly.
        //
        // Concerns:
        //   Each method under test:
        //     a. Calls the appropriate underlying function.
        //     b. Arguments are correctly passed through.
        //     c. Return values are correctly passed back.
        //
        // Plan:
        //   We first bootstrap test the primary constructor.  It is trivial
        //   in its operation: it forwards arguments to sub-object constructor,
        //   and then initializes a single data member.  We test it by
        //   confirming that a test allocator allocates memory for the object,
        //   and that a subsequent call to the (untested) 'length' method
        //   returns 0.
        //
        // Testing:
        //   BOOTSTRAP: bdem_ListImp(AS allocMode, bslma_Allocator *alloc = 0);
        //   void *insertElement(int dstIndex, const void *value, D *elemAttr);
        //   int length() const;
        //   bdem_ElemType::Type elemType(int index) const;
        //   const bdem_RowData& rowData() const;
        //   bdem_RowData& rowData();
        //--------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing primary manipulator and accessors" << endl
            << "=========================================" << endl;

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- constructor bootstrap" << endl;

        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);
            bslma_Allocator *alloc = &testAllocator;

            ASSERT(0 == testAllocator.numBytesInUse());

            Obj list(PT, alloc);

            ASSERT(0 <  testAllocator.numBytesInUse());
            ASSERT(0 == list.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\t- insertElement, length, elemType, row" <<
                         endl;

        {
            const struct {
                      int   d_line;      // source line number
                      int   d_elemIdx0;  // index into EDA/ETA
                      int   d_elemIdx1;  // index into EDA/ETA
                const void *d_val0;      // input and expected value 0
                const void *d_val1;      // input and expected value 1
            } DATA[] = {

                //    |------ input -------|   |------- input/output ------|
                //
                //    elem idx 0  elem idx 1         value 0         value 1
                //    ----------  ----------         -------         -------

                { L_,          6,          6, &STRING_VALUE1, &STRING_VALUE1 },
                { L_,          6,          6, &STRING_VALUE1, &STRING_VALUE2 },
                { L_,          6,          2, &STRING_VALUE1,    &INT_VALUE1 },
                { L_,          6,          2, &STRING_VALUE2,    &INT_VALUE2 },
                { L_,          2,          2,    &INT_VALUE1,    &INT_VALUE2 },
                { L_,          2,          2,    &INT_VALUE1,    &INT_VALUE1 },
            };

            const int DATA_SIZE = sizeof DATA/sizeof *DATA;

            for (int i = 0; i < DATA_SIZE; ++i) {
                const int      LINE      = DATA[i].d_line;

                // test with each bsl::vector twice:
                //     insert (e1, e2) and (e2, e1)
                int elemIdx0[2] = { DATA[i].d_elemIdx0, DATA[i].d_elemIdx1 };
                int elemIdx1[2] = { DATA[i].d_elemIdx1, DATA[i].d_elemIdx0 };
                const void *val0[2] = { DATA[i].d_val0, DATA[i].d_val1 };
                const void *val1[2] = { DATA[i].d_val1, DATA[i].d_val0 };
                void *p1;
                void *p2;

                for (int j = 0; j < 2; ++j) {
                    Obj list(PT);
                    LOOP3_ASSERT(LINE, i, j, 0 == list.length());

                    p1 = list.insertElement(0, val0[j], EDA[elemIdx0[j]]);
                    p2 = list.rowData().elemRef(0).data();
                    LOOP3_ASSERT(LINE, i, j, p1 == p2);
                    LOOP3_ASSERT(LINE, i, j, 1 == list.length());
                    LOOP3_ASSERT(LINE, i, j, ETA[elemIdx0[j]] ==
                                 list.elemType(0));
                    LOOP3_ASSERT(LINE, i, j, isEqual(ETA[elemIdx0[j]],
                                                     val0[j], p1));

                    p1 = list.insertElement(1, val1[j], EDA[elemIdx1[j]]);
                    p2 = list.rowData().elemRef(1).data();
                    LOOP3_ASSERT(LINE, i, j, p1 == p2);
                    LOOP3_ASSERT(LINE, i, j, 2 == list.length());
                    LOOP3_ASSERT(LINE, i, j, ETA[elemIdx1[j]] ==
                                 list.elemType(1));
                    LOOP3_ASSERT(LINE, i, j, isEqual(ETA[elemIdx1[j]],
                                                     val1[j], p1));

                    p1 = list.rowData().elemRef(0).data();
                    LOOP3_ASSERT(LINE, i, j, isEqual(ETA[elemIdx0[j]],
                                                     val0[j], p1));
                }
            }

            // test non-const rowData()
            Obj list(PT);
            list.insertElement(0, &INT_VALUE1, EDA[2]);
            ASSERT(INT_VALUE1 == *(int *)list.rowData().elemRef(0).data());
        }

      } break;
      case 1: {
        //--------------------------------------------------------------------
        // BREATHING TEST
        // Concerns:
        //   We are concerned that the basic functionality of 'bdem_ListImp'
        //   works properly.
        //
        // Plan:
        //
        // Testing:
        //   This breathing test exercises basic functionality as a preliminary
        //   to the thorough testing done in subsequent cases.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Breathing Test" << endl
                          << "==============" << endl;

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
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
