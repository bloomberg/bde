// bdem_rowdata.t.cpp                                               -*-C++-*-

#include <bdem_rowdata.h>

#include <bdem_aggregateoption.h>
#include <bdem_descriptor.h>
#include <bdem_elemtype.h>
#include <bdem_functiontemplates.h>
#include <bdem_properties.h>
#include <bdem_rowlayout.h>

#include <bdema_bufferedsequentialallocator.h>   // for testing only

#include <bdet_date.h>                           // for testing only
#include <bdet_datetime.h>                       // for testing only
#include <bdet_datetimetz.h>                     // for testing only
#include <bdet_datetz.h>                         // for testing only
#include <bdet_time.h>                           // for testing only
#include <bdet_timetz.h>                         // for testing only

#include <bdetu_unset.h>                         // for testing only

#include <bdex_byteinstream.h>
#include <bdex_byteoutstream.h>
#include <bdex_testinstream.h>
#include <bdex_testinstreamexception.h>
#include <bdex_testoutstream.h>

#include <bslma_deallocatorproctor.h>            // for testing only
#include <bslma_default.h>                       // for testing only
#include <bslma_defaultallocatorguard.h>         // for testing only
#include <bslma_testallocator.h>                 // for testing only
#include <bslma_testallocatorexception.h>        // for testing only
#include <bsls_alignmentfromtype.h>
#include <bsls_alignmentutil.h>
#include <bsls_platform.h>                       // for testing only
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_strstream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_c_ctype.h>                            // isspace()

#ifdef BSLS_PLATFORM__OS_WINDOWS
#include <crtdbg.h>  // _CrtSetReportMode, to suppress popups
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// TBD: Test plan
//-----------------------------------------------------------------------------
// CLASS METHODS
// [13] int maxSupportedBdexVersion();
//
// CREATORS
// [ 4] bdem_RowData(strategy, bslma_Allocator *ba);
// [ 4] bdem_RowData(layout, strategy, bslma_Allocator *ba);
// [ 7] bdem_RowData(rowLayout, original, allocMode, *allocator);
// [ 7] bdem_RowData(rowLayout, original, si, ne, allocMode, *allocator);
// [ 4] ~bdem_RowData();
//
// MANIPULATORS
// [ 8] bdem_RowData& operator=(const bdem_RowData& rhs);
// [ 4] void *elemData(int index);
// [ 4] bdem_ElemRef elemRef(int index);
// [11] void *insertElement(int dstIndex, const void *value);
// [11] void *insertElementRaw(int dstIndex);
// [11] void  insertElements(int dstIndex, srcRowData, si, ne);
// [11] void *insertNullElement(int dstIndex);
// [11] void  insertNullElements(int dstIndex, int numElements);
// [10] void makeNull(int index);
// [10] void makeAllNull();
// [12] void removeElement(int dstIndex);
// [12] void removeElements(int dstIndex, int numElements);
// [14] void replaceValues(const bdem_RowData& other);
// [ 9] void reset();
// [ 9] void reset(rowLayout);
// [ 9] void reset(rowLayout, rowdata);
// [ 4] void setValue(int index, const void *value);
// [15] void swapElements(int index1, int index2);
// [16] void swap(bdem_RowData& rhs);
// [13] template <class STREAM>
//      STREAM& bdexStreamInImp(STREAM&                       stream,
//                              int                           version,
//                              const bdem_Descriptor *const *attrLookup;
//
// ACCESSORS
// [ 4] bool isAnyInRangeNonNull(int startIndex, int numElements) const;
// [ 4] bool isAnyInRangeNull(int startIndex, int numElements) const;
// [ 4] bool isNull(int index) const;
// [ 4] int length() const;
// [ 5] bsl::ostream& printRow(stream, level, spl, labels) const;
// [ 4] const bdem_RowLayout *rowLayout() const;
// [13] template <class STREAM>
//      STREAM& bdexStreamOutImp(STREAM& stream,
//                               int     version,
//                               int     minorVersion) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const bdem_RowData& lhs,
//                      const bdem_RowData& rhs);
// [ 6] bool operator!=(const bdem_RowData& lhs,
//                      const bdem_RowData& rhs);

//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] HELPER FUNCTIONS
// [ 3] ggg GENERATOR FUNCTION

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
#define T_ cout << "\t" << flush;                // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdem_RowData         Obj;
typedef bsls_Types::Int64    Int64;

typedef bdet_Datetime        Datetime;
typedef bdet_DatetimeTz      DatetimeTz;
typedef bdet_Date            Date;
typedef bdet_DateTz          DateTz;
typedef bdet_Time            Time;
typedef bdet_TimeTz          TimeTz;

typedef bdem_AggregateOption AggOption;
typedef bdem_ConstElemRef    CERef;
typedef bdem_Descriptor      Desc;
typedef bdem_ElemRef         ERef;
typedef bdem_ElemType        ET;
typedef bdem_Properties      Prop;

typedef bdem_RowData         Obj;
typedef bdem_RowLayout       Layout;
typedef bdem_Descriptor      Descriptor;

typedef bslma_Allocator      Allocator;
typedef bslma_TestAllocator  TestAllocator;

const bdem_AggregateOption::AllocationStrategy POOL_OPT =
                                         bdem_AggregateOption::BDEM_WRITE_MANY;
const bdem_AggregateOption::AllocationStrategy PT =
                                       bdem_AggregateOption::BDEM_PASS_THROUGH;
const bdem_AggregateOption::AllocationStrategy WM =
                                         bdem_AggregateOption::BDEM_WRITE_MANY;
const bdem_AggregateOption::AllocationStrategy WO =
                                         bdem_AggregateOption::BDEM_WRITE_ONCE;

//=============================================================================
//                           GLOBAL CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace BloombergLP {

class Base {
  public:
    enum { BUFSIZE = 100 };

    char             d_value;
    bool             d_isNull;
    void            *d_buf;
    bslma_Allocator *d_allocator;

    explicit
    Base(bslma_Allocator *a = 0)
    : d_allocator(bslma_Default::allocator(a)) {
        makeNull();
        d_buf = d_allocator->allocate(BUFSIZE);
    }
    explicit
    Base(char v, bslma_Allocator *a = 0)
    : d_value(v)
    , d_allocator(bslma_Default::allocator(a)) {
        d_isNull = false;
        d_buf = d_allocator->allocate(BUFSIZE);
    }
    explicit
    Base(const Base& original, bslma_Allocator *a = 0)
    : d_value(original.d_value)
    , d_isNull(original.d_isNull)
    , d_allocator(bslma_Default::allocator(a)) {
        d_buf = d_allocator->allocate(BUFSIZE);
    }
    const Base& operator=(const Base& rhs) {
        d_value = rhs.d_value;
        d_isNull = rhs.d_isNull;
        return *this;
    }
    ~Base() {
        d_allocator->deallocate(d_buf);
    }

    // MANIPULATORS
    void makeNull();

    // ACCESSORS
    bool isUnset() const;
};

bool operator==(const Base& lhs, const Base& rhs)
{
    return lhs.d_value == rhs.d_value && lhs.d_isNull == rhs.d_isNull;
}

bool operator!=(const Base& lhs, const Base& rhs)
{
    return !(lhs == rhs);
}

// MANIPULATORS
void Base::makeNull()
{
    d_value = 0;
    d_isNull = true;
}

// ACCESSORS
bool Base::isUnset() const
{
    return d_isNull;
}

class bdem_List : public Base {
  public:
    BSLALG_DECLARE_NESTED_TRAITS(bdem_List,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    explicit
    bdem_List(bslma_Allocator *a = 0) : Base(a) {}
    explicit
    bdem_List(char v, bslma_Allocator *a = 0) : Base(v, a) {}
    explicit
    bdem_List(const Base& original, bslma_Allocator *a = 0)
    : Base(* (const Base *) &original, a) {}
    const bdem_List& operator=(const bdem_List& rhs) {
        d_value = rhs.d_value;
        d_isNull = rhs.d_isNull;
        return *this;
    }
    ~bdem_List() {}
};

class bdem_Table : public Base {
  public:
    BSLALG_DECLARE_NESTED_TRAITS(bdem_Table,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    explicit
    bdem_Table(bslma_Allocator *a = 0) : Base(a) {}
    explicit
    bdem_Table(char v, bslma_Allocator *a = 0) : Base(v, a) {}
    bdem_Table(const Base& original, bslma_Allocator *a = 0)
    : Base(* (const Base *) &original, a) {}
    const bdem_Table& operator=(const bdem_Table& rhs) {
        d_value = rhs.d_value;
        d_isNull = rhs.d_isNull;
        return *this;
    }
    ~bdem_Table() {}
};

class bdem_Choice : public Base {
  public:
    BSLALG_DECLARE_NESTED_TRAITS(bdem_Choice,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    explicit
    bdem_Choice(bslma_Allocator *a = 0) : Base(a) {}
    explicit
    bdem_Choice(char v, bslma_Allocator *a = 0) : Base(v, a) {}
    explicit
    bdem_Choice(const Base& original, bslma_Allocator *a = 0)
    : Base(* (const Base *) &original, a) {}
    const bdem_Choice& operator=(const bdem_Choice& rhs) {
        d_value = rhs.d_value;
        d_isNull = rhs.d_isNull;
        return *this;
    }
    ~bdem_Choice() {}
};

class bdem_ChoiceArray : public Base {
  public:
    BSLALG_DECLARE_NESTED_TRAITS(bdem_ChoiceArray,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    explicit
    bdem_ChoiceArray(bslma_Allocator *a = 0) : Base(a) {}
    explicit
    bdem_ChoiceArray(char v, bslma_Allocator *a = 0) : Base(v, a) {}
    explicit
    bdem_ChoiceArray(const Base& original, bslma_Allocator *a = 0)
    : Base(* (const Base *) &original, a) {}
    const bdem_ChoiceArray& operator=(const bdem_ChoiceArray& rhs) {
        d_value = rhs.d_value;
        d_isNull = rhs.d_isNull;
        return *this;
    }
    ~bdem_ChoiceArray() {}
};

}

        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Empty Dummy descriptors for list, table, choice and choiceArray
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

                          // ==========================
                          // struct bdem_List_AttrFuncs
                          // ==========================

struct bdem_List_AttrFuncs
{
    // Namespace for static functions to store in a bdem_Descriptor structure.

    static
    void defaultConstruct(void                                     *obj,
                          bdem_AggregateOption::AllocationStrategy  allocMode,
                          bslma_Allocator                          *alloc);
        // Construct a list object into raw memory.  The prototype for this
        // function must match the defaultConstruct function pointer in
        // bdem_Descriptor.

    static
    void copyConstruct(void                                     *obj,
                       const void                               *rhs,
                       bdem_AggregateOption::AllocationStrategy  allocMode,
                       bslma_Allocator                          *alloc);
        // Copy construct a list object into raw memory.  The prototype for
        // this function must match the copyConstruct function pointer in
        // bdem_Descriptor.

    static void makeNull(void *obj);

    static bool isUnset(const void *obj);

    static bool areEqual(const void *lhs, const void *rhs);

    static bsl::ostream& print(const void    *obj,
                               bsl::ostream&  stream,
                               int            level,
                               int            spacesPerLevel);
        // Print this list.
};

                          // --------------------------
                          // struct bdem_List_AttrFuncs
                          // --------------------------

void bdem_List_AttrFuncs::defaultConstruct(
                           void                                     *obj,
                           bdem_AggregateOption::AllocationStrategy  allocMode,
                           bslma_Allocator                          *alloc)
{
    new (obj) bdem_List(alloc);
}

void bdem_List_AttrFuncs::copyConstruct(
                           void                                     *obj,
                           const void                               *rhs,
                           bdem_AggregateOption::AllocationStrategy  allocMode,
                           bslma_Allocator                          *alloc)
{
    const bdem_List& rhsList = *static_cast<const bdem_List *>(rhs);
    new (obj) bdem_List(rhsList);
}

void bdem_List_AttrFuncs::makeNull(void *obj)
{
    static_cast<bdem_List *>(obj)->makeNull();
}

bool bdem_List_AttrFuncs::isUnset(const void *obj)
{
    return static_cast<const bdem_List *>(obj)->isUnset();
}

bool bdem_List_AttrFuncs::areEqual(const void *lhs, const void *rhs)
{
    return * (const Base *) lhs == * (const Base *) rhs;
}

bsl::ostream& bdem_List_AttrFuncs::print(const void    *obj,
                                         bsl::ostream&  stream,
                                         int            level,
                                         int            spacesPerLevel)
{
    return stream;
}

const bdem_Descriptor listAttr = {
    bdem_ElemType::BDEM_LIST,
    sizeof(bdem_List),
    bsls_AlignmentFromType<bdem_List>::VALUE,
    &bdem_List_AttrFuncs::defaultConstruct,
    &bdem_List_AttrFuncs::copyConstruct,
    &bdem_FunctionTemplates::destroy<bdem_List>,
    &bdem_FunctionTemplates::assign<bdem_List>,
    &bdem_FunctionTemplates::bitwiseMove<bdem_List>,
    &bdem_List_AttrFuncs::makeNull,
    &bdem_List_AttrFuncs::isUnset,
    &bdem_List_AttrFuncs::areEqual,
    &bdem_List_AttrFuncs::print
};

                          // ===========================
                          // struct bdem_Table_AttrFuncs
                          // ===========================

struct bdem_Table_AttrFuncs {
    // Namespace for static functions to store in a bdem_Descriptor structure.

    static
    void defaultConstruct(void                                     *obj,
                          bdem_AggregateOption::AllocationStrategy  allocMode,
                          bslma_Allocator                          *alloc);
        // Construct a table object into raw memory.  The prototype for this
        // function must match the defaultConstruct function pointer in
        // bdem_Descriptor.

    static
    void copyConstruct(void                                     *obj,
                       const void                               *rhs,
                       bdem_AggregateOption::AllocationStrategy  allocMode,
                       bslma_Allocator                          *alloc);
        // Copy construct a table object into raw memory.  The prototype for
        // this function must match the copyConstruct function pointer in
        // bdem_Descriptor.

    static void makeNull(void *obj);

    static bool isUnset(const void *obj);

    static bool areEqual(const void *lhs, const void *rhs);

    static bsl::ostream& print(const void    *obj,
                               bsl::ostream&  stream,
                               int            level,
                               int            spacesPerLevel);
        // Print this table.
};

                          // ---------------------------
                          // struct bdem_Table_AttrFuncs
                          // ---------------------------

void bdem_Table_AttrFuncs::defaultConstruct(
                           void                                     *obj,
                           bdem_AggregateOption::AllocationStrategy  allocMode,
                           bslma_Allocator                          *alloc)
{
    new (obj) bdem_Table(alloc);;
}

void bdem_Table_AttrFuncs::copyConstruct(
                          void                                     *obj,
                          const void                               *rhs,
                          bdem_AggregateOption::AllocationStrategy  allocMode,
                          bslma_Allocator                          *alloc)
{
    const bdem_Table& rhsTable = *static_cast<const bdem_Table*>(rhs);
    new (obj) bdem_Table(rhsTable);
}

void bdem_Table_AttrFuncs::makeNull(void *obj)
{
    static_cast<bdem_Table *>(obj)->makeNull();
}

bool bdem_Table_AttrFuncs::isUnset(const void *obj)
{
    return static_cast<const bdem_Table *>(obj)->isUnset();
}

bool bdem_Table_AttrFuncs::areEqual(const void *lhs, const void *rhs)
{
    return * (const Base *) lhs == * (const Base *) rhs;
}

bsl::ostream& bdem_Table_AttrFuncs::print(const void    *obj,
                                          bsl::ostream&  stream,
                                          int            level,
                                          int            spacesPerLevel)
{
    return stream;
}

const bdem_Descriptor tableAttr = {
    bdem_ElemType::BDEM_TABLE,
    sizeof(bdem_Table),
    bsls_AlignmentFromType<bdem_Table>::VALUE,
    &bdem_Table_AttrFuncs::defaultConstruct,
    &bdem_Table_AttrFuncs::copyConstruct,
    &bdem_FunctionTemplates::destroy<bdem_Table>,
    &bdem_FunctionTemplates::assign<bdem_Table>,
    &bdem_FunctionTemplates::bitwiseMove<bdem_Table>,
    &bdem_Table_AttrFuncs::makeNull,
    &bdem_Table_AttrFuncs::isUnset,
    &bdem_Table_AttrFuncs::areEqual,
    &bdem_Table_AttrFuncs::print
};

                          // ============================
                          // struct bdem_Choice_AttrFuncs
                          // ============================

struct bdem_Choice_AttrFuncs
{
    // Namespace for static functions to store in a bdem_Descriptor structure.

    static
    void defaultConstruct(void                                     *obj,
                          bdem_AggregateOption::AllocationStrategy  allocMode,
                          bslma_Allocator                          *alloc);
        // Construct a choice object into raw memory.  The prototype for this
        // function must match the defaultConstruct function pointer in
        // bdem_Descriptor.

    static
    void copyConstruct(void                                     *obj,
                       const void                               *rhs,
                       bdem_AggregateOption::AllocationStrategy  allocMode,
                       bslma_Allocator                          *alloc);
        // Copy construct a choice object into raw memory.  The prototype for
        // this function must match the copyConstruct function pointer in
        // bdem_Descriptor.

    static void makeNull(void *obj);

    static bool isUnset(const void *obj);

    static bool areEqual(const void *lhs, const void *rhs);

    static bsl::ostream& print(const void    *obj,
                               bsl::ostream&  stream,
                               int            level,
                               int            spacesPerLevel);
        // Print this choice.
};

                          // ----------------------------
                          // struct bdem_Choice_AttrFuncs
                          // ----------------------------

void bdem_Choice_AttrFuncs::defaultConstruct(
                           void                                     *obj,
                           bdem_AggregateOption::AllocationStrategy  allocMode,
                           bslma_Allocator                          *alloc)
{
    new (obj) bdem_Choice(alloc);;
}

void bdem_Choice_AttrFuncs::copyConstruct(
                           void                                     *obj,
                           const void                               *rhs,
                           bdem_AggregateOption::AllocationStrategy  allocMode,
                           bslma_Allocator                          *alloc)
{
    const bdem_Choice& rhsChoice = *static_cast<const bdem_Choice *>(rhs);
    new (obj) bdem_Choice(rhsChoice);
}

void bdem_Choice_AttrFuncs::makeNull(void *obj)
{
    static_cast<bdem_Choice *>(obj)->makeNull();
}

bool bdem_Choice_AttrFuncs::isUnset(const void *obj)
{
    return static_cast<const bdem_Choice *>(obj)->isUnset();
}

bool bdem_Choice_AttrFuncs::areEqual(const void *lhs, const void *rhs)
{
    return * (const Base *) lhs == * (const Base *) rhs;
}

bsl::ostream& bdem_Choice_AttrFuncs::print(const void    *obj,
                                         bsl::ostream&  stream,
                                         int            level,
                                         int            spacesPerLevel)
{
    return stream;
}

const bdem_Descriptor choiceAttr = {
    bdem_ElemType::BDEM_CHOICE,
    sizeof(bdem_Choice),
    bsls_AlignmentFromType<bdem_Choice>::VALUE,
    &bdem_Choice_AttrFuncs::defaultConstruct,
    &bdem_Choice_AttrFuncs::copyConstruct,
    &bdem_FunctionTemplates::destroy<bdem_Choice>,
    &bdem_FunctionTemplates::assign<bdem_Choice>,
    &bdem_FunctionTemplates::bitwiseMove<bdem_Choice>,
    &bdem_Choice_AttrFuncs::makeNull,
    &bdem_Choice_AttrFuncs::isUnset,
    &bdem_Choice_AttrFuncs::areEqual,
    &bdem_Choice_AttrFuncs::print
};

