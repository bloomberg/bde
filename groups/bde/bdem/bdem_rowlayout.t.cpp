// bdem_rowlayout.t.cpp                                               -*-C++-*-

#include <bdem_rowlayout.h>

#include <bdem_elemtype.h>

#include <bdema_bufferedsequentialallocator.h>   // for testing only

#include <bdet_datetime.h>                       // for testing only
#include <bdet_datetimetz.h>                     // for testing only
#include <bdet_date.h>                           // for testing only
#include <bdet_datetz.h>                         // for testing only
#include <bdet_time.h>                           // for testing only
#include <bdet_timetz.h>                         // for testing only

#include <bdex_testinstream.h>                   // for testing only
#include <bdex_testoutstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>          // for testing only

#include <bdeu_printmethods.h>                   // for testing only

#include <bslma_testallocator.h>                 // for testing only
#include <bslma_testallocatorexception.h>        // for testing only

#include <bsls_alignmentfromtype.h>
#include <bsls_alignedbuffer.h>                  // for testing only
#include <bsls_platform.h>                       // for testing only
#include <bsls_types.h>                          // for testing only

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_c_ctype.h>                            // isspace()

#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                               ----------
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'insert' and 'removeAll' methods.  Additional helper functions are provided
// to facilitate perturbation of internal state (e.g.  capacity).  Note that
// each manipulator must support aliasing, and those that perform memory
// allocation must be tested for exception neutrality via the
// 'bdema_testallocator' component.  Exception neutrality involving streaming
// is verified using 'bdex_testinstream' (and 'bdex_testoutstream').
//
// Note further that the 'bdem_RowLayout' is a parameterized type therefore
// in order to test it here, we provide a type that instructs the map regarding
// what size/alignment values to use for each element type.  This class, called
// 'TestType', provides the desired static member functions to the offset map
// and allows the test driver to adjust those values between those tests,
// setting the size/alignment of each of the 22 types as needed.  In addition,
// we provide a helper function to set the 'bdem_RowLayout' total offset
// (i.e., the cursor position).
//
//-----------------------------------------------------------------------------
// [ 2] bdem_RowLayout(bslma_Allocator *ba = 0);
// [16] bdem_RowLayout(const InitialCapacity& ne, *ba = 0);
// [11] bdem_RowLayout(const my_ElemTypeArray& sa, *ba = 0);
// [ 7] bdem_RowLayout(const bdem_RowLayout& original, *ba = 0);
// [ 2] ~bdem_RowLayout();
// [12] void append(bdem_ElemType::Type item);
// [12] void append(const my_ElemTypeArray& sa);
// [12] void append(const my_ElemTypeArray& sa, int si, int ne);
// [12] void insert(int di, bdem_ElemType::Type item);
// [12] void insert(int di, const my_ElemTypeArray& sa);
// [12] void insert(int di, const my_ElemTypeArray& sa, int si, int ne);
// [12] void remove(int index);
// [12] void remove(int index, int ne);
// [ 2] void removeAll();
// [13] void replace(int di, bdem_ElemType::Type item);
// [13] void replace(int di, const my_ElemTypeArray& sa, int si, int ne);
// [18] void resetElemTypes(const my_ElemTypeArray& sa);
// [15] void swap(int index1, int index2);
// [10] static int typeAlignment(bdem_ElemType::Type item);
// [10] static int typeSize(bdem_ElemType::Type item);
// [17] void compact();
// [16] void reserveCapacity(int ne);
// [16] void reserveCapacityRaw(int ne);
// [19] STREAM& bdexStreamIn(STREAM& stream, int version, *attrLookup[]);
// [ 4] bdem_ElemType::Type getType(int index) const;
// [ 4] int getOffset(int index) const;
// [ 4] int totalOffset() const;
// [ 4] const my_ElemTypeArray& elemTypeArray() const;
// [ 4] int length() const;
// [14] ostream& print(ostream& os, int level, int spacesPerLevel) const;
// [19] STREAM& bdexStreamOut(STREAM& stream, int version) const;
//-----------------------------------------------------------------------------
// [ 6] bool operator==(const bdem_RowLayout& lhs,
//                      const bdem_RowLayout& rhs);
// [ 6] bool operator!=(const bdem_RowLayout& lhs,
//                      const bdem_RowLayout& rhs);
// [ 5] ostream& operator<<(ostream& os, const bdem_RowLayout& rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [21] USAGE EXAMPLE
// [ 2] BOOTSTRAP: void insert(int di, bdem_ElemType::Type item);  // !aliasing
// [ 8] bsl::vector<double> g(const char *spec);
// [ 3] Obj&               gg(Obj *obj, const char *spec);
// [ 3] int               ggg(Obj *obj, const char *spec, int vF = 1);
// [ 3] void stretchState(Obj *obj, int cursor);
// [ 3] void stretch(Obj *object, int size);
// [ 3] void stretchRemoveAll(Obj *object, int size);
// [ 9] bdem_RowLayout& operator=(const bdem_RowLayout& rhs);
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

typedef bsls_Types::Int64 Int64;

typedef bdem_RowLayout         Obj;
typedef const bdem_Descriptor *Element;

typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

struct AggregateOptionRep {
    enum {
        DUMMYA = 1,
        DUMMYB = 2
    };
    enum Strategy {
        DUMMYC = 1,
        DUMMYD = 2,
        DUMMYE = 3,
        DUMMYF = 4
    };
};

struct AggregateRep {
    AggregateOptionRep::Strategy  d_allocMode;
    void                         *d_originalAllocator_p;
    void                         *d_internalAllocator_p;
    void                         *d_rowdDef_p;
};

struct InfrequentDeleteBlockListRep {
    void *d_head_p;
    void *d_allocator_p;
};

struct PoolRep {
    int                           d_objectSize;
    int                           d_numObjects;
    void                         *d_freeList_p;
    InfrequentDeleteBlockListRep  d_blockList;
};

struct ChoiceHeaderRep {
    void                                   *d_catalog_p;
    int                                     d_selector;
    AggregateOptionRep::Strategy            d_allocMode;
    bsls_AlignedBuffer<14 * sizeof(void*)>  d_selectionBuf;
};

struct AllocatorContainerRep {
    AggregateOptionRep::Strategy  d_allocMode;
    void                         *d_basicAllocator;
    void                         *d_internalAllocator;
};

struct CatalogContainerRep {
    void                         *d_catalog_p;
    AggregateOptionRep::Strategy  d_allocMode;
};

struct ListRep {
    AggregateRep  d_aggImp;
    void         *d_row_p;
};

struct TableRep {
    AggregateRep       d_aggImp;
    PoolRep            d_rowPool;
    bsl::vector<void*> d_rows;
};

struct ChoiceRep {
    ChoiceHeaderRep  d_header;
    void            *d_originalAllocator;
};

struct ChoiceArrayRep {
    AllocatorContainerRep        d_allocContainer;
    CatalogContainerRep          d_catalogContainer;
#if !defined(BSLS_PLATFORM__CMP_MSVC)
    bsl::vector<ChoiceHeaderRep> d_headers;
#else
    int                          d_headers;  // lie to avoid Windows internal
                                             // compiler error
#endif
};

template<bdem_ElemType::Type e, typename t>
struct ElemAttGenerator {
    static bdem_Descriptor s;
};

template<bdem_ElemType::Type e, typename t>
bdem_Descriptor ElemAttGenerator<e,t>::s =
{
    e,
    sizeof(t),
    bsls_AlignmentFromType<t>::VALUE,
    0,0,0,0,0,0,0,0,0
};

const int NUM_TYPES = 32;

const bdem_Descriptor *const typesLookupTable[NUM_TYPES] =
{
    &ElemAttGenerator<bdem_ElemType::BDEM_CHAR,char>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_SHORT,short>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_INT,int>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_INT64,Int64>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_FLOAT,float>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_DOUBLE,double>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_STRING,bsl::string>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_DATETIME,bdet_Datetime>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_DATE,bdet_Date>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_TIME,bdet_Time>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_CHAR_ARRAY,bsl::vector<char> >::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_SHORT_ARRAY,bsl::vector<short> >::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_INT_ARRAY,bsl::vector<int> >::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_INT64_ARRAY,bsl::vector<Int64> >::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_FLOAT_ARRAY,bsl::vector<float> >::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_DOUBLE_ARRAY,
                      bsl::vector<double> >::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_STRING_ARRAY,
                                               bsl::vector<bsl::string> >::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_DATETIME_ARRAY,
                                               bsl::vector<bdet_Datetime> >::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_DATE_ARRAY,
                      bsl::vector<bdet_Date> >::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_TIME_ARRAY,
                      bsl::vector<bdet_Time> >::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_LIST,ListRep>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_TABLE,TableRep>::s,
    // New types added
    &ElemAttGenerator<bdem_ElemType::BDEM_BOOL,bool>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_DATETIMETZ,bdet_DatetimeTz>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_DATETZ,bdet_DateTz>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_TIMETZ,bdet_TimeTz>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_BOOL_ARRAY,bsl::vector<bool> >::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_DATETIMETZ_ARRAY,
                                             bsl::vector<bdet_DatetimeTz> >::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_DATETZ_ARRAY,
                                                 bsl::vector<bdet_DateTz> >::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_TIMETZ_ARRAY,
                                                 bsl::vector<bdet_TimeTz> >::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_CHOICE,ChoiceRep>::s,
    &ElemAttGenerator<bdem_ElemType::BDEM_CHOICE_ARRAY,ChoiceArrayRep>::s,
};

const Element VALUES[] = {
    typesLookupTable[bdem_ElemType::BDEM_CHAR],
    typesLookupTable[bdem_ElemType::BDEM_SHORT],
    typesLookupTable[bdem_ElemType::BDEM_INT],
    typesLookupTable[bdem_ElemType::BDEM_INT64],
    typesLookupTable[bdem_ElemType::BDEM_FLOAT],
}; // avoid DEFAULT_VALUE

const Element &V0 = VALUES[0], &VA = V0,
              &V1 = VALUES[1], &VB = V1,
              &V2 = VALUES[2], &VC = V2,
              &V3 = VALUES[3], &VD = V3,
              &V4 = VALUES[4], &VE = V4;

const int NUM_VALUES         = sizeof VALUES / sizeof *VALUES;
static const char bdemType[] = "ABCDEFGHIJKLMNOPQRSTUVabcdefghij";

enum {
    PTR_SIZE = sizeof(char *)
};

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <typename TYPE>
const TYPE *vectorData(const bsl::vector<TYPE>& v)
    // Return a pointer to the first element of 'v' or null if 'v' is empty.
{
    return v.empty() ? 0 : &v.front();
}

typedef bsl::vector<bdem_ElemType::Type> my_ElemTypeArray;

void getElemTypeArray(my_ElemTypeArray         *retArray,
                      const bdem_RowLayout&     map)
{
    int myLength = map.length();

    retArray->resize(myLength);
    for (int i = 0; i < myLength; ++i) {
        (*retArray)[i] = (bdem_ElemType::Type)map[i].attributes()->d_elemEnum;
    }
}

void getElemAttrArray(bsl::vector<const bdem_Descriptor*> *retArray,
                      const bdem_RowLayout&                map)
{
    int  myLength = map.length();
    retArray->resize(myLength);
    for (int i = 0; i < myLength; ++i) {
        (*retArray)[i] = map[i].attributes();
    }
}

void getOffsetArray(bsl::vector<int> *retArray, const bdem_RowLayout& map)
{
    int  myLength = map.length();
    retArray->resize(myLength);
    for (int i = 0; i < myLength; ++i) {
        (*retArray)[i] = map[i].offset();
    }
}

void stretch(Obj *object, int size)
    // Using only primary manipulators, extend the length of the specified
    // 'object' by the specified size.  The resulting value is not specified.
    // The behavior is undefined unless 0 <= size.
{
    ASSERT(object);
    ASSERT(0 <= size);
    for (int i = 0; i < size; ++i) {
        object->append(typesLookupTable[bdem_ElemType::BDEM_CHAR]);
    }
    ASSERT(object->length() >= size);
}

void stretchRemoveAll(Obj *object, int size)
    // Using only primary manipulators, extend the capacity of the specified
    // 'object' to (at least) the specified size; then remove all elements
    // leaving 'object' empty.  The behavior is undefined unless 0 <= size.
{
    ASSERT(object);
    ASSERT(0 <= size);
    stretch(object, size);
    object->removeAll();
    ASSERT(0 == object->length());
}

void stretchState(Obj *object, int cursor)
    // Using only 'removeAll' and 'stretch', set the cursor position of the
    // specified 'object' to the specified position.
{
    object->removeAll();
    stretch(object, cursor);
}

// Note that 'operator==' and 'operator!=' are not defined in the interface
// of 'bdem_RowLayout'.  They are defined here to facilitate testing.

bool operator==(const bdem_RowLayout& lhs, const bdem_RowLayout& rhs)
{
    bsl::vector<int> lhsOffsets;
    my_ElemTypeArray lhsTypes;
    getElemTypeArray(&lhsTypes,lhs);
    getOffsetArray(&lhsOffsets,lhs);

    bsl::vector<int> rhsOffsets;
    my_ElemTypeArray rhsTypes;
    getElemTypeArray(&rhsTypes,rhs);
    getOffsetArray(&rhsOffsets,rhs);

    return lhs.totalOffset() == rhs.totalOffset()
        && lhsTypes          == rhsTypes
        && lhsOffsets        == rhsOffsets;
}

bool operator!=(const bdem_RowLayout& lhs, const bdem_RowLayout& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const bdem_RowLayout& rhs)
{
    bsl::vector<int> rhsOffsets;
    my_ElemTypeArray rhsTypes;
    getElemTypeArray(&rhsTypes, rhs);
    getOffsetArray(&rhsOffsets, rhs);

    stream << "Types: ";
    bdeu_PrintMethods::print(stream, rhsTypes, 0, -1);
    stream << " Offsets: ";
    bdeu_PrintMethods::print(stream, rhsOffsets, 0, -1);
    stream << " TotalOffset: " << rhs.totalOffset();

    return stream;
}

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. V] correspond to arbitrary (but unique) element types to be
// inserted into the 'bdem_RowLayout' object.  A tilde ('~') indicates that
// the logical (but not necessarily physical) state of the object is to be set
// to its initial, empty state (via the 'removeAll' method).
//
// LANGUAGE SPECIFICATION
// ----------------------
//
// <SPEC>       ::= <EMPTY>   | <LIST>
//
// <EMPTY>      ::=
//
// <LIST>       ::= <ITEM>    | <ITEM><LIST>
//
// <ITEM>       ::= <ELEMENT> | <REMOVE_ALL>
//
// <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' |
//                  'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' |
//                  'S' | 'T' | 'U' | 'V' | 'a' | 'b' | 'c' | 'd' | 'e' |
//                  'f' | 'g' | 'h' | 'i' | 'j'
//
// <REMOVE_ALL> ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// "A"          Append the value corresponding to A.
// "AA"         Append two values both corresponding to A.
// "ABC"        Append three values corresponding to A, B and C.
// "ABC~"       Append three values corresponding to A, B and C and then remove
//              all the elements (set array length to 0).  Note that this spec
//              yields an object that is logically equivalent (but not
//              necessarily identical internally) to one yielded by ("").
// "ABC~DE"     Append three values corresponding to A, B, and C; empty the
//              object; then append values corresponding to D and E.
//
//-----------------------------------------------------------------------------

int ggg(Obj *obj, const char *spec, int VF=1)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'append' and white-box
    // manipulator 'removeAll'.  Optionally specify a zero 'verboseFlag' to
    // suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    enum { SUCCESS = -1 };
    // No modifications in the two cases of empty string.
    if (!spec || *spec == '\0') {
        return SUCCESS;
    }
    int len = (int)strlen(spec);

    for (int i = 0; i < len; i++) {
        if (spec[i] == '~') {
            obj->removeAll();
        }
        else if (spec[i] >= 'A' && spec[i] <= 'V') {
            bdem_ElemType::Type item = (bdem_ElemType::Type)(spec[i]-'A');
            obj->insert(obj->length(), typesLookupTable[item]);
        }
        else if (spec[i] >= 'a' && spec[i] <= 'j') {
            // New types added at the 22nd position
            bdem_ElemType::Type item = (bdem_ElemType::Type)(spec[i]-'a'+22);
            obj->insert(obj->length(), typesLookupTable[item]);
        }
        else {
            // Report where we failed.
            if (VF) {
                cerr << "Error, bad character ('"
                     << spec[i]
                     << "') in spec \""
                     << spec
                     << "\" at position "
                     << i
                     << '.' << endl;
            }
            return i;
        }
    }
    return SUCCESS;
}

Obj& gg(Obj *obj, const char *spec)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(obj, spec) < 0);
    return *obj;
}

