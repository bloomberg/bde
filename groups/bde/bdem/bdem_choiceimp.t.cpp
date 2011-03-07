// bdem_choiceimp.t.cpp                                               -*-C++-*-

#include <bdem_choiceimp.h>
#include <bdem_functiontemplates.h>
#include <bdem_properties.h>

#include <bdema_sequentialallocator.h>
#include <bdesb_memoutstreambuf.h>
#include <bdesb_fixedmeminstreambuf.h>
#include <bdetu_unset.h>
#include <bdeu_printmethods.h>
#include <bdex_testoutstream.h>
#include <bdex_testinstream.h>

#include <bslalg_typetraits.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_types.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This component is a low-level component that has no information about the
// various data types that can be stored in it.  It knows only about the
// catalog of descriptors that it gets constructed with.  Additionally the
// choice header object used in construction of choice imp object can store
// values in one of two ways.  Either it can store the value inside its
// footprint or by allocating additional memory for it and storing it there.
//
// So all our testing can be done by constructing a set of four data types,
// one that can be stored in the footprint of the choice header, the second
// that is too large for that and needs to be stored in allocated memory, and
// their counterparts that take an allocator.  Also, we need to define
// descriptors for each of those new types.
//
// After that we need to construct the choice imp object with various
// combinations of the newly constructed types and test the functionality as
// we would a value-semantic type.  Note that the private function
// 'bdexMinorVersion' is tested indirectly.
//-----------------------------------------------------------------------------
// CREATORS
// [10] bdem_ChoiceImp(
//        bdem_AggregateOption::AllocationStrategy  allocMode,
//        bslma_Allocator                          *basicAllocator = 0);
// [10] bdem_ChoiceImp(const bdem_ElemType::Type    selectionTypes[],
//        int                                       numSelectionTypes,
//        const bdem_Descriptor *const              attrLookupTbl[],
//        bdem_AggregateOption::AllocationStrategy  allocMode,
//        bslma_Allocator                          *basicAllocator=0);
// [10] bdem_ChoiceImp(
//        const bdem_ChoiceHeader&                  choiceHeader,
//        bdem_AggregateOption::AllocationStrategy  allocMode,
//        bslma_Allocator                          *basicAllocator = 0);
// [ 7] bdem_ChoiceImp(const bdem_ChoiceImp&       original,
//        bslma_Allocator                          *basicAllocator = 0);
// [ 7] bdem_ChoiceImp(const bdem_ChoiceImp&       original,
//        bdem_AggregateOption::AllocationStrategy allocMode,
//        bslma_Allocator                          *basicAllocator = 0);
// [ 4] bdem_ChoiceImp(bslma_Allocator             *basicAlloc = 0);
// [ 4] ~bdem_ChoiceImp();

// MANIPULATORS
// [ 8] bdem_ChoiceHeader& operator=(const bdem_ChoiceHeader& rhs);
// [ 9] bdexStreamInImp(...);
// [ 4] void *selectionPointer();
// [ 4] void *setSelectionValue(const void *value);
// [13] void reset();
// [13] void reset(const bdem_ElemType::Type    selectionTypes[],
//                 int                          numSelections,
//                 const bdem_Descriptor *const attrLookup[]);
// [13] void clear();

// ACCESSORS
// [ 4] void addSelection(const bdem_Descriptor *elemAttr);
// [ 4] int selector() const;
// [ 4] int numSelections() const;
// [ 4] int size() const;
// [ 4] void *makeSelection(int index);
// [ 4] void *makeSelection(int index, const void *value);
// [ 4] bdem_ElemType::Type selectionType(int index) const;
// [ 4] const void *selectionPointer() const;
// [ 4] bdem_ConstElemRef selection() const;
// [ 9] bdexStreamOutImp(...) const;
// [ 5] bsl::ostream& print(...) const;

// FREE OPERATORS
// [ 6] bool operator==(const ChoiceImp& lhs, const ChoiceImp& rhs);
// [ 6] bool operator!=(const ChoiceImp& lhs, const ChoiceImp& rhs);
// [ 5] ostream& operator<<(ostream& stream, const ChoiceImp& rhs);
//
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS
// [ 3] HELPER FUNCTIONS
// [13] USAGE EXAMPLE
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
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush; // P(X) w/o
                                                                  // '\n'
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdem_ChoiceImp                       Obj;
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

typedef bdex_TestOutStream                   Out;
typedef bdex_TestInStream                    In;

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
//   LargeString      - sizeof(bdem_ChoiceImp)-byte object without
//                      allocator trait
//   LargeStringAlloc - sizeof(bdem_ChoiceImp)-byte object with
//                      allocator trait

struct NoBdemaAllocTrait { };

template <int FOOTPRINT, typename ALLOC_TRAIT = NoBdemaAllocTrait>
class FixedString {

    // Count constructor and destructor calls
    static int d_constructorCount;
    static int d_destructorCount;

    // Calculate buffer size so that total size equals desired footprint.
    enum { BUFSIZE = FOOTPRINT - sizeof(bslma_Allocator*) };

    bslma_Allocator *d_alloc;
    char             d_buffer[BUFSIZE];

  public:
    BSLALG_DECLARE_NESTED_TRAITS2(FixedString, ALLOC_TRAIT,
                                  bslalg_TypeTraitBitwiseMoveable);

    static int constructorCount();
    static int destructorCount();
    static int instanceCount();
    static int maxLength();
    static int maxSupportedBdexVersion();

    FixedString(bslma_Allocator *alloc = 0);
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
};