                          // =================================
                          // struct bdem_ChoiceArray_AttrFuncs
                          // =================================

struct bdem_ChoiceArray_AttrFuncs {
    // Namespace for static functions to store in a bdem_Descriptor structure.

    static
    void defaultConstruct(void                                     *obj,
                          bdem_AggregateOption::AllocationStrategy  allocMode,
                          bslma_Allocator                          *alloc);
        // Construct a choice array object into raw memory.  The prototype for
        // this function must match the defaultConstruct function pointer in
        // bdem_Descriptor.

    static
    void copyConstruct(void                                     *obj,
                       const void                               *rhs,
                       bdem_AggregateOption::AllocationStrategy  allocMode,
                       bslma_Allocator                          *alloc);
        // Copy construct a choiceArray object into raw memory.  The prototype
        // for this function must match the copyConstruct function pointer in
        // bdem_Descriptor.

    static void makeNull(void *obj);

    static bool isUnset(const void *obj);

    static bool areEqual(const void *lhs, const void *rhs);

    static bsl::ostream& print(const void    *obj,
                               bsl::ostream&  stream,
                               int            level,
                               int            spacesPerLevel);
        // Print this choice array.
};

                          // ---------------------------------
                          // struct bdem_ChoiceArray_AttrFuncs
                          // ---------------------------------

void bdem_ChoiceArray_AttrFuncs::defaultConstruct(
                           void                                     *obj,
                           bdem_AggregateOption::AllocationStrategy  allocMode,
                           bslma_Allocator                          *alloc)
{
    new (obj) bdem_ChoiceArray(alloc);;
}

void bdem_ChoiceArray_AttrFuncs::copyConstruct(
                          void                                     *obj,
                          const void                               *rhs,
                          bdem_AggregateOption::AllocationStrategy  allocMode,
                          bslma_Allocator                          *alloc)
{
    const bdem_ChoiceArray& rhsChoiceArray =
                                    *static_cast<const bdem_ChoiceArray*>(rhs);
    new (obj) bdem_ChoiceArray(rhsChoiceArray);
}

void bdem_ChoiceArray_AttrFuncs::makeNull(void *obj)
{
    static_cast<bdem_ChoiceArray *>(obj)->makeNull();
}

bool bdem_ChoiceArray_AttrFuncs::isUnset(const void *obj)
{
    return static_cast<const bdem_ChoiceArray *>(obj)->isUnset();
}

bool bdem_ChoiceArray_AttrFuncs::areEqual(const void *lhs, const void *rhs)
{
    return * (const Base *) lhs == * (const Base *) rhs;
}

bsl::ostream& bdem_ChoiceArray_AttrFuncs::print(const void    *obj,
                                          bsl::ostream&  stream,
                                          int            level,
                                          int            spacesPerLevel)
{
    return stream;
}

const bdem_Descriptor choiceArrayAttr = {
    bdem_ElemType::BDEM_CHOICE_ARRAY,
    sizeof(bdem_ChoiceArray),
    bsls_AlignmentFromType<bdem_ChoiceArray>::VALUE,
    &bdem_ChoiceArray_AttrFuncs::defaultConstruct,
    &bdem_ChoiceArray_AttrFuncs::copyConstruct,
    &bdem_FunctionTemplates::destroy<bdem_ChoiceArray>,
    &bdem_FunctionTemplates::assign<bdem_ChoiceArray>,
    &bdem_FunctionTemplates::bitwiseMove<bdem_ChoiceArray>,
    &bdem_ChoiceArray_AttrFuncs::makeNull,
    &bdem_ChoiceArray_AttrFuncs::isUnset,
    &bdem_ChoiceArray_AttrFuncs::areEqual,
    &bdem_ChoiceArray_AttrFuncs::print
};

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

const bsls_Types::Int64
                       A03 = -100;
const bsls_Types::Int64
                       B03 = -200;
const bsls_Types::Int64
                       N03 = bdetu_Unset<bsls_Types::Int64>::unsetValue();

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

static bsl::vector<bsls_Types::Int64>           fA13() {
       bsl::vector<bsls_Types::Int64> t;
                                     t.push_back(A03); return t; }
static bsl::vector<bsls_Types::Int64>           fB13() {
       bsl::vector<bsls_Types::Int64> t;
                                     t.push_back(B03); return t; }
const  bsl::vector<bsls_Types::Int64>            A13 = fA13();
const  bsl::vector<bsls_Types::Int64>            B13 = fB13();
const  bsl::vector<bsls_Types::Int64>            N13;

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

static bdem_List        A20(1), B20(2), N20;
static bdem_Table       A21(3), B21(4), N21;
static bdem_Choice      A30(4), B30(5), N30;
static bdem_ChoiceArray A31(5), B31(6), N31;

static char SPECIFICATIONS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
    // This string stores the valid specification values representing the row.
    // Each character specifies an element type and the order
    // for character to element type mapping is similar to that in
    // bdem_elemtype.h.  Thus, 'A' corresponds to 'CHAR', 'B'- 'SHORT',
    // ...  Z' - 'TIMETZ', 'a' - 'BOOL_ARRAY', ...  f' - 'CHOICE_ARRAY'.
    // Additionally for this test 'g' - 'SmallString', 'h' -
    // 'SmallStringAlloc', 'i' - 'LargeString' and 'j' - 'LargeStringAlloc'.
const int  SPEC_LEN   = sizeof SPECIFICATIONS - 1;
const char UNSET_CHAR = '@';
const char NULL_CHAR  = '*';

static const AggOption::AllocationStrategy PASSTH =
                                                  AggOption::BDEM_PASS_THROUGH;

                        // ===========================
                        // Dummy streaming descriptors
                        // ===========================

                        // -------------------------
                        // struct streamInAttrLookup
                        // -------------------------

template <class STREAM>
struct streamInAttrLookup
{
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
                        streamInFundamental<bsls_Types::Int64,STREAM> },
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
                              streamInArray<bsls_Types::Int64,STREAM> },
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
struct streamOutAttrLookup
{
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
                      streamOutFundamental<bsls_Types::Int64, STREAM> },
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
                            streamOutArray<bsls_Types::Int64, STREAM> },
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

static const Desc * const DESC[] =
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
const int NUM_DESCS = sizeof DESC / sizeof *DESC;

enum {
    CHAR_INDEX             = 0,
    SHORT_INDEX            = 1,
    INT_INDEX              = 2,
    INT64_INDEX            = 3,
    FLOAT_INDEX            = 4,
    DOUBLE_INDEX           = 5,
    STRING_INDEX           = 6,
    DATETIME_INDEX         = 7,
    DATE_INDEX             = 8,
    TIME_INDEX             = 9,

    CHAR_ARRAY_INDEX       = 10,
    SHORT_ARRAY_INDEX      = 11,
    INT_ARRAY_INDEX        = 12,
    INT64_ARRAY_INDEX      = 13,
    FLOAT_ARRAY_INDEX      = 14,
    DOUBLE_ARRAY_INDEX     = 15,
    STRING_ARRAY_INDEX     = 16,
    DATETIME_ARRAY_INDEX   = 17,
    DATE_ARRAY_INDEX       = 18,
    TIME_ARRAY_INDEX       = 19,

    LIST_INDEX             = 20,
    TABLE_INDEX            = 21,

    BOOL_INDEX             = 22,
    DATETIMETZ_INDEX       = 23,
    DATETZ_INDEX           = 24,
    TIMETZ_INDEX           = 25,
    BOOL_ARRAY_INDEX       = 26,
    DATETIMETZ_ARRAY_INDEX = 27,
    DATETZ_ARRAY_INDEX     = 28,
    TIMETZ_ARRAY_INDEX     = 29,

    CHOICE_INDEX           = 30,
    CHOICE_ARRAY_INDEX     = 31
};

//=============================================================================
//              GENERATOR FUNCTIONS 'g', 'gg', 'ggg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Alphabets
// [A .. f] correspond to arbitrary (but unique) element types to be inserted
// into the 'bdem_RowLayout' object held by this 'bdem_RowData'.  A tilde ('~')
// indicates that the logical (but not necessarily physical) state of the
// object is to be set to its initial, empty state (via the 'removeAll'
// method).  The characters 'N', '1', and '2' are used to specify the values
// for the elements.
//
// LANGUAGE SPECIFICATION
// ----------------------
//
// <SPEC>       ::= <EMPTY>   | <ROW_DATA>
//
// <EMPTY>      ::=
//
// <ROW_DATA>   ::= <ELEMENT> | <ELEMENT><LIST>
//
// <ELEMENT>    ::= <TYPE> | <VALUE>
//
// <TYPE>       ::= 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' |
//                  'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' |
//                  'S' | 'T' | 'U' | 'V' | 'a' | 'b' | 'c' | 'd' | 'e' |
//                  'f' | 'g' | 'h'
//
// <VALUE>      ::= 'N' | '1' | '2'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// "A1"         Append the element having the type corresponding to 'A' and the
//              value corresponding to '1'.
// "A1AN"       Append two elements having the type corresponding to 'A' and
//              the values corresponding to '1' and 'N' respectively.
//-----------------------------------------------------------------------------

static ET::Type getElemType(char spec)
    // Return the element type corresponding to the specified
    // 'spec' value.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".
{
    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return (ET::Type) index;
}

static const Desc *getDescriptor(char spec)
    // Return the non-modifiable descriptor corresponding to the specified
    // 'spec' value.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".
{
    return DESC[getElemType(spec)];
}

static const CERef VALUES_A[] =
    // This array stores a set of values corresponding to each element
    // type specified in the spec string.
{
    CERef((const void *) &A00, DESC[0]),
    CERef((const void *) &A01, DESC[1]),
    CERef((const void *) &A02, DESC[2]),
    CERef((const void *) &A03, DESC[3]),
    CERef((const void *) &A04, DESC[4]),
    CERef((const void *) &A05, DESC[5]),
    CERef((const void *) &A06, DESC[6]),
    CERef((const void *) &A07, DESC[7]),
    CERef((const void *) &A08, DESC[8]),
    CERef((const void *) &A09, DESC[9]),
    CERef((const void *) &A10, DESC[10]),
    CERef((const void *) &A11, DESC[11]),
    CERef((const void *) &A12, DESC[12]),
    CERef((const void *) &A13, DESC[13]),
    CERef((const void *) &A14, DESC[14]),
    CERef((const void *) &A15, DESC[15]),
    CERef((const void *) &A16, DESC[16]),
    CERef((const void *) &A17, DESC[17]),
    CERef((const void *) &A18, DESC[18]),
    CERef((const void *) &A19, DESC[19]),
    CERef((const void *) &A20, DESC[20]),
    CERef((const void *) &A21, DESC[21]),
    CERef((const void *) &A22, DESC[22]),
    CERef((const void *) &A23, DESC[23]),
    CERef((const void *) &A24, DESC[24]),
    CERef((const void *) &A25, DESC[25]),
    CERef((const void *) &A26, DESC[26]),
    CERef((const void *) &A27, DESC[27]),
    CERef((const void *) &A28, DESC[28]),
    CERef((const void *) &A29, DESC[29]),
    CERef((const void *) &A30, DESC[30]),
    CERef((const void *) &A31, DESC[31]),
};
const int NUM_VALUESA = sizeof VALUES_A / sizeof *VALUES_A;

static const CERef VALUES_B[] =
    // This array stores a set of values corresponding to each element
    // type specified in the spec string.
{
    CERef((const void *) &B00, DESC[0]),
    CERef((const void *) &B01, DESC[1]),
    CERef((const void *) &B02, DESC[2]),
    CERef((const void *) &B03, DESC[3]),
    CERef((const void *) &B04, DESC[4]),
    CERef((const void *) &B05, DESC[5]),
    CERef((const void *) &B06, DESC[6]),
    CERef((const void *) &B07, DESC[7]),
    CERef((const void *) &B08, DESC[8]),
    CERef((const void *) &B09, DESC[9]),
    CERef((const void *) &B10, DESC[10]),
    CERef((const void *) &B11, DESC[11]),
    CERef((const void *) &B12, DESC[12]),
    CERef((const void *) &B13, DESC[13]),
    CERef((const void *) &B14, DESC[14]),
    CERef((const void *) &B15, DESC[15]),
    CERef((const void *) &B16, DESC[16]),
    CERef((const void *) &B17, DESC[17]),
    CERef((const void *) &B18, DESC[18]),
    CERef((const void *) &B19, DESC[19]),
    CERef((const void *) &B20, DESC[20]),
    CERef((const void *) &B21, DESC[21]),
    CERef((const void *) &B22, DESC[22]),
    CERef((const void *) &B23, DESC[23]),
    CERef((const void *) &B24, DESC[24]),
    CERef((const void *) &B25, DESC[25]),
    CERef((const void *) &B26, DESC[26]),
    CERef((const void *) &B27, DESC[27]),
    CERef((const void *) &B28, DESC[28]),
    CERef((const void *) &B29, DESC[29]),
    CERef((const void *) &B30, DESC[30]),
    CERef((const void *) &B31, DESC[31]),
};
const int NUM_VALUESB = sizeof VALUES_B / sizeof *VALUES_B;

static const CERef VALUES_U[] =
    // This array stores a set of values corresponding to each element
    // type specified in the spec string.
{
    CERef((const void *) &N00, DESC[0]),
    CERef((const void *) &N01, DESC[1]),
    CERef((const void *) &N02, DESC[2]),
    CERef((const void *) &N03, DESC[3]),
    CERef((const void *) &N04, DESC[4]),
    CERef((const void *) &N05, DESC[5]),
    CERef((const void *) &N06, DESC[6]),
    CERef((const void *) &N07, DESC[7]),
    CERef((const void *) &N08, DESC[8]),
    CERef((const void *) &N09, DESC[9]),
    CERef((const void *) &N10, DESC[10]),
    CERef((const void *) &N11, DESC[11]),
    CERef((const void *) &N12, DESC[12]),
    CERef((const void *) &N13, DESC[13]),
    CERef((const void *) &N14, DESC[14]),
    CERef((const void *) &N15, DESC[15]),
    CERef((const void *) &N16, DESC[16]),
    CERef((const void *) &N17, DESC[17]),
    CERef((const void *) &N18, DESC[18]),
    CERef((const void *) &N19, DESC[19]),
    CERef((const void *) &N20, DESC[20]),
    CERef((const void *) &N21, DESC[21]),
    CERef((const void *) &N22, DESC[22]),
    CERef((const void *) &N23, DESC[23]),
    CERef((const void *) &N24, DESC[24]),
    CERef((const void *) &N25, DESC[25]),
    CERef((const void *) &N26, DESC[26]),
    CERef((const void *) &N27, DESC[27]),
    CERef((const void *) &N28, DESC[28]),
    CERef((const void *) &N29, DESC[29]),
    CERef((const void *) &N30, DESC[30]),
    CERef((const void *) &N31, DESC[31]),
};
const int NUM_VALUESU = sizeof VALUES_U / sizeof *VALUES_U;

static const int NULLNESS_BIT = 1, NULLNESS_OFFSET = 0;
static const CERef VALUES_N[] =
    // This array stores a set of values corresponding to each element
    // type specified in the spec string.
{
    CERef((const void *) &N00, DESC[0], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N01, DESC[1], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N02, DESC[2], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N03, DESC[3], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N04, DESC[4], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N05, DESC[5], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N06, DESC[6], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N07, DESC[7], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N08, DESC[8], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N09, DESC[9], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N10, DESC[10], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N11, DESC[11], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N12, DESC[12], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N13, DESC[13], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N14, DESC[14], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N15, DESC[15], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N16, DESC[16], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N17, DESC[17], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N18, DESC[18], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N19, DESC[19], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N20, DESC[20], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N21, DESC[21], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N22, DESC[22], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N23, DESC[23], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N24, DESC[24], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N25, DESC[25], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N26, DESC[26], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N27, DESC[27], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N28, DESC[28], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N29, DESC[29], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N30, DESC[30], &NULLNESS_BIT, NULLNESS_OFFSET),
    CERef((const void *) &N31, DESC[31], &NULLNESS_BIT, NULLNESS_OFFSET),
};
const int NUM_VALUESN = sizeof VALUES_N / sizeof *VALUES_N;

                        // ================
                        // class BdexHelper
                        // ================

template <class CHAR_TYPE>
class BdexHelper {
    // This 'class' is used in testing of 'bdex' streaming whereby version 1
    // maps null values to "unset", and version 2 maps "unset" values to null.

    // DATA
    int d_version;

  public:
    BdexHelper(int version)
    : d_version(version)
    {
    }

    bool operator()(CHAR_TYPE& value) const
    {
       if (1 == d_version) {
           if ('*' == value) {
               value = '@';
           }
       }
       else if (2 == d_version) {
           if ('@' == value) {
               value = '*';
           }
       }
       return true;
    }
};

bool isUnset(const CERef& ref)
    // Return 'true' if the specified element 'ref' references an element that
    // has the "unset" value for its type, and 'false' otherwise.
{
    return ref.descriptor()->isUnset(ref.data());
}

CERef getCERef(ET::Type type, char valueSpec)
    // Return the const elem ref value corresponding to the specified 'value'
    // (one of '1', '2', 'n', and 'u') of the specified 'type'.
{
    switch (valueSpec) {
      case 'u': return VALUES_U[(int) type];
      case 'n': return VALUES_N[(int) type];
      case '1': return VALUES_A[(int) type];
      case '2': return VALUES_B[(int) type];
      default: ASSERT(0);
    }
    return CERef(0, 0);
}

ERef getERef(ET::Type type, char valueSpec)
    // Return the elem ref value corresponding to the specified 'value'
    // (one of 'A', 'B', 'N') of the specified 'type'.
{
    CERef T = getCERef(type, valueSpec);
    return ERef((void *) T.data(), T.descriptor());
}

static const void *getValueA(char spec)
    // Return the 'A' value corresponding to the specified 'spec'.  Valid
    // input consists of uppercase letters where the index of each letter is
    // in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".
{
    ASSERT('U' != spec || 'V' != spec || 'e' != spec || 'f' != spec);

    return getCERef(getElemType(spec), '1').data();
}

static const void *getValueB(char spec)
    // Return the 'B' value corresponding to the specified 'spec'.
{
    ASSERT('U' != spec || 'V' != spec || 'e' != spec || 'f' != spec);

    return getCERef(getElemType(spec), '2').data();
}

static const void *getValueU(char spec)
    // Return the 'N' value corresponding to the specified 'spec'.
{
    ASSERT('U' != spec || 'V' != spec || 'e' != spec || 'f' != spec);

    return getCERef(getElemType(spec), 'u').data();
}

static const void *getValueN(char spec)
    // Return the 'N' value corresponding to the specified 'spec'.
{
    ASSERT('U' != spec || 'V' != spec || 'e' != spec || 'f' != spec);

    return getCERef(getElemType(spec), 'n').data();
}

static const void *getValue(char type, char valueSpec)
{
    switch (valueSpec) {
      case 'u': return getValueU(type);
      case 'n': return getValueN(type);
      case '1': return getValueA(type);
      case '2': return getValueB(type);
      default:  return 0;
    }
    return 0;
}

Layout gLayout(const char *spec)
{
    const int len = bsl::strlen(spec);
    bsl::vector<ET::Type> types(len);
    for (int i = 0; i < len; ++i) {
        ASSERT(bsl::strchr(SPECIFICATIONS, spec[i]));

        types[i] = getElemType(spec[i]);
    }
    return Layout(types.begin(), types.size(), DESC);
}

int ggg(Obj *obj, const char *spec, Allocator *ta = 0, int verboseFlag = 1)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'insert' and white-box
    // manipulator 'reset'.  Optionally specify a zero 'verboseFlag' to
    // suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    enum { FAILURE = -1, SUCCESS = 0 };