Obj g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    Obj object((bslma_Allocator *)0);
    return gg(&object, spec);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    my_ElemTypeArray eta;
    my_ElemTypeArray eta2;
    bslma_TestAllocator  testAllocator(veryVeryVerbose);
    bslma_Allocator     *Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 20: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;

        enum {
            CHAR  = bdem_ElemType::BDEM_CHAR,
            SHORT = bdem_ElemType::BDEM_SHORT,
            INT   = bdem_ElemType::BDEM_INT
        };

        // First, create an empty row layout:

        bdem_RowLayout m;     ASSERT(0 == m.length());

        // Add some elements to our layout by appending to the end:

        m.append(typesLookupTable[CHAR]);
        ASSERT( 1    == m.length());
        ASSERT(CHAR  == m[0].attributes()->d_elemEnum);
        ASSERT( 0    == m[0].offset());
        ASSERT( 1    == m.totalOffset());

        m.append(typesLookupTable[INT]);
        ASSERT( 2    == m.length());
        ASSERT(INT   == m[1].attributes()->d_elemEnum);
        ASSERT( 4    == m[1].offset());
        ASSERT( 8    == m.totalOffset());

        m.append(typesLookupTable[INT]);
        ASSERT( 3    == m.length());
        ASSERT(INT   == m[2].attributes()->d_elemEnum);
        ASSERT( 8    == m[2].offset());
        ASSERT(12    == m.totalOffset());

        m.append(typesLookupTable[SHORT]);
        ASSERT( 4    == m.length());
        ASSERT(SHORT == m[3].attributes()->d_elemEnum);
        ASSERT(12    == m[3].offset());
        ASSERT(14    == m.totalOffset());

        // Note we can insert in the middle.  While according to the indexes,
        // the new element is in the middle of the list, while according to the
        // offsets it was added onto the end.

        m.insert(2, typesLookupTable[SHORT]);
        ASSERT( 5    == m.length());
        ASSERT(SHORT == m[2].attributes()->d_elemEnum);
        ASSERT(14    == m[2].offset());
        ASSERT(16    == m.totalOffset());

        ASSERT(CHAR  == m[0].attributes()->d_elemEnum);
        ASSERT(INT   == m[1].attributes()->d_elemEnum);
        ASSERT(SHORT == m[2].attributes()->d_elemEnum);
        ASSERT(INT   == m[3].attributes()->d_elemEnum);
        ASSERT(SHORT == m[4].attributes()->d_elemEnum);

        // Let's print out the layout, which will give us "<TYPE> <OFFSET>"
        // pairs separated by '\n's:

        char buf[256];
        ostrstream out(buf, sizeof buf);
        memset(buf, 0xff, sizeof buf);  // Scribble on buf.

        m.print(out, 0, 0) << ends;

        const char *EXP1 = "{\nCHAR 0\nINT 4\nSHORT 14\nINT 8\nSHORT 12\n}\n";
        ASSERT(0 == strcmp(EXP1, buf));
        ASSERT(16 == m.totalOffset());

        m.remove(1);    // Remove element 1, which is of type 'INT'.

        ASSERT( 4 == m.length());       // one shorter
        ASSERT(16 == m.totalOffset());  // unchanged

        // Print out the layout, showing that that 'INT' is now gone, and that
        // the offsets of all the other elements are unchanged:

        memset(buf, 0xff, sizeof buf);        out.seekp(0);
        m.print(out, 0, 0) << ends;

        const char *const EXP2 = "{\nCHAR 0\nSHORT 14\nINT 8\nSHORT 12\n}\n";
        ASSERT(0 == strcmp(EXP2, buf));

        // So our contiguous memory segment is taking 16 bytes to store
        // 1 + 2 + 4 + 2 == 9 bytes of actual objects -- not very efficient.
        // Let's run 'compact' which will sort the objects in the segment by
        // non-increasing alignment requirements, but the type for any index
        // will be unchanged.

        m.compact();

        ASSERT(9 == m.totalOffset());   // now optimal
        ASSERT(4 == m.length());        // unchanged

        // Now print it out so we can see that the types are still occurring
        // in the same order, and we can look at what happened to the offsets:

        memset(buf, 0xff, sizeof buf);        out.seekp(0);
        m.print(out, 0, 0) << ends;

        const char *const EXP3 = "{\nCHAR 8\nSHORT 4\nINT 0\nSHORT 6\n}\n";
        ASSERT(0 == strcmp(EXP3, buf));

        // Observe, 'compact' left the sequence of types unchanged, but the
        // offsets are completely changed, and the ordering of the objects in
        // the segment was completely rearranged.

        // Note that specifying an index outside of the valid range will result
        // in undefined behavior.
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING VERSION 1.0
        //
        // Concerns:
        //   1. The (free) streaming operators '<<' and '>>' are implemented
        //      using the respective member functions 'bdexStreamOut' and
        //      'bdexStreamIn'.
        //   2. Streaming must be neutral to exceptions thrown as a result of
        //      either allocating memory or streaming in values.
        //   3. Ensure that streaming works under the following conditions:
        //       VALID      - may contain any sequence of valid values.
        //       EMPTY      - valid, but contains no data.
        //       INVALID    - may or may not be empty.
        //       INCOMPLETE - the stream is truncated, but otherwise valid.
        //       CORRUPTED  - the data contains explicitly inconsistent fields.
        //
        // Plan:
        //   First perform a trivial direct (breathing) test of the
        //   'bdexStreamOut' and 'bdexStreamIn' methods (to address concern 1).
        //   Note that the rest of the testing will use the stream operators.
        //
        //   Next, specify a set S of unique object values with substantial
        //   and varied differences.  For each value in S, construct an
        //   object x.
        //
        //   Note that, for performance reasons, the 'bdex' and 'bdema'
        //   exception testing are done serially rather than nested.  The
        //   code is substantially repeated for the 'bdema' exception test,
        //   but it is *not* necessary to repeat the 'ASSERT' statements.
        //
        //   VALID STREAMS (and exceptions)
        //     Using all combinations of (u, v) in S x S, stream out the value
        //     of u into a buffer and stream it back into (an independent
        //     instance of) v, and assert that u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each x in S, attempt to stream into (a temporary copy of) x
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct a truncated input stream and attempt to read into
        //     objects initialized with distinct values.  Verify values of
        //     objects that are either successfully modified or left entirely
        //     unmodified,  and that the stream becomes invalid immediately
        //     after the first incomplete read.  Finally ensure that each
        //     object streamed into is in some valid state by creating a copy
        //     and then assigning a known value to that copy; allow the
        //     original instance to leave scope without further modification,
        //     so that the destructor asserts internal object invariants
        //     appropriately.
        //
        //   CORRUPTED DATA
        //     We will assume that the incomplete test fails every field,
        //     including a char (multi-byte representation) hence we need
        //     only to produce values that are inconsistent with a valid
        //     value and verify that they are detected.  Use the underlying
        //     stream package to simulate an instance of a typical valid
        //     (control) stream and verify that it can be streamed in
        //     successfully.  Then for each data field in the stream (beginning
        //     with the version number), provide one or more similar tests with
        //     that data field corrupted.  After each test, verify that the
        //     object is in some valid state after streaming, and that the
        //     input stream has gone invalid.
        //
        //     Note that it is not possible to test corrupted lengths (i.e.,
        //     record and field counts, and lengths of record and field names).
        //     A corrupted positive length cannot be distinguished from a
        //     non-corrupted positive length.  A negative length, a definitive
        //     corruption, cannot be simulated since the bdex 'getLength'
        //     methods always return non-negative values.
        //
        // Testing:
        //   STREAM& bdexStreamIn(STREAM& stream, int version, *attrLookup[]);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting 'bdex' Streaming Version 1.0"
                                  "\n===================================="
                               << bsl::endl;

        const int VERSION = 1;

        if (verbose) bsl::cout << "\nDirect initial trial of 'bdexStreamOut' "
                                  " and (valid) 'bdexStreamIn' functionality."
                               << bsl::endl;
        {
            const Obj X(g("DCBA"), &testAllocator);
            if (veryVerbose) {
                bsl::cout << "  Value being streamed:\n"; PS(X);
            }

            Out out;  X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            In in(OD, LOD);  ASSERT(in);  ASSERT(!in.isEmpty());

            Obj t(g("ABC"), &testAllocator);

            if (veryVerbose) {
                bsl::cout << "  Value being overwritten:\n"; PS(t);
            }
            ASSERT(X != t);

            t.bdexStreamIn(in, VERSION, typesLookupTable);
            ASSERT(in);  ASSERT(in.isEmpty());

            if (veryVerbose) {
                bsl::cout << "  Value after overwrite:\n"; PS(t);
            }
            ASSERT(X == t);
        }

        if (verbose)
            bsl::cout << "\tOn valid, non-empty stream data." << bsl::endl;
        {
            static const struct {
                int           d_lineNum;  // source line number
                const char   *d_spec;     // elements we want to stream
            } DATA[] = {
                // line  spec
                // ----  ----
                {  L_,   "",                     },
                {  L_,   "A",                    },
                {  L_,   "AB",                   },
                {  L_,   "ABC",                  },
                {  L_,   "CBA",                  },
                {  L_,   "ABCD",                 },
                {  L_,   "DCBA",                 },
                {  L_,   "AEBCD",                },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE   = DATA[i].d_lineNum;
                const char *const U_SPEC = DATA[i].d_spec;
                const Obj         UU     = g(U_SPEC);               // control

                Obj u(Z);  const Obj& U = u;  gg(&u, U_SPEC);

                Out out;
                U.bdexStreamOut(out, VERSION);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Must reset stream for each iteration of inner loop.
                In testInStream(OD, LOD);  In& in = testInStream;
                in.setSuppressVersionCheck(1);
                LOOP_ASSERT(LINE, in);
                LOOP_ASSERT(LINE, !in.isEmpty());

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int         JLINE  = DATA[j].d_lineNum;
                    const char *const V_SPEC = DATA[j].d_spec;
                    const Obj         VV     = g(V_SPEC);            // control
                    const int         iEQj   = i == j;

                    if (veryVeryVerbose) { T_();  T_();  P(V_SPEC); }

                    BEGIN_BDEX_EXCEPTION_TEST {
                      in.reset();
                      LOOP2_ASSERT(LINE, JLINE, in);
                      LOOP2_ASSERT(LINE, JLINE, !in.isEmpty());

                      Obj v(Z);  const Obj& V = v;  gg(&v, V_SPEC);

                      LOOP2_ASSERT(LINE, JLINE,   UU == U);
                      LOOP2_ASSERT(LINE, JLINE,   VV == V);
                      LOOP4_ASSERT(LINE, JLINE, U, V, iEQj == (U == V));

                      v.bdexStreamIn(in, VERSION, typesLookupTable);

                      LOOP2_ASSERT(LINE, JLINE, UU == U);
                      LOOP2_ASSERT(LINE, JLINE, UU == V);
                      LOOP4_ASSERT(LINE, JLINE, U, V, U == V);

                    } END_BDEX_EXCEPTION_TEST

                    // Repeat the above streaming with 'bdema' exception-test.
                    // Note that it is *not* necessary to 'ASSERT' anything!
                    BEGIN_BSLMA_EXCEPTION_TEST {
                      in.reset();
                      const int AL = testAllocator.allocationLimit();
                      testAllocator.setAllocationLimit(-1);
                      Obj v(Z);  gg(&v, V_SPEC);

                      testAllocator.setAllocationLimit(AL);
                      v.bdexStreamIn(in, VERSION, typesLookupTable);
                    } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose)
            bsl::cout << "\tOn incomplete (but otherwise valid) data."
                      << bsl::endl;
        {
            const Obj X1 = g("AB"),   Y1 = g("C"),  Z1 = g("DE");
            const Obj X2 = g("FGH"),  Y2 = g("I"),  Z2 = g("JKLM");
            const Obj X3 = g("N"),    Y3 = g(""),   Z3 = g("OPQ");
            const Obj E(Z);  // Reference empty offset map

            Out out;
            const int LOD0 = out.length();
            Y1.bdexStreamOut(out, VERSION);
            const int LOD1 = out.length();
            Y2.bdexStreamOut(out, VERSION);
            const int LOD2 = out.length();
            Y3.bdexStreamOut(out, VERSION);
            const int LOD  = out.length();
            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                In testInStream(OD, i);  In& in = testInStream;
                in.setSuppressVersionCheck(1);
                LOOP_ASSERT(i, in);      LOOP_ASSERT(i, !i == in.isEmpty());

                if (veryVerbose) { bsl::cout << "\t\t"; P(i); }

                Obj t1(X1, Z), t2(X2, Z), t3(X3, Z);

                if (i < LOD1 - LOD0) {
                  BEGIN_BDEX_EXCEPTION_TEST {
                    in.reset();
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;  t2 = X2;  t3 = X3;

                    t1.bdexStreamIn(in, VERSION, typesLookupTable);
                       LOOP_ASSERT(i, !in);
                    t2.bdexStreamIn(in, VERSION, typesLookupTable);
                       LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X2 == t2);
                    t3.bdexStreamIn(in, VERSION, typesLookupTable);
                       LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                }
                else if (i < LOD2 - LOD0) {
                  BEGIN_BDEX_EXCEPTION_TEST {
                    in.reset();
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;  t2 = X2;  t3 = X3;

                    t1.bdexStreamIn(in, VERSION, typesLookupTable);
                       LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    t2.bdexStreamIn(in, VERSION, typesLookupTable);
                       LOOP_ASSERT(i, !in);
                    t3.bdexStreamIn(in, VERSION, typesLookupTable);
                       LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                }
                else {
                  BEGIN_BDEX_EXCEPTION_TEST {
                    in.reset();
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;  t2 = X2;  t3 = X3;

                    t1.bdexStreamIn(in, VERSION, typesLookupTable);
                       LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    t2.bdexStreamIn(in, VERSION, typesLookupTable);
                       LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y2 == t2);
                    t3.bdexStreamIn(in, VERSION, typesLookupTable);
                       LOOP_ASSERT(i, !in);
                  } END_BDEX_EXCEPTION_TEST
                }

                // Repeat the above streaming with 'bdema' exception-test.
                // Note that it is *not* necessary to 'ASSERT' anything!
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    in.reset();
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    t1 = X1;  t2 = X2;  t3 = X3;
                    testAllocator.setAllocationLimit(AL);
                    t1.bdexStreamIn(in, VERSION, typesLookupTable);
                    t2.bdexStreamIn(in, VERSION, typesLookupTable);
                    t3.bdexStreamIn(in, VERSION, typesLookupTable);
                    ASSERT(!in);
                  } END_BSLMA_EXCEPTION_TEST
                }

                Obj w1(t1), w2(t2), w3(t3);  // make copies to be sure we can

                                LOOP_ASSERT(i, Z1 != w1);
                w1 = Z1;        LOOP_ASSERT(i, Z1 == w1);

                                LOOP_ASSERT(i, Z2 != w2);
                w2 = Z2;        LOOP_ASSERT(i, Z2 == w2);

                                LOOP_ASSERT(i, Z3 != w3);
                w3 = Z3;        LOOP_ASSERT(i, Z3 == w3);
            }
        }

        if (verbose) bsl::cout << "\tOn corrupted data." << bsl::endl;
        {
            const Obj W = g("");    // default value
            const Obj X = g("DE");  // original value
            const Obj Y = g("C");   // new value

            if (verbose) bsl::cout << "\t\tGood stream (for control)."
                                   << bsl::endl;
            {
                Out out;
                out.putLength(1);
                bdem_ElemType::bdexStreamOut(out, bdem_ElemType::BDEM_INT, 1);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);   ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
                In in(OD, LOD); ASSERT(in);
                in.setSuppressVersionCheck(1);

                t.bdexStreamIn(in, VERSION, typesLookupTable);
                ASSERT(in);
                            ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y == t);
            }

            if (veryVerbose) bsl::cout << "\t\tEmpty stream test."
                                       << bsl::endl;
            {
                Obj s(Z);  gg(&s, "AB");
                In in;                       ASSERT(in);
                in.setSuppressVersionCheck(1);
                s.bdexStreamIn(in, VERSION, typesLookupTable);
                                             ASSERT(!in);
            }

            if (veryVerbose) bsl::cout << "\t\tLength only." << bsl::endl;
            {
                Obj s(Z);  gg(&s, "AB");

                Out out;
                out.putLength(1);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                In in(OD, LOD);              ASSERT(in);
                in.setSuppressVersionCheck(1);
                s.bdexStreamIn(in, VERSION, typesLookupTable);
                                             ASSERT(!in);
            }

            if (veryVerbose) bsl::cout << "\t\tInvalid 'bdem' type."
                                       << bsl::endl;
            {
                Obj s(Z);  gg(&s, "AB");

                Out out;
                out.putLength(1);
                out.putInt8(67);  // out of range

                const char *const OD  = out.data();
                const int         LOD = out.length();

                In in(OD, LOD);              ASSERT(in);
                in.setSuppressVersionCheck(1);
                s.bdexStreamIn(in, VERSION, typesLookupTable);
                                             ASSERT(!in);
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING RESETELEMTYPES METHOD
        // Concerns:
        //   We are concerned that the 'resetElemTypes' operates exactly like
        //   the 'removeAll', 'append(sa)', 'compact' methods called in
        //   sequence on the same sa.  In this sense, the results of calling
        //   this method are equivalent to appending various elements to an
        //   empty offset map and calling 'compact'.
        //
        // Plan:
        //   Use the DATA table used to test the 'compact' method.
        //   We generate a map with the designated elements inserted
        //   and invoke 'resetElemTypes' in one of three cases:
        //    1. The map whose elements are being reset is empty.
        //    2. The map whose elements are being replaced is
        //       the src array.
        //    3. The map whose elements are being reset is not empty.
        //
        // Testing:
        //   void resetElemTypes(const my_ElemTypeArray& sa);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'resetElemTypes'" << endl
                          << "========================" << endl;

        const int D_MAX_ELEM = 12;               // max elements to insert
        static const struct {
            int           d_lineNum;              // source line number
            const char   *d_spec;                 // elements we want to insert
            const char   *d_elem;                 // unique elements
            int           d_offset[D_MAX_ELEM];   // expected offset
            int           d_total;                // total offset
        } DATA[] = {
            // Note that the offsets given assume that 'compact' has been
            // run, which sorts the elements primarily by putting those with
            // the largest alignment requirements first, and secondarily by
            // putting those most recently appended first.  Note that
            // 'resetElemTypes' ends with a call to 'compact'.

            // line  spec     elem     offsets             total
            // ----  ----     ----     -------             -----
            {  L_,   "",      "",      {},                   0            },
            {  L_,   "A",     "A",     {  0 },               1            },
            {  L_,   "AB",    "AB",    {  2,  0 },           3            },
            {  L_,   "ABC",   "ABC",   {  6,  4,  0 },       7            },
            {  L_,   "CBA",   "ABC",   {  0,  4,  6 },       7            },
            {  L_,   "DCBA",  "ABCD",  {  0,  8, 12, 14 },  15             },
#if (defined(BSLS_PLATFORM__OS_LINUX) && defined(BSLS_PLATFORM__CPU_32_BIT))
            // This is a proof of the above comment.  Since the 'int' and
            // 'int64' have the same alignment on linux 32 bit, the later
            // inserted one, this time 'int64' (D), will go after 'int' (C).
            {  L_,   "ABCD",  "ABCD",  { 14, 12,  0,  4 },  15            },
            {  L_,   "CDBA",  "ABCD",  {  0,  4, 12, 14 },  15             },
#else
            {  L_,   "ABCD",  "ABCD",  { 14, 12,  8,  0 },  15            },
            {  L_,   "CDBA",  "ABCD",  {  8,  0, 12, 14 },  15             },
#endif
            {  L_,   "CCCC",  "CCCC",  {  0,  4,  8, 12 },  16             },
            {  L_,   "BBCC",  "CCBB",  {  8, 10,  0,  4 },  12             },
            {  L_,   "CCBB",  "CCBB",  {  0,  4,  8, 10 },  12             },
            {  L_,   "FAFA",  "FFAA",  {  0, 16,  8, 17 },  18             },
#if (defined(BSLS_PLATFORM__OS_LINUX) && defined(BSLS_PLATFORM__CPU_32_BIT)) \
  || defined(BSLS_PLATFORM__OS_AIX)
            // The alignment of double on ibm & 32 bit linux is 4, while on
            // other platforms it is 8.  So we have to special case ibm &
            // 32-bit linux.
            {  L_,   "FCFC",  "CFCF",  {  0,  8, 12, 20 },  24             },
#else
            {  L_,   "FCFC",  "FFCC",  {  0, 16,  8, 20 },  24             },
#endif
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ti++) {
            const int   LINE   = DATA[ti].d_lineNum;
            const char *SPEC   = DATA[ti].d_spec;
            const int  *OFFSET = DATA[ti].d_offset;
            const int   TOTAL  = DATA[ti].d_total;

            // Create an appropriate object with those elements inserted.
            Obj x; const Obj &X = x; Obj y; const Obj &Y = y;
            gg(&x, SPEC);

            // a. Reset where map was empty.
            getElemTypeArray(&eta,x);
            y.resetElemTypes(vectorData(eta), eta.size(), typesLookupTable);
            if (veryVerbose) {
                P(X);
                P(Y);
            }

            // Assert that each inserted element has now been placed at the
            // expected offset.  And the total offset is what is expected.
            for (int idx = 0; idx < (int)strlen(SPEC); idx++) {
                // Each element placed in the map
                // should have a matching expected offset.
                if (veryVerbose) {
                    cout << "\tActual offset: "   << Y[idx].offset() << endl
                         << "\tExpected offset: " << OFFSET[idx]     << endl
                         << "\tPosition: "        << idx             << endl;
                }
                LOOP3_ASSERT(LINE, Y[idx].offset(), OFFSET[idx],
                                               Y[idx].offset() == OFFSET[idx]);
            }
            // Assert we have the correct expected offset.
            LOOP_ASSERT(LINE, Y.totalOffset() == TOTAL);
            if (veryVerbose) {
                cout << "\tActual Total Offset: "   << Y.totalOffset() << endl
                     << "\tExpected Total Offset: " << TOTAL           << endl;
            }

            // b. Reset self, i.e., aliasing issues.
            getElemTypeArray(&eta,x);
            x.resetElemTypes(vectorData(eta), eta.size(), typesLookupTable);

            if (veryVerbose) P(X);

            // Assert that each inserted element has now been placed at the
            // expected offset.  And, the total offset is what is expected.
            for (int idx = 0; idx < (int)strlen(SPEC); idx++) {
                // Each element placed in the map
                // should have a matching expected offset.
                if (veryVerbose) {
                    cout << "\tActual offset: "   << X[idx].offset() << endl
                         << "\tExpected offset: " << OFFSET[idx]     << endl
                         << "\tPosition: "        << idx             << endl;
                }
                LOOP3_ASSERT(LINE, X[idx].offset(), OFFSET[idx],
                                               X[idx].offset() == OFFSET[idx]);
            }
            // Assert we have the correct expected offset.
            LOOP_ASSERT(LINE, X.totalOffset() == TOTAL);
            if (veryVerbose) {
                cout << "\tActual Total Offset: "   << X.totalOffset() << endl
                     << "\tExpected Total Offset: " << TOTAL           << endl;
            }

            // c. Reset with non-empty map.
            Obj z; const Obj &Z = z;
            // Just require that the object is non-empty.
            z.append(typesLookupTable[bdem_ElemType::BDEM_CHAR]);
            getElemTypeArray(&eta,x);
            z.resetElemTypes(vectorData(eta), eta.size(), typesLookupTable);

            if (veryVerbose) { P(X); P(Z); }

            // Assert that each inserted element has now been placed at the
            // expected offset.  And, the total offset is what is expected.
            for (int idx = 0; idx < (int)strlen(SPEC); idx++) {
                // Each element placed in the map
                // should have a matching expected offset.
                if (veryVerbose) {
                    cout << "\tActual offset: "   << Z[idx].offset() << endl
                         << "\tExpected offset: " << OFFSET[idx]     << endl
                         << "\tPosition: "        << idx             << endl;
                }
                LOOP_ASSERT(LINE, Z[idx].offset() == OFFSET[idx]);
            }
            // Assert we have the correct expected offset.
            LOOP_ASSERT(LINE, Z.totalOffset() == TOTAL);
            if (veryVerbose) {
                cout << "\tActual Total Offset: "   << Z.totalOffset() << endl
                     << "\tExpected Total Offset: " << TOTAL           << endl;
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING COMPACT METHOD
        // Concerns:
        //   1. We are concerned that the 'compact' method called after various
        //      element types have been inserted produces an appropriate
        //      minimal packing.
        //   2. The total offset should be minimal.
        //   3. The offsets for each inserted element should be such that the
        //      minimal total offset is achieved.  This is possible only if
        //      the elements are ordered from those with the largest alignment
        //      to those with the smallest.
        //
        // Plan:
        //    For each of various sets of data exhibiting specific properties,
        //    we set the desired size and alignment for each element to be
        //    inserted, insert those elements, call the 'compact' method and
        //    then verify that the results match our expected value for offsets
        //    and total offset.  The data sets are to be defined as follows:
        //
        //    a. Empty data.
        //    b. Null data (size and alignment zero).
        //    c. Ordered data non-decreasing.
        //    d. Ordered data non-increasing.
        //    e. Partially ordered data.
        //    f. Same value for all data elements.
        //
        //  By data, we mean the size and alignment as defined for a computer
        //  system as inserted into a slot in the map.
        //
        //  The test proceeds by setting up a DATA table representing the
        //  elements to be inserted, their respective size and alignments and
        //  the expected offsets after the 'compact' method is called.  The
        //  test proceeds to then set up the size and alignment for each
        //  element, invokes the generator functions to create an object which
        //  contains those elements, calls 'compact' and then verifies that
        //  each offset for each elements inserted is correct.
        //
        // Testing:
        //   void compact();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'compact'" << endl
                          << "=================" << endl;

        const int D_MAX_ELEM = 12;                // max elements to insert
        static const struct {
            int           d_lineNum;              // source line number
            const char   *d_spec;                 // elements we want to insert
            const char   *d_elem;                 // unique elements
            int           d_offset[D_MAX_ELEM];   // expected offset
            int           d_total;                // total offset
        } DATA[] = {
            // line  spec     elem     offsets             total
            // ----  ----     ----     -------             -----
            {  L_,   "",      "",      { },                  0             },
            {  L_,   "A",     "A",     {  0 },               1             },
            {  L_,   "AB",    "AB",    {  2,  0 },           3             },
            {  L_,   "ABC",   "ABC",   {  6,  4,  0 },       7             },
            {  L_,   "CBA",   "ABC",   {  0,  4,  6 },       7             },
            {  L_,   "DCBA",  "ABCD",  {  0,  8, 12, 14 },  15             },
#if (defined(BSLS_PLATFORM__OS_LINUX) && defined(BSLS_PLATFORM__CPU_32_BIT))
            // These is a proof of the above comment.  Since the 'int' and
            // 'int64' have the same alignment, the later inserted one, this
            // time 'int64' (D), will go in front of 'int' (C), as shown by the
            // offset of 0.
            {  L_,   "ABCD",  "ABCD",  { 14, 12,  0,  4 },  15             },
            {  L_,   "CDBA",  "ABCD",  {  0,  4, 12, 14 },  15             },
#else
            {  L_,   "ABCD",  "ABCD",  { 14, 12,  8,  0 },  15             },
            {  L_,   "CDBA",  "ABCD",  {  8,  0, 12, 14 },  15             },
#endif
            {  L_,   "CCCC",  "CCCC",  {  0,  4,  8, 12 },  16             },
            {  L_,   "BBCC",  "CCBB",  {  8, 10,  0,  4 },  12             },
            {  L_,   "CCBB",  "CCBB",  {  0,  4,  8, 10 },  12             },
            {  L_,   "FAFA",  "FFAA",  {  0, 16,  8, 17 },  18             },
#if (defined(BSLS_PLATFORM__OS_LINUX) && defined(BSLS_PLATFORM__CPU_32_BIT)) \
  || defined(BSLS_PLATFORM__OS_AIX)
            // The alignment of double on ibm & 32 bit linux is 4, while on
            // other platforms it is 8.  So we have to special case ibm &
            // 32-bit linux.
            {  L_,   "FCFC",  "CFCF",  {  0,  8, 12, 20 },  24             },
#else
            {  L_,   "FCFC",  "FFCC",  {  0, 16,  8, 20 },  24             },
#endif
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ti++) {
            const int   LINE   = DATA[ti].d_lineNum;
            const char *SPEC   = DATA[ti].d_spec;
            const int  *OFFSET = DATA[ti].d_offset;
            const int   TOTAL  = DATA[ti].d_total;
            if (veryVerbose) { cout << "\t\t"; P(SPEC); }

            // Create an appropriate object with those elements inserted.
            Obj x; const Obj &X = x;
            gg(&x, SPEC);

            x.compact();
            if (veryVerbose) P(X);

            // Assert that each inserted element has now been placed at the
            // expected offset.  And the total offset is what is expected.
            for (int idx = 0; idx < (int)strlen(SPEC); idx++) {
                // Each element placed in the map
                // should have a matching expected offset.
                if (veryVerbose) {
                    cout << "\tActual offset:   " << X[idx].offset() << endl
                         << "\tExpected offset: " << OFFSET[idx]     << endl
                         << "\tPosition:        " << idx             << endl;
                }
                LOOP4_ASSERT(LINE, idx, X[idx].offset(), OFFSET[idx],
                                               X[idx].offset() == OFFSET[idx]);
            }
            // Assert we have the correct expected offset.
            LOOP_ASSERT(LINE, X.totalOffset() == TOTAL);
            if (veryVerbose) {
                cout << "\tActual Total Offset: " << X.totalOffset()
                     << endl << "\tExpected Total Offset: " << TOTAL << endl;
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING CAPACITY-RESERVING CONSTRUCTOR AND METHODS
        // Concerns:
        //   The concerns are as follows:
        //    1. capacity-reserving constructor:
        //       a. The initial value is correct (empty).
        //       b. The initial capacity is correct.
        //       c. The constructor is exception neutral w.r.t. allocation.
        //       d. The internal memory management system is hooked up properly
        //          so that *all* internally allocated memory draws from a
        //          user-supplied allocator whenever one is specified.
        //    2. 'reserveCapacityRaw' method:
        //       a. The resulting value is correct (unchanged).
        //       b. The resulting capacity is correct (not less than initial).
        //       c. The method is exception neutral w.r.t. allocation.
        //    3. 'reserveCapacity' method:
        //       a. The resulting value is correct (unchanged).
        //       b. The resulting capacity is correct (not less than initial).
        //       c. The method is exception neutral w.r.t. allocation.
        //       d. The resulting value is unchanged in the event of
        //          exceptions.
        //       e. The amount of memory allocated is current * 2^N
        //
        // Plan:
        //   In a loop, use the capacity-reserving constructor to create empty
        //   objects with increasing initial capacity.  Verify that each object
        //   has the same value as a control default object.  Then, append as
        //   many values as the requested initial capacity, and use
        //   'bslma_TestAllocator' to verify that no additional allocations
        //   have occurred.  Perform each test in the standard 'bdema'
        //   exception-testing macro block.
        //
        //   Repeat the constructor test initially specifying no allocator and
        //   again, specifying a static buffer allocator.  These tests (without
        //   specifying a 'bslma_TestAllocator') cannot confirm correct
        //   capacity-reserving behavior, but can test for rudimentary correct
        //   object behavior via the destructor and Purify, and, in
        //   'veryVerbose' mode, via the print statements.
        //
        //   To test 'reserveCapacity', specify a table of initial object
        //   values and subsequent capacities to reserve.  Construct each
        //   tabulated value, call 'reserveCapacity' with the tabulated number
        //   of elements, and confirm that the test object has the same value
        //   as a separately constructed control object.  Then, append as many
        //   values as required to bring the test object's length to the
        //   specified number of elements, and use 'bslma_TestAllocator' to
        //   verify that no additional allocations have occurred.  Perform
        //   each test in the standard 'bdema' exception-testing macro block.
        //
        // Testing:
        //   bdem_RowLayout(const InitialCapacity& ne, *ba = 0);
        //
        //   void reserveCapacityRaw(int ne);
        //   void reserveCapacity(int ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Capacity-Reserving Constructor and Methods" << endl
            << "==================================================" << endl;

        if (verbose) cout <<
            "\nTesting 'bdem_RowLayout(capacity, ba)' Constructor" << endl;
        if (verbose) cout << "\twith a 'bslma_TestAllocator':" << endl;
        {
            const Obj W(&testAllocator);  // control value
            const int MAX_NUM_ELEMS = 9;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const Obj::InitialCapacity NE(ne);
                    Obj mX(NE, &testAllocator);  const Obj &X = mX;
                    LOOP_ASSERT(ne, W == X);
                    const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                    const int NUM_BYTES  = testAllocator.numBytesInUse();
                    if (veryVerbose) P_(X);
                    for (int i = 0; i < ne; ++i) {
                        mX.append(V0);
                    }
                    if (veryVerbose) P(X);
                    LOOP_ASSERT(ne, NUM_BLOCKS ==
                                              testAllocator.numBlocksTotal());
                    LOOP_ASSERT(ne, NUM_BYTES  ==
                                              testAllocator.numBytesInUse());
                  } END_BSLMA_EXCEPTION_TEST
                }
            }

            if (verbose) cout << "\twith no allocator (exercise only):"
                              << endl;
            {
                const Obj W(&testAllocator);  // control value
                const int MAX_NUM_ELEMS = 9;
                for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                    if (veryVerbose) { cout << "\t\t"; P(ne) }
                    const Obj::InitialCapacity NE(ne);
                    Obj mX(NE);  const Obj &X = mX;
                    LOOP_ASSERT(ne, W == X);
                    if (veryVerbose) P_(X);
                    for (int i = 0; i < ne; ++i) {
                        mX.append(V0);
                    }
                    if (veryVerbose) P(X);
            }
        }

        if (verbose)
            cout << "\twith a buffer allocator (exercise only):" << endl;
        {
            char memory[4096];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);
            const Obj W(&testAllocator);  // control value
            const int MAX_NUM_ELEMS = 9;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                Obj *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(NE, &a);
                Obj &mX = *doNotDelete;  const Obj &X = mX;
                LOOP_ASSERT(ne, W == X);
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.append(V0);
                }
                if (veryVerbose) P(X);
            }
            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }

        // Test 'reserveCapacityRaw' and 'reserveCapacity' methods.
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_x;        // initial value
            int         d_ne;       // total number of elements to reserve
            int         d_alloc;    // number of elements allocated
        } DATA[] = {
            //line  initialValue   numElements  Allocated
            //----  ------------   -----------  --------
            { L_,   "",              0,            0       },
            { L_,   "",              1,            8       },
            { L_,   "",              2,            16      },
            { L_,   "",              3,            24      },
            { L_,   "",              4,            32      },
            { L_,   "",              5,            40      },
            { L_,   "",              15,           120     },
            { L_,   "",              16,           128     },
            { L_,   "",              17,           136     },

            { L_,   "A",             0,            0       },
            { L_,   "A",             1,            0       },
            { L_,   "A",             2,            16      },
            { L_,   "A",             3,            24      },
            { L_,   "A",             4,            32      },
            { L_,   "A",             5,            40      },
            { L_,   "A",             15,           120     },
            { L_,   "A",             16,           128     },
            { L_,   "A",             17,           136     },

            { L_,   "AB",            0,            0       },
            { L_,   "AB",            1,            0       },
            { L_,   "AB",            2,            0       },
            { L_,   "AB",            3,            24      },
            { L_,   "AB",            4,            32      },
            { L_,   "AB",            5,            40      },
            { L_,   "AB",            15,           120     },
            { L_,   "AB",            16,           128     },
            { L_,   "AB",            17,           136     },

            { L_,   "ABCDE",         0,            0       },
            { L_,   "ABCDE",         1,            0       },
            { L_,   "ABCDE",         2,            0       },
            { L_,   "ABCDE",         3,            0       },
            { L_,   "ABCDE",         4,            0       },
            { L_,   "ABCDE",         5,            0       },
            { L_,   "ABCDE",         15,           120     },
            { L_,   "ABCDE",         16,           128     },
            { L_,   "ABCDE",         17,           136     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose)
            cout << "\nTesting the 'reserveCapacityRaw' method" << endl;
        {
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
              BEGIN_BSLMA_EXCEPTION_TEST {
                const int   LINE = DATA[ti].d_lineNum;
                const char *SPEC = DATA[ti].d_x;
                const int   NE   = DATA[ti].d_ne;
                if (veryVerbose) { cout << "\t\t"; P_(SPEC); P(NE); }

                const Obj W(g(SPEC), &testAllocator);
                Obj mX(W, &testAllocator);  const Obj &X = mX;
                mX.reserveCapacityRaw(NE);
                LOOP_ASSERT(LINE, W == X);
                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                if (veryVerbose) P_(X);
                for (int i = X.length(); i < NE; ++i) {
                    mX.append(V0);
                }
                if (veryVerbose) P(X);
                LOOP_ASSERT(LINE, NUM_BLOCKS==testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES ==testAllocator.numBytesInUse());
              } END_BSLMA_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\nTesting the 'reserveCapacity' method" << endl;
        {
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
              BEGIN_BSLMA_EXCEPTION_TEST {
                const int   LINE  = DATA[ti].d_lineNum;
                const char *SPEC  = DATA[ti].d_x;
                const int   NE    = DATA[ti].d_ne;
                const int   ALLOC = DATA[ti].d_alloc;

                const Obj W(g(SPEC), &testAllocator);
                Obj mX(W, &testAllocator); const Obj &X = mX;
                const int BT = testAllocator.numBytesTotal();

                {
                    // Verify that X doesn't change after exceptions in
                    // reserveCapacity by comparing it to the control W.
                    ASSERT(X == W);
                    mX.reserveCapacity(NE);
                }

                const int AT = testAllocator.numBytesTotal();
                if (veryVerbose) {
                    cout << "\t\t"; P_(SPEC); P_(NE); P_(ALLOC); P_(BT); P(AT);
                }

                LOOP_ASSERT(LINE, W == X);

                // Memory allocated should be as specified.
                // Note that the memory allocated is of size char and not enum
                // since the array stores data as char.
                LOOP_ASSERT(LINE, ALLOC == (AT - BT)
                                           / (1 + (8 == sizeof(void *))));

                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) P_(X);
                for (int i = X.length(); i < NE; ++i) {
                    mX.append(V0);
                }
                if (veryVerbose) P(X);
                LOOP_ASSERT(LINE, NUM_BLOCKS== testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
              } END_BSLMA_EXCEPTION_TEST
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING SWAP METHOD
        // Concerns:
        //   We are concerned that, for an object of any length, 'swap' must
        //   exchange the values at any valid pair of index positions while
        //   leaving all other elements unaffected.  Note that, upon inspecting
        //   the implementation, we are explicitly not concerned about specific
        //   element values (i.e., a single pair of test values is sufficient,
        //   and cannot mask a "stuck at" error).
        //
        // Plan:
        //   For each object X in a set of objects ordered by increasing
        //   length L containing uniform values, V0.  For each valid index
        //   position P1 in [0 .. L-1]:
        //   For each valid index position P2 in [0 .. L-1]:
        //     1.  Create a test object T from X using the copy ctor.
        //     2.  Replace the element at P1 with V1 and at P2 with V2.
        //     3.  Swap these elements in T.
        //     4.  Verify that
        //          (i)     V2 == T[P1]     always
        //     5.  Verify that
        //          (ii)    V1 == T[P2]     if (P1 != P2) V2 == T[P2]
        //                  if (P1 == P2)
        //     6.  For each index position, i, in [0 .. L-1] verify that:
        //          (iii)   V0 == T[i]      if (P1 != i && P2 != i)
        //
        // Testing:
        //   void swap(int index1, int index2);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'swap' method" << endl
                          << "=====================" << endl;

        if (verbose) cout << "\nTesting swap(index1, index2)" << endl;

        const int NUM_TRIALS = 10;

        Obj mX(&testAllocator);  const Obj& X = mX;  // control

        for (int iLen = 0; iLen < NUM_TRIALS; ++iLen) { // iLen: initial length
            if (verbose) { cout << "\t"; P_(iLen); P(X); }
            for (int pos1 = 0; pos1 < iLen; ++pos1) { // for each position
                for (int pos2 = 0; pos2 < iLen; ++pos2) { // for each position
                    Obj mT(X, &testAllocator);
                    const Obj& T = mT;            // object under test
                    LOOP3_ASSERT(iLen, pos1, pos2, X == T);

                    mT.replace(pos1, V3);  // values distinct from initial
                    mT.replace(pos2, V4);  // values distinct from initial
                    if (veryVerbose) { cout << "\t\t";  P_(pos1);
                                                        P_(pos2); P(T); }
                    mT.swap(pos1, pos2);
                    if (veryVerbose) { cout << "\t\t";  P_(pos1);
                                                        P_(pos2); P(T); }

                    LOOP3_ASSERT(iLen, pos1, pos2,
                           V4->d_elemEnum == T[pos1].attributes()->d_elemEnum);

                    const Element& VX = pos1 == pos2 ? V4 : V3;
                    LOOP3_ASSERT(iLen, pos1, pos2,
                           VX->d_elemEnum == T[pos2].attributes()->d_elemEnum);

                    for (int i = 0; i < iLen; ++i) {
                        if (i == pos1 || i == pos2) continue;
                        LOOP4_ASSERT(iLen, pos1, pos2, i,
                                     VALUES[i % 3]->d_elemEnum ==
                                                T[i].attributes()->d_elemEnum);
                    }
                }
            }
            if (veryVerbose) cout <<
            "\t--------------------------------------------------" << endl;
            mX.append(VALUES[iLen % 3]);  // Extend control with cyclic values.
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING PRINT METHOD
        // Concerns:
        //   The print method formats the value of the object directly from the
        //   underlying state information according to supplied arguments.
        //   Ensure that the method formats properly for:
        //     - empty and non-empty values
        //     - negative, 0, and positive levels.
        //     - 0 and non-zero spaces per level.
        //
        // Plan:
        //   For each of an enumerated set of object, 'level', and
        //   'spacesPerLevel' values, ordered by increasing object length, use
        //   'ostrstream' to 'print' that object's value, using the tabulated
        //   parameters, to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.
        //
        // Testing:
        //   ostream& print(ostream& os, int level, int spacesPerLevel) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'print' method" << endl
                          << "======================" << endl;

        if (verbose) cout  << "\nTesting 'print' (ostream)." << endl;
#define NL "\n"
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //ln  spec  level  SPL format   // ADJUST
                //--  ----  -----  --- -------  ------------------
                { L_, "",      0,   0, "{"              NL
                                       "}"              NL },

                { L_, "",      0,   2, "{"              NL
                                       "}"              NL },

                { L_, "",      1,   1, " {"             NL
                                       " }"             NL },

                { L_, "",      1,   2, "  {"            NL
                                       "  }"            NL  },

                { L_, "",     -1,   2, "{"              NL
                                       "  }"            NL  },

                { L_, "A",     0,   0, "{"              NL
                                       "CHAR 0"         NL
                                       "}"              NL  },

                { L_, "A",    -2,   1, "{"              NL
                                       "   CHAR 0"      NL
                                       "  }"            NL  },

                { L_, "BC",    1,   2, "  {"            NL
                                       "    SHORT 0"    NL
                                       "    INT 4"      NL
                                       "  }"            NL},

                { L_, "BC",    2,   1, "  {"            NL
                                       "   SHORT 0"     NL
                                       "   INT 4"       NL
                                       "  }"            NL},
                { L_, "BC",    5,  -1, "{ SHORT 0 INT 4 }" },
                { L_, "BC",    0,  -5, "{ SHORT 0 INT 4 }" },
                { L_, "BC",   -5,  -5, "{ SHORT 0 INT 4 }" },
                { L_, "ABCDE", 1,   3, "   {"           NL
                                       "      CHAR 0"   NL
                                       "      SHORT 2"  NL
                                       "      INT 4"    NL
                                       "      INT64 8"  NL
                                       "      FLOAT 16" NL
                                       "   }"           NL},
            };
#undef NL
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = (char) 0xff;  // Used to represent an unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         IND    = DATA[ti].d_indent;
                const int         SPL    = DATA[ti].d_spaces;
                const char *const FMT    = DATA[ti].d_fmt_p;
                const int         curLen = (int) strlen(SPEC);

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                Obj mX(&testAllocator);  const Obj& X = gg(&mX, SPEC);
                LOOP_ASSERT(ti, curLen == X.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen < curLen);  // strictly increasing
                    oldLen = curLen;
                }

                if (verbose) {
                    cout << "\t\tSpec = \"" << SPEC << "\", ";
                    P_(IND); P(SPL);
                }
                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl<<FMT<<endl;
                ostrstream out1(buf1, SIZE);  X.print(out1, IND, SPL) << ends;
                ostrstream out2(buf2, SIZE);  X.print(out2, IND, SPL) << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl<<buf1<<endl;

                const int SZ = (int) strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(ti, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING REPLACE METHODS
        // Concerns:
        //   For the 'replace' method, the following properties must hold:
        //    1. The source is left unaffected (apart from aliasing).
        //    2. The subsequent existence of the source has no effect on the
        //       result object (apart from aliasing).
        //    3. The function is alias safe.
        //    4. The function preserves object invariants.
        //
        //   In addition, when we 'replace' an element, we must
        //   assert that the map has the correct offsets for the
        //   elements in the map and that the total offset is correct.
        //
        //   Note that the 'replace' method in the rowlayout allocates
        //   a temporary map, and therefore should be tested for exception
        //   neutrality.
        //
        // Plan:
        //   Use the enumeration technique to a depth of 5 for both the normal
        //   and alias cases.  Data is tabulated explicitly for the 'replace'
        //   method that takes a range from a source array (or itself, for the
        //   aliasing test); the "scalar" 'replace' test selects a subset of
        //   the table by testing 'if (1 == NE)' where 'NE' is the tabulated
        //   number of elements to replace.
        //     - In the "canonical state" (black-box) tests, we confirm that
        //       the source is unmodified by the method call, and that its
        //       subsequent destruction has no effect on the destination
        //       object.
        //     - In all cases we want to make sure that after the application
        //       of the operation, the object is allowed to go out of scope
        //       directly to enable the destructor to assert object invariants.
        //     - Each object constructed should be wrapped in separate
        //       BSLMA test assert macros and use gg as an optimization.
        //     - At every test for 'replace' we check that the map
        //       has the correct offset for each element replaced and
        //       the correct total offset.
        //
        // Testing:
        //   void replace(int di, bdem_ElemType::Type item);
        //   void replace(int di, const my_ElemTypeArray& sa,
        //                int si, int ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'replace'" << endl
                          << "=================" << endl;

        // void replace(int  di, const &srcMap, int si, int ne);
        if (verbose) cout <<
            "\nTesting replace(di, sa, si, ne) et al. (no aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_daSpec;   // initial (destination) array
                int         d_di;       // index at which to replace into da
                const char *d_saSpec;   // source array
                int         d_si;       // index at which to replace from sa
                int         d_ne;       // number of elements to replace
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 0
                { L_,   "",      0,  "",      0,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 1
                { L_,   "A",     0,  "",      0,  0, "A"     },
                { L_,   "A",     1,  "",      0,  0, "A"     },

                { L_,   "",      0,  "B",     0,  0, ""      },
                { L_,   "",      0,  "B",     1,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 2
                { L_,   "AB",    0,  "",      0,  0, "AB"    },
                { L_,   "AB",    1,  "",      0,  0, "AB"    },
                { L_,   "AB",    2,  "",      0,  0, "AB"    },

                { L_,   "A",     0,  "B",     0,  0, "A"     },
                { L_,   "A",     0,  "B",     0,  1, "B"     },
                { L_,   "A",     0,  "B",     1,  0, "A"     },
                { L_,   "A",     1,  "B",     0,  0, "A"     },
                { L_,   "A",     1,  "B",     1,  0, "A"     },

                { L_,   "",      0,  "AB",    0,  0, ""      },
                { L_,   "",      0,  "AB",    1,  0, ""      },
                { L_,   "",      0,  "AB",    2,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 3
                { L_,   "ABC",   0,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   1,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   2,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   3,  "",      0,  0, "ABC"   },

                { L_,   "AB",    0,  "C",     0,  0, "AB"    },
                { L_,   "AB",    0,  "C",     0,  1, "CB"    },
                { L_,   "AB",    0,  "C",     1,  0, "AB"    },
                { L_,   "AB",    1,  "C",     0,  0, "AB"    },
                { L_,   "AB",    1,  "C",     0,  1, "AC"    },
                { L_,   "AB",    1,  "C",     1,  0, "AB"    },
                { L_,   "AB",    2,  "C",     0,  0, "AB"    },
                { L_,   "AB",    2,  "C",     1,  0, "AB"    },

                { L_,   "A",     0,  "BC",    0,  0, "A"     },
                { L_,   "A",     0,  "BC",    0,  1, "B"     },
                { L_,   "A",     0,  "BC",    1,  0, "A"     },
                { L_,   "A",     0,  "BC",    1,  1, "C"     },
                { L_,   "A",     0,  "BC",    2,  0, "A"     },
                { L_,   "A",     1,  "BC",    0,  0, "A"     },
                { L_,   "A",     1,  "BC",    1,  0, "A"     },
                { L_,   "A",     1,  "BC",    2,  0, "A"     },

                { L_,   "",      0,  "ABC",   0,  0, ""      },
                { L_,   "",      0,  "ABC",   1,  0, ""      },
                { L_,   "",      0,  "ABC",   2,  0, ""      },
                { L_,   "",      0,  "ABC",   3,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 4
                { L_,   "ABCD",  0,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  1,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  2,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  3,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  4,  "",      0,  0, "ABCD"  },

                { L_,   "ABC",   0,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   0,  "D",     0,  1, "DBC"   },
                { L_,   "ABC",   0,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  1, "ADC"   },
                { L_,   "ABC",   1,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  1, "ABD"   },
                { L_,   "ABC",   2,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   3,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   3,  "D",     1,  0, "ABC"   },

                { L_,   "AB",    0,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    0,  "CD",    0,  1, "CB"    },
                { L_,   "AB",    0,  "CD",    0,  2, "CD"    },
                { L_,   "AB",    0,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    0,  "CD",    1,  1, "DB"    },
                { L_,   "AB",    0,  "CD",    2,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    0,  1, "AC"    },
                { L_,   "AB",    1,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    1,  1, "AD"    },
                { L_,   "AB",    1,  "CD",    2,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    2,  0, "AB"    },

                { L_,   "A",     0,  "BCD",   0,  0, "A"     },
                { L_,   "A",     0,  "BCD",   0,  1, "B"     },
                { L_,   "A",     0,  "BCD",   1,  0, "A"     },
                { L_,   "A",     0,  "BCD",   1,  1, "C"     },
                { L_,   "A",     0,  "BCD",   2,  0, "A"     },
                { L_,   "A",     0,  "BCD",   2,  1, "D"     },
                { L_,   "A",     0,  "BCD",   3,  0, "A"     },
                { L_,   "A",     1,  "BCD",   0,  0, "A"     },
                { L_,   "A",     1,  "BCD",   1,  0, "A"     },
                { L_,   "A",     1,  "BCD",   2,  0, "A"     },
                { L_,   "A",     1,  "BCD",   3,  0, "A"     },

                { L_,   "",      0,  "ABCD",  0,  0, ""      },
                { L_,   "",      0,  "ABCD",  1,  0, ""      },
                { L_,   "",      0,  "ABCD",  2,  0, ""      },
                { L_,   "",      0,  "ABCD",  3,  0, ""      },
                { L_,   "",      0,  "ABCD",  4,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 1,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 2,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 3,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 4,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 5,  "",      0,  0, "ABCDE" },

                { L_,   "ABCD",  0,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  0,  "E",     0,  1, "EBCD" },
                { L_,   "ABCD",  0,  "E",     1,  0, "ABCD" },
                { L_,   "ABCD",  1,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  1,  "E",     0,  1, "AECD" },
                { L_,   "ABCD",  1,  "E",     1,  0, "ABCD" },
                { L_,   "ABCD",  2,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  2,  "E",     0,  1, "ABED" },
                { L_,   "ABCD",  2,  "E",     1,  0, "ABCD" },
                { L_,   "ABCD",  3,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  3,  "E",     0,  1, "ABCE" },
                { L_,   "ABCD",  3,  "E",     1,  0, "ABCD" },
                { L_,   "ABCD",  4,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  4,  "E",     1,  0, "ABCD" },

                { L_,   "ABC",   0,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   0,  "DE",    0,  1, "DBC"   },
                { L_,   "ABC",   0,  "DE",    0,  2, "DEC"   },
                { L_,   "ABC",   0,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   0,  "DE",    1,  1, "EBC"   },
                { L_,   "ABC",   0,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  1, "ADC"   },
                { L_,   "ABC",   1,  "DE",    0,  2, "ADE"   },
                { L_,   "ABC",   1,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    1,  1, "AEC"   },
                { L_,   "ABC",   1,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  1, "ABD"   },
                { L_,   "ABC",   2,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    1,  1, "ABE"   },
                { L_,   "ABC",   2,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    2,  0, "ABC"   },

                { L_,   "AB",    0,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   0,  1, "CB"    },
                { L_,   "AB",    0,  "CDE",   0,  2, "CD"    },
                { L_,   "AB",    0,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   1,  1, "DB"    },
                { L_,   "AB",    0,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   2,  1, "EB"    },
                { L_,   "AB",    0,  "CDE",   3,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  1, "AC"    },
                { L_,   "AB",    1,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   1,  1, "AD"    },
                { L_,   "AB",    1,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   2,  1, "AE"    },
                { L_,   "AB",    1,  "CDE",   3,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   3,  0, "AB"    },

                { L_,   "A",     0,  "BCDE",  0,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  0,  1, "B"     },
                { L_,   "A",     0,  "BCDE",  1,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  1,  1, "C"     },
                { L_,   "A",     0,  "BCDE",  2,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  2,  1, "D"     },
                { L_,   "A",     0,  "BCDE",  3,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  3,  1, "E"     },
                { L_,   "A",     0,  "BCDE",  4,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  0,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  1,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  2,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  3,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  4,  0, "A"     },

                { L_,   "",      0,  "ABCDE", 0,  0, ""      },
                { L_,   "",      0,  "ABCDE", 1,  0, ""      },
                { L_,   "",      0,  "ABCDE", 2,  0, ""      },
                { L_,   "",      0,  "ABCDE", 3,  0, ""      },
                { L_,   "",      0,  "ABCDE", 4,  0, ""      },
                { L_,   "",      0,  "ABCDE", 5,  0, ""      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_daSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_saSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = (int)strlen(D_SPEC) + (int)strlen(S_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Obj DD(g(D_SPEC));   // control for destination
                Obj SS(g(S_SPEC));   // control for source
                Obj EE(g(E_SPEC));   // control for expected result

                if (veryVerbose) {
                    cout << "\t  =================================="
                            "==================================" << endl;
                    cout << "\t  ";
                    P_(D_SPEC); P_(DI); P_(S_SPEC);
                    P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(SS);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                // void replace(int di, bdem_ElemType::Type item);
                if (veryVerbose) cout << "\t\treplace(di, item)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) {
                            cout << "\t\t\tBEFORE: "; P(X);
                        }
                        int old_offset = x.totalOffset();
                        x.replace(DI,
                             typesLookupTable[s[SI].attributes()->d_elemEnum]);
                                                             // src non-'const'
                        if (veryVerbose) {
                            cout << "\t\t\t AFTER: "; P(X);
                        }
                        if (EE != X) {
                            // Check to see we have the expected type.
                            LOOP_ASSERT(LINE, X[DI].attributes()->d_elemEnum ==
                                               S[SI].attributes()->d_elemEnum);
                            // Check to see we have the expected offset.
                            const int align =
                                       typesLookupTable[S[SI].attributes()->
                                                      d_elemEnum]->d_alignment;
                            const int size =
                                       typesLookupTable[S[SI].attributes()->
                                                           d_elemEnum]->d_size;
                            int new_offset  =
                                       (old_offset + align - 1) & ~(align - 1);
                            if (veryVerbose) {
                                cout << "Expected Offset: "
                                     << new_offset
                                     << endl
                                     << "Actual Offset: "
                                     << X[DI].offset()
                                     << endl;
                            }
                            LOOP_ASSERT(LINE, X[DI].offset() == new_offset);
                            int new_totalOffset = new_offset + size;
                            if (veryVerbose) {
                                cout << "Expected Total Offset: "
                                     << new_totalOffset
                                     << endl
                                     << "Actual Total Offset: "
                                     << X.totalOffset()
                                     << endl;
                            }
                            LOOP_ASSERT(LINE,
                                           X.totalOffset() == new_totalOffset);
                        }
                        LOOP_ASSERT(LINE, SS == S);  // source unchanged?
                    }
                                                     // source is out of scope
                    getElemTypeArray(&eta,EE);
                    getElemTypeArray(&eta2,X);
                    LOOP_ASSERT(LINE, eta == eta2);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;
                const int STRETCH_SIZE = 50;
                stretchRemoveAll(&x, STRETCH_SIZE);

                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();

                // void replace(int di, bdem_ElemType::Type item);
                if (veryVerbose) cout << "\t\treplace(di, item)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    int old_offset = x.totalOffset();
                    x.replace(DI,
                            typesLookupTable[SS[SI].attributes()->d_elemEnum]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    if (EE != x) {
                        // Check to see we have the expected type.
                        LOOP_ASSERT(LINE, x[DI].attributes()->d_elemEnum ==
                                              SS[SI].attributes()->d_elemEnum);
                        // Check to see we have the expected offset.
                        const int align =
                                      typesLookupTable[SS[SI].attributes()->
                                                      d_elemEnum]->d_alignment;
                        const int size =
                                      typesLookupTable[SS[SI].attributes()->
                                                           d_elemEnum]->d_size;
                        int new_offset =
                                       (old_offset + align - 1) & ~(align - 1);
                        if (veryVerbose) {
                            cout << "Expected Offset: "
                                 << new_offset
                                 << endl
                                 << "Actual Offset: "
                                 << x[DI].offset()
                                 << endl;
                        }
                        LOOP_ASSERT(LINE, x[DI].offset() == new_offset);
                        int new_totalOffset = new_offset + size;
                        if (veryVerbose) {
                            cout << "Expected Total Offset: "
                                 << new_totalOffset
                                 << endl
                                 << "Actual Total Offset: "
                                 << x.totalOffset()
                                 << endl;
                        }
                        LOOP_ASSERT(LINE, x.totalOffset() == new_totalOffset);
                    }
                }
                LOOP_ASSERT(LINE, NUM_BLOCKS ==
                                             testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES  ==
                                             testAllocator.numBytesInUse());
            }
        }

        // --------------------------------------------------------------------
        // void replace(int di, const &srcMap, int si, int ne);
        if (verbose) cout <<
            "\nTesting x.replace(di, sa, si, ne) et al. (aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial array (= sa)
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 0
                { L_,   "",      0,  0,  0,  ""      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 1
                { L_,   "A",     0,  0,  0,  "A"     },
                { L_,   "A",     0,  0,  1,  "A"     },
                { L_,   "A",     0,  1,  0,  "A"     },

                { L_,   "A",     1,  0,  0,  "A"     },
                { L_,   "A",     1,  1,  0,  "A"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 2
                { L_,   "AB",    0,  0,  0,  "AB"    },
                { L_,   "AB",    0,  0,  1,  "AB"    },
                { L_,   "AB",    0,  0,  2,  "AB"    },
                { L_,   "AB",    0,  1,  0,  "AB"    },
                { L_,   "AB",    0,  1,  1,  "BB"    },
                { L_,   "AB",    0,  2,  0,  "AB"    },

                { L_,   "AB",    1,  0,  0,  "AB"    },
                { L_,   "AB",    1,  0,  1,  "AA"    },
                { L_,   "AB",    1,  1,  0,  "AB"    },
                { L_,   "AB",    1,  1,  1,  "AB"    },
                { L_,   "AB",    1,  2,  0,  "AB"    },

                { L_,   "AB",    2,  0,  0,  "AB"    },
                { L_,   "AB",    2,  1,  0,  "AB"    },
                { L_,   "AB",    2,  2,  0,  "AB"    },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 3
                { L_,   "ABC",   0,  0,  0,  "ABC"   },
                { L_,   "ABC",   0,  0,  1,  "ABC"   },
                { L_,   "ABC",   0,  0,  2,  "ABC"   },
                { L_,   "ABC",   0,  0,  3,  "ABC"   },
                { L_,   "ABC",   0,  1,  0,  "ABC"   },
                { L_,   "ABC",   0,  1,  1,  "BBC"   },
                { L_,   "ABC",   0,  1,  2,  "BCC"   },
                { L_,   "ABC",   0,  2,  0,  "ABC"   },
                { L_,   "ABC",   0,  2,  1,  "CBC"   },
                { L_,   "ABC",   0,  3,  0,  "ABC"   },

                { L_,   "ABC",   1,  0,  0,  "ABC"   },
                { L_,   "ABC",   1,  0,  1,  "AAC"   },
                { L_,   "ABC",   1,  0,  2,  "AAB"   },
                { L_,   "ABC",   1,  1,  0,  "ABC"   },
                { L_,   "ABC",   1,  1,  1,  "ABC"   },
                { L_,   "ABC",   1,  1,  2,  "ABC"   },
                { L_,   "ABC",   1,  2,  0,  "ABC"   },
                { L_,   "ABC",   1,  2,  1,  "ACC"   },
                { L_,   "ABC",   1,  3,  0,  "ABC"   },

                { L_,   "ABC",   2,  0,  0,  "ABC"   },
                { L_,   "ABC",   2,  0,  1,  "ABA"   },
                { L_,   "ABC",   2,  1,  0,  "ABC"   },
                { L_,   "ABC",   2,  1,  1,  "ABB"   },
                { L_,   "ABC",   2,  2,  0,  "ABC"   },
                { L_,   "ABC",   2,  2,  1,  "ABC"   },
                { L_,   "ABC",   2,  3,  0,  "ABC"   },

                { L_,   "ABC",   3,  0,  0,  "ABC"   },
                { L_,   "ABC",   3,  1,  0,  "ABC"   },
                { L_,   "ABC",   3,  2,  0,  "ABC"   },
                { L_,   "ABC",   3,  3,  0,  "ABC"   },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 4
                { L_,   "ABCD",  0,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  0,  0,  1,  "ABCD"  },
                { L_,   "ABCD",  0,  0,  2,  "ABCD"  },
                { L_,   "ABCD",  0,  0,  3,  "ABCD"  },
                { L_,   "ABCD",  0,  0,  4,  "ABCD"  },
                { L_,   "ABCD",  0,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  0,  1,  1,  "BBCD"  },
                { L_,   "ABCD",  0,  1,  2,  "BCCD"  },
                { L_,   "ABCD",  0,  1,  3,  "BCDD"  },
                { L_,   "ABCD",  0,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  0,  2,  1,  "CBCD"  },
                { L_,   "ABCD",  0,  2,  2,  "CDCD"  },
                { L_,   "ABCD",  0,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  0,  3,  1,  "DBCD"  },
                { L_,   "ABCD",  0,  4,  0,  "ABCD"  },

                { L_,   "ABCD",  1,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  0,  1,  "AACD"  },
                { L_,   "ABCD",  1,  0,  2,  "AABD"  },
                { L_,   "ABCD",  1,  0,  3,  "AABC"  },
                { L_,   "ABCD",  1,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  1,  1,  "ABCD"  },
                { L_,   "ABCD",  1,  1,  2,  "ABCD"  },
                { L_,   "ABCD",  1,  1,  3,  "ABCD"  },
                { L_,   "ABCD",  1,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  2,  1,  "ACCD"  },
                { L_,   "ABCD",  1,  2,  2,  "ACDD"  },
                { L_,   "ABCD",  1,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  3,  1,  "ADCD"  },
                { L_,   "ABCD",  1,  4,  0,  "ABCD"  },

                { L_,   "ABCD",  2,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  0,  1,  "ABAD"  },
                { L_,   "ABCD",  2,  0,  2,  "ABAB"  },
                { L_,   "ABCD",  2,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  1,  1,  "ABBD"  },
                { L_,   "ABCD",  2,  1,  2,  "ABBC"  },
                { L_,   "ABCD",  2,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  2,  1,  "ABCD"  },
                { L_,   "ABCD",  2,  2,  2,  "ABCD"  },
                { L_,   "ABCD",  2,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  3,  1,  "ABDD"  },
                { L_,   "ABCD",  2,  4,  0,  "ABCD"  },

                { L_,   "ABCD",  3,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  0,  1,  "ABCA"  },
                { L_,   "ABCD",  3,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  1,  1,  "ABCB"  },
                { L_,   "ABCD",  3,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  2,  1,  "ABCC"  },
                { L_,   "ABCD",  3,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  3,  1,  "ABCD"  },
                { L_,   "ABCD",  3,  4,  0,  "ABCD"  },

                { L_,   "ABCD",  4,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  4,  0,  "ABCD"  },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  1,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  2,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  3,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  4,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  5,  "ABCDE" },
                { L_,   "ABCDE", 0,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  1,  1,  "BBCDE" },
                { L_,   "ABCDE", 0,  1,  2,  "BCCDE" },
                { L_,   "ABCDE", 0,  1,  3,  "BCDDE" },
                { L_,   "ABCDE", 0,  1,  4,  "BCDEE" },
                { L_,   "ABCDE", 0,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  2,  1,  "CBCDE" },
                { L_,   "ABCDE", 0,  2,  2,  "CDCDE" },
                { L_,   "ABCDE", 0,  2,  3,  "CDEDE" },
                { L_,   "ABCDE", 0,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  3,  1,  "DBCDE" },
                { L_,   "ABCDE", 0,  3,  2,  "DECDE" },
                { L_,   "ABCDE", 0,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  4,  1,  "EBCDE" },
                { L_,   "ABCDE", 0,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 1,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  0,  1,  "AACDE" },
                { L_,   "ABCDE", 1,  0,  2,  "AABDE" },
                { L_,   "ABCDE", 1,  0,  3,  "AABCE" },
                { L_,   "ABCDE", 1,  0,  4,  "AABCD" },
                { L_,   "ABCDE", 1,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  1,  1,  "ABCDE" },
                { L_,   "ABCDE", 1,  1,  2,  "ABCDE" },
                { L_,   "ABCDE", 1,  1,  3,  "ABCDE" },
                { L_,   "ABCDE", 1,  1,  4,  "ABCDE" },
                { L_,   "ABCDE", 1,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  2,  1,  "ACCDE" },
                { L_,   "ABCDE", 1,  2,  2,  "ACDDE" },
                { L_,   "ABCDE", 1,  2,  3,  "ACDEE" },
                { L_,   "ABCDE", 1,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  3,  1,  "ADCDE" },
                { L_,   "ABCDE", 1,  3,  2,  "ADEDE" },
                { L_,   "ABCDE", 1,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  4,  1,  "AECDE" },
                { L_,   "ABCDE", 1,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 2,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  0,  1,  "ABADE" },
                { L_,   "ABCDE", 2,  0,  2,  "ABABE" },
                { L_,   "ABCDE", 2,  0,  3,  "ABABC" },
                { L_,   "ABCDE", 2,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  1,  1,  "ABBDE" },
                { L_,   "ABCDE", 2,  1,  2,  "ABBCE" },
                { L_,   "ABCDE", 2,  1,  3,  "ABBCD" },
                { L_,   "ABCDE", 2,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  2,  1,  "ABCDE" },
                { L_,   "ABCDE", 2,  2,  2,  "ABCDE" },
                { L_,   "ABCDE", 2,  2,  3,  "ABCDE" },
                { L_,   "ABCDE", 2,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  3,  1,  "ABDDE" },
                { L_,   "ABCDE", 2,  3,  2,  "ABDEE" },
                { L_,   "ABCDE", 2,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  4,  1,  "ABEDE" },
                { L_,   "ABCDE", 2,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 3,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  0,  1,  "ABCAE" },
                { L_,   "ABCDE", 3,  0,  2,  "ABCAB" },
                { L_,   "ABCDE", 3,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  1,  1,  "ABCBE" },
                { L_,   "ABCDE", 3,  1,  2,  "ABCBC" },
                { L_,   "ABCDE", 3,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  2,  1,  "ABCCE" },
                { L_,   "ABCDE", 3,  2,  2,  "ABCCD" },
                { L_,   "ABCDE", 3,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  3,  1,  "ABCDE" },
                { L_,   "ABCDE", 3,  3,  2,  "ABCDE" },
                { L_,   "ABCDE", 3,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  4,  1,  "ABCEE" },
                { L_,   "ABCDE", 3,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 4,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  0,  1,  "ABCDA" },
                { L_,   "ABCDE", 4,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  1,  1,  "ABCDB" },
                { L_,   "ABCDE", 4,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  2,  1,  "ABCDC" },
                { L_,   "ABCDE", 4,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  3,  1,  "ABCDD" },
                { L_,   "ABCDE", 4,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  4,  1,  "ABCDE" },
                { L_,   "ABCDE", 4,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 5,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  5,  0,  "ABCDE" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = (int) strlen(X_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Obj DD(g(X_SPEC));         // control for destination
                const Obj SS(g(X_SPEC));   // control for source
                Obj EE(g(E_SPEC));         // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                            "==================================" << endl;
                    cout << "\t  "; P_(X_SPEC); P_(DI);
                                    P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                // void replace(int di, bdem_ElemType::Type item);
                if (veryVerbose) cout << "\t\treplace(di, item)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.replace(DI,
                             typesLookupTable[X[SI].attributes()->d_elemEnum]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    if (EE != x) {
                        // Check to see we have the expected type.
                        LOOP_ASSERT(LINE, x[DI].attributes()->d_elemEnum ==
                                               X[SI].attributes()->d_elemEnum);
                        // Check to see we have the expected offset.
                        const int align =
                                       typesLookupTable[X[SI].attributes()->
                                                      d_elemEnum]->d_alignment;
                        const int size =
                                       typesLookupTable[X[SI].attributes()->
                                                           d_elemEnum]->d_size;
                        int new_offset =
                                 (SS.totalOffset() + align - 1) & ~(align - 1);
                        if (veryVerbose) {
                            cout << "Expected Offset: "
                                 << new_offset
                                  << endl
                                  << "Actual Offset: "
                                  << x[DI].offset()
                                  << endl;
                        }
                        LOOP_ASSERT(LINE, x[DI].offset() == new_offset);
                        int new_totalOffset = new_offset + size;
                        if (veryVerbose) {
                            cout << "Expected Total Offset: "
                                 << new_totalOffset
                                 << endl
                                 << "Actual Total Offset: "
                                 << x.totalOffset()
                                 << endl;
                        }
                        LOOP_ASSERT(LINE, x.totalOffset() == new_totalOffset);
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                // void replace(int di, bdem_ElemType::Type item);
                if (veryVerbose) cout << "\t\treplace(di, item)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.replace(DI,
                             typesLookupTable[X[SI].attributes()->d_elemEnum]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    if (EE != x) {
                        // Check to see we have the expected type.
                        LOOP_ASSERT(LINE, x[DI].attributes()->d_elemEnum ==
                                               X[SI].attributes()->d_elemEnum);
                        // Check to see we have the expected offset.
                        const int align =
                                       typesLookupTable[X[SI].attributes()->
                                                      d_elemEnum]->d_alignment;
                        const int size =
                                       typesLookupTable[X[SI].attributes()->
                                                           d_elemEnum]->d_size;
                        int new_offset =
                                 (SS.totalOffset() + align - 1) & ~(align - 1);
                        if (veryVerbose) {
                            cout << "Expected Offset: "
                                 << new_offset
                                 << endl
                                 << "Actual Offset: "
                                 << x[DI].offset()
                                 << endl;
                        }
                        LOOP_ASSERT(LINE, x[DI].offset() == new_offset);
                        int new_totalOffset = new_offset + size;
                        if (veryVerbose) {
                            cout << "Expected Total Offset: "
                                 << new_totalOffset
                                 << endl
                                 << "Actual Total Offset: "
                                 << x.totalOffset()
                                 << endl;
                        }
                        LOOP_ASSERT(LINE, x.totalOffset() == new_totalOffset);
                    }
                }
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING APPEND, INSERT, REMOVE METHODS
        // Concerns:
        //   For the 'append' and 'insert' methods that have the same
        //   contract as the arrays, the following properties must hold:
        //
        //   1. The source is left unaffected (apart from aliasing).
        //   2. The subsequent existing of the source has no effect on the
        //      result object (apart from aliasing).
        //   3. The function is alias safe.
        //   4. The function is exception neutral (w.r.t. allocation).
        //   5. The function preserves object invariants.
        //   6. The function is independent of internal representation.
        //
        //   In addition, when appends, inserts and removes occur,
        //   we must assert that the map has the correct offsets
        //   for the elements added and/or remaining, and that the total
        //   offset is correct.
        //
        //   Note that all (contingent) reallocations occur strictly before
        //   the essential implementation of each method.  Therefore,
        //   concerns 1, 2, and 4 above are valid for objects in the
        //   "canonical state", but need not be repeated when concern 6
        //   ("white-box test") is addressed.
        //
        //   We are also concerned with the following for all
        //   insert and append methods that operate by inserting or appending
        //   a single element type.
        //
        //    a. Insert/Append place an element at the designated
        //       index of the specified bdem_ElemType.
        //    b. After Insert/Append, we have the correct offset for
        //       the item inserted given varying initial cursor positions.
        //    c. After Insert/Append, we have the correct total offset
        //       given varying initial cursor positions.
        //    d. That we have capacity increase as needed.
        //
        //   For the 'remove' methods, the concerns are to cover the
        //   full range of possible indices and numbers of elements
        //   and to assert that we have the correct offsets and total offset.
        //
        // Plan:
        //   Use the enumeration technique to a depth of 5 for both the normal
        //   and alias cases.  Data is tabulated explicitly for the 'insert'
        //   method that takes a range from a source array (or itself, for the
        //   aliasing test); other methods are tested using a subset of the
        //   full test vector table.  In particular, the 'append' methods use
        //   data where the destination index equals the destination length
        //   ((int) strlen(D_SPEC) == DI).  All methods using the entire source
        //   object use test data where the source length equals the number of
        //   elements ((int) strlen(S_SPEC) == NE), while the "scalar" methods
        //   use data where the number of elements equals 1 (1 == NE).  In
        //   addition, the 'remove' methods switch the "d-array" and "expected"
        //   values from the 'insert' table.
        //     - In the "canonical state" (black-box) tests, we confirm that
        //       the source is unmodified by the method call, and that its
        //       subsequent destruction has no effect on the destination
        //       object.
        //     - In all cases we want to make sure that after the application
        //       of the operation, the object is allowed to go out of scope
        //       directly to enable the destructor to assert object invariants.
        //     - Each object constructed should be wrapped in separate
        //       BSLMA test assert macros and use gg as an optimization.
        //     - At every test for insert, append and remove, we check
        //       that the map has the correct offset for each element
        //       appended, inserted or remaining in the map and that we have
        //       the correct total offset.
        //
        //   NOTE:
        //   The following pair of member functions:
        //    void insert(int di, const &srcMap);
        //    void insert(int di, const &srcMap, int si, int ne);
        //   are implemented in terms of the following pair:
        //    void insert(int di, const my_ElemTypeArray& sa);
        //    void insert(int di, const my_ElemTypeArray& sa,
        //                int si, int ne);
        //
        //   Therefore, testing the former set implicitly tests the latter.
        //
        //   Similarly for the following pairs of append functions:
        //    void append(const bdem_RowLayout<T>& srcMap, int si, int ne);
        //    void append(const bdem_RowLayout<T>& srcMap);
        //
        //    void append(const my_ElemTypeArray& sa, int si, int ne);
        //    void append(const my_ElemTypeArray& sa);
        //
        //   With respect to append/insert member functions that operate
        //   on individual element types, we test each append and insert
        //   using a table containing initial cursor position and expected
        //   final offset and cursor position.  In a loop, we vary the initial
        //   cursor position of the map and append or insert the element type.
        //   We then:
        //     - Verify that the appropriate element was inserted in
        //       the correct position.
        //     - Verify that it has the correct offset based on size
        //       and alignment.
        //     - Verify that we have the correct total offset for
        //       'bdem_RowLayout'.
        //
        // Testing:
        //   void append(bdem_ElemType::Type item);
        //   void append(const my_ElemTypeArray& sa);
        //   void append(const my_ElemTypeArray& sa, int si, int ne);
        //
        //   void insert(int di, bdem_ElemType::Type item);
        //   void insert(int di, const my_ElemTypeArray& sa);
        //   void insert(int di, const my_ElemTypeArray& sa, int si, int ne);
        //
        //   void remove(int index);
        //   void remove(int index, int ne);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "Testing 'append', 'insert', and 'remove'\n"
                    "========================================\n";
        }

        if (verbose) cout <<
            "\nTesting x.insert(di, sa, si, ne) et al. (no aliasing)"
            << endl;
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_daSpec;   // initial (destination) array
            int         d_di;       // index at which to insert into da
            const char *d_saSpec;   // source array
            int         d_si;       // index at which to insert from sa
            int         d_ne;       // number of elements to insert
            const char *d_expSpec;  // expected array value
        } DATA[] = {
            //line  d-array di   s-array si  ne  expected
            //----  ------- --   ------- --  --  --------   Depth = 0
            { L_,   "",      0,  "",      0,  0, ""      },

            //line  d-array di   s-array si  ne  expected
            //----  ------- --   ------- --  --  --------   Depth = 1
            { L_,   "A",     0,  "",      0,  0, "A"     },
            { L_,   "A",     1,  "",      0,  0, "A"     },

            { L_,   "",      0,  "A",     0,  0, ""      },
            { L_,   "",      0,  "A",     0,  1, "A"     },
            { L_,   "",      0,  "A",     1,  0, ""      },

            //line  d-array di   s-array si  ne  expected
            //----  ------- --   ------- --  --  --------   Depth = 2
            { L_,   "AB",    0,  "",      0,  0, "AB"    },
            { L_,   "BA",    0,  "",      0,  0, "BA"    },
            { L_,   "AB",    1,  "",      0,  0, "AB"    },
            { L_,   "AB",    2,  "",      0,  0, "AB"    },

            { L_,   "A",     0,  "B",     0,  0, "A"     },
            { L_,   "A",     0,  "B",     0,  1, "BA"    },
            { L_,   "A",     0,  "B",     1,  0, "A"     },
            { L_,   "A",     1,  "B",     0,  0, "A"     },
            { L_,   "A",     1,  "B",     0,  1, "AB"    },
            { L_,   "A",     1,  "B",     1,  0, "A"     },

            { L_,   "",      0,  "AB",    0,  0, ""      },
            { L_,   "",      0,  "AB",    0,  1, "A"     },
            { L_,   "",      0,  "AB",    0,  2, "AB"    },
            { L_,   "",      0,  "AB",    1,  0, ""      },
            { L_,   "",      0,  "AB",    1,  1, "B"     },
            { L_,   "",      0,  "AB",    2,  0, ""      },

            //line  d-array di   s-array si  ne  expected
            //----  ------- --   ------- --  --  --------   Depth = 3
            { L_,   "ABC",   0,  "",      0,  0, "ABC"   },
            { L_,   "ABC",   1,  "",      0,  0, "ABC"   },
            { L_,   "ABC",   2,  "",      0,  0, "ABC"   },
            { L_,   "ABC",   3,  "",      0,  0, "ABC"   },

            { L_,   "AB",    0,  "C",     0,  0, "AB"    },
            { L_,   "AB",    0,  "C",     0,  1, "CAB"   },
            { L_,   "AB",    0,  "C",     1,  0, "AB"    },
            { L_,   "AB",    1,  "C",     0,  0, "AB"    },
            { L_,   "AB",    1,  "C",     0,  1, "ACB"   },
            { L_,   "AB",    1,  "C",     1,  0, "AB"    },
            { L_,   "AB",    2,  "C",     0,  0, "AB"    },
            { L_,   "AB",    2,  "C",     0,  1, "ABC"   },
            { L_,   "AB",    2,  "C",     1,  0, "AB"    },

            { L_,   "A",     0,  "BC",    0,  0, "A"     },
            { L_,   "A",     0,  "BC",    0,  1, "BA"    },
            { L_,   "A",     0,  "BC",    0,  2, "BCA"   },
            { L_,   "A",     0,  "BC",    1,  0, "A"     },
            { L_,   "A",     0,  "BC",    1,  1, "CA"    },
            { L_,   "A",     0,  "BC",    2,  0, "A"     },
            { L_,   "A",     1,  "BC",    0,  0, "A"     },
            { L_,   "A",     1,  "BC",    0,  1, "AB"    },
            { L_,   "A",     1,  "BC",    0,  2, "ABC"   },
            { L_,   "A",     1,  "BC",    1,  0, "A"     },
            { L_,   "A",     1,  "BC",    1,  1, "AC"    },
            { L_,   "A",     1,  "BC",    2,  0, "A"     },

            { L_,   "",      0,  "ABC",   0,  0, ""      },
            { L_,   "",      0,  "ABC",   0,  1, "A"     },
            { L_,   "",      0,  "ABC",   0,  2, "AB"    },
            { L_,   "",      0,  "ABC",   0,  3, "ABC"   },
            { L_,   "",      0,  "ABC",   1,  0, ""      },
            { L_,   "",      0,  "ABC",   1,  1, "B"     },
            { L_,   "",      0,  "ABC",   1,  2, "BC"    },
            { L_,   "",      0,  "ABC",   2,  0, ""      },
            { L_,   "",      0,  "ABC",   2,  1, "C"     },
            { L_,   "",      0,  "ABC",   3,  0, ""      },

            //line  d-array di   s-array si  ne  expected
            //----  ------- --   ------- --  --  --------   Depth = 4
            { L_,   "ABCD",  0,  "",      0,  0, "ABCD"  },
            { L_,   "ABCD",  1,  "",      0,  0, "ABCD"  },
            { L_,   "ABCD",  2,  "",      0,  0, "ABCD"  },
            { L_,   "ABCD",  3,  "",      0,  0, "ABCD"  },
            { L_,   "ABCD",  4,  "",      0,  0, "ABCD"  },

            { L_,   "ABC",   0,  "D",     0,  0, "ABC"   },
            { L_,   "ABC",   0,  "D",     0,  1, "DABC"  },
            { L_,   "ABC",   0,  "D",     1,  0, "ABC"   },
            { L_,   "ABC",   1,  "D",     0,  0, "ABC"   },
            { L_,   "ABC",   1,  "D",     0,  1, "ADBC"  },
            { L_,   "ABC",   1,  "D",     1,  0, "ABC"   },
            { L_,   "ABC",   2,  "D",     0,  0, "ABC"   },
            { L_,   "ABC",   2,  "D",     0,  1, "ABDC"  },
            { L_,   "ABC",   2,  "D",     1,  0, "ABC"   },
            { L_,   "ABC",   3,  "D",     0,  0, "ABC"   },
            { L_,   "ABC",   3,  "D",     0,  1, "ABCD"  },
            { L_,   "ABC",   3,  "D",     1,  0, "ABC"   },

            { L_,   "AB",    0,  "CD",    0,  0, "AB"    },
            { L_,   "AB",    0,  "CD",    0,  1, "CAB"   },
            { L_,   "AB",    0,  "CD",    0,  2, "CDAB"  },
            { L_,   "AB",    0,  "CD",    1,  0, "AB"    },
            { L_,   "AB",    0,  "CD",    1,  1, "DAB"   },
            { L_,   "AB",    0,  "CD",    2,  0, "AB"    },
            { L_,   "AB",    1,  "CD",    0,  0, "AB"    },
            { L_,   "AB",    1,  "CD",    0,  1, "ACB"   },
            { L_,   "AB",    1,  "CD",    0,  2, "ACDB"  },
            { L_,   "AB",    1,  "CD",    1,  0, "AB"    },
            { L_,   "AB",    1,  "CD",    1,  1, "ADB"   },
            { L_,   "AB",    1,  "CD",    2,  0, "AB"    },
            { L_,   "AB",    2,  "CD",    0,  0, "AB"    },
            { L_,   "AB",    2,  "CD",    0,  1, "ABC"   },
            { L_,   "AB",    2,  "CD",    0,  2, "ABCD"  },
            { L_,   "AB",    2,  "CD",    1,  0, "AB"    },
            { L_,   "AB",    2,  "CD",    1,  1, "ABD"   },
            { L_,   "AB",    2,  "CD",    2,  0, "AB"    },

            { L_,   "A",     0,  "BCD",   0,  0, "A"     },
            { L_,   "A",     0,  "BCD",   0,  1, "BA"    },
            { L_,   "A",     0,  "BCD",   0,  2, "BCA"   },
            { L_,   "A",     0,  "BCD",   0,  3, "BCDA"  },
            { L_,   "A",     0,  "BCD",   1,  0, "A"     },
            { L_,   "A",     0,  "BCD",   1,  1, "CA"    },
            { L_,   "A",     0,  "BCD",   1,  2, "CDA"   },
            { L_,   "A",     0,  "BCD",   2,  0, "A"     },
            { L_,   "A",     0,  "BCD",   2,  1, "DA"    },
            { L_,   "A",     0,  "BCD",   3,  0, "A"     },
            { L_,   "A",     1,  "BCD",   0,  0, "A"     },
            { L_,   "A",     1,  "BCD",   0,  1, "AB"    },
            { L_,   "A",     1,  "BCD",   0,  2, "ABC"   },
            { L_,   "A",     1,  "BCD",   0,  3, "ABCD"  },
            { L_,   "A",     1,  "BCD",   1,  0, "A"     },
            { L_,   "A",     1,  "BCD",   1,  1, "AC"    },
            { L_,   "A",     1,  "BCD",   1,  2, "ACD"   },
            { L_,   "A",     1,  "BCD",   2,  0, "A"     },
            { L_,   "A",     1,  "BCD",   2,  1, "AD"    },
            { L_,   "A",     1,  "BCD",   3,  0, "A"     },

            { L_,   "",      0,  "ABCD",  0,  0, ""      },
            { L_,   "",      0,  "ABCD",  0,  1, "A"     },
            { L_,   "",      0,  "ABCD",  0,  2, "AB"    },
            { L_,   "",      0,  "ABCD",  0,  3, "ABC"   },
            { L_,   "",      0,  "ABCD",  0,  4, "ABCD"  },
            { L_,   "",      0,  "ABCD",  1,  0, ""      },
            { L_,   "",      0,  "ABCD",  1,  1, "B"     },
            { L_,   "",      0,  "ABCD",  1,  2, "BC"    },
            { L_,   "",      0,  "ABCD",  1,  3, "BCD"   },
            { L_,   "",      0,  "ABCD",  2,  0, ""      },
            { L_,   "",      0,  "ABCD",  2,  1, "C"     },
            { L_,   "",      0,  "ABCD",  2,  2, "CD"    },
            { L_,   "",      0,  "ABCD",  3,  0, ""      },
            { L_,   "",      0,  "ABCD",  3,  1, "D"     },
            { L_,   "",      0,  "ABCD",  4,  0, ""      },

            //line  d-array di   s-array si  ne  expected
            //----  ------- --   ------- --  --  --------   Depth = 5
            { L_,   "ABCDE", 0,  "",      0,  0, "ABCDE" },
            { L_,   "ABCDE", 1,  "",      0,  0, "ABCDE" },
            { L_,   "ABCDE", 2,  "",      0,  0, "ABCDE" },
            { L_,   "ABCDE", 3,  "",      0,  0, "ABCDE" },
            { L_,   "ABCDE", 4,  "",      0,  0, "ABCDE" },
            { L_,   "ABCDE", 5,  "",      0,  0, "ABCDE" },

            { L_,   "ABCD",  0,  "E",     0,  0, "ABCD"  },
            { L_,   "ABCD",  0,  "E",     0,  1, "EABCD" },
            { L_,   "ABCD",  0,  "E",     1,  0, "ABCD"  },
            { L_,   "ABCD",  1,  "E",     0,  0, "ABCD"  },
            { L_,   "ABCD",  1,  "E",     0,  1, "AEBCD" },
            { L_,   "ABCD",  1,  "E",     1,  0, "ABCD"  },
            { L_,   "ABCD",  2,  "E",     0,  0, "ABCD"  },
            { L_,   "ABCD",  2,  "E",     0,  1, "ABECD" },
            { L_,   "ABCD",  2,  "E",     1,  0, "ABCD"  },
            { L_,   "ABCD",  3,  "E",     0,  0, "ABCD"  },
            { L_,   "ABCD",  3,  "E",     0,  1, "ABCED" },
            { L_,   "ABCD",  3,  "E",     1,  0, "ABCD"  },
            { L_,   "ABCD",  4,  "E",     0,  0, "ABCD"  },
            { L_,   "ABCD",  4,  "E",     0,  1, "ABCDE" },
            { L_,   "ABCD",  4,  "E",     1,  0, "ABCD"  },

            { L_,   "ABC",   0,  "DE",    0,  0, "ABC"   },
            { L_,   "ABC",   0,  "DE",    0,  1, "DABC"  },
            { L_,   "ABC",   0,  "DE",    0,  2, "DEABC" },
            { L_,   "ABC",   0,  "DE",    1,  0, "ABC"   },
            { L_,   "ABC",   0,  "DE",    1,  1, "EABC"  },
            { L_,   "ABC",   0,  "DE",    2,  0, "ABC"   },
            { L_,   "ABC",   1,  "DE",    0,  0, "ABC"   },
            { L_,   "ABC",   1,  "DE",    0,  1, "ADBC"  },
            { L_,   "ABC",   1,  "DE",    0,  2, "ADEBC" },
            { L_,   "ABC",   1,  "DE",    1,  0, "ABC"   },
            { L_,   "ABC",   1,  "DE",    1,  1, "AEBC"  },
            { L_,   "ABC",   1,  "DE",    2,  0, "ABC"   },
            { L_,   "ABC",   2,  "DE",    0,  0, "ABC"   },
            { L_,   "ABC",   2,  "DE",    0,  1, "ABDC"  },
            { L_,   "ABC",   2,  "DE",    0,  2, "ABDEC" },
            { L_,   "ABC",   2,  "DE",    1,  0, "ABC"   },
            { L_,   "ABC",   2,  "DE",    1,  1, "ABEC"  },
            { L_,   "ABC",   2,  "DE",    2,  0, "ABC"   },
            { L_,   "ABC",   3,  "DE",    0,  0, "ABC"   },
            { L_,   "ABC",   3,  "DE",    0,  1, "ABCD"  },
            { L_,   "ABC",   3,  "DE",    0,  2, "ABCDE" },
            { L_,   "ABC",   3,  "DE",    1,  0, "ABC"   },
            { L_,   "ABC",   3,  "DE",    1,  1, "ABCE"  },
            { L_,   "ABC",   3,  "DE",    2,  0, "ABC"   },

            { L_,   "AB",    0,  "CDE",   0,  0, "AB"    },
            { L_,   "AB",    0,  "CDE",   0,  1, "CAB"   },
            { L_,   "AB",    0,  "CDE",   0,  2, "CDAB"  },
            { L_,   "AB",    0,  "CDE",   0,  3, "CDEAB" },
            { L_,   "AB",    0,  "CDE",   1,  0, "AB"    },
            { L_,   "AB",    0,  "CDE",   1,  1, "DAB"   },
            { L_,   "AB",    0,  "CDE",   1,  2, "DEAB"  },
            { L_,   "AB",    0,  "CDE",   2,  0, "AB"    },
            { L_,   "AB",    0,  "CDE",   2,  1, "EAB"   },
            { L_,   "AB",    0,  "CDE",   3,  0, "AB"    },
            { L_,   "AB",    1,  "CDE",   0,  0, "AB"    },
            { L_,   "AB",    1,  "CDE",   0,  1, "ACB"   },
            { L_,   "AB",    1,  "CDE",   0,  2, "ACDB"  },
            { L_,   "AB",    1,  "CDE",   0,  3, "ACDEB" },
            { L_,   "AB",    1,  "CDE",   1,  0, "AB"    },
            { L_,   "AB",    1,  "CDE",   1,  1, "ADB"   },
            { L_,   "AB",    1,  "CDE",   1,  2, "ADEB"  },
            { L_,   "AB",    1,  "CDE",   2,  0, "AB"    },
            { L_,   "AB",    1,  "CDE",   2,  1, "AEB"   },
            { L_,   "AB",    1,  "CDE",   3,  0, "AB"    },
            { L_,   "AB",    2,  "CDE",   0,  0, "AB"    },
            { L_,   "AB",    2,  "CDE",   0,  1, "ABC"   },
            { L_,   "AB",    2,  "CDE",   0,  2, "ABCD"  },
            { L_,   "AB",    2,  "CDE",   0,  3, "ABCDE" },
            { L_,   "AB",    2,  "CDE",   1,  0, "AB"    },
            { L_,   "AB",    2,  "CDE",   1,  1, "ABD"   },
            { L_,   "AB",    2,  "CDE",   1,  2, "ABDE"  },
            { L_,   "AB",    2,  "CDE",   2,  0, "AB"    },
            { L_,   "AB",    2,  "CDE",   2,  1, "ABE"   },
            { L_,   "AB",    2,  "CDE",   3,  0, "AB"    },

            { L_,   "A",     0,  "BCDE",  0,  0, "A"     },
            { L_,   "A",     0,  "BCDE",  0,  1, "BA"    },
            { L_,   "A",     0,  "BCDE",  0,  2, "BCA"   },
            { L_,   "A",     0,  "BCDE",  0,  3, "BCDA"  },
            { L_,   "A",     0,  "BCDE",  1,  0, "A"     },
            { L_,   "A",     0,  "BCDE",  1,  1, "CA"    },
            { L_,   "A",     0,  "BCDE",  1,  2, "CDA"   },
            { L_,   "A",     0,  "BCDE",  2,  0, "A"     },
            { L_,   "A",     0,  "BCDE",  2,  1, "DA"    },
            { L_,   "A",     0,  "BCDE",  3,  0, "A"     },
            { L_,   "A",     1,  "BCDE",  0,  0, "A"     },
            { L_,   "A",     1,  "BCDE",  0,  1, "AB"    },
            { L_,   "A",     1,  "BCDE",  0,  2, "ABC"   },
            { L_,   "A",     1,  "BCDE",  0,  3, "ABCD"  },
            { L_,   "A",     1,  "BCDE",  1,  0, "A"     },
            { L_,   "A",     1,  "BCDE",  1,  1, "AC"    },
            { L_,   "A",     1,  "BCDE",  1,  2, "ACD"   },
            { L_,   "A",     1,  "BCDE",  2,  0, "A"     },
            { L_,   "A",     1,  "BCDE",  2,  1, "AD"    },
            { L_,   "A",     1,  "BCDE",  3,  0, "A"     },

            { L_,   "",      0,  "ABCDE", 0,  0, ""      },
            { L_,   "",      0,  "ABCDE", 0,  1, "A"     },
            { L_,   "",      0,  "ABCDE", 0,  2, "AB"    },
            { L_,   "",      0,  "ABCDE", 0,  3, "ABC"   },
            { L_,   "",      0,  "ABCDE", 0,  4, "ABCD"  },
            { L_,   "",      0,  "ABCDE", 0,  5, "ABCDE" },
            { L_,   "",      0,  "ABCDE", 1,  0, ""      },
            { L_,   "",      0,  "ABCDE", 1,  1, "B"     },
            { L_,   "",      0,  "ABCDE", 1,  2, "BC"    },
            { L_,   "",      0,  "ABCDE", 1,  3, "BCD"   },
            { L_,   "",      0,  "ABCDE", 1,  4, "BCDE"  },
            { L_,   "",      0,  "ABCDE", 2,  0, ""      },
            { L_,   "",      0,  "ABCDE", 2,  1, "C"     },
            { L_,   "",      0,  "ABCDE", 2,  2, "CD"    },
            { L_,   "",      0,  "ABCDE", 2,  3, "CDE"   },
            { L_,   "",      0,  "ABCDE", 3,  0, ""      },
            { L_,   "",      0,  "ABCDE", 3,  1, "D"     },
            { L_,   "",      0,  "ABCDE", 3,  2, "DE"    },
            { L_,   "",      0,  "ABCDE", 4,  0, ""      },
            { L_,   "",      0,  "ABCDE", 4,  1, "E"     },
            { L_,   "",      0,  "ABCDE", 5,  0, ""      },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldDepth = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int   LINE   = DATA[ti].d_lineNum;
            const char *D_SPEC = DATA[ti].d_daSpec;
            const int   DI     = DATA[ti].d_di;
            const char *S_SPEC = DATA[ti].d_saSpec;
            const int   SI     = DATA[ti].d_si;
            const int   NE     = DATA[ti].d_ne;
            const char *E_SPEC = DATA[ti].d_expSpec;

            const int   DEPTH  = (int) strlen(D_SPEC) + (int) strlen(S_SPEC);
            if (DEPTH > oldDepth) {
                oldDepth = DEPTH;
                if (verbose) { cout << '\t';  P(DEPTH); }
            }

            Obj DD(g(D_SPEC));  // control for destination
            Obj SS(g(S_SPEC));  // control for source
            Obj EE(g(E_SPEC));  // control for expected value

            if (veryVerbose) {
                cout << "\t  =================================="
                        "==================================" << endl;
                cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                P_(SI); P_(NE); P(E_SPEC);
                cout << "\t\t"; P(DD);
                cout << "\t\t"; P(SS);
                cout << "\t\t"; P(EE);

                cout << "\t\t\t---------- BLACK BOX ----------" << endl;
            }

            // void insert(int di, const &sa, int si, int ne);
            if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
            {
              BEGIN_BSLMA_EXCEPTION_TEST {
                Obj x(DD, &testAllocator);  const Obj &X = x;
                {
                    Obj s(SS, &testAllocator);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    int off = x.totalOffset();
                    getElemTypeArray(&eta,s);
                    x.insert(DI,
                             eta.size() <= SI ? 0 : &eta[SI],
                             NE,
                             typesLookupTable);
                    // source non-'const'
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    // Test the offsets.  The map uses the total offset as
                    // a start for the inserts/appends, so equality is useful
                    // for the element types, and the code below is useful
                    // for checking the offsets.
                    for (int idx = 0; idx < NE; idx++) {
                        const int ALIGN =
                                   typesLookupTable[s[SI+idx].attributes()->
                                                      d_elemEnum]->d_alignment;
                        const int OFFSET = (off + ALIGN - 1) & ~(ALIGN - 1);
                        LOOP_ASSERT(LINE, OFFSET == x[DI+idx].offset());
                        off = OFFSET +
                                   typesLookupTable[s[SI+idx].attributes()->
                                                           d_elemEnum]->d_size;
                    }
                    getElemTypeArray(&eta,EE);
                    getElemTypeArray(&eta2,X);
                    LOOP_ASSERT(LINE, eta == eta2);    // source unchanged?
                }
                                                    // source is out of scope
                getElemTypeArray(&eta,EE);
                getElemTypeArray(&eta2,X);
                LOOP_ASSERT(LINE, eta == eta2);
              } END_BSLMA_EXCEPTION_TEST
            }

            // void insert(int di, const my_ElemTypeArray& sa);
            if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
            if ((int) strlen(S_SPEC) == NE) {
              BEGIN_BSLMA_EXCEPTION_TEST {
                Obj x(DD, &testAllocator);  const Obj &X = x;
                {
                    Obj s(SS, &testAllocator);  const Obj &S = s;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    int off = x.totalOffset();
                    getElemTypeArray(&eta,s);
                    x.insert(DI, vectorData(eta), eta.size(),
                            typesLookupTable);            // source non-'const'
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    for (int idx = 0; idx < s.length(); idx++) {
                        const int ALIGN =
                                      typesLookupTable[s[idx].attributes()->
                                                      d_elemEnum]->d_alignment;
                        const int OFFSET = (off + ALIGN - 1) & ~(ALIGN - 1);
                        LOOP_ASSERT(LINE, OFFSET == x[DI + idx].offset());
                        off = OFFSET +
                                      typesLookupTable[s[idx].attributes()->
                                                           d_elemEnum]->d_size;
                    }
                    getElemTypeArray(&eta,EE);
                    getElemTypeArray(&eta2,X);
                    LOOP_ASSERT(LINE, eta == eta2);
                    LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                }
                                                    // source is out of scope
                getElemTypeArray(&eta,EE);
                getElemTypeArray(&eta2,X);
                LOOP_ASSERT(LINE, eta == eta2);
              } END_BSLMA_EXCEPTION_TEST
            }

            // void insert(int di, bdem_ElemType::Type item);
            if (veryVerbose) cout << "\t\tinsert(di, item)" << endl;
            if (1 == NE) {
              BEGIN_BSLMA_EXCEPTION_TEST {
                Obj x(DD, &testAllocator);  const Obj &X = x;
                {
                    Obj s(SS, &testAllocator);  const Obj &S = s;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    const int ALIGN =
                                  typesLookupTable[s[SI].attributes()->
                                                      d_elemEnum]->d_alignment;
                    const int OFFSET =
                                  (x.totalOffset() + ALIGN - 1) & ~(ALIGN - 1);
                    x.insert(DI,
                             typesLookupTable[s[SI].attributes()->d_elemEnum]);
                                                          // source non-'const'
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, x[DI].offset() == OFFSET);
                    LOOP_ASSERT(LINE, SS == S);     // source unchanged ?
                }
                                                    // source is out of scope
                getElemTypeArray(&eta,EE);
                getElemTypeArray(&eta2,X);
                LOOP_ASSERT(LINE, eta == eta2);
              } END_BSLMA_EXCEPTION_TEST
            }

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // testing append
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            // void append(const my_ElemTypeArray& sa, int si, int ne);
            if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
            if ((int) strlen(D_SPEC) == DI) {
              BEGIN_BSLMA_EXCEPTION_TEST {
                Obj x(DD, &testAllocator);  const Obj &X = x;
                {
                    Obj s(SS, &testAllocator);  const Obj &S = s;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    getElemTypeArray(&eta,s);
                    x.append(eta.size() <= SI ? 0 : &eta[SI],
                             NE,
                             typesLookupTable);
                    // source non-'const'
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                    LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                }
                LOOP_ASSERT(LINE, EE == X);         // source is out of scope
              } END_BSLMA_EXCEPTION_TEST
            }

            // void append(const my_ElemTypeArray& sa);
            if (veryVerbose) cout << "\t\tappend(sa)" << endl;
            if ((int) strlen(D_SPEC) == DI && (int) strlen(S_SPEC) == NE) {
              BEGIN_BSLMA_EXCEPTION_TEST {
                Obj x(DD, &testAllocator);  const Obj &X = x;
                {
                    Obj s(SS, &testAllocator);  const Obj &S = s;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    getElemTypeArray(&eta,s);
                    x.append(vectorData(eta), eta.size(), typesLookupTable);
                                                          // source non-'const'
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                    LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                }
                LOOP_ASSERT(LINE, EE == X);         // source is out of scope
              } END_BSLMA_EXCEPTION_TEST
            }

            // void append(bdem_ElemType::Type item);
            if (veryVerbose) cout << "\t\tappend(item)" << endl;
            if ((int) strlen(D_SPEC) == DI && 1 == NE) {
              BEGIN_BSLMA_EXCEPTION_TEST {
                Obj x(DD, &testAllocator);  const Obj &X = x;
                {
                    Obj s(SS, &testAllocator);  const Obj &S = s;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(typesLookupTable[s[SI].attributes()->d_elemEnum]);
                                                          // source non-'const'
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                    LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                }
                LOOP_ASSERT(LINE, EE == X);         // source is out of scope
              } END_BSLMA_EXCEPTION_TEST
            }

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // testing remove
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            // void remove(int index, int ne);
            if (veryVerbose) cout << "\t\tremove(index, ne)" << endl;
            {
              BEGIN_BSLMA_EXCEPTION_TEST {        // Note specs are switched.
                Obj x(EE, &testAllocator);  const Obj &X = x;
                if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                x.remove(DI, NE);
                if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                {
                    getElemTypeArray(&eta,DD);
                    getElemTypeArray(&eta2,X);
                    LOOP_ASSERT(LINE, eta == eta2);
                    LOOP_ASSERT(LINE, EE.totalOffset() == X.totalOffset());
                    if (veryVerbose) {
                        P_(DD.totalOffset()); P(X.totalOffset());
                    }
                    int idx;
                    for (idx = 0; idx < DI; idx++) {
                        LOOP_ASSERT(LINE,
                                          EE[idx].offset() == X[idx].offset());
                    }
                    for (; idx < X.length(); idx++) {
                        LOOP_ASSERT(LINE,
                                     EE[idx + NE].offset() == X[idx].offset());
                    }
                }
              } END_BSLMA_EXCEPTION_TEST
            }

            // void remove(int index);
            if (veryVerbose) cout << "\t\tremove(index)" << endl;
            if (1 == NE) {
              BEGIN_BSLMA_EXCEPTION_TEST {  // Note specs are switched
                Obj x(EE, &testAllocator);  const Obj &X = x;
                if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                x.remove(DI);
                if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                getElemTypeArray(&eta,X);
                getElemTypeArray(&eta2,DD);
                LOOP_ASSERT(LINE, eta == eta2);
                LOOP_ASSERT(LINE, X.totalOffset() == EE.totalOffset());
                int idx;
                for (idx = 0; idx < DI; idx++) {
                    LOOP_ASSERT(LINE, EE[idx].offset() == X[idx].offset());
                }
                for (; idx < X.length(); idx++) {
                    LOOP_ASSERT(LINE,
                                     EE[idx + NE].offset() == X[idx].offset());
                }
              } END_BSLMA_EXCEPTION_TEST
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INITIAL-LENGTH CONSTRUCTORS
        // Concerns:
        //   1. The initial value is correct.
        //   2. The constructor is exception neutral w.r.t. memory allocation.
        //   3. The internal memory management system is hooked up properly
        //      so that *all* internally allocated memory draws from a
        //      user-supplied allocator whenever one is specified.
        //
        // Plan:
        //   For each constructor we will create objects
        //    - With and without passing in an allocator.
        //    - In the presence of exceptions during memory allocations using
        //      a 'bslma_TestAllocator' and varying its *allocation* *limit*.
        //    - Where the object is constructed entirely in static memory
        //      (using a 'bdema_BufferedSequentialAllocator') and never
        //      destroyed.  and use direct accessors to verify
        //    - length
        //    - element value at each index position { 0 .. length - 1 } using
        //      getType().
        //
        // Testing:
        //   bdem_RowLayout(const my_ElemTypeArray& sa, *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Initial-Length Constructor" << endl
                          << "==================================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\nTesting ctor using my_ElemTypeArray."
                          << endl;
        {
            const bdem_ElemType::Type da[] = {
                bdem_ElemType::BDEM_CHAR,
                bdem_ElemType::BDEM_SHORT,
                bdem_ElemType::BDEM_INT,
                bdem_ElemType::BDEM_INT64,
                bdem_ElemType::BDEM_FLOAT,
                bdem_ElemType::BDEM_DOUBLE,
            }; // ADJUST

            const int NUM_ELEMENTS = sizeof da / sizeof *da;
            my_ElemTypeArray DA;
            for (int idx=0; idx < NUM_ELEMENTS; idx++) {
                DA.push_back(da[idx]);
            }

            if (verbose) cout << "\tWithout passing in an allocator." << endl;
            {
                for (int ne = 0; ne <= NUM_ELEMENTS; ++ne) {
                    if (verbose) P(ne);
                    my_ElemTypeArray DB;
                    for (int i=0; i < ne; i++) {
                        DB.push_back(DA[i]);
                    }
                    Obj mX(vectorData(DB), DB.size(), typesLookupTable);
                    const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP_ASSERT(ne, ne == X.length());
                    for (int i = 0; i < ne; ++i) {
                        LOOP2_ASSERT(ne, i,
                                     DB[i] == X[i].attributes()->d_elemEnum);
                    }
                }
            }

            if (verbose) cout << "\tPassing in an allocator." << endl;

            if (verbose) cout << "\t\tWith no exceptions." << endl;
            {
                for (int ne = 0; ne <= NUM_ELEMENTS; ++ne) {
                    if (verbose) P(ne);
                    my_ElemTypeArray DB;
                    for (int i=0; i < ne; i++) {
                        DB.push_back(DA[i]);
                    }
                    Obj mX(vectorData(DB),
                           DB.size(),
                           typesLookupTable,
                           &testAllocator);
                    const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP_ASSERT(ne, ne == X.length());
                    for (int i = 0; i < ne; ++i) {
                         LOOP2_ASSERT(ne, i,
                                      DB[i] == X[i].attributes()->d_elemEnum);
                    }
                }
            }

            if (verbose) cout << "\t\tWith exceptions." << endl; {
                for (int ne = 0; ne <= NUM_ELEMENTS; ++ne) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    if (verbose) P(ne);
                    my_ElemTypeArray DB;
                    for (int i=0; i < ne; i++) {
                        DB.push_back(DA[i]);
                    }
                    Obj mX(vectorData(DB),
                           DB.size(),
                           typesLookupTable,
                           &testAllocator);
                    const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP_ASSERT(ne, ne == X.length());
                    for (int i = 0; i < ne; ++i) {
                         LOOP2_ASSERT(ne, i,
                                      DB[i] == X[i].attributes()->d_elemEnum);
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }
            }

            if (verbose) cout << "\tIn place using a buffer allocator." <<endl;
            {
                char memory[4096];
                bdema_BufferedSequentialAllocator a(memory, sizeof memory);
                for (int ne = 0; ne <= NUM_ELEMENTS; ++ne) {
                    if (verbose) P(ne);
                    my_ElemTypeArray DB;
                    for (int i=0; i < ne; i++) {
                        DB.push_back(DA[i]);
                    }
                    Obj *doNotDelete = new(a.allocate(sizeof(Obj)))
                          Obj(vectorData(DB), DB.size(), typesLookupTable, &a);
                    Obj &mX = *doNotDelete;  const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP_ASSERT(ne, ne == X.length());
                    for (int i = 0; i < ne; ++i) {
                        LOOP2_ASSERT(ne, i,
                                     DB[i] == X[i].attributes()->d_elemEnum);
                    }
                }
                // No destructor is called; will produce memory leak in purify
                // if internal allocators are not hooked up properly.
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING TYPEALIGNMENT AND TYPESIZE METHODS
        // Concerns:
        //   Ensure enumerator values are consecutive integers in the range
        //   [0 .. LENGTH - 1] and that all names are unique.  Verify that the
        //   'evalType' functions correctly map their argument to their
        //   respective Type enumerators.
        //
        // Plan:
        //   Enumerate through all of the 32 fundamental types defined, and
        //   assert that each of the types' alignment and size corresponds to
        //   the values returned by bsls_AlignmentFromType<Type>::VALUE meta
        //   metafunction and sizeof(TYPE) function respectively.
        //
        // Testing:
        //   typesLookupTable
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'typeAlignment' and 'typeSize'" << endl
                          << "======================================" << endl;

        {
            for (int ti = 0; ti < NUM_TYPES; ++ti) {
              switch (bdemType[ti]) {  // AB...UVab..gh
                case 'A': {
                  ASSERT(bsls_AlignmentFromType<char>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_CHAR
                                         ]->d_alignment);
                  ASSERT(sizeof(char) ==
                         typesLookupTable[bdem_ElemType::BDEM_CHAR]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_CHAR
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<char>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_CHAR]->d_size);
                      T_(); P(sizeof(char));
                  }
                } break;
                case 'B': {
                  ASSERT(bsls_AlignmentFromType<short>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_SHORT
                                         ]->d_alignment);
                  ASSERT(sizeof(short) ==
                         typesLookupTable[bdem_ElemType::BDEM_SHORT]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_SHORT
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<short>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_SHORT]->d_size);
                      T_(); P(sizeof(short));
                  }
                } break;
                case 'C': {
                  ASSERT(bsls_AlignmentFromType<int>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_INT
                                         ]->d_alignment);
                  ASSERT(sizeof(int) ==
                         typesLookupTable[bdem_ElemType::BDEM_INT]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_INT
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<int>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_INT]->d_size);
                      T_(); P(sizeof(int));
                  }
                } break;
                case 'D': {
                  ASSERT(bsls_AlignmentFromType<Int64>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_INT64
                                         ]->d_alignment);
                  ASSERT(sizeof(Int64) ==
                         typesLookupTable[bdem_ElemType::BDEM_INT64]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_INT64
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<Int64>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_INT64]->d_size);
                      T_(); P(sizeof(Int64));
                  }
                } break;
                case 'E': {
                  ASSERT(bsls_AlignmentFromType<float>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_FLOAT
                                         ]->d_alignment);
                  ASSERT(sizeof(float) ==
                         typesLookupTable[bdem_ElemType::BDEM_FLOAT]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_FLOAT
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<float>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_FLOAT]->d_size);
                                      T_(); P(sizeof(float));
                  }
                } break;
                case 'F': {
                  ASSERT(bsls_AlignmentFromType<double>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_DOUBLE
                                         ]->d_alignment);
                  ASSERT(sizeof(double) ==
                         typesLookupTable[bdem_ElemType::BDEM_DOUBLE]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_DOUBLE
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<double>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_DOUBLE]->d_size);
                      T_(); P(sizeof(double));
                  }
                } break;
                case 'G': {
                  ASSERT(bsls_AlignmentFromType<bsl::string>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_STRING
                                         ]->d_alignment);
                  ASSERT(sizeof(bsl::string) ==
                         typesLookupTable[bdem_ElemType::BDEM_STRING]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_STRING
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<bsl::string>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_STRING]->d_size);
                      T_(); P(sizeof(bsl::string));
                  }
                } break;
                case 'H': {
                  ASSERT(bsls_AlignmentFromType<bdet_Datetime>::VALUE ==
                       typesLookupTable[
                                            bdem_ElemType::BDEM_DATETIME
                                       ]->d_alignment);
                  ASSERT(sizeof(bdet_Datetime) ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_DATETIME
                                         ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_DATETIME
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<bdet_Datetime>::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_DATETIME
                                        ]->d_size);
                      T_(); P(sizeof(bdet_Datetime));
                  }
                } break;
                case 'I': {
                  ASSERT(bsls_AlignmentFromType<bdet_Date>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_DATE
                                         ]->d_alignment);
                  ASSERT(sizeof(bdet_Date) ==
                         typesLookupTable[bdem_ElemType::BDEM_DATE]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_DATE
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<bdet_Date>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_DATE]->d_size);
                      T_(); P(sizeof(bdet_Date));
                  }
                } break;
                case 'J': {
                  ASSERT(bsls_AlignmentFromType<bdet_Time>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_TIME
                                         ]->d_alignment);
                  ASSERT(sizeof(bdet_Time) ==
                         typesLookupTable[bdem_ElemType::BDEM_TIME]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_TIME
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<bdet_Time>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_TIME]->d_size);
                      T_(); P(sizeof(bdet_Time));
                  }
                } break;
                case 'K': {
                  ASSERT(bsls_AlignmentFromType<bsl::vector<char> >::VALUE ==
                     typesLookupTable[
                                          bdem_ElemType::BDEM_CHAR_ARRAY
                                     ]->d_alignment);
                  ASSERT(sizeof(bsl::vector<char>) ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_CHAR_ARRAY
                                         ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_CHAR_ARRAY
                                        ]->d_alignment);
                      T_();
                      P(bsls_AlignmentFromType<bsl::vector<char> >::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_CHAR_ARRAY
                                        ]->d_size);
                      T_(); P(sizeof(bsl::vector<char>));
                  }
                } break;
                case 'L': {
                  ASSERT(bsls_AlignmentFromType<bsl::vector<short> >::VALUE ==
                    typesLookupTable[
                                         bdem_ElemType::BDEM_SHORT_ARRAY
                                    ]->d_alignment);
                  ASSERT(sizeof(bsl::vector<short>) ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_SHORT_ARRAY
                                         ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_SHORT_ARRAY
                                        ]->d_alignment);
                      T_();
                      P(bsls_AlignmentFromType<bsl::vector<short> >::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_SHORT_ARRAY
                                        ]->d_size);
                      T_(); P(sizeof(bsl::vector<short>));
                  }
                } break;
                case 'M': {
                  ASSERT(bsls_AlignmentFromType<bsl::vector<int> >::VALUE ==
                      typesLookupTable[
                                           bdem_ElemType::BDEM_INT_ARRAY
                                      ]->d_alignment);
                  ASSERT(sizeof(bsl::vector<int>) ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_INT_ARRAY
                                         ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_INT_ARRAY
                                        ]->d_alignment);
                      T_();
                      P(bsls_AlignmentFromType<bsl::vector<int> >::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_INT_ARRAY
                                        ]->d_size);
                      T_(); P(sizeof(bsl::vector<int>));
                  }
                } break;
                case 'N': {
                  ASSERT(bsls_AlignmentFromType<bsl::vector<Int64> >::VALUE ==
                    typesLookupTable[
                                         bdem_ElemType::BDEM_INT64_ARRAY
                                    ]->d_alignment);
                  ASSERT(sizeof(bsl::vector<Int64>) ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_INT64_ARRAY
                                         ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_INT64_ARRAY
                                        ]->d_alignment);
                      T_();
                      P(bsls_AlignmentFromType<bsl::vector<Int64> >::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_INT64_ARRAY
                                        ]->d_size);
                      T_(); P(sizeof(bsl::vector<Int64>));
                  }
                } break;
                case 'O': {
                  ASSERT(bsls_AlignmentFromType<bsl::vector<float> >::VALUE ==
                    typesLookupTable[
                                         bdem_ElemType::BDEM_FLOAT_ARRAY
                                    ]->d_alignment);
                  ASSERT(sizeof(bsl::vector<float>) ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_FLOAT_ARRAY
                                         ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_FLOAT_ARRAY
                                        ]->d_alignment);
                      T_();
                      P(bsls_AlignmentFromType<bsl::vector<float> >::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_FLOAT_ARRAY
                                        ]->d_size);
                      T_(); P(sizeof(bsl::vector<float>));
                  }
                } break;
                case 'P': {
                  ASSERT(bsls_AlignmentFromType<bsl::vector<double> >::VALUE ==
                   typesLookupTable[
                                        bdem_ElemType::BDEM_DOUBLE_ARRAY
                                   ]->d_alignment);
                  ASSERT(sizeof(bsl::vector<double>) ==
                        typesLookupTable[
                                             bdem_ElemType::BDEM_DOUBLE_ARRAY
                                        ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_DOUBLE_ARRAY
                                        ]->d_alignment);
                      T_();
                      P(bsls_AlignmentFromType<bsl::vector<double> >::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_DOUBLE_ARRAY
                                        ]->d_size);
                      T_(); P(sizeof(bsl::vector<double>));
                  }
                } break;
                case 'Q': {
                  ASSERT(
                    bsls_AlignmentFromType<bsl::vector<bsl::string> >::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_STRING_ARRAY
                                         ]->d_alignment);
                  ASSERT(sizeof(bsl::vector<bsl::string>) ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_STRING_ARRAY
                                         ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_STRING_ARRAY
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<
                                                  bsl::vector<bsl::string>
                                              >::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_STRING_ARRAY
                                        ]->d_size);
                      T_(); P(sizeof(bsl::vector<bsl::string>));
                  }
                } break;
                case 'R': {
                  ASSERT(
                    bsls_AlignmentFromType<bsl::vector<bdet_Datetime> >::VALUE
                     == typesLookupTable[bdem_ElemType::BDEM_DATETIME_ARRAY]->
                                                                  d_alignment);
                  ASSERT(sizeof(bsl::vector<bdet_Datetime>) ==
                      typesLookupTable[
                                           bdem_ElemType::BDEM_DATETIME_ARRAY
                                      ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[bdem_ElemType::BDEM_DATETIME_ARRAY]->
                                                                  d_alignment);
                      T_();
                      P(
                   bsls_AlignmentFromType<bsl::vector<bdet_Datetime> >::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_DATETIME_ARRAY]->
                                                                       d_size);
                      T_(); P(sizeof(bsl::vector<bdet_Datetime>));
                  }
                } break;
                case 'S': {
                  ASSERT(
                     bsls_AlignmentFromType<bsl::vector<bdet_Date> >::VALUE ==
                         typesLookupTable[bdem_ElemType::BDEM_DATE_ARRAY]->
                                                                  d_alignment);
                  ASSERT(sizeof(bsl::vector<bdet_Date>) ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_DATE_ARRAY
                                         ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[bdem_ElemType::BDEM_DATE_ARRAY]->
                              d_alignment);
                      T_(); P(bsls_AlignmentFromType<
                                                  bsl::vector<bdet_Date>
                                              >::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_DATE_ARRAY
                                        ]->d_size);
                      T_(); P(sizeof(bsl::vector<bdet_Date>));
                  }
                } break;
                case 'T': {
                  ASSERT(
                      bsls_AlignmentFromType<bsl::vector<bdet_Time> >::VALUE ==
                         typesLookupTable[bdem_ElemType::BDEM_TIME_ARRAY]->
                                                                  d_alignment);
                  ASSERT(sizeof(bsl::vector<bdet_Time>) ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_TIME_ARRAY
                                         ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[bdem_ElemType::BDEM_TIME_ARRAY]->
                              d_alignment);
                      T_(); P(bsls_AlignmentFromType<
                                                   bsl::vector<bdet_Time>
                                              >::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_TIME_ARRAY
                                        ]->d_size);
                      T_(); P(sizeof(bsl::vector<bdet_Time>));
                  }
                } break;
                case 'U': {
                  ASSERT(bsls_AlignmentFromType<ListRep>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_LIST
                                         ]->d_alignment);
                  ASSERT(sizeof(ListRep) ==
                         typesLookupTable[bdem_ElemType::BDEM_LIST]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_LIST
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<ListRep>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_LIST]->d_size);
                      T_(); P(sizeof(ListRep));
                  }
                } break;
                case 'V': {
                  ASSERT(bsls_AlignmentFromType<TableRep>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_TABLE
                                         ]->d_alignment);
                  ASSERT(sizeof(TableRep) ==
                         typesLookupTable[bdem_ElemType::BDEM_TABLE]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_TABLE
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<TableRep>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_TABLE]->d_size);
                      T_(); P(sizeof(TableRep));
                  }
                } break;
                case 'a': {
                  ASSERT(bsls_AlignmentFromType<bool>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_BOOL
                                         ]->d_alignment);
                  ASSERT(sizeof(bool) ==
                         typesLookupTable[bdem_ElemType::BDEM_BOOL]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_BOOL
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<bool>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_BOOL]->d_size);
                      T_(); P(sizeof(bool));
                  }
                } break;
                case 'b': {
                  ASSERT(bsls_AlignmentFromType<bdet_DatetimeTz>::VALUE ==
                         typesLookupTable[bdem_ElemType::BDEM_DATETIMETZ]->
                                                                  d_alignment);
                  ASSERT(sizeof(bdet_DatetimeTz) ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_DATETIMETZ
                                         ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_DATETIMETZ
                                        ]->d_alignment);
                      T_();
                      P(bsls_AlignmentFromType<bdet_DatetimeTz>::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_DATETIMETZ
                                        ]->d_size);
                      T_(); P(sizeof(bdet_DatetimeTz));
                  }
                } break;
                case 'c': {
                  ASSERT(bsls_AlignmentFromType<bdet_DateTz>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_DATETZ
                                         ]->d_alignment);
                  ASSERT(sizeof(bdet_DateTz) ==
                         typesLookupTable[bdem_ElemType::BDEM_DATETZ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_DATETZ
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<bdet_DateTz>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_DATETZ]->d_size);
                      T_(); P(sizeof(bdet_DateTz));
                  }
                } break;
                case 'd': {
                  ASSERT(bsls_AlignmentFromType<bdet_TimeTz>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_TIMETZ
                                         ]->d_alignment);
                  ASSERT(sizeof(bdet_TimeTz) ==
                         typesLookupTable[bdem_ElemType::BDEM_TIMETZ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_TIMETZ
                                        ]->d_alignment);
                      T_(); P(bsls_AlignmentFromType<bdet_TimeTz>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_TIMETZ]->d_size);
                      T_(); P(sizeof(bdet_TimeTz));
                  }
                } break;
                case 'e': {
                  ASSERT(bsls_AlignmentFromType<bsl::vector<bool> >::VALUE ==
                         typesLookupTable[bdem_ElemType::BDEM_BOOL_ARRAY]->
                                                                  d_alignment);
                  ASSERT(sizeof(bsl::vector<bool>) ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_BOOL_ARRAY
                                         ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_BOOL_ARRAY
                                        ]->d_alignment);
                      T_();
                      P(bsls_AlignmentFromType<
                                            bsl::vector<bool>
                                        >::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_BOOL_ARRAY
                                        ]->d_size);
                      T_(); P(sizeof(bsl::vector<bool>));
                  }
                } break;
                case 'f': {
                  ASSERT(
                      bsls_AlignmentFromType<bsl::vector<bdet_DatetimeTz> >::
                                                                       VALUE ==
                         typesLookupTable[
                                           bdem_ElemType::BDEM_DATETIMETZ_ARRAY
                                         ]->d_alignment);
                  ASSERT(sizeof(bsl::vector<bdet_DatetimeTz>) ==
                         typesLookupTable[
                                           bdem_ElemType::BDEM_DATETIMETZ_ARRAY
                                         ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                           bdem_ElemType::BDEM_DATETIMETZ_ARRAY
                                        ]->d_alignment);
                      T_();
                      P(bsls_AlignmentFromType<
                                            bsl::vector<bdet_DatetimeTz>
                                        >::VALUE);
                      P(typesLookupTable[
                                           bdem_ElemType::BDEM_DATETIMETZ_ARRAY
                                        ]->d_size);
                      T_(); P(sizeof(bsl::vector<bdet_DatetimeTz>));
                  }
                } break;
                case 'g': {
                  ASSERT(
                    bsls_AlignmentFromType<bsl::vector<bdet_DateTz> >::VALUE ==
                         typesLookupTable[bdem_ElemType::BDEM_DATETZ_ARRAY]->
                                                                  d_alignment);
                  ASSERT(sizeof(bsl::vector<bdet_DateTz>) ==
                         typesLookupTable[bdem_ElemType::BDEM_DATETZ_ARRAY]->
                                                                       d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_DATETZ_ARRAY
                                        ]->d_alignment);
                      T_();
                      P(bsls_AlignmentFromType<bsl::vector<bdet_DateTz> >
                              ::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_DATETZ_ARRAY
                                        ]->d_size);
                      T_(); P(sizeof(bsl::vector<bdet_DateTz>));
                  }
                } break;
                case 'h': {
                  ASSERT(
                    bsls_AlignmentFromType<bsl::vector<bdet_TimeTz> >::VALUE ==
                         typesLookupTable[bdem_ElemType::BDEM_TIMETZ_ARRAY]->
                                                                  d_alignment);
                  ASSERT(sizeof(bsl::vector<bdet_TimeTz>) ==
                         typesLookupTable[bdem_ElemType::BDEM_TIMETZ_ARRAY]->
                                                                       d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[bdem_ElemType::BDEM_TIMETZ_ARRAY]->
                              d_alignment);
                      T_();
                      P(
                     bsls_AlignmentFromType<bsl::vector<bdet_TimeTz> >::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_TIMETZ_ARRAY
                                        ]->d_size);
                      T_(); P(sizeof(bsl::vector<bdet_TimeTz>));
                  }
                } break;
                case 'i': {
                  ASSERT(bsls_AlignmentFromType<ChoiceRep>::VALUE ==
                         typesLookupTable[
                                              bdem_ElemType::BDEM_CHOICE
                                         ]->d_alignment);
                  ASSERT(sizeof(ChoiceRep) ==
                              typesLookupTable[
                                                   bdem_ElemType::BDEM_CHOICE
                                              ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_CHOICE
                                        ]->d_alignment);
                      T_();
                      P(bsls_AlignmentFromType<ChoiceRep>::VALUE);
                      P(typesLookupTable[bdem_ElemType::BDEM_CHOICE]->d_size);
                      T_(); P(sizeof(ChoiceRep));
                  }
                } break;
                case 'j': {
                  ASSERT(bsls_AlignmentFromType<ChoiceArrayRep>::VALUE ==
                         typesLookupTable[bdem_ElemType::BDEM_CHOICE_ARRAY]->
                                                                  d_alignment);
                  ASSERT(sizeof(ChoiceArrayRep) ==
                              typesLookupTable[
                                               bdem_ElemType::BDEM_CHOICE_ARRAY
                                              ]->d_size);
                  if (veryVerbose) {
                      P(typesLookupTable[bdem_ElemType::BDEM_CHOICE_ARRAY]->
                              d_alignment);
                      T_();
                      P(bsls_AlignmentFromType<ChoiceArrayRep>::VALUE);
                      P(typesLookupTable[
                                             bdem_ElemType::BDEM_CHOICE_ARRAY
                                        ]->d_size);
                      T_(); P(sizeof(ChoiceArrayRep));
                  }
                } break;
                default: {
                  ASSERT(0 == 1);
                } break;
              }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        // Concerns:
        //   We have the following concerns:
        //   1.  The value represented by any instance can be assigned to any
        //       other instance regardless of how either value is represented
        //       internally.
        //   2.  The 'rhs' value must not be affected by the operation.
        //   3.  'rhs' going out of scope has no effect on the value of 'lhs'
        //       after the assignment.
        //   4.  Aliasing (x = x): The assignment operator must always work --
        //       even when the lhs and rhs are identically the same object.
        //   5.  The assignment operator must be neutral with respect to memory
        //       allocation exceptions.
        //
        // Plan:
        //   Specify a set S of unique object values with substantial and
        //   varied differences, ordered by increasing length.  For each value
        //   in S, construct an object x along with a sequence of similarly
        //   constructed duplicates x1, x2, ..., xN.  Attempt to affect every
        //   aspect of white-box state by altering each xi in a unique way.
        //   Let the union of all such objects be the set T.
        //
        //   To address concerns 1, 2, and 5, construct tests u = v for all
        //   (u, v) in T X T.  Using canonical controls UU and VV, assert
        //   before the assignment that UU == u, VV == v, and v == u iff
        //   VV == UU.  After the assignment, assert that VV == u, VV == v,
        //   and, for grins, that v == u.  Let v go out of scope and confirm
        //   that VV == u.  All of these tests are performed within the 'bdema'
        //   exception testing apparatus.
        //
        //   As a separate exercise, we address 4 and 5 by constructing tests
        //   y = y for all y in T.  Using a canonical control X, we will verify
        //   that X == y before and after the assignment, again within the
        //   bdema exception testing apparatus.
        //
        // Testing:
        //   bdem_RowLayout& operator=(const bdem_RowLayout& rhs);
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Assignment Operator" << endl
                          << "===========================" << endl;

        if (verbose) cout <<
            "\nAssign cross product of values with varied representations."
            << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9,
                "",        "A",    "BC",     "CDE",    "DEAB",   "EABCD",
                "AEDCBAE",         "CBAEDCBA",         "EDCBAEDCB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9,
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)strlen(U_SPEC);

                if (verbose) {
                    cout << "\tFor lhs objects of length " << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);   // strictly increasing
                uOldLen = uLen;

                const Obj UU = g(U_SPEC);              // control
                LOOP_ASSERT(ui, uLen == UU.length());  // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int vLen = (int)strlen(V_SPEC);

                    if (veryVerbose) {
                        cout << "\t\tFor rhs objects of length " << vLen
                                                                 << ":\t";
                        P(V_SPEC);
                    }

                    const Obj VV = g(V_SPEC);           // control

                    const int Z = ui == vi; // flag indicating same values

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                          const int V_N = EXTEND[vj];

                          BEGIN_BSLMA_EXCEPTION_TEST {
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
                            Obj mU(&testAllocator); stretchRemoveAll(&mU, U_N);
                            const Obj& U = mU; gg(&mU, U_SPEC);
                            {
                            //--^
                            Obj mV(&testAllocator); stretchRemoveAll(&mV, V_N);
                            const Obj& V = mV; gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose ||
                                               (veryVerbose && firstFew > 0)) {
                                cout << "\t| "; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(V==U));

                            testAllocator.setAllocationLimit(AL);
                            mU = V; // test assignment here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                            //--v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                          } END_BSLMA_EXCEPTION_TEST
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing)." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = EXTEND[tj];
                    Obj mY(&testAllocator);  stretchRemoveAll(&mY, N);
                    const Obj& Y = mY;       gg(&mY, SPEC);

                    if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    mY = Y; // test assignment here

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION, G
        // Concerns:
        //   Since 'g' is implemented almost entirely using 'gg', we need to
        //   verify only that the arguments are properly forwarded, that 'g'
        //   does not affect the test allocator, and that 'g' returns an object
        //   by value.
        //
        // Plan:
        //   For each SPEC in a short list of specifications, compare the
        //   object returned (by value) from the generator function, 'g(SPEC)'
        //   with the value of a newly constructed OBJECT configured using
        //   'gg(&OBJECT, SPEC)'.  Compare the results of calling the
        //   allocator's 'numBlocksTotal' and 'numBytesInUse' methods before
        //   and after calling 'g' in order to demonstrate that 'g' has no
        //   effect on the test allocator.  Finally, use 'sizeof' to confirm
        //   that the (temporary) returned by 'g' differs in size from that
        //   returned by 'gg'.
        //
        // Testing:
        //   bsl::vector<double> g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Generator Function 'g'" << endl
                          << "==============================" << endl;

        static const char *SPECS[] = {
            "", "~", "A", "B", "C", "D", "E", "A~B~C~D~E", "ABCDE", "ABC~DE",
        0}; // Null string required as last element.

        if (verbose) cout <<
            "\nCompare values produced by 'g' and 'gg' for various inputs."
            << endl;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *spec = SPECS[ti];
            if (veryVerbose) { P_(ti);  P(spec); }
            Obj mX(&testAllocator);  gg(&mX, spec);  const Obj& X = mX;
            if (veryVerbose) {
                cout << "\t g = " << g(spec) << endl;
                cout << "\tgg = " << X       << endl;
            }
            const int TOTAL_BLOCKS_BEFORE = testAllocator.numBlocksTotal();
            const int IN_USE_BYTES_BEFORE = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, X == g(spec));
            const int TOTAL_BLOCKS_AFTER = testAllocator.numBlocksTotal();
            const int IN_USE_BYTES_AFTER = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
            LOOP_ASSERT(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
        }

        if (verbose) cout << "\nConfirm return-by-value." << endl;
        {
            const char *spec = "ABCDE";

            ASSERT(sizeof(Obj) == sizeof g(spec));      // compile-time fact

            Obj x(&testAllocator);                      // runtime tests
            Obj& r1 = gg(&x, spec);
            Obj& r2 = gg(&x, spec);
            const Obj& r3 = g(spec);
            const Obj& r4 = g(spec);
            ASSERT(&r2 == &r1);
            ASSERT(&x  == &r1);
            ASSERT(&r4 != &r3);
            ASSERT(&x  != &r3);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        // Concerns:
        //   1. The new object's value is the same as that of the original
        //       object (relying on the previously tested equality operators).
        //   2. All internal representations of a given value can be used to
        //        create a new object of equivalent value.
        //   3. The value of the original object is left unaffected.
        //   4. Subsequent changes in or destruction of the source object have
        //      no effect on the copy-constructed object.
        //   5. The function is exception neutral w.r.t. memory allocation.
        //   6. The object has its internal memory management system hooked up
        //      properly so that *all* internally allocated memory draws from a
        //      user-supplied allocator whenever one is specified.
        //
        // Plan:
        //   To address concerns 1 - 3, specify a set S of object values with
        //   substantial and varied differences, ordered by increasing length.
        //    For each value in S, initialize objects w and x, copy construct y
        //    from x and use 'operator==' to verify that both x and y
        //    subsequently have the same value as w.  Let x go out of scope
        //    and again verify that w == x.
        //   Repeat this test with x having the same *logical* value, but
        //   perturbed so as to have potentially different internal
        //   representations.
        //
        //   To address concern 5, we will perform each of the above tests in
        //   the presence of exceptions during memory allocations using a
        //   'bslma_TestAllocator' and varying its *allocation* *limit*.
        //
        //   To address concern 6, we will repeat the above tests:
        //     - When passing in no allocator.
        //     - When passing in a null pointer: (bslma_Allocator *)0.
        //     - When passing in a test allocator (see concern 5).
        //     - Where the object is constructed entirely in static memory
        //       (using a 'bdema_BufferedSequentialAllocator') and never
        //       destroyed.
        //     - After the (dynamically allocated) source object is
        //       deleted and its footprint erased (see concern 4).
        //
        // Testing:
        //   bdem_RowLayout(const bdem_RowLayout& original, *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Copy Constructor" << endl
                          << "========================" << endl;

        if (verbose) cout <<
             "\nCopy construct values with varied representations." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }

                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                // Create control object w.
                Obj mW(&testAllocator); gg(&mW, SPEC); const Obj& W = mW;
                LOOP_ASSERT(ti, curLen == W.length()); // same lengths
                if (veryVerbose) { cout << "\t"; P(W); }

                // Stretch capacity of x object by different amounts.

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                    const int N = EXTEND[ei];
                    if (veryVerbose) { cout << "\t\t"; P(N) }

                    Obj *pX = new Obj(&testAllocator);
                    Obj &mX = *pX;              stretchRemoveAll(&mX, N);
                    const Obj& X = mX;          gg(&mX, SPEC);
                    if (veryVerbose) { cout << "\t\t"; P(X); }

                    {                                   // No allocator.
                        const Obj Y0(X);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y0); }
                        LOOP2_ASSERT(SPEC, N, W == Y0);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    {                                   // Null allocator.
                        const Obj Y1(X, (bslma_Allocator *) 0);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y1); }
                        LOOP2_ASSERT(SPEC, N, W == Y1);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    BEGIN_BSLMA_EXCEPTION_TEST {        // Test allocator.
                      const Obj Y2(X, &testAllocator);
                      if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                      LOOP2_ASSERT(SPEC, N, W == Y2);
                      LOOP2_ASSERT(SPEC, N, W == X);
                    } END_BSLMA_EXCEPTION_TEST

                    {                                   // Buffer Allocator.
                        char memory[4096];
                        bdema_BufferedSequentialAllocator a(memory,
                                                            sizeof memory);
                        Obj *Y = new(a.allocate(sizeof(Obj))) Obj(X, &a);
                        if (veryVerbose) { cout << "\t\t\t"; P(*Y); }
                        LOOP2_ASSERT(SPEC, N, W == *Y);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    {                             // with 'original' destroyed
                        const Obj Y2(X, &testAllocator);

                        // testAllocator will erase the footprint of pX
                        // preventing further reference to this object.

                        delete pX;
                        if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        //LOOP2_ASSERT(SPEC, N, W == X);  // X (*pX) is gone
                    }
                }
            }
        }
      } break;
      case 6: {
        //--------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        // Concerns:
        //   1. operator== is implemented in terms of the offset and element
        //      type map.  It is therefore sufficient to verify that a
        //      difference in value of any of the two arrays of the given
        //      objects implies inequality.  We note here that the offsetmap
        //      is a parameterized type, where we compare lhs and rhs with the
        //      same parameter type.  Therefore, we do not consider the
        //      possibility that the lhs will have different size and alignment
        //      than the rhs.
        //
        //   2. Also tests that no other internal state information is being
        //      considered and verifies that 'operator==' reports true when
        //      applied to any two objects whose internal representations may
        //      be different yet still represent the same value.  Differences
        //      can be in length of the map and allocated memory.  Neither
        //      'lhs' nor 'rhs' values' may be modified.
        //
        // Plan:
        //   First specify a set S of unique object values having various minor
        //   or subtle differences, ordered by non-decreasing length.  Verify
        //   the correctness of 'operator==' and 'operator!=' using all
        //   elements (u, v) of the cross product S X S.
        //
        //   Next specify a second set S' containing a representative variety
        //   of (black-box) box values ordered by increasing (logical) length.
        //   For each value in S', construct an object x along with a sequence
        //   of similarly constructed duplicates x1, x2, ..., xN.  Attempt to
        //   affect every aspect of white-box state by altering each xi in a
        //   unique way.  Verify correctness of 'operator==' and 'operator!='
        //   by asserting that each element in { x, x1, x2, ..., xN } is
        //   equivalent to every other element.
        //
        // Testing:
        //   bool operator==(const bdem_RowLayout& lhs,
        //                   const bdem_RowLayout& rhs);
        //   bool operator!=(const bdem_RowLayout& lhs,
        //                   const bdem_RowLayout& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Equality Operators" << endl
                          << "==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of similar values (u, v) in S X S." << endl;
        {
            static const char *SPECS[] = {
                "",
                "A",      "B",
                "AA",     "AB",     "BB",     "BA",
                "AAA",    "BAA",    "ABA",    "AAB",
                "AAAA",   "BAAA",   "ABAA",   "AABA",   "AAAB",
                "AAAAA",  "BAAAA",  "ABAAA",  "AABAA",  "AAABA",  "AAAAB",
                "AAAAAA", "BAAAAA", "AABAAA", "AAABAA", "AAAAAB",
                "AAAAAAA",          "BAAAAAA",          "AAAAABA",
                "AAAAAAAA",         "ABAAAAAA",         "AAAAABAA",
                "AAAAAAAAA",        "AABAAAAAA",        "AAAAABAAA",
                "AAAAAAAAAA",       "AAABAAAAAA",       "AAAAABAAAA",
            0}; // Null string required as last element.

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const U_SPEC = SPECS[ti];
                const int curLen = (int)strlen(U_SPEC);

                Obj mU(&testAllocator); gg(&mU, U_SPEC); const Obj& U = mU;
                LOOP_ASSERT(ti, curLen == U.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing lhs objects of length "
                                      << curLen << '.' << endl;
                    LOOP_ASSERT(U_SPEC, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) { P_(ti); P_(U_SPEC); P(U); }

                for (int tj = 0; SPECS[tj]; ++tj) {
                    const char *const V_SPEC = SPECS[tj];
                    Obj mV(&testAllocator); gg(&mV, V_SPEC); const Obj& V = mV;

                    if (veryVerbose) { cout << "  "; P_(tj); P_(V_SPEC); P(V);}
                    const bool isSame = ti == tj;
                    LOOP2_ASSERT(ti, tj,  isSame == (U == V));
                    LOOP2_ASSERT(ti, tj, !isSame == (U != V));
                }
            }
        }

        if (verbose) cout << "\nCompare objects of equal value having "
                             "potentially different internal state." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "AB",     "ABC",    "ABCD",   "ABCDE",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing objects having (logical) "
                                         "length " << curLen << '.' << endl;
                    LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                Obj mX(&testAllocator); gg(&mX, SPEC); const Obj& X = mX;
                LOOP_ASSERT(ti, curLen == X.length()); // same lengths
                if (veryVerbose) { cout << "\t\t"; P_(ti); P_(SPEC); P(X)}

                for (int u = 0; u < NUM_EXTEND; ++u) {
                    const int U_N = EXTEND[u];
                    Obj mU(&testAllocator);  stretchRemoveAll(&mU, U_N);
                    const Obj& U = mU;       gg(&mU, SPEC);

                    if (veryVerbose) { cout << "\t\t\t"; P_(U_N); P(U)}

                    // compare canonical representation with every variation

                    LOOP2_ASSERT(SPEC, U_N, 1 == (U == X));
                    LOOP2_ASSERT(SPEC, U_N, 1 == (X == U));
                    LOOP2_ASSERT(SPEC, U_N, 0 == (U != X));
                    LOOP2_ASSERT(SPEC, U_N, 0 == (X != U));

                    for (int v = 0; v < NUM_EXTEND; ++v) {
                        const int V_N = EXTEND[v];
                        Obj mV(&testAllocator);  stretchRemoveAll(&mV, V_N);
                        const Obj& V = mV;       gg(&mV, SPEC);

                        static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                        if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                            cout << "\t| "; P_(U_N); P_(V_N); P_(U); P(V);
                            --firstFew;
                        }

                        // compare every variation with every other one

                        LOOP3_ASSERT(SPEC, U_N, V_N, 1 == (U == V));
                        LOOP3_ASSERT(SPEC, U_N, V_N, 0 == (U != V));
                    }
                }
            }
        }
      } break;
      case 5: {
        //---------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        // Concerns:
        //   1. That the output is reported consistently.
        //
        //   Since the output operator is layered on direct accessors, it is
        //   sufficient to test only the output *format* (and to ensure that no
        //   additional characters are written past the terminating null).
        //
        // Plan:
        //   For each of a small representative set of object values, ordered
        //   by increasing length, use 'ostrstream' to write that object's
        //   value to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.
        //
        // Testing:
        //   ostream& operator<<(ostream& os, const bdem_RowLayout& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Output (<<) Operator" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec        output format                // ADJUST
                //----  --------    ----------------------------------------
                { L_,    "",        "Types: [ ] Offsets: [ ] "
                                    "TotalOffset: 0"                       },
                { L_,    "A",       "Types: [ CHAR ] Offsets: [ 0 ] "
                                    "TotalOffset: 1"                       },
                { L_,    "B",       "Types: [ SHORT ] Offsets: [ 0 ] "
                                    "TotalOffset: 2"                       },
                { L_,    "BC",      "Types: [ SHORT INT ] Offsets: [ 0 4 ] "
                                    "TotalOffset: 8"                       },
                { L_,    "ABCDE",   "Types: [ CHAR SHORT INT INT64 FLOAT ] "
                                    "Offsets: [ 0 2 4 8 16 ] "
                                    "TotalOffset: 20"                      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold
                                   // output string.
            const char Z1 = (char) 0xff;  // Used to represent an unset char.
            const char Z2 = 0x00; // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int LINE            = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const FMT  = DATA[ti].d_fmt_p;
                const int curLen = (int) strlen(SPEC);

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                Obj mX(&testAllocator);  const Obj& X = gg(&mX, SPEC);
                LOOP_ASSERT(ti, curLen == X.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                if (verbose) cout << "\t\tSpec = \"" << SPEC << '"' << endl;
                if (veryVerbose) P(X.totalOffset());
                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl<<FMT<<endl;
                ostrstream out1(buf1, SIZE);  out1 << X << ends;
                ostrstream out2(buf2, SIZE);  out2 << X << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl<<buf1<<endl;

                const int SZ = (int) strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }
      } break;
      case 4: {
        //--------------------------------------------------------------------
        // TESTING DIRECT ACCESSORS
        // Concerns:
        //   Having implemented several helper mechanisms:
        //   'stretchState', 'stretch', and 'stretchRemoveAll', we now would
        //   like to test the direct accessor functions:
        //     - length() const
        //     - bdem_ElemType::Type getType(int index) const
        //     - int getOffset(int index) const
        //     - int totalOffset() const
        //     - const my_ElemTypeArray& elemTypeArray() const
        //
        //   Specifically:
        //
        //   1. The 'length' method returns the correct number of elements
        //      inserted.
        //   2. The 'getType' and 'getOffset' methods return the correct values
        //      for the items inserted at specified positions.
        //   3. The 'totaloffset' is correct after insertion/removal of
        //      elements.
        //
        // Plan:
        //   A subset of element types will be inserted into the
        //   'bdem_RowLayout'.  Based on input from table DATA, the size and
        //   alignment of the types to be inserted will be set, as will the
        //   map's cursor position, using the helper routine.  At each case,
        //   we compare the length with the expected length and verify the
        //   expected type exists at the specified index.  We also verify that
        //   the type at the specified index has the correct offset and that
        //   the total offset is correct.  After insertion of elements, a test
        //   of the 'elemTypeArray' method is done by iterating through those
        //   arrays and comparing the results in each position with those
        //   returned by 'getType' and 'getOffset'.
        //
        // Testing:
        //   bdem_ElemType::Type getType(int index) const;
        //   int getOffset(int index) const;
        //   int totalOffset() const;
        //   const my_ElemTypeArray& elemTypeArray() const;
        //   int length() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Direct Accessors" << endl
                          << "========================" << endl;

        static const struct {
            int         d_lineNum;          // source line number
            int         d_cp;               // cursor position
            int         d_eoff;             // expected offset
            int         d_etot;             // expected total
        } DATA[] = {
            // line  cursor  expected offset  expected total
            // ----  ------  ---------------  --------------
            {  L_,   0,       0,               4            },
            {  L_,   1,       4,               8            },
            {  L_,   2,       4,               8            },
            {  L_,   3,       4,               8            },
            {  L_,   4,       4,               8            },
            {  L_,   5,       8,              12            },
            {  L_,   6,       8,              12            },
            {  L_,   7,       8,              12            },
            {  L_,   8,       8,              12            },
            {  L_,   9,      12,              16            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting direct accessors." << endl;
        {
            for (int i = 0; i < NUM_DATA; i++) {
                const int LINE   = DATA[i].d_lineNum;
                const int CURSOR = DATA[i].d_cp;

                for (int j = 0; j < NUM_TYPES; j++) {
                    Obj DD;

                    stretchState(&DD, CURSOR);
                    int tmp = DD.length();

                    // Insert an element at the cursor position.
                    DD.insert(DD.totalOffset(),
                              typesLookupTable[(bdem_ElemType::Type) j]);

                    // ASSERT we have inserted the element at the designated
                    // place.
                   LOOP_ASSERT(LINE, j == DD[CURSOR].attributes()->d_elemEnum);

                    // ASSERT we have the correct offset.
                    const int JTH_ALIGN =
                         typesLookupTable[(bdem_ElemType::Type)j]->d_alignment;
                    const int JTH_OFFSET =
                                   (CURSOR + JTH_ALIGN - 1) & ~(JTH_ALIGN - 1);
                    LOOP_ASSERT(LINE, JTH_OFFSET == DD[CURSOR].offset());

                    // ASSERT we have the correct total offset.
                    LOOP_ASSERT(LINE,
                                JTH_OFFSET +
                               typesLookupTable[(bdem_ElemType::Type)j]->d_size
                                                          == DD.totalOffset());

                    // ASSERT we have the correct length.
                    LOOP_ASSERT(LINE, tmp + 1 == DD.length());

                    // ASSERT we have consistency for 'elemTypeArray'.
                    my_ElemTypeArray e_array;
                    getElemTypeArray(&e_array,DD);
                    bsl::vector<int> o_array;
                    getOffsetArray(&o_array,DD);

                    // ASSERT we have consistency with respect to the length
                    // of the element and offset arrays.
                    ASSERT(e_array.size() == o_array.size());
                    for (int k = 0; k < (int)e_array.size(); k++) {
                        LOOP_ASSERT(LINE,
                                 DD[k].attributes()->d_elemEnum == e_array[k]);
                        LOOP_ASSERT(LINE, DD[k].offset() == o_array[k]);
                    }
                }
            }
        }
      } break;
      case 3: {
        //--------------------------------------------------------------------
        // TESTING HELPER FUNCTIONS
        // Concerns:
        //   1. With respect to the generator function 'gg', all symbols must
        //      be recognized successfully, and all possible state transitions
        //      must be verified.  In addition, we test for appropriate
        //      response on invalid characters in the stream.
        //
        //   2. That the 'stretchState' routine produces objects configured
        //      based on the specified input parameters for cursor position,
        //      size and alignment.
        //
        //   3. Internal memory organization is behaving as intended
        //      with respect to 'stretch' and 'stretchRemoveAll'.
        //
        //   Having demonstrated that our primary manipulators work as expected
        //   under normal conditions, we want to verify that providing a valid
        //   set of input produces objects of the expected internal state.
        //   This state has a predetermined total offset, as well as size and
        //   alignment given for a designated type, with all other element
        //   types being held at zero.
        //
        //   We want also to make trustworthy some additional test helper
        //   functionality that we will use within the first 10 test cases:
        //   - 'stretch'          Tested separately to observe stretch occurs.
        //   - 'stretchRemoveAll' Deliberately implemented using 'stretch'.
        //
        //   Finally we want to make sure that we can rationalize the internal
        //   memory management with respect to the primary manipulators (i.e.,
        //   precisely when new blocks are allocated and deallocated).
        //
        // Plan:
        //   To verify 'gg', we test the state switch from empty to append of
        //   1 of the 22 types.  Test 'gg' on a number of correct
        //   combinations.  Test 'ggg' on a number of incorrect symbols in
        //   place.
        //
        //   To verify 'stretchState', we run through the total number of
        //   cases and verify that the created object for each case has the
        //   appropriate total offset, size and alignment for the specified
        //   type.
        //
        //   To verify that the stretching functions work as expected (and to
        //   cross-check that internal memory is being managed as intended),
        //   create a depth-ordered enumeration of initial values and sizes
        //   by which to extend the initial value.  Record as expected values
        //   the total number of memory blocks allocated during the first and
        //   second modifications of each object.  For each test vector,
        //   construct two identical objects X and Y and bring each to the
        //   initial state.  Assert that the memory allocation for the two
        //   operations are identical and consistent with the first expected
        //   value.  Next apply the 'stretch' and 'stretchRemoveAll'
        //   functions to X and Y (respectively) and again compare the memory
        //   allocation characteristics for the two functions.  Note that we
        //   will track the *total* number of *blocks* allocated as well as
        //   the *current* number of *bytes* in use -- this to measure
        //   different aspects of operation while remaining insensitive to
        //   the array 'Element' size.
        //
        // Testing:
        //   Obj& gg(Obj *object, const char *spec);
        //   int ggg(Obj *object, const char *spec, int vF = 1);
        //   void stretch(Obj *object, int size);
        //   void stretchRemoveAll(Obj *object, int size);
        //   void stretchState(Obj *object, int cursor);
        // ----------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'gg', 'ggg' and other helpers" << endl
                          << "=====================================" << endl;
        {
            Obj x1(Z); const Obj& X1= x1;
            {
                if (verbose)  cout << "\nTesting 'gg' with empty string."
                                   << endl;
                gg(&x1, "");  ASSERT(0 == X1.length());
            }
            if (verbose) cout << "\nTesting 'gg' all types."  << endl;

            // Apply remove all, then append one of the 32 types and then
            // test length.
            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~%"; // The % gets overwritten.
                spec[1] = bdemType[i];

                if (veryVerbose) cout << "testing 'gg' using " << spec << endl;

                gg(&x1, spec);
                LOOP_ASSERT(i, 1 == X1.length());
                if (veryVerbose) PS(x1);
            }
            if (verbose)
                cout << endl
                     << "Testing Primitive Generator Function 'gg'" << endl
                     << "=========================================" << endl;

            if (verbose) cout << "\nTesting generator on valid specs." << endl;
            {
                const int SZ = 10;
                const Element NUL = (Element) 0;
                static const struct {
                    int         d_lineNum;          // source line number
                    const char *d_spec_p;           // specification string
                    int         d_length;           // expected length
                    Element     d_elements[SZ];     // expected element values
                } DATA[] = {
                    //line  spec            length  elements
                    //----  --------------  ------  ------------------------
                    { L_,   "",             0,      { NUL }                 },

                    { L_,   "A",            1,      { VA }                  },
                    { L_,   "B",            1,      { VB }                  },
                    { L_,   "~",            0,      { NUL }                 },

                    { L_,   "CD",           2,      { VC, VD }              },
                    { L_,   "E~",           0,      { NUL }                 },
                    { L_,   "~E",           1,      { VE }                  },
                    { L_,   "~~",           0,      { NUL }                 },

                    { L_,   "ABC",          3,      { VA, VB, VC }          },
                    { L_,   "~BC",          2,      { VB, VC }              },
                    { L_,   "A~C",          1,      { VC }                  },
                    { L_,   "AB~",          0,      { NUL }                 },
                    { L_,   "~~C",          1,      { VC }                  },
                    { L_,   "~B~",          0,      { NUL }                 },
                    { L_,   "A~~",          0,      { NUL }                 },
                    { L_,   "~~~",          0,      { NUL }                 },

                    { L_,   "ABCD",         4,      { VA, VB, VC, VD }      },
                    { L_,   "~BCD",         3,      { VB, VC, VD }          },
                    { L_,   "A~CD",         2,      { VC, VD }              },
                    { L_,   "AB~D",         1,      { VD }                  },
                    { L_,   "ABC~",         0,      { NUL }                 },

                    { L_,   "ABCDE",        5,      { VA, VB, VC, VD, VE }  },
                    { L_,   "~BCDE",        4,      { VB, VC, VD, VE }      },
                    { L_,   "AB~DE",        2,      { VD, VE }              },
                    { L_,   "ABCD~",        0,      { NUL }                 },
                    { L_,   "A~C~E",        1,      { VE }                  },
                    { L_,   "~B~D~",        0,      { NUL }                 },

                    { L_,   "~CBA~~ABCDE",  5,      { VA, VB, VC, VD, VE }  },

                    { L_,   "ABCDE~CDEC~E", 1,      { VE }                  },
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                int oldLen = -1;

                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const int LINE         = DATA[ti].d_lineNum;
                    const char *const SPEC = DATA[ti].d_spec_p;
                    const int LENGTH       = DATA[ti].d_length;
                    const Element *const e = DATA[ti].d_elements;
                    const int curLen       = (int)strlen(SPEC);

                    Obj mX(&testAllocator);
                    const Obj& X = gg(&mX, SPEC);   // original spec

                    static const char *const MORE_SPEC =
                                                      "~ABCDEABCDEABCDEABCDE~";
                    char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

                    Obj mY(&testAllocator);
                    const Obj& Y = gg(&mY, buf);    // extended spec

                    if (curLen != oldLen) {
                        if (verbose) cout << "\tof length "
                                          << curLen << ':' << endl;
                        LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                        oldLen = curLen;
                    }

                    if (veryVerbose) {
                        cout << "\t\t   Spec = \"" << SPEC << '"' << endl;
                        cout << "\t\tBigSpec = \"" << buf << '"' << endl;
                        cout << "\t\t\t"; P(X);
                        cout << "\t\t\t"; P(Y);
                    }

                    LOOP_ASSERT(LINE, LENGTH == X.length());
                    LOOP_ASSERT(LINE, LENGTH == Y.length());
                    for (int i = 0; i < LENGTH; ++i) {
                        LOOP2_ASSERT(LINE, i, e[i]->d_elemEnum ==
                                                X[i].attributes()->d_elemEnum);
                        LOOP2_ASSERT(LINE, i, e[i]->d_elemEnum ==
                                                Y[i].attributes()->d_elemEnum);
                    }
                }
            }

            if (verbose)
                cout << "\nTesting generator on invalid specs." << endl;
            {
                static const struct {
                    int         d_lineNum;  // source line number
                    const char *d_spec_p;   // specification string
                    int         d_index;    // offending character index
                } DATA[] = {
                    //line  spec            index
                    //----  -------------   -----
                    { L_,   "",             -1,     }, // control

                    { L_,   "~",            -1,     }, // control
                    { L_,   " ",             0,     },
                    { L_,   ".",             0,     },

                    { L_,   "AE",           -1,     }, // control
                    { L_,   ",E",            0,     },
                    { L_,   "A/",            1,     },
                    { L_,   ".~",            0,     },
                    { L_,   "~!",            1,     },
                    { L_,   "  ",            0,     },

                    { L_,   "ABC",          -1,     }, // control
                    { L_,   " BC",           0,     },
                    { L_,   "A C",           1,     },
                    { L_,   "AB ",           2,     },
                    { L_,   "?#:",           0,     },
                    { L_,   "   ",           0,     },

                    { L_,   "ABCDE",        -1,     }, // control
                    { L_,   ",BCDE",         0,     },
                    { L_,   "AB,DE",         2,     },
                    { L_,   "ABCD,",         4,     },
                    { L_,   "A,C,E",         1,     },
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                int oldLen = -1;
                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const int LINE         = DATA[ti].d_lineNum;
                    const char *const SPEC = DATA[ti].d_spec_p;
                    const int INDEX        = DATA[ti].d_index;
                    const int curLen       = (int)strlen(SPEC);

                    Obj mX(&testAllocator);

                    if (curLen != oldLen) {
                        if (verbose) cout << "\tof length "
                                          << curLen << ':' << endl;
                        LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                        oldLen = curLen;
                    }

                    if (veryVerbose)
                        cout << "\t\tSpec = \"" << SPEC << '"' << endl;

                    int result = ggg(&mX, SPEC, veryVerbose);
                    LOOP_ASSERT(LINE, INDEX == result);
                }
            }
        }

        if (verbose) cout <<
             "\nTesting 'stretch' and 'stretchRemoveAll'." << endl;
        {
            static const struct {
                int         d_lineNum;       // source line number
                const char *d_spec_p;        // specification string
                int         d_size;          // amount to grow (also length)
                int         d_firstResize;   // total blocks allocated
                int         d_secondResize;  // total blocks allocated

                // Note: total blocks (first/second Resize) and whether or not
                // 'removeAll' deallocates memory depends on 'Element' type.
            } DATA[] = {
                //line  spec            size     firstResize   secondResize
                //----  --------        ----     -----------   ------------
                { L_,   "",             0,       0,            0    },

                { L_,   "",             1,       0,            1    },
                { L_,   "A",            0,       1,            0    },

                { L_,   "",             2,       0,            2    },
                { L_,   "A",            1,       1,            1    },
                { L_,   "AB",           0,       2,            0    },

                { L_,   "",             3,       0,            3    },
                { L_,   "A",            2,       1,            2    },
                { L_,   "AB",           1,       2,            1    },
                { L_,   "ABC",          0,       3,            0    },

                { L_,   "",             4,       0,            3    },
                { L_,   "A",            3,       1,            2    },
                { L_,   "AB",           2,       2,            1    },
                { L_,   "ABC",          1,       3,            0    },
                { L_,   "ABCD",         0,       3,            0    },

                { L_,   "",             5,       0,            4    },
                { L_,   "A",            4,       1,            3    },
                { L_,   "AB",           3,       2,            2    },
                { L_,   "ABC",          2,       3,            1    },
                { L_,   "ABCD",         1,       3,            1    },
                { L_,   "ABCDE",        0,       4,            0    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int size         = DATA[ti].d_size;
                const int firstResize  = DATA[ti].d_firstResize;
                const int secondResize = DATA[ti].d_secondResize;
                const int curLen       = (int) strlen(SPEC);
                const int curDepth     = curLen + size;

                Obj mX(&testAllocator);  const Obj& X = mX;
                Obj mY(&testAllocator);  const Obj& Y = mY;

                if (curDepth != oldDepth) {
                    if (verbose) cout << "\ton test vectors of depth "
                                      << curDepth << '.' << endl;
                    LOOP_ASSERT(LINE, oldDepth <= curDepth); // non-decreasing
                    oldDepth = curDepth;
                }

                if (veryVerbose) {
                    cout << "\t\t"; P_(SPEC); P(size);
                    P_(firstResize); P_(secondResize);
                    P_(curLen);      P(curDepth);
                }

                // Create identical objects using the gg function.
                {
                    int blocks1A = testAllocator.numBlocksTotal();
                    int bytes1A  = testAllocator.numBytesInUse();

                    gg(&mX, SPEC);

                    int blocks2A = testAllocator.numBlocksTotal();
                    int bytes2A  = testAllocator.numBytesInUse();

                    gg(&mY, SPEC);

                    int blocks3A = testAllocator.numBlocksTotal();
                    int bytes3A  = testAllocator.numBytesInUse();

                    int blocks12A = blocks2A - blocks1A;
                    int bytes12A  = bytes2A - bytes1A;

                    int blocks23A = blocks3A - blocks2A;
                    int bytes23A  = bytes3A - bytes2A;

                    if (veryVerbose) {
                        P_(bytes12A);  P_(bytes23A);
                        P_(blocks12A); P(blocks23A);
                    }

                    LOOP_ASSERT(LINE, curLen == X.length()); // same lengths
                    LOOP_ASSERT(LINE, curLen == Y.length()); // same lengths

                    LOOP_ASSERT(LINE, firstResize == blocks12A);

                    LOOP_ASSERT(LINE, blocks12A == blocks23A);
                    LOOP_ASSERT(LINE, bytes12A == bytes23A);
                }

                // Apply both functions under test to the respective objects.
                {
                    int blocks1B = testAllocator.numBlocksTotal();
                    int bytes1B  = testAllocator.numBytesInUse();

                    stretch(&mX, size);

                    int blocks2B = testAllocator.numBlocksTotal();
                    int bytes2B  = testAllocator.numBytesInUse();

                    stretchRemoveAll(&mY, size);

                    int blocks3B = testAllocator.numBlocksTotal();
                    int bytes3B  = testAllocator.numBytesInUse();

                    int blocks12B = blocks2B - blocks1B;
                    int bytes12B  = bytes2B - bytes1B;

                    int blocks23B = blocks3B - blocks2B;
                    int bytes23B  = bytes3B - bytes2B;

                    if (veryVerbose) {
                        P_(bytes12B);  P_(bytes23B);
                        P_(blocks12B); P(blocks23B);
                    }

                    LOOP_ASSERT(LINE, curDepth == X.length());
                    LOOP_ASSERT(LINE,        0 == Y.length());

                    LOOP_ASSERT(LINE, secondResize == blocks12B);

                    LOOP_ASSERT(LINE, blocks12B == blocks23B); // Always true.

                    LOOP_ASSERT(LINE, bytes12B == bytes23B);   // True for POD
                } // else > or >=.
            }
        }

        if (verbose) cout << "\nTesting 'stretchState'." << endl;
        {
            static const struct {
                int         d_lineNum;          // source line number
                int         d_cp;               // cursor position
                int         d_eoff;             // expected offset
                int         d_ecurr;            // expected cursor
            } DATA[] = {
                // line  cursor  expected offset  expected cursor
                // ----  ------  ---------------  ---------------
                {  L_,   0,      0,                1           },
                {  L_,   0,      0,                2           },
                {  L_,   0,      0,                4           },
                {  L_,   0,      0,                8           },
                // ----  ------  ---------------  -------------
                {  L_,   1,      2,                3           },
                {  L_,   1,      1,                3           },
                {  L_,   1,      8,               12           },
                {  L_,   1,      4,               12           },
                // ----  ------  ---------------  -------------
                {  L_,   2,      2,                3           },
                {  L_,   2,      2,                4           },
                {  L_,   2,      8,               12           },
                {  L_,   2,      4,               12           },
                // ----  ------  ---------------  -------------
                {  L_,   4,      4,                5           },
                {  L_,   4,      4,                6           },
                {  L_,   4,      8,               12           },
                {  L_,   4,      4,               12           },
                // ----  ------  ---------------  -------------
                {  L_,   8,      8,                9           },
                {  L_,   8,      8,               10           },
                {  L_,   8,      8,               12           },
                {  L_,   8,      8,               16           },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; i++) {
                const int LINE   = DATA[i].d_lineNum;
                const int CURSOR = DATA[i].d_cp;
                Obj DD;
                for (int j = 0; j < NUM_TYPES; j++) {
                    stretchState(&DD, CURSOR);
                    // ASSERT we have the correct total offset.
                    LOOP_ASSERT(LINE, CURSOR == DD.totalOffset());
                }
            }
        }
      } break;
      case 2: {
        //--------------------------------------------------------------------
        // BOOTSTRAP TEST ( Primary manipulators test ).
        // Concerns:
        //   1. The default 'bdem_RowLayout' constructor
        //      works properly, with any ( or no ) valid allocator supplied and
        //      in the presence of exceptions.
        //   2. The destructor works properly as implicitly tested in the
        //      various scopes of this test and in the presence of exceptions.
        //   3. Test the primary manipulators:
        //
        //      void insert(int di, bdem_ElemType::Type item); (black-box)
        //      void removeAll();                              (white-box)
        //
        //    wherein we are concerned about the following for 'insert':
        //        a. Insertion process places an element at the designated
        //           index of the specified bdem_ElemType.
        //        b. After insertion, we have the correct offset for
        //           the item inserted given varying initial cursor positions.
        //        c. After insertion, we have the correct total offset
        //           given varying initial cursor positions.
        //        d. That we have capacity increase as needed.
        //
        //    wherein we are concerned about the following for 'removeAll':
        //        a. The method deletes all elements from the offset map.
        //        b. The total offset is reset to zero.
        //        c. Does not attempt to free our acquire memory.
        //
        // Plan:
        //   a) Create an offset map using the default constructor:
        //        - With and without passing in an allocator.
        //        - In the presence of exceptions during memory allocations
        //          using a 'bslma_TestAllocator' and varying its *allocation*
        //          *limit*.
        //        - Where the object is constructed entirely in static memory
        //          (using a 'bdema_BufferedSequentialAllocator') and never
        //          destroyed.
        //
        //   b)   - At each iteration of a loop, we vary the initial cursor
        //          position of the map and insert an integer.
        //        - Verify that there is an integer at the expected position.
        //        - Verify we have the correct offset for the integer
        //          based on its size and alignment.
        //        - Verify we have the correct total offset for the
        //          'bdem_RowLayout'.
        //        - Vary the initial cursor position of the map and repeat the
        //          test for those variations.
        //
        //   c)   - At each iteration of a loop, we vary the initial cursor
        //          position of the map and insert a subset of elements and
        //          for each element inserted we:
        //        - Verify that it is inserted in the correct position.
        //        - Verify that it has the correct offset based on its
        //          size and alignment.
        //        - Verify that we have the correct total offset for the
        //          'bdem_RowLayout'.
        //
        //   d) Create an offset map and, in a loop of two iterations:
        //        - insert a subset of elements
        //        - call 'removeAll'
        //          * Verify that the 'bdem_RowLayout' has no elements
        //            after the call.
        //          * Verify that the total offset is 0.
        //
        // Testing:
        //   bdem_RowLayout(bslma_Allocator *ba = 0);
        //   ~bdem_RowLayout();
        //   void removeAll();
        //   BOOTSTRAP: void insert(int di, bdem_ElemType::Type item);
        //--------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Primary Manipulators" << endl
                          << "============================" << endl;

        // a.
        if (verbose) cout
            << "\nTesting 'bdem_RowLayout()' Constructor." << endl;
        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X((bslma_Allocator *)0);
            if (veryVerbose) { T_(); T_(); PS(X); }
            ASSERT(0 == X.length());
        }
        if (verbose) cout << "\tPassing in an allocator." << endl;
        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(Z);
            if (veryVerbose) { T_(); T_(); PS(X); }
            ASSERT(0 == X.length());
        }
        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
          BEGIN_BSLMA_EXCEPTION_TEST {
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Default Ctor" << endl;
            const Obj X(&testAllocator);
            if (veryVerbose) { T_(); T_(); PS(X); }
            ASSERT(0 == X.length());
          } END_BSLMA_EXCEPTION_TEST
        }
        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[4096];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);
            void *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(&a);
            ASSERT(doNotDelete);

            // No destructor is called; will produce memory leak in purify if
            // internal allocators are not hooked up properly.
        }

        // Test data shared by subcases b and c.
        static const struct {
            int         d_lineNum;          // source line number
            int         d_cp;               // cursor position
            int         d_eoff;             // expected offset
            int         d_etot;             // expected total
        } DATA[] = {
            // line  cursor  expected offset  expected total
            // ----  ------  ---------------  --------------
            {  L_,   0,       0,               4            },
            {  L_,   1,       4,               8            },
            {  L_,   2,       4,               8            },
            {  L_,   3,       4,               8            },
            {  L_,   4,       4,               8            },
            {  L_,   5,       8,              12            },
            {  L_,   6,       8,              12            },
            {  L_,   7,       8,              12            },
            {  L_,   8,       8,              12            },
            {  L_,   9,      12,              16            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // b.
        if (verbose) cout << "\nTesting insert of INT." << endl;
        {
            for (int i = 0; i < NUM_DATA; i++) {
                const int LINE   = DATA[i].d_lineNum;
                const int CURSOR = DATA[i].d_cp;
                const int OFFSET = DATA[i].d_eoff;
                const int TOTAL  = DATA[i].d_etot;

                Obj DD;

                // Set cursor position.
                for (int j = 0; j < CURSOR; j++) {
                    DD.append(typesLookupTable[bdem_ElemType::BDEM_CHAR]);
                }

                // Insert an INT at the cursor position.
                DD.insert(DD.totalOffset(),
                          typesLookupTable[bdem_ElemType::BDEM_INT]);

                // ASSERT we have inserted an INT at the designated place.
                LOOP_ASSERT(LINE, bdem_ElemType::BDEM_INT ==
                                          DD[CURSOR].attributes()->d_elemEnum);

                // ASSERT we have the correct offset.
                LOOP_ASSERT(LINE, OFFSET == DD[CURSOR].offset());

                // ASSERT we have the correct total offset.
                LOOP_ASSERT(LINE, TOTAL  == DD.totalOffset());
            }
        }

        // c.
        if (verbose) cout << "\nTesting insert for subset of elements."
                          << endl;
        {
            for (int i = 0; i < NUM_DATA; i++) {
                const int LINE   = DATA[i].d_lineNum;
                const int CURSOR = DATA[i].d_cp;

                for (int j = 0; j < NUM_TYPES; j++) {
                    Obj DD1;

                    // Set cursor position.
                    for (int k = 0; k < CURSOR; k++) {
                        DD1.append(typesLookupTable[bdem_ElemType::BDEM_CHAR]);
                    }

                    // Insert an element at the cursor position.
                    DD1.insert(DD1.totalOffset(),
                               typesLookupTable[(bdem_ElemType::Type)j]);

                    // ASSERT we have inserted the element
                    // at the designated place.
                    LOOP_ASSERT(LINE,
                                    j == DD1[CURSOR].attributes()->d_elemEnum);

                    // ASSERT we have the correct offset.
                    const int JTH_ALIGN =
                         typesLookupTable[(bdem_ElemType::Type)j]->d_alignment;
                    const int JTH_OFFSET =
                                   (CURSOR + JTH_ALIGN - 1) & ~(JTH_ALIGN - 1);
                    LOOP_ASSERT(LINE, JTH_OFFSET == DD1[CURSOR].offset());

                    // ASSERT we have the correct total offset.
                    LOOP_ASSERT(LINE,
                                JTH_OFFSET +
                                     typesLookupTable[(bdem_ElemType::Type)j]->
                                                                         d_size
                                                         == DD1.totalOffset());
                }
            }
        }

        // d.
        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            Obj DD2;
            for (int i = 0; i < 2; i++) {
                DD2.insert(0, typesLookupTable[bdem_ElemType::BDEM_CHAR]);
                DD2.insert(1, typesLookupTable[bdem_ElemType::BDEM_INT]);
                DD2.insert(2, typesLookupTable[bdem_ElemType::BDEM_DOUBLE]);
                DD2.insert(3, typesLookupTable[bdem_ElemType::BDEM_LIST]);
                DD2.insert(4, typesLookupTable[bdem_ElemType::BDEM_TABLE]);
                DD2.removeAll();
                ASSERT(0 == DD2.length());
                ASSERT(0 == DD2.totalOffset());
            }
        }
      } break;
      case 1: {
        //--------------------------------------------------------------------
        // BREATHING TEST
        // Concerns:
        //   We are concerned that the basic (value-semantic) functionality of
        //   'bdem_RowLayout' works properly.  We are concerned that the
        //   basic (value-semantic) functionality of 'bdem_RowLayout' works
        //   properly.
        //
        // Plan:
        //   We exercise basic functionality in 12 steps, inspired by the
        //   'bsl::vector<double>' test driver 11-step breathing test, to which
        //   we add a 12th step to exercise basic streaming functionality.
        //
        //   We will use 4 objects for our tests, x1, x2, x3, and x4.
        //
        //   After each step, we will confirm that a) the object we just
        //   modified is in the expected state (by checking 'length()' and
        //   other accessors) b) the object is equal/not equal to the other
        //   existing objects, and equal to itself.
        //
        //   The exercises:
        //      1) Build x1 using default constructor
        //          a) 0 == x1.length()
        //          b) x1 == x1
        //      2) Build x2 using copy c'tor from x1
        //          a) 0 == x2.length()
        //          b) x2 == x1
        //      3) Append a char to X1.
        //          a) 1 == x1.length()
        //          b) x1 != x2
        //      4) Append char to X2.
        //          a) 1 == x2.length()
        //          b) x1 == x2
        //      5) x1.removeAll()
        //          a) 0 == x1.length()
        //          b) x1 != x2
        //      6) Re-append char to x1
        //          a) 1 == x1.length()
        //          b) x1 == x2
        //      7) Empty x1, and then create empty x3 using default c'tor
        //          a) 0 == x3.length()
        //          b) x3 == x1 x3 != x2
        //      8) Create bdem_RowLayout x4(x2)
        //          a) 1 == x4.length()
        //          b) x4 == x2 x4 != x1 x4 != x3
        //      9) x1=x2, trying assignment of longer map to shorter
        //          a) 1 == x1.length()
        //          b) x1 == x2 x1 == x4 x1 != x3
        //     10) x4=x3,
        //          trying assignment of shorter map to longer
        //          a) 0 == x4.length()
        //          b) x4 == x3 x4 != x1 x4 != x2
        //     11) x2=x2, trying self-assignment
        //          a) 1 == x2.length()
        //          b) x2 == x2 x2 == x1 x2 != x3 x2 != x4
        //     12)
        //          stream x2 out, and then stream same data back in to x4
        //          a) 2 == x4.length()
        //          b) x4 == x2 x4 == x1 x4 != x3 x4 == x4
        //
        // Testing:
        //   This breathing test exercises basic functionality as a preliminary
        //   to the thorough testing done in subsequent cases.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Breathing Test" << endl
                          << "==============" << endl;

        if (verbose) cout << "\n 1. Create an object x1 (default ctor)."
                             "\t\t{ x1: }" << endl;
        Obj x1(Z);  const Obj& X1 = x1;
        ASSERT( 0 == X1.length());
        ASSERT((X1 == X1) == 1);      ASSERT((X1 != X1) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) {
            cout << "\n 2. Create a second object x2 (copy from x1)."
                    "\t{ x1:  x2: }" << endl;
        }
        Obj x2(X1, Z);  const Obj& X2 = x2;
        ASSERT( 0 == X2.length());
        ASSERT((X1 == X1) == 1);      ASSERT((X1 != X1) == 0);
        ASSERT((X1 == X2) == 1);      ASSERT((X1 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Append a char to x1."
                             "\t\t\t{ x1:V x2: }" << endl;
        x1.append(typesLookupTable[bdem_ElemType::BDEM_CHAR]);
        ASSERT(X1 != X2);
        ASSERT( 1 == X1.length());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Append a char to x2."
                             "\t\t{ x1:V x2:V }" << endl;
        x2.append(typesLookupTable[bdem_ElemType::BDEM_CHAR]);
        ASSERT(X1 == X2);
        ASSERT( 1 == X2.length());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Empty x1 using 'removeAll'."
                             "\t\t\t{ x1:  x2:V }" << endl;

        x1.removeAll();

        ASSERT( 0 == X1.length());
        ASSERT( 1 == X2.length());
        ASSERT((X1 == X1) == 1);      ASSERT((X1 != X1) == 0);
        ASSERT((X1 == X2) == 0);      ASSERT((X1 != X2) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Append the same elements back to x1."
                             "\t\t{ x1:V x2:V }" << endl;

        x1.append(typesLookupTable[bdem_ElemType::BDEM_CHAR]);

        ASSERT( 1 == X1.length());
        ASSERT((x1 == x1) == 1);      ASSERT((x1 != x1) == 0);
        ASSERT((x1 == x2) == 1);      ASSERT((x1 != x2) == 0);

        if (veryVerbose) PS(X1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Empty x1 and create (empty) x3."
                             "\t\t{ x1:  x2:V x3: }" << endl;
        x1.removeAll();
        ASSERT( 0 == X1.length());

        Obj x3(Z);  const Obj& X3 = x3;

        ASSERT( 0 == X3.length());

        ASSERT((X3 == X1) == 1);      ASSERT((X3 != X1) == 0);
        ASSERT((X3 == X2) == 0);      ASSERT((X3 != X2) == 1);
        ASSERT((X3 == X3) == 1);      ASSERT((X3 != X3) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Create x4 (copy of x2)."
                             "\t\t\t{ x1:  x2:V x3:  x4:V }" << endl;
        Obj x4(X2, Z);  const Obj& X4 = x4;  if (veryVerbose) PS(X4);

        ASSERT( 1 == X4.length());
        ASSERT((X4 == X1) == 0);      ASSERT((X4 != X1) == 1);
        ASSERT((X4 == X2) == 1);      ASSERT((X4 != X2) == 0);
        ASSERT((X4 == X3) == 0);      ASSERT((X4 != X3) == 1);
        ASSERT((X4 == X4) == 1);      ASSERT((X4 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) {
            cout << "\n 9. Assign x1 = x2 (empty becomes non-empty)."
                    "\t{ x1:V x2:V x3:  x4:V }" << endl;
        }
        x1 = X2;

        ASSERT( 1 == X1.length());
        ASSERT((X1 == X1) == 1);      ASSERT((X1 != X1) == 0);
        ASSERT((X1 == X2) == 1);      ASSERT((X1 != X2) == 0);
        ASSERT((X1 == X3) == 0);      ASSERT((X1 != X3) == 1);
        ASSERT((X1 == X4) == 1);      ASSERT((X1 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) {
            cout << "\n10. Assign x4 = x3 (non-empty becomes empty)."
                    "\t{ x1:V x2:V x3:  x4: }" << endl;
        }
        x4 = X3;
        ASSERT( 0 == X4.length());

        ASSERT((X4 == X1) == 0);      ASSERT((X4 != X1) == 1);
        ASSERT((X4 == X2) == 0);      ASSERT((X4 != X2) == 1);
        ASSERT((X4 == X3) == 1);      ASSERT((X4 != X3) == 0);
        ASSERT((X4 == X4) == 1);      ASSERT((X4 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n11. Assign x2 = x2 (aliasing)."
                             "\t\t\t{ x1:V x2:V x3:  x4: }" << endl;
        x2 = X2;
        ASSERT( 1 == X2.length());

        ASSERT((X2 == X1) == 1);      ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);      ASSERT((X2 != X2) == 0);
        ASSERT((X2 == X3) == 0);      ASSERT((X2 != X3) == 1);
        ASSERT((X2 == X4) == 0);      ASSERT((X2 != X4) == 1);

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
