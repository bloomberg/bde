// bdem_choiceheader.t.cpp                                            -*-C++-*-

#include <bdem_choiceheader.h>
#include <bdem_descriptor.h>
#include <bdem_elemtype.h>
#include <bdem_functiontemplates.h>
#include <bdem_properties.h>

#include <bdema_sequentialallocator.h>
#include <bdetu_unset.h>
#include <bdeu_printmethods.h>

#include <bdex_byteinstream.h>
#include <bdex_byteoutstream.h>
#include <bdex_testinstream.h>
#include <bdex_testoutstream.h>

#include <bslalg_typetraits.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This component is a low-level component that has no information about the
// various data types that can be stored in it.  It knows only about the
// catalog of descriptors that it gets constructed with.  Additionally the
// choice header can store values in one of two ways.  Either it can store the
// value inside its footprint or by allocating additional memory for it and
// then storing it there.
//
// So all our testing can be done by constructing a set of four data types,
// one that can be stored in the footprint of the choice header, the second
// that is too large for that and needs to be stored in allocated memory, and
// their counterparts that take an allocator.  Also, we need to define
// descriptors for each of those new types.
//
// After that we need to construct the choice header with various combinations
// of the newly constructed types and test the functionality as we would a
// value-semantic type.

//-----------------------------------------------------------------------------

// STATIC DATA MEMBERS
// [12] static const bdem_Descriptor d_choiceItemAttr;

// PRIVATE ACCESSORS
// [ 9] int bdexMinorVersion() const;

// CREATORS
// [ 4] bdem_ChoiceHeader(Catalog* catalog, Mode allocMode);
// [ 7] bdem_ChoiceHeader(const bdem_ChoiceHeader& orig, Mode allocMode);
// [ 4] ~bdem_ChoiceHeader();

// MANIPULATORS
// [ 8] bdem_ChoiceHeader& operator=(const bdem_ChoiceHeader& rhs);
// [ 9] bdexStreamIn(...);
// [10] void *makeSelection(int newSelector);
// [10] void *makeSelection(int newSelector, const void *value);
// [ 4] void *selectionPointer();
// [ 4] int& flags();
// [11] void clearNullnessBit();
// [11] void reset();

// ACCESSORS
// [ 4] int numSelections() const;
// [ 4] int selector() const;
// [ 4] const bdem_Descriptor *selectionDescriptor(int index) const;
// [ 4] const void *selectionPointer() const;
// [ 4] const int& flags() const;
// [ 4] Catalog *catalog() const;
// [ 4] Allocator *allocator() const;
// [ 4] Mode allocMode() const;
// [ 9] bdexStreamOut(...) const;
// [ 5] bsl::ostream& print(...) const;

// FREE OPERATORS
// [ 6] bool operator==(const ChoiceHeader& lhs, const ChoiceHeader& rhs);
// [ 6] bool operator!=(const ChoiceHeader& lhs, const ChoiceHeader& rhs);
// [ 5] ostream& operator<<(ostream& stream, const ChoiceHeader& rhs);
//
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS
// [ 3] HELPER FUNCTIONS
// [13] USAGE EXAMPLE

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
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                                   // Print ID and value.
#define PV(X) bsl::cout << #X " = " << bdeu_PrintMethods::print(bsl::cout, X)\
                        << bsl::endl;
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                                   // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                                   // P(X) w/o '\n'
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << bsl::flush;        // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdem_ChoiceHeader                    Obj;
typedef bdem_ChoiceHeader::DescriptorCatalog Catalog;
typedef bdem_Properties                      Prop;
typedef bdem_Descriptor                      Desc;
typedef bdem_ElemType                        EType;
typedef bdem_AggregateOption                 AggOption;

typedef bsls_Types::Int64                    Int64;

typedef bdet_Datetime                        Datetime;
typedef bdet_Date                            Date;
typedef bdet_Time                            Time;
typedef bdet_DatetimeTz                      DatetimeTz;
typedef bdet_DateTz                          DateTz;
typedef bdet_TimeTz                          TimeTz;

typedef bdem_AggregateOption                 AggOption;

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

//   FixedString<FOOTPRINT, ALLOC_TRAIT> is a test class that can be
//   instantiated with any footprint size and with or without the
//   'bslalg_TypeTraitsUsesBslmaAllocator' trait.  It has a descriptor
//   object that allows bdem aggregate types to construct, destruct,
//   copy, compare, and stream test objects.  There are typedefs for
//   four instantiations:
//
//   SmallString      - 32-byte object without allocator trait
//   SmallStringAlloc - 32-byte object with allocator trait
//   LargeString      - sizeof(bdem_ChoiceHeader)-byte object without
//                      allocator trait
//   LargeStringAlloc - sizeof(bdem_ChoiceHeader)-byte object with
//                      allocator trait

enum {
    SMALL_STRING_BUFSIZE = 16,
    LARGE_STRING_BUFSIZE = 32
};

template <int FOOTPRINT, bool ALLOC_TRAIT = false>
class FixedString {

    // Count constructor and destructor calls.
    static int s_constructorCount;
    static int s_destructorCount;

    // Calculate buffer size so that object size equals desired footprint.
    enum {
        BUFSIZE = FOOTPRINT - sizeof(bslma_Allocator*)
    };

    // Make the string size uniform across platforms.
    enum {
        STRING_BUFSIZE = BUFSIZE >= LARGE_STRING_BUFSIZE ? LARGE_STRING_BUFSIZE
                                                         : SMALL_STRING_BUFSIZE
    };

    bslma_Allocator *d_alloc;
    char             d_buffer[BUFSIZE];

  public:
    enum { MAXLEN = STRING_BUFSIZE - 1 };

    BSLMF_NESTED_TRAIT_DECLARATION_IF(FixedString,
                                      bslma::UsesBslmaAllocator,
                                      ALLOC_TRAIT);
    BSLMF_NESTED_TRAIT_DECLARATION(FixedString, bslmf::IsBitwiseMoveable);

    static int constructorCount();
    static int destructorCount();
    static int instanceCount();
    static int maxLength();
    static int maxSupportedBdexVersion();

    explicit FixedString(bslma_Allocator *alloc = 0);
    FixedString(const char* s, bslma_Allocator *alloc = 0);
    FixedString(const FixedString& original, bslma_Allocator *alloc = 0);
    ~FixedString();

    FixedString& operator=(const FixedString& rhs);

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;