template <int FOOTPRINT, typename ALLOC_TRAIT>
bool operator==(const FixedString<FOOTPRINT, ALLOC_TRAIT>& a,
                const FixedString<FOOTPRINT, ALLOC_TRAIT>& b) {
    return 0 == bsl::strcmp(a.c_str(), b.c_str());
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
inline
bool operator!=(const FixedString<FOOTPRINT, ALLOC_TRAIT>& a,
                const FixedString<FOOTPRINT, ALLOC_TRAIT>& b) {
    return ! (a == b);
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
bsl::ostream& operator<<(bsl::ostream&                              os,
                         const FixedString<FOOTPRINT, ALLOC_TRAIT>& s)
{
    return os << s.c_str();
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
int FixedString<FOOTPRINT, ALLOC_TRAIT>::d_constructorCount = 0;

template <int FOOTPRINT, typename ALLOC_TRAIT>
int FixedString<FOOTPRINT, ALLOC_TRAIT>::d_destructorCount = 0;

template <int FOOTPRINT, typename ALLOC_TRAIT>
inline
int FixedString<FOOTPRINT, ALLOC_TRAIT>::constructorCount() {
    return d_constructorCount;
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
inline
int FixedString<FOOTPRINT, ALLOC_TRAIT>::destructorCount() {
    return d_destructorCount;
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
inline
int FixedString<FOOTPRINT, ALLOC_TRAIT>::instanceCount() {
    return d_constructorCount - d_destructorCount;
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
inline
int FixedString<FOOTPRINT, ALLOC_TRAIT>::maxLength() {
    return BUFSIZE - 1;
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
inline
int FixedString<FOOTPRINT, ALLOC_TRAIT>::maxSupportedBdexVersion() {
    return 1;
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
FixedString<FOOTPRINT, ALLOC_TRAIT>::FixedString(bslma_Allocator *alloc)
    : d_alloc(alloc) {
    bsl::memset(d_buffer, 0, BUFSIZE);
    ++d_constructorCount;
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
FixedString<FOOTPRINT, ALLOC_TRAIT>::FixedString(const char      *s,
                                                 bslma_Allocator *alloc)
    : d_alloc(alloc) {
    bsl::strncpy(d_buffer, s, BUFSIZE - 1);
    d_buffer[BUFSIZE - 1] = '\0';
    ++d_constructorCount;
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
FixedString<FOOTPRINT, ALLOC_TRAIT>::FixedString(const FixedString&  original,
                                                 bslma_Allocator    *alloc)
    : d_alloc(alloc) {
    bsl::memcpy(d_buffer, original.d_buffer, BUFSIZE);
    ++d_constructorCount;
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
FixedString<FOOTPRINT, ALLOC_TRAIT>::~FixedString() {
    ++d_destructorCount;
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
FixedString<FOOTPRINT, ALLOC_TRAIT>&
FixedString<FOOTPRINT, ALLOC_TRAIT>::operator=(const FixedString& rhs) {
    bsl::memcpy(d_buffer, rhs.d_buffer, BUFSIZE);
    return *this;
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
template <class STREAM>
STREAM&
FixedString<FOOTPRINT, ALLOC_TRAIT>::bdexStreamIn(STREAM& stream, int version){
    stream.getArrayInt8(d_buffer, BUFSIZE);
    return stream;
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
template <class STREAM>
STREAM&
FixedString<FOOTPRINT, ALLOC_TRAIT>::bdexStreamOut(STREAM& stream,
                                                   int     version) const {
    stream.putArrayInt8(d_buffer, BUFSIZE);
    return stream;
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
bslma_Allocator *FixedString<FOOTPRINT, ALLOC_TRAIT>::get_allocator() const {
    return d_alloc;
}

template <int FOOTPRINT, typename ALLOC_TRAIT>
const char *FixedString<FOOTPRINT, ALLOC_TRAIT>::c_str() const {
    return d_buffer;
}

const int SFP = 32;                     // Small FOOTPRINT
const int LFP = sizeof(bdem_ChoiceImp); // Large FOOTPRINT
typedef FixedString<SFP>                                     SmallString;
typedef FixedString<SFP, bslalg_TypeTraitUsesBslmaAllocator> SmallStringAlloc;
typedef FixedString<LFP>                                     LargeString;
typedef FixedString<LFP, bslalg_TypeTraitUsesBslmaAllocator> LargeStringAlloc;

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

} // Close namespace BloombergLP

namespace my_ElemTypes {
    enum ElemType {
        MY_SMALL_STRING,
        MY_SMALL_STRING_ALLOC,
        MY_LARGE_STRING,
        MY_LARGE_STRING_ALLOC,
        MY_NUM_TYPES
    };
}

typedef bsl::vector<my_ElemTypes::ElemType>             TypesCatalog;

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

const bdem_Descriptor *const
my_descriptorLookupTable[my_ElemTypes::MY_NUM_TYPES] = {
    &my_SmallStringDescriptor,
    &my_SmallStringAllocDescriptor,
    &my_LargeStringDescriptor,
    &my_LargeStringAllocDescriptor
};

// Sample data values
bslma_TestAllocator sampleAlloc;

// SmallString values must be under 23 characters long.
const SmallString SS_UNSET(&sampleAlloc);
const SmallString SS_1("Bye world!", &sampleAlloc);
const SmallString SS_2("Nice job.", &sampleAlloc);

const SmallStringAlloc SSA_UNSET(&sampleAlloc);
const SmallStringAlloc SSA_1("A penny", &sampleAlloc);
const SmallStringAlloc SSA_2("earned.", &sampleAlloc);

const LargeString LS_UNSET(&sampleAlloc);
const LargeString LS_1("Early to bed and early to rise...", &sampleAlloc);
const LargeString LS_2("makes a man health wealthy and wise.", &sampleAlloc);

const LargeStringAlloc LSA_UNSET(&sampleAlloc);
const LargeStringAlloc LSA_1("Never give a sucker an even break",&sampleAlloc);
const LargeStringAlloc LSA_2("Live fast, die young, and leave a "
                             "good looking corpse.", &sampleAlloc);

static char SPECIFICATIONS[] = "ABCD";
    // This string stores the valid specification values used for constructing
    // a TypesCatalog.  Each character specifies an element type 'A' -
    // 'SmallString', 'B' - 'SmallStringAlloc', 'C' - 'LargeString' and 'D' -
    // 'LargeStringAlloc'.
const int SPEC_LEN = sizeof SPECIFICATIONS - 1;

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
const int NUM_STREAMIN_DESCS =
                           sizeof STREAMIN_DESCS / sizeof *STREAMIN_DESCS;

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

static const AggOption::AllocationStrategy PASSTH =
                                                  AggOption::BDEM_PASS_THROUGH;

                        // =========================
                        // struct streamInAttrLookup
                        // =========================

template <class STREAM>
struct streamInAttrLookup
{
    static const bdem_DescriptorStreamIn<STREAM>* lookupTable();
};

                        // ==========================
                        // struct streamOutAttrLookup
                        // ==========================

template <class STREAM>
struct streamOutAttrLookup
{
    static const bdem_DescriptorStreamOut<STREAM>* lookupTable();
};

                        // ===========================
                        // Dummy streaming descriptors
                        // ===========================

template <class STREAM>
const bdem_DescriptorStreamIn<STREAM>*
streamInAttrLookup<STREAM>::lookupTable()
{
    static const bdem_DescriptorStreamIn<STREAM>
        table[] =
    {
        { &bdem_FunctionTemplates::streamInFundamental<SmallString,STREAM> },
        { &bdem_FunctionTemplates::streamInFundamental<SmallStringAlloc,
                                                       STREAM>             },
        { &bdem_FunctionTemplates::streamInFundamental<LargeString,STREAM> },
        { &bdem_FunctionTemplates::streamInFundamental<LargeStringAlloc,
                                                       STREAM>             },

    };

    return table;
}

template <class STREAM>
const bdem_DescriptorStreamOut<STREAM>*
streamOutAttrLookup<STREAM>::lookupTable()
{
    static const bdem_DescriptorStreamOut<STREAM>
        table[] =
    {
        { &bdem_FunctionTemplates::streamOutFundamental<SmallString,STREAM> },
        { &bdem_FunctionTemplates::streamOutFundamental<SmallStringAlloc,
                                                        STREAM> },
        { &bdem_FunctionTemplates::streamOutFundamental<LargeString,STREAM> },
        { &bdem_FunctionTemplates::streamOutFundamental<LargeStringAlloc,
                                                        STREAM> },
    };

    return table;
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static const Desc *getDescriptor(char spec)
    // Return the non-modifiable descriptor corresponding to the specified
    // 'spec' value.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCD".
{
    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return DESCRIPTORS[index];
}

static const my_ElemTypes::ElemType getElemType(char spec)
    // Return the element type corresponding to the specified
    // 'spec' value.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCD".
{

    switch (spec) {
      case 'A': return my_ElemTypes::MY_SMALL_STRING;
      case 'B': return my_ElemTypes::MY_SMALL_STRING_ALLOC;
      case 'C': return my_ElemTypes::MY_LARGE_STRING;
      case 'D': return my_ElemTypes::MY_LARGE_STRING_ALLOC;
      default: ASSERT(0); return my_ElemTypes::MY_SMALL_STRING;
    }
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

static void populateTypesCatalog(TypesCatalog *catalog, const char *spec)
    // Populate the specified types 'catalog' according to the specified
    // 'spec' string.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCD".
{
    ASSERT(catalog);
    ASSERT(spec);

    catalog->clear();
    for (const char *s = spec; *s; ++s) {
        if (' ' == *s || '\t' == *s || '\n' == *s) continue; // ignore WS
        catalog->push_back(getElemType(*s));
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

static bool compare(const void *p, const void *q, char spec)
{
    switch (spec) {
      case 'A': return *(SmallString *) p == *(SmallString *) q;
      case 'B': return *(SmallStringAlloc *) p == *(SmallStringAlloc *) q;
      case 'C': return *(LargeString *) p == *(LargeString *) q;
      case 'D': return *(LargeStringAlloc *) p == *(LargeStringAlloc *) q;
      default: ASSERT(0); return false;
    }
}

const int STREAM_VERSIONS[] = { 1 };
const int NUM_STREAM_VERSIONS = sizeof STREAM_VERSIONS
                              / sizeof *STREAM_VERSIONS;

//=============================================================================
//                  FUNCTION FOR TESTING USAGE EXAMPLES
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
// The 'divide' function reads two 'double' values, divides the first by the
// second, and returns a choice imp representing the result of the division.
// The signature of the function looks as follows:
//..
bdem_ChoiceImp divide(double dividend, double divisor) {
//..
// The division normally will result in a double value, but will sometimes
// result in an error string.  The latter case can occur because the division
// itself failed because the divisor was zero.  The result of the division is
// therefore packaged in a 'bdem_ChoiceImp' which can store either a 'double'
// or a 'string'.  The current selection is indexed by a value of 0 or 1 for
// the double or string for division error.  We specify the types catalog for
// our choice imp object below:
//..
     static const bdem_ElemType::Type CHOICE_TYPES[] = {
         bdem_ElemType::BDEM_DOUBLE,  // result value
         bdem_ElemType::BDEM_STRING   // division error
     };
     static const int NUM_CHOICES = sizeof CHOICE_TYPES / sizeof *CHOICE_TYPES;
//..
// Now we will construct our choice imp object providing the types catalog,
// and a descriptor array that was previously constructed.
//..
     enum { RESULT_VAL, DIVISION_ERROR };

     bdem_ChoiceImp result(CHOICE_TYPES,
                           NUM_CHOICES,
                           MY_DESCRIPTORS,
                           bdem_AggregateOption::BDEM_PASS_THROUGH);
     ASSERT(-1 == result.selector());

     if (0 == divisor) {
//..
// If the division fails, the choice imp is set to 'DIVISION_ERROR' and the
// corresponding selection is set to an error string.  Note the alternate
// syntax (selection().theString()) for setting and getting the current
// element.
//..
         bsl::string errString = "Divided by zero.";
         result.makeSelection(DIVISION_ERROR);
         result.selection().theModifiableString() = errString;
         ASSERT(DIVISION_ERROR == result.selector());
         ASSERT(errString == result.selection().theString());
     }
     else {
//..
// If there are no errors, compute the quotient and store it as a 'double'
// with selector, 'RESULT_VAL'.
//..
         const double quotient = dividend / divisor;
         result.makeSelection(RESULT_VAL).theModifiableDouble() = quotient;
         ASSERT(RESULT_VAL == result.selector());
         ASSERT(quotient == result.selection().theDouble());
     }
//..
// Once the result is calculated, it is returned from the function.
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

    bslma_TestAllocator         defaultAlloc;
    bslma_DefaultAllocatorGuard allocGuard(&defaultAlloc);

    switch (test) { case 0:  // Zero is always the leading case.
      case 14: {
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

         bsl::string errString = "Divided by zero.";

         static const bdem_ElemType::Type CHOICE_TYPES[] = {
             bdem_ElemType::BDEM_DOUBLE,  // result value
             bdem_ElemType::BDEM_STRING   // division error
         };
         static const int NUM_CHOICES =
                                    sizeof CHOICE_TYPES / sizeof *CHOICE_TYPES;

         enum { RESULT_VAL, DIVISION_ERROR };

        // Try both scenarios
        // Scenario 1
        {
            double VALUE1 = 10.25, VALUE2 = 0;
            Obj mX = divide(VALUE1, VALUE2); const Obj& X = mX;
            ASSERT(NUM_CHOICES    == X.numSelections());
            ASSERT(DIVISION_ERROR == X.selector());
            ASSERT(errString      == X.selection().theString());
        }

        // Scenario 2
        {
            double VALUE1 = 10.5, VALUE2 = 5.25, RESULT = VALUE1 / VALUE2;
            Obj mX = divide(VALUE1, VALUE2); const Obj& X = mX;
            ASSERT(NUM_CHOICES == X.numSelections());
            ASSERT(RESULT_VAL  == X.selector());
            ASSERT(RESULT      == X.selection().theDouble());
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'reset' and 'clear' FUNCTIONS:
        //
        // Concerns:
        //   An object that has been reset has state identical to its state
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
        //   void reset(const bdem_ElemType::Type    selectionTypes[],
        //       int                          numSelections,
        //       const bdem_Descriptor *const attrLookup[]);
        //   void clear();
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting reset Function"
                               << "\n======================" << bsl::endl;

        {
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

          const Desc **D = DESCRIPTORS;

          bslma_TestAllocator testAllocator(veryVeryVerbose);
          bslma_TestAllocator &tAlloc = testAllocator;
          //iterate U over catalogs in S
          for (int i = 0; i < NUM_DATA; ++i) {
              const char *SPEC  = DATA[i].d_catalogSpec;

              if (veryVerbose) { P(SPEC); }

              TypesCatalog cat;
              populateTypesCatalog(&cat,SPEC);
              my_ElemTypes::ElemType *catSt =
                                          (cat.size()>0) ? &cat.front() : NULL;

              {
                  //create and populate mX1 and mX2
                  Obj mX1((bdem_ElemType::Type*) catSt, cat.size(),
                          my_descriptorLookupTable, PASSTH, &tAlloc);
                  const Obj &X1 = mX1;

                  Obj mX2(mX1, PASSTH, &tAlloc); const Obj &X2 = mX2;
                  Obj mX3(mX1, PASSTH, &tAlloc); const Obj &X3 = mX3;

                  //create the control arrays
                  Obj mY; const Obj &Y = mY;
                  Obj mZ((bdem_ElemType::Type*)catSt, cat.size(),
                         my_descriptorLookupTable, PASSTH, &tAlloc);
                  const Obj &Z = mZ;

                  //reset
                  mX1.reset();
                  mX2.clear();
                  mX3.reset((bdem_ElemType::Type*)catSt, cat.size(),
                            my_descriptorLookupTable);

                  //compare
                  LOOP_ASSERT(i, X1 == Y);
                  LOOP_ASSERT(i, X2 == Y);
                  LOOP_ASSERT(i, X3 == Z);
              }
              {
                  // create and populate 'mX1' and 'mX2'
                  Obj mX1((bdem_ElemType::Type*)catSt, cat.size(),
                          my_descriptorLookupTable, PASSTH, &tAlloc);
                  const Obj &X1 = mX1;
                  LOOP_ASSERT(i, !X1.selection().isBound());

                  Obj mX2(mX1, PASSTH, &tAlloc); const Obj &X2 = mX2;
                  Obj mX3(mX1, PASSTH, &tAlloc); const Obj &X3 = mX3;
                  LOOP_ASSERT(i, !X2.selection().isBound());
                  LOOP_ASSERT(i, !X3.selection().isBound());

                  // create the control arrays
                  Obj mY; const Obj &Y = mY;
                  Obj mZ((bdem_ElemType::Type*)catSt,cat.size(),
                         my_descriptorLookupTable,PASSTH,&tAlloc);
                  const Obj &Z = mZ;

                  // reset
                  mX1.reset();
                  mX2.clear();
                  mX3.reset((bdem_ElemType::Type*)catSt,cat.size(),
                            my_descriptorLookupTable);

                  // compare
                  LOOP_ASSERT(i, X1 == Y);
                  LOOP_ASSERT(i, X2 == Y);
                  LOOP_ASSERT(i, X3 == Z);

                  LOOP_ASSERT(i, !X1.selection().isBound());
                  LOOP_ASSERT(i, !X2.selection().isBound());
                  LOOP_ASSERT(i, !X3.selection().isBound());
              }
          }
        }
      } break;
      case 12: {
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

        typedef bdem_ChoiceImp Obj;

        ASSERT((0 == bslmf_IsConvertible<bslma_Allocator*, Obj>::VALUE));
        ASSERT((1 ==
             bslalg_HasTrait<Obj, bslalg_TypeTraitUsesBslmaAllocator>::VALUE));

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'reset' and 'clear' FUNCTIONS:
        //
        // Concerns:
        //   An object that has been reset has state identical to its state
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
        //   void reset(const bdem_ElemType::Type    selectionTypes[],
        //       int                          numSelections,
        //       const bdem_Descriptor *const attrLookup[]);
        //   void clear();
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting reset Function"
                               << "\n======================" << bsl::endl;

        {
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

          const Desc **D = DESCRIPTORS;

          bslma_TestAllocator testAllocator(veryVeryVerbose);
          bslma_TestAllocator &tAlloc = testAllocator;
          //iterate U over catalogs in S
          for (int i = 0; i < NUM_DATA; ++i) {
              const char *SPEC  = DATA[i].d_catalogSpec;

              if (veryVerbose) { P(SPEC); }

              TypesCatalog cat;
              populateTypesCatalog(&cat,SPEC);
              my_ElemTypes::ElemType *catSt =
                                          (cat.size()>0) ? &cat.front() : NULL;

              {
                  // create and populate mX1 and mX2
                  Obj mX1((bdem_ElemType::Type*) catSt, cat.size(),
                          my_descriptorLookupTable, PASSTH, &tAlloc);
                  const Obj &X1 = mX1;

                  Obj mX2(mX1, PASSTH, &tAlloc); const Obj &X2 = mX2;
                  Obj mX3(mX1, PASSTH, &tAlloc); const Obj &X3 = mX3;

                  // create the control arrays
                  Obj mY; const Obj &Y = mY;
                  Obj mZ((bdem_ElemType::Type*)catSt, cat.size(),
                         my_descriptorLookupTable, PASSTH, &tAlloc);
                  const Obj &Z = mZ;

                  // reset
                  mX1.reset();
                  mX2.clear();
                  mX3.reset((bdem_ElemType::Type*)catSt, cat.size(),
                            my_descriptorLookupTable);

                  // compare
                  LOOP_ASSERT(i, X1 == Y);
                  LOOP_ASSERT(i, X2 == Y);
                  LOOP_ASSERT(i, X3 == Z);
              }
              {
                  // create and populate mX1 and mX2
                  Obj mX1((bdem_ElemType::Type*)catSt, cat.size(),
                          my_descriptorLookupTable, PASSTH, &tAlloc);
                  const Obj &X1 = mX1;
                  LOOP_ASSERT(i, !X1.selection().isBound());

                  Obj mX2(mX1, PASSTH, &tAlloc); const Obj &X2 = mX2;
                  Obj mX3(mX1, PASSTH, &tAlloc); const Obj &X3 = mX3;
                  LOOP_ASSERT(i, !X2.selection().isBound());
                  LOOP_ASSERT(i, !X3.selection().isBound());

                  // create the control arrays
                  Obj mY; const Obj &Y = mY;
                  Obj mZ((bdem_ElemType::Type*)catSt,cat.size(),
                         my_descriptorLookupTable,PASSTH,&tAlloc);
                  const Obj &Z = mZ;

                  // reset
                  mX1.reset();
                  mX2.clear();
                  mX3.reset((bdem_ElemType::Type*)catSt,cat.size(),
                            my_descriptorLookupTable);

                  // compare
                  LOOP_ASSERT(i, X1 == Y);
                  LOOP_ASSERT(i, X2 == Y);
                  LOOP_ASSERT(i, X3 == Z);

                  LOOP_ASSERT(i, !X1.selection().isBound());
                  LOOP_ASSERT(i, !X2.selection().isBound());
                  LOOP_ASSERT(i, !X3.selection().isBound());
              }
          }
        }
      } break;
      case 10:  {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Concerns:
        //   - That the specified or bslma_Default::defaultAllocator is used.
        //   - That the specified or default (BDEM_PASS_THROUGH) strategy is
        //     used.
        //   - That the value is initialized properly.
        //   - That the destructor cleans up properly.
        //
        // Plan:
        //   1. Create a default allocator guard.  Create a test choice imp
        //      using our default guard, create a test choice imp using a
        //      allocator we explicitly specify.  Inspect the allocators to
        //      make sure when that when the objects are created memory is
        //      allocated from the correct allocator.
        //  2. Create a set of choice imp objects,A1-4, using the 4
        //     constructors under test and the BDEM_PASS_THROUGH allocation
        //     strategy,create a second set of choice imp objects, B, using
        //     the BDEM_WRITE_ONCE alloc strategy.  Add and remove elements
        //     from the objects.  Ensure objects using the same catalog
        //     allocated with BDEM_PASS_THROUGH have allocated an equal amount
        //     of memory.  Ensure that the memory allocated by
        //     BDEM_PASS_THROUGH arrays is less than the memory allocated by
        //     objects using BDEM_WRITE_ONCE.
        //
        // Testing:
        //     bdem_ChoiceImp(
        //       bslma_Allocator                          *basicAllocator = 0);
        //
        //     bdem_ChoiceImp(
        //       bdem_AggregateOption::AllocationStrategy  allocMode,
        //       bslma_Allocator                          *basicAllocator = 0);
        //
        //     bdem_ChoiceImp(const bdem_ElemType::Type      selectionTypes[],
        //          int                                      numSelectionTypes,
        //          const bdem_Descriptor *const             attrLookupTbl[],
        //          bdem_AggregateOption::AllocationStrategy allocMode,
        //          bslma_Allocator                         *basicAllocator=0);
        //
        //     bdem_ChoiceImp(
        //       const bdem_ChoiceHeader&                  choiceHeader,
        //       bdem_AggregateOption::AllocationStrategy  allocMode,
        //       bslma_Allocator                          *basicAllocator = 0);
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Constructors"
                               << "\n====================" << bsl::endl;

        {

           if (veryVerbose) bsl::cout << "Verify the allocator is passed "
                                       << "correctly" << bsl::endl;

            //a simple catalog to work with
            TypesCatalog cat;
            populateTypesCatalog(&cat,SPECIFICATIONS);
            my_ElemTypes::ElemType *catSt = (cat.size()>0) ? &cat.front()
            : NULL;

            bslma_TestAllocator alloc1,alloc2;

            //Set a default alloc guard

            const bslma_DefaultAllocatorGuard dag1(&alloc1);

            ASSERT(0 == alloc1.numBytesInUse());
            ASSERT(0 == alloc2.numBytesInUse());

            //create choice imp objects using the default allocator
            int N=0;
            Obj a1;
            ASSERT(N <  alloc1.numBytesInUse()); N = alloc1.numBytesInUse();
            ASSERT(0 == alloc2.numBytesInUse());
            Obj a2(PASSTH);
            ASSERT(N < alloc1.numBytesInUse());  N = alloc1.numBytesInUse();
            ASSERT(0 == alloc2.numBytesInUse());

            Obj a3((bdem_ElemType::Type*)catSt,cat.size(),
               my_descriptorLookupTable, PASSTH);
            ASSERT(N < alloc1.numBytesInUse());  N = alloc1.numBytesInUse();
            ASSERT(0 == alloc2.numBytesInUse());

            Catalog catalog(&alloc1);
            populateCatalog(&catalog, SPECIFICATIONS);

            bdem_ChoiceHeader ch1(&catalog);
            const bdem_ChoiceHeader &C1 = ch1;

            Obj a4(C1, PASSTH);
            ASSERT(N < alloc1.numBytesInUse());  N = alloc1.numBytesInUse();
            ASSERT(0 == alloc2.numBytesInUse());

            //create choice imp objects using alloc 2
            int M=0;
            Obj b1(&alloc2);
            ASSERT(N == alloc1.numBytesInUse());
            ASSERT(M < alloc2.numBytesInUse());  M = alloc2.numBytesInUse();

            Obj b2(PASSTH,&alloc2);
            ASSERT(N == alloc1.numBytesInUse());
            ASSERT(M <  alloc2.numBytesInUse()); M = alloc2.numBytesInUse();

            Obj b3((bdem_ElemType::Type*)catSt,cat.size(),
              my_descriptorLookupTable, PASSTH ,&alloc2);
            ASSERT(N == alloc1.numBytesInUse());
            ASSERT(M < alloc2.numBytesInUse());  M = alloc2.numBytesInUse();

            Catalog catalog2(&alloc2);
            populateCatalog(&catalog2, SPECIFICATIONS);

            bdem_ChoiceHeader ch2(&catalog);
            const bdem_ChoiceHeader &C2 = ch2;

            Obj b4(C2, PASSTH ,&alloc2);
            ASSERT(N == alloc1.numBytesInUse());
            ASSERT(M < alloc2.numBytesInUse());  M = alloc2.numBytesInUse();

        }

        {
            if (veryVerbose) bsl::cout << "Verify the allocation mode is "
                                       << "passed correctly" << bsl::endl;

            //This test checks if a the allocation mode parameter is
            //passed to the delegating class.  It does not attempt to verify
            //that all the allocation modes behave correctly.

            //a simple catalog to work with
            TypesCatalog cat;
            populateTypesCatalog(&cat,SPECIFICATIONS);
            my_ElemTypes::ElemType *catSt = (cat.size()>0) ? &cat.front()
            : NULL;

            bslma_TestAllocator alloc1,alloc2,alloc3,alloc4,alloc5;
            bslma_TestAllocator alloc2B,alloc3B, badAlloc;

            // Set a default alloc guard
            const bslma_DefaultAllocatorGuard dag1(&badAlloc);

            // create a choice imp object using the BDEM_PASS_THROUGH mode
            Obj a1(&alloc1);
            Obj a2(PASSTH,&alloc2);
            Obj a3((bdem_ElemType::Type*)catSt,cat.size(),
               my_descriptorLookupTable,PASSTH,&alloc3);

            Catalog catalog(&alloc5);
            populateCatalog(&catalog, SPECIFICATIONS);

            bdem_ChoiceHeader ch1(&catalog);
            const bdem_ChoiceHeader &C1 = ch1;
            Obj a4(C1,PASSTH, &alloc4);

            // create a choice imp object using BDEM_WRITE_ONCE mode
            Obj b2(AggOption::BDEM_WRITE_ONCE,&alloc2B);
            Obj b3((bdem_ElemType::Type*)catSt,cat.size(),
               my_descriptorLookupTable, AggOption::BDEM_WRITE_ONCE,&alloc3B);

            ASSERT(0 < alloc1.numBytesInUse());
            ASSERT(0 < alloc2.numBytesInUse());
            ASSERT(0 < alloc3.numBytesInUse());
            ASSERT(0 < alloc4.numBytesInUse());
            ASSERT(0 < alloc2B.numBytesInUse());
            ASSERT(0 < alloc3B.numBytesInUse());
            ASSERT(0 == badAlloc.numBytesInUse());

            a1.addSelection(&my_SmallStringDescriptor);
            *(SmallString*)a1.makeSelection(0).data() = SS_1;

            a2.addSelection(&my_SmallStringDescriptor);
            *(SmallString*)a2.makeSelection(0).data() = SS_1;

            a3.addSelection(&my_SmallStringDescriptor);
            *(SmallString*)a3.makeSelection(0).data() = SS_1;

            a4.addSelection(&my_SmallStringDescriptor);
            *(SmallString*)a4.makeSelection(0).data() = SS_1;

            b2.addSelection(&my_SmallStringDescriptor);
            *(SmallString*)b2.makeSelection(0).data() = SS_1;

            b3.addSelection(&my_SmallStringDescriptor);
            *(SmallString*)b3.makeSelection(0).data() = SS_1;

            //All the BDEM_PASS_THROUGH allocators should have used the
            //same amount of memory
            ASSERT(alloc1.numBytesInUse() == alloc2.numBytesInUse());
            ASSERT(alloc3.numBytesInUse() == alloc4.numBytesInUse());

            //the array with a BDEM_PASS_THROUGH allocator should have
            //used less memory
            ASSERT(alloc2.numBytesInUse() < alloc2B.numBytesInUse());
            ASSERT(alloc3.numBytesInUse() < alloc3B.numBytesInUse());

        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //   The 'bdex' streaming concerns for this component are standard.
        //   We thoroughly test streaming functionality relying on the
        //   available bdex stream functions 'streamOut' and 'streamIn'
        //   which forward appropriate calls to the member functions of this
        //   component.  We next step through the sequence of possible stream
        //   states (valid, empty, invalid, incomplete, and corrupted),
        //   appropriately selecting data sets as described below.  In all
        //   cases, exception neutrality is confirmed using the specially
        //   instrumented 'bdex_TestInStream' and a pair of standard macros,
        //   'BEGIN_BDEX_EXCEPTION_TEST' and 'END_BDEX_EXCEPTION_TEST', which
        //   configure the 'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   PRELIMINARY MEMBER FUNCTION TEST
        //     First test 'maxSupportedBdexVersion' explicitly, and then
        //     perform a trivial direct test of the 'outStream' and 'inStream'
        //     methods (the rest of the testing will use the stream operators).
        //
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
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate an instance of a
        //     typical valid (control) stream and verify that it can be
        //     streamed in successfully.  Then for each data field in the
        //     stream (beginning with the version number), provide one or more
        //     similar tests with that data field corrupted.  After each test,
        //     verify that the object is in some valid state after streaming,
        //     and that the input stream has gone invalid.
        //
        //   Finally, tests of the explicit wire format will be performed.
        //
        // Testing:
        //   bdexStreamOutImp(
        //       STREAM&                         stream,
        //       int                                     version,
        //       const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup) const;
        //
        //   bdexStreamInImp(
        //          STREAM&                                stream,
        //          int                                    version,
        //          const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
        //          const bdem_Descriptor                 *const attrLookup[]);
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Streaming Functionality"
                               << "\n==============================="
                               << bsl::endl;

        static const struct TestRow {
            int         d_line;
            const char *d_catalogSpec;  // Specification to create the catalog
            bool        d_originalIsNull;
            bool        d_streamedIsNull;
        } DATA[] = {
            // Line     DescriptorSpec
            // ====     ==============
            { L_,       "A",           false,    false },
            { L_,       "A",           true ,    false },
            { L_,       "A",           false,    true  },
            { L_,       "A",           true ,    true  },

            { L_,       "B",           false,    false },
            { L_,       "B",           true ,    false },
            { L_,       "B",           false,    true  },
            { L_,       "B",           true ,    true  },

            { L_,       "C",           false,    false },
            { L_,       "C",           true ,    false },
            { L_,       "C",           false,    true  },
            { L_,       "C",           true ,    true  },

            { L_,       "D",           false,    false },
            { L_,       "D",           true ,    false },
            { L_,       "D",           false,    true  },
            { L_,       "D",           true ,    true  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int v = 1; v < 4; ++v) {
            const int VERSION = v;

            for (int i1 = 0; i1 < NUM_DATA; ++i1) {
                const int   LINE1   = DATA[i1].d_line;
                const char *SPEC1   = DATA[i1].d_catalogSpec;
                const int   LEN1    = bsl::strlen(SPEC1);
                const bool  XISNULL = DATA[i1].d_originalIsNull;
                const bool  AISNULL = DATA[i1].d_streamedIsNull;

                if (veryVerbose) { P(SPEC1); }

                for (int j1 = 0; j1 < LEN1; ++j1) {
                    const char  S1   = SPEC1[j1];
                    const void *DA   = VALUES_A[S1 - 'A'];
                    const void *DB   = VALUES_B[S1 - 'A'];
                    const void *DN   = VALUES_N[S1 - 'A'];

                    TypesCatalog cat;
                    populateTypesCatalog(&cat,SPEC1);
                    my_ElemTypes::ElemType *catSt = (cat.size() > 0)
                                                  ? &cat.front()
                                                  : NULL;

                    bslma_TestAllocator alloc1(veryVeryVerbose);

                    //create and populate mX1
                    Obj mX((bdem_ElemType::Type*)catSt,cat.size(),
                           my_descriptorLookupTable, PASSTH, &alloc1);

                    const Obj& X = mX;

                    if (XISNULL) {
                        mX.makeSelection(j1);
                    }
                    else {
                        mX.makeSelection(j1, DA);
                    }
                    LOOP_ASSERT(LINE1, X.selection().isNull() == XISNULL);

                    bdex_TestOutStream os1, os2;

                    X.bdexStreamOutImp(os1, VERSION, STREAMOUT_DESCS);

                    // Stream a constructed obj to an empty obj
                    {
                        bslma_TestAllocator tmpAlloc(veryVeryVerbose);
                        Obj mA(&tmpAlloc); const Obj& A = mA;
                        LOOP_ASSERT(LINE1, X != A);

                        bdex_TestInStream is1(os1.data(), os1.length());

                        is1.setSuppressVersionCheck(1);
                        mA.bdexStreamInImp(
                          is1,
                          VERSION,
                          streamInAttrLookup<bdex_TestInStream>::lookupTable(),
                          DESCRIPTORS);

                        if (VERSION < 3) {
                            LOOP3_ASSERT(LINE1, X, A,
                                         compare(X.selectionPointer(),
                                                 A.selectionPointer(),
                                                 S1));
                            LOOP_ASSERT(LINE1, !A.selection().isNull());
                        }
                        else {
                            LOOP4_ASSERT(LINE1, VERSION, X, A, X == A);
                            LOOP_ASSERT(LINE1,
                                        A.selection().isNull() == XISNULL);
                        }
                    }

                    // Stream a constructed obj to an non-empty obj.
                    for (int i2 = 0; i2 < NUM_DATA; ++i2) {
                        const int   LINE2  = DATA[i2].d_line;
                        const char *SPEC2  = DATA[i2].d_catalogSpec;
                        const int   LEN2   = bsl::strlen(SPEC2);

                        if (veryVerbose) { P(SPEC2); }

                        for (int j2 = 0; j2 < LEN2; ++j2) {

                            bslma_TestAllocator testAllocator(veryVeryVerbose);

                            TypesCatalog cat;
                            populateTypesCatalog(&cat,SPEC1);
                            my_ElemTypes::ElemType *catSt = (cat.size()>0)
                                ? &cat.front()
                                : NULL;

                            Obj mA((bdem_ElemType::Type*)catSt,cat.size(),
                                   my_descriptorLookupTable,
                                   PASSTH,
                                   &testAllocator);
                            const Obj& A = mA;

                            if (AISNULL) {
                                mA.makeSelection(j2);
                            }
                            else {
                                mA.makeSelection(j2, DB);
                            }

                            LOOP2_ASSERT(LINE1, LINE2,
                                         A.selection().isNull() == AISNULL);

                            if (j1 != j2) {
                                LOOP2_ASSERT(LINE1, LINE2, X != A);
                            }

                            bdex_TestInStream is1(os1.data(), os1.length());

                            is1.setSuppressVersionCheck(1);
                            mA.bdexStreamInImp(
                          is1,
                          VERSION,
                          streamInAttrLookup<bdex_TestInStream>::lookupTable(),
                          DESCRIPTORS);

                            if (VERSION < 3) {
                                LOOP3_ASSERT(LINE1, X, A,
                                             compare(X.selectionPointer(),
                                                     A.selectionPointer(),
                                                     S1));
                                LOOP_ASSERT(LINE1, !A.selection().isNull());
                            }
                            else {
                                LOOP3_ASSERT(LINE1, X, A, X == A);
                                LOOP_ASSERT(LINE1,
                                            A.selection().isNull() == XISNULL);
                            }
                        }
                    }

                    // Stream from an empty and invalid stream
                    {
                        bslma_TestAllocator tmpAlloc(veryVeryVerbose);
                        Obj mA(mX, &tmpAlloc); const Obj& A = mA;
                        Obj mB(mX, &tmpAlloc); const Obj& B = mB;
                        if (AISNULL) {
                            mA.makeSelection(j1);
                            mB.makeSelection(j1);
                        }
                        else {
                            mA.makeSelection(j1, DA);
                            mB.makeSelection(j1, DA);
                        }

                        LOOP_ASSERT(LINE1, A.selection().isNull() == AISNULL);
                        LOOP_ASSERT(LINE1, B.selection().isNull() == AISNULL);

                        if (XISNULL == AISNULL) {
                            LOOP3_ASSERT(LINE1, A, X, A == X);
                            LOOP_ASSERT(LINE1, B == X);
                        }
                        else {
                            LOOP_ASSERT(LINE1, A != X);
                            LOOP_ASSERT(LINE1, B != X);
                        }

                        bdex_TestInStream is1;  // Empty stream
                        bdex_TestInStream is2;  // Empty stream

                        is1.setSuppressVersionCheck(1);
                        mA.bdexStreamInImp(
                          is1,
                          VERSION,
                          streamInAttrLookup<bdex_TestInStream>::lookupTable(),
                          DESCRIPTORS);

                        if (XISNULL == AISNULL) {
                            LOOP_ASSERT(LINE1, A == X);
                            LOOP3_ASSERT(LINE1, B, X, B == X);
                        }
                        else {
                            LOOP_ASSERT(LINE1, A != X);
                            LOOP_ASSERT(LINE1, B != X);
                        }
                        LOOP3_ASSERT(LINE1, A, B, A == B);
                        LOOP_ASSERT(LINE1, !is1);

                        bdex_TestInStream is3(os2.data(), os2.length());
                        is3.invalidate();  // Invalid stream

                        is3.setSuppressVersionCheck(1);
                        mA.bdexStreamInImp(
                          is3,
                          VERSION,
                          streamInAttrLookup<bdex_TestInStream>::lookupTable(),
                          DESCRIPTORS);

                        if (XISNULL == AISNULL) {
                            LOOP_ASSERT(LINE1, A == X);
                            LOOP_ASSERT(LINE1, B == X);
                        }
                        else {
                            LOOP_ASSERT(LINE1, A != X);
                            LOOP_ASSERT(LINE1, B != X);
                        }
                        LOOP_ASSERT(LINE1, A == B);
                        LOOP_ASSERT(LINE1, !is3);
                    }

                    // Stream an empty obj to a constructed obj
                    {
                        bslma_TestAllocator tmpAlloc(veryVeryVerbose);
                        Obj mY(mX, &tmpAlloc); const Obj& Y = mY;
                        Obj mA(&tmpAlloc); const Obj& A = mA;
                        if (AISNULL) {
                            mY.makeSelection(j1);
                        }
                        else {
                            mY.makeSelection(j1, DA);
                        }

                        if (veryVerbose) { P(X) P(Y) P(A) }
                        LOOP_ASSERT(LINE1, Y.selection().isNull() == AISNULL);

                        if (XISNULL == AISNULL) {
                            LOOP_ASSERT(LINE1, Y == X);
                        }
                        else {
                            LOOP_ASSERT(LINE1, Y != X);
                        }
                        LOOP_ASSERT(LINE1, X != A);

                        bdex_TestOutStream os1;
                        bdex_TestOutStream os2;

                        A.bdexStreamOutImp(os1, VERSION, STREAMOUT_DESCS);

                        bdex_TestInStream is1(os1.data(), os1.length());

                        is1.setSuppressVersionCheck(1);
                        mX.bdexStreamInImp(
                          is1,
                          VERSION,
                          streamInAttrLookup<bdex_TestInStream>::lookupTable(),
                          DESCRIPTORS);

                        if (veryVerbose) { P(X) P(Y) P(A) }
                        LOOP3_ASSERT(LINE1, X, A, X == A);
                        LOOP_ASSERT(LINE1, Y != A);
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
        //   bdem_ChoiceImp& operator=(const bdem_ChoiceImp& rhs);
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

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_line;
            const char *SPEC  = DATA[i].d_catalogSpec;
            const int   LEN   = bsl::strlen(SPEC);

            bslma_TestAllocator alloc(veryVeryVerbose);

            Catalog catalog(&alloc);
            populateCatalog(&catalog, SPEC);

            bdem_ChoiceHeader ch1(&catalog);
            const bdem_ChoiceHeader &C1 = ch1;

            Obj mX(C1, PASSTH); const Obj& X = mX;
            Obj mY(C1, PASSTH); const Obj& Y = mY;
            Obj mZ(C1, PASSTH); const Obj& Z = mZ;

            for (int j1 = 0; j1 < LEN; ++j1) {
                const char S = SPEC[j1];

                mX.makeSelection(j1);
                mY.makeSelection(j1);
                mZ.makeSelection(j1);
                mZ.selection().makeNull();

                LOOP_ASSERT(LINE, X == Y);
                LOOP_ASSERT(LINE, X == Z);

                for (int j2 = 0; j2 < LEN; ++j2) {
                    const char S = SPEC[j2];

                    Obj mA(C1,PASSTH); const Obj& A = mA;
                    Obj mB(C1,PASSTH); const Obj& B = mB;
                    mA.makeSelection(j2);
                    mB.makeSelection(j2);
                    mB.selection().makeNull();

                    mA = X;
                    LOOP_ASSERT(LINE, X == A);
                    LOOP_ASSERT(LINE, X == Y);

                    mB = X;
                    LOOP_ASSERT(LINE, X == B);
                    LOOP_ASSERT(LINE, X == Y);

                    mA = Z;
                    LOOP_ASSERT(LINE, Z == A);
                    LOOP_ASSERT(LINE, Z == X);

                    mB = Z;
                    LOOP_ASSERT(LINE, Z == B);
                    LOOP_ASSERT(LINE, Z == X);
                }

                // Test Self-Assignment
                {
                    Obj mZ(C1, PASSTH); const Obj& Z = mZ;
                    mZ.makeSelection(j1);
                    LOOP_ASSERT(LINE, X == Z);

                    mZ = Z;
                    LOOP_ASSERT(LINE, X == Z);
                    LOOP_ASSERT(LINE, Z == Z);

                    mZ.selection().makeNull();
                    mZ = Z;
                    LOOP_ASSERT(LINE, X == Z);
                    LOOP_ASSERT(LINE, Z == Z);
                }
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //   - That the specified or bslma_Default::defaultAllocator is used.
        //   - That the specified or default (BDEM_PASS_THROUGH) strategy is
        //     used.
        //   - Any value must be able to be copy constructed without affecting
        //     its argument.  For values that require dynamic allocation, each
        //     copy must allocate storage independently from its own allocator.
        //
        // Plan:
        //  1. Create a default allocator guard.  Create a test array using
        //     our default guard, create a test array using a allocator we
        //     explicitly specify.  Inspect the allocators to make sure when
        //     that when the objects are created memory is allocated from
        //     the correct allocator.
        //  2. Create choice imp objects A1 & A2,using the 2 constructors
        //     under test and the BDEM_PASS_THROUGH allocation strategy, create
        //     a second set of arrays, B, using the BDEM_WRITE_ONCE alloc
        //     strategy.  Add and remove elements from the arrays.  Ensure
        //     objects using the same catalog allocated with BDEM_PASS_THROUGH
        //     have allocated an equal amount of memory.  Ensure that the
        //     memory allocated by BDEM_PASS_THROUGH arrays is less than the
        //     memory allocated by arrays using BDEM_WRITE_ONCE.
        //  3. Specify a set S whose elements have substantial and varied
        //     differences in value.  For each element in S, construct and
        //     initialize identically valued objects w and x, with different
        //     allocators, using tested methods.  Then copy construct an object
        //     y from x with a third allocator, and use the equality operator
        //     to assert that both x and y have the same value as w.  Modify y
        //     by changing one attribute and verify that w and x still have the
        //     same value but that y is different from both.  Repeat the entire
        //     process, but change different attributes of y in the last step.
        //
        // Testing:
        //     bdem_ChoiceImp(const bdem_ChoiceImp&  original,
        //                    bslma_Allocator       *basicAllocator = 0);
        //     bdem_ChoiceImp(
        //                 const bdem_ChoiceImp&                     original,
        //                 bdem_AggregateOption::AllocationStrategy  allocMode,
        //                 bslma_Allocator                          *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Copy Constructor"
                               << "\n========================" << bsl::endl;

        {
            if (veryVerbose) bsl::cout << "Verify the allocator is passed "
                                       << "correctly" << bsl::endl;

            bslma_TestAllocator badAlloc, da, alloc;

            //Set a default alloc guard
            Obj orig(&badAlloc);  const Obj& ORIG = orig;
            const int OB = badAlloc.numBytesInUse();

            const bslma_DefaultAllocatorGuard dag1(&da);

            ASSERT(0 == da.numBytesInUse());

            //create an choiceImp object using the default allocator
            int N = 0;
            Obj a1(ORIG);
            ASSERT(N  <  da.numBytesInUse()); N = da.numBytesInUse();
            ASSERT(0  == alloc.numBytesInUse());
            ASSERT(OB == badAlloc.numBytesInUse());

            Obj a2(ORIG, PASSTH);
            ASSERT(N  <  da.numBytesInUse()); N = da.numBytesInUse();
            ASSERT(0  == alloc.numBytesInUse());
            ASSERT(OB == badAlloc.numBytesInUse());

            int M = 0;
            //create an choiceImp object using alloc 2
            Obj b1(ORIG, &alloc);
            ASSERT(N == da.numBytesInUse());
            ASSERT(M < alloc.numBytesInUse());  M = alloc.numBytesInUse();
            ASSERT(OB  == badAlloc.numBytesInUse());

            Obj b2(ORIG,PASSTH, &alloc);
            ASSERT(N == da.numBytesInUse());
            ASSERT(M <  alloc.numBytesInUse());
            ASSERT(OB  == badAlloc.numBytesInUse());

        }

        {
            if (veryVerbose) bsl::cout << "Verify the allocation mode is "
                                       << "passed correctly" << bsl::endl;

            //This test checks if a the allocation mode parameters is
            //passed to the delegating class.  It does not attempt to verify
            //that all the allocation modes behave correctly.

            //a simple catalog to work with
            bslma_TestAllocator alloc1,alloc2;
            bslma_TestAllocator alloc2B, badAlloc1, badAlloc2;

            Obj orig(&badAlloc1); const Obj& ORIG = orig;
            const int badAllocBytes = badAlloc1.numBytesInUse();

            //Set a default alloc guard
            const bslma_DefaultAllocatorGuard dag1(&badAlloc2);

            //create an array using the BDEM_PASS_THROUGH mode
            Obj a1(ORIG,&alloc1);
            Obj a2(ORIG,PASSTH,&alloc2);

            //create an array using BDEM_WRITE_ONCE mode
            Obj b2(ORIG,AggOption::BDEM_WRITE_ONCE,&alloc2B);

            ASSERT(0 < alloc1.numBytesInUse());
            ASSERT(0 < alloc2.numBytesInUse());
            ASSERT(0 < alloc2B.numBytesInUse());
            ASSERT(badAllocBytes == badAlloc1.numBytesInUse());
            ASSERT(0 == badAlloc2.numBytesInUse());

            //All the BDEM_PASS_THROUGH allocators should have used the
            //same amount of memory
            ASSERT(alloc1.numBytesInUse() == alloc2.numBytesInUse());

            //the array with a BDEM_PASS_THROUGH allocator should have
            //used less memory
            ASSERT(alloc2.numBytesInUse() < alloc2B.numBytesInUse());
            ASSERT(badAllocBytes == badAlloc1.numBytesInUse());
            ASSERT(0 == badAlloc2.numBytesInUse());

        }

        if (veryVerbose) { bsl::cout << "Copy construct empty objects"
                                     << bsl::endl; }
        {
            bslma_TestAllocator alloc1(veryVeryVerbose), da(veryVeryVerbose);

            bslma_DefaultAllocatorGuard dag(&da);

            Obj mX(&alloc1); const Obj& X = mX;

            Obj mA(X); const Obj& A = mA;

            ASSERT(X == A);
            ASSERT(-1 == A.selector());

            for (int i = 0; i < 4; ++i) {

                AggOption::AllocationStrategy MODE =
                                              (AggOption::AllocationStrategy)i;

                bslma_TestAllocator testAllocator(veryVeryVerbose);

                bdema_SequentialAllocator seqAlloc(
                                       &bslma_NewDeleteAllocator::singleton());

                bslma_Allocator *alloc;
                if (AggOption::BDEM_SUBORDINATE == MODE) {
                    alloc = &seqAlloc;
                }
                else {
                    alloc = &testAllocator;
                }

                bslma_DefaultAllocatorGuard guard(alloc);

                Obj mA(X, MODE); const Obj& A = mA;

                ASSERT(X == A);
                ASSERT(-1 == A.selector());
            }

            if (veryVerbose) {
                            bsl::cout << "\tModify mX to hold small string "
                                      << " with value: " << SS_1
                                      << bsl::endl; }
            mX.addSelection(&my_SmallStringDescriptor);
            ASSERT(1  == X.numSelections());

            ASSERT(X != A);

            if (veryVerbose) {
                            bsl::cout << "\tVerify copy constructed object "
                                      <<  "has selection value: " << SS_1
                                      << bsl::endl; }

            Obj mB(X); const Obj& B = mB;
            ASSERT(X == B);

            *(SmallString*)mB.makeSelection(0).data() = SS_1;
            ASSERT(1 == B.numSelections());
        }

        {
            if (veryVerbose) bsl::cout << "Verify the nullablity is passed "
                                       << "correctly" << bsl::endl;

            if (veryVerbose) {
                            bsl::cout << "\tNo alloc mode" << bsl::endl; }
            {
                bslma_TestAllocator alloc1(veryVeryVerbose), da;

                bslma_DefaultAllocatorGuard dag(&da);

                Obj mX(&alloc1); const Obj& X = mX;

                Obj mA(X); const Obj& A = mA;

                ASSERT( X == A);
                ASSERT(-1 == A.selector());
                ASSERT(     !X.selection().isBound());
                ASSERT(     !A.selection().isBound());

                Obj mB(X); const Obj& B = mB;

                ASSERT( X == B);
                ASSERT(-1 == B.selector());
                ASSERT(     !X.selection().isBound());
                ASSERT(     !B.selection().isBound());
            }

            if (veryVerbose) bsl::cout << "\tWith alloc mode" << bsl::endl;
            {
                bslma_TestAllocator alloc1(veryVeryVerbose), da;

                bslma_DefaultAllocatorGuard dag(&da);

                Obj mX(&alloc1); const Obj& X = mX;

                Obj mA(mX, AggOption::BDEM_WRITE_ONCE); const Obj& A = mA;

                ASSERT( X == A);
                ASSERT(-1 == A.selector());
                ASSERT(     !X.selection().isBound());
                ASSERT(     !A.selection().isBound());

                Obj mB(X, AggOption::BDEM_WRITE_ONCE); const Obj& B = mB;

                ASSERT( X == B);
                ASSERT(-1 == B.selector());
                ASSERT(     !X.selection().isBound());
                ASSERT(     !B.selection().isBound());
            }
        }

        {
            static const struct TestRow {
                int         d_line;
                const char *d_catalogSpec;  // catalog spec
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
                                              (AggOption::AllocationStrategy)k;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE = DATA[i].d_line;
                    const char *SPEC = DATA[i].d_catalogSpec;
                    const int   LEN  = bsl::strlen(SPEC);

                    bslma_TestAllocator testAllocator(veryVeryVerbose);

                    bdema_SequentialAllocator seqAlloc(
                                       &bslma_NewDeleteAllocator::singleton());

                    bslma_Allocator *alloc;
                    if (AggOption::BDEM_SUBORDINATE == MODE) {
                        alloc = &seqAlloc;
                    }
                    else {
                        alloc = &testAllocator;
                    }

                    bslma_DefaultAllocatorGuard guard(alloc);

                    Catalog catalog;
                    populateCatalog(&catalog, SPEC);

                    bdem_ChoiceHeader ch(&catalog);
                    const bdem_ChoiceHeader &CH = ch;

                    Obj mX(CH, MODE); const Obj& X = mX;

                    for (int j = 0; j <= LEN; ++j) {
                        if (LEN == j) {
                            mX.makeSelection(-1);
                        }
                        else {
                            mX.makeSelection(j, getValueA(SPEC[j]));
                        }

                        Obj mY(X); const Obj& Y = mY;
                        Obj mZ(X, MODE); const Obj& Z = mZ;
                        LOOP_ASSERT(LINE, X == Y);
                        LOOP_ASSERT(LINE, X == Z);
                    }
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
        //   operator==(const bdem_ChoiceImp&, const bdem_ChoiceImp&);
        //   operator!=(const bdem_ChoiceImp&, const bdem_ChoiceImp&);
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

                        bdem_ChoiceHeader ch1(&catalog1);
                        const bdem_ChoiceHeader &C1 = ch1;
                        bdem_ChoiceHeader ch2(&catalog2);
                        const bdem_ChoiceHeader &C2 = ch2;

                        Obj mX(C1, PASSTH); const Obj& X = mX;
                        Obj mY(C2, PASSTH); const Obj& Y = mY;

                        mX.makeSelection(j1);
                        mY.makeSelection(j2);

                        if (i1 == i2 && j1 == j2) {
                            // This assumes that each spec is unique.

                            LOOP2_ASSERT(LINE1, LINE2,    X == Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X != Y));

                            // some perturbation
                            mX.makeSelection(-1);

                            LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X == Y));
                            LOOP4_ASSERT(LINE1, LINE2, X, Y,    X != Y);

                            mY.makeSelection(-1);

                            LOOP4_ASSERT(LINE1, LINE2, X, Y,    X == Y);
                            LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X != Y));

                            // back to original state
                            mX.makeSelection(j1);

                            LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X == Y));
                            LOOP4_ASSERT(LINE1, LINE2, X, Y,    X != Y);

                            mY.makeSelection(j2);

                            LOOP4_ASSERT(LINE1, LINE2, X, Y,    X == Y);
                            LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X != Y));

                        }
                        else {
                            LOOP2_ASSERT(LINE1, LINE2,    X != Y);
                            LOOP2_ASSERT(LINE1, LINE2,  !(X == Y));

                            // Some perturbation
                            mX.selection().makeNull();

                            LOOP4_ASSERT(LINE1, LINE2, X, Y,    X != Y);
                            LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X == Y));

                            mY.selection().makeNull();

                            LOOP4_ASSERT(LINE1, LINE2, X, Y,    X != Y);
                            LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X == Y));

                            // back to original state
                            mX.makeSelection(j1);

                            LOOP4_ASSERT(LINE1, LINE2, X, Y,    X != Y);
                            LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X == Y));

                            mY.makeSelection(j2);

                            LOOP4_ASSERT(LINE1, LINE2, X, Y,    X != Y);
                            LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X == Y));

                            mX.makeSelection(-1);

                            LOOP4_ASSERT(LINE1, LINE2, X, Y,    X != Y);
                            LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X == Y));

                            mY.makeSelection(-1);

                            if (i1 == i2) {
                                LOOP4_ASSERT(LINE1, LINE2, X, Y,    X == Y);
                                LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X != Y));
                            }
                            else {
                                LOOP4_ASSERT(LINE1, LINE2, X, Y,    X != Y);
                                LOOP4_ASSERT(LINE1, LINE2, X, Y,  !(X == Y));
                            }
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
        //   operator<<(ostream&, const bdem_ChoiceImp&);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                          << "Testing Output Functions" << bsl::endl
                          << "========================" << bsl::endl;

        if (veryVerbose) {
                      bsl::cout << "\tTesting empty object" << bsl::endl; }
        {

           const char *EXP_P1 = "    {\n"
                                 "        Selection Types: [\n"
                                 "        ]\n"
                                 "        {\n"
                                 "            VOID NULL\n"
                                 "        }\n"
                                 "    }\n";
            const char *EXP_P2 = "{\n"
                                 "        Selection Types: [\n"
                                 "        ]\n"
                                 "        {\n"
                                 "            VOID NULL\n"
                                 "        }\n"
                                 "    }\n";
            const char *EXP_P3 = "    { Selection Types: [ ] "
                                 "{ VOID NULL } }";
            const char *EXP_P4 = "{ Selection Types: [ ] { VOID NULL } }";
            const char *EXP_OP = "{ Selection Types: [ ] { VOID NULL } }";

            Catalog x1;
            populateCatalog(&x1, "");

            bdem_ChoiceHeader ch1(&x1);
            const bdem_ChoiceHeader &C1 = ch1;

            Obj mX(C1, PASSTH);
            const Obj& X = mX;

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
            bsl::cout << "\tTesting empty object (null)" << bsl::endl; }
        {

           const char *EXP_P1 = "    {\n"
                                 "        Selection Types: [\n"
                                 "        ]\n"
                                 "        {\n"
                                 "            VOID NULL\n"
                                 "        }\n"
                                 "    }\n";
            const char *EXP_P2 = "{\n"
                                 "        Selection Types: [\n"
                                 "        ]\n"
                                 "        {\n"
                                 "            VOID NULL\n"
                                 "        }\n"
                                 "    }\n";
            const char *EXP_P3 = "    { Selection Types: [ ] "
                                 "{ VOID NULL } }";
            const char *EXP_P4 =
                "{ Selection Types: [ ] { VOID NULL } }";
            const char *EXP_OP =
                "{ Selection Types: [ ] { VOID NULL } }";

            Catalog x1;
            populateCatalog(&x1, "");

            bdem_ChoiceHeader ch1(&x1);
            const bdem_ChoiceHeader &C1 = ch1;

            Obj mX(C1, PASSTH);
            const Obj& X = mX;

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
                        "        Selection Types: [\n"
                        "            CHAR\n"
                        "        ]\n"
                        "        {\n"
                        "            CHAR Bye world!\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            CHAR\n"
                        "        ]\n"
                        "        {\n"
                        "            CHAR NULL\n"
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
                        "        {\n"
                        "            CHAR Bye world!\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            CHAR\n"
                        "        ]\n"
                        "        {\n"
                        "            CHAR NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                       "    { Selection Types: [ CHAR ] { CHAR Bye world! } }",
                       "    { Selection Types: [ CHAR ] { CHAR NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ CHAR ] { CHAR Bye world! } }",
                        "{ Selection Types: [ CHAR ] { CHAR NULL } }",
                    },
                },
                "{ Selection Types: [ CHAR ] { CHAR Bye world! } }",
                "{ Selection Types: [ CHAR ] { CHAR NULL } }"
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
                        "        Selection Types: [\n"
                        "            SHORT\n"
                        "        ]\n"
                        "        {\n"
                        "            SHORT A penny\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            SHORT\n"
                        "        ]\n"
                        "        {\n"
                        "            SHORT NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        Selection Types: [\n"
                        "            SHORT\n"
                        "        ]\n"
                        "        {\n"
                        "            SHORT A penny\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            SHORT\n"
                        "        ]\n"
                        "        {\n"
                        "            SHORT NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { Selection Types: [ SHORT ] { SHORT A penny } }",
                        "    { Selection Types: [ SHORT ] { SHORT NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ SHORT ] { SHORT A penny } }",
                        "{ Selection Types: [ SHORT ] { SHORT NULL } }",
                    },
                },
                "{ Selection Types: [ SHORT ] { SHORT A penny } }",
                "{ Selection Types: [ SHORT ] { SHORT NULL } }",
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
                        "        Selection Types: [\n"
                        "            INT\n"
                        "        ]\n"
                        "        {\n"
                        "            INT Early to bed and early to rise...\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            INT\n"
                        "        ]\n"
                        "        {\n"
                        "            INT NULL\n"
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
                        "        {\n"
                        "            INT Early to bed and early to rise...\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            INT\n"
                        "        ]\n"
                        "        {\n"
                        "            INT NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { Selection Types: [ INT ] { INT Early to bed and"
                        " early to rise... } }",
                        "    { Selection Types: [ INT ] { INT NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ INT ] { INT Early to bed and "
                        "early to rise... } }",
                        "{ Selection Types: [ INT ] { INT NULL } }",
                    },
                },
                "{ Selection Types: [ INT ] { INT Early to bed and early to "
                "rise... } }",
                "{ Selection Types: [ INT ] { INT NULL } }",
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
                        "        Selection Types: [\n"
                        "            INT64\n"
                        "        ]\n"
                        "        {\n"
                        "            INT64 Never give a sucker an even break\n"
                        "        }\n"
                        "    }\n",
                        "    {\n"
                        "        Selection Types: [\n"
                        "            INT64\n"
                        "        ]\n"
                        "        {\n"
                        "            INT64 NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                       -1,
                        4,
                        "{\n"
                        "        Selection Types: [\n"
                        "            INT64\n"
                        "        ]\n"
                        "        {\n"
                        "            INT64 Never give a sucker an even break\n"
                        "        }\n"
                        "    }\n",
                        "{\n"
                        "        Selection Types: [\n"
                        "            INT64\n"
                        "        ]\n"
                        "        {\n"
                        "            INT64 NULL\n"
                        "        }\n"
                        "    }\n",
                    },
                    {
                        1,
                        -4,
                        "    { Selection Types: [ INT64 ] { INT64 Never give a"
                        " sucker an even break } }",
                        "    { Selection Types: [ INT64 ] { INT64 NULL } }",
                    },
                    {
                        -1,
                        -4,
                        "{ Selection Types: [ INT64 ] { INT64 Never give a"
                        " sucker an even break } }",
                        "{ Selection Types: [ INT64 ] { INT64 NULL } }",
                    },
                },
                "{ Selection Types: [ INT64 ] { INT64 Never give a sucker an "
                "even break } }",
                "{ Selection Types: [ INT64 ] { INT64 NULL } }",
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

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

            bdem_ChoiceHeader ch1(&x1);
            const bdem_ChoiceHeader &C1 = ch1;

            Obj mX(C1, PASSTH);
            const Obj& X = mX;

            if (getDescriptor(SPEC) == &my_SmallStringDescriptor)
            {
               mX.makeSelection(0, &SS_1);
            }

            else if (getDescriptor(SPEC) == &my_SmallStringAllocDescriptor)
            {
               SmallStringAlloc s1((char*)VALUE,&sampleAlloc);
               mX.makeSelection(0, &SSA_1);
            }

            else if (getDescriptor(SPEC) == &my_LargeStringDescriptor)
            {
               LargeString s1((char*)VALUE);
               mX.makeSelection(0, &LS_1);
            }

            else
            {
               LargeStringAlloc s1((char*)VALUE, &sampleAlloc);
               mX.makeSelection(0, &LSA_1);
            }

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

            mX.selection().makeNull();

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
        // TESTING BASIC MANIPULATORS (BOOTSTRAP) AND PRIMARY ACCESSORS:
        //
        // Concerns:
        //   We are able to create a 'bdem_ChoiceImp' object and put it into
        //   any legal state.  We are not concerned in this test about trying
        //   to move the object from one state to another.
        //
        // Plan:
        //   A 'bdem_ChoiceImp' object can be put into any state using the
        //   one-argument constructor, and 'addSelection and 'makeSelection'
        //   manipulators.  The state of the object can be determined using the
        //   primary accessors, 'selectionType', 'selector', 'selection' and
        //   'selectionPointer'.  The number of available selection can be
        //   determined using 'numSelections' and 'size' accessors.
        //
        //   Build objects containing 5 different values (including the
        //   "unset" value) of each of the types ('SmallString',
        //   'SmallStringAlloc', 'LargeString', 'LargeStringAlloc') first
        //   default-constructing each object, then using 'makeSelection'
        //   to set its selection type and value.  Using the primary accessors,
        //   verify number of selections, the type and
        //   value of each constructed object.
        //
        // Testing:
        //   bdem_ChoiceImp(bslma_Allocator *basicAlloc = 0);
        //   ~bdem_ChoiceImp();
        //   void addSelection(const bdem_Descriptor *elemAttr);
        //   int numSelections() const;
        //   int size() const;
        //   void *makeSelection(int index);
        //   void makeSelectionNonNull();
        //   bdem_ElemType::Type selectionType(int index) const;
        //   const void *selectionPointer() const;
        //   int selector() const;
        //   bdem_ConstElemRef selection() const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING BASIC MANIPULATORS AND ACCESSORS"
                               << "\n========================================"
                               << bsl::endl;

        if (veryVerbose) { bsl::cout << "\tDefault construct Obj mU"
                                     << bsl::endl; }
        bslma_TestAllocator alloc(veryVeryVerbose);

        Obj mU(&alloc); const Obj& U = mU;

        ASSERT(0  == U.numSelections());
        ASSERT(0  == U.size());
        ASSERT(-1 == U.selector());

        if (veryVerbose) { bsl::cout << "\tU:" << bsl::endl;
        U.print(bsl::cout, 1, 4); }

        if (veryVerbose) {
           bsl::cout << "\tModify mU to hold string "
                     <<  "value: " << SS_UNSET << bsl::endl; }

        mU.addSelection(&my_SmallStringDescriptor);
        ASSERT(1  == U.numSelections());
        ASSERT(1  == U.size());
        ASSERT(-1 == U.selector());

        ASSERT(my_ElemTypes::MY_SMALL_STRING == U.selectionType(0));

        mU.makeSelection(0, &SS_UNSET);
        ASSERT(1 == U.numSelections());
        ASSERT(1 == U.size());
        ASSERT(0 == U.selector());
        ASSERT(my_ElemTypes::MY_SMALL_STRING == U.selectionType(0));
        ASSERT(SS_UNSET    == *(SmallString *) U.selectionPointer());
        ASSERT(bdem_ConstElemRef(&SS_UNSET, &my_SmallStringDescriptor) ==
               U.selection());
        ASSERT(!U.selection().isNull());

        mU.selection().makeNull();
        ASSERT(1 == U.numSelections());
        ASSERT(1 == U.size());
        ASSERT(0 == U.selector());
        ASSERT(my_ElemTypes::MY_SMALL_STRING == U.selectionType(0));
        ASSERT(SS_UNSET == *(SmallString *) U.selectionPointer());
        ASSERT(bdem_ConstElemRef(&SS_UNSET, &my_SmallStringDescriptor) !=
               U.selection());

        int bits = 1;
        ASSERT(bdem_ConstElemRef(&SS_UNSET, &my_SmallStringDescriptor,
                                 &bits, 0) == U.selection());
        ASSERT(1 == U.selection().isNull());

        bits = 1;
        mU.makeSelection(0);
        ASSERT(bdem_ConstElemRef(&SS_UNSET, &my_SmallStringDescriptor,
                                 &bits, 0) == U.selection());

        if (veryVerbose) { bsl::cout << "\tU:" << bsl::endl;
        U.print(bsl::cout, 1, 4); }

        if (veryVerbose) { bsl::cout << "\tDefault construct Obj mX"
                                     << bsl::endl;
        }

        Obj mX(&alloc); const Obj& X = mX;

        ASSERT(0  == X.numSelections());
        ASSERT(0  == X.size());
        ASSERT(-1 == X.selector());

        if (veryVerbose) { bsl::cout << "\tX:" << bsl::endl;
        X.print(bsl::cout, 1, 4); }

        if (veryVerbose) {
           bsl::cout << "\tModify mX to hold string "
                     <<  "value: " << SS_1 << bsl::endl; }

        mX.addSelection(&my_SmallStringDescriptor);
        ASSERT(1  == X.numSelections());
        ASSERT(1  == X.size());
        ASSERT(-1 == X.selector());

        ASSERT(my_ElemTypes::MY_SMALL_STRING == X.selectionType(0));

        mX.makeSelection(0, &SS_1);
        ASSERT(1 == X.numSelections());
        ASSERT(1 == X.size());
        ASSERT(0 == X.selector());
        ASSERT(my_ElemTypes::MY_SMALL_STRING == X.selectionType(0));
        ASSERT(SS_1 == *(SmallString *) X.selectionPointer());
        ASSERT(bdem_ConstElemRef(&SS_1, &my_SmallStringDescriptor) ==
               X.selection());
        ASSERT(!X.selection().isNull());

        mX.selection().makeNull();
        ASSERT(1 == X.numSelections());
        ASSERT(1 == X.size());
        ASSERT(0 == X.selector());
        ASSERT(my_ElemTypes::MY_SMALL_STRING == X.selectionType(0));
        ASSERT(SS_UNSET == *(SmallString *) X.selectionPointer());
        ASSERT(bdem_ConstElemRef(&SS_1, &my_SmallStringDescriptor) !=
               X.selection());

        bits = 1;
        ASSERT(bdem_ConstElemRef(&SS_UNSET, &my_SmallStringDescriptor,
                                 &bits, 0) == X.selection());
        ASSERT(1 == X.selection().isNull());

        bits = 1;
        mX.makeSelection(0);
        ASSERT(bdem_ConstElemRef(&SS_UNSET, &my_SmallStringDescriptor,
                                 &bits, 0) == X.selection());

        if (veryVerbose) { bsl::cout << "\tX:" << bsl::endl;
        X.print(bsl::cout, 1, 4); }

        if (veryVerbose) { bsl::cout << "\tDefault construct Obj mY"
                                     << bsl::endl;
        }

        Obj mY(&alloc); const Obj& Y = mY;

        ASSERT(0  == Y.numSelections());
        ASSERT(0  == Y.size());
        ASSERT(-1 == Y.selector());

        if (veryVerbose) { bsl::cout << "\tY:" << bsl::endl;
        Y.print(bsl::cout, 1, 4); }

        if (veryVerbose) {
           bsl::cout << "\tModify mY to hold string "
                     <<  "value: " << SSA_1 << bsl::endl; }

        mY.addSelection(&my_SmallStringAllocDescriptor);
        ASSERT(1  == Y.numSelections());
        ASSERT(1  == Y.size());
        ASSERT(-1 == Y.selector());

        ASSERT(my_ElemTypes::MY_SMALL_STRING_ALLOC == Y.selectionType(0));

        mY.makeSelection(0, &SSA_1);
        ASSERT(1 == Y.numSelections());
        ASSERT(1 == Y.size());
        ASSERT(0 == Y.selector());
        ASSERT(my_ElemTypes::MY_SMALL_STRING_ALLOC == Y.selectionType(0));
        ASSERT(SSA_1 == *(SmallStringAlloc *) Y.selectionPointer());
        ASSERT(bdem_ConstElemRef(&SSA_1, &my_SmallStringAllocDescriptor) ==
           Y.selection());
        ASSERT(!Y.selection().isNull());

        mY.selection().makeNull();
        ASSERT(1 == Y.numSelections());
        ASSERT(1 == Y.size());
        ASSERT(0 == Y.selector());
        ASSERT(my_ElemTypes::MY_SMALL_STRING_ALLOC == Y.selectionType(0));
        ASSERT(SSA_UNSET == *(SmallStringAlloc *) Y.selectionPointer());
        ASSERT(bdem_ConstElemRef(&SS_1, &my_SmallStringAllocDescriptor) !=
               Y.selection());

        bits = 1;
        ASSERT(bdem_ConstElemRef(&SSA_UNSET, &my_SmallStringAllocDescriptor,
                                 &bits, 0) == Y.selection());
        ASSERT(1 == Y.selection().isNull());

        bits = 1;
        mY.makeSelection(0);
        ASSERT(bdem_ConstElemRef(&SSA_UNSET, &my_SmallStringAllocDescriptor,
                                 &bits, 0) == Y.selection());

        if (veryVerbose) { bsl::cout << "\tY:" << bsl::endl;
        Y.print(bsl::cout, 1, 4); }

        if (veryVerbose) { bsl::cout << "\tDefault construct Obj mZ"
                                     << bsl::endl;
        }

        Obj mZ(&alloc); const Obj& Z = mZ;

        ASSERT(0  == Z.numSelections());
        ASSERT(0  == Z.size());
        ASSERT(-1 == Z.selector());

        if (veryVerbose) { bsl::cout << "\tZ:" << bsl::endl;
        Z.print(bsl::cout, 1, 4); }

        if (veryVerbose) {
           bsl::cout << "\tModify mZ to hold string "
                     <<  "value: " << LS_1 << bsl::endl; }

        mZ.addSelection(&my_LargeStringDescriptor);
        ASSERT(1  == Z.numSelections());
        ASSERT(1  == Z.size());
        ASSERT(-1 == Z.selector());

        ASSERT(my_ElemTypes::MY_LARGE_STRING == Z.selectionType(0));

        mZ.makeSelection(0, &LS_1);
        ASSERT(1 == Z.numSelections());
        ASSERT(1 == Z.size());
        ASSERT(0 == Z.selector());
        ASSERT(my_ElemTypes::MY_LARGE_STRING == Z.selectionType(0));
        ASSERT(LS_1 == *(LargeString *) Z.selectionPointer());
        ASSERT(bdem_ConstElemRef(&LS_1, &my_LargeStringDescriptor) ==
               Z.selection());
        ASSERT(!Z.selection().isNull());

        mZ.selection().makeNull();
        ASSERT(1 == Z.numSelections());
        ASSERT(1 == Z.size());
        ASSERT(0 == Z.selector());
        ASSERT(my_ElemTypes::MY_LARGE_STRING == Z.selectionType(0));
        ASSERT(LS_UNSET == *(LargeString *) Z.selectionPointer());
        ASSERT(bdem_ConstElemRef(&LS_1, &my_LargeStringDescriptor) !=
               Z.selection());

        bits = 1;
        ASSERT(bdem_ConstElemRef(&LS_UNSET, &my_LargeStringDescriptor,
                                 &bits, 0) == Z.selection());
        ASSERT(1 == Z.selection().isNull());

        bits = 1;
        mZ.makeSelection(0);
        ASSERT(bdem_ConstElemRef(&LS_UNSET, &my_LargeStringDescriptor,
                                 &bits, 0) == Z.selection());

        if (veryVerbose) { bsl::cout << "\tZ:" << bsl::endl;
        Z.print(bsl::cout, 1, 4); }

        if (veryVerbose) { bsl::cout << "\tDefault construct Obj mW"
                                     << bsl::endl;
        }

        Obj mW(&alloc); const Obj& W = mW;

        ASSERT(0  == W.numSelections());
        ASSERT(0  == W.size());
        ASSERT(-1 == W.selector());

        if (veryVerbose) { bsl::cout << "\tW:" << bsl::endl;
        W.print(bsl::cout, 1, 4); }

        if (veryVerbose) {
           bsl::cout << "\tModify mW to hold string "
                     <<  "value: " << LSA_1 << bsl::endl; }

        mW.addSelection(&my_LargeStringAllocDescriptor);
        ASSERT(1  == W.numSelections());
        ASSERT(1  == W.size());
        ASSERT(-1 == W.selector());

        ASSERT(my_ElemTypes::MY_LARGE_STRING_ALLOC == W.selectionType(0));

        mW.makeSelection(0, &LSA_1);
        ASSERT(1 == W.numSelections());
        ASSERT(1 == W.size());
        ASSERT(0 == W.selector());
        ASSERT(my_ElemTypes::MY_LARGE_STRING_ALLOC == W.selectionType(0));
        ASSERT(LSA_1 == *(LargeStringAlloc *) W.selectionPointer());
        ASSERT(bdem_ConstElemRef(&LSA_1, &my_LargeStringAllocDescriptor) ==
           W.selection());
        ASSERT(!W.selection().isNull());

        mW.selection().makeNull();
        ASSERT(1 == W.numSelections());
        ASSERT(1 == W.size());
        ASSERT(0 == W.selector());
        ASSERT(my_ElemTypes::MY_LARGE_STRING_ALLOC == W.selectionType(0));
        ASSERT(LSA_UNSET == *(LargeStringAlloc *) W.selectionPointer());
        ASSERT(bdem_ConstElemRef(&LSA_1, &my_LargeStringAllocDescriptor) !=
               W.selection());

        bits = 1;
        ASSERT(bdem_ConstElemRef(&LSA_UNSET, &my_LargeStringAllocDescriptor,
                                 &bits, 0) == W.selection());
        ASSERT(1 == W.selection().isNull());

        bits = 1;
        mW.makeSelection(0);
        ASSERT(bdem_ConstElemRef(&LSA_UNSET, &my_LargeStringAllocDescriptor,
                                 &bits, 0) == W.selection());

        if (veryVerbose) { bsl::cout << "\tW:" << bsl::endl;
        W.print(bsl::cout, 1, 4); }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING helper functions
        //
        // Concerns:
        //  1. Confirm that 'populateCatalog' and 'populateTypesCatalog'
        //     functions works as expected
        //  2. Confirm that 'getValueA', 'getValueB', 'getValueN' work as
        //     expected.
        //  3. Confirm that 'getDescriptor' and 'getElemtype' work as expected.
        //
        // Plan:
        //  1. Test generation of the types catalog with various spec
        //     strings.
        //  2. Test that the getValue functions return the expected value for
        //     each value.  (Brute force testing).
        //  3. Test that the getDescriptor and getElemType return the expected
        //     value for various spec strings.
        //
        // Testing:
        //   void populateCatalog(Catalog *catalog, const char *spec);
        //   void populateTypesCatalog(TypesCatalog *catalog, const char *spec)
        //   const void *getValueA(char spec);
        //   const void *getValueB(char spec);
        //   const void *getValueN(char spec);
        //   const Desc *getDescriptor(char spec);
        //   const my_ElemTypes::ElemType getElemType(char spec);
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
            bsl::cout << "\nTesting 'populateTypes Catalog' for a different"
                         " length catalog"
                      << bsl::endl;

        for (int i = 0; i < NUM_SPECS; ++i) {
            const int   LINE = SPECS[i].d_line;
            const char *SPEC = SPECS[i].d_spec;
            const int   LEN  = bsl::strlen(SPEC);

            if (veryVerbose) { P(SPEC) }

            TypesCatalog x1(Z);  const TypesCatalog& X1 = x1;
            populateTypesCatalog(&x1, SPEC);
            LOOP_ASSERT(LINE, LEN == X1.size());

            for (int j = 0; j < LEN; ++j) {
                LOOP_ASSERT(LINE, getElemType(SPEC[j]) == X1[j]);
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
            bsl::cout << "\nTesting 'getElemType'." << bsl::endl;
        {
            for (int i = 0; i < SPEC_LEN; ++i) {
                LOOP_ASSERT(SPECIFICATIONS[i],
                            i == getElemType(SPECIFICATIONS[i]));
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
        //      'bslalg_TypeTraitUsesBslmaAllocator' trait if and only if that
        //      trait is specified as a template parameter.
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

        if (verbose) bsl::cout << "\nTESTING 'FixedString' CLASS"
                               << "\n===========================" << bsl::endl;

        const SmallString *DATA[] = { &SS_UNSET, &SS_1, &SS_2 };
        const int DATA_SIZE = sizeof DATA / sizeof *DATA;

        if (veryVerbose) bsl::cout << "Testing footprint" << bsl::endl;
        ASSERT(SFP == sizeof(SmallString));
        ASSERT(SFP == sizeof(SmallStringAlloc));
        ASSERT(LFP == sizeof(LargeString));
        ASSERT(LFP == sizeof(LargeStringAlloc));
        ASSERT(SFP-sizeof(bslma_Allocator*)-1 == SmallString::maxLength());
        ASSERT(SFP-sizeof(bslma_Allocator*)-1 ==
                                               SmallStringAlloc::maxLength());
        ASSERT(LFP-sizeof(bslma_Allocator*)-1 == LargeString::maxLength());
        ASSERT(LFP-sizeof(bslma_Allocator*)-1 ==
               LargeStringAlloc::maxLength());

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
            ASSERT(sizeof ALPHABET2 - 1 > SmallString::maxLength());

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
        for (int i = 0; i < DATA_SIZE; ++i) {
            const SmallString& U = *DATA[i];
            for (int j = 0; j < DATA_SIZE; ++j) {
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
        for (int i = 0; i < DATA_SIZE; ++i)
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
        //   bdem_ChoiceImp object are as expected, check that memory is
        //   being used as expected, and (if in verbose mode), print the value
        //   of the created or modified object.
        //   1. Create an object, mX, using the default constructor.
        //   2. Modify mX so that it holds small string.
        //   3. Modify mX so that it holds small alloc string, verify
        //      allocators.
        //   4. Modify mX so that it holds large string.
        //   5. Modify mX so that it holds large alloc string, verify
        //      allocators.
        //   6. Copy-construct mY from mX, verify that they are equal.
        //   7. Default construct mZ object.  Assign mZ = mY.
        //   8. Stream each choice imp into a test stream.
        //   9. Read all choice imps from the test stream into a vector of
        //      choice imps.
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        {
            if (veryVerbose) { bsl::cout << "\tDefault construct Obj mX"
                                         << bsl::endl; }
            bslma_TestAllocator alloc(veryVeryVerbose);

            Obj mX(&alloc); const Obj& X = mX;
            ASSERT(0  == X.numSelections());
            ASSERT(-1 == X.selector());
            ASSERT(!X.selection().isBound());
            if (veryVerbose) { bsl::cout << "\tX:" << bsl::endl;
                               X.print(bsl::cout, 1, 4); }

            if (veryVerbose) {
                bsl::cout << "\tModify mX to hold small string with value: "
                          << SS_1 << bsl::endl;
            }
            mX.addSelection(&my_SmallStringDescriptor);
            ASSERT(1  == X.numSelections());
            ASSERT(-1 == X.selector());
            ASSERT(!X.selection().isBound());
            ASSERT(my_ElemTypes::MY_SMALL_STRING == X.selectionType(0));
            mX.makeSelection(0);
            ASSERT(X.selection().isNull());
            ASSERT(SS_UNSET == * (SmallString *) X.selection().data());
            mX.selection().makeNull();
            ASSERT(X.selection().isNull());

            *(SmallString*)mX.makeSelection(0).data() = SS_1;
            ASSERT(1 == X.numSelections());
            ASSERT(0 == X.selector());
            ASSERT(my_ElemTypes::MY_SMALL_STRING == X.selectionType(0));
            ASSERT(SS_1    == *(SmallString *) X.selectionPointer());
            if (veryVerbose) { bsl::cout << "\tX:" << bsl::endl;
            X.print(bsl::cout, 1, 4); }

            if (veryVerbose) {
                 bsl::cout << "\tModify mX to hold small alloc string "
                              "with value: " << SSA_1 << bsl::endl;
            }

            mX.addSelection(&my_SmallStringAllocDescriptor);
            ASSERT(2  == X.numSelections());
            ASSERT(0 == X.selector());
            ASSERT(my_ElemTypes::MY_SMALL_STRING_ALLOC == X.selectionType(1));

            *(SmallStringAlloc*)mX.makeSelection(1).data() = SSA_1;
            ASSERT(2 == X.numSelections());
            ASSERT(1 == X.selector());
            ASSERT(my_ElemTypes::MY_SMALL_STRING_ALLOC == X.selectionType(1));
            ASSERT(SSA_1 == *(SmallStringAlloc *) X.selectionPointer());

            // verify allocators used
            ASSERT(&sampleAlloc == SSA_1.get_allocator());
            ASSERT(&alloc ==
                  (*(SmallStringAlloc *)X.selectionPointer()).get_allocator());

            if (veryVerbose) { bsl::cout << "\tX:" << bsl::endl;
            X.print(bsl::cout, 1, 4); }

            if (veryVerbose) {
                bsl::cout << "\tModify mX to hold large string with value: "
                          << LS_1 << bsl::endl;
            }
            mX.addSelection(&my_LargeStringDescriptor);
            ASSERT(3 == X.numSelections());
            ASSERT(1 == X.selector());
            ASSERT(my_ElemTypes::MY_LARGE_STRING == X.selectionType(2));

            *(LargeString*)mX.makeSelection(2).data() = LS_1;
            ASSERT(3 == X.numSelections());
            ASSERT(2 == X.selector());
            ASSERT(my_ElemTypes::MY_LARGE_STRING == X.selectionType(2));
            ASSERT(LS_1    == *(LargeString *) X.selectionPointer());
            if (veryVerbose) {
                bsl::cout << "\tX:" << bsl::endl;
                X.print(bsl::cout, 1, 4);
            }

            if (veryVerbose) {
                bsl::cout << "\tModify mX to hold large alloc string with "
                             "value: " << LSA_1 << bsl::endl;
            }
            mX.addSelection(&my_LargeStringAllocDescriptor);
            ASSERT(4 == X.numSelections());
            ASSERT(2 == X.selector());
            ASSERT(my_ElemTypes::MY_LARGE_STRING_ALLOC == X.selectionType(3));

            mX.makeSelection(3, &LSA_1);
            ASSERT(4 == X.numSelections());
            ASSERT(3 == X.selector());
            ASSERT(my_ElemTypes::MY_LARGE_STRING_ALLOC == X.selectionType(3));
            ASSERT(LSA_1    == *(LargeStringAlloc *) X.selectionPointer());
            if (veryVerbose) {
                bsl::cout << "\tX:" << bsl::endl;
                X.print(bsl::cout, 1, 4);
            }

            // verify allocators used
            ASSERT(&sampleAlloc == LSA_1.get_allocator());
            ASSERT(&alloc ==
               (*(LargeStringAlloc *) X.selectionPointer()).get_allocator());

            if (veryVerbose) {
                bsl::cout << "\tCopy Construct mY from mX" << bsl::endl;
            }
            ASSERT(!X.selection().isNull());
            Obj mY(mX, &alloc); const Obj& Y = mY;

            ASSERT(4 == Y.numSelections());
            ASSERT(3 == Y.selector());
            ASSERT(!Y.selection().isNull());
            ASSERT(my_ElemTypes::MY_LARGE_STRING_ALLOC == Y.selectionType(3));
            ASSERT(LSA_1    == *(LargeStringAlloc *) Y.selectionPointer());

            ASSERT(Y == X);
            ASSERT(!(Y != X));
            if (veryVerbose) { bsl::cout << "\tX:" << bsl::endl;
                               X.print(bsl::cout, 1, 4); }
            if (veryVerbose) { bsl::cout << "\tY:" << bsl::endl;
                               Y.print(bsl::cout, 1, 4); }

            if (veryVerbose) { bsl::cout << "\tDefault construct Obj mZ"
                                         << bsl::endl; }

            Obj mZ(&alloc); Obj& Z = mZ;
            ASSERT(0  == Z.numSelections());
            ASSERT(-1 == Z.selector());
            ASSERT(!Z.selection().isBound());
            if (veryVerbose) {
                bsl::cout << "\tZ:" << bsl::endl;
                Z.print(bsl::cout, 1, 4);
                bsl::cout << "\tAssign mZ to mY" << bsl::endl;
            }

            Z = Y;
            ASSERT(4     == Z.numSelections());
            ASSERT(3     == Z.selector());
            ASSERT(my_ElemTypes::MY_LARGE_STRING_ALLOC == Z.selectionType(3));
            ASSERT(LSA_1 == *(LargeStringAlloc *) Z.selectionPointer());

            if (veryVerbose) {
                bsl::cout << "\tStream out each choice imp" << bsl::endl;
            }
            bdex_TestOutStream os(&alloc);

            X.bdexStreamOutImp(
                 os,
                 1,
                 streamOutAttrLookup<bdex_TestOutStream>::lookupTable());

            Y.bdexStreamOutImp(
                 os,
                 1,
                 streamOutAttrLookup<bdex_TestOutStream>::lookupTable());

            Z.bdexStreamOutImp(
                 os,
                 1,
                 streamOutAttrLookup<bdex_TestOutStream>::lookupTable());

            ASSERT(os);

            bdex_TestInStream is(os.data(), os.length());
            bsl::vector<Obj> objVec(3);
            objVec[0].bdexStreamInImp(
                         is,
                         1,
                         streamInAttrLookup<bdex_TestInStream>::lookupTable(),
                         DESCRIPTORS);
            objVec[1].bdexStreamInImp(
                        is,
                         1,
                         streamInAttrLookup<bdex_TestInStream>::lookupTable(),
                         DESCRIPTORS);
            objVec[2].bdexStreamInImp(
                         is,
                         1,
                         streamInAttrLookup<bdex_TestInStream>::lookupTable(),
                         DESCRIPTORS);

            ASSERT(X == objVec[0]);
            ASSERT(!(X != objVec[0]));
            ASSERT(Y == objVec[1]);
            ASSERT(!(Y != objVec[1]));
            ASSERT(Z == objVec[2]);
            ASSERT(!(Z != objVec[2]));

            if (veryVerbose) {
                bsl::cout << "\tX:" << bsl::endl;
                X.print(bsl::cout, 1, 4);
                bsl::cout << "\tY:" << bsl::endl;
                Y.print(bsl::cout, 1, 4);
                bsl::cout << "\tZ:" << bsl::endl;
                Z.print(bsl::cout, 1, 4);
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