    const int len = bsl::strlen(spec);
    ta = bslma_Default::allocator(ta);
    Layout *layout = new (*ta) bdem_RowLayout(ta);

    obj->reset(layout);

    int si = 0, di = 0;
    while (si < len) {
        if ('~' == spec[si]) {
            obj->removeElements(0, obj->length());
            layout->removeAll();
            di = 0;
        }

        if (si + 1 >= len) {
            return FAILURE;
        }

        char typeSpec  = spec[si];
        char valueSpec = spec[si + 1];

        if (!bsl::strchr(SPECIFICATIONS, typeSpec)
         || ('u' != valueSpec && 'n' != valueSpec
             && '1' != valueSpec && '2' != valueSpec)) {
            obj->reset();
            ta->deleteObject(layout);

            return FAILURE;
        }

        layout->append(getDescriptor(typeSpec));
        if ('n' == valueSpec) {
            obj->insertNullElement(di);
        }
        else {
            obj->insertElement(di, getValue(typeSpec, valueSpec));
        }
        si += 2, ++di;
    }

    return SUCCESS;
}

Obj& gg(Obj *obj, const char *spec, Allocator *ta = 0)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec' and using the specified 'ta' for
    // memory allocations.
{
    ASSERT(ggg(obj, spec, ta) < 0);
    return *obj;
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

bsl::ostream& operator<<(bsl::ostream& os, const Obj& object)
{
    return object.printRow(os, 0, -1);
}

static int compareUsingVersion(const Obj& src, const Obj& dst, int version)
    // Compare the specified 'dst' and 'src' row data objects using the
    // specified 'version' and return 'true' if they are equal, and 'false'
    // otherwise.  Note that for version 1 a null element in 'src' is equal
    // to an unset element in 'dst', and in version 2 an unset element in
    // 'src' is equal to a null element in 'dst'.
{
    if (src.length() != dst.length()) {
        return false;                                                 // RETURN
    }

    switch (version) {
      case 1: {
          for (int i = 0; i < src.length(); ++i) {
              const bdem_ConstElemRef& srcRef = src.elemRef(i);
              const bdem_ConstElemRef& dstRef = dst.elemRef(i);

              if (srcRef.type() != dstRef.type()) {
                  return false;                                       // RETURN
              }

              if (srcRef != dstRef) {
                  if (srcRef.isNull() && isUnset(dstRef)) {
                      continue;
                  }
                  return false;                                       // RETURN
              }
          }
      } break;
      case 2: {
          for (int i = 0; i < src.length(); ++i) {
              const bdem_ConstElemRef& srcRef = src.elemRef(i);
              const bdem_ConstElemRef& dstRef = dst.elemRef(i);

              if (srcRef.type() != dstRef.type()) {
                  return false;                                       // RETURN
              }

              if (srcRef != dstRef) {
                  if (isUnset(srcRef) && dstRef.isNull()) {
                      continue;
                  }
                  return false;                                       // RETURN
              }
          }
      } break;
      default: {
          return dst == src;                                          // RETURN
      } break;
    }
    return true;
}

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
      case 'U': return 1; // Comparing bdem_List
      case 'V': return 1; // Comparing bdem_Table
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
      case 'e': return 1;
      case 'f': return 1;
      default: ASSERT(0); return 0;
    }
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
    int veryVeryVeryVerbose = argc > 5;
    int veryVeryVeryVeryVerbose = argc > 6;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

#ifdef BSLS_PLATFORM__OS_WINDOWS
    // Suppress all windows debugging popups
    _CrtSetReportMode(_CRT_ASSERT, 0);
    _CrtSetReportMode(_CRT_ERROR,  0);
    _CrtSetReportMode(_CRT_WARN,   0);