    bslma_Allocator *get_allocator() const;
    const char* c_str() const;
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

template <int FOOTPRINT, bool ALLOC_TRAIT>
bool operator==(const FixedString<FOOTPRINT, ALLOC_TRAIT>& a,
                const FixedString<FOOTPRINT, ALLOC_TRAIT>& b)
{
    return 0 == bsl::strcmp(a.c_str(), b.c_str());
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
inline
bool operator!=(const FixedString<FOOTPRINT, ALLOC_TRAIT>& a,
                const FixedString<FOOTPRINT, ALLOC_TRAIT>& b)
{
    return ! (a == b);
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
bsl::ostream& operator<<(bsl::ostream&                              os,
                         const FixedString<FOOTPRINT, ALLOC_TRAIT>& s)
{
    return s.print(os, 0, -1);
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
int FixedString<FOOTPRINT, ALLOC_TRAIT>::s_constructorCount = 0;

template <int FOOTPRINT, bool ALLOC_TRAIT>
int FixedString<FOOTPRINT, ALLOC_TRAIT>::s_destructorCount = 0;

template <int FOOTPRINT, bool ALLOC_TRAIT>
inline
int FixedString<FOOTPRINT, ALLOC_TRAIT>::constructorCount()
{
    return s_constructorCount;
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
inline
int FixedString<FOOTPRINT, ALLOC_TRAIT>::destructorCount()
{
    return s_destructorCount;
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
inline
int FixedString<FOOTPRINT, ALLOC_TRAIT>::instanceCount()
{
    return s_constructorCount - s_destructorCount;
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
inline
int FixedString<FOOTPRINT, ALLOC_TRAIT>::maxLength()
{
    return MAXLEN;
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
inline
int FixedString<FOOTPRINT, ALLOC_TRAIT>::maxSupportedBdexVersion()
{
    return 1;
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
FixedString<FOOTPRINT, ALLOC_TRAIT>::FixedString(bslma_Allocator *alloc)
: d_alloc(alloc)
{
    bsl::memset(d_buffer, 0, STRING_BUFSIZE);
    ++s_constructorCount;
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
FixedString<FOOTPRINT, ALLOC_TRAIT>::FixedString(const char      *s,
                                                 bslma_Allocator *alloc)
: d_alloc(alloc)
{
    bsl::strncpy(d_buffer, s, MAXLEN);
    d_buffer[MAXLEN] = '\0';
    ++s_constructorCount;
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
FixedString<FOOTPRINT, ALLOC_TRAIT>::FixedString(const FixedString&  original,
                                                 bslma_Allocator    *alloc)
: d_alloc(alloc)
{
    bsl::memcpy(d_buffer, original.d_buffer, STRING_BUFSIZE);
    ++s_constructorCount;
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
FixedString<FOOTPRINT, ALLOC_TRAIT>::~FixedString()
{
    ++s_destructorCount;
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
FixedString<FOOTPRINT, ALLOC_TRAIT>&
FixedString<FOOTPRINT, ALLOC_TRAIT>::operator=(const FixedString& rhs)
{
    bsl::memcpy(d_buffer, rhs.d_buffer, STRING_BUFSIZE);
    return *this;
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
template <class STREAM>
STREAM&
FixedString<FOOTPRINT, ALLOC_TRAIT>::bdexStreamIn(STREAM& stream, int version)
{
    stream.getArrayInt8(d_buffer, STRING_BUFSIZE);
    return stream;
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
template <class STREAM>
STREAM&
FixedString<FOOTPRINT, ALLOC_TRAIT>::bdexStreamOut(STREAM& stream,
                                                   int     version) const
{
    stream.putArrayInt8(d_buffer, STRING_BUFSIZE);
    return stream;
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
bslma_Allocator *FixedString<FOOTPRINT, ALLOC_TRAIT>::get_allocator() const
{
    return d_alloc;
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
const char *FixedString<FOOTPRINT, ALLOC_TRAIT>::c_str() const
{
    return d_buffer;
}

template <int FOOTPRINT, bool ALLOC_TRAIT>
bsl::ostream&
FixedString<FOOTPRINT, ALLOC_TRAIT>::print(
                                       bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    stream << d_buffer;
    if (0 <= spacesPerLevel) {
        stream << '\n';
    }
    return stream;
}

const int SFP = 32;                        // Small Footprint
const int LFP = sizeof(bdem_ChoiceHeader); // Large Footprint
typedef FixedString<SFP>       SmallString;
typedef FixedString<SFP, true> SmallStringAlloc;
typedef FixedString<LFP>       LargeString;
typedef FixedString<LFP, true> LargeStringAlloc;

namespace BloombergLP {
// Specialized null values for FixedString types:
template <>
inline SmallString bdetu_Unset<SmallString>::unsetValue()
    // Return the null value for the type 'char'.
{
    return SmallString();
}

template <>
inline SmallStringAlloc bdetu_Unset<SmallStringAlloc>::unsetValue()
    // Return the null value for the type 'char'.
{
    return SmallStringAlloc();
}

template <>
inline LargeString bdetu_Unset<LargeString>::unsetValue()
    // Return the null value for the type 'char'.
{
    return LargeString();
}

template <>
inline LargeStringAlloc bdetu_Unset<LargeStringAlloc>::unsetValue()
    // Return the null value for the type 'char'.
{
    return LargeStringAlloc();
}

}  // close namespace BloombergLP

namespace my_ElemTypes {
    enum ElemType {
        MY_SMALL_STRING = 0,
        MY_SMALL_STRING_ALLOC,
        MY_LARGE_STRING,
        MY_LARGE_STRING_ALLOC,
        MY_NUM_TYPES
    };
}

// Initialization of element attributes for basic (non-array) types:
#define BDEM_BASIC_DESCRIPTOR_INIT(T,ENUM)              \
    ENUM,                                               \
    sizeof(T),                                          \
    bsls_AlignmentFromType<T >::VALUE,                  \
    &bdem_FunctionTemplates::unsetConstruct<T >,        \
    &bdem_FunctionTemplates::copyConstruct<T >,         \
    &bdem_FunctionTemplates::destroy<T >,               \
    &bdem_FunctionTemplates::assign<T >,                \
    &bdem_FunctionTemplates::bitwiseMove<T >,           \
    &bdem_FunctionTemplates::makeUnset<T >,             \
    &bdem_FunctionTemplates::isUnset<T >,               \
    &bdem_FunctionTemplates::areEqual<T >,              \
    &bdem_FunctionTemplates::print<T >

const bdem_Descriptor my_SmallStringDescriptor = {
    BDEM_BASIC_DESCRIPTOR_INIT(SmallString, my_ElemTypes::MY_SMALL_STRING)
};

const bdem_Descriptor my_SmallStringAllocDescriptor = {
    BDEM_BASIC_DESCRIPTOR_INIT(SmallStringAlloc,
                               my_ElemTypes::MY_SMALL_STRING_ALLOC)
};

const bdem_Descriptor my_LargeStringDescriptor = {
    BDEM_BASIC_DESCRIPTOR_INIT(LargeString, my_ElemTypes::MY_LARGE_STRING)
};

const bdem_Descriptor my_LargeStringAllocDescriptor = {
    BDEM_BASIC_DESCRIPTOR_INIT(LargeStringAlloc,
                               my_ElemTypes::MY_LARGE_STRING_ALLOC)
};

// Sample data values

bslma_TestAllocator sampleAlloc;
const SmallString SS_UNSET(&sampleAlloc);
const SmallString SS_1("Bye world!", &sampleAlloc);
const SmallString SS_2("Nice job.", &sampleAlloc);

const SmallStringAlloc SSA_UNSET(&sampleAlloc);
const SmallStringAlloc SSA_1("A penny", &sampleAlloc);
const SmallStringAlloc SSA_2("earned.", &sampleAlloc);

const LargeString LS_UNSET(&sampleAlloc);
const LargeString LS_1("Early to bed and early to rise", &sampleAlloc);
const LargeString LS_2("...makes a man healthy wealthy and wise.",
                       &sampleAlloc);

const LargeStringAlloc LSA_UNSET(&sampleAlloc);
const LargeStringAlloc LSA_1("A man a plan a canal Panama.",
                             &sampleAlloc);
const LargeStringAlloc LSA_2("Live fast, die young, and leave a "
                             "good looking corpse.", &sampleAlloc);

static char SPECIFICATIONS[] = "ABCD";
    // This string stores the valid specification values used for constructing
    // a TypesCatalog.  Each character specifies an element type 'A' -
    // 'SmallString', 'B' - 'SmallStringAlloc', 'C' - 'LargeString' and 'D' -
    // 'LargeStringAlloc'.
const int SPEC_LEN = sizeof(SPECIFICATIONS) - 1;

static const Desc *DESCRIPTORS[] =
    // This array stores the descriptors corresponding to each element
    // type specified in the spec string.
{
    &my_SmallStringDescriptor,
    &my_SmallStringAllocDescriptor,
    &my_LargeStringDescriptor,
    &my_LargeStringAllocDescriptor,
};
const int NUM_DESCS = sizeof DESCRIPTORS / sizeof *DESCRIPTORS;

static const bdem_DescriptorStreamOut<bdex_TestOutStream> STREAMOUT_DESCS[] =
    // This array stores the stream out descriptors corresponding to each
    // element type specified in the spec string.
{
    { &bdem_FunctionTemplates::streamOut<SmallString, bdex_TestOutStream> },
    { &bdem_FunctionTemplates::streamOut<SmallStringAlloc,
                                         bdex_TestOutStream> },
    { &bdem_FunctionTemplates::streamOut<LargeString, bdex_TestOutStream> },
    { &bdem_FunctionTemplates::streamOut<LargeStringAlloc,
                                         bdex_TestOutStream> },
};
const int NUM_STREAMOUT_DESCS =
                              sizeof STREAMOUT_DESCS / sizeof *STREAMOUT_DESCS;

static const bdem_DescriptorStreamIn<bdex_TestInStream> STREAMIN_DESCS[] =
    // This array stores the stream in descriptors corresponding to each
    // element type specified in the spec string.
{
    { &bdem_FunctionTemplates::streamIn<SmallString, bdex_TestInStream> },
    { &bdem_FunctionTemplates::streamIn<SmallStringAlloc, bdex_TestInStream> },
    { &bdem_FunctionTemplates::streamIn<LargeString, bdex_TestInStream> },
    { &bdem_FunctionTemplates::streamIn<LargeStringAlloc, bdex_TestInStream> },
};
const int NUM_STREAMIN_DESCS = sizeof STREAMIN_DESCS / sizeof *STREAMIN_DESCS;

static const void *VALUES_A[] =
    // This array stores a set of values corresponding to each element
    // type specified in the spec string.
{
    &SS_1,
    &SSA_1,
    &LS_1,
    &LSA_1,
};
const int NUM_VALUESA = sizeof VALUES_A / sizeof *VALUES_A;

static const void *VALUES_B[] =
    // This array stores a second set of values corresponding to each element
    // type specified in the spec string.
{
    &SS_2,
    &SSA_2,
    &LS_2,
    &LSA_2,
};
const int NUM_VALUESB = sizeof VALUES_B / sizeof *VALUES_B;

static const void *VALUES_N[] =
    // This array stores unset values corresponding to each element
    // type specified in the spec string.
{
    &SS_UNSET,
    &SSA_UNSET,
    &LS_UNSET,
    &LSA_UNSET,
};
const int NUM_VALUESN = sizeof VALUES_N / sizeof *VALUES_N;

struct tempStruct {
    static int assertion1[SPEC_LEN == NUM_DESCS];
    static int assertion2[SPEC_LEN == NUM_VALUESA];
    static int assertion3[SPEC_LEN == NUM_VALUESB];
    static int assertion4[SPEC_LEN == NUM_VALUESN];
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
      case 'A': return *(SmallString *) p == *(SmallString *) q;
      case 'B': return *(SmallStringAlloc *) p == *(SmallStringAlloc *) q;
      case 'C': return *(LargeString *) p == *(LargeString *) q;
      case 'D': return *(LargeStringAlloc *) p == *(LargeStringAlloc *) q;
      default: ASSERT(0); return 0;
    }
}

static const Desc *getDescriptor(char spec)
    // Return the non-modifiable descriptor corresponding to the specified
    // 'spec' value.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".
{
    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return DESCRIPTORS[index];
}

static void populateCatalog(Catalog *catalog, const char *spec)
    // Populate the specified descriptor 'catalog' according to the specified
    // 'spec' string.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCD".
{
    ASSERT(catalog);
    ASSERT(spec);

    catalog->clear();
    for (const char *s = spec; *s; ++s) {
        if (' ' == *s || '\t' == *s || '\n' == *s) continue; // ignore WS
        catalog->push_back(getDescriptor(*s));
    }
}

static const void *getValueA(char spec)
    // Return the 'A' value corresponding to the specified 'spec'.
{
    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return VALUES_A[index];
}

static const void *getValueB(char spec)
    // Return the 'B' value corresponding to the specified 'spec'.
{
    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return VALUES_B[index];
}

static const void *getValueN(char spec)
    // Return the 'N' value corresponding to the specified 'spec'.
{
    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return VALUES_N[index];
}

//=============================================================================
//                  FUNCTION FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// The 'divide' function reads two 'double' values from an input stream,
// divides the first by the second, and streams the result to an output
// stream.
//..
  void divide(bsl::istream& is, bsl::ostream& os) {

      // Read input parameters
      double dividend, divisor;
      is >> dividend >> divisor;
//..
// The division normally will result in a double value, but will sometimes
// result in an error string.  The latter case can occur either because the
// input stream is corrupt or because the division itself failed because of
// the divisor was zero.  The result of the division is therefore packaged in
// a 'bdem_ChoiceHeader' which can store either a 'double' or a 'string'.  The
// current selection is indexed by a value of 0 through 2 for the double
// result, string for input error, or string for division error.  Note that
// input errors and division errors are represented by two separate 'STRING'
// items in the array of types, so that the selector will indicate
// which of the two types of error strings is currently being held by the
// choice header.
//..
      enum { RESULT_VAL, INPUT_ERROR, DIVISION_ERROR };

      bdem_ChoiceHeader::DescriptorCatalog catalog;
      catalog.push_back(&bdem_Properties::d_doubleAttr);
      catalog.push_back(&bdem_Properties::d_stringAttr);
      catalog.push_back(&bdem_Properties::d_stringAttr);

      bdem_ChoiceHeader outMessage(&catalog);
      ASSERT(-1 == outMessage.selector());
      ASSERT(3  == outMessage.numSelections());

      if (! is) {
//..
// If the input fails, the choice header is set to 'INPUT_ERROR' and the
// corresponding selection is set to an error string.
//..
          bsl::string inputErrString = "Failed to read arguments.";
          outMessage.makeSelection(INPUT_ERROR, &inputErrString);
          ASSERT(INPUT_ERROR == outMessage.selector());
          ASSERT(inputErrString ==
                            *(bsl::string *) outMessage.selectionPointer());
      }
      else if (0 == divisor) {
//..
// If the division fails, the header is set to 'DIVISION_ERROR' and the
// corresponding selection is set to an error string.
//..
          bsl::string errString = "Divided by zero.";
          outMessage.makeSelection(DIVISION_ERROR, &errString);
          ASSERT(DIVISION_ERROR == outMessage.selector());
          ASSERT(errString == *(bsl::string *) outMessage.selectionPointer());
      }
      else {
//..
// If there are no errors, compute the quotient and store it as a 'double'
// with selector, 'RESULT_VAL'.
//..
          const double quotient = dividend / divisor;
          outMessage.makeSelection(RESULT_VAL, &quotient);
          ASSERT(RESULT_VAL == outMessage.selector());
          ASSERT(quotient == *(double *) outMessage.selectionPointer());
      }
//..
// Once the result is calculated, it is streamed out to be sent back to the
//..
      os << outMessage;
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

    switch (test) { case 0:  // Zero is always the leading case.
      case 13: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating how one might use a choice header
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

        if (verbose) bsl::cout << "\nTESTING USAGE EXAMPLE"
                               << "\n====================="
                               << bsl::endl;

        // Try all three scenarios
        // Scenario 1
        {
            bsl::istringstream in;
            bsl::ostringstream out;
            const bsl::string EXP = "{ STRING Failed to read arguments. }";

            in.clear(bsl::ios_base::badbit);
            divide(in, out);

            ASSERT(EXP == out.str());
        }

        // Scenario 2
        {
            double VALUE1 = 10.25, VALUE2 = 0;
            const bsl::string EXP = "{ STRING Divided by zero. }";
            bsl::ostringstream out;
            out << VALUE1 << " " << VALUE2;

            bsl::istringstream in(out.str());
            out.str("");
            divide(in, out);

            LOOP_ASSERT(out.str(), EXP == out.str());
        }

        // Scenario 3
        {
            double VALUE1 = 10.5, VALUE2 = 5.25;
            const bsl::string EXP = "{ DOUBLE 2 }";

            bsl::ostringstream out;
            out << VALUE1 << " " << VALUE2;

            bsl::istringstream in(out.str());
            out.str("");
            divide(in, out);

            LOOP_ASSERT(out.str(), EXP == out.str());
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'd_choiceItemAttr' STRUCT
        //
        // Concerns:
        //   The 'd_choiceItemAttr' struct is initialized such that:
        //   1. 'd_elemEnum == bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM'
        //   2. 'd_size == sizeof(bdem_ChoiceHeader)'
        //   3. 'd_alignment ==
        //                    bsls_AlignmentFromType<bdem_ChoiceHeader>::VALUE'
        //   4. Function 'makeUnset' calls 'reset'.
        //   5. Function 'isUnset' returns true if object has a void
        //       selection.
        //   6. Function 'areEqual' returns the result of operator==.
        //   7. Function 'print' calls the 'bdem_ChoiceImp::print'.
        //
        // Plan:
        //   Test the data variables for the stated condition.  Use ad-hoc
        //   data to test each of the function pointers once or twice to
        //   convince ourselves that the correct pass-through operation is
        //   occurring.
        //
        // Testing:
        //   static const bdem_Descriptor d_choiceItemAttr;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting 'd_choiceItemAttr' descriptor"
                               << "\n====================================="
                               << bsl::endl;
        const bdem_Descriptor& Desc = Obj::d_choiceItemAttr;

        ASSERT(bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM == Desc.d_elemEnum);
        ASSERT(sizeof(Obj)                           == Desc.d_size);
        ASSERT(bsls_AlignmentFromType<Obj>::VALUE    == Desc.d_alignment);

        static const struct TestRow {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "" },

            { L_,       "A" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },

            { L_,       "AA" },
            { L_,       "AB" },
            { L_,       "AC" },
            { L_,       "AD" },

            { L_,       "DD" },
            { L_,       "DC" },
            { L_,       "DB" },
            { L_,       "DA" },

            { L_,       "BB" },
            { L_,       "CC" },
            { L_,       "BC" },
            { L_,       "CB" },

            { L_,       "ABC" },
            { L_,       "DCB" },

            { L_,       "ABCD" },
            { L_,       "DCBA" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_line;
            const char *SPEC  = DATA[i].d_catalogSpec;
            const int   LEN   = bsl::strlen(SPEC);

            Catalog catalog;
            populateCatalog(&catalog, SPEC);

            Obj mX(&catalog);  const Obj& X  = mX;
            Obj mXN(&catalog); const Obj& XN = mXN;
            Obj mY(&catalog);  const Obj& Y  = mY;
            Obj mYN(&catalog); const Obj& YN = mYN;

            for (int j = 0; j < LEN; ++j) {
                char        S     = SPEC[j];
                const void *VA    = getValueA(S);

                bsl::ostringstream os1;
                bsl::ostringstream os2;

                mX.makeSelection(j, VA);
                mY.makeSelection(j, VA);
                LOOP_ASSERT(LINE, Desc.areEqual(&X, &Y));
                LOOP_ASSERT(LINE, !Desc.isUnset(&X));
                LOOP_ASSERT(LINE, !Desc.isUnset(&Y));

                X.print(os1, 1, 4);
                Desc.print(&Y, os2, 1, 4);

                LOOP_ASSERT(LINE, os1.str() == os2.str());

                Desc.makeUnset(&mX);
                LOOP_ASSERT(LINE, 0 == Desc.areEqual(&X, &Y));
                LOOP_ASSERT(LINE, Desc.isUnset(&X));
                LOOP_ASSERT(LINE, !Desc.isUnset(&Y));

                Desc.makeUnset(&mY);
                LOOP_ASSERT(LINE, Desc.areEqual(&X, &Y));
                LOOP_ASSERT(LINE, Desc.isUnset(&X));
                LOOP_ASSERT(LINE, Desc.isUnset(&Y));

                bsl::ostringstream os1n;
                bsl::ostringstream os2n;

                mXN.makeSelection(j, VA);
                mXN.setNullnessBit();
                mYN.makeSelection(j, VA);
                mYN.setNullnessBit();

                LOOP_ASSERT(LINE, Desc.areEqual(&XN, &YN));
                LOOP_ASSERT(LINE, Desc.isUnset(&XN));
                LOOP_ASSERT(LINE, Desc.isUnset(&YN));
                LOOP_ASSERT(LINE, XN.isSelectionNull());
                LOOP_ASSERT(LINE, YN.isSelectionNull());

                X.print(os1n, 1, 4);
                Desc.print(&Y, os2n, 1, 4);

                LOOP_ASSERT(LINE, os1n.str() == os2n.str());

                Desc.makeUnset(&mXN);
                LOOP_ASSERT(LINE, 0 == Desc.areEqual(&XN, &YN));
                LOOP_ASSERT(LINE, Desc.isUnset(&XN));
                LOOP_ASSERT(LINE, Desc.isUnset(&YN));
                LOOP_ASSERT(LINE, XN.isSelectionNull());
                LOOP_ASSERT(LINE, YN.isSelectionNull());

                Desc.makeUnset(&mYN);
                LOOP_ASSERT(LINE, Desc.areEqual(&XN, &YN));
                LOOP_ASSERT(LINE, Desc.isUnset(&XN));
                LOOP_ASSERT(LINE, Desc.isUnset(&YN));
                LOOP_ASSERT(LINE, XN.isSelectionNull());
                LOOP_ASSERT(LINE, YN.isSelectionNull());
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'reset' FUNCTION:
        //
        // Concerns:
        //   A object that has been reset has state identical to its state
        //   immediately after default construction.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all objects, u,
        //   in the set S.  Default-construct an object, v.  Call u.reset().
        //   Assert that u == v.  Assert that no heap memory is used after
        //   the reset.
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting reset Function"
                               << "\n======================" << bsl::endl;

        static const struct TestRow {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "A" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },

            { L_,       "AA" },
            { L_,       "AB" },
            { L_,       "AC" },
            { L_,       "AD" },

            { L_,       "DD" },
            { L_,       "DC" },
            { L_,       "DB" },
            { L_,       "DA" },

            { L_,       "BB" },
            { L_,       "CC" },
            { L_,       "BC" },
            { L_,       "CB" },

            { L_,       "ABC" },
            { L_,       "DCB" },

            { L_,       "ABCD" },
            { L_,       "DCBA" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int k = 0; k < 4; ++k) {
            AggOption::AllocationStrategy MODE =
                                     (AggOption::AllocationStrategy) k;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                bslma_TestAllocator alloc(veryVeryVerbose);

                Catalog catalog(&alloc);
                populateCatalog(&catalog, SPEC);

                Obj mY(&catalog, MODE);  const Obj& Y = mY;

                for (int j = 0; j < LEN; ++j) {
                    const char  S  = SPEC[j];
                    const void *VA = getValueA(S);

                    // test reset
                    Obj mX(&catalog, MODE); const Obj& X = mX;
                    mX.makeSelection(j, VA);
                    LOOP_ASSERT(LINE, X != Y);
                    LOOP_ASSERT(LINE, !X.isSelectionNull());

                    mX.reset();
                    LOOP_ASSERT(LINE, X == Y);
                    LOOP_ASSERT(LINE, X.isSelectionNull());

                    // test reset (null)
                    Obj mXN(&catalog, MODE); const Obj& XN = mXN;
                    mXN.makeSelection(j, VA);
                    mXN.setNullnessBit();
                    LOOP_ASSERT(LINE, XN != Y);
                    LOOP_ASSERT(LINE, XN.isSelectionNull());

                    mXN.reset();
                    LOOP_ASSERT(LINE, XN == Y);
                    LOOP_ASSERT(LINE, XN.isSelectionNull());

                    // test clearNullnessBit
                    mXN.makeSelection(j, VA);
                    mXN.setNullnessBit();
                    LOOP_ASSERT(LINE, XN != Y);
                    LOOP_ASSERT(LINE, XN.isSelectionNull());

                    mXN.clearNullnessBit();
                    LOOP_ASSERT(LINE, XN != Y);
                    LOOP_ASSERT(LINE, !XN.isSelectionNull());

                    mXN.reset();
                    LOOP_ASSERT(LINE, XN == Y);
                    LOOP_ASSERT(LINE, XN.isSelectionNull());
                }
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'makeSelection' FUNCTIONS:
        //
        // Concerns:
        //   The 'makeSelection' functions have the same concerns as
        //   assignment.  Any value must be assignable to an object having any
        //   initial value without affecting the rhs operand value.  Also, any
        //   object must be settable to itself.  Assigning a value to an
        //   object with the same value type should use assignment, whereas
        //   assigning a value to an object with a different value type should
        //   use destruction and copy construction.  The two-argument version
        //   of 'makeSelection' assigns the unset value to the object.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all objects
        //   mX and mY with values in S.  For each element in the catalog of
        //   the constructed object call the two-arg function with three
        //   distinct values of that type.  Finally, call the two-arg function
        //   with a selector value of -1.  After each function call verify
        //   that the value and the selector value is as expected.  For the
        //   one-arg makeSelection call it twice with each selector value for
        //   that catalog and finally with a selector value of -1.  Again,
        //   confirm that the value is the unset value for that type and the
        //   selector value is as expected.  Finally, ensure that no memory is
        //   leaked, even in the presence of exceptions.
        //
        // Testing:
        //   void *makeSelection(int index);
        //   void *makeSelection(int index, const void *value);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting makeSelection Functions"
                               << "\n==============================="
                               << bsl::endl;

        static const struct TestRow {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "" },

            { L_,       "A" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },

            { L_,       "AA" },
            { L_,       "AB" },
            { L_,       "AC" },
            { L_,       "AD" },

            { L_,       "DD" },
            { L_,       "DC" },
            { L_,       "DB" },
            { L_,       "DA" },

            { L_,       "BB" },
            { L_,       "CC" },
            { L_,       "BC" },
            { L_,       "CB" },

            { L_,       "ABC" },
            { L_,       "DCB" },

            { L_,       "ABCD" },
            { L_,       "DCBA" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int k = 0; k < 4; ++k) {
            AggOption::AllocationStrategy MODE =
                                     (AggOption::AllocationStrategy) k;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                for (int j = 0; j < LEN; ++j) {
                    char        S     = SPEC[j];
                    const void *VA    = getValueA(S);
                    const void *VB    = getValueB(S);
                    const void *VN    = getValueN(S);
                    void *rv1, *rv2;

                    bslma_TestAllocator testAllocator(veryVeryVerbose);

                  BEGIN_BSLMA_EXCEPTION_TEST {

                    bdema_SequentialAllocator  seqAlloc(&testAllocator);
                    bslma_Allocator           *alloc = &testAllocator;

                    if (veryVerbose) { P(MODE); }

                    if (AggOption::BDEM_SUBORDINATE == MODE) {
                        alloc = &seqAlloc;
                    }

                    Catalog catalog(alloc);
                    populateCatalog(&catalog, SPEC);

                    if (veryVerbose) { P(SPEC) P(MODE) }

                    LOOP_ASSERT(LINE, LEN == (int) catalog.size());

                    Obj mX(&catalog,  MODE);  const Obj& X  = mX;
                    Obj mXN(&catalog, MODE);  const Obj& XN = mXN;
                    Obj mY(&catalog,  MODE);  const Obj& Y  = mY;
                    Obj mYN(&catalog, MODE);  const Obj& YN = mYN;

                    mXN.setNullnessBit();
                    mYN.setNullnessBit();

                    // Test two-arg makeSelection
                    rv1 = mX.makeSelection(j, VA);

                    LOOP3_ASSERT(LINE, j, S, X.selectionPointer() == rv1);
                    LOOP3_ASSERT(LINE, j, S, compare(VA, rv1, S));
                    LOOP3_ASSERT(LINE, j, S, j == X.selector());
                    LOOP3_ASSERT(LINE, j, S, !X.isSelectionNull());

                    rv1 = mX.makeSelection(j, VN);

                    LOOP3_ASSERT(LINE, j, S, X.selectionPointer() == rv1);
                    LOOP3_ASSERT(LINE, j, S, compare(VN, rv1, S));
                    LOOP3_ASSERT(LINE, j, S, j == X.selector());
                    LOOP3_ASSERT(LINE, j, S, !X.isSelectionNull());

                    rv1 = mX.makeSelection(j, VB);

                    LOOP3_ASSERT(LINE, j, S, X.selectionPointer() == rv1);
                    LOOP3_ASSERT(LINE, j, S, compare(VB, rv1, S));
                    LOOP3_ASSERT(LINE, j, S, j == X.selector());
                    LOOP3_ASSERT(LINE, j, S, !X.isSelectionNull());

                    rv1 = mX.makeSelection(-1, 0);
                    LOOP3_ASSERT(LINE, j, S, -1 == X.selector());
                    LOOP3_ASSERT(LINE, j, S, X.isSelectionNull());
                    LOOP3_ASSERT(LINE, j, S, 0 == rv1);

                    // Test two-arg makeSelection (null)
                    LOOP3_ASSERT(LINE, j, S, XN.isSelectionNull());
                    rv1 = mXN.makeSelection(j, VA);

                    LOOP3_ASSERT(LINE, j, S, XN.selectionPointer() == rv1);
                    LOOP3_ASSERT(LINE, j, S, compare(VA, rv1, S));
                    LOOP3_ASSERT(LINE, j, S, j == XN.selector());
                    LOOP3_ASSERT(LINE, j, S, !XN.isSelectionNull());

                    mXN.setNullnessBit();
                    LOOP3_ASSERT(LINE, j, S, XN.isSelectionNull());
                    rv1 = mXN.makeSelection(j, VN);

                    LOOP3_ASSERT(LINE, j, S, XN.selectionPointer() == rv1);
                    LOOP3_ASSERT(LINE, j, S, compare(VN, rv1, S));
                    LOOP3_ASSERT(LINE, j, S, j == XN.selector());
                    LOOP3_ASSERT(LINE, j, S, !XN.isSelectionNull());

                    mXN.setNullnessBit();
                    LOOP3_ASSERT(LINE, j, S, XN.isSelectionNull());
                    rv1 = mXN.makeSelection(j, VB);

                    LOOP3_ASSERT(LINE, j, S, XN.selectionPointer() == rv1);
                    LOOP3_ASSERT(LINE, j, S, compare(VB, rv1, S));
                    LOOP3_ASSERT(LINE, j, S, j == XN.selector());
                    LOOP3_ASSERT(LINE, j, S, !XN.isSelectionNull());

                    mXN.setNullnessBit();
                    LOOP3_ASSERT(LINE, j, S, XN.isSelectionNull());

                    rv1 = mXN.makeSelection(-1, 0);

                    LOOP3_ASSERT(LINE, j, S, -1 == XN.selector());
                    LOOP3_ASSERT(LINE, j, S, XN.isSelectionNull());
                    LOOP3_ASSERT(LINE, j, S, 0 == rv1);

                    // Test one-arg makeSelection
                    rv2 = mY.makeSelection(j);

                    LOOP3_ASSERT(LINE, j, S, Y.selectionPointer() == rv2);
                    LOOP3_ASSERT(LINE, j, S, compare(VN, rv2, S));
                    LOOP3_ASSERT(LINE, j, S, j == Y.selector());
                    LOOP3_ASSERT(LINE, j, S, Y.isSelectionNull());

                    rv2 = mY.makeSelection(j);

                    LOOP3_ASSERT(LINE, j, S, Y.selectionPointer() == rv2);
                    LOOP3_ASSERT(LINE, j, S, compare(VN, rv2, S));
                    LOOP3_ASSERT(LINE, j, S, j == Y.selector());
                    LOOP3_ASSERT(LINE, j, S, Y.isSelectionNull());

                    rv1 = mX.makeSelection(-1);
                    LOOP3_ASSERT(LINE, j, S, -1 == X.selector());
                    LOOP3_ASSERT(LINE, j, S, Y.isSelectionNull());

                    // Test one-arg makeSelection (null)
                    LOOP3_ASSERT(LINE, j, S, YN.isSelectionNull());
                    rv2 = mYN.makeSelection(j);

                    LOOP3_ASSERT(LINE, j, S, YN.selectionPointer() == rv2);
                    LOOP3_ASSERT(LINE, j, S, compare(VN, rv2, S));
                    LOOP3_ASSERT(LINE, j, S, j == YN.selector());
                    LOOP3_ASSERT(LINE, j, S, YN.isSelectionNull());

                    mYN.setNullnessBit();
                    LOOP3_ASSERT(LINE, j, S, YN.isSelectionNull());
                    rv2 = mYN.makeSelection(j);

                    LOOP3_ASSERT(LINE, j, S, YN.selectionPointer() == rv2);
                    LOOP3_ASSERT(LINE, j, S, compare(VN, rv2, S));
                    LOOP3_ASSERT(LINE, j, S, j == YN.selector());
                    LOOP3_ASSERT(LINE, j, S, YN.isSelectionNull());

                    mYN.setNullnessBit();
                    LOOP3_ASSERT(LINE, j, S, YN.isSelectionNull());
                    rv1 = mYN.makeSelection(-1);

                    LOOP3_ASSERT(LINE, j, S, -1 == XN.selector());
                    LOOP3_ASSERT(LINE, j, S, YN.isSelectionNull());
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //   The 'bdex' streaming concerns for this component are standard.
        //   We next step through the sequence of possible stream states
        //   (valid, empty, invalid, and incomplete), appropriately selecting
        //   data sets as described below.  In all cases, exception neutrality
        //   is confirmed using the specially instrumented 'bdex_TestInStream'
        //   and a pair of standard macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
        //   'END_BDEX_EXCEPTION_TEST', which configure the 'bdex_TestInStream'
        //   object appropriately in a loop.
        //
        // Plan:
        //   VALID STREAMS
        //     For the set S of globally-defined test values, use all
        //     combinations (u, v) in the cross product S X S, stream the
        //     value of v into (a temporary copy of) u and assert u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each u in S, create a copy and attempt to stream into it
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
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
        //   bdexStreamOut(STREAM&                    stream,
        //                 int                        version,
        //                 const DescStrmOut<STREAM> *strmAttrLookup) const;
        //   bdexStreamIn(STREAM&           stream,
        //                int               version,
        //                const DescStrmIn *strmAttrLookup,
        //                const Descriptor *const attrLookup[]);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Streaming Functionality"
                               << "\n==============================="
                               << bsl::endl;

// TBD the following is *not* a test for backward-compatibility

        if (verbose) bsl::cout << "\n\nBackward-Compatibility Test\n"
                               <<     "= = = = = = = = = = = = = ="
                               << bsl::endl;
        {
            static const struct {
                int         d_line;
                const char *d_catalogSpec;  // Specification to create the
                                            // catalog
                int         d_catalogSpecIndex;
                int         d_length;
                const char *d_input;
            } DATA[] = {
              // Line Spec    Idx Len Input
              // ==== ====    === === =====
                { L_, "A",    0,  26, "\xe6\x0\x0\x0\x0\xe0\x0\x0\x0\x10"
                                      "Bye world!"
                                      "\x0\x0\x0\x0\x0\x0"
                },
                { L_, "B",    0,  26, "\xe6\x0\x0\x0\x0\xe0\x0\x0\x0\x10"
                                      "A penny"
                                      "\x0\x0\x0\x0\x0\x0\x0\x0"
                                      "\x0"
                },
                { L_, "C",    0, 42, "\xe6\x0\x0\x0\x0\xe0\x0\x0\x0\x20"
                                     "Early to bed and early to rise"
                                     "\x0\x0\x0\x0\x0\x0\x0\x0"
                                     "\x0\x0\x0\x0\x0\x0\x0"
                },
                { L_, "D",    0, 42, "\xe6\x0\x0\x0\x0\xe0\x0\x0\x0\x20"
                                     "A man a plan a canal Panama."
                                     "\x0\x0\x0\x0\x0\x0\x0\x0"
                                     "\x0\x0\x0\x0\x0\x0\x0"
                },
                { L_, "AA",   1, 26, "\xe6\x0\x0\x0\x1\xe0\x0\x0\x0\x10"
                                     "Bye world!"
                                     "\x0\x0\x0\x0\x0\x0"
                },
                { L_, "AB",   1, 26, "\xe6\x0\x0\x0\x1\xe0\x0\x0\x0\x10"
                                     "A penny"
                                     "\x0\x0\x0\x0\x0\x0\x0\x0"
                                     "\x0"
                },
                { L_, "CC",   1, 42, "\xe6\x0\x0\x0\x1\xe0\x0\x0\x0\x20"
                                     "Early to bed and early to rise"
                                     "\x0\x0\x0\x0\x0\x0\x0\x0"
                                     "\x0\x0\x0\x0\x0\x0\x0"
                },
                { L_, "CD",   1, 42, "\xe6\x0\x0\x0\x1\xe0\x0\x0\x0\x20"
                                     "A man a plan a canal Panama."
                                     "\x0\x0\x0\x0\x0\x0\x0\x0"
                                     "\x0\x0\x0\x0\x0\x0\x0"
                },
                { L_, "ABCD", 0, 26, "\xe6\x0\x0\x0\x0\xe0\x0\x0\x0\x10"
                                     "Bye world!"
                                     "\x0\x0\x0\x0\x0\x0"
                },
                { L_, "ABCD", 1, 26, "\xe6\x0\x0\x0\x1\xe0\x0\x0\x0\x10"
                                     "A penny"
                                     "\x0\x0\x0\x0\x0\x0\x0\x0"
                                     "\x0"
                },
                { L_, "ABCD", 2, 42, "\xe6\x0\x0\x0\x2\xe0\x0\x0\x0\x20"
                                     "Early to bed and early to rise"
                                     "\x0\x0\x0\x0\x0\x0\x0\x0"
                                     "\x0\x0\x0\x0\x0\x0\x0"
                },
                { L_, "ABCD", 3, 42, "\xe6\x0\x0\x0\x3\xe0\x0\x0\x0\x20"
                                     "A man a plan a canal Panama."
                                     "\x0\x0\x0\x0\x0\x0\x0\x0"
                                     "\x0\x0\x0\x0\x0\x0\x0"
                },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int k = 0; k < 4; ++k) {
                AggOption::AllocationStrategy MODE =
                                            (AggOption::AllocationStrategy) k;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE    = DATA[i].d_line;
                    const char *SPEC    = DATA[i].d_catalogSpec;
                    const int   SPECIDX = DATA[i].d_catalogSpecIndex;
                    const int   LENGTH  = DATA[i].d_length;
                    const char *INPUT   = DATA[i].d_input;

                    bslma_TestAllocator alloc(veryVeryVerbose);

                    if (veryVerbose) { P_(SPEC); P_(SPECIDX); P(LENGTH); }

                    Catalog catalog(&alloc);
                    populateCatalog(&catalog, SPEC);

                    const char S1    = SPEC[SPECIDX];
                    const void *V1_A = getValueA(S1);

                    Obj mX(&catalog, MODE); const Obj& X = mX;
                    Obj mY(&catalog, MODE); const Obj& Y = mY;
                    mX.makeSelection(SPECIDX, V1_A);

                    Obj mA(&catalog, MODE); const Obj& A = mA;
                    bdex_TestInStream is(INPUT, LENGTH);
                    is.setSuppressVersionCheck(1);

                    if (veryVeryVerbose) { T_; P(is); }
                    if (veryVerbose) {
                        bsl::cout << "\tObjects before stream:\n";
                        T_; T_; P(MODE); T_; T_; P(X); T_; T_; P(A);
                    }

                    mA.bdexStreamIn(is, 1, STREAMIN_DESCS, DESCRIPTORS);
                    LOOP_ASSERT(LINE, X == A);
                    if (veryVerbose) {
                        bsl::cout << "\tObjects after stream :\n";
                        T_; T_; P(MODE); T_; T_; P(X); T_; T_; P(A);
                    }

                    bdex_TestOutStream os;
                    mA.bdexStreamOut(os, 1, STREAMOUT_DESCS);
                    LOOP_ASSERT(LINE, os.length() == LENGTH);
                    LOOP_ASSERT(LINE,
                                0 == bsl::memcmp(os.data(), INPUT, LENGTH));
                }
            }
        }

        if (verbose) bsl::cout << "\n\nWith-null Test\n"
                               <<     "= = = = = = = ="
                               << bsl::endl;
        static const struct TestRow {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
            bool        d_originalIsNull;
            bool        d_streamedIsNull;
        } DATA[] = {
            // Line     DescriptorSpec  OrgIsNull StrmIsNull
            // ====     ==============  ========= ==========
            { L_,       "A",            false,    false },
            { L_,       "A",            true ,    false },
            { L_,       "A",            false,    true  },
            { L_,       "A",            true ,    true  },

            { L_,       "B",            false,    false },
            { L_,       "B",            true ,    false },
            { L_,       "B",            false,    true  },
            { L_,       "B",            true ,    true  },

            { L_,       "C",            false,    false },
            { L_,       "C",            true ,    false },
            { L_,       "C",            false,    true  },
            { L_,       "C",            true ,    true  },

            { L_,       "D",            false,    false },
            { L_,       "D",            true ,    false },
            { L_,       "D",            false,    true  },
            { L_,       "D",            true ,    true  },

            { L_,       "AA",           false,    false },
            { L_,       "AA",           true ,    false },
            { L_,       "AA",           false,    true  },
            { L_,       "AA",           true ,    true  },

            { L_,       "AB",           false,    false },
            { L_,       "AB",           true ,    false },
            { L_,       "AB",           false,    true  },
            { L_,       "AB",           true ,    true  },

            { L_,       "AC",           false,    false },
            { L_,       "AC",           true ,    false },
            { L_,       "AC",           false,    true  },
            { L_,       "AC",           true ,    true  },

            { L_,       "AD",           false,    false },
            { L_,       "AD",           true ,    false },
            { L_,       "AD",           false,    true  },
            { L_,       "AD",           true ,    true  },

            { L_,       "DD",           false,    false },
            { L_,       "DD",           true ,    false },
            { L_,       "DD",           false,    true  },
            { L_,       "DD",           true ,    true  },

            { L_,       "DC",           false,    false },
            { L_,       "DC",           true ,    false },
            { L_,       "DC",           false,    true  },
            { L_,       "DC",           true ,    true  },

            { L_,       "DB",           false,    false },
            { L_,       "DB",           true ,    false },
            { L_,       "DB",           false,    true  },
            { L_,       "DB",           true ,    true  },

            { L_,       "DA",           false,    false },
            { L_,       "DA",           true ,    false },
            { L_,       "DA",           false,    true  },
            { L_,       "DA",           true ,    true  },

            { L_,       "BB",           false,    false },
            { L_,       "BB",           true ,    false },
            { L_,       "BB",           false,    true  },
            { L_,       "BB",           true ,    true  },

            { L_,       "CC",           false,    false },
            { L_,       "CC",           true ,    false },
            { L_,       "CC",           false,    true  },
            { L_,       "CC",           true ,    true  },

            { L_,       "BC",           false,    false },
            { L_,       "BC",           true ,    false },
            { L_,       "BC",           false,    true  },
            { L_,       "BC",           true ,    true  },

            { L_,       "CB",           false,    false },
            { L_,       "CB",           true ,    false },
            { L_,       "CB",           false,    true  },
            { L_,       "CB",           true ,    true  },

            { L_,       "ABC",          false,    false },
            { L_,       "ABC",          true ,    false },
            { L_,       "ABC",          false,    true  },
            { L_,       "ABC",          true ,    true  },

            { L_,       "DCB",          false,    false },
            { L_,       "DCB",          true ,    false },
            { L_,       "DCB",          false,    true  },
            { L_,       "DCB",          true ,    true  },

            { L_,       "ABCD",         false,    false },
            { L_,       "ABCD",         true ,    false },
            { L_,       "ABCD",         false,    true  },
            { L_,       "ABCD",         true ,    true  },

            { L_,       "DCBA",         false,    false },
            { L_,       "DCBA",         true ,    false },
            { L_,       "DCBA",         false,    true  },
            { L_,       "DCBA",         true ,    true  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int v = 1; v < 4; ++v) {
            const int VERSION = v;

            for (int k = 0; k < 4; ++k) {
                AggOption::AllocationStrategy MODE =
                                             (AggOption::AllocationStrategy) k;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE    = DATA[i].d_line;
                    const char *SPEC    = DATA[i].d_catalogSpec;
                    const int   LEN     = bsl::strlen(SPEC);
                    const bool  XISNULL = VERSION > 2
                                        ? DATA[i].d_originalIsNull
                                        : false;
                    const bool  AISNULL = VERSION > 2
                                        ? DATA[i].d_streamedIsNull
                                        : false;

                    bslma_TestAllocator alloc(veryVeryVerbose);

                    Catalog catalog(&alloc);
                    populateCatalog(&catalog, SPEC);

                    Obj mX(&catalog, MODE);  const Obj& X  = mX;

                    for (int j1 = 0; j1 < LEN; ++j1) {
                        const char  S1   = SPEC[j1];
                        const void *V1_A = getValueA(S1);
                        const void *V1_N = getValueN(S1);

                        mX.makeSelection(j1, V1_A);

                        bdex_TestOutStream os;
                        if (XISNULL) {
                            // Set to the unset value
                            mX.makeSelection(j1, V1_N);
                            mX.setNullnessBit();
                        }

                        if (veryVerbose) {
                            P(X)
                        }

                        X.bdexStreamOut(os, VERSION, STREAMOUT_DESCS);

                        // Stream a constructed obj to an empty obj.
                        {
                            Obj mA(&catalog, MODE); const Obj& A = mA;

                            LOOP2_ASSERT(LINE, j1, X != A);

                            if (veryVerbose) {
                                P(A)
                            }

                            bdex_TestInStream is(os.data(), os.length());

                            is.setSuppressVersionCheck(1);
                            mA.bdexStreamIn(is,
                                            VERSION,
                                            STREAMIN_DESCS, DESCRIPTORS);

                            LOOP4_ASSERT(LINE, VERSION, X, A, X == A);
                        }

                        // Stream a constructed obj to an non-empty obj.
                        for (int j2 = 0; j2 < LEN; ++j2) {
                            const char  S2   = SPEC[j2];
                            const void *V2_A = getValueA(S2);
                            const void *V2_N = getValueN(S2);

                            Obj mA(&catalog, MODE); const Obj& A = mA;
                            mA.makeSelection(j2, V2_A);
                            if (AISNULL) {
                                mA.makeSelection(j2, V2_N);
                                mA.setNullnessBit();
                            }

                            if (veryVerbose) {
                                P(A)
                            }

                            if (j1 != j2) {
                                LOOP3_ASSERT(LINE, j1, j2, X != A);
                            }

                            LOOP3_ASSERT(LINE, j1, j2,
                                         A.isSelectionNull() == AISNULL);

                            bdex_TestInStream is(os.data(), os.length());

                            is.setSuppressVersionCheck(1);
                            mA.bdexStreamIn(is,
                                            VERSION,
                                            STREAMIN_DESCS, DESCRIPTORS);

                            LOOP3_ASSERT(LINE, j1, j2, X == A);
                            LOOP3_ASSERT(LINE, j1, j2,
                                         A.isSelectionNull() == XISNULL);
                        }

                        // Stream from an empty and invalid stream
                        {
                            Obj mA(&catalog, MODE); const Obj& A = mA;
                            mA.makeSelection(j1, V1_A);
                            Obj mY(&catalog, MODE); const Obj& Y = mY;
                            mY.makeSelection(j1, V1_A);

                            if (AISNULL) {
                                mA.makeSelection(j1, V1_N);
                                mY.makeSelection(j1, V1_N);
                                mA.setNullnessBit();
                                mY.setNullnessBit();
                            }
                            LOOP_ASSERT(LINE, A == Y);
                            LOOP_ASSERT(LINE, A.isSelectionNull() == AISNULL);
                            LOOP_ASSERT(LINE, Y.isSelectionNull() == AISNULL);

                            if (veryVerbose) {
                                P(Y) P(A)
                            }

                            if (XISNULL == AISNULL) {
                                LOOP_ASSERT(LINE, A == X);
                                LOOP_ASSERT(LINE, Y == X);
                            }
                            else {
                                LOOP_ASSERT(LINE, A != X);
                                LOOP_ASSERT(LINE, Y != X);
                            }

                            bdex_TestInStream is;  // Empty stream

                            is.setSuppressVersionCheck(1);
                            mA.bdexStreamIn(is,
                                            VERSION,
                                            STREAMIN_DESCS, DESCRIPTORS);

                            LOOP_ASSERT(LINE, A == Y);
                            LOOP_ASSERT(LINE, !is);

                            bdex_TestInStream is2(os.data(), os.length());
                            is2.invalidate();  // Invalid stream

                            is2.setSuppressVersionCheck(1);
                            mA.bdexStreamIn(is2,
                                            VERSION,
                                            STREAMIN_DESCS,
                                            DESCRIPTORS);

                            LOOP_ASSERT(LINE, A == Y);
                            LOOP_ASSERT(LINE, !is2);
                        }

                        // Stream an empty obj to a constructed obj
                        {
                            Obj mA(&catalog, MODE); const Obj& A = mA;

                            LOOP_ASSERT(LINE, X != A);

                            if (veryVerbose) {
                                P(X) P(A)
                            }

                            bdex_TestOutStream os;

                            A.bdexStreamOut(os,
                                            VERSION,
                                            STREAMOUT_DESCS);

                            bdex_TestInStream is(os.data(), os.length());

                            is.setSuppressVersionCheck(1);
                            mX.bdexStreamIn(is,
                                            VERSION,
                                            STREAMIN_DESCS, DESCRIPTORS);

                            LOOP_ASSERT(LINE, X == A);
                        }
                    }
                }
            }
        }
      } break;
      case 8: {
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
        //   bdem_ChoiceHeader& operator=(const bdem_ChoiceHeader& rhs);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Assignment Operator"
                               << "\n===========================" << bsl::endl;

        static const struct TestRow {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "A" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },

            { L_,       "AA" },
            { L_,       "AB" },
            { L_,       "AC" },
            { L_,       "AD" },

            { L_,       "DD" },
            { L_,       "DC" },
            { L_,       "DB" },
            { L_,       "DA" },

            { L_,       "BB" },
            { L_,       "CC" },
            { L_,       "BC" },
            { L_,       "CB" },

            { L_,       "ABC" },
            { L_,       "DCB" },

            { L_,       "ABCD" },
            { L_,       "DCBA" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int k = 0; k < 4; ++k) {
            AggOption::AllocationStrategy MODE =
                                             (AggOption::AllocationStrategy) k;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                bslma_TestAllocator alloc(veryVeryVerbose);

                Catalog catalog(&alloc);
                populateCatalog(&catalog, SPEC);

                Obj mX(&catalog, MODE); const Obj& X = mX;
                Obj mY(&catalog, MODE); const Obj& Y = mY;

                for (int j1 = 0; j1 < LEN; ++j1) {
                    const char  S    = SPEC[j1];
                    const void *V1_A = getValueA(S);
                    const void *V1_B = getValueB(S);

                    mX.makeSelection(j1, V1_A);
                    mY.makeSelection(j1, V1_A);
                    LOOP_ASSERT(LINE, X == Y);

                    for (int j2 = 0; j2 < LEN; ++j2) {
                        const char  S    = SPEC[j2];
                        const void *V2_A = getValueA(S);

                        Obj mA(&catalog, MODE); const Obj& A = mA;
                        mA.makeSelection(j2, V2_A);

                        mA = X;
                        LOOP3_ASSERT(LINE, X, A, X == A);
                        LOOP_ASSERT(LINE, X == Y);
                        LOOP_ASSERT(LINE, !A.isSelectionNull());
                    }

                    mX.makeSelection(j1, V1_A);
                    mY.makeSelection(j1, V1_A);
                    LOOP_ASSERT(LINE, X == Y);

                    // Test Self-Assignment
                    {
                        Obj mZ(&catalog, MODE); const Obj& Z = mZ;
                        mZ.makeSelection(j1, V1_A);
                        LOOP3_ASSERT(LINE, X, Z, X == Z);

                        mZ = Z;
                        LOOP_ASSERT(LINE, X == Z);
                        LOOP_ASSERT(LINE, !Z.isSelectionNull());
                    }

                    // Test source unset assignment
                    {
                        Obj mA(&catalog, MODE); const Obj& A = mA;
                        mA.makeSelection(-1);
                        Obj mB(&catalog, MODE); const Obj& B = mB;
                        mB.makeSelection(j1, V1_A);
                        LOOP3_ASSERT(LINE, A, B, A != B);

                        mB = A;
                        LOOP_ASSERT(LINE, A == B);
                        LOOP_ASSERT(LINE, A.isSelectionNull());
                        LOOP_ASSERT(LINE, B.isSelectionNull());
                    }

                    // Test dest unset assignment
                    {
                        Obj mA(&catalog, MODE); const Obj& A = mA;
                        mA.makeSelection(-1);
                        Obj mB(&catalog, MODE); const Obj& B = mB;
                        mB.makeSelection(j1, V1_A);
                        LOOP3_ASSERT(LINE, A, B, A != B);

                        mA = B;
                        LOOP_ASSERT(LINE, A == B);
                        LOOP_ASSERT(LINE, !A.isSelectionNull());
                    }

                    for (int j2 = 0; j2 < LEN; ++j2) {
                        const char  S    = SPEC[j2];
                        const void *V2_A = getValueA(S);

                        mX.setNullnessBit();
                        ASSERT(X.isSelectionNull());
                        mY.setNullnessBit();
                        ASSERT(Y.isSelectionNull());

                        Obj mA(&catalog); const Obj& A = mA;
                        mA.makeSelection(j2, V2_A);

                        mA = X;
                        LOOP_ASSERT(LINE, X == A);
                        LOOP_ASSERT(LINE, X == Y);
                        LOOP_ASSERT(LINE, A.isSelectionNull());
                    }

                    // Test Self-Assignment (null)
                    mX.setNullnessBit();
                    {
                        Obj mZ(&catalog, MODE); const Obj& Z = mZ;
                        mZ.makeSelection(j1, V1_A);

                        mZ.setNullnessBit();
                        ASSERT(Z.isSelectionNull());
                        LOOP_ASSERT(LINE, X == Z);

                        mZ = Z;
                        LOOP_ASSERT(LINE, X == Z);
                        LOOP_ASSERT(LINE, Z.isSelectionNull());
                    }
                }
            }
        }
      } break;
      case 7: {
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
        //   initialize identically valued objects w and x, with different
        //   allocators, using tested methods.  Then copy construct an object
        //   y from x with a third allocator, and use the equality operator to
        //   assert that both x and y have the same value as w.  Modify y by
        //   changing one attribute and verify that w and x still have the
        //   same value but that y is different from both.  Repeat the entire
        //   process, but change different attributes of y in the last step.
        //
        // Testing:
        //   bdem_ChoiceHeader(const bdem_ChoiceHeader& original);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Copy Constructor"
                               << "\n========================" << bsl::endl;

        static const struct TestRow {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "A" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },

            { L_,       "AA" },
            { L_,       "AB" },
            { L_,       "AC" },
            { L_,       "AD" },

            { L_,       "DD" },
            { L_,       "DC" },
            { L_,       "DB" },
            { L_,       "DA" },

            { L_,       "BB" },
            { L_,       "CC" },
            { L_,       "BC" },
            { L_,       "CB" },

            { L_,       "ABC" },
            { L_,       "DCB" },

            { L_,       "ABCD" },
            { L_,       "DCBA" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (veryVerbose) { bsl::cout << "Copy construct empty objects"
                                     << bsl::endl; }
        {
            bslma_TestAllocator alloc1(veryVeryVerbose);

            Catalog catalog(&alloc1);
            populateCatalog(&catalog, "");

            Obj mX(&catalog); const Obj& X = mX;

            {
                Obj mA(mX); const Obj& A = mA;

                ASSERT(X == A);
                ASSERT(AggOption::BDEM_PASS_THROUGH == A.allocMode());
                ASSERT(-1 == A.selector());
                ASSERT(A.isSelectionNull());
                ASSERT(X.isSelectionNull());

                mX.clearNullnessBit();
                ASSERT(A.isSelectionNull());
                ASSERT(!X.isSelectionNull());
            }

            mX.setNullnessBit();
            ASSERT(X.isSelectionNull());
            {
                Obj mA(mX); const Obj& A = mA;

                ASSERT(X == A);
                ASSERT(AggOption::BDEM_PASS_THROUGH == A.allocMode());
                ASSERT(-1 == A.selector());
                ASSERT(A.isSelectionNull());
                ASSERT(X.isSelectionNull());

                mA.clearNullnessBit();
                ASSERT(!A.isSelectionNull());
                ASSERT(X.isSelectionNull());
            }

            mX.clearNullnessBit();
            ASSERT(!X.isSelectionNull());
            for (int i = 0; i < 4; ++i) {
                AggOption::AllocationStrategy MODE =
                    (AggOption::AllocationStrategy) i;

                Obj mA(mX, MODE); const Obj& A = mA;

                ASSERT(X    == A);
                ASSERT(A.allocMode() == AggOption::AllocationStrategy(
                                  MODE & ~AggOption::BDEM_OWN_ALLOCATOR_FLAG));
                ASSERT(-1   == A.selector());
                ASSERT(!A.isSelectionNull());
            }

            mX.setNullnessBit();
            ASSERT(X.isSelectionNull());
            for (int i = 0; i < 4; ++i) {
                AggOption::AllocationStrategy MODE =
                    (AggOption::AllocationStrategy) i;

                Obj mA(mX, MODE); const Obj& A = mA;

                ASSERT(X    == A);
                ASSERT(A.allocMode() == AggOption::AllocationStrategy(
                                  MODE & ~AggOption::BDEM_OWN_ALLOCATOR_FLAG));
                ASSERT(-1   == A.selector());
                ASSERT(A.isSelectionNull());
            }
        }

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_line;
            const char *SPEC  = DATA[i].d_catalogSpec;
            const int   LEN   = bsl::strlen(SPEC);

            bslma_TestAllocator alloc(veryVeryVerbose);

            Catalog catalog(&alloc);
            populateCatalog(&catalog, SPEC);

            Obj mX(&catalog); const Obj& X = mX;
            Obj mY(&catalog); const Obj& Y = mY;

            for (int j = 0; j < LEN; ++j) {
                const char S = SPEC[j];
                const void *VA = getValueA(S);
                const void *VB = getValueB(S);

                mX.makeSelection(j, VA);
                mY.makeSelection(j, VA);
                LOOP_ASSERT(LINE, X == Y);

                for (int k = 0; k < 4; ++k) {
                    AggOption::AllocationStrategy MODE =
                          (AggOption::AllocationStrategy) k;

                    AggOption::AllocationStrategy EXP_MODE =
                          AggOption::AllocationStrategy
                                  (MODE & ~AggOption::BDEM_OWN_ALLOCATOR_FLAG);

                    Obj mA(X); const Obj& A = mA;
                    LOOP_ASSERT(LINE, X == A);
                    LOOP_ASSERT(LINE,
                                AggOption::BDEM_PASS_THROUGH == A.allocMode());
                    LOOP_ASSERT(LINE, j == A.selector());
                    ASSERT(!A.isSelectionNull());
                    LOOP3_ASSERT(LINE, X, Y, X == Y);

                    Obj mB(X, MODE); const Obj& B = mB;
                    LOOP_ASSERT(LINE, X == B);
                    LOOP_ASSERT(LINE, EXP_MODE == B.allocMode());
                    LOOP_ASSERT(LINE, j == B.selector());
                    LOOP_ASSERT(LINE, X == Y);
                    ASSERT(!B.isSelectionNull());
                }

                for (int k = 0; k < 4; ++k) {
                    mX.setNullnessBit();
                    ASSERT(X.isSelectionNull());
                    mY.setNullnessBit();
                    ASSERT(Y.isSelectionNull());
                    AggOption::AllocationStrategy MODE =
                          (AggOption::AllocationStrategy) k;

                    AggOption::AllocationStrategy EXP_MODE =
                          AggOption::AllocationStrategy
                                  (MODE & ~AggOption::BDEM_OWN_ALLOCATOR_FLAG);

                    Obj mA(X); const Obj& A = mA;
                    LOOP_ASSERT(LINE, X == A);
                    LOOP_ASSERT(LINE,
                                AggOption::BDEM_PASS_THROUGH == A.allocMode());
                    LOOP_ASSERT(LINE, j == A.selector());
                    ASSERT(A.isSelectionNull());
                    LOOP3_ASSERT(LINE, X, Y, X == Y);

                    Obj mB(X, MODE); const Obj& B = mB;
                    LOOP_ASSERT(LINE, X == B);
                    LOOP_ASSERT(LINE, EXP_MODE == B.allocMode());
                    LOOP_ASSERT(LINE, j == B.selector());
                    LOOP_ASSERT(LINE, X == Y);
                    ASSERT(B.isSelectionNull());
               }
            }
        }
      } break;
      case 6: {
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
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.  Orthogonal perturbation: u and v are constructed with
        //   the same or with different allocators.
        //
        // Testing:
        //   bool operator==(const ChoiceHeader&, const ChoiceHeader&);
        //   bool operator!=(const ChoiceHeader&, const ChoiceHeader&);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Equality Operators"
                               << "\n==========================" << bsl::endl;

        static const struct TestRow {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog

        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "" },

            { L_,       "A" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },

            { L_,       "AB" },
            { L_,       "AC" },
            { L_,       "AD" },

            { L_,       "DC" },
            { L_,       "DB" },
            { L_,       "DA" },

            { L_,       "BC" },
            { L_,       "CB" },

            { L_,       "ABC" },
            { L_,       "DCB" },

            { L_,       "ABCD" },
            { L_,       "DCBA" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i1 = 0; i1 < NUM_DATA; ++i1) {
            const int   LINE1  = DATA[i1].d_line;
            const char *SPEC1  = DATA[i1].d_catalogSpec;
            const int   LEN1   = bsl::strlen(SPEC1);

            for (int j1 = 0; j1 < LEN1; ++j1) {

                for (int i2 = 0; i2 < NUM_DATA; ++i2) {
                    const int   LINE2 = DATA[i2].d_line;
                    const char *SPEC2 = DATA[i2].d_catalogSpec;
                    const int   LEN2  = bsl::strlen(SPEC2);

                    for (int j2 = 0; j2 < LEN2; ++j2) {

                        const char S1 = SPEC1[j1];
                        const char S2 = SPEC2[j2];

                        bslma_TestAllocator alloc1(veryVeryVerbose);
                        bslma_TestAllocator alloc2(veryVeryVerbose);

                        if (veryVerbose) { P(SPEC1); P(SPEC2); }

                        Catalog catalog1(&alloc1);
                        populateCatalog(&catalog1, SPEC1);

                        Catalog catalog2(&alloc2);
                        populateCatalog(&catalog2, SPEC2);

                        const void *V1_A = getValueA(S1);
                        const void *V1_B = getValueB(S1);
                        const void *V2_A = getValueA(S2);

                        Obj mX(&catalog1); const Obj& X = mX;
                        Obj mY(&catalog2); const Obj& Y = mY;

                        mX.makeSelection(j1, V1_A);
                        mY.makeSelection(j2, V2_A);

                        if (i1 == i2 && j1 == j2) {
                            // This assumes that each spec is unique

                            LOOP2_ASSERT(LINE1, LINE2,    X == Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X != Y));

                            // values are equal, x null, y non-null
                            mX.setNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            // values are equal, x null, y null
                            mY.setNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    X == Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X != Y));

                            // values are equal, x non-null, y null
                            mX.clearNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            // values are equal, x non-null, y non-null
                            mX.clearNullnessBit();
                            mY.clearNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    X == Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X != Y));

                            // Some perturbation
                            // - - - - - - - - - - - - - - - - - - - - - - - -
                            mX.makeSelection(j1, V1_B);

                            LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X == Y));
                            LOOP4_ASSERT(LINE1, LINE2, X, Y,    X != Y);

                            // values are not equal, x null, y non-null
                            mX.setNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            // values are not equal, x null, y null
                            mY.setNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            // values are not equal, x non-null, y null
                            mX.clearNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            // values are not equal, x non-null, y non-null
                            mX.clearNullnessBit();
                            mY.clearNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            mY.makeSelection(j1, V1_B);

                            LOOP4_ASSERT(LINE1, LINE2, X, Y,    X == Y);
                            LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X != Y));

                            // values are equal, x null, y non-null
                            mX.setNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            // values are equal, x null, y null
                            mY.setNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    X == Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X != Y));

                            // values are equal, x non-null, y null
                            mX.clearNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            // values are equal, x non-null, y non-null
                            mX.clearNullnessBit();
                            mY.clearNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    X == Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X != Y));

                            // UNSET
                            // - - - - - - - - - - - - - - - - - - - - - - - -
                            mX.makeSelection(-1, 0);

                            LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X == Y));
                            LOOP4_ASSERT(LINE1, LINE2, X, Y,    X != Y);

                            // values are not equal, x null, y non-null
                            mX.setNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            // values are not equal, x null, y null
                            mY.setNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            // values are not equal, x non-null, y null
                            mX.clearNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            // values are not equal, x non-null, y non-null
                            mX.clearNullnessBit();
                            mY.clearNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            mY.makeSelection(-1, 0);
                            mX.setNullnessBit();
                            LOOP4_ASSERT(LINE1, LINE2, X, Y,    X == Y);
                            LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X != Y));

                            // values are equal, x non-null, y null
                            mX.clearNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            // values are equal, x non-null, y non-null
                            mX.clearNullnessBit();
                            mY.clearNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    X == Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X != Y));
                        }
                        else {
                            LOOP2_ASSERT(LINE1, LINE2,    X != Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X == Y));

                            // values are not equal, x null, y non-null
                            mX.setNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            // values are not equal, x null, y null
                            mY.setNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            // values are not equal, x non-null, y null
                            mX.clearNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);

                            // values are not equal, x non-null, y non-null
                            mX.clearNullnessBit();
                            mY.clearNullnessBit();
                            LOOP2_ASSERT(LINE1, LINE2,    !(X == Y));
                            LOOP2_ASSERT(LINE1, LINE2,      X != Y);
                        }
                    }
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING print() FUNCTION AND OUTPUT (<<) OPERATOR:
        //
        // Concerns:
        //   The print() function is correctly forwarded to the appropriate
        //   print function for the type stored in this object.  The
        //   '<<' operator prints the output on one line.
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostringstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& stream,
        //                       int           level = 0,
        //                       int           spacesPerLevel = 4) const;
        //   ostream& operator<<(ostream&, const bdem_ChoiceHeader&);
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                          << "Testing Output Functions" << bsl::endl
                          << "========================" << bsl::endl;

        if (veryVerbose) {
                      bsl::cout << "\tTesting empty object" << bsl::endl; }
        {
            const char *EXP_P1 = "    {\n"
                                 "        VOID NULL\n"
                                 "    }\n";
            const char *EXP_P2 = "{\n"
                                 "        VOID NULL\n"
                                 "    }\n";
            const char *EXP_P3 = "    { VOID NULL }";
            const char *EXP_P4 = "{ VOID NULL }";
            const char *EXP_OP = "{ VOID NULL }";

            Catalog x1;
            populateCatalog(&x1, "");

            Obj mX(&x1); const Obj& X = mX;

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

        if (veryVerbose) {
            bsl::cout << "\tTesting empty object (NULL)" << bsl::endl; }
        {
            const char *EXP_P1 = "    {\n"
                                 "        VOID NULL\n"
                                 "    }\n";
            const char *EXP_P2 = "{\n"
                                 "        VOID NULL\n"
                                 "    }\n";
            const char *EXP_P3 = "    { VOID NULL }";
            const char *EXP_P4 = "{ VOID NULL }";
            const char *EXP_OP = "{ VOID NULL }";

            Catalog x1;
            populateCatalog(&x1, "");

            Obj mX(&x1); const Obj& X = mX;

            mX.setNullnessBit();

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

        if (veryVerbose) {
                      bsl::cout << "\tTesting non-empty object" << bsl::endl; }

        const int MAX_PRINT_TESTS = 4;
        const struct {
            int         d_line;       // Line number
            const char  d_spec;       // Type Specification string
            char        d_valueSpec;  // Spec specifying the value for that
            // type.  Can be one of 'A', 'B' or 'N'
            // corresponding to the A, B or N value
            // for that type.
            struct {
                int         d_level;           // Level to print at
                int         d_spacesPerLevel;  // spaces per level to print at
                const char *d_printExpOutput;  // Expected o/p from print
                const char *d_printExpOutputN; // Expected o/p from print(null)
            } PRINT_TESTS[MAX_PRINT_TESTS];
            const char *d_outputOpExpOutput;  // Expected o/p from operator<<
            const char *d_outputOpExpOutputN; // Expected o/p from operator<<
                                              // (null)
        } DATA[] = {
            {
                L_,
                'A',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        CHAR Bye world!\n"
                        "    }\n",
                        "    {\n"
                        "        CHAR NULL\n"
                        "    }\n",
                    },
                    {
                        -1,
                        4,
                        "{\n"
                        "        CHAR Bye world!\n"
                        "    }\n",
                        "{\n"
                        "        CHAR NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { CHAR Bye world! }",
                        "    { CHAR NULL }",
                    },
                    {
                        -1,
                        -4,
                        "{ CHAR Bye world! }",
                        "{ CHAR NULL }",
                    },
                },
                "{ CHAR Bye world! }",
                "{ CHAR NULL }"
            },

            {
                L_,
                'B',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        SHORT A penny\n"
                        "    }\n",
                        "    {\n"
                        "        SHORT NULL\n"
                        "    }\n",
                    },
                    {
                        -1,
                        4,
                        "{\n"
                        "        SHORT A penny\n"
                        "    }\n",
                        "{\n"
                        "        SHORT NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { SHORT A penny }",
                        "    { SHORT NULL }",
                    },
                    {
                        -1,
                        -4,
                        "{ SHORT A penny }",
                        "{ SHORT NULL }",
                    },
                },
                "{ SHORT A penny }",
                "{ SHORT NULL }"
            },

            {
                L_,
                'C',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        INT Early to bed and early to rise\n"
                        "    }\n",
                        "    {\n"
                        "        INT NULL\n"
                        "    }\n",
                    },
                    {
                        -1,
                        4,
                        "{\n"
                        "        INT Early to bed and early to rise\n"
                        "    }\n",
                        "{\n"
                        "        INT NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { INT Early to bed and early to rise }",
                        "    { INT NULL }",
                    },
                    {
                        -1,
                        -4,
                        "{ INT Early to bed and early to rise }",
                        "{ INT NULL }",
                    },
                },
                "{ INT Early to bed and early to rise }",
                "{ INT NULL }"
            },

            {
                L_,
                'D',
                'A',
                {
                    {
                        1,
                        4,
                        "    {\n"
                        "        INT64 A man a plan a canal Panama.\n"
                        "    }\n",
                        "    {\n"
                        "        INT64 NULL\n"
                        "    }\n",
                    },
                    {
                        -1,
                        4,
                        "{\n"
                        "        INT64 A man a plan a canal Panama.\n"
                        "    }\n",
                        "{\n"
                        "        INT64 NULL\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { INT64 A man a plan a canal Panama. }",
                        "    { INT64 NULL }",
                    },
                    {
                        -1,
                        -4,
                        "{ INT64 A man a plan a canal Panama. }",
                        "{ INT64 NULL }",
                    },
                },
                "{ INT64 A man a plan a canal Panama. }",
                "{ INT64 NULL }",
            },
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE      = DATA[i].d_line;
            const char  SPEC      = DATA[i].d_spec;
            const char  VAL_SPEC  = DATA[i].d_valueSpec;
            const char *OP_EXP    = DATA[i].d_outputOpExpOutput;
            const char *OP_EXPN   = DATA[i].d_outputOpExpOutputN;

            const void *VALUE;
            if ('A' == VAL_SPEC) {
                VALUE = getValueA(SPEC);
            }
            else if ('B' == VAL_SPEC) {
                VALUE = getValueB(SPEC);
            }
            else if ('N' == VAL_SPEC) {
                VALUE = getValueN(SPEC);
            }
            else {
                ASSERT(0);
            }

            bslma_TestAllocator alloc(veryVeryVerbose);

            Catalog x1(&alloc);
            x1.push_back(getDescriptor(SPEC));

            Obj mX(&x1); const Obj& X = mX;
            mX.makeSelection(0, VALUE);

            bsl::ostringstream os;
            os << X;
            LOOP3_ASSERT(LINE, OP_EXP,
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
                LOOP3_ASSERT(LINE,
                             PRINT_EXP,
                             os1.str(),
                             0 == bsl::strcmp(PRINT_EXP, os1.str().c_str()));
            }

            mX.setNullnessBit();

            bsl::ostringstream osn;
            osn << X;
            LOOP3_ASSERT(LINE, OP_EXPN,
                         osn.str(),
                         0 == bsl::strcmp(OP_EXPN, osn.str().c_str()));

            for (int j = 0; j < MAX_PRINT_TESTS; ++j) {
                const int   LEVEL     = DATA[i].PRINT_TESTS[j].d_level;
                const int   SPL       =
                                      DATA[i].PRINT_TESTS[j].d_spacesPerLevel;
                const char *PRINT_EXP =
                                      DATA[i].PRINT_TESTS[j].d_printExpOutputN;

                bsl::ostringstream os1;
                X.print(os1, LEVEL, SPL);
                LOOP3_ASSERT(LINE,
                             PRINT_EXP,
                             os1.str(),
                             0 == bsl::strcmp(PRINT_EXP, os1.str().c_str()));
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CREATORS AND PRIMARY ACCESSORS:
        //
        // Concerns:
        //   1. The primary constructor constructs the object correctly and
        //      the destructor destroys the object and releases its resources.
        //   2. The accessors return the correct values
        //
        // Plan:
        //   1. Create a 'bdem_ChoiceHeader' object from a types catalog
        //      generated by the 'populateCatalog' function from a pre-decided
        //      specification.  Additionally, create objects by passing
        //      different allocation modes.
        //      a. Confirm that the allocator passed to the catalog is the
        //         allocator of the created object.
        //      b. Confirm the allocation mode is correct
        //   2. For each type in the types catalog of the created object:
        //      Call the primary manipulator to make the object hold a value
        //      of the specified type as the current selection.  Verify using
        //      the accessors that the selection pointer and selector value
        //      that the selection was correctly changed.  Perform this for
        //      two different data values of that type.  Finally, call the
        //      makeSelection function with a selector value of -1.  Confirm
        //      that the accessors return the expected values.
        //      Note that this test is the bootstrap test and assumes that the
        //      primary manipulator is correct.  The primary manipulator is
        //      tested in a later test.
        //
        // Testing:
        //   bdem_ChoiceHeader(const DescriptorCatalog       *catalogPtr,
        //                     AggOption::AllocationStrategy  allocMode);
        //   ~bdem_ChoiceHeader();
        //   void *selectionPointer();
        //   int flags();
        //   int numSelections() const;
        //   int selector() const;
        //   const bdem_Descriptor *selectionDescriptor(int index) const;
        //   const void *selectionPointer() const;
        //   const int flags() const;
        //   Catalog *catalog() const;
        //   Allocator *allocator() const;
        //   Mode allocMode() const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING BASIC ACCESSORS"
                               << "\n=======================" << bsl::endl;

        static const struct TestRow {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "" },

            { L_,       "A" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },

            { L_,       "AA" },
            { L_,       "AB" },
            { L_,       "AC" },
            { L_,       "AD" },

            { L_,       "DD" },
            { L_,       "DC" },
            { L_,       "DB" },
            { L_,       "DA" },

            { L_,       "BB" },
            { L_,       "CC" },
            { L_,       "BC" },
            { L_,       "CB" },

            { L_,       "ABC" },
            { L_,       "DCB" },

            { L_,       "ABCD" },
            { L_,       "DCBA" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int k = 0; k < 4; ++k) {
            AggOption::AllocationStrategy MODE =
                                     (AggOption::AllocationStrategy) k;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_catalogSpec;
                const int   LEN   = bsl::strlen(SPEC);

                bslma_TestAllocator testAllocator(veryVeryVerbose);
                bdema_SequentialAllocator  seqAlloc(&testAllocator);
                bslma_Allocator           *alloc = &testAllocator;

                if (veryVerbose) { P(MODE); }

                if (AggOption::BDEM_SUBORDINATE == MODE) {
                    alloc = &seqAlloc;
                }

                Catalog catalog(alloc);
                populateCatalog(&catalog, SPEC);

                if (veryVerbose) { P(SPEC) P(MODE) }

                LOOP_ASSERT(LINE, LEN == catalog.size());

                // Test that default allocation mode is passed through
                // correctly
                {
                    Obj mY(&catalog); const Obj& Y = mY;
                    LOOP_ASSERT(LINE, &catalog == Y.catalog());
                    LOOP_ASSERT(LINE, alloc    == Y.allocator());
                    LOOP_ASSERT(LINE,
                                AggOption::BDEM_PASS_THROUGH == Y.allocMode());
                    LOOP_ASSERT(LINE, -1       == Y.selector());
                    LOOP_ASSERT(LINE, Y.isSelectionNull());
                    LOOP_ASSERT(LINE, LEN == Y.numSelections());
                    LOOP_ASSERT(LINE, EType::BDEM_VOID ==
                                       Y.selectionDescriptor(-1)->d_elemEnum);
                }

                Obj mX(&catalog, MODE); const Obj& X = mX;

                LOOP_ASSERT(LINE, &catalog == X.catalog());
                LOOP_ASSERT(LINE, alloc    == X.allocator());
                LOOP3_ASSERT(LINE, MODE, X.allocMode(),
                             (MODE & ~AggOption::BDEM_OWN_ALLOCATOR_FLAG)
                                                            == X.allocMode());
                LOOP_ASSERT(LINE, LEN == X.numSelections());
                LOOP_ASSERT(LINE, -1  == X.selector());
                LOOP_ASSERT(LINE, X.isSelectionNull());
                LOOP_ASSERT(LINE, EType::BDEM_VOID ==
                               X.selectionDescriptor(-1)->d_elemEnum);

                for (int j = 0; j < LEN; ++j) {
                    char        S     = SPEC[j];
                    const void *VA    = getValueA(S);
                    const void *VB    = getValueB(S);
                    const Desc *DESC  = getDescriptor(S);

                    LOOP3_ASSERT(i, j, S, DESC == X.selectionDescriptor(j));

                    void *value = mX.makeSelection(j, VA);
                    LOOP_ASSERT(LINE, !X.isSelectionNull());

                          void *mP = mX.selectionPointer();
                    const void *P  = X.selectionPointer();

                    LOOP3_ASSERT(i, j, S, P  == value);
                    LOOP3_ASSERT(i, j, S, mP == value);

                    LOOP3_ASSERT(i, j, S, compare(VA, P, S));
                    LOOP3_ASSERT(i, j, S, j == X.selector());

                          int mN = mX.flags();
                    const int N  = X.flags();

                    LOOP3_ASSERT(i, j, S, 0 == mN);
                    LOOP3_ASSERT(i, j, S, 0 == N);

                    value = mX.makeSelection(j, VB);
                    LOOP_ASSERT(LINE, !X.isSelectionNull());

                    mP = mX.selectionPointer();
                    P  = X.selectionPointer();

                    LOOP3_ASSERT(i, j, S, P  == value);
                    LOOP3_ASSERT(i, j, S, mP == value);

                    LOOP3_ASSERT(i, j, S, compare(VB, P, S));
                    LOOP3_ASSERT(i, j, S, j == X.selector());

                    mN = mX.flags();

                    LOOP3_ASSERT(i, j, S, 0 == mN);

                    value = mX.makeSelection(-1, 0);
                    LOOP3_ASSERT(i, j, S, -1 == X.selector());
                    LOOP_ASSERT(LINE, X.isSelectionNull());

                    mN = mX.flags();

                    LOOP3_ASSERT(i, j, S, 1 == mN);
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING helper functions
        //
        // Concerns:
        //  1. Confirm that 'populateCatalog' functions works as expected
        //  2. Confirm that 'getValueA', 'getValueB', 'getValueN' work as
        //     expected.
        //
        // Plan:
        //  1. Test generation of the types catalog with various spec
        //     strings.
        //  2. Test that the getValue functions return the expected value for
        //     each value.  (Brute force testing).
        //
        // Testing:
        //   void populateCatalog(Catalog *catalog, const char *spec);
        //   const void *getValueA(char spec);
        //   const void *getValueB(char spec);
        //   const void *getValueN(char spec);
        //   const Desc *getDescriptor(char spec)
        //   int compare(const void *p, const void *q, char spec);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting 'populateCatalog'"
                               << "\n=========================" << bsl::endl;

        const struct {
                int         d_line;
                const char *d_spec;
        } SPECS[] =
        {
            { L_,       "" },

            { L_,       "A" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },

            { L_,       "AA" },
            { L_,       "AB" },
            { L_,       "AC" },
            { L_,       "AD" },

            { L_,       "DD" },
            { L_,       "DC" },
            { L_,       "DB" },
            { L_,       "DA" },

            { L_,       "BB" },
            { L_,       "CC" },
            { L_,       "BC" },
            { L_,       "CB" },

            { L_,       "ABC" },
            { L_,       "DCB" },

            { L_,       "ABCD" },
            { L_,       "DCBA" },
        };
        const int NUM_SPECS = sizeof (SPECS) / sizeof (*SPECS);

        if (verbose)
            bsl::cout << "\nTesting 'populateCatalog' for a different length"
                      << " catalog" << bsl::endl;

        bslma_TestAllocator  testAllocator;
        bslma_Allocator     *Z = &testAllocator;

        for (int i = 0; i < NUM_SPECS; ++i) {
            const int   LINE = SPECS[i].d_line;
            const char *SPEC = SPECS[i].d_spec;
            const int   LEN  = bsl::strlen(SPEC);

            if (veryVerbose) { P(SPEC) }

            Catalog x1(Z);  const Catalog& X1 = x1;
            populateCatalog(&x1, SPEC);
            LOOP_ASSERT(LINE, LEN == X1.size());

            for (int j = 0; j < LEN; ++j) {
                LOOP_ASSERT(LINE, getDescriptor(SPEC[j]) == X1[j]);
            }
        }

        if (verbose)
            bsl::cout << "\nTesting 'getValueA'." << bsl::endl;
        {
            ASSERT(SS_1  == *(SmallString *)getValueA(SPECIFICATIONS[0]));
            ASSERT(SSA_1 == *(SmallStringAlloc *)getValueA(SPECIFICATIONS[1]));
            ASSERT(LS_1  == *(LargeString *)getValueA(SPECIFICATIONS[2]));
            ASSERT(LSA_1 == *(LargeStringAlloc *)getValueA(SPECIFICATIONS[3]));
        }

        if (verbose)
            bsl::cout << "\nTesting 'getValueB'." << bsl::endl;
        {
            ASSERT(SS_2  == *(SmallString *)getValueB(SPECIFICATIONS[0]));
            ASSERT(SSA_2 == *(SmallStringAlloc *)getValueB(SPECIFICATIONS[1]));
            ASSERT(LS_2  == *(LargeString *)getValueB(SPECIFICATIONS[2]));
            ASSERT(LSA_2 == *(LargeStringAlloc *)getValueB(SPECIFICATIONS[3]));
        }

        if (verbose)
            bsl::cout << "\nTesting 'getValueN'." << bsl::endl;
        {
            ASSERT(SS_UNSET  == *(SmallString *) getValueN(SPECIFICATIONS[0]));
            ASSERT(SSA_UNSET ==
                           *(SmallStringAlloc *) getValueN(SPECIFICATIONS[1]));
            ASSERT(LS_UNSET  == *(LargeString *) getValueN(SPECIFICATIONS[2]));
            ASSERT(LSA_UNSET ==
                           *(LargeStringAlloc *) getValueN(SPECIFICATIONS[3]));
        }

        if (verbose)
            bsl::cout << "\nTesting 'getDescriptor'." << bsl::endl;
        {
            for (int i = 0; i < SPEC_LEN; ++i) {
                LOOP_ASSERT(SPECIFICATIONS[i],
                            DESCRIPTORS[i] ==
                                             getDescriptor(SPECIFICATIONS[i]));
            }
        }

        if (verbose)
            bsl::cout << "\nTesting 'compare'." << bsl::endl;
        {
            for (int i = 0; i < SPEC_LEN; ++i) {
                const char S   = SPECIFICATIONS[i];
                const void *VA = getValueA(S);
                const void *VB = getValueB(S);

                LOOP_ASSERT(S, compare(VA, VA, S));
                LOOP_ASSERT(S, !compare(VA, VB, S));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TEST CLASS 'FixedString'
        //
        // Concerns:
        //   1. Objects instantiated from FixedString have the expected
        //      footprint.
        //   2. Objects instantiated from FixedString have
        //      'bslma::IsBslmaAllocator' trait if and only if 'true'
        //      specified as the 'ALLOC_TRAIT' template parameter.
        //   3. The constructor, destructor, copy-constructor, and assignment
        //      operator work as designed.  Constructors increment the
        //      instance count; destructor decrements the instance count.
        //   4. Objects can be tested for equality and inequality.
        //   5. Objects can be streamed in and out.
        //   6. Objects can be printed.
        //
        // Plan:
        //   The design of 'FixedString' is extremely simple.  We will test
        //   each function by simply throwing one or two test cases at it.
        //   All four versions of 'FixedString' ('SmallString',
        //   'SmallStringAlloc', 'LargeString', and 'LargeStringAlloc' have
        //   the same implementation, so only those attributes that differ
        //   between them are tested for all of them.  Otherwise, only one of
        //   the types is tested.
        //
        // Testing:
        //   FixedString test class.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING TEST CLASSES"
                               << "\n====================" << bsl::endl;

        const SmallString *DATA[] = { &SS_UNSET, &SS_1, &SS_2 };
        const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

        if (veryVerbose) bsl::cout << "Testing footprint" << bsl::endl;

        ASSERT(SFP == sizeof(SmallString));
        ASSERT(SFP == sizeof(SmallStringAlloc));
        ASSERT(LFP == sizeof(LargeString));
        ASSERT(LFP == sizeof(LargeStringAlloc));

        ASSERT(SMALL_STRING_BUFSIZE - 1 == SmallString::maxLength());
        ASSERT(SMALL_STRING_BUFSIZE - 1 == SmallStringAlloc::maxLength());
        ASSERT(LARGE_STRING_BUFSIZE - 1 == LargeString::maxLength());
        ASSERT(LARGE_STRING_BUFSIZE - 1 == LargeStringAlloc::maxLength());

        if (veryVerbose) bsl::cout << "Testing traits" << bsl::endl;
        ASSERT(! (bslalg_HasTrait<SmallString,
                                 bslalg_TypeTraitUsesBslmaAllocator>::VALUE));
        ASSERT(  (bslalg_HasTrait<SmallStringAlloc,
                                 bslalg_TypeTraitUsesBslmaAllocator>::VALUE));
        ASSERT(! (bslalg_HasTrait<LargeString,
                                 bslalg_TypeTraitUsesBslmaAllocator>::VALUE));
        ASSERT(  (bslalg_HasTrait<LargeStringAlloc,
                                 bslalg_TypeTraitUsesBslmaAllocator>::VALUE));

        bslma_TestAllocator defAlloc;
        bslma_DefaultAllocatorGuard defAllocGuard(&defAlloc);

        if (veryVerbose) bsl::cout << "Testing constructors" << bsl::endl;
        int initialCount = SmallString::instanceCount();
        {
            bslma_TestAllocator t1;
            SmallString w(&t1);
            ASSERT(&t1 == w.get_allocator());
            ASSERT(0 == bsl::strcmp("", w.c_str()));
            ASSERT(SmallString::instanceCount() == initialCount + 1);
            ASSERT(0 == defAlloc.numBytesInUse());
            ASSERT(0 == t1.numBytesInUse());
        }
        ASSERT(SmallString::instanceCount() == initialCount);
        {
            bslma_TestAllocator t1;

            // ALPHABET2 is a longer than the longest SmallString.
            static const char ALPHABET2[] = "abcdefghijklmnopqrstuvwxyz"
                                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            ASSERT(sizeof(ALPHABET2) - 1 > SmallString::maxLength());

            SmallString w(ALPHABET2, &t1);
            ASSERT(&t1 == w.get_allocator());
            ASSERT(SmallString::maxLength() == bsl::strlen(w.c_str()));
            ASSERT(0 == bsl::strncmp(ALPHABET2, w.c_str(),
                                     SmallString::maxLength()));
            ASSERT(SmallString::instanceCount() == initialCount + 1);
            ASSERT(0 == defAlloc.numBytesInUse());
            ASSERT(0 == t1.numBytesInUse());

            bslma_TestAllocator t2;
            SmallString x(w, &t2);
            ASSERT(&t2 == x.get_allocator());
            ASSERT(0 == bsl::strcmp(w.c_str(), x.c_str()));
            ASSERT(SmallString::instanceCount() == initialCount + 2);
            ASSERT(0 == defAlloc.numBytesInUse());
            ASSERT(0 == t1.numBytesInUse());
            ASSERT(0 == t2.numBytesInUse());
        }
        ASSERT(SmallString::instanceCount() == initialCount);

        if (veryVerbose) bsl::cout << "Testing assignment" << bsl::endl;
        {
            bslma_TestAllocator t1;
            SmallString w(&t1);
            ASSERT(&t1 == w.get_allocator());
            ASSERT(0 == t1.numBytesInUse());
            ASSERT(0 == bsl::strlen(w.c_str()));
            ASSERT(SmallString::instanceCount() == initialCount + 1);

            w = SS_1;
            ASSERT(&t1 == w.get_allocator());
            ASSERT(0 == t1.numBytesInUse());
            ASSERT(0 == bsl::strcmp(SS_1.c_str(), w.c_str()));
            ASSERT(SmallString::instanceCount() == initialCount + 1);

            w = SS_2;
            ASSERT(&t1 == w.get_allocator());
            ASSERT(0 == t1.numBytesInUse());
            ASSERT(0 == bsl::strcmp(SS_2.c_str(), w.c_str()));
            ASSERT(SmallString::instanceCount() == initialCount + 1);
        }
        ASSERT(SmallString::instanceCount() == initialCount);

        if (veryVerbose) bsl::cout << "Testing equality" << bsl::endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const SmallString& U = *DATA[i];
            for (int j = 0; j < NUM_DATA; ++j) {
                const SmallString& V = *DATA[j];

                if (i == j) {
                    ASSERT(U == V);
                    ASSERT(! (U != V));
                }
                else {
                    ASSERT(U != V);
                    ASSERT(! (U == V));
                }
            }
        }

        if (veryVerbose) bsl::cout << "Testing streaming" << bsl::endl;
        {
            ASSERT(1 == SmallString::maxSupportedBdexVersion());

            bdesb_MemOutStreamBuf osb(&bslma_NewDeleteAllocator::singleton());
            bdex_TestOutStreamFormatter outStream(&osb);

            SS_UNSET.bdexStreamOut(outStream, 1);
            SS_1.bdexStreamOut(outStream, 1);
            SS_2.bdexStreamOut(outStream, 1);
            ASSERT(outStream);

            bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
            bdex_TestInStreamFormatter inStream(&isb);
            inStream.setSuppressVersionCheck(true);

            bslma_TestAllocator t1;
            SmallString w(&t1), x(&t1), y(&t1);
            w.bdexStreamIn(inStream, 1);
            ASSERT(inStream);
            ASSERT(SS_UNSET == w);
            x.bdexStreamIn(inStream, 1);
            ASSERT(inStream);
            ASSERT(SS_1 == x);
            y.bdexStreamIn(inStream, 1);
            ASSERT(inStream);
            ASSERT(SS_2 == y);
        }

        if (veryVerbose) bsl::cout << "Testing printing" << bsl::endl;
        for (int i = 0; i < NUM_DATA; ++i)
        {
            bdesb_MemOutStreamBuf osb(&bslma_NewDeleteAllocator::singleton());
            bsl::ostream os(&osb);
            const SmallString& S = *DATA[i];
            os << S;
            ASSERT(bsl::strlen(S.c_str()) == osb.length());
            ASSERT(0 == bsl::strncmp(S.c_str(), (const char*) osb.data(),
                                     osb.length()));
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
        //   bdem_ChoiceHeader object are as expected, check that memory is
        //   being used as expected, and (if in verbose mode), print the value
        //   of the created or modified object.
        //   0. Construct a types catalog (vector<bdem_ElemType::Type>)
        //      containing the types, (INT, STRING, DOUBLE).
        //   1. Create an object, choice1, using the types catalog.
        //   2. Modify choice1 so that it holds an int, 35.
        //   3. Create an object, choice2, that holds an unset string.
        //   3. Modify the value in choice2 so that it holds "hello world"
        //   4. Copy-construct choice3 from choice2.
        //   5. Assign choice2 = choice1.
        //   6. Change choice3 to hold a double
        //   7. Stream each choice into a test stream.
        //   8. Read all choices from the test stream into a vector of
        //      choices.
        //
        // Testing:
        //   This test case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        bslma_TestAllocator t1;

        bdem_ChoiceHeader::DescriptorCatalog catalog(&t1);
        const int INT_IDX = catalog.size();
        catalog.push_back(&bdem_Properties::d_intAttr);
        const int STR_IDX = catalog.size();
        catalog.push_back(&bdem_Properties::d_stringAttr);
        const int DBL_IDX = catalog.size();
        catalog.push_back(&bdem_Properties::d_doubleAttr);

        const int baseBlocks = t1.numBlocksInUse();

        if (verbose) bsl::cout << "Construct" << bsl::endl;
        bdem_ChoiceHeader choice1(&catalog);
        ASSERT(&bdem_Properties::d_voidAttr ==
               choice1.selectionDescriptor(choice1.selector()));
        ASSERT(-1 == choice1.selector());
        ASSERT(0 == choice1.selectionPointer());
        ASSERT(choice1.isSelectionNull());
        ASSERT(baseBlocks == t1.numBlocksInUse());

        if (verbose) bsl::cout << "Integer value" << bsl::endl;
        const int iv = 35;
        int* ip = (int*) choice1.makeSelection(INT_IDX, &iv);
        ASSERT(iv == *ip);
        ASSERT(choice1.selectionPointer() == ip);
        ASSERT(choice1.selector() == INT_IDX);
        ASSERT(!choice1.isSelectionNull());
        ASSERT(&bdem_Properties::d_intAttr ==
               choice1.selectionDescriptor(choice1.selector()));
        // Verify in-place allocation:
        ASSERT((char*) ip > (char*) &choice1 &&
               (char*) ip - (char*) &choice1 < sizeof(choice1));
        ASSERT(baseBlocks == t1.numBlocksInUse());

        if (verbose) bsl::cout << "Construct unset string" << bsl::endl;
        bdem_ChoiceHeader choice2(&catalog);
        choice2.makeSelection(STR_IDX);
        ASSERT(choice2.selector() == STR_IDX);
        ASSERT(choice2.isSelectionNull());
        ASSERT(&bdem_Properties::d_stringAttr ==
               choice2.selectionDescriptor(choice2.selector()));
        bsl::string *sp = (bsl::string*) choice2.selectionPointer();
        ASSERT("" == *sp);
        // Verify in-place allocation:
        ASSERT((char*) sp > (char*) &choice2 &&
               (char*) sp - (char*) &choice2 < sizeof(choice2));
        ASSERT(t1.numBlocksInUse() <= baseBlocks + 1);

        // Verify that string is using specified allocator - revise
        *sp = "Hello world";
        ASSERT("Hello world" == *sp);

        if (verbose) bsl::cout << "Copy-construct string choice" << bsl::endl;
        bdem_ChoiceHeader choice3(choice2);
        ASSERT(choice2.selector() == STR_IDX);
        ASSERT(choice3.selector() == STR_IDX);
        ASSERT(choice3.isSelectionNull());
        ASSERT(&bdem_Properties::d_stringAttr ==
               choice2.selectionDescriptor(choice2.selector()));
        ASSERT(&bdem_Properties::d_stringAttr ==
               choice3.selectionDescriptor(choice3.selector()));
        bsl::string* sp2 = (bsl::string*) choice2.selectionPointer();
        bsl::string* sp3 = (bsl::string*) choice3.selectionPointer();
        ASSERT("Hello world" == *sp2);
        ASSERT("Hello world" == *sp3);
        ASSERT(sp2 != sp3);

        if (verbose) bsl::cout << "Assign int choice" << bsl::endl;
        choice2 = choice1;
        ASSERT(choice1.selector() == INT_IDX);
        ASSERT(choice2.selector() == INT_IDX);
        ASSERT(!choice2.isSelectionNull());
        ASSERT(&bdem_Properties::d_intAttr ==
               choice1.selectionDescriptor(choice1.selector()));
        ASSERT(&bdem_Properties::d_intAttr ==
               choice2.selectionDescriptor(choice2.selector()));
        int* ip1 = (int*) choice1.selectionPointer();
        int* ip2 = (int*) choice2.selectionPointer();
        ASSERT(iv == *ip1);
        ASSERT(iv == *ip2);
        ASSERT(ip1 != ip2);

        if (verbose) bsl::cout << "Change string choice to double"
                               << bsl::endl;
        double dv = 1.23;
        double *dp = (double*) choice3.makeSelection(DBL_IDX, &dv);
        ASSERT(choice3.selector() == DBL_IDX);
        ASSERT(choice3.selectionPointer() == dp);
        ASSERT(!choice3.isSelectionNull());
        ASSERT(*dp == dv);
        ASSERT(dp != &dv);
        ASSERT(baseBlocks == t1.numBlocksInUse());

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
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