#endif

    bslma_TestAllocator  testAllocator(veryVeryVeryVeryVerbose);
    bslma_TestAllocator *Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'swapElement' FUNCTION
        //
        // Concerns:
        //   Test 'swapElement' function.
        //
        // Plan:
        //
        // Testing:
        //   void swapElement(int index1, int index2);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'swapElement' function\n"
                             "==============================\n";

        struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            { L_, "" },
            { L_, "A1" },
            { L_, "A2" },
            { L_, "Au" },
            { L_, "An" },
            { L_, "F1" },
            { L_, "F2" },
            { L_, "Fu" },
            { L_, "Fn" },
            { L_, "G1" },
            { L_, "G2" },
            { L_, "Gu" },
            { L_, "Gn" },
            { L_, "A1A2A1" },
            { L_, "A1A2A2" },
            { L_, "A1A2Au" },
            { L_, "A1A2An" },
            { L_, "A1B2C1" },
            { L_, "E1F1G1" },
            { L_, "I1J1K1" },
            { L_, "M1N1O1" },
            { L_, "M1O1P1" },
            { L_, "M1O2P1" },
            { L_, "M1OuP1" },
            { L_, "M1OnP1" },
            { L_, "M1N1On" },
            { L_, "M1N1P1" },
            { L_, "A1A2AuAn" },
            { L_, "A1B2C1D2" },
            { L_, "E1F1G1H1" },
            { L_, "E1F1GnHu" },
            { L_, "I1J1K1L1" },
            { L_, "M1N1O1P1" },
            { L_, "M1N1O1Pu" },
            { L_, "M1N1OnP1" }
            // TBD: Add test cases
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;

            TestAllocator ta;

            const Layout *L;
            {
                Obj mX(PT, &ta); const Obj& X = mX;
                int rc = ggg(&mX, SPEC, &ta);
                LOOP_ASSERT(LINE, !rc);

                if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(X) };

                L = X.rowLayout();

                const int LEN = X.length();
                for (int j = 0; j < LEN; ++j) {
                    for (int k = 0; k < LEN; ++k) {
                        bool JNULL = X.isNull(j);
                        bool KNULL = X.isNull(k);

                        mX.swapElements(j, k);

                        ASSERT(KNULL == X.isNull(j));
                        ASSERT(JNULL == X.isNull(k));
                    }
                }
            }
            ta.deleteObjectRaw(L);
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'swap' FUNCTION
        //
        // Concerns:
        //   Test 'swap' function.
        //
        // Plan:
        //
        // Testing:
        //   void swap(bdem_RowData& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'swap' functions\n"
                             "========================\n";

        struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            { L_, "" },
            { L_, "A1" },
            { L_, "A2" },
            { L_, "Au" },
            { L_, "An" },
            { L_, "F1" },
            { L_, "F2" },
            { L_, "Fu" },
            { L_, "Fn" },
            { L_, "G1" },
            { L_, "G2" },
            { L_, "Gu" },
            { L_, "Gn" },
            { L_, "A1A2A1" },
            { L_, "A1A2A2" },
            { L_, "A1A2Au" },
            { L_, "A1A2An" },
            { L_, "A1B2C1" },
            { L_, "E1F1G1" },
            { L_, "I1J1K1" },
            { L_, "M1N1O1" },
            { L_, "M1O1P1" },
            { L_, "M1O2P1" },
            { L_, "M1OuP1" },
            { L_, "M1OnP1" },
            { L_, "M1N1On" },
            { L_, "M1N1P1" },
            { L_, "A1A2AuAn" },
            { L_, "A1B2C1D2" },
            { L_, "E1F1G1H1" },
            { L_, "E1F1GnHu" },
            { L_, "I1J1K1L1" },
            { L_, "M1N1O1P1" },
            { L_, "M1N1O1Pu" },
            { L_, "M1N1OnP1" }
            // TBD: Add test cases
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int k = 0; k < 4; ++k) {
            AggOption::AllocationStrategy strategy =
                                  (bdem_AggregateOption::AllocationStrategy) k;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE1 = DATA[i].d_line;
                const char *SPEC1 = DATA[i].d_spec;

                TestAllocator ta;
                if (AggOption::BDEM_NODESTRUCT_FLAG & strategy) {
                    ta.setQuiet(1);
                }

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int   LINE2 = DATA[j].d_line;
                    const char *SPEC2 = DATA[j].d_spec;

                    const Layout *LX, *LY;
                    {
                        Obj mX(strategy, &ta); const Obj& X = mX;
                        int rc = ggg(&mX, SPEC1, &ta);
                        LOOP_ASSERT(LINE1, !rc);

                        LX = X.rowLayout();

                        Obj mCX(LX, X, PT); const Obj& CX = mCX;

                        ASSERT(X == CX);

                        Obj mY(strategy, &ta); const Obj& Y = mY;
                        rc = ggg(&mY, SPEC2, &ta);
                        LOOP_ASSERT(LINE2, !rc);

                        LY = Y.rowLayout();

                        Obj mCY(LY, Y, PT); const Obj& CY = mCY;

                        ASSERT(Y == CY);

                        if (veryVerbose) { T_ P_(LINE1) P_(LINE2)
                                   P_(SPEC1) P_(SPEC2) P(X) P(Y) P(CX) P(CY) };

                      BEGIN_BSLMA_EXCEPTION_TEST {
                        mY.swap(mX);

                        LOOP2_ASSERT(Y, CX, Y == CX);
                        LOOP2_ASSERT(LINE1, LINE2, X == CY);
                      } END_BSLMA_EXCEPTION_TEST
                    }
                    ta.deleteObjectRaw(LX);
                    ta.deleteObjectRaw(LY);
                }
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'replaceValues' FUNCTIONS
        //
        // Concerns:
        //   Test 'replaceValues' functions.
        //
        // Plan:
        //
        // Testing:
        //   void replaceValues(const bdem_RowData& other);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'replaceValues' functions\n"
                             "=================================\n";

        struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            { L_, "" },
            { L_, "A1" },
            { L_, "A2" },
            { L_, "Au" },
            { L_, "An" },
            { L_, "F1" },
            { L_, "F2" },
            { L_, "Fu" },
            { L_, "Fn" },
            { L_, "G1" },
            { L_, "G2" },
            { L_, "Gu" },
            { L_, "Gn" },
            { L_, "A1A2A1" },
            { L_, "A1A2A2" },
            { L_, "A1A2Au" },
            { L_, "A1A2An" },
            { L_, "A1B2C1" },
            { L_, "E1F1G1" },
            { L_, "I1J1K1" },
            { L_, "M1N1O1" },
            { L_, "M1O1P1" },
            { L_, "M1O2P1" },
            { L_, "M1OuP1" },
            { L_, "M1OnP1" },
            { L_, "M1N1On" },
            { L_, "M1N1P1" },
            { L_, "A1A2AuAn" },
            { L_, "A1B2C1D2" },
            { L_, "E1F1G1H1" },
            { L_, "E1F1GnHu" },
            { L_, "I1J1K1L1" },
            { L_, "M1N1O1P1" },
            { L_, "M1N1O1Pu" },
            { L_, "M1N1OnP1" }
            // TBD: Add test cases
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;

            TestAllocator ta;

            const Layout *L;
            {
                Obj mX(PT, &ta); const Obj& X = mX;
                int rc = ggg(&mX, SPEC, &ta);
                LOOP_ASSERT(LINE, !rc);

                if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(X) };

                // With allocator specified
                for (int j = 0; j < 4; ++j) {
                    AggOption::AllocationStrategy strategy =
                                  (bdem_AggregateOption::AllocationStrategy) j;

                    TestAllocator testAllocator, da;
                    if (AggOption::BDEM_NODESTRUCT_FLAG & strategy) {
                        testAllocator.setQuiet(1);
                    }

                    bslma_DefaultAllocatorGuard dag(&da);

                    Obj mY(X.rowLayout(), strategy, &testAllocator);
                    const Obj& Y = mY;

                  BEGIN_BSLMA_EXCEPTION_TEST {

                    mY.replaceValues(X);

                    LOOP_ASSERT(LINE,    X == Y);
                    LOOP_ASSERT(LINE, ! (X != Y));
                    LOOP_ASSERT(LINE, 0 == da.numBytesInUse());

                  } END_BSLMA_EXCEPTION_TEST
                }

                // Without allocator specified
                for (int j = 0; j < 4; ++j) {
                    AggOption::AllocationStrategy strategy =
                                  (bdem_AggregateOption::AllocationStrategy) j;

                    TestAllocator testAllocator;
                    if (AggOption::BDEM_NODESTRUCT_FLAG & strategy) {
                        testAllocator.setQuiet(1);
                    }

                    bslma_DefaultAllocatorGuard dag(&testAllocator);

                    Obj mY(X.rowLayout(), strategy);
                    const Obj& Y = mY;

                  BEGIN_BSLMA_EXCEPTION_TEST {

                    mY.replaceValues(X);

                    LOOP_ASSERT(LINE,    X == Y);
                    LOOP_ASSERT(LINE, ! (X != Y));

                  } END_BSLMA_EXCEPTION_TEST

                    L = X.rowLayout();
                }

                L = X.rowLayout();
            }
            ta.deleteObjectRaw(L);
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // BDEX STREAMING
        //   Ensure that a 'bdem_RowData' can convert itself to/from a stream.
        //
        // Concerns:
        //
        // Plan:
        //
        // Tactics:
        //
        // Testing:
        //   int maxSupportedBdexVersion();
        //   STREAM& bdexStreamOutImp(stream, version, strmLookup) const;
        //   STREAM& bdexStreamInImp(s, v, strmLookup, attrLookup);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BDEX STREAMING" << endl
                                  << "==============" << endl;
        const int MAX_VERSION = 3;
        {
            ASSERT(MAX_VERSION == Obj::maxSupportedBdexVersion());
        }

        const struct {
            const int   d_line;    // line
            const char *d_spec;    // list spec
        } DATA[] = {

        // version    Out      In
        // -------    -----    -----
        //    1       unset    unset
        //            null     unset  (so * -> @ below)
        //
        //    2       unset    null   (so @ -> * below)
        //            null     null
        //
        //    3       unset    unset
        //            null     null

               // Line          Spec
               // ----          ----
            {      L_,          ""                                     },

            // One element
            {      L_,          "A1"                                    },
            {      L_,          "C2"                                    },
            {      L_,          "F1"                                    },
            {      L_,          "G2"                                    },
            {      L_,          "O1"                                    },
            {      L_,          "Q2"                                    },
            {      L_,          "R1"                                    },
//             {      L_,          "U2"                                    },
//             {      L_,          "V1"                                    },
            {      L_,          "W2"                                    },
            {      L_,          "b1"                                    },
//             {      L_,          "e2"                                    },
//             {      L_,          "f1"                                    },
            {      L_,          "Bu",                                  },
            {      L_,          "Bn",                                  },
            {      L_,          "Dn",                                  },

            // Two elements
            {      L_,          "A1B2"                                   },
            {      L_,          "CuD1"                                   },
            {      L_,          "E2Fn"                                   },
            {      L_,          "G1H1"                                   },
            {      L_,          "O2Mu"                                   },
            {      L_,          "QuPu"                                   },
            {      L_,          "RnSn"                                   },
            {      L_,          "Wucn"                                   },
            {      L_,          "anbn"                                   },
            {      L_,          "AnB1"                                  },
            {      L_,          "C2Du"                                  },
            {      L_,          "C1Dn"                                  },
            {      L_,          "aubu"                                 },
            {      L_,          "anbn"                                 },

            // Three elements
            {      L_,          "A1B2an"                                  },
            {      L_,          "C2D1du"                                  },
            {      L_,          "EuFnA1"                                  },
            {      L_,          "G1H2Fn"                                  },
            {      L_,          "O2M2Wu"                                  },
            {      L_,          "Q1P1Qn"                                  },
            {      L_,          "R2S2Ju"                                  },
            {      L_,          "W1cuAu"                                  },
            {      L_,          "a2bnDn"                                  },
            {      L_,          "aub1D2"                                 },
            {      L_,          "anb1D2"                                 },

            {      L_,        "A1BuCnD2EuFnG1H2InJuKuLnM1N2OnPuQ1"
                              "R2SnTuWuXnYnZ1a2b1cndn"       },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;

            TestAllocator ta;

            const Layout *L;
            {
                Obj mX(PT, &ta); const Obj& X = mX;
                int rc = ggg(&mX, SPEC, &ta);
                LOOP_ASSERT(LINE, !rc);

                if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(X) };

                for (int VERSION = 1; VERSION < 4; ++VERSION) {
                    bdex_TestOutStream os;
                    X.bdexStreamOutImp(
                       os,
                       VERSION,
                       streamOutAttrLookup<bdex_TestOutStream>::lookupTable());

                    // With allocator specified
                    for (int j = 0; j < 4; ++j) {
                        AggOption::AllocationStrategy strategy =
                                  (bdem_AggregateOption::AllocationStrategy) j;

                        TestAllocator testAllocator, da;
                        if (AggOption::BDEM_NODESTRUCT_FLAG & strategy) {
                            testAllocator.setQuiet(1);
                        }

                        bslma_DefaultAllocatorGuard dag(&da);

                        Obj mY(X.rowLayout(), strategy, &testAllocator);
                        const Obj& Y = mY;

                        bdex_TestInStream testInStream(os.data(), os.length());
                        testInStream.setSuppressVersionCheck(1);

                      BEGIN_BDEX_EXCEPTION_TEST {
                        testInStream.reset();
                        mY.bdexStreamInImp(
                          testInStream,
                          VERSION,
                          streamInAttrLookup<bdex_TestInStream>::lookupTable(),
                          DESC);

                        LOOP4_ASSERT(LINE, VERSION, X, Y,
                                     compareUsingVersion(X, Y, VERSION));
                     } END_BDEX_EXCEPTION_TEST
                    }
                }
                L = X.rowLayout();
            }
            ta.deleteObjectRaw(L);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'removeElement' functions
        //
        // Concerns:
        //   Test 'removeElement' functions
        //
        // Plan:
        //
        // Testing:
        //   void removeElement(int dstIndex);
        //   void removeElements(int dstIndex, int numElements);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'removeElement' functions\n"
                             "=================================\n";

        if (verbose) cout << "Removing one element\n"
                             "====================\n";

        {
            struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
             // Line     DescriptorSpec
             // ====     ==============
                { L_,       "A1" },
                { L_,       "B2" },
                { L_,       "C1" },
                { L_,       "D2" },
                { L_,       "Eu" },
                { L_,       "F2" },
                { L_,       "G1" },
                { L_,       "H2" },
                { L_,       "I1" },
                { L_,       "Ju" },
                { L_,       "K1" },
                { L_,       "L2" },
                { L_,       "M1" },
                { L_,       "N2" },
                { L_,       "Ou" },
                { L_,       "P2" },
                { L_,       "Q1" },
                { L_,       "R2" },
                { L_,       "S1" },
                { L_,       "Tu" },
                { L_,       "U1" },
                { L_,       "V2" },
                { L_,       "W1" },
                { L_,       "X2" },
                { L_,       "Yu" },
                { L_,       "Z2" },
                { L_,       "a1" },
                { L_,       "b2" },
                { L_,       "c1" },
                { L_,       "du" },
                { L_,       "e1" },
                { L_,       "f2" },

                // Testing various interesting combinations
                { L_,       "A1A2" },
                { L_,       "AuC2" },
                { L_,       "C2D1" },
                { L_,       "C2Eu" },
                { L_,       "F1G1" },
                { L_,       "GuG2" },
                { L_,       "G1H2" },
                { L_,       "M1Nu" },
                { L_,       "O1Pu" },
                { L_,       "P1Q2" },
                { L_,       "K1Q1" },
                { L_,       "S2T2" },
                { L_,       "W2Xu" },
                { L_,       "UuV2" },
                { L_,       "YuZu" },
                { L_,       "a1bu" },

                { L_,       "H1I2Ju" },
                { L_,       "KuL1M2" },
                { L_,       "R1S2Tu" },
                { L_,       "YuZ2a1" },

                { L_,       "B1L2RuWu" },
                { L_,       "D2GuNuQ1" },
                { L_,       "Q1R2SuTu" },

                { L_,       "JuHuK1H2SuKu" },
                { L_,       "RuI1S2X1L2SuW1" },
                { L_,       "M1XuK2ZuO1LuP1R2" },
                { L_,       "G1G1G2G2GuGuGuG1G1" },
                { L_,       "QuQuQ1Q2Q1Q2QuQuQ1Q2" },
                { L_,       "a1b2AuBuC1DuE1FuG1" },
                { L_,       "F1DuLuA1L1Aua1b2A1DuSuF1" },

                { L_,       "G1S2RuWuS1AuSuD1E2F1D2SuAu" },
                { L_,       "P1KuJuHuAuL1K1Gua1b2AuSuDuA1" },
                { L_,       "X1Z2SuAuEuW1R2P2O2I1J1L2KuauAuS1" },
                { L_,       "G1A2L1KuauK1A2S1J2DuKuS1W2E1I2Ou" },

                { L_,       "A1B2C1DuEuF1G2HuI1J2K1LuMuNuO1P1Q2R2S1"
                            "TuUuV1W2XuYuZ1aub1c1due1fu" }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                bslma_TestAllocator ta;
                const Layout *LX;
                {
                    Obj mX(PT, &ta); const Obj& X = mX;
                    int rc = ggg(&mX, SPEC, &ta);
                    LOOP_ASSERT(LINE, !rc);

                    LX = X.rowLayout();

                    if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(X) };

                    const int LEN = X.length();
                    for (int k = 0; k < LEN; ++k) {
                        Layout *LA;
                        {
                            Obj mA(PT, &ta); const Obj& A = mA;
                            rc = ggg(&mA, SPEC, &ta);
                            LOOP_ASSERT(LINE, !rc);

                            LA = (Layout *) A.rowLayout();

                            mA.removeElement(k);
                            LA->remove(k);

                            for (int ii = 0; ii < k; ++ii) {
                                LOOP_ASSERT(LINE, X.elemRef(ii) ==
                                                                A.elemRef(ii));
                            }
                            for (int ii = k; ii < LEN - k - 1; ++ii) {
                                LOOP_ASSERT(LINE, X.elemRef(ii + 1) ==
                                                                A.elemRef(ii));
                            }
                        }
                        ta.deleteObjectRaw(LA);
                    }

                    for(int si = 0; si < LEN; ++si) {
                        for (int ne = 0; ne < LEN - si - 1; ++ne) {
                            Layout *LA;
                            {
                                Obj mA(PT, &ta); const Obj& A = mA;
                                rc = ggg(&mA, SPEC, &ta);
                                LOOP_ASSERT(LINE, !rc);

                                LA = (Layout *) A.rowLayout();

                                mA.removeElements(si, ne);
                                LA->remove(si, ne);

                                for (int ii = 0; ii < si; ++ii) {
                                    LOOP_ASSERT(LINE, X.elemRef(ii) ==
                                                                A.elemRef(ii));
                                }
                                for (int ii = si;
                                     ii < LEN - si - ne - 1; ++ii) {
                                   LOOP_ASSERT(LINE, X.elemRef(si + ne + ii) ==
                                               A.elemRef(si + ii));
                                }
                            }
                            ta.deleteObjectRaw(LA);
                        }
                    }
                }
                ta.deleteObjectRaw(LX);
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'insertElement' functions
        //
        // Concerns:
        //   Test 'insertElement' functions
        //
        // Plan:
        //
        // Testing:
        //   void *insertElement(int dstIndex, const void *value);
        //   void *insertElementRaw(int dstIndex);
        //   void  insertElements(int dstIndex, srcRowData, si, ne);
        //   void *insertNullElement(int dstIndex);
        //   void  insertNullElements(int dstIndex, int numElements);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'insertElement' functions\n"
                             "=================================\n";

        if (verbose) cout << "Inserting one element\n"
                             "=====================\n";

        {
            struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
             // Line     DescriptorSpec
             // ====     ==============
                { L_,       "A1" },
                { L_,       "B2" },
                { L_,       "C1" },
                { L_,       "D2" },
                { L_,       "Eu" },
                { L_,       "F2" },
                { L_,       "G1" },
                { L_,       "H2" },
                { L_,       "I1" },
                { L_,       "Ju" },
                { L_,       "K1" },
                { L_,       "L2" },
                { L_,       "M1" },
                { L_,       "N2" },
                { L_,       "Ou" },
                { L_,       "P2" },
                { L_,       "Q1" },
                { L_,       "R2" },
                { L_,       "S1" },
                { L_,       "Tu" },
                { L_,       "U1" },
                { L_,       "V2" },
                { L_,       "W1" },
                { L_,       "X2" },
                { L_,       "Yu" },
                { L_,       "Z2" },
                { L_,       "a1" },
                { L_,       "b2" },
                { L_,       "c1" },
                { L_,       "du" },
                { L_,       "e1" },
                { L_,       "f2" },

                // Testing various interesting combinations
                { L_,       "A1A2" },
                { L_,       "AuC2" },
                { L_,       "C2D1" },
                { L_,       "C2Eu" },
                { L_,       "F1G1" },
                { L_,       "GuG2" },
                { L_,       "G1H2" },
                { L_,       "M1Nu" },
                { L_,       "O1Pu" },
                { L_,       "P1Q2" },
                { L_,       "K1Q1" },
                { L_,       "S2T2" },
                { L_,       "W2Xu" },
                { L_,       "UuV2" },
                { L_,       "YuZu" },
                { L_,       "a1bu" },

                { L_,       "H1I2Ju" },
                { L_,       "KuL1M2" },
                { L_,       "R1S2Tu" },
                { L_,       "YuZ2a1" },

                { L_,       "B1L2RuWu" },
                { L_,       "D2GuNuQ1" },
                { L_,       "Q1R2SuTu" },

                { L_,       "JuHuK1H2SuKu" },
                { L_,       "RuI1S2X1L2SuW1" },
                { L_,       "M1XuK2ZuO1LuP1R2" },
                { L_,       "G1G1G2G2GuGuGuG1G1" },
                { L_,       "QuQuQ1Q2Q1Q2QuQuQ1Q2" },
                { L_,       "a1b2AuBuC1DuE1FuG1" },
                { L_,       "F1DuLuA1L1Aua1b2A1DuSuF1" },

                { L_,       "G1S2RuWuS1AuSuD1E2F1D2SuAu" },
                { L_,       "P1KuJuHuAuL1K1Gua1b2AuSuDuA1" },
                { L_,       "X1Z2SuAuEuW1R2P2O2I1J1L2KuauAuS1" },
                { L_,       "G1A2L1KuauK1A2S1J2DuKuS1W2E1I2Ou" },

                { L_,       "A1B2C1DuEuF1G2HuI1J2K1LuMuNuO1P1Q2R2S1"
                            "TuUuV1W2XuYuZ1aub1c1due1fu" }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                bslma_TestAllocator ta;
                const Layout *LX;
                {
                    Obj mX(PT, &ta); const Obj& X = mX;
                    int rc = ggg(&mX, SPEC, &ta);
                    LOOP_ASSERT(LINE, !rc);

                    LX = X.rowLayout();

                    if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(X) };

                    // With allocator specified
                    for (int j = 0; j < 4; ++j) {
                        AggOption::AllocationStrategy strategy =
                                  (bdem_AggregateOption::AllocationStrategy) j;

                        const int LEN = X.length();
                        {
                            TestAllocator testAllocator;
                            if (AggOption::BDEM_NODESTRUCT_FLAG & strategy) {
                                testAllocator.setQuiet(1);
                            }

                            Layout mL;

                            Obj mA(&mL, strategy, &testAllocator);
                            const Obj& A = mA;
                            Obj mB(&mL, strategy, &testAllocator);
                            const Obj& B = mB;
                            Obj mC(&mL, strategy, &testAllocator);
                            const Obj& C = mC;

                            for (int k = 0; k < LEN; ++k) {
                                const Descriptor *DESC = (*LX)[k].attributes();
                                const void       *SRC  = X.elemData(k);

                                mL.append(DESC);

                              BEGIN_BSLMA_EXCEPTION_TEST {
                                void *VAL = mA.insertElement(k, SRC);

                                LOOP2_ASSERT(LINE, k, k + 1 == A.length());
                                LOOP2_ASSERT(LINE, k, !A.isNull(k));
                                LOOP2_ASSERT(LINE, k,
                                             compare(SRC, VAL, SPEC[2 * k]));
                              } END_BSLMA_EXCEPTION_TEST

                              BEGIN_BSLMA_EXCEPTION_TEST {
                                void *VAL = mB.insertElementRaw(k);
                                DESC->unsetConstruct(VAL,
                                                     strategy,
                                                     &testAllocator);
                                DESC->assign(VAL, SRC);

                                LOOP2_ASSERT(LINE, k, k + 1 == B.length());
                                LOOP2_ASSERT(LINE, k, !B.isNull(k));
                                LOOP2_ASSERT(LINE, k,
                                             compare(SRC, VAL, SPEC[2 * k]));
                              } END_BSLMA_EXCEPTION_TEST

                              BEGIN_BSLMA_EXCEPTION_TEST {
                                mC.insertNullElement(k);

                                LOOP2_ASSERT(LINE, k, k + 1 == C.length());
                                LOOP2_ASSERT(LINE, k, C.isNull(k));
                              } END_BSLMA_EXCEPTION_TEST
                            }

                            LOOP_ASSERT(LINE, A == X);
                            LOOP_ASSERT(LINE, B == X);
                        }
                    }
                }
                ta.deleteObjectRaw(LX);
            }
        }

        if (verbose) cout << "Inserting multiple elements\n"
                             "===========================\n";
        {
            struct {
                int         d_line;
                const char *d_spec;
            } DATA[] = {
             // Line     DescriptorSpec
             // ====     ==============
                { L_,       "" },
                { L_,       "A1" },
                { L_,       "G1" },
                { L_,       "N2" },
                { L_,       "P2" },
                { L_,       "Q1" },
                { L_,       "U1" },
                { L_,       "V2" },
                { L_,       "b2" },
                { L_,       "e1" },
                { L_,       "f2" },
                { L_,       "P1Q2" },
                { L_,       "UuV2" },

                { L_,       "H1I2Ju" },
                { L_,       "R1S2Tu" },

                { L_,       "D2GuNuQ1" },

                { L_,       "JuHuK1H2SuKu" },
                { L_,       "RuI1S2X1L2SuW1" },
                { L_,       "a1b2AuBuC1DuE1FuG1" },
                { L_,       "F1DuLuA1L1Aua1b2A1DuSuF1" },

//                 { L_,       "G1S2RuWuS1AuSuD1E2F1D2SuAu" },
//                 { L_,       "P1KuJuHuAuL1K1Gua1b2AuSuDuA1" },
//                 { L_,       "X1Z2SuAuEuW1R2P2O2I1J1L2KuauAuS1" },
//                 { L_,       "G1A2L1KuauK1A2S1J2DuKuS1W2E1I2Ou" },

//                 { L_,       "A1B2C1DuEuF1G2HuI1J2K1LuMuNuO1P1Q2R2S1"
//                             "TuUuV1W2XuYuZ1aub1c1due1fu" }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec;

                bslma_TestAllocator ta;
                const Layout *LX;
                {
                    Obj mX(PT, &ta); const Obj& X = mX;
                    int rc = ggg(&mX, SPEC, &ta);
                    LOOP_ASSERT(LINE, !rc);

                    LX = X.rowLayout();

                    if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(X) };

                    for (int j = 0; j < 4; ++j) {
                        const int LEN = X.length();
                        TestAllocator testAllocator;

                        for (int ni = 0; ni < NUM_DATA; ++ni) {
                            const int   LINE2 = DATA[i].d_line;
                            const char *SPEC2 = DATA[i].d_spec;
                            const int   DLEN   = bsl::strlen(SPEC2) / 2;
                            const int   SLEN   = X.length();

                            for (int di = 0; di < DLEN; ++di) {
                                for (int si = 0; si < SLEN; ++si) {
                                    for (int ne = 0; ne < SLEN -si - 1; ++ne) {
                                        Layout *LC, *LD, *LE;
                                        {
                                            Obj mC(PT, &testAllocator);
                                            const Obj& C = mC;
                                            Obj mD(PT, &testAllocator);
                                            const Obj& D = mD;
                                            Obj mE(PT, &testAllocator);
                                            const Obj& E = mE;

                                            int rc = ggg(&mC,
                                                         SPEC2,
                                                         &testAllocator);
                                            LOOP_ASSERT(LINE, !rc);
                                            rc = ggg(&mD,
                                                     SPEC2,
                                                     &testAllocator);
                                            LOOP_ASSERT(LINE, !rc);
                                            rc = ggg(&mE,
                                                     SPEC2,
                                                     &testAllocator);
                                            LOOP_ASSERT(LINE, !rc);

                                            LC = (Layout *) C.rowLayout();
                                            LD = (Layout *) D.rowLayout();
                                            LE = (Layout *) E.rowLayout();

                                            if (veryVerbose) { T_ P_(LINE)
                                                              P_(LINE2) P(C)
                                                              P_(SPEC2) P(D) };

                                            LD->insert(di, *LX, si, ne);
                                            LE->insert(di, *LX, si, ne);

                                          BEGIN_BSLMA_EXCEPTION_TEST {
                                            mD.insertElements(di, X, si, ne);

                                            LOOP_ASSERT(LINE2,
                                                      DLEN + ne == D.length());

                                            for (int ii = 0; ii < di; ++ii) {
                                                LOOP_ASSERT(LINE2,
                                                            C.elemRef(ii) ==
                                                                D.elemRef(ii));
                                            }
                                            for (int ii = 0; ii < ne; ++ii) {
                                                LOOP_ASSERT(LINE2,
                                                          X.elemRef(si + ii) ==
                                                           D.elemRef(di + ii));
                                            }

                                            const int REM = C.length() - di;

                                            for (int ii = 0;
                                                 ii < REM; ++ii) {
                                                LOOP3_ASSERT(LINE2, C, D,
                                                      C.elemRef(di + ii) ==
                                                  D.elemRef(di + ne + ii));
                                            }
                                          } END_BSLMA_EXCEPTION_TEST

                                          BEGIN_BSLMA_EXCEPTION_TEST {
                                            mE.insertNullElements(di, ne);
                                            LOOP_ASSERT(LINE2,
                                                      DLEN + ne == E.length());
                                            for (int ii = 0; ii < di; ++ii) {
                                                LOOP_ASSERT(LINE2,
                                                            C.elemRef(ii) ==
                                                                D.elemRef(ii));
                                            }
                                            for (int ii = 0; ii < ne; ++ii) {
                                                LOOP_ASSERT(LINE2,
                                                       E.elemRef(di).isNull());
                                            }
                                            const int REM2 = C.length() - di;

                                            for (int ii = 0;
                                                 ii < REM2; ++ii) {
                                                LOOP3_ASSERT(LINE2, C, D,
                                                      C.elemRef(di + ii) ==
                                                  D.elemRef(di + ne + ii));
                                            }
                                          } END_BSLMA_EXCEPTION_TEST
                                        }
                                        testAllocator.deleteObjectRaw(LC);
                                        testAllocator.deleteObjectRaw(LD);
                                        testAllocator.deleteObjectRaw(LE);
                                    }
                                }
                            }
                        }
                    }
                }
                ta.deleteObjectRaw(LX);
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'makeNull' and 'makeAllNull'
        //
        // Concerns:
        //   Test 'makeNull' and 'makeAllNull'.
        //
        // Plan:
        //
        // Testing:
        //   void makeNull(int index);
        //   void makeAllNull();
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'makeNull' and 'makeAllNull'\n"
                             "====================================\n";

        struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
             // Line     DescriptorSpec
             // ====     ==============
                { L_,       "A1" },
                { L_,       "B2" },
                { L_,       "C1" },
                { L_,       "D2" },
                { L_,       "Eu" },
                { L_,       "F2" },
                { L_,       "G1" },
                { L_,       "H2" },
                { L_,       "I1" },
                { L_,       "Ju" },
                { L_,       "K1" },
                { L_,       "L2" },
                { L_,       "M1" },
                { L_,       "N2" },
                { L_,       "Ou" },
                { L_,       "P2" },
                { L_,       "Q1" },
                { L_,       "R2" },
                { L_,       "S1" },
                { L_,       "Tu" },
                { L_,       "U1" },
                { L_,       "V2" },
                { L_,       "W1" },
                { L_,       "X2" },
                { L_,       "Yu" },
                { L_,       "Z2" },
                { L_,       "a1" },
                { L_,       "b2" },
                { L_,       "c1" },
                { L_,       "du" },
                { L_,       "e1" },
                { L_,       "f2" },

                // Testing various interesting combinations
                { L_,       "A1A2" },
                { L_,       "AuC2" },
                { L_,       "C2D1" },
                { L_,       "C2Eu" },
                { L_,       "F1G1" },
                { L_,       "GuG2" },
                { L_,       "G1H2" },
                { L_,       "M1Nu" },
                { L_,       "O1Pu" },
                { L_,       "P1Q2" },
                { L_,       "K1Q1" },
                { L_,       "S2T2" },
                { L_,       "W2Xu" },
                { L_,       "UuV2" },
                { L_,       "YuZu" },
                { L_,       "a1bu" },

                { L_,       "H1I2Ju" },
                { L_,       "KuL1M2" },
                { L_,       "R1S2Tu" },
                { L_,       "YuZ2a1" },

                { L_,       "B1L2RuWu" },
                { L_,       "D2GuNuQ1" },
                { L_,       "Q1R2SuTu" },

                { L_,       "JuHuK1H2SuKu" },
                { L_,       "RuI1S2X1L2SuW1" },
                { L_,       "M1XuK2ZuO1LuP1R2" },
                { L_,       "G1G1G2G2GuGuGuG1G1" },
                { L_,       "QuQuQ1Q2Q1Q2QuQuQ1Q2" },
                { L_,       "a1b2AuBuC1DuE1FuG1" },
                { L_,       "F1DuLuA1L1Aua1b2A1DuSuF1" },

                { L_,       "G1S2RuWuS1AuSuD1E2F1D2SuAu" },
                { L_,       "P1KuJuHuAuL1K1Gua1b2AuSuDuA1" },
                { L_,       "X1Z2SuAuEuW1R2P2O2I1J1L2KuauAuS1" },
                { L_,       "G1A2L1KuauK1A2S1J2DuKuS1W2E1I2Ou" },

                { L_,       "A1B2C1DuEuF1G2HuI1J2K1LuMuNuO1P1Q2R2S1"
                            "TuUuV1W2XuYuZ1aub1c1due1fu" }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;
            const int   SLEN = bsl::strlen(SPEC) / 2;

            // With allocator specified
            for (int j = 0; j < 4; ++j) {
                AggOption::AllocationStrategy strategy =
                                  (bdem_AggregateOption::AllocationStrategy) j;

                TestAllocator ta;
                const Layout *LA, *LB;

                if (AggOption::BDEM_NODESTRUCT_FLAG & strategy) {
                    ta.setQuiet(1);
                }

                {
                    Obj mA(strategy, &ta);
                    Obj mB(strategy, &ta);
                    const Obj& A = mA;
                    const Obj& B = mB;
                    int rc = ggg(&mA, SPEC, &ta);
                    LOOP_ASSERT(LINE, !rc);
                    rc = ggg(&mB, SPEC, &ta);
                    LOOP_ASSERT(LINE, !rc);

                    if (veryVerbose) { T_ P_(LINE) P_(SPEC)
                                           P_(A) P(B) };

                    LA = A.rowLayout();
                    LB = B.rowLayout();

                    LOOP_ASSERT(LINE, A == B);

                    for (int k = 0; k < SLEN; ++k) {
                        LOOP2_ASSERT(LINE, k, !A.isNull(k));

                        mA.makeNull(k);

                        LOOP2_ASSERT(LINE, k, A.isNull(k));
                        LOOP2_ASSERT(LINE, k, isUnset(mA.elemRef(k)));
                        LOOP2_ASSERT(LINE, k, A != B);
                    }

                    mB.makeAllNull();

                    LOOP_ASSERT(LINE, A == B);
                }

                ta.deleteObjectRaw(LA);
                ta.deleteObjectRaw(LB);
            }

            // Without allocator specified
            for (int j = 0; j < 4; ++j) {
                AggOption::AllocationStrategy strategy =
                                  (bdem_AggregateOption::AllocationStrategy) j;

                TestAllocator ta, da;
                const Layout *LA, *LB;

                if (AggOption::BDEM_NODESTRUCT_FLAG & strategy) {
                    ta.setQuiet(1);
                    da.setQuiet(1);
                }

                {
                    bslma_DefaultAllocatorGuard dag(&da);

                    Obj mA(strategy);
                    Obj mB(strategy);
                    const Obj& A = mA;
                    const Obj& B = mB;
                    int rc = ggg(&mA, SPEC, &ta);
                    LOOP_ASSERT(LINE, !rc);
                    rc = ggg(&mB, SPEC, &ta);
                    LOOP_ASSERT(LINE, !rc);

                    if (veryVerbose) { T_ P_(LINE) P_(SPEC)
                                           P_(A) P(B) };

                    LA = A.rowLayout();
                    LB = B.rowLayout();

                    LOOP_ASSERT(LINE, A == B);

                    for (int k = 0; k < SLEN; ++k) {
                        LOOP2_ASSERT(LINE, k, !A.isNull(k));

                        mA.makeNull(k);

                        LOOP2_ASSERT(LINE, k, A.isNull(k));
                        LOOP2_ASSERT(LINE, k, isUnset(mA.elemRef(k)));
                        LOOP2_ASSERT(LINE, k, A != B);
                    }

                    mB.makeAllNull();

                    LOOP_ASSERT(LINE, A == B);
                }

                ta.deleteObjectRaw(LA);
                ta.deleteObjectRaw(LB);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'reset' FUNCTIONS
        //
        // Concerns:
        //   Test 'reset' functions.
        //
        // Plan:
        //
        // Testing:
        //   void reset();
        //   void reset(rowLayout);
        //   void reset(rowLayout, rowdata);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'reset' functions\n"
                             "=========================\n";

        struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            { L_, "" },
            { L_, "A1" },
            { L_, "A2" },
            { L_, "Au" },
            { L_, "An" },
            { L_, "F1" },
            { L_, "F2" },
            { L_, "Fu" },
            { L_, "Fn" },
            { L_, "G1" },
            { L_, "G2" },
            { L_, "Gu" },
            { L_, "Gn" },
            { L_, "A1A2A1" },
            { L_, "A1A2A2" },
            { L_, "A1A2Au" },
            { L_, "A1A2An" },
            { L_, "A1B2C1" },
            { L_, "E1F1G1" },
            { L_, "I1J1K1" },
            { L_, "M1N1O1" },
            { L_, "M1O1P1" },
            { L_, "M1O2P1" },
            { L_, "M1OuP1" },
            { L_, "M1OnP1" },
            { L_, "M1N1On" },
            { L_, "M1N1P1" },
            { L_, "A1A2AuAn" },
            { L_, "A1B2C1D2" },
            { L_, "E1F1G1H1" },
            { L_, "E1F1GnHu" },
            { L_, "I1J1K1L1" },
            { L_, "M1N1O1P1" },
            { L_, "M1N1O1Pu" },
            { L_, "M1N1OnP1" }
            // TBD: Add test cases
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE1 = DATA[i].d_line;
            const char *SPEC1 = DATA[i].d_spec;

            TestAllocator ta;
            const Layout *LX;
            {
                Obj mX(PT, &ta); const Obj& X = mX;
                int rc = ggg(&mX, SPEC1, &ta);
                LOOP_ASSERT(LINE1, !rc);

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int   LINE2 = DATA[j].d_line;
                    const char *SPEC2 = DATA[j].d_spec;

                    const Layout *LA, *LB, *LC;
                    {
                        TestAllocator testAllocator(veryVeryVerbose), da;

                        // With allocator specified
                        for (int k = 0; k < 4; ++k) {
                            AggOption::AllocationStrategy strategy =
                                  (bdem_AggregateOption::AllocationStrategy) j;

                            if (AggOption::BDEM_NODESTRUCT_FLAG & strategy) {
                                testAllocator.setQuiet(1);
                            }

                            bslma_DefaultAllocatorGuard dag(&da);

                            Obj mA(strategy, &testAllocator);
                            Obj mB(strategy, &testAllocator);
                            Obj mC(strategy, &testAllocator);
                            const Obj& A = mA;
                            const Obj& B = mB;
                            const Obj& C = mC;
                            rc = ggg(&mA, SPEC2, &testAllocator);
                            LOOP_ASSERT(LINE2, !rc);
                            rc = ggg(&mB, SPEC2, &testAllocator);
                            LOOP_ASSERT(LINE2, !rc);
                            rc = ggg(&mC, SPEC2, &testAllocator);
                            LOOP_ASSERT(LINE2, !rc);

                            LOOP_ASSERT(LINE2, 0 == da.numBytesInUse());

                            if (veryVerbose) { T_ P_(LINE1) P_(LINE2)
                                               P_(SPEC1) P_(SPEC2) P(X) P(A) };

                            LA = A.rowLayout();
                            LB = B.rowLayout();
                            LC = C.rowLayout();

                          BEGIN_BSLMA_EXCEPTION_TEST {
                            mA.reset();
                            LOOP2_ASSERT(LINE1, LINE2, 0 == A.rowLayout());
                            LOOP_ASSERT(LINE2, 0 == da.numBytesInUse());
                          } END_BSLMA_EXCEPTION_TEST

                          BEGIN_BSLMA_EXCEPTION_TEST {
                            mB.reset(X.rowLayout());
                            LOOP2_ASSERT(LINE1, LINE2,
                                         X.rowLayout() == B.rowLayout());
                            LOOP2_ASSERT(LINE1, LINE2,
                                         0 == da.numBytesInUse());
                            const int len = X.length();
                            for (int ei = 0; ei < len; ++ei) {
                                LOOP2_ASSERT(LINE1, LINE2,
                                             0 == da.numBytesInUse());
                                LOOP2_ASSERT(LINE1, LINE2,
                                 X.elemRef(ei).type() == B.elemRef(ei).type());
                                LOOP2_ASSERT(LINE1, LINE2, B.isNull(ei));
                                LOOP2_ASSERT(LINE1, LINE2,
                                             isUnset(B.elemRef(ei)));
                            }
                          } END_BSLMA_EXCEPTION_TEST

                          BEGIN_BSLMA_EXCEPTION_TEST {
                            mC.reset(X.rowLayout(), X);
                            LOOP2_ASSERT(LINE1, LINE2,
                                         X.rowLayout() == C.rowLayout());
                            LOOP2_ASSERT(LINE1, LINE2, C == X);
                            LOOP2_ASSERT(LINE1, LINE2,
                                         0 == da.numBytesInUse());
                          } END_BSLMA_EXCEPTION_TEST

                          testAllocator.deleteObjectRaw(LA);
                          testAllocator.deleteObjectRaw(LB);
                          testAllocator.deleteObjectRaw(LC);
                        }
                    }
                }
                LX = X.rowLayout();
            }
            ta.deleteObjectRaw(LX);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        //   Test assignment operator.
        //
        // Plan:
        //
        // Testing:
        //   bdem_RowData& operator=(const bdem_RowData& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Assignment Operator\n"
                             "===========================\n";

        struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            { L_, "" },
            { L_, "A1" },
            { L_, "A2" },
            { L_, "Au" },
            { L_, "An" },
            { L_, "F1" },
            { L_, "F2" },
            { L_, "Fu" },
            { L_, "Fn" },
            { L_, "G1" },
            { L_, "G2" },
            { L_, "Gu" },
            { L_, "Gn" },
            { L_, "A1A2A1" },
            { L_, "A1A2A2" },
            { L_, "A1A2Au" },
            { L_, "A1A2An" },
            { L_, "A1B2C1" },
            { L_, "E1F1G1" },
            { L_, "I1J1K1" },
            { L_, "M1N1O1" },
            { L_, "M1O1P1" },
            { L_, "M1O2P1" },
            { L_, "M1OuP1" },
            { L_, "M1OnP1" },
            { L_, "M1N1On" },
            { L_, "M1N1P1" },
            { L_, "A1A2AuAn" },
            { L_, "A1B2C1D2" },
            { L_, "E1F1G1H1" },
            { L_, "E1F1GnHu" },
            { L_, "I1J1K1L1" },
            { L_, "M1N1O1P1" },
            { L_, "M1N1O1Pu" },
            { L_, "M1N1OnP1" }
            // TBD: Add test cases
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;

            TestAllocator ta;

            const Layout *L;
            {
                Obj mX(PT, &ta); const Obj& X = mX;
                int rc = ggg(&mX, SPEC, &ta);
                LOOP_ASSERT(LINE, !rc);

                if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(X) };

                // With allocator specified
                for (int j = 0; j < 4; ++j) {
                    AggOption::AllocationStrategy strategy =
                                  (bdem_AggregateOption::AllocationStrategy) j;

                    TestAllocator testAllocator, da;
                    if (AggOption::BDEM_NODESTRUCT_FLAG & strategy) {
                        testAllocator.setQuiet(1);
                    }

                    bslma_DefaultAllocatorGuard dag(&da);

                    Obj mY(X.rowLayout(), strategy, &testAllocator);
                    const Obj& Y = mY;

                  BEGIN_BSLMA_EXCEPTION_TEST {

                    mX = Y;

                    LOOP_ASSERT(LINE,    X == Y);
                    LOOP_ASSERT(LINE, ! (X != Y));
                    LOOP_ASSERT(LINE, 0 == da.numBytesInUse());

                  } END_BSLMA_EXCEPTION_TEST
                }

                // Without allocator specified
                for (int j = 0; j < 4; ++j) {
                    AggOption::AllocationStrategy strategy =
                                  (bdem_AggregateOption::AllocationStrategy) j;

                    TestAllocator testAllocator;
                    if (AggOption::BDEM_NODESTRUCT_FLAG & strategy) {
                        testAllocator.setQuiet(1);
                    }

                    bslma_DefaultAllocatorGuard dag(&testAllocator);

                    Obj mY(X.rowLayout(), strategy);
                    const Obj& Y = mY;

                  BEGIN_BSLMA_EXCEPTION_TEST {

                    mX = Y;

                    LOOP_ASSERT(LINE,    X == Y);
                    LOOP_ASSERT(LINE, ! (X != Y));

                  } END_BSLMA_EXCEPTION_TEST

                    L = X.rowLayout();
                }

                L = X.rowLayout();
            }
            ta.deleteObjectRaw(L);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   bdem_RowData(rowLayout, original, allocMode, *allocator);
        //   bdem_RowData(rowLayout, original, si, ne, allocMode, *allocator);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Copy Constructor\n"
                             "========================\n";

        struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            { L_, "" },
            { L_, "A1" },
            { L_, "A2" },
            { L_, "Au" },
            { L_, "An" },
            { L_, "F1" },
            { L_, "F2" },
            { L_, "Fu" },
            { L_, "Fn" },
            { L_, "G1" },
            { L_, "G2" },
            { L_, "Gu" },
            { L_, "Gn" },
            { L_, "A1A2A1" },
            { L_, "A1A2A2" },
            { L_, "A1A2Au" },
            { L_, "A1A2An" },
            { L_, "A1B2C1" },
            { L_, "E1F1G1" },
            { L_, "I1J1K1" },
            { L_, "M1N1O1" },
            { L_, "M1O1P1" },
            { L_, "M1O2P1" },
            { L_, "M1OuP1" },
            { L_, "M1OnP1" },
            { L_, "M1N1On" },
            { L_, "M1N1P1" },
            { L_, "A1A2AuAn" },
            { L_, "A1B2C1D2" },
            { L_, "E1F1G1H1" },
            { L_, "E1F1GnHu" },
            { L_, "I1J1K1L1" },
            { L_, "M1N1O1P1" },
            { L_, "M1N1O1Pu" },
            { L_, "M1N1OnP1" }
            // TBD: Add test cases
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;

            TestAllocator ta;
            const Layout *L;
            {
                Obj mX(PT, &ta); const Obj& X = mX;
                int rc = ggg(&mX, SPEC, &ta);
                LOOP_ASSERT(LINE, !rc);
                L = X.rowLayout();

                // With allocator specified
                for (int j = 0; j < 4; ++j) {
                    AggOption::AllocationStrategy strategy =
                                  (bdem_AggregateOption::AllocationStrategy) j;

                    TestAllocator testAllocator, da;
                    if (AggOption::BDEM_NODESTRUCT_FLAG & strategy) {
                        testAllocator.setQuiet(1);
                    }

                    bslma_DefaultAllocatorGuard dag(&da);

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mY(L, X, strategy, &testAllocator); const Obj& Y = mY;

                    if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(X) P(Y) };

                    LOOP_ASSERT(LINE,    X == Y);
                    LOOP_ASSERT(LINE, ! (X != Y));
                    LOOP_ASSERT(LINE, L == Y.rowLayout());
                    LOOP_ASSERT(LINE, 0 == da.numBytesInUse());

                  } END_BSLMA_EXCEPTION_TEST
                }

                // Without allocator specified
                for (int j = 0; j < 4; ++j) {
                    AggOption::AllocationStrategy strategy =
                                  (bdem_AggregateOption::AllocationStrategy) j;

                    TestAllocator testAllocator;
                    if (AggOption::BDEM_NODESTRUCT_FLAG & strategy) {
                        testAllocator.setQuiet(1);
                    }

                    bslma_DefaultAllocatorGuard dag(&testAllocator);

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mY(L, X, strategy); const Obj& Y = mY;

                    if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(X) P(Y) };

                    LOOP_ASSERT(LINE,    X == Y);
                    LOOP_ASSERT(LINE, ! (X != Y));
                    LOOP_ASSERT(LINE, L == Y.rowLayout());

                  } END_BSLMA_EXCEPTION_TEST
                }
                L = X.rowLayout();
            }
            ta.deleteObjectRaw(L);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //
        // Concerns:
        //   Test equality operators.
        //
        // Plan:
        //
        // Testing:
        //   bool operator==(const bdem_RowData& lhs,
        //                   const bdem_RowData& rhs);
        //   bool operator!=(const bdem_RowData& lhs,
        //                   const bdem_RowData& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Equality Operators\n"
                             "==========================\n";

        struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            { L_, "" },
            { L_, "A1" },
            { L_, "A2" },
            { L_, "Au" },
            { L_, "An" },
            { L_, "F1" },
            { L_, "F2" },
            { L_, "Fu" },
            { L_, "Fn" },
            { L_, "G1" },
            { L_, "G2" },
            { L_, "Gu" },
            { L_, "Gn" },
            { L_, "A1A2A1" },
            { L_, "A1A2A2" },
            { L_, "A1A2Au" },
            { L_, "A1A2An" },
            { L_, "A1B2C1" },
            { L_, "E1F1G1" },
            { L_, "I1J1K1" },
            { L_, "M1N1O1" },
            { L_, "M1O1P1" },
            { L_, "M1O2P1" },
            { L_, "M1OuP1" },
            { L_, "M1OnP1" },
            { L_, "M1N1On" },
            { L_, "M1N1P1" },
            { L_, "A1A2AuAn" },
            { L_, "A1B2C1D2" },
            { L_, "E1F1G1H1" },
            { L_, "E1F1GnHu" },
            { L_, "I1J1K1L1" },
            { L_, "M1N1O1P1" },
            { L_, "M1N1O1Pu" },
            { L_, "M1N1OnP1" }
            // TBD: Add test cases
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE1 = DATA[i].d_line;
            const char *SPEC1 = DATA[i].d_spec;

            TestAllocator ta;
            const Layout *L1;
            {
                Obj mX(PT, &ta); const Obj& X = mX;
                int rc = ggg(&mX, SPEC1, &ta);
                LOOP_ASSERT(LINE1, !rc);

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int   LINE2 = DATA[j].d_line;
                    const char *SPEC2 = DATA[j].d_spec;

                    const Layout *L2;
                    {
                        Obj mY(PT, &ta); const Obj& Y = mY;
                        rc = ggg(&mY, SPEC2, &ta);
                        LOOP_ASSERT(LINE2, !rc);

                        if (veryVerbose) { T_ P_(LINE1) P_(LINE2)
                                              P_(SPEC1) P_(SPEC2) P(X) P(Y) };

                        if (i == j) {
                            LOOP_ASSERT(LINE1,    X == Y);
                            LOOP_ASSERT(LINE1, ! (X != Y));
                        }
                        else {
                            LOOP_ASSERT(LINE1,    X != Y);
                            LOOP_ASSERT(LINE1, ! (X == Y));
                        }

                        L2 = Y.rowLayout();
                    }
                    ta.deleteObjectRaw(L2);
                }
                L1 = X.rowLayout();
            }
            ta.deleteObjectRaw(L1);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CORRECTNESS OF 'printRow'
        //
        // Concerns:
        //   That 'printRow' correctly prints the types and contents of a
        //   'bdem_RowData'.
        //
        // Plan:
        //   Create a variety of types and values and print them out, verify
        //   printing of all 4 test values of all types up to ET::BDEM_TIME,
        //   except for "A@", the unset char, which, when printed, prints a
        //   null char to the buffer, which is problematic with the test
        //   apparatus.
        //
        // Testing:
        //   bsl::ostream& printRow(stream, level, spl, labels) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'printRow'\n"
                             "==================\n";

        struct {
            int         d_line;
            const char *d_spec;
            const char *d_expected;
        } DATA[] = {
            { L_, "",             " { }" },
            { L_, "A1",           " { CHAR A }" },
            { L_, "G1",           " { STRING one }" },
            { L_, "C1G2",         " { INT 10 STRING two }" },
            { L_, "A1A2C1An",     " { CHAR A CHAR B INT 10 CHAR NULL }" },
            { L_, "A1B2C1D2",     " { CHAR A SHORT -2 INT 10 INT64 -200 }" },
            { L_, "E1F1GnH1",     " { FLOAT -1.5 DOUBLE 10.5 STRING NULL"
                                  " DATETIME 01JAN2000_00:01:02.003 }" },
            { L_, "A1JuC1An",     " { CHAR A TIME 24:00:00.000 INT 10"
                                  " CHAR NULL }" },
            { L_, "A2E1C1",       " { CHAR B FLOAT -1.5 INT 10 }" },
            { L_, "A1F2C1C2FnH1", " { CHAR A DOUBLE 20.5 INT 10 INT 20"
                                  " DOUBLE NULL"
                                  " DATETIME 01JAN2000_00:01:02.003 }" },
            { L_, "A2A1GuAn",     " { CHAR B CHAR A STRING  CHAR NULL }" },
            { L_, "A1BnC1",       " { CHAR A SHORT NULL INT 10 }" },
            { L_, "C1D2HnJnGu",   " { INT 10 INT64 -200 DATETIME NULL"
                                  " TIME NULL STRING  }" },
            { L_, "E1F1G2",       " { FLOAT -1.5 DOUBLE 10.5 STRING two }" },
            { L_, "E1F1G1H1",     " { FLOAT -1.5 DOUBLE 10.5 STRING one"
                                  " DATETIME 01JAN2000_00:01:02.003 }" },
            { L_, "A1B1C1",       " { CHAR A SHORT -1 INT 10 }" },
            { L_, "D1E1F1",       " { INT64 -100 FLOAT -1.5 DOUBLE 10.5 }" },
            { L_, "G1H1I1",       " { STRING one"
                                  " DATETIME 01JAN2000_00:01:02.003"
                                  " DATE 01JAN2000 }" },
            { L_, "J1",           " { TIME 00:01:02.003 }" },
            { L_, "A2B2C2",       " { CHAR B SHORT -2 INT 20 }" },
            { L_, "D2E2F2",       " { INT64 -200 FLOAT -2.5 DOUBLE 20.5 }" },
            { L_, "G2H2I2",       " { STRING two"
                                  " DATETIME 31DEC9999_04:05:06.789"
                                  " DATE 31DEC9999 }" },
            { L_, "J2",           " { TIME 04:05:06.789 }" },
            { L_, "BuCu",         " { SHORT -32768 INT -2147483648 }" },
#ifdef BSLS_PLATFORM__CMP_MSVC
            { L_, "DuEuFu",       " { INT64 -9223372036854775808"
                                  " FLOAT -6.56419e-015"
                                  " DOUBLE -2.42454e-014 }" },
#else
            { L_, "DuEuFu",       " { INT64 -9223372036854775808"
                                  " FLOAT -6.56419e-15"
                                  " DOUBLE -2.42454e-14 }" },
#endif
            { L_, "GuHuIu",       " { STRING  DATETIME 01JAN0001_24:00:00.000"
                                  " DATE 01JAN0001 }" },
            { L_, "Ju",           " { TIME 24:00:00.000 }" },
            { L_, "AnBnCn",       " { CHAR NULL SHORT NULL INT NULL }" },
            { L_, "DnEnFn",       " { INT64 NULL FLOAT NULL DOUBLE NULL }" },
            { L_, "GnHnIn",       " { STRING NULL DATETIME NULL DATE NULL }" },
            { L_, "Jn",           " { TIME NULL }" }

            // TBD: Add level and spl test cases
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE     = DATA[i].d_line;
            const char *SPEC     = DATA[i].d_spec;
            const char *EXPECTED = DATA[i].d_expected;

            TestAllocator ta;
            const Layout *L;
            {
                Obj mX(PT, &ta); const Obj& X = mX;
                int rc = ggg(&mX, SPEC, &ta);

                LOOP_ASSERT(LINE, !rc);

                if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(X) };

                const int BUF_LEN = 4096;
                char buf[BUF_LEN];
                ostrstream out(buf, BUF_LEN);
                X.printRow(out, 1, -1) << ends;
                LOOP_ASSERT(LINE, '}' == buf[strlen(buf) - 1]);

#if 0
            // Code used to print strings in source above.  Keep it around
            // in case we want to print some more someday.
            cout << "            { L_, \"" << SPEC << "\", \"" << buf <<
                                                                     "\" },\n";
#else
                LOOP_ASSERT(LINE, !strcmp(buf, EXPECTED));
#endif
                L = X.rowLayout();
            }
            ta.deleteObjectRaw(L);
        }
      } break;
      case 4: {
        //--------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS (BOOTSTRAP) AND PRIMARY ACCESSORS:
        // TBD:
        // Concerns:
        //   1. The 2-argument 'bdem_RowData' constructor works properly:
        //      a. The initial value is correct.
        //      b. The constructor is exception neutral w.r.t. memory
        //         allocation.
        //      c. The internal memory management system is hooked up properly
        //         so that *all* internally allocated memory draws from a
        //         user-supplied allocator whenever one is specified.
        //      d. The string usage hint, pooling option, and memory allocator
        //         are properly recorded in the object.
        //   2. The destructor works properly as implicitly tested in the
        //      various scopes of this test and in the presence of exceptions.
        //   3. The (non-const) 'columnTypes' manipulator works properly.
        //
        // Plan:
        //   Create a test object using the 3-argument constructor: 1) without
        //   exceptions and 2) in the presence of exceptions during memory
        //   allocations using a 'bslma_TestAllocator' and varying its
        //   *allocation* *limit*.  In a loop, obtain a reference to the
        //   modifiable offset-index map held by the object using the
        //   'columnTypes' method.   Append an element to the map in each
        //   iteration and verify that the object has the expected value.
        //   When the object goes out of scope, verify that the destructor
        //   properly deallocates all memory that had been allocated to it.
        //
        //   Concern 1d is addressed by applying the suite of direct
        //   accessors to the test objects.
        //
        //   White-Box Testing: Repeat the constructor test initially
        //   specifying no allocator and again specifying a static buffer
        //   allocator.  These test for rudimentary correct object behavior
        //   via the destructor and Purify.  Note: When tested stand-alone,
        //   'bdem_RowData' works correctly when passed a null allocator.
        //   However, clients are expected to instantiate 'bdem_RowData'
        //   with a non-null allocator.
        //
        // Testing:
        //   bdem_RowData(strategy, bslma_Allocator *ba);
        //   bdem_RowData(layout, strategy, bslma_Allocator *ba);
        //   ~bdem_RowData();
        //   const void *elemData(int index) const;
        //   void *elemData(int index);
        //   void setValue(int index, const void *value);
        //   bdem_ConstElemRef elemRef(int index) const;
        //   bdem_ElemRef elemRef(int index);
        //   bool isAnyInRangeNonNull(int startIndex, int numElements) const;
        //   bool isAnyInRangeNull(int startIndex, int numElements) const;
        //   bool isNull(int index) const;
        //   int length() const;
        //   const bdem_RowLayout *rowLayout() const;
        //---------------------------------------------------------------------

        if (verbose) cout << "Testing Primary Manipulators and Accessors\n"
                             "==========================================\n";

        if (verbose) cout
            << "\nTesting 'bdem_RowData(option, ba)' ctor" << endl;

        {
            TestAllocator ta, da;
            bslma_DefaultAllocatorGuard dag(&da);

            for (int i = 0; i < 4; ++i) {
                AggOption::AllocationStrategy strategy =
                                             (AggOption::AllocationStrategy) i;

                Obj mX(strategy, &ta); const Obj &X = mX;

                LOOP_ASSERT(i, 0 == X.rowLayout());
                LOOP_ASSERT(i, 0 == da.numBlocksInUse());
                LOOP_ASSERT(i, 0 == da.numBytesInUse());
                LOOP_ASSERT(i, 0 == ta.numBlocksInUse());
                LOOP_ASSERT(i, 0 == ta.numBytesInUse());
            }
            {
                for (int i = 0; i < 4; ++i) {
                    AggOption::AllocationStrategy strategy =
                                             (AggOption::AllocationStrategy) i;

                    Obj mX(strategy); const Obj &X = mX;

                    LOOP_ASSERT(i, 0 == X.rowLayout());
                    LOOP_ASSERT(i, 0 == da.numBlocksInUse());
                    LOOP_ASSERT(i, 0 == da.numBytesInUse());
                    LOOP_ASSERT(i, 0 == ta.numBlocksInUse());
                    LOOP_ASSERT(i, 0 == ta.numBytesInUse());
                }
            }
        }

        if (verbose) cout
            << "\nTesting 'bdem_RowData(layout, option, ba)' ctor" << endl;

        {
            static const struct {
                int         d_line;
                const char *d_layoutSpec;  // Layout specification
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
                { L_,       "U" },
                { L_,       "V" },
                { L_,       "W" },
                { L_,       "X" },
                { L_,       "Y" },
                { L_,       "Z" },
                { L_,       "a" },
                { L_,       "b" },
                { L_,       "c" },
                { L_,       "d" },
                { L_,       "e" },
                { L_,       "f" },

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
                { L_,       "UV" },
                { L_,       "YZ" },
                { L_,       "ab" },

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
                { L_,       "abABCDEFG" },
                { L_,       "FDLALAabADSF" },

                { L_,       "GSRWSASDEFDSA" },
                { L_,       "PKJHALKGabASDA" },
                { L_,       "XZSAEWRPOIJLKaAS" },
                { L_,       "GALKaKASJDKSWEIO" },

                { L_,       "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef" }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_layoutSpec;
                const int   LEN   = bsl::strlen(SPEC);

                Layout mL(gLayout(SPEC)); const Layout& L = mL;

                if (veryVerbose) { T_ P_(LINE) P_(SPEC)
                                   L.print(bsl::cout, 1, 4); };

                LOOP_ASSERT(LINE, LEN == L.length());

                for (int j = 0; j < 4; ++j) {
                    AggOption::AllocationStrategy strategy =
                                             (AggOption::AllocationStrategy) j;

                    // With allocator specified
                    {
                        TestAllocator ta, da;
                        bslma_DefaultAllocatorGuard dag(&da);

                        if (AggOption::BDEM_NODESTRUCT_FLAG & strategy) {
                            ta.setQuiet(1);
                        }

#if !defined(BSLS_PLATFORM__CMP_MSVC)
                      BEGIN_BSLMA_EXCEPTION_TEST {
#endif
                        Obj mX(&L, strategy, &ta); const Obj &X = mX;
                        LOOP_ASSERT(LINE, &L  == X.rowLayout());
                        LOOP3_ASSERT(LINE, LEN, X.length(), LEN == X.length());
                        LOOP_ASSERT(LINE, 0 == X.isAnyInRangeNonNull(0, LEN));
                        if (LEN) {
                            LOOP_ASSERT(LINE, 1 == X.isAnyInRangeNull(0, LEN));
                        }
                        LOOP_ASSERT(LINE, 0 == da.numBytesInUse());
                        LOOP_ASSERT(LINE, 0 == da.numBlocksInUse());

                        for (int k = 0; k < LEN; ++k) {
                            const char  SPEC_CHAR = SPEC[k];
                            const void *VA        = getValueA(SPEC_CHAR);
                            const void *VB        = getValueB(SPEC_CHAR);
                            const void *VU        = getValueU(SPEC_CHAR);

                            LOOP2_ASSERT(LINE, k, X.isNull(k));
                            LOOP2_ASSERT(LINE, k, isUnset(mX.elemRef(k)));
                            LOOP2_ASSERT(LINE, k, isUnset(X.elemRef(k)));
                            LOOP2_ASSERT(LINE, k, compare(VU,
                                                          mX.elemData(k),
                                                          SPEC_CHAR));
                            LOOP2_ASSERT(LINE, k, compare(VU,
                                                          X.elemData(k),
                                                          SPEC_CHAR));

                            mX.setValue(k, VA);
                            LOOP2_ASSERT(LINE, k, compare(VA,
                                                          mX.elemData(k),
                                                          SPEC_CHAR));
                            LOOP2_ASSERT(LINE, k, compare(VA,
                                                          X.elemData(k),
                                                          SPEC_CHAR));

                            mX.setValue(k, VB);
                            LOOP2_ASSERT(LINE, k, compare(VB,
                                                          mX.elemData(k),
                                                          SPEC_CHAR));
                            LOOP2_ASSERT(LINE, k, compare(VB,
                                                          X.elemData(k),
                                                          SPEC_CHAR));
                        }

#if !defined(BSLS_PLATFORM__CMP_MSVC)
                      } END_BSLMA_EXCEPTION_TEST
#endif
                    }

                    // Without allocator specified
                    {
                        TestAllocator da;
                        TestAllocator& testAllocator = da;
                        if (AggOption::BDEM_NODESTRUCT_FLAG & strategy) {
                            da.setQuiet(1);
                        }
                        bslma_DefaultAllocatorGuard dag(&da);

#if !defined(BSLS_PLATFORM__CMP_MSVC)
                      BEGIN_BSLMA_EXCEPTION_TEST {
#endif
                        Obj mX(&L, strategy); const Obj &X = mX;
                        LOOP_ASSERT(LINE, &L  == X.rowLayout());
                        LOOP3_ASSERT(LINE, LEN, X.length(), LEN == X.length());
                        LOOP_ASSERT(LINE, 0 == X.isAnyInRangeNonNull(0, LEN));
                        if (LEN) {
                            LOOP_ASSERT(LINE, 1 == X.isAnyInRangeNull(0, LEN));
                        }

                        for (int k = 0; k < LEN; ++k) {
                            const char  SPEC_CHAR = SPEC[k];
                            const void *VA        = getValueA(SPEC_CHAR);
                            const void *VB        = getValueB(SPEC_CHAR);
                            const void *VU        = getValueU(SPEC_CHAR);

                            LOOP2_ASSERT(LINE, k, X.isNull(k));
                            LOOP2_ASSERT(LINE, k, isUnset(mX.elemRef(k)));
                            LOOP2_ASSERT(LINE, k, isUnset(X.elemRef(k)));
                            LOOP2_ASSERT(LINE, k, compare(VU,
                                                          mX.elemData(k),
                                                          SPEC_CHAR));
                            LOOP2_ASSERT(LINE, k, compare(VU,
                                                          X.elemData(k),
                                                          SPEC_CHAR));

                            mX.setValue(k, VA);
                            LOOP2_ASSERT(LINE, k, compare(VA,
                                                          mX.elemData(k),
                                                          SPEC_CHAR));
                            LOOP2_ASSERT(LINE, k, compare(VA,
                                                          X.elemData(k),
                                                          SPEC_CHAR));

                            mX.setValue(k, VB);
                            LOOP2_ASSERT(LINE, k, compare(VB,
                                                          mX.elemData(k),
                                                          SPEC_CHAR));
                            LOOP2_ASSERT(LINE, k, compare(VB,
                                                          X.elemData(k),
                                                          SPEC_CHAR));
                        }

#if !defined(BSLS_PLATFORM__CMP_MSVC)
                      } END_BSLMA_EXCEPTION_TEST
#endif
                    }
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS
        //
        // Concerns:
        //   1. The generator functions should construct a valid 'bdem_RowData'
        //      and 'bdem_RowLayout' for a valid spec.
        //   2. The generator functions should return an erroneous return
        //      code for an invalid spec.
        //
        // Plan:
        //   We specify a list of valid and invalid specifications that are
        //   used to construct various 'bdem_RowData' and 'bdem_RowLayout'
        //   objects using the two generator functions.  The constructed
        //   objects are verified using the accessor functions.  As there isn't
        //   a single manipulator function that can be used to construct the
        //   various objects we rely on a small subset of functions in the
        //   generator functions.  These functions are:
        //     bdem_RowData(AggOption::AllocationStrategy  allocMode,
        //                  bslma_Allocator               *basicAllocator);
        //     void reset(const bdem_RowLayout *rowLayout);
        //     void *insertNullElement(int index);
        //     void *insertElement(int index, const void *value);
        //
        //   In addition we will assume that the accessor functions used to
        //   verify that the constructed objects are correct.
        //
        // Testing:
        //  int ggg(Obj *obj, const char *spec, Allocator *ta, bool verbose);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING GENERATOR FUNCTION"
                               << "\n==========================" << bsl::endl;

        const struct {
            int         d_line;
            const char *d_spec_p;
            bool        d_isValid;
            const char *d_types_p;
            const char *d_values_p;
        } DATA[] = {
            // line  spec         isValid     types       values
            // ----  ----         -------     -----       ------
            {   L_,  "",          true,       "",         ""        },

            {   L_,  "A1",        true,       "A",        "1"       },
            {   L_,  "B2",        true,       "B",        "2"       },
            {   L_,  "Cu",        true,       "C",        "u"       },
            {   L_,  "Dn",        true,       "D",        "n"       },
            {   L_,  "En",        true,       "E",        "n"       },

            {   L_,  "A1B2",      true,       "AB",       "12"      },
            {   L_,  "CnDu",      true,       "CD",       "nu"      },
            {   L_,  "E1Fn",      true,       "EF",       "1n"      },

            {   L_,  "G1HnIn",    true,       "GHI",      "1nn"     },
            {   L_,  "JnK2Ln",    true,       "JKL",      "n2n"     },
            {   L_,  "MnNnOn",    true,       "MNO",      "nnn"     },
            {   L_,  "PnQnRn",    true,       "PQR",      "nnn"     },

            {   L_,  "SnTnUnVn",  true,       "STUV",     "nnnn"    },
            {   L_,  "W1XnY2Zn",  true,       "WXYZ",     "1n2n"    },
            {   L_,  "duanbnc1",  true,       "dabc",     "unn1"    },
            {   L_,  "eufnbnc1",  true,       "efbc",     "unn1"    },

            {   L_,  "A1BnC1D2EnFuGnH1InJ1KnLuM2NuOnP1Qn"
                     "RnSuTnU1VnW2XnYnZ1anb1cnduenfn",
                                  true,
                                          "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef",
                                          "1n12nun1n1nu2un1nnun1n2nn1n1nunn" },

            // Invalid specs
            {   L_,  "z1",        false,      "z",        "1"       },
            {   L_,  "g2",        false,      "g",        "2"       },

            {   L_,  "A0",        false,      "A",        "0"       },
            {   L_,  "B4",        false,      "B",        "4"       },
            {   L_,  "C#",        false,      "C",        "#"       },
            {   L_,  "D@",        false,      "D",        "@"       },
            {   L_,  "B*",        false,      "D",        "*"       },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE      = DATA[i].d_line;
            const char *SPEC      = DATA[i].d_spec_p;
            const bool  VALID     = DATA[i].d_isValid;
            const char *TYPES     = DATA[i].d_types_p;
            const char *VALUES    = DATA[i].d_values_p;
            const int   NUM_ELEMS = strlen(TYPES);

            LOOP_ASSERT(LINE, (int) strlen(SPEC)   == 2 * NUM_ELEMS);
            LOOP_ASSERT(LINE, (int) strlen(VALUES) == NUM_ELEMS);

            if (veryVerbose) { P(SPEC) }

            bslma_TestAllocator ta(veryVeryVerbose);
            const Layout *L;
            {
                Obj mX(PT, &ta); const Obj& X = mX;
                int rc = ggg(&mX, SPEC, &ta, false);

                LOOP_ASSERT(LINE, VALID ? !rc : rc);

                if (!VALID) { continue; }

                if (veryVerbose) { X.printRow(bsl::cout, 1, 4); }

                LOOP3_ASSERT(LINE, NUM_ELEMS, X.length(),
                             NUM_ELEMS == X.length());

                for (int j = 0; j < NUM_ELEMS; ++j) {
                    const Desc *EXP_DESC = getDescriptor(TYPES[j]);

                    LOOP2_ASSERT(LINE, j,
                                 EXP_DESC == (*X.rowLayout())[j].attributes());
                }

                for (int j = 0; j < NUM_ELEMS; ++j) {
                    if ('n' == VALUES[j]) {
                        LOOP2_ASSERT(LINE, j, X.elemRef(j).isNull());
                        LOOP2_ASSERT(LINE, j, isUnset(X.elemRef(j)));
                    }
                    else {
                        LOOP2_ASSERT(LINE, j,!X.elemRef(j).isNull());
                        const void *EXP_VALUE = getValue(TYPES[j], VALUES[j]);
                        LOOP2_ASSERT(LINE, j,
                                  compare(EXP_VALUE, X.elemData(j), TYPES[j]));
                    }
                }
                L = X.rowLayout();
            }
            ta.deleteObjectRaw(L);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING HELPER FUNCTIONS
        //
        // Concerns:
        //  1. Confirm that the various helper functions namely, 'getValueA',
        //     'getValueB', 'getValueN', 'getElemType', 'getDescriptor',
        //     and 'getValue' work as expected.
        //
        // Plan:
        //  1. Test that the getValue functions return the expected value for
        //     each value.  (Brute force testing).
        //
        // Testing:
        //   ET::Type getElemType(char spec);
        //   const Descriptor* getDescriptor(char spec);
        //   const void *getValueA(char spec);
        //   const void *getValueB(char spec);
        //   const void *getValueU(char spec);
        //   const void *getValueN(char spec);
        //   const void *getValue(char typeSpec, char valueSpec);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING HELPER FUNCTIONS"
                               << "\n========================" << bsl::endl;

        const char *SPEC = SPECIFICATIONS;

        bslma_TestAllocator testAllocator;

        if (verbose)  bsl::cout << "\nTesting 'getElemType'" << bsl::endl;
        {
            ASSERT(ET::BDEM_CHAR     == getElemType(SPEC[CHAR_INDEX]));
            ASSERT(ET::BDEM_SHORT    == getElemType(SPEC[SHORT_INDEX]));
            ASSERT(ET::BDEM_INT      == getElemType(SPEC[INT_INDEX]));
            ASSERT(ET::BDEM_INT64    == getElemType(SPEC[INT64_INDEX]));
            ASSERT(ET::BDEM_FLOAT    == getElemType(SPEC[FLOAT_INDEX]));
            ASSERT(ET::BDEM_DOUBLE   == getElemType(SPEC[DOUBLE_INDEX]));
            ASSERT(ET::BDEM_STRING   == getElemType(SPEC[STRING_INDEX]));
            ASSERT(ET::BDEM_DATETIME == getElemType(SPEC[DATETIME_INDEX]));
            ASSERT(ET::BDEM_DATE     == getElemType(SPEC[DATE_INDEX]));
            ASSERT(ET::BDEM_TIME     == getElemType(SPEC[TIME_INDEX]));

            ASSERT(ET::BDEM_CHAR_ARRAY   ==
                                          getElemType(SPEC[CHAR_ARRAY_INDEX]));
            ASSERT(ET::BDEM_SHORT_ARRAY  ==
                                         getElemType(SPEC[SHORT_ARRAY_INDEX]));
            ASSERT(ET::BDEM_INT_ARRAY    ==
                                         getElemType(SPEC[INT_ARRAY_INDEX]));
            ASSERT(ET::BDEM_INT64_ARRAY  ==
                                         getElemType(SPEC[INT64_ARRAY_INDEX]));
            ASSERT(ET::BDEM_FLOAT_ARRAY  ==
                                         getElemType(SPEC[FLOAT_ARRAY_INDEX]));
            ASSERT(ET::BDEM_DOUBLE_ARRAY ==
                                        getElemType(SPEC[DOUBLE_ARRAY_INDEX]));
            ASSERT(ET::BDEM_STRING_ARRAY ==
                                        getElemType(SPEC[STRING_ARRAY_INDEX]));
            ASSERT(ET::BDEM_DATETIME_ARRAY ==
                                      getElemType(SPEC[DATETIME_ARRAY_INDEX]));
            ASSERT(ET::BDEM_DATE_ARRAY   ==
                                      getElemType(SPEC[DATE_ARRAY_INDEX]));
            ASSERT(ET::BDEM_TIME_ARRAY   ==
                                      getElemType(SPEC[TIME_ARRAY_INDEX]));

            ASSERT(ET::BDEM_BOOL          == getElemType(SPEC[BOOL_INDEX]));
            ASSERT(ET::BDEM_DATETIMETZ    ==
                                          getElemType(SPEC[DATETIMETZ_INDEX]));
            ASSERT(ET::BDEM_DATETZ        ==
                                              getElemType(SPEC[DATETZ_INDEX]));
            ASSERT(ET::BDEM_TIMETZ        ==
                                              getElemType(SPEC[TIMETZ_INDEX]));
            ASSERT(ET::BDEM_BOOL_ARRAY    ==
                                          getElemType(SPEC[BOOL_ARRAY_INDEX]));
            ASSERT(ET::BDEM_DATETIMETZ_ARRAY ==
                                    getElemType(SPEC[DATETIMETZ_ARRAY_INDEX]));
            ASSERT(ET::BDEM_DATETZ_ARRAY  ==
                                        getElemType(SPEC[DATETZ_ARRAY_INDEX]));
            ASSERT(ET::BDEM_TIMETZ_ARRAY  ==
                                        getElemType(SPEC[TIMETZ_ARRAY_INDEX]));
        }

        if (verbose)  bsl::cout << "\nTesting 'getDescriptor'" << bsl::endl;
        {
            ASSERT(DESC[CHAR_INDEX]  == getDescriptor(SPEC[CHAR_INDEX]));
            ASSERT(DESC[SHORT_INDEX] == getDescriptor(SPEC[SHORT_INDEX]));
            ASSERT(DESC[INT_INDEX]   == getDescriptor(SPEC[INT_INDEX]));
            ASSERT(DESC[INT64_INDEX] == getDescriptor(SPEC[INT64_INDEX]));
            ASSERT(DESC[FLOAT_INDEX] == getDescriptor(SPEC[FLOAT_INDEX]));
            ASSERT(DESC[DOUBLE_INDEX] == getDescriptor(SPEC[DOUBLE_INDEX]));
            ASSERT(DESC[STRING_INDEX] == getDescriptor(SPEC[STRING_INDEX]));
            ASSERT(DESC[DATETIME_INDEX] ==
                                          getDescriptor(SPEC[DATETIME_INDEX]));
            ASSERT(DESC[DATE_INDEX] == getDescriptor(SPEC[DATE_INDEX]));
            ASSERT(DESC[TIME_INDEX] == getDescriptor(SPEC[TIME_INDEX]));

            ASSERT(DESC[CHAR_ARRAY_INDEX] ==
                                        getDescriptor(SPEC[CHAR_ARRAY_INDEX]));
            ASSERT(DESC[SHORT_ARRAY_INDEX]  ==
                                       getDescriptor(SPEC[SHORT_ARRAY_INDEX]));
            ASSERT(DESC[INT_ARRAY_INDEX] ==
                                         getDescriptor(SPEC[INT_ARRAY_INDEX]));
            ASSERT(DESC[INT64_ARRAY_INDEX]  ==
                                       getDescriptor(SPEC[INT64_ARRAY_INDEX]));
            ASSERT(DESC[FLOAT_ARRAY_INDEX] ==
                                       getDescriptor(SPEC[FLOAT_ARRAY_INDEX]));
            ASSERT(DESC[DOUBLE_ARRAY_INDEX]==
                                      getDescriptor(SPEC[DOUBLE_ARRAY_INDEX]));
            ASSERT(DESC[STRING_ARRAY_INDEX]==
                                      getDescriptor(SPEC[STRING_ARRAY_INDEX]));
            ASSERT(DESC[DATETIME_ARRAY_INDEX]==
                                    getDescriptor(SPEC[DATETIME_ARRAY_INDEX]));
            ASSERT(DESC[DATE_ARRAY_INDEX] ==
                                        getDescriptor(SPEC[DATE_ARRAY_INDEX]));
            ASSERT(DESC[TIME_ARRAY_INDEX] ==
                                        getDescriptor(SPEC[TIME_ARRAY_INDEX]));

            ASSERT(DESC[BOOL_INDEX] == getDescriptor(SPEC[BOOL_INDEX]));
            ASSERT(DESC[DATETIMETZ_INDEX]   ==
                                        getDescriptor(SPEC[DATETIMETZ_INDEX]));
            ASSERT(DESC[DATETZ_INDEX] == getDescriptor(SPEC[DATETZ_INDEX]));
            ASSERT(DESC[TIMETZ_INDEX] == getDescriptor(SPEC[TIMETZ_INDEX]));
            ASSERT(DESC[BOOL_ARRAY_INDEX]   ==
                                        getDescriptor(SPEC[BOOL_ARRAY_INDEX]));
            ASSERT(DESC[DATETIMETZ_ARRAY_INDEX]==
                                  getDescriptor(SPEC[DATETIMETZ_ARRAY_INDEX]));
            ASSERT(DESC[DATETZ_ARRAY_INDEX] ==
                                      getDescriptor(SPEC[DATETZ_ARRAY_INDEX]));
            ASSERT(DESC[TIMETZ_ARRAY_INDEX] ==
                                      getDescriptor(SPEC[TIMETZ_ARRAY_INDEX]));
        }

        if (verbose)
            bsl::cout << "\nTesting 'getValueA'." << bsl::endl;
        {
            ASSERT(A00   == *(char *) getValueA(SPEC[CHAR_INDEX]));
            ASSERT(A01   == *(short *) getValueA(SPEC[SHORT_INDEX]));
            ASSERT(A02   == *(int *) getValueA(SPEC[INT_INDEX]));
            ASSERT(A03   == *(Int64 *) getValueA(SPEC[INT64_INDEX]));
            ASSERT(A04   == *(float *) getValueA(SPEC[FLOAT_INDEX]));
            ASSERT(A05   == *(double *) getValueA(SPEC[DOUBLE_INDEX]));
            ASSERT(A06   == *(bsl::string *) getValueA(SPEC[STRING_INDEX]));
            ASSERT(A07   == *(Datetime *) getValueA(SPEC[DATETIME_INDEX]));
            ASSERT(A08   == *(Date *) getValueA(SPEC[DATE_INDEX]));
            ASSERT(A09   == *(Time *) getValueA(SPEC[TIME_INDEX]));

            ASSERT(A10   == *(bsl::vector<char> *)
                                            getValueA(SPEC[CHAR_ARRAY_INDEX]));
            ASSERT(A11   == *(bsl::vector<short> *)
                                           getValueA(SPEC[SHORT_ARRAY_INDEX]));
            ASSERT(A12   == *(bsl::vector<int> *)
                                             getValueA(SPEC[INT_ARRAY_INDEX]));
            ASSERT(A13   == *(bsl::vector<Int64> *)
                                           getValueA(SPEC[INT64_ARRAY_INDEX]));
            ASSERT(A14   == *(bsl::vector<float> *)
                                           getValueA(SPEC[FLOAT_ARRAY_INDEX]));
            ASSERT(A15   == *(bsl::vector<double> *)
                                          getValueA(SPEC[DOUBLE_ARRAY_INDEX]));
            ASSERT(A16   == *(bsl::vector<bsl::string> *)
                                          getValueA(SPEC[STRING_ARRAY_INDEX]));
            ASSERT(A17   == *(bsl::vector<Datetime> *)
                                       getValueA(SPEC[DATETIME_ARRAY_INDEX]));
            ASSERT(A18   == *(bsl::vector<Date> *)
                                            getValueA(SPEC[DATE_ARRAY_INDEX]));
            ASSERT(A19   == *(bsl::vector<Time> *)
                                            getValueA(SPEC[TIME_ARRAY_INDEX]));

            ASSERT(A22   == *(bool *) getValueA(SPEC[BOOL_INDEX]));
            ASSERT(A23   == *(DatetimeTz *) getValueA(SPEC[DATETIMETZ_INDEX]));
            ASSERT(A24   == *(DateTz *) getValueA(SPEC[DATETZ_INDEX]));
            ASSERT(A25   == *(TimeTz *) getValueA(SPEC[TIMETZ_INDEX]));
            ASSERT(A26   == *(bsl::vector<bool> *)
                                            getValueA(SPEC[BOOL_ARRAY_INDEX]));
            ASSERT(A27   == *(bsl::vector<DatetimeTz> *)
                                      getValueA(SPEC[DATETIMETZ_ARRAY_INDEX]));
            ASSERT(A28   == *(bsl::vector<DateTz> *)
                                          getValueA(SPEC[DATETZ_ARRAY_INDEX]));
            ASSERT(A29   == *(bsl::vector<TimeTz> *)
                                          getValueA(SPEC[TIMETZ_ARRAY_INDEX]));
        }

        if (verbose)
            bsl::cout << "\nTesting 'getValueB'." << bsl::endl;
        {
            ASSERT(B00   == *(char *) getValueB(SPEC[CHAR_INDEX]));
            ASSERT(B01   == *(short *) getValueB(SPEC[SHORT_INDEX]));
            ASSERT(B02   == *(int *) getValueB(SPEC[INT_INDEX]));
            ASSERT(B03   == *(Int64 *) getValueB(SPEC[INT64_INDEX]));
            ASSERT(B04   == *(float *) getValueB(SPEC[FLOAT_INDEX]));
            ASSERT(B05   == *(double *) getValueB(SPEC[DOUBLE_INDEX]));
            ASSERT(B06   == *(bsl::string *) getValueB(SPEC[STRING_INDEX]));
            ASSERT(B07   == *(Datetime *) getValueB(SPEC[DATETIME_INDEX]));
            ASSERT(B08   == *(Date *) getValueB(SPEC[DATE_INDEX]));
            ASSERT(B09   == *(Time *) getValueB(SPEC[TIME_INDEX]));

            ASSERT(B10   == *(bsl::vector<char> *)
                                            getValueB(SPEC[CHAR_ARRAY_INDEX]));
            ASSERT(B11   == *(bsl::vector<short> *)
                                           getValueB(SPEC[SHORT_ARRAY_INDEX]));
            ASSERT(B12   == *(bsl::vector<int> *)
                                             getValueB(SPEC[INT_ARRAY_INDEX]));
            ASSERT(B13   == *(bsl::vector<Int64> *)
                                           getValueB(SPEC[INT64_ARRAY_INDEX]));
            ASSERT(B14   == *(bsl::vector<float> *)
                                           getValueB(SPEC[FLOAT_ARRAY_INDEX]));
            ASSERT(B15   == *(bsl::vector<double> *)
                                          getValueB(SPEC[DOUBLE_ARRAY_INDEX]));
            ASSERT(B16   == *(bsl::vector<bsl::string> *)
                                          getValueB(SPEC[STRING_ARRAY_INDEX]));
            ASSERT(B17   == *(bsl::vector<Datetime> *)
                                       getValueB(SPEC[DATETIME_ARRAY_INDEX]));
            ASSERT(B18   == *(bsl::vector<Date> *)
                                            getValueB(SPEC[DATE_ARRAY_INDEX]));
            ASSERT(B19   == *(bsl::vector<Time> *)
                                            getValueB(SPEC[TIME_ARRAY_INDEX]));

            ASSERT(B22   == *(bool *) getValueB(SPEC[BOOL_INDEX]));
            ASSERT(B23   == *(DatetimeTz *) getValueB(SPEC[DATETIMETZ_INDEX]));
            ASSERT(B24   == *(DateTz *) getValueB(SPEC[DATETZ_INDEX]));
            ASSERT(B25   == *(TimeTz *) getValueB(SPEC[TIMETZ_INDEX]));
            ASSERT(B26   == *(bsl::vector<bool> *)
                                            getValueB(SPEC[BOOL_ARRAY_INDEX]));
            ASSERT(B27   == *(bsl::vector<DatetimeTz> *)
                                      getValueB(SPEC[DATETIMETZ_ARRAY_INDEX]));
            ASSERT(B28   == *(bsl::vector<DateTz> *)
                                          getValueB(SPEC[DATETZ_ARRAY_INDEX]));
            ASSERT(B29   == *(bsl::vector<TimeTz> *)
                                          getValueB(SPEC[TIMETZ_ARRAY_INDEX]));
        }

        if (verbose)
            bsl::cout << "\nTesting 'getValueU'." << bsl::endl;
        {
            ASSERT(N00   == *(char *) getValueU(SPEC[CHAR_INDEX]));
            ASSERT(N01   == *(short *) getValueU(SPEC[SHORT_INDEX]));
            ASSERT(N02   == *(int *) getValueU(SPEC[INT_INDEX]));
            ASSERT(N03   == *(Int64 *) getValueU(SPEC[INT64_INDEX]));
            ASSERT(N04   == *(float *) getValueU(SPEC[FLOAT_INDEX]));
            ASSERT(N05   == *(double *) getValueU(SPEC[DOUBLE_INDEX]));
            ASSERT(N06   == *(bsl::string *) getValueU(SPEC[STRING_INDEX]));
            ASSERT(N07   == *(Datetime *) getValueU(SPEC[DATETIME_INDEX]));
            ASSERT(N08   == *(Date *) getValueU(SPEC[DATE_INDEX]));
            ASSERT(N09   == *(Time *) getValueU(SPEC[TIME_INDEX]));

            ASSERT(N10   == *(bsl::vector<char> *)
                                            getValueU(SPEC[CHAR_ARRAY_INDEX]));
            ASSERT(N11   == *(bsl::vector<short> *)
                                           getValueU(SPEC[SHORT_ARRAY_INDEX]));
            ASSERT(N12   == *(bsl::vector<int> *)
                                             getValueU(SPEC[INT_ARRAY_INDEX]));
            ASSERT(N13   == *(bsl::vector<Int64> *)
                                           getValueU(SPEC[INT64_ARRAY_INDEX]));
            ASSERT(N14   == *(bsl::vector<float> *)
                                           getValueU(SPEC[FLOAT_ARRAY_INDEX]));
            ASSERT(N15   == *(bsl::vector<double> *)
                                          getValueU(SPEC[DOUBLE_ARRAY_INDEX]));
            ASSERT(N16   == *(bsl::vector<bsl::string> *)
                                          getValueU(SPEC[STRING_ARRAY_INDEX]));
            ASSERT(N17   == *(bsl::vector<Datetime> *)
                                       getValueU(SPEC[DATETIME_ARRAY_INDEX]));
            ASSERT(N18   == *(bsl::vector<Date> *)
                                            getValueU(SPEC[DATE_ARRAY_INDEX]));
            ASSERT(N19   == *(bsl::vector<Time> *)
                                            getValueU(SPEC[TIME_ARRAY_INDEX]));

            ASSERT(N22   == *(bool *) getValueU(SPEC[BOOL_INDEX]));
            ASSERT(N23   == *(DatetimeTz *) getValueU(SPEC[DATETIMETZ_INDEX]));
            ASSERT(N24   == *(DateTz *) getValueU(SPEC[DATETZ_INDEX]));
            ASSERT(N25   == *(TimeTz *) getValueU(SPEC[TIMETZ_INDEX]));
            ASSERT(N26   == *(bsl::vector<bool> *)
                                            getValueU(SPEC[BOOL_ARRAY_INDEX]));
            ASSERT(N27   == *(bsl::vector<DatetimeTz> *)
                                      getValueU(SPEC[DATETIMETZ_ARRAY_INDEX]));
            ASSERT(N28   == *(bsl::vector<DateTz> *)
                                          getValueU(SPEC[DATETZ_ARRAY_INDEX]));
            ASSERT(N29   == *(bsl::vector<TimeTz> *)
                                          getValueU(SPEC[TIMETZ_ARRAY_INDEX]));
        }

        if (verbose)
            bsl::cout << "\nTesting 'getValueN'." << bsl::endl;
        {
            ASSERT(N00   == *(char *) getValueN(SPEC[CHAR_INDEX]));
            ASSERT(N01   == *(short *) getValueN(SPEC[SHORT_INDEX]));
            ASSERT(N02   == *(int *) getValueN(SPEC[INT_INDEX]));
            ASSERT(N03   == *(Int64 *) getValueN(SPEC[INT64_INDEX]));
            ASSERT(N04   == *(float *) getValueN(SPEC[FLOAT_INDEX]));
            ASSERT(N05   == *(double *) getValueN(SPEC[DOUBLE_INDEX]));
            ASSERT(N06   == *(bsl::string *) getValueN(SPEC[STRING_INDEX]));
            ASSERT(N07   == *(Datetime *) getValueN(SPEC[DATETIME_INDEX]));
            ASSERT(N08   == *(Date *) getValueN(SPEC[DATE_INDEX]));
            ASSERT(N09   == *(Time *) getValueN(SPEC[TIME_INDEX]));

            ASSERT(N10   == *(bsl::vector<char> *)
                                            getValueN(SPEC[CHAR_ARRAY_INDEX]));
            ASSERT(N11   == *(bsl::vector<short> *)
                                           getValueN(SPEC[SHORT_ARRAY_INDEX]));
            ASSERT(N12   == *(bsl::vector<int> *)
                                             getValueN(SPEC[INT_ARRAY_INDEX]));
            ASSERT(N13   == *(bsl::vector<Int64> *)
                                           getValueN(SPEC[INT64_ARRAY_INDEX]));
            ASSERT(N14   == *(bsl::vector<float> *)
                                           getValueN(SPEC[FLOAT_ARRAY_INDEX]));
            ASSERT(N15   == *(bsl::vector<double> *)
                                          getValueN(SPEC[DOUBLE_ARRAY_INDEX]));
            ASSERT(N16   == *(bsl::vector<bsl::string> *)
                                          getValueN(SPEC[STRING_ARRAY_INDEX]));
            ASSERT(N17   == *(bsl::vector<Datetime> *)
                                       getValueN(SPEC[DATETIME_ARRAY_INDEX]));
            ASSERT(N18   == *(bsl::vector<Date> *)
                                            getValueN(SPEC[DATE_ARRAY_INDEX]));
            ASSERT(N19   == *(bsl::vector<Time> *)
                                            getValueN(SPEC[TIME_ARRAY_INDEX]));

            ASSERT(N22   == *(bool *) getValueN(SPEC[BOOL_INDEX]));
            ASSERT(N23   == *(DatetimeTz *) getValueN(SPEC[DATETIMETZ_INDEX]));
            ASSERT(N24   == *(DateTz *) getValueN(SPEC[DATETZ_INDEX]));
            ASSERT(N25   == *(TimeTz *) getValueN(SPEC[TIMETZ_INDEX]));
            ASSERT(N26   == *(bsl::vector<bool> *)
                                            getValueN(SPEC[BOOL_ARRAY_INDEX]));
            ASSERT(N27   == *(bsl::vector<DatetimeTz> *)
                                      getValueN(SPEC[DATETIMETZ_ARRAY_INDEX]));
            ASSERT(N28   == *(bsl::vector<DateTz> *)
                                          getValueN(SPEC[DATETZ_ARRAY_INDEX]));
            ASSERT(N29   == *(bsl::vector<TimeTz> *)
                                          getValueN(SPEC[TIMETZ_ARRAY_INDEX]));
        }

        if (verbose)
            bsl::cout << "\nTesting 'getValue'." << bsl::endl;
        {
            const char *VALUE_CHARS = "un12";
            const int   NUM_CHARS   = strlen(VALUE_CHARS);

            for (int i = 0; i < NUM_CHARS; ++i) {
                char         VALUE_CHAR = VALUE_CHARS[i];
                const CERef *VALUES     = 'u' == VALUE_CHAR ? VALUES_U
                                        : 'n' == VALUE_CHAR ? VALUES_N
                                        : '1' == VALUE_CHAR ? VALUES_A
                                        : '2' == VALUE_CHAR ? VALUES_B
                                        : 0;
                ASSERT(VALUES);

                ASSERT(VALUES[CHAR_INDEX].data() ==
                                       getValue(SPEC[CHAR_INDEX], VALUE_CHAR));
                ASSERT(VALUES[SHORT_INDEX].data() ==
                                      getValue(SPEC[SHORT_INDEX], VALUE_CHAR));
                ASSERT(VALUES[INT_INDEX].data() ==
                                        getValue(SPEC[INT_INDEX], VALUE_CHAR));
                ASSERT(VALUES[INT64_INDEX].data() ==
                                      getValue(SPEC[INT64_INDEX], VALUE_CHAR));
                ASSERT(VALUES[FLOAT_INDEX].data() ==
                                      getValue(SPEC[FLOAT_INDEX], VALUE_CHAR));
                ASSERT(VALUES[DOUBLE_INDEX].data() ==
                                     getValue(SPEC[DOUBLE_INDEX], VALUE_CHAR));
                ASSERT(VALUES[STRING_INDEX].data() ==
                                     getValue(SPEC[STRING_INDEX], VALUE_CHAR));
                ASSERT(VALUES[DATETIME_INDEX].data() ==
                                   getValue(SPEC[DATETIME_INDEX], VALUE_CHAR));
                ASSERT(VALUES[DATE_INDEX].data() ==
                                       getValue(SPEC[DATE_INDEX], VALUE_CHAR));
                ASSERT(VALUES[TIME_INDEX].data() ==
                                       getValue(SPEC[TIME_INDEX], VALUE_CHAR));

                ASSERT(VALUES[CHAR_ARRAY_INDEX].data() ==
                                 getValue(SPEC[CHAR_ARRAY_INDEX], VALUE_CHAR));
                ASSERT(VALUES[SHORT_ARRAY_INDEX].data() ==
                                getValue(SPEC[SHORT_ARRAY_INDEX], VALUE_CHAR));
                ASSERT(VALUES[INT_ARRAY_INDEX].data() ==
                                  getValue(SPEC[INT_ARRAY_INDEX], VALUE_CHAR));
                ASSERT(VALUES[INT64_ARRAY_INDEX].data() ==
                                getValue(SPEC[INT64_ARRAY_INDEX], VALUE_CHAR));
                ASSERT(VALUES[FLOAT_ARRAY_INDEX].data() ==
                                getValue(SPEC[FLOAT_ARRAY_INDEX], VALUE_CHAR));
                ASSERT(VALUES[DOUBLE_ARRAY_INDEX].data() ==
                               getValue(SPEC[DOUBLE_ARRAY_INDEX], VALUE_CHAR));
                ASSERT(VALUES[STRING_ARRAY_INDEX].data() ==
                               getValue(SPEC[STRING_ARRAY_INDEX], VALUE_CHAR));
                ASSERT(VALUES[DATETIME_ARRAY_INDEX].data() ==
                             getValue(SPEC[DATETIME_ARRAY_INDEX], VALUE_CHAR));
                ASSERT(VALUES[DATE_ARRAY_INDEX].data() ==
                                 getValue(SPEC[DATE_ARRAY_INDEX], VALUE_CHAR));
                ASSERT(VALUES[TIME_ARRAY_INDEX].data() ==
                                 getValue(SPEC[TIME_ARRAY_INDEX], VALUE_CHAR));

                ASSERT(VALUES[BOOL_INDEX].data() ==
                                       getValue(SPEC[BOOL_INDEX], VALUE_CHAR));
                ASSERT(VALUES[DATETIMETZ_INDEX].data() ==
                                 getValue(SPEC[DATETIMETZ_INDEX], VALUE_CHAR));
                ASSERT(VALUES[DATETZ_INDEX].data() ==
                                     getValue(SPEC[DATETZ_INDEX], VALUE_CHAR));
                ASSERT(VALUES[TIMETZ_INDEX].data() ==
                                     getValue(SPEC[TIMETZ_INDEX], VALUE_CHAR));
                ASSERT(VALUES[BOOL_ARRAY_INDEX].data() ==
                                 getValue(SPEC[BOOL_ARRAY_INDEX], VALUE_CHAR));
                ASSERT(VALUES[DATETIMETZ_ARRAY_INDEX].data() ==
                           getValue(SPEC[DATETIMETZ_ARRAY_INDEX], VALUE_CHAR));
                ASSERT(VALUES[DATETZ_ARRAY_INDEX].data() ==
                               getValue(SPEC[DATETZ_ARRAY_INDEX], VALUE_CHAR));
                ASSERT(VALUES[TIMETZ_ARRAY_INDEX].data() ==
                               getValue(SPEC[TIMETZ_ARRAY_INDEX], VALUE_CHAR));
            }
        }
      } break;
      case 1: {
        //--------------------------------------------------------------------
        // BREATHING TEST
        // Concerns:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        //   Create four test objects by using the default, initializing, and
        //   copy constructors.  Exercise the basic value-semantic methods and
        //   the equality operators using these test objects.  Invoke the
        //   primary manipulator [3, 6], copy constructor [2, 5], and
        //   assignment operator without [7, 8] and with [9] aliasing.  Use the
        //   basic accessors to verify the expected results.  Display object
        //   values frequently in verbose mode.  Note that 'VA', 'VB', and 'VC'
        //   denote unique, but otherwise arbitrary, object values, while 'U'
        //   denotes the valid, but "unknown", default object value.
        //
        // 1. Create an object x1 (init. to VA).    { x1:VA }
        // 2. Create an object x2 (copy from x1).   { x1:VA x2:VA }
        // 3. Set x1 to VB.                         { x1:VB x2:VA }
        // 4. Create an object x3 (default ctor).   { x1:VB x2:VA x3:U }
        // 5. Create an object x4 (copy from x3).   { x1:VB x2:VA x3:U  x4:U }
        // 6. Set x3 to VC.                         { x1:VB x2:VA x3:VC x4:U }
        // 7. Assign x2 = x1.                       { x1:VB x2:VB x3:VC x4:U }
        // 8. Assign x1 = x1 (aliasing).            { x1:VB x2:VB x3:VC x4:U }
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Breathing Test" << endl
                          << "==============" << endl;

        Layout mL; const Layout& L = mL;
        mL.append(DESC[CHAR_INDEX]);
        mL.append(DESC[DOUBLE_INDEX]);
        mL.append(DESC[STRING_INDEX]);
        mL.append(DESC[BOOL_ARRAY_INDEX]);
        mL.append(DESC[INT64_ARRAY_INDEX]);

        Layout mL2; const Layout& L2 = mL2;

        Layout mL3; const Layout& L3 = mL3;
        mL3.append(DESC[STRING_INDEX]);
        mL3.append(DESC[BOOL_INDEX]);
        mL3.append(DESC[DATETZ_INDEX]);

        TestAllocator ta;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object x1 (init to VA)."
                             "\t\t{ x1: }" << endl;

        Obj mX1(&L, PT, &ta);  const Obj& X1 = mX1;
        if (veryVerbose) { T_ X1.printRow(bsl::cout, 1, 4); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(5 == X1.length());
        ASSERT(ET::BDEM_CHAR        == X1.elemRef(0).type());
        ASSERT(ET::BDEM_DOUBLE      == X1.elemRef(1).type());
        ASSERT(ET::BDEM_STRING      == X1.elemRef(2).type());
        ASSERT(ET::BDEM_BOOL_ARRAY  == X1.elemRef(3).type());
        ASSERT(ET::BDEM_INT64_ARRAY == X1.elemRef(4).type());

        ASSERT(X1.elemRef(0).isNull());
        ASSERT(X1.elemRef(1).isNull());
        ASSERT(X1.elemRef(2).isNull());
        ASSERT(X1.elemRef(3).isNull());
        ASSERT(X1.elemRef(4).isNull());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;

        Obj mX2(&L, X1, PT, &ta);  const Obj& X2 = mX2;
        if (veryVerbose) { T_ X2.printRow(bsl::cout, 1, 4); }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(5 == X2.length());
        ASSERT(ET::BDEM_CHAR        == X2.elemRef(0).type());
        ASSERT(ET::BDEM_DOUBLE      == X2.elemRef(1).type());
        ASSERT(ET::BDEM_STRING      == X2.elemRef(2).type());
        ASSERT(ET::BDEM_BOOL_ARRAY  == X2.elemRef(3).type());
        ASSERT(ET::BDEM_INT64_ARRAY == X2.elemRef(4).type());

        ASSERT(X2.elemRef(0).isNull());
        ASSERT(X2.elemRef(1).isNull());
        ASSERT(X2.elemRef(2).isNull());
        ASSERT(X2.elemRef(3).isNull());
        ASSERT(X2.elemRef(4).isNull());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;

        mX1.elemRef(0).theModifiableChar()       = A00;
        mX1.elemRef(1).theModifiableDouble()     = A05;
        mX1.elemRef(2).theModifiableString()     = A06;
        mX1.elemRef(3).theModifiableBoolArray()  = A26;
        mX1.elemRef(4).theModifiableInt64Array() = A13;

        if (verbose) cout << "\ta. Check new state of x1." << endl;

        ASSERT(A00 == X1.elemRef(0).theChar());
        ASSERT(A05 == X1.elemRef(1).theDouble());
        ASSERT(A06 == X1.elemRef(2).theString());
        ASSERT(A26 == X1.elemRef(3).theBoolArray());
        ASSERT(A13 == X1.elemRef(4).theInt64Array());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;

        Obj mX3(&L2, PT, &ta); const Obj& X3 = mX3;

        if (veryVerbose) { T_ X3.printRow(bsl::cout, 1, 4); }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(0 == X3.length());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;

        Obj mX4(&L2, X3, PT, &ta);  const Obj& X4 = mX4;
        if (veryVerbose) { T_ X4.printRow(bsl::cout, 1, 4); }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(0 == X4.length());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;

        mX3.reset(&mL3);
        mX3.elemRef(0).theModifiableString() = B06;
        mX3.elemRef(1).theModifiableBool()   = B22;
        mX3.elemRef(2).theModifiableDateTz() = B24;

        if (veryVerbose) { T_ X3.printRow(bsl::cout, 1, 4); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(3 == X3.length());
        ASSERT(ET::BDEM_STRING == X3.elemRef(0).type());
        ASSERT(ET::BDEM_BOOL   == X3.elemRef(1).type());
        ASSERT(ET::BDEM_DATETZ == X3.elemRef(2).type());

        ASSERT(B06 == X3.elemRef(0).theString());
        ASSERT(B22 == X3.elemRef(1).theBool());
        ASSERT(B24 == X3.elemRef(2).theDateTz());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { T_ X2.printRow(bsl::cout, 1, 4); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(A00 == X2.elemRef(0).theChar());
        ASSERT(A05 == X2.elemRef(1).theDouble());
        ASSERT(A06 == X2.elemRef(2).theString());
        ASSERT(A26 == X2.elemRef(3).theBoolArray());
        ASSERT(A13 == X2.elemRef(4).theInt64Array());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

        mX1 = X1;
        if (veryVerbose) { T_ X1.printRow(bsl::cout, 1, 4); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(5 == X1.length());
        ASSERT(ET::BDEM_CHAR        == X1.elemRef(0).type());
        ASSERT(ET::BDEM_DOUBLE      == X1.elemRef(1).type());
        ASSERT(ET::BDEM_STRING      == X1.elemRef(2).type());
        ASSERT(ET::BDEM_BOOL_ARRAY  == X1.elemRef(3).type());
        ASSERT(ET::BDEM_INT64_ARRAY == X1.elemRef(4).type());

        ASSERT(A00 == X1.elemRef(0).theChar());
        ASSERT(A05 == X1.elemRef(1).theDouble());
        ASSERT(A06 == X1.elemRef(2).theString());
        ASSERT(A26 == X1.elemRef(3).theBoolArray());
        ASSERT(A13 == X1.elemRef(4).theInt64Array());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == X2));        ASSERT(0 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));
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
