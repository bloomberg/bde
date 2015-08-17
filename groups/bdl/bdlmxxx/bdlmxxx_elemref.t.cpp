// bdlmxxx_elemref.t.cpp                                              -*-C++-*-

#include <bdlmxxx_elemref.h>

#include <bdlmxxx_aggregateoption.h>
#include <bdlmxxx_descriptor.h>

#include <bdlimpxxx_bitwisecopy.h>

#include <bdltuxxx_unset.h>

#include <bslma_allocator.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_exception.h>
#include <bsl_iostream.h>
#include <bsl_new.h>                    // placement 'new' syntax
#include <bsl_string.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

#include <bsl_c_limits.h>
#include <bsl_c_stdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This component implements four classes that support reference semantics,
// which makes each of these classes a kind of *mechanism*.  Each class has
// exactly one primary constructor (taking the address of the data and its
// descriptor) and (for this unusual, reference-semantics, case) no primary
// manipulators.  For the purposes of this test driver we have selected the
// 'const' (and, for the derived type, also non-'const') version of the
// 'data' and 'descriptor' methods as being the maximal suite of basic
// (i.e., direct "eye-witness") accessors.
//
// Much of the work done by this component is simple function-forwarding to
// the appropriate 'bdlmxxx::Row' method.  The only substantive complexity is in
// the interoperability of the 'const'/non-'const' base/derived members of
// the two micro-class hierarchies.  Note that the free operators, by design,
// need be implemented for only the 'bdlmxxx::ConstElemRef' base class.  It is
// important, however, that both classes suppress assignment; otherwise a link
// error will result.  Note that we may decide to support assignment (for
// 'bdlmxxx::ElemRef' only).  The signatures would be as follows:
//
//  bdlmxxx::ElemRef& bdlmxxx::ElemRef::operator=(const bdlmxxx::ConstElemRef& rhs);
//  const bdlmxxx::ElemRef& bdlmxxx::ElemRef::operator=(
//                                         const bdlmxxx::ConstElemRef& rhs) const;
//
// General concerns:
//   (1) Intuitive interoperability between constant and non-constant element
//       references.
//   (2) Without explicit casting, ensure that it is not possible to enable an
//       element to be modified given a 'bdlmxxx::ConstElemRef'.
//   (3) Avoid link-time errors -- e.g, when mistakenly trying to rebind an
//       instance of a 'bdlmxxx::ElemRef' variable from another 'bdlmxxx::ElemRef'-
//       or 'bdlmxxx::ConstElemRef'-valued expression
//
// Test case organization:
//    o Get the component basically working in the breathing test.
//       - Initially requires implementing only a few of the 22 required
//         descriptors in full.
//       - Ultimately will require implementing all 22 descriptors to some
//         degree (but providing just the elemEnum will prove sufficient).
//       - Both 'bdlmxxx::List' and 'bdlmxxx::Table' types will necessarily have
//         to be faked out.
//       - For the sake of the usage example, we will choose to implement
//         the 'bdlmxxx::Table' fully, but as if it were the logical and physical
//         equivalent of a 'float' (for simplicity, and because we can).
//    o Follow the standard approach for verifying the essential functionality
//      of a *mechanism* (addressing both base and derived classes
//      simultaneously.
//       - Because this component requires no custom "test" apparatus, we will
//         combine the usual cases 2-4, and test the basic accessors along
//         with the primary constructors in a single test case (2).
//       - We will then verify bsl::ostream output operators in case (3).
//    o Then ensure that each element accessor works as advertised.
//       - requires implementing all 22 element descriptors (to some degree).
//    o Next address other function such as copy ctor and 'unboundElemRef' that
//      are implemented *without* needing a descriptor.
//    o After that, address functions like 'makeNull', 'isNull', and 'print',
//      which simply forward to the underlying descriptor function.
//       - will create dummy descriptor that records arguments.
//    o Throughout, address specific concerns explicitly.
//         - E.g., all combinations of 'ConstElemRef'/'ElemRef' with operator==
//    o Finally, verify the usage example.
//       - requires implementing 'bdlmxxx::Table' (e.g., as a 'float').
//-----------------------------------------------------------------------------
//                         =======================
//                         class bdlmxxx::ConstElemRef
//                         =======================
// CLASS METHODS
// [ 5] static bdlmxxx::ConstElemRef unboundElemRef();
//
// CREATORS
// [ 2] bdlmxxx::ConstElemRef(const void *data, const bdlmxxx::Descriptor *desc);
// [ 7] bdlmxxx::ConstElemRef(const bdlmxxx::ConstElemRef& original);
// [ 2] ~bdlmxxx::ConstElemRef();
//
// ACCESSORS
// [ 3] const bool& theBool() const;
// [ 3] const char& theChar() const;
// [ 3] const short& theShort() const;
// [ 3] const int& theInt() const;
// [ 3] const bsls::Types::Int64& theInt64() const;
// [ 3] const float& theFloat() const;
// [ 3] const double& theDouble() const;
// [ 3] const bsl::string& theString() const;
// [ 3] const bdlt::Datetime& theDatetime() const;
// [ 3] const bdlt::DatetimeTz& theDatetimeTz() const;
// [ 3] const bdlt::Date& theDate() const;
// [ 3] const bdlt::DateTz& theDateTz() const;
// [ 3] const bdlt::Time& theTime() const;
// [ 3] const bdlt::TimeTz& theTimeTz() const;
// [ 3] const vector<bool>& theBoolArray() const;
// [ 3] const vector<char>& theCharArray() const;
// [ 3] const vector<short>& theShortArray() const;
// [ 3] const vector<int>& theIntArray() const;
// [ 3] const vector<bsls::Types::Int64>& theInt64Array() const;
// [ 3] const vector<float>& theFloatArray() const;
// [ 3] const vector<double>& theDoubleArray() const;
// [ 3] const vector<bsl::string>& theStringArray() const;
// [ 3] const vector<bdlt::Datetime>& theDatetimeArray() const;
// [ 3] const vector<bdlt::DatetimeTz>& theDatetimeTzArray() const;
// [ 3] const vector<bdlt::Date>& theDateArray() const;
// [ 3] const vector<bdlt::DateTz>& theDateTzArray() const;
// [ 3] const vector<bdlt::Time>& theTimeArray() const;
// [ 3] const vector<bdlt::TimeTz>& theTimeTzArray() const;
// [ 3] const bdlmxxx::Row& theRow() const;
// [ 3] const bdlmxxx::List& theList() const;
// [ 3] const bdlmxxx::Table& theTable() const;
// [ 3] const bdlmxxx::ChoiceArrayItem& theChoiceArrayItem() const;
// [ 3] const bdlmxxx::Choice& theChoice() const;
// [ 3] const bdlmxxx::ChoiceArray& theChoiceArray() const;
// [ 5] bdlmxxx::ElemType::Type type() const;
// [ 2] const bdlmxxx::Descriptor *descriptor() const;
// [ 2] const void *data() const;
// [ 5] bool isBound() const;
// [ 6] bsl::ostream& print(bsl::ostream& s, int l, int spl) const;
//
// FREE OPERATORS
// [ 6] operator==(const bdlmxxx::ConstElemRef&, const bdlmxxx::ConstElemRef&);
// [ 6] operator!=(const bdlmxxx::ConstElemRef&, const bdlmxxx::ConstElemRef&);
// [ 6] operator<<(bsl::ostream&, const bdlmxxx::ConstElemRef&);
//
//                      ==================
//                      class bdlmxxx::ElemRef
//                      ==================
// CLASS METHODS
// [ 5] static bdlmxxx::ElemRef unboundElemRef();
//
// CREATORS
// [ 2] bdlmxxx::ElemRef(void *data, const bdlmxxx::Descriptor *desc);
// [ 7] bdlmxxx::ElemRef(const bdlmxxx::ElemRef& original);
// [ 2] ~bdlmxxx::ElemRef();
//
// REFERENCED-VALUE MANIPULATORS
// [ 6] void makeNull() const;
// [ 4] theModifiableBool(bool value) const;
// [ 4] theModifiableChar(char value) const;
// [ 4] theModifiableShort(short value) const;
// [ 4] theModifiableInt(int value) const;
// [ 4] theModifiableInt64(bsls::Types::Int64 value) const;
// [ 4] theModifiableFloat(float value) const;
// [ 4] theModifiableDouble(double value) const;
// [ 4] theModifiableString(const bsl::string& value) const;
// [ 4] theModifiableDatetime(const bdlt::Datetime& value) const;
// [ 4] theModifiableDatetimeTz(const bdlt::DatetimeTz& value) const;
// [ 4] theModifiableDate(const bdlt::Date& value) const;
// [ 4] theModifiableDateTz(const bdlt::DateTz& value) const;
// [ 4] theModifiableTime(const bdlt::Time& value) const;
// [ 4] theModifiableTimeTz(const bdlt::TimeTz& value) const;
// [ 4] theModifiableBoolArray(const bsl::vector<bool>& value) const;
// [ 4] theModifiableCharArray(const bsl::vector<char>& value) const;
// [ 4] theModifiableShortArray(const bsl::vector<short>& value) const;
// [ 4] theModifiableIntArray(const bsl::vector<int>& value) const;
// [ 4] theModifiableInt64Array(
//               const bsl::vector<bsls::Types::Int64>& value) const;
// [ 4] theModifiableFloatArray(const bsl::vector<float>& value) const;
// [ 4] theModifiableDoubleArray(const bsl::vector<double>& value) const;
// [ 4] theModifiableStringArray(const bsl::vector<bsl::string>& value) const;
// [ 4] theModifiableDatetimeArray(
//               const bsl::vector<bdlt::Datetime>& value) const;
// [ 4] theModifiableDatetimeTzArray(
//               const bsl::vector<bdlt::DatetimeTz>& value) const;
// [ 4] theModifiableDateArray(const bsl::vector<bdlt::Date>& value) const;
// [ 4] theModifiableDateTzArray(const bsl::vector<bdlt::DateTz>& value) const;
// [ 4] theModifiableTimeArray(const bsl::vector<bdlt::Time>& value) const;
// [ 4] theModifiableTimeTzArray(const bsl::vector<bdlt::TimeTz>& value) const;
// [ 4] theModifiableList(const bdlmxxx::List& value) const;
// [ 4] theModifiableTable(const bdlmxxx::Table& value) const;
// [ 4] theModifiableChoice(const bdlmxxx::Choice& value) const;
// [ 4] theModifiableChoiceArray(const bdlmxxx::ChoiceArray& value) const;
//
// ACCESSORS
// [ 3] const bool& theBool() const;
// [ 3] const char& theChar() const;
// [ 3] const short& theShort() const;
// [ 3] const int& theInt() const;
// [ 3] const bsls::Types::Int64& theInt64() const;
// [ 3] const float& theFloat() const;
// [ 3] const double& theDouble() const;
// [ 3] const bsl::string& theString() const;
// [ 3] const bdlt::Datetime& theDatetime() const;
// [ 3] const bdlt::DatetimeTz& theDatetimeTz() const;
// [ 3] const bdlt::Date& theDate() const;
// [ 3] const bdlt::DateTz& theDateTz() const;
// [ 3] const bdlt::Time& theTime() const;
// [ 3] const bdlt::TimeTz& theTimeTz() const;
// [ 3] const vector<bool>& theBoolArray() const;
// [ 3] const vector<char>& theCharArray() const;
// [ 3] const vector<short>& theShortArray() const;
// [ 3] const vector<int>& theIntArray() const;
// [ 3] const vector<bsls::Types::Int64>& theInt64Array() const;
// [ 3] const vector<float>& theFloatArray() const;
// [ 3] const vector<double>& theDoubleArray() const;
// [ 3] const vector<bsl::string>& theStringArray() const;
// [ 3] const vector<bdlt::Datetime>& theDatetimeArray() const;
// [ 3] const vector<bdlt::DatetimeTz>& theDatetimeTzArray() const;
// [ 3] const vector<bdlt::Date>& theDateArray() const;
// [ 3] const vector<bdlt::DateTz>& theDateTzArray() const;
// [ 3] const vector<bdlt::Time>& theTimeArray() const;
// [ 3] const vector<bdlt::TimeTz>& theTimeTzArray() const;
// [ 3] const bdlmxxx::Row& theRow() const;
// [ 3] const bdlmxxx::List& theList() const;
// [ 3] const bdlmxxx::Table& theTable() const;
// [ 3] const bdlmxxx::ChoiceArrayItem& theChoiceArrayItem() const;
// [ 3] const bdlmxxx::Choice& theChoice() const;
// [ 3] const bdlmxxx::ChoiceArray& theChoiceArray() const;
//
// [ 3] bool& theModifiableBool() const;
// [ 3] char& theModifiableChar() const;
// [ 3] short& theModifiableShort() const;
// [ 3] int& theModifiableInt() const;
// [ 3] bsls::Types::Int64& theModifiableInt64() const;
// [ 3] float& theModifiableFloat() const;
// [ 3] double& theModifiableDouble() const;
// [ 3] bsl::string& theModifiableString() const;
// [ 3] bdlt::Datetime& theModifiableDatetime() const;
// [ 3] bdlt::DatetimeTz& theModifiableDatetimeTz() const;
// [ 3] bdlt::Date& theModifiableDate() const;
// [ 3] bdlt::DateTz& theModifiableDateTz() const;
// [ 3] bdlt::Time& theModifiableTime() const;
// [ 3] bdlt::TimeTz& theModifiableTimeTz() const;
// [ 3] vector<bool>& theModifiableBoolArray() const;
// [ 3] vector<char>& theModifiableCharArray() const;
// [ 3] vector<short>& theModifiableShortArray() const;
// [ 3] vector<int>& theModifiableIntArray() const;
// [ 3] vector<bsls::Types::Int64>& theModifiableInt64Array() const;
// [ 3] vector<float>& theModifiableFloatArray() const;
// [ 3] vector<double>& theModifiableDoubleArray() const;
// [ 3] vector<bsl::string>& theModifiableStringArray() const;
// [ 3] vector<bdlt::Datetime>& theModifiableDatetimeArray() const;
// [ 3] vector<bdlt::DatetimeTz>& theModifiableDatetimeTzArray() const;
// [ 3] vector<bdlt::Date>& theModifiableDateArray() const;
// [ 3] vector<bdlt::DateTz>& theModifiableDateTzArray() const;
// [ 3] vector<bdlt::Time>& theModifiableTimeArray() const;
// [ 3] vector<bdlt::TimeTz>& theModifiableTimeTzArray() const;
// [ 3] bdlmxxx::Row& theModifiableRow() const;
// [ 3] bdlmxxx::List& theModifiableList() const;
// [ 3] bdlmxxx::Table& theModifiableTable() const;
// [ 3] bdlmxxx::ChoiceArrayItem& theModifiableChoiceArrayItem() const;
// [ 3] bdlmxxx::Choice& theModifiableChoice() const;
// [ 3] bdlmxxx::ChoiceArray& theModifiableChoiceArray() const;
// [ 2] void *data() const;
// [ 6] void replaceValue(const bdlmxxx::ConstElemRef& ro) const;
//
// FREE OPERATORS
// [ 7] const bdlmxxx::Row& theRow() const;
// [ 7] const bdlmxxx::ChoiceArrayItem& theChoiceArrayItem() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] INTUITIVE INTEROPERABILITY BETWEEN CLASSES
// [ 5] EQUALITY COMPARISONS WORK BETWEEN CONST/NON-CONST
// [ 1] CONST CORRECTNESS BETWEEN CLASSES (NEGATIVE)
// [ 1] ATTEMPTED RE-BINDING =) "COMPILE" (NOT "LINK") ERROR (NEGATIVE)
// [10] USAGE
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_  cout << "\t" << flush;            // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlmxxx::ElemType      ET;
typedef bdlmxxx::ElemRef       ERef;
typedef bdlmxxx::ConstElemRef  CERef;

typedef bsls::Types::Int64 Int64; // abbreviation

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// We have chosen to implement the following descriptors fully:
//
//     0. CHAR, 1. SHORT, 2. INT, 5. DOUBLE, and 6. STRING.
//
// The rest (except for TABLE, see below) have been dummied out such that only
// the type and a unique data address is provided.  Since 'bdlmxxx::List' and
// 'bdlmxxx::Table' do not exist at this level, we are going to implement the
// boundary 'bdlmxxx::Table' type as a 'float' (because we can).

                        // ------------------
                        // 0. CHAR DESCRIPTOR
                        // ------------------

static void unsetConstructChar(void                                      *obj,
                               bdlmxxx::AggregateOption::AllocationStrategy,
                               bslma::Allocator *)
{
    new(obj) char;
    bdltuxxx::Unset<char>::makeUnset(static_cast<char *>(obj));
}

static void copyConstructChar(void                                      *obj,
                              const void                                *other,
                              bdlmxxx::AggregateOption::AllocationStrategy,
                              bslma::Allocator *)
{
    new(obj) char(*static_cast<const char *>(other));
}

static void destroyChar(void *obj)
{
    //static_cast<char *>(obj)->~char();
}

static void assignChar(void *lhs, const void *rhs)
{
    *static_cast<char *>(lhs) = *static_cast<const char *>(rhs);
}

static void moveChar(void *lhs, void *rhs)
{
    bdlimpxxx::BitwiseCopy<char>::copy(static_cast<char *>(lhs),
                                   static_cast<char *>(rhs));
}

static void makeUnsetChar(void *obj)
{
    bdltuxxx::Unset<char>::makeUnset(static_cast<char *>(obj));
}

static bool isUnsetChar(const void *obj)
{
    return bdltuxxx::Unset<char>::isUnset(*static_cast<const char *>(obj));
}

static bool areEqualChar( const void *lhs, const void *rhs)
{
    return *static_cast<const char *>(lhs) ==
           *static_cast<const char *>(rhs);
}

static bsl::ostream& printChar(const void    *obj,
                               bsl::ostream&  stream,
                               int,
                               int)
{
    return stream << *static_cast<const char *>(obj);
}

static const bdlmxxx::Descriptor charDescriptor = {
    bdlmxxx::ElemType::BDEM_CHAR,     // enumeration constant for this type
    sizeof(char),                 // size of instances of this type
    bsls::AlignmentFromType<char>::VALUE,  // alignment requirement for this
                                           // type
    unsetConstructChar,
    copyConstructChar,
    destroyChar,
    assignChar,
    moveChar,
    makeUnsetChar,
    isUnsetChar,
    areEqualChar,
    printChar
};

                        // -------------------
                        // 1. SHORT DESCRIPTOR
                        // -------------------

static void unsetConstructShort(void                                      *obj,
                                bdlmxxx::AggregateOption::AllocationStrategy,
                                bslma::Allocator *)
{
    new(obj) short;
    bdltuxxx::Unset<short>::makeUnset(static_cast<short *>(obj));
}

static void copyConstructShort(
                              void                                      *obj,
                              const void                                *other,
                              bdlmxxx::AggregateOption::AllocationStrategy,
                              bslma::Allocator *)
{
    new(obj) short(*static_cast<const short *>(other));
}

static void destroyShort(void *obj)
{
//    static_cast<short *>(obj)->~short();
}

static void assignShort(void *lhs, const void *rhs)
{
    *static_cast<short *>(lhs) = *static_cast<const short *>(rhs);
}

static void moveShort(void *lhs, void *rhs)
{
    bdlimpxxx::BitwiseCopy<short>::copy(static_cast<short *>(lhs),
                                    static_cast<short *>(rhs));
}

static void makeUnsetShort(void *obj)
{
    bdltuxxx::Unset<short>::makeUnset(static_cast<short *>(obj));
}

static bool isUnsetShort(const void *obj)
{
    return bdltuxxx::Unset<short>::isUnset(*static_cast<const short *>(obj));
}

static bool areEqualShort( const void *lhs, const void *rhs)
{
    return *static_cast<const short *>(lhs) ==
           *static_cast<const short *>(rhs);
}

static bsl::ostream& printShort(const void    *obj,
                                bsl::ostream&  stream,
                                int,
                                int)
{
    return stream << *static_cast<const short *>(obj);
}

static const bdlmxxx::Descriptor shortDescriptor = {
    bdlmxxx::ElemType::BDEM_SHORT,     // enumeration constant for this type
    sizeof(short),                 // size of instances of this type
    bsls::AlignmentFromType<short>::VALUE,  // alignment requirement for this
                                            // type
    unsetConstructShort,
    copyConstructShort,
    destroyShort,
    assignShort,
    moveShort,
    makeUnsetShort,
    isUnsetShort,
    areEqualShort,
    printShort
};
                        // -----------------
                        // 2. INT DESCRIPTOR
                        // -----------------

static void unsetConstructInt(void                                      *obj,
                              bdlmxxx::AggregateOption::AllocationStrategy,
                              bslma::Allocator *)
{
    new(obj) int;
    bdltuxxx::Unset<int>::makeUnset(static_cast<int *>(obj));
}

static void copyConstructInt(void                                      *obj,
                             const void                                *other,
                             bdlmxxx::AggregateOption::AllocationStrategy,
                             bslma::Allocator *)
{
    new(obj) int(*static_cast<const int *>(other));
}

static void destroyInt(void *obj)
{
//    static_cast<int *>(obj)->~int();
}

static void assignInt(void *lhs, const void *rhs)
{
    *static_cast<int *>(lhs) = *static_cast<const int *>(rhs);
}

static void moveInt(void *lhs, void *rhs)
{
    bdlimpxxx::BitwiseCopy<int>::copy(static_cast<int *>(lhs),
                                  static_cast<int *>(rhs));
}

static void makeUnsetInt(void *obj)
{
    bdltuxxx::Unset<int>::makeUnset(static_cast<int *>(obj));
}

static bool isUnsetInt(const void *obj)
{
    return bdltuxxx::Unset<int>::isUnset(*static_cast<const int *>(obj));
}

static bool areEqualInt( const void *lhs, const void *rhs)
{
    return *static_cast<const int *>(lhs) ==
           *static_cast<const int *>(rhs);
}

static bsl::ostream& printInt(const void    *obj,
                              bsl::ostream&  stream,
                              int,
                              int)
{
    return stream << *static_cast<const int *>(obj);
}

static const bdlmxxx::Descriptor intDescriptor = {
    bdlmxxx::ElemType::BDEM_INT,     // enumeration constant for this type
    sizeof(int),                 // size of instances of this type
    bsls::AlignmentFromType<int>::VALUE, // alignment requirement for this type
    unsetConstructInt,
    copyConstructInt,
    destroyInt,
    assignInt,
    moveInt,
    makeUnsetInt,
    isUnsetInt,
    areEqualInt,
    printInt
};
                        // ---------------------------
                        // 3. INT64 DESCRIPTOR (DUMMY)
                        // ---------------------------

static const bdlmxxx::Descriptor int64Descriptor = {
    bdlmxxx::ElemType::BDEM_INT64,           // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

                        // ---------------------------
                        // 4. FLOAT DESCRIPTOR (DUMMY)
                        // ---------------------------

static const bdlmxxx::Descriptor floatDescriptor = {
    bdlmxxx::ElemType::BDEM_FLOAT,           // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

                        // --------------------
                        // 5. DOUBLE DESCRIPTOR
                        // --------------------

static void unsetConstructDouble(void                                     *obj,
                                 bdlmxxx::AggregateOption::AllocationStrategy,
                                 bslma::Allocator *)
{
    new(obj) double;
    bdltuxxx::Unset<double>::makeUnset(static_cast<double *>(obj));
}

static void copyConstructDouble(
                              void                                      *obj,
                              const void                                *other,
                              bdlmxxx::AggregateOption::AllocationStrategy,
                              bslma::Allocator *)
{
    new(obj) double(*static_cast<const double *>(other));
}

static void destroyDouble(void *obj)
{
//    static_cast<double *>(obj)->~double();
}

static void assignDouble(void *lhs, const void *rhs)
{
    *static_cast<double *>(lhs) = *static_cast<const double *>(rhs);
}

static void moveDouble(void *lhs, void *rhs)
{
    bdlimpxxx::BitwiseCopy<double>::copy(static_cast<double *>(lhs),
                                     static_cast<double *>(rhs));
}

static void makeUnsetDouble(void *obj)
{
    bdltuxxx::Unset<double>::makeUnset(static_cast<double *>(obj));
}

static bool isUnsetDouble(const void *obj)
{
    return bdltuxxx::Unset<double>::isUnset(*static_cast<const double *>(obj));
}

static bool areEqualDouble( const void *lhs, const void *rhs)
{
    return *static_cast<const double *>(lhs) ==
           *static_cast<const double *>(rhs);
}

static bsl::ostream& printDouble(const void    *obj,
                                 bsl::ostream&  stream,
                                 int,
                                 int)
{
    return stream << *static_cast<const double *>(obj);
}

static const bdlmxxx::Descriptor doubleDescriptor = {
    bdlmxxx::ElemType::BDEM_DOUBLE,     // enumeration constant for this type
    sizeof(double),                 // size of instances of this type
    bsls::AlignmentFromType<double>::VALUE,  // alignment requirement for this
                                             // type
    unsetConstructDouble,
    copyConstructDouble,
    destroyDouble,
    assignDouble,
    moveDouble,
    makeUnsetDouble,
    isUnsetDouble,
    areEqualDouble,
    printDouble
};

                        // --------------------
                        // 6. STRING DESCRIPTOR
                        // --------------------

static void unsetConstructString(
                              void                                      *obj,
                              bdlmxxx::AggregateOption::AllocationStrategy,
                              bslma::Allocator                          *alloc)
{
    new(obj) bsl::string(alloc);
    bdltuxxx::Unset<bsl::string>::makeUnset(static_cast<bsl::string *>(obj));
}

static void copyConstructString(
                              void                                      *obj,
                              const void                                *other,
                              bdlmxxx::AggregateOption::AllocationStrategy,
                              bslma::Allocator                          *alloc)
{
    new(obj) bsl::string(*static_cast<const bsl::string *>(other), alloc);
}

static void destroyString(void *obj)
{
//    static_cast<bsl::string *>(obj)->~bsl::string();
}

static void assignString(void *lhs, const void *rhs)
{
    *static_cast<bsl::string *>(lhs) = *static_cast<const bsl::string *>(rhs);
}

static void moveString(void *lhs, void *rhs)
{
    bdlimpxxx::BitwiseCopy<bsl::string>::copy(static_cast<bsl::string *>(lhs),
                                          static_cast<bsl::string *>(rhs));
}

static void makeUnsetString(void *obj)
{
    bdltuxxx::Unset<bsl::string>::makeUnset(static_cast<bsl::string *>(obj));
}

static bool isUnsetString(const void *obj)
{
    return bdltuxxx::Unset<bsl::string>::isUnset(
                                      *static_cast<const bsl::string *>(obj));
}

static bool areEqualString( const void *lhs, const void *rhs)
{
    return *static_cast<const bsl::string *>(lhs) ==
           *static_cast<const bsl::string *>(rhs);
}

static bsl::ostream& printString(const void    *obj,
                                 bsl::ostream&  stream,
                                 int,
                                 int)
{
    return stream << *static_cast<const bsl::string *>(obj);
}

static const bdlmxxx::Descriptor stringDescriptor = {
    bdlmxxx::ElemType::BDEM_STRING,          // enumeration constant for this type
    sizeof(bsl::string),                 // size of instances of this type
    bsls::AlignmentFromType<bsl::string>::VALUE,// alignment requirement for
                                                // this type
    unsetConstructString,
    copyConstructString,
    destroyString,
    assignString,
    moveString,
    makeUnsetString,
    isUnsetString,
    areEqualString,
    printString
};
                        // ----------------------------------------
                        // 7. DATETIME - 20. LIST DUMMY DESCRIPTORS
                        // ----------------------------------------

static const bdlmxxx::Descriptor voidDescriptor = {
    bdlmxxx::ElemType::BDEM_VOID,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const bdlmxxx::Descriptor boolDescriptor = {
    bdlmxxx::ElemType::BDEM_BOOL,             // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor datetimeDescriptor = {
    bdlmxxx::ElemType::BDEM_DATETIME,         // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor datetimeTzDescriptor = {
    bdlmxxx::ElemType::BDEM_DATETIMETZ,       // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor dateDescriptor = {
    bdlmxxx::ElemType::BDEM_DATE,             // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor dateTzDescriptor = {
    bdlmxxx::ElemType::BDEM_DATETZ,           // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor timeDescriptor = {
    bdlmxxx::ElemType::BDEM_TIME,             // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor timeTzDescriptor = {
    bdlmxxx::ElemType::BDEM_TIMETZ,           // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor boolArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_BOOL_ARRAY,       // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor charArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_CHAR_ARRAY,       // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor shortArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_SHORT_ARRAY,      // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor intArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_INT_ARRAY,        // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor int64ArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_INT64_ARRAY,      // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor floatArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_FLOAT_ARRAY,      // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor doubleArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY,     // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor stringArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_STRING_ARRAY,     // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor datetimeArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_DATETIME_ARRAY,   // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor datetimeTzArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_DATETIMETZ_ARRAY, // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor dateArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_DATE_ARRAY,       // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor dateTzArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_DATETZ_ARRAY,     // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor timeArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_TIME_ARRAY,       // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor timeTzArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_TIMETZ_ARRAY,     // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor rowDescriptor = {
    bdlmxxx::ElemType::BDEM_ROW,              // enumeration constant for this type
};
static const bdlmxxx::Descriptor listDescriptor = {
    bdlmxxx::ElemType::BDEM_LIST,             // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor choiceArrayItemDescriptor = {
    bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM,// enumeration constant for this type
};
static const bdlmxxx::Descriptor choiceDescriptor = {
    bdlmxxx::ElemType::BDEM_CHOICE,           // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const bdlmxxx::Descriptor choiceArrayDescriptor = {
    bdlmxxx::ElemType::BDEM_CHOICE_ARRAY,     // enumeration constant for this type
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

                // --------------------------------------------
                // 21. TABLE DESCRIPTOR -- IMPLEMENTED AS FLOAT
                // --------------------------------------------

namespace BloombergLP {

namespace bdlmxxx {
class Table {
    float d_data;
  public:
    Table() { }
    Table(float value) : d_data(value) { }
    ~Table() { }
    operator float&() { return d_data; }
    operator const float&() const { return d_data; }
};
}  // close package namespace

}  // close enterprise namespace

static void unsetConstructTable(void                                      *obj,
                                bdlmxxx::AggregateOption::AllocationStrategy,
                                bslma::Allocator *)
{
    new(obj) bdlmxxx::Table;
    bdltuxxx::Unset<float>::makeUnset(&(float&)*static_cast<bdlmxxx::Table *>(obj));
}

static void copyConstructTable(
                              void                                      *obj,
                              const void                                *other,
                              bdlmxxx::AggregateOption::AllocationStrategy,
                              bslma::Allocator *)
{
    new(obj) bdlmxxx::Table(*static_cast<const bdlmxxx::Table *>(other));
}

static void destroyTable(void *obj)
{
    static_cast<bdlmxxx::Table *>(obj)->~Table();
}

static void assignTable(void *lhs, const void *rhs)
{
    *static_cast<bdlmxxx::Table *>(lhs) = *static_cast<const bdlmxxx::Table *>(rhs);
}

static void moveTable(void *lhs, void *rhs)
{
    bdlimpxxx::BitwiseCopy<bdlmxxx::Table>::copy(static_cast<bdlmxxx::Table *>(lhs),
                                         static_cast<bdlmxxx::Table *>(rhs));
}

static void makeUnsetTable(void *obj)
{
    bdltuxxx::Unset<float>::makeUnset((float*) static_cast<bdlmxxx::Table *>(obj));
}

static bool isUnsetTable(const void *obj)
{
        return bdltuxxx::Unset<float>::isUnset((const float&)
                                        *static_cast<const bdlmxxx::Table *>(obj));
}

static bool areEqualTable( const void *lhs, const void *rhs)
{
    return *static_cast<const bdlmxxx::Table *>(lhs) ==
           *static_cast<const bdlmxxx::Table *>(rhs);
}

static bsl::ostream& printTable(const void    *obj,
                                bsl::ostream&  stream,
                                int,
                                int)
{
    return stream << *static_cast<const bdlmxxx::Table *>(obj);
}

static const bdlmxxx::Descriptor tableDescriptor = {
    bdlmxxx::ElemType::BDEM_TABLE,           // enumeration constant for this type
    sizeof(bdlmxxx::Table),                  // size of instances of this type
    bsls::AlignmentFromType<bdlmxxx::Table>::VALUE, // alignment requirement for
                                               // this type
    unsetConstructTable,
    copyConstructTable,
    destroyTable,
    assignTable,
    moveTable,
    makeUnsetTable,
    isUnsetTable,
    areEqualTable,
    printTable
};

// ----------------------------------------------------------------------------
// Descriptor lookup

struct {
    bdlmxxx::ElemType::Type    d_descIdx;
    const bdlmxxx::Descriptor *d_desc;
} descriptorLookup[] = {
    { ET::BDEM_CHAR,             &charDescriptor },
    { ET::BDEM_SHORT,            &shortDescriptor },
    { ET::BDEM_INT,              &intDescriptor },
    { ET::BDEM_INT64,            &int64Descriptor },
    { ET::BDEM_FLOAT,            &floatDescriptor },
    { ET::BDEM_DOUBLE,           &doubleDescriptor },
    { ET::BDEM_STRING,           &stringDescriptor },
    { ET::BDEM_DATETIME,         &datetimeDescriptor },
    { ET::BDEM_DATE,             &dateDescriptor },
    { ET::BDEM_TIME,             &timeDescriptor },
    { ET::BDEM_CHAR_ARRAY,       &charArrayDescriptor },
    { ET::BDEM_SHORT_ARRAY,      &shortArrayDescriptor },
    { ET::BDEM_INT_ARRAY,        &intArrayDescriptor },
    { ET::BDEM_INT64_ARRAY,      &int64ArrayDescriptor },
    { ET::BDEM_FLOAT_ARRAY,      &floatArrayDescriptor },
    { ET::BDEM_DOUBLE_ARRAY,     &doubleArrayDescriptor },
    { ET::BDEM_STRING_ARRAY,     &stringArrayDescriptor },
    { ET::BDEM_DATETIME_ARRAY,   &datetimeArrayDescriptor },
    { ET::BDEM_DATE_ARRAY,       &dateArrayDescriptor },
    { ET::BDEM_TIME_ARRAY,       &timeArrayDescriptor }
};

enum { DESCRIPTOR_LOOKUP_LENGTH = sizeof descriptorLookup /
                                                    sizeof *descriptorLookup };

// This object exists just to run it's c'tor and check that all the descriptors
// in 'descriptorLookup' are at their proper indexes.
struct CheckDescriptorLookup {
    CheckDescriptorLookup() {
        for (int i = 0; i < DESCRIPTOR_LOOKUP_LENGTH; ++i) {
            LOOP_ASSERT(i, (int) descriptorLookup[i].d_descIdx == i);
        }
    }
} checkDescriptorLookup;

bool isUnset(const CERef& ref)
    // Return 'true' if the specified element 'ref' references an element that
    // has the "unset" value for its type, and 'false' otherwise.
{
    return ref.descriptor()->isUnset(ref.data());
}

static const int         controlTestInt = 1;
static const int         controlBits    = 0x0C;
static const char        controlA1      = '1';
static const char        controlA2      = '2';
static const char        controlAU      = bdltuxxx::Unset<char>::unsetValue();
static const bsl::string controlB1      = "ONE";
static const bsl::string controlB2      = "TWO";
static const bsl::string controlBU      =
                                        bdltuxxx::Unset<bsl::string>::unsetValue();

int                      dataTestInt    = controlTestInt;
int                      dataNullBits   = controlBits;
char                     dataA1         = controlA1;
char                     dataA2         = controlA2;
char                     dataAU         = controlAU;
bsl::string              dataB1         = controlB1;
bsl::string              dataB2         = controlB2;
bsl::string              dataBU         = controlBU;

struct ELEMREFS {
    CERef       d_CER;
    const CERef d_cCER; // lowercase c stands for the const-ness of the
    ERef        d_ER;   // variable
    const ERef  d_cER;
};

ELEMREFS UNBOUND  = {
    bdlmxxx::ConstElemRef(0, &voidDescriptor),
    bdlmxxx::ConstElemRef(0, &voidDescriptor),
         bdlmxxx::ElemRef(0, &voidDescriptor),
         bdlmxxx::ElemRef(0, &voidDescriptor)
};
ELEMREFS UNBOUNDN  = {
    bdlmxxx::ConstElemRef(0, &voidDescriptor, &dataNullBits, 1),
    bdlmxxx::ConstElemRef(0, &voidDescriptor, &dataNullBits, 1),
         bdlmxxx::ElemRef(0, &voidDescriptor, &dataNullBits, 1),
         bdlmxxx::ElemRef(0, &voidDescriptor, &dataNullBits, 1)
};

ELEMREFS NA  = {
    bdlmxxx::ConstElemRef(&dataAU, &charDescriptor, &dataNullBits, 2),
    bdlmxxx::ConstElemRef(&dataAU, &charDescriptor, &dataNullBits, 2),
         bdlmxxx::ElemRef(&dataAU, &charDescriptor, &dataNullBits, 2),
         bdlmxxx::ElemRef(&dataAU, &charDescriptor, &dataNullBits, 2)
};
ELEMREFS NB  = {
    bdlmxxx::ConstElemRef(&dataBU, &stringDescriptor, &dataNullBits, 3),
    bdlmxxx::ConstElemRef(&dataBU, &stringDescriptor, &dataNullBits, 3),
         bdlmxxx::ElemRef(&dataBU, &stringDescriptor, &dataNullBits, 3),
         bdlmxxx::ElemRef(&dataBU, &stringDescriptor, &dataNullBits, 3)
};

ELEMREFS UA  = {
    bdlmxxx::ConstElemRef(&dataAU, &charDescriptor),
    bdlmxxx::ConstElemRef(&dataAU, &charDescriptor),
         bdlmxxx::ElemRef(&dataAU, &charDescriptor),
         bdlmxxx::ElemRef(&dataAU, &charDescriptor)
};
ELEMREFS UB  = {
    bdlmxxx::ConstElemRef(&dataBU, &stringDescriptor),
    bdlmxxx::ConstElemRef(&dataBU, &stringDescriptor),
         bdlmxxx::ElemRef(&dataBU, &stringDescriptor),
         bdlmxxx::ElemRef(&dataBU, &stringDescriptor)
};

ELEMREFS UNA  = {
    bdlmxxx::ConstElemRef(&dataAU, &charDescriptor, &dataNullBits, 4),
    bdlmxxx::ConstElemRef(&dataAU, &charDescriptor, &dataNullBits, 4),
         bdlmxxx::ElemRef(&dataAU, &charDescriptor, &dataNullBits, 4),
         bdlmxxx::ElemRef(&dataAU, &charDescriptor, &dataNullBits, 4)
};
ELEMREFS UNB  = {
    bdlmxxx::ConstElemRef(&dataBU, &stringDescriptor, &dataNullBits, 5),
    bdlmxxx::ConstElemRef(&dataBU, &stringDescriptor, &dataNullBits, 5),
         bdlmxxx::ElemRef(&dataBU, &stringDescriptor, &dataNullBits, 5),
         bdlmxxx::ElemRef(&dataBU, &stringDescriptor, &dataNullBits, 5)
};

// same memory and nullability information as na, NA, etc ...
ELEMREFS UNNA  = {
    bdlmxxx::ConstElemRef(NA.d_CER),
    bdlmxxx::ConstElemRef(NA.d_cCER),
         bdlmxxx::ElemRef(NA.d_ER),
         bdlmxxx::ElemRef(NA.d_cER)
};
ELEMREFS UNNB  = {
    bdlmxxx::ConstElemRef(NB.d_CER),
    bdlmxxx::ConstElemRef(NB.d_cCER),
        bdlmxxx::ElemRef(NB.d_ER),
        bdlmxxx::ElemRef(NB.d_cER)
};

ELEMREFS SA1  = {
    bdlmxxx::ConstElemRef(&dataA1, &charDescriptor),
    bdlmxxx::ConstElemRef(&dataA1, &charDescriptor),
         bdlmxxx::ElemRef(&dataA1, &charDescriptor),
         bdlmxxx::ElemRef(&dataA1, &charDescriptor)
};
ELEMREFS SNA1  = {
    bdlmxxx::ConstElemRef(&dataA1, &charDescriptor, &dataNullBits, 6),
    bdlmxxx::ConstElemRef(&dataA1, &charDescriptor, &dataNullBits, 6),
         bdlmxxx::ElemRef(&dataA1, &charDescriptor, &dataNullBits, 6),
         bdlmxxx::ElemRef(&dataA1, &charDescriptor, &dataNullBits, 6)
};

ELEMREFS SA2  = {
    bdlmxxx::ConstElemRef(&dataA2, &charDescriptor),
    bdlmxxx::ConstElemRef(&dataA2, &charDescriptor),
         bdlmxxx::ElemRef(&dataA2, &charDescriptor),
         bdlmxxx::ElemRef(&dataA2, &charDescriptor)
};
ELEMREFS SNA2  = {
    bdlmxxx::ConstElemRef(&dataA2, &charDescriptor, &dataNullBits, 7),
    bdlmxxx::ConstElemRef(&dataA2, &charDescriptor, &dataNullBits, 7),
         bdlmxxx::ElemRef(&dataA2, &charDescriptor, &dataNullBits, 7),
         bdlmxxx::ElemRef(&dataA2, &charDescriptor, &dataNullBits, 7)
};

ELEMREFS SB1  = {
    bdlmxxx::ConstElemRef(&dataB1, &stringDescriptor),
    bdlmxxx::ConstElemRef(&dataB1, &stringDescriptor),
         bdlmxxx::ElemRef(&dataB1, &stringDescriptor),
         bdlmxxx::ElemRef(&dataB1, &stringDescriptor)
};
ELEMREFS SNB1  = {
    bdlmxxx::ConstElemRef(&dataB1, &stringDescriptor, &dataNullBits, 8),
    bdlmxxx::ConstElemRef(&dataB1, &stringDescriptor, &dataNullBits, 8),
         bdlmxxx::ElemRef(&dataB1, &stringDescriptor, &dataNullBits, 8),
         bdlmxxx::ElemRef(&dataB1, &stringDescriptor, &dataNullBits, 8)
};

ELEMREFS SB2  = {
    bdlmxxx::ConstElemRef(&dataB2, &stringDescriptor),
    bdlmxxx::ConstElemRef(&dataB2, &stringDescriptor),
         bdlmxxx::ElemRef(&dataB2, &stringDescriptor),
         bdlmxxx::ElemRef(&dataB2, &stringDescriptor)
};
ELEMREFS SNB2  = {
    bdlmxxx::ConstElemRef(&dataB2, &stringDescriptor, &dataNullBits, 9),
    bdlmxxx::ConstElemRef(&dataB2, &stringDescriptor, &dataNullBits, 9),
         bdlmxxx::ElemRef(&dataB2, &stringDescriptor, &dataNullBits, 9),
         bdlmxxx::ElemRef(&dataB2, &stringDescriptor, &dataNullBits, 9)
};

//=============================================================================
//             GLOBAL HELPER FUNCTIONS AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

// static int globalVerbose = 0;          // set at top of main procedure
static int globalVeryVerbose = 0;         // set at top of main procedure
// static int globalVeryVeryVerbose = 0;  // set at top of main procedure

                        // ---------------
                        // TEST DESCRIPTOR
                        // ---------------

#if 0 // Keep this for reference in case we need additional functions.
...............................................................................
static void defaultConstructTest(
                                void                                      *obj,
                                bdlmxxx::AggregateOption::AllocationStrategy,
                                bslma::Allocator *)
{
    new(obj) char;
}

static void unsetConstructTest(void                                      *obj,
                               bdlmxxx::AggregateOption::AllocationStrategy,
                               bslma::Allocator *)
{
    new(obj) char;
    bdltuxxx::Unset<char>::makeUnset(static_cast<char *>(obj));
}

static void copyConstructTest(
                              void                                      *obj,
                              const void                                *other,
                              bdlmxxx::AggregateOption::AllocationStrategy,
                              bslma::Allocator *)
{
    new(obj) char(*static_cast<char *>(other));
}

static void destroyTest(void *obj)
{
    static_cast<char *>(obj)->~char();
}

static void assignTest(void *lhs, const void *rhs)
{
    *static_cast<char *>(lhs) = *static_cast<const char *>(rhs);
}

static void moveTest(void *lhs, void *rhs)
{
    bdlimpxxx::BitwiseCopy<char>::copy(static_cast<char *>(lhs),
                                   static_cast<char *>(rhs));
}

static void makeUnsetTest(void *obj)
{
    bdltuxxx::Unset<char>::makeUnset(static_cast<char *>(obj));
}

static bool isUnsetTest(const void *obj)
{
    return bdltuxxx::Unset<char>::isUnset(*static_cast<const char *>(obj));
}

static bool areEqualTest( const void *lhs, const void *rhs)
{
    return *static_cast<const char *>(lhs) ==
           *static_cast<const char *>(rhs);
}
...............................................................................

#endif

static bsl::ostream *globalPrintTest_stream;
static int           globalPrintTest_level;
static int           globalPrintTest_spacesPerLevel;

static bsl::ostream *globalPrintTest_result;

static char        latestAssignTest_firstByte;
static void       *latestAssignTest_lhs;
static const void *latestAssignTest_rhs;

static void assignTest(void *lhs, const void *rhs)
{
    if (globalVeryVerbose) {
        bsl::cout << "AssignTest: " << bsl::endl;
    }

    latestAssignTest_lhs = lhs;
    latestAssignTest_rhs = rhs;
    latestAssignTest_firstByte = *static_cast<const char *>(rhs);
}

static bsl::ostream& printTest(const void    *obj,
                               bsl::ostream&  stream,
                               int            level,
                               int            spacesPerLevel)
{
    if (globalVeryVerbose) {
        cout << "PrintTest: "; P_(obj); P_(&stream);
        P_(level); P_(spacesPerLevel); P(globalPrintTest_result);
    }

    globalPrintTest_stream         = &stream;
    globalPrintTest_level          = level;
    globalPrintTest_spacesPerLevel = spacesPerLevel;

    return *globalPrintTest_result;
}

static const bdlmxxx::Descriptor testDescriptor = {
    123,                                // enumeration constant for this type
    456,                                // size of instances of this type
    789,                                // alignment requirement for this type
    0,                                  // unsetConstruct
    0,                                  // copyConstruct
    0,                                  // destroy
    assignTest,                         // assign
    0,                                  // move
    0,                                  // makeUnset
    0,                                  // isUnset
    0,                                  // areEqual
    printTest                           // print
};

ELEMREFS TEST  = {
    bdlmxxx::ConstElemRef(&dataTestInt, &testDescriptor),
    bdlmxxx::ConstElemRef(&dataTestInt, &testDescriptor),
         bdlmxxx::ElemRef(&dataTestInt, &testDescriptor),
         bdlmxxx::ElemRef(&dataTestInt, &testDescriptor)
};
ELEMREFS TESTN  = {
    bdlmxxx::ConstElemRef(&dataTestInt, &testDescriptor, &dataNullBits, 0),
    bdlmxxx::ConstElemRef(&dataTestInt, &testDescriptor, &dataNullBits, 0),
         bdlmxxx::ElemRef(&dataTestInt, &testDescriptor, &dataNullBits, 0),
         bdlmxxx::ElemRef(&dataTestInt, &testDescriptor, &dataNullBits, 0)
};

void resetDataFromControls() {
    dataTestInt   = controlTestInt;
    dataNullBits  = controlBits;
    dataA1        = controlA1;
    dataA2        = controlA2;
    dataAU        = controlAU;
    dataB1        = controlB1;
    dataB2        = controlB2;
    dataBU        = controlBU;
}

void printERefs() {
    bsl::cout << "test reference" << bsl::endl;
    //T_; P_(TEST.d_CER); P_(TEST.d_cCER); P_(TEST.d_ER); P(TEST.d_cER);
    //T_; P_(TEST.d_CER); P_(TEST.d_cCER); P_(TEST.d_ER); P(TEST.d_cER);

    bsl::cout << "unbound reference" << bsl::endl;
    T_; P_(UNBOUND.d_CER);  P(UNBOUND.d_cCER);
    T_; P_(UNBOUND.d_ER); P(UNBOUND.d_cER);
    T_; P_(UNBOUNDN.d_CER); P(UNBOUNDN.d_cCER);
    T_; P_(UNBOUNDN.d_ER); P(UNBOUNDN.d_cER);

    bsl::cout << "null reference" << bsl::endl;
    T_; P_(NA.d_CER); P_(NA.d_cCER); P_(NA.d_ER); P(NA.d_cER);
    T_; P_(NB.d_CER); P_(NB.d_cCER); P_(NB.d_ER); P(NB.d_cER);

    bsl::cout << "unset reference, non-nullable" << bsl::endl;
    T_; P_(UA.d_CER); P_(UA.d_cCER); P_(UA.d_ER); P(UA.d_cER);
    T_; P_(UB.d_CER); P_(UB.d_cCER); P_(UB.d_ER); P(UB.d_cER);

    bsl::cout << "unset reference, nullable, non-null" << bsl::endl;
    T_; P_(UNA.d_CER); P_(UNA.d_cCER); P_(UNA.d_ER); P(UNA.d_cER);
    T_; P_(UNB.d_CER); P_(UNB.d_cCER); P_(UNB.d_ER); P(UNB.d_cER);

    bsl::cout << "unset reference, nullable, null" << bsl::endl;
    T_; P_(UNNA.d_CER); P_(UNNA.d_cCER); P_(UNNA.d_ER); P(UNNA.d_cER);
    T_; P_(UNNB.d_CER); P_(UNNB.d_cCER); P_(UNNB.d_ER); P(UNNB.d_cER);

    bsl::cout << "set (da1) reference" << bsl::endl;
    T_; P_(SA1.d_CER);  P_(SA1.d_cCER);  P_(SA1.d_ER);  P(SA1.d_cER);
    T_; P_(SNA1.d_CER); P_(SNA1.d_cCER); P_(SNA1.d_ER); P(SNA1.d_cER);

    bsl::cout << "set (da2) reference" << bsl::endl;
    T_; P_(SA2.d_CER);  P_(SA2.d_cCER);  P_(SA2.d_ER);  P(SA2.d_cER);
    T_; P_(SNA2.d_CER); P_(SNA2.d_cCER); P_(SNA2.d_ER); P(SNA2.d_cER);

    bsl::cout << "set (db1) reference" << bsl::endl;
    T_; P_(SB1.d_CER);  P_(SB1.d_cCER);  P_(SB1.d_ER);  P(SB1.d_cER);
    T_; P_(SNB1.d_CER); P_(SNB1.d_cCER); P_(SNB1.d_ER); P(SNB1.d_cER);

    bsl::cout << "set (db2) reference" << bsl::endl;
    T_; P_(SB2.d_CER);  P_(SB2.d_cCER);  P_(SB2.d_ER);  P(SB2.d_cER);
    T_; P_(SNB2.d_CER); P_(SNB2.d_cCER); P_(SNB2.d_ER); P(SNB2.d_cER);
}

bool referenceSameData(const bdlmxxx::ConstElemRef& ref1,
                       const bdlmxxx::ConstElemRef& ref2) {
    if (!ref1.isBound() && !ref2.isBound()) {
        return true;                                                  // RETURN
    }
    if (!ref1.isBound() || !ref2.isBound()) {
        return false;                                                 // RETURN
    }
    return ref1.data()       == ref2.data()
        && ref1.descriptor() == ref2.descriptor()
        && ref1.isNullable() == ref2.isNullable()
        && ref1.isNonNull()  == ref2.isNonNull()
        && ref1.isNull()     == ref2.isNull();
}

//=============================================================================
//                           SET UP FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------

                        // FIRST ``isMember'' IMPLEMENTATION
                        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    static bool isMember(const bdlmxxx::ConstElemRef  sequence[],
                         int                      length,
                         const bdlmxxx::ConstElemRef& element)
        // Return 'true' if the value of the specified 'element' (with
        // matching type) is contained in the specified 'sequence' of
        // the specified 'length', and 'false' otherwise.
    {
        for (int i = 0; i < length; ++i) {
            if (element == sequence[i]) {
                return true;                                          // RETURN
            }
        }
        return false;
    }

                        // SECOND ``isMember'' IMPLEMENTATION
                        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    static bool isMember(const void                *sequenceData[],
                         const bdlmxxx::ElemType::Type  sequenceTypes[],
                         int                        sequenceLength,
                         const void                *dataValue,
                         bdlmxxx::ElemType::Type        dataType)
        // Return 'true' if the element value and type indicated by the
        // specified 'dataValue' and 'dataType' respectively, match those of
        // an element in the sequence indicated by the specified 'sequenceData'
        // and 'sequenceTypes', each of the specified 'sequenceLength', and
        // 'false' otherwise.
    {
        for (int i = 0; i < sequenceLength; ++i) {
            if (dataType != sequenceTypes[i]) {
                continue;  // no need to compare values!
            }
            switch (dataType) {                 // Same type; must compare.
              case bdlmxxx::ElemType::BDEM_CHAR: {
                if (*static_cast<const char *>(dataValue) ==
                    *static_cast<const char *>(sequenceData[i])) return true;
                                                                      // RETURN
              } break;
              case bdlmxxx::ElemType::BDEM_SHORT: {
                if (*static_cast<const short *>(dataValue) ==
                    *static_cast<const short *>(sequenceData[i])) return true;
                                                                      // RETURN
              } break;
              case bdlmxxx::ElemType::BDEM_INT: {
                if (*static_cast<const int *>(dataValue) ==
                    *static_cast<const int *>(sequenceData[i])) return true;
                                                                      // RETURN
              } break;
              case bdlmxxx::ElemType::BDEM_INT64: {
                if (*static_cast<const Int64 *>(dataValue) ==
                    *static_cast<const Int64 *>(sequenceData[i])) return true;
                                                                      // RETURN
              } break;
              case bdlmxxx::ElemType::BDEM_FLOAT: {
                if (*static_cast<const float *>(dataValue) ==
                    *static_cast<const float *>(sequenceData[i])) return true;
                                                                      // RETURN
              } break;
              case bdlmxxx::ElemType::BDEM_DOUBLE: {
                if (*static_cast<const double *>(dataValue) ==
                    *static_cast<const double *>(sequenceData[i])) return true;
                                                                      // RETURN
              } break;
              case bdlmxxx::ElemType::BDEM_STRING: {
                typedef bsl::string String;
                if (*static_cast<const String *>(dataValue) ==
                    *static_cast<const String *>(sequenceData[i])) return true;
                                                                      // RETURN
              } break;
                // .
                // .   (14 case statements omitted)
                // .
              case bdlmxxx::ElemType::BDEM_TABLE: {
                typedef bdlmxxx::Table Table;
                if (*static_cast<const Table *>(dataValue) ==
                    *static_cast<const Table *>(sequenceData[i])) return true;
                                                                      // RETURN
              } break;
              default: {
                ASSERT("Error: Enum Value out of range [0..21]" && 0);
              }
            } // switch
        } // for
        return false;
    }

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;            // globalVerbose = verbose;
    int veryVerbose = argc > 3;           globalVeryVerbose = veryVerbose;
    // int veryVeryVerbose = argc > 4; // globalVeryVeryVerbose =
                                       //                      veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   This case verifies that the usage example works as advertised.
        //
        // Concerns:
        //   - That the usage example compiles, links, and runs as expected.
        //
        // Plan:
        //   - Replicate the usage example here
        //
        // Tactics:
        //   - Ad-Hoc test data selection method
        //   - Brute Force implementation technique
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;
///Usage
///-----
// A 'bdlmxxx::ConstElemRef' or 'bdlmxxx::ElemRef', is a proxy for a specific (typed)
// non-modifiable or modifiable element (respectively), which can be used to
// facilitate communication across intermediaries that do not care about the
// specific type or value of that element.  For example, suppose we have a
// heterogeneous collection of values whose types correspond to those
// identified by the enumeration 'bdlmxxx::ElemType::Type':
//..
    // ...
    char        aChar;
    int         anInt;
    int         anotherInt;
    double      myDouble;
    bsl::string theString;        // not at file scope!
    short       someShort;
    double      yourDouble;

    charDescriptor.makeUnset(&aChar);
    intDescriptor.makeUnset(&anInt);
    intDescriptor.makeUnset(&anotherInt);
    doubleDescriptor.makeUnset(&myDouble);
    stringDescriptor.makeUnset(&theString);
    shortDescriptor.makeUnset(&someShort);
    doubleDescriptor.makeUnset(&yourDouble);
//..
// Suppose further that we want to create an 'isMember' function to determine
// whether a specified value (of the same type) is a member of that collection.
// We could create a linear sequence of NULL element references as follows:
//..
    int nullBits = 0x7F;

    const int LENGTH = 7;

    bsls::ObjectBuffer<bdlmxxx::ElemRef> rawSpaceForList[LENGTH];

    bdlmxxx::ElemRef *ref = reinterpret_cast<bdlmxxx::ElemRef *> (rawSpaceForList);
    new(ref + 0) bdlmxxx::ElemRef(&aChar     , &charDescriptor  , &nullBits, 0);
    new(ref + 1) bdlmxxx::ElemRef(&anInt     , &intDescriptor   , &nullBits, 1);
    new(ref + 2) bdlmxxx::ElemRef(&anotherInt, &intDescriptor   , &nullBits, 2);
    new(ref + 3) bdlmxxx::ElemRef(&myDouble  , &doubleDescriptor, &nullBits, 3);
    new(ref + 4) bdlmxxx::ElemRef(&theString , &stringDescriptor, &nullBits, 4);
    new(ref + 5) bdlmxxx::ElemRef(&someShort , &shortDescriptor , &nullBits, 5);
    new(ref + 6) bdlmxxx::ElemRef(&yourDouble, &doubleDescriptor, &nullBits, 6);

    for (int refIndex = 0; refIndex < LENGTH; ++refIndex) {
        ASSERT(ref[refIndex].isNull());
    }
//..
// We can then assign specific values to the elements by taking modifiable
// references to them.  The act of taking a modifiable reference makes them
// non-null as follows:
//..
    ref[0].theModifiableChar() = 'a';
    ref[1].theModifiableInt() = 10;
    ref[2].theModifiableInt() = 20;
    ref[3].theModifiableDouble() = 12.5;
    ref[4].theModifiableString() = "Foo Bar";
    ref[5].theModifiableShort() = -5;
    ref[6].theModifiableDouble() = 34.75;

    for (int refIndex = 0; refIndex < LENGTH; ++refIndex) {
        ASSERT(ref[refIndex].isNonNull());
    }
//..
// Note that the creation of the various 'bdlmxxx::Descriptor' structures is
// not shown.  See 'bdlmxxx_descriptor' for more information on descriptors.
// Also note that descriptors corresponding to each of 22 'bdlmxxx::ElemType'
// enumerators can be found in the 'bdlmxxx_properties', 'bdlmxxx_listimp', and
// 'bdlmxxx_tableimp' components, respectively.
//
// We can now write the 'isMember' function (at file scope) as follows:
//..
                        // FIRST ``isMember'' IMPLEMENTATION
                        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//..
// As clients, we can invoke this function as follows:
//..
    const int   V1 = 20;
    const int   V2 = 15;
    bsl::string v3 = "Foo";     bdlmxxx::ConstElemRef cer3(&v3, &stringDescriptor);
    bsl::string v4 = "Foo Bar"; const bdlmxxx::ElemRef ER4(&v4, &stringDescriptor);

    ASSERT(1 == isMember(ref, 7, bdlmxxx::ConstElemRef(&V1, &intDescriptor)));
    ASSERT(0 == isMember(ref, 7, bdlmxxx::ConstElemRef(&V2, &intDescriptor)));
    ASSERT(0 == isMember(ref, 7, cer3));
    ASSERT(1 == isMember(ref, 5, ER4))
    ASSERT(0 == isMember(ref, 4, ER4));
//..
// An excellent use of 'bdlmxxx::ElemRef' as well as 'bdlmxxx::ConstElemRef' can be
// found in the 'bdlmxxx::Row' component'.
//
// Without 'bdlmxxx::ConstElemRef' (and the underlying 'bdlmxxx::Descriptor'), we
// would represent an element as a pointer to its data and its type:
//..
    const void *DATA[] = {                     // Should NOT be at file scope.
        &aChar,      &anInt,     &anotherInt,
        &myDouble,   &theString, &someShort,   // String is not POD.
        &yourDouble,
    };

    const bdlmxxx::ElemType::Type TYPES[] = {      // Could be at file scope.
        bdlmxxx::ElemType::BDEM_CHAR,   bdlmxxx::ElemType::BDEM_INT,
        bdlmxxx::ElemType::BDEM_INT,    bdlmxxx::ElemType::BDEM_DOUBLE,
        bdlmxxx::ElemType::BDEM_STRING, bdlmxxx::ElemType::BDEM_SHORT,
        bdlmxxx::ElemType::BDEM_DOUBLE
    };
//..
// Without element references, the 'isMember' function would be much more
// cumbersome to write.  In particular, 'isMember' would need first to check
// the type and, if it is the same as the element in question, cast both
// elements to that type before invoking the native ('operator==') comparison:
//..
                        // SECOND ``isMember'' IMPLEMENTATION
                        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//..
// A client could invoke this second 'isMember' implementation as follows:
//..
    ASSERT(1 == isMember(DATA, TYPES, 7, &V1, bdlmxxx::ElemType::BDEM_INT));
    ASSERT(0 == isMember(DATA, TYPES, 7, &V2, bdlmxxx::ElemType::BDEM_INT));
    ASSERT(0 == isMember(DATA, TYPES, 7, &v3, bdlmxxx::ElemType::BDEM_STRING));
    ASSERT(1 == isMember(DATA, TYPES, 5, &v4, bdlmxxx::ElemType::BDEM_STRING));
    ASSERT(0 == isMember(DATA, TYPES, 4, &v4, bdlmxxx::ElemType::BDEM_STRING));
//..
// Element references are valuable because the alternative, in addition to
// being bulky and error-prone, forces not only a *link-time*, but also a
// *COMPILE-TIME*, physical dependency of 'isMember' on *SPECIFIC*
// *IMPLEMENTATIONS* of *ALL* 22 types -- even though many implementations
// (e.g., of datetime, date, time, all ten arrays, list, and table) are not
// needed by the client of this code.

// ############################################################################

        if (verbose) cout <<
            "\nAdditional Tests not part of usage example proper." << endl;
        const char        X0 = 'a';
        const int         X1 = 10;
        const int         X2 = 20;
        const double      X3 = 12.5;
        const bsl::string X4 = "Foo Bar";
        const short       X5 = -5;
        const double      X6 = 34.75;
        const void *x;

        const bdlmxxx::ConstElemRef Y0(&X0, &charDescriptor);
        const bdlmxxx::ConstElemRef Y1(&X1, &intDescriptor);
        const bdlmxxx::ConstElemRef Y2(&X2, &intDescriptor);
        const bdlmxxx::ConstElemRef Y3(&X3, &doubleDescriptor);
        const bdlmxxx::ConstElemRef Y4(&X4, &stringDescriptor);
        const bdlmxxx::ConstElemRef Y5(&X5, &shortDescriptor);
        const bdlmxxx::ConstElemRef Y6(&X6, &doubleDescriptor);
        const bdlmxxx::ConstElemRef *y;

        const bdlmxxx::ElemType::Type Z0 = bdlmxxx::ElemType::BDEM_CHAR;
        const bdlmxxx::ElemType::Type Z1 = bdlmxxx::ElemType::BDEM_INT;
        const bdlmxxx::ElemType::Type Z2 = bdlmxxx::ElemType::BDEM_INT;
        const bdlmxxx::ElemType::Type Z3 = bdlmxxx::ElemType::BDEM_DOUBLE;
        const bdlmxxx::ElemType::Type Z4 = bdlmxxx::ElemType::BDEM_STRING;
        const bdlmxxx::ElemType::Type Z5 = bdlmxxx::ElemType::BDEM_SHORT;
        const bdlmxxx::ElemType::Type Z6 = bdlmxxx::ElemType::BDEM_DOUBLE;
        bdlmxxx::ElemType::Type z;

        int i;

        if (verbose) cout << "\tVerifying first implementation." << endl;

        i = 0; y = &Y0;
        ASSERT(1 == isMember(ref,     i+1,   *y));
        ASSERT(0 == isMember(ref,     i,     *y));
        ASSERT(0 == isMember(ref+i+1, 7-i-1, *y));
        ASSERT(1 == isMember(ref+i,   7-i,   *y));

        i = 1; y = &Y1;
        ASSERT(1 == isMember(ref,     i+1,   *y));
        ASSERT(0 == isMember(ref,     i,     *y));
        ASSERT(0 == isMember(ref+i+1, 7-i-1, *y));
        ASSERT(1 == isMember(ref+i,   7-i,   *y));

        i = 2; y = &Y2;
        ASSERT(1 == isMember(ref,     i+1,   *y));
        ASSERT(0 == isMember(ref,     i,     *y));
        ASSERT(0 == isMember(ref+i+1, 7-i-1, *y));
        ASSERT(1 == isMember(ref+i,   7-i,   *y));

        i = 3; y = &Y3;
        ASSERT(1 == isMember(ref,     i+1,   *y));
        ASSERT(0 == isMember(ref,     i,     *y));
        ASSERT(0 == isMember(ref+i+1, 7-i-1, *y));
        ASSERT(1 == isMember(ref+i,   7-i,   *y));

        i = 4; y = &Y4;
        ASSERT(1 == isMember(ref,     i+1,   *y));
        ASSERT(0 == isMember(ref,     i,     *y));
        ASSERT(0 == isMember(ref+i+1, 7-i-1, *y));
        ASSERT(1 == isMember(ref+i,   7-i,   *y));

        i = 5; y = &Y5;
        ASSERT(1 == isMember(ref,     i+1,   *y));
        ASSERT(0 == isMember(ref,     i,     *y));
        ASSERT(0 == isMember(ref+i+1, 7-i-1, *y));
        ASSERT(1 == isMember(ref+i,   7-i,   *y));

        i = 6; y = &Y6;
        ASSERT(1 == isMember(ref,     i+1,   *y));
        ASSERT(0 == isMember(ref,     i,     *y));
        ASSERT(0 == isMember(ref+i+1, 7-i-1, *y));
        ASSERT(1 == isMember(ref+i,   7-i,   *y));

        if (verbose) cout << "\tVerifying second implementation." << endl;

        i = 0; x = &X0; z = Z0;
        ASSERT(1 == isMember(DATA,     TYPES,     i+1,   x, z));
        ASSERT(0 == isMember(DATA,     TYPES,     i,     x, z));
        ASSERT(0 == isMember(DATA+i+1, TYPES+i+1, 7-i-1, x, z));
        ASSERT(1 == isMember(DATA+i,   TYPES+i,   7-i,   x, z));

        i = 1; x = &X1; z = Z1;
        ASSERT(1 == isMember(DATA,     TYPES,     i+1,   x, z));
        ASSERT(0 == isMember(DATA,     TYPES,     i,     x, z));
        ASSERT(0 == isMember(DATA+i+1, TYPES+i+1, 7-i-1, x, z));
        ASSERT(1 == isMember(DATA+i,   TYPES+i,   7-i,   x, z));

        i = 2; x = &X2; z = Z2;
        ASSERT(1 == isMember(DATA,     TYPES,     i+1,   x, z));
        ASSERT(0 == isMember(DATA,     TYPES,     i,     x, z));
        ASSERT(0 == isMember(DATA+i+1, TYPES+i+1, 7-i-1, x, z));
        ASSERT(1 == isMember(DATA+i,   TYPES+i,   7-i,   x, z));

        i = 3; x = &X3; z = Z3;
        ASSERT(1 == isMember(DATA,     TYPES,     i+1,   x, z));
        ASSERT(0 == isMember(DATA,     TYPES,     i,     x, z));
        ASSERT(0 == isMember(DATA+i+1, TYPES+i+1, 7-i-1, x, z));
        ASSERT(1 == isMember(DATA+i,   TYPES+i,   7-i,   x, z));

        i = 4; x = &X4; z = Z4;
        ASSERT(1 == isMember(DATA,     TYPES,     i+1,   x, z));
        ASSERT(0 == isMember(DATA,     TYPES,     i,     x, z));
        ASSERT(0 == isMember(DATA+i+1, TYPES+i+1, 7-i-1, x, z));
        ASSERT(1 == isMember(DATA+i,   TYPES+i,   7-i,   x, z));

        i = 5; x = &X5; z = Z5;
        ASSERT(1 == isMember(DATA,     TYPES,     i+1,   x, z));
        ASSERT(0 == isMember(DATA,     TYPES,     i,     x, z));
        ASSERT(0 == isMember(DATA+i+1, TYPES+i+1, 7-i-1, x, z));
        ASSERT(1 == isMember(DATA+i,   TYPES+i,   7-i,   x, z));

        i = 6; x = &X6; z = Z6;
        ASSERT(1 == isMember(DATA,     TYPES,     i+1,   x, z));
        ASSERT(0 == isMember(DATA,     TYPES,     i,     x, z));
        ASSERT(0 == isMember(DATA+i+1, TYPES+i+1, 7-i-1, x, z));
        ASSERT(1 == isMember(DATA+i,   TYPES+i,   7-i,   x, z));

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //   Any reference must be able to be copy constructed without
        //   affecting its argument.  The new object must reference the same
        //   memory (data, descriptor and nullBits) that the object being
        //   copied references.
        //
        // Plan:
        //
        // Testing:
        //   bdlmxxx::ConstElemRef(const bdlmxxx::ConstElemRef& original);
        //   bdlmxxx::ElemRef(const bdlmxxx::ElemRef& original);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING COPY CONSTRUCTOR"
                               << "\n========================" << bsl::endl;

        if (veryVerbose) printERefs();

        const struct {
            int              d_line;
            const char      *d_description;
            const ELEMREFS  *d_elemRefs;
        } DATA[] = {
            { L_, "unbound",                   &UNBOUND,  },
            { L_, "unbound (nullable)",        &UNBOUNDN, },
            { L_, "null (a)",                  &NA,       },
            { L_, "unset (non-nullable, a)",   &UA        },
            { L_, "unset (nullable, a)",       &UNA       },
            { L_, "unset (nullable, null, a)", &UNNA      },
            { L_, "set (non-nullable, a1)",    &SA1       },
            { L_, "set (non-nullable, a2)",    &SA2       },
            { L_, "set (nullable, a1)",        &SNA1      },
            { L_, "set (nullable, a2)",        &SNA2      },

            { L_, "null (b)",                  &NB        },
            { L_, "unset (non-nullable, b)",   &UB        },
            { L_, "unset (nullable, b)",       &UNB       },
            { L_, "unset (nullable, null, b)", &UNNB      },
            { L_, "set (non-nullable, b1)",    &SB1       },
            { L_, "set (non-nullable, b2)",    &SB2       },
            { L_, "set (nullable, b1)",        &SNB1      },
            { L_, "set (nullable, b2)",        &SNB2      }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int              LINE        = DATA[i].d_line;
            const char     *const  DESCRIPTION = DATA[i].d_description;
            const ELEMREFS *const  EREFS       = DATA[i].d_elemRefs;

            if (veryVerbose) bsl::cout << "\t" << DESCRIPTION << bsl::endl;

            {
                const bdlmxxx::ConstElemRef CER(EREFS->d_cCER);
                LOOP_ASSERT(LINE, referenceSameData(EREFS->d_cCER, CER));
            }
            {
                const bdlmxxx::ConstElemRef CER(EREFS->d_CER);
                LOOP_ASSERT(LINE, referenceSameData(EREFS->d_CER, CER));
            }
            {
                const bdlmxxx::ConstElemRef CER(EREFS->d_cER);
                LOOP_ASSERT(LINE, referenceSameData(EREFS->d_cER, CER));
            }
            {
                const bdlmxxx::ConstElemRef CER(EREFS->d_ER);
                LOOP_ASSERT(LINE, referenceSameData(EREFS->d_ER, CER));
            }
            {
                bdlmxxx::ConstElemRef cer(EREFS->d_cCER);
                LOOP_ASSERT(LINE, referenceSameData(EREFS->d_cCER, cer));
            }
            {
                bdlmxxx::ConstElemRef cer(EREFS->d_CER);
                LOOP_ASSERT(LINE, referenceSameData(EREFS->d_CER, cer));
            }
            {
                bdlmxxx::ConstElemRef cer(EREFS->d_cER);
                LOOP_ASSERT(LINE, referenceSameData(EREFS->d_cER, cer));
            }
            {
                bdlmxxx::ConstElemRef cer(EREFS->d_ER);
                LOOP_ASSERT(LINE, referenceSameData(EREFS->d_ER, cer));
            }
            {
                // const bdlmxxx::ElemRef ER(EREFS->d_cCER); // should not compile
            }
            {
                // const bdlmxxx::ElemRef ER(EREFS->d_CER); // should not compile
            }
            {
                const bdlmxxx::ElemRef ER(EREFS->d_cER);
                LOOP_ASSERT(LINE, referenceSameData(EREFS->d_cER, ER));
            }
            {
                const bdlmxxx::ElemRef ER(EREFS->d_ER);
                LOOP_ASSERT(LINE, referenceSameData(EREFS->d_ER, ER));
            }
            {
                // bdlmxxx::ElemRef er(EREFS->d_cCER); // should not compile
            }
            {
                // bdlmxxx::ElemRef er(EREFS->d_CER); // should not compile
            }
            {
                bdlmxxx::ElemRef er(EREFS->d_cER);
                LOOP_ASSERT(LINE, referenceSameData(EREFS->d_cER, er));
            }
            {
                bdlmxxx::ElemRef er(EREFS->d_ER);
                LOOP_ASSERT(LINE, referenceSameData(EREFS->d_ER, er));
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // DESCRIPTOR-BASED METHODS
        //   This case verifies remaining functions that require descriptors.
        //
        // Concerns:
        //   - That the function forwards the arguments to the function
        //     and/or forwards the result back to the caller.
        //   - Note that issues such as aliasing and exception neutrality are
        //     entirely the responsibility of the descriptor callback, and are
        //     not a concern here.
        //   - That ==, !=, << and 'replaceValue' accept 'bdlmxxx::ConstElemRef'
        //     arguments.
        //
        // Plan:
        //   - First create two type descriptors and two instances of each
        //     type.
        //   - Try each of the methods on these types just to be sure.
        //   - Then, where needed, use a special test descriptor initialized
        //     with custom test functions that capture the arguments in global
        //     variables for subsequent verification by the test script.  Any
        //     return values from such functions will be similarly settable
        //     from globally accessible variables.
        //   - Make sure that 'bdlmxxx::ConstElemRef' arguments are supplied.
        //
        // Tactics:
        //   - Ad-Hoc test data selection method
        //   - Brute Force implementation technique
        //
        // Testing:
        //                      GENERAL
        //                      ~~~~~~~
        //   INTUITIVE INTEROPERABILITY BETWEEN CLASSES
        //   EQUALITY COMPARISONS WORK BETWEEN CONST/NON-CONST
        //
        //                      bdlmxxx::ConstElemRef
        //                      ~~~~~~~~~~~~~~~~~
        //   bdlmxxx::ElemType::Type type() const;
        //   bsl::ostream& print(bsl::ostream& s, int l, int spl) const;
        //   operator==(const bdlmxxx::ConstElemRef&, const bdlmxxx::ConstElemRef&);
        //   operator!=(const bdlmxxx::ConstElemRef&, const bdlmxxx::ConstElemRef&);
        //   operator<<(bsl::ostream&, const bdlmxxx::ConstElemRef&);
        //
        //                      bdlmxxx::ElemRef
        //                      ~~~~~~~~~~~~
        //   void makeNull() const;
        //   void replaceValue(const bdlmxxx::ConstElemRef& ro) const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nDESCRIPTOR-BASED METHODS"
                               << "\n========================" << bsl::endl;

        if (veryVerbose) printERefs();

        if (verbose) bsl::cout << "\ttype" << bsl::endl;
        {
            const struct {
                int             d_line;
                const char     *d_description;
                const ELEMREFS *d_elemRefs;
                ET::Type        d_expectedType;
            } DATA[] = {
                // Line        Description           ERefs     ET
                // ==== ===========================  =====  ========
                { L_, "null (a)",                  &NA,   ET::BDEM_CHAR,   },
                { L_, "unset (non-nullable, a)",   &UA,   ET::BDEM_CHAR,   },
                { L_, "unset (nullable, a)",       &UNA,  ET::BDEM_CHAR,   },
                { L_, "unset (nullable, null, a)", &UNNA, ET::BDEM_CHAR,   },
                { L_, "set (non-nullable, a1)",    &SA1,  ET::BDEM_CHAR,   },
                { L_, "set (non-nullable, a2)",    &SA2,  ET::BDEM_CHAR,   },
                { L_, "set (nullable, a1)",        &SNA1, ET::BDEM_CHAR,   },
                { L_, "set (nullable, a2)",        &SNA2, ET::BDEM_CHAR,   },

                { L_, "null (b)",                  &NB,   ET::BDEM_STRING, },
                { L_, "unset (non-nullable, b)",   &UB,   ET::BDEM_STRING, },
                { L_, "unset (nullable, b)",       &UNB,  ET::BDEM_STRING, },
                { L_, "unset (nullable, null, b)", &UNNB, ET::BDEM_STRING, },
                { L_, "set (non-nullable, b1)",    &SB1,  ET::BDEM_STRING, },
                { L_, "set (non-nullable, b2)",    &SB2,  ET::BDEM_STRING, },
                { L_, "set (nullable, b1)",        &SNB1, ET::BDEM_STRING, },
                { L_, "set (nullable, b2)",        &SNB2, ET::BDEM_STRING, }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE        = DATA[i].d_line;
                const char     *const DESCRIPTION = DATA[i].d_description;
                const ELEMREFS *const EREFS       = DATA[i].d_elemRefs;
                const ET::Type        ET          = DATA[i].d_expectedType;

                if (veryVerbose) bsl::cout << "\t\t" << DESCRIPTION
                                           << bsl::endl;

                LOOP_ASSERT(LINE, ET == EREFS->d_CER.type());
                LOOP_ASSERT(LINE, ET == EREFS->d_cCER.type());
                LOOP_ASSERT(LINE, ET == EREFS->d_ER.type());
                LOOP_ASSERT(LINE, ET == EREFS->d_cER.type());
            }
        }

        if (verbose) bsl::cout << "\tisNonNull/isNull/isUnset" << bsl::endl;
        {
            const struct {
                int             d_line;
                const char     *d_description;
                const ELEMREFS *d_elemRefs;
                bool            d_isNull;
                bool            d_isUnset;
            } DATA[] = {
                //Line Description                      ERefs  null   unset
                //---- -------------------------------- -----  -----  -----
                { L_,  "null (a)",                      &NA,   false, true   },
                { L_,  "null (b)",                      &NB,   false, true   },
                { L_,  "unset (non-nullable, a)",       &UA,   true,  true   },
                { L_,  "unset (non-nullable, b)",       &UB,   true,  true   },
                { L_,  "unset (nullable, non-null, a)", &UNA,  true,  true   },
                { L_,  "unset (nullable, non-null, a)", &UNB,  true,  true   },
                { L_,  "unset (nullable, null, a)",     &UNNA, false, true   },
                { L_,  "unset (nullable, null, a)",     &UNNB, false, true   },
                { L_,  "set (non-nullable, a1)",        &SA1,  true,  false  },
                { L_,  "set (non-nullable, a2)",        &SA2,  true,  false  },
                { L_,  "set (non-nullable, b1)",        &SB1,  true,  false  },
                { L_,  "set (non-nullable, b2)",        &SB2,  true,  false  },
                { L_,  "set (nullable, a1)",            &SNA1, true,  false  },
                { L_,  "set (nullable, a2)",            &SNA2, true,  false  },
                { L_,  "set (nullable, b1)",            &SNB1, true,  false  },
                { L_,  "set (nullable, b2)",            &SNB2, true,  false  }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE        = DATA[i].d_line;
                const char     *const DESCRIPTION = DATA[i].d_description;
                const ELEMREFS *const EREFS       = DATA[i].d_elemRefs;
                const bool            EVNull      = DATA[i].d_isNull;
                const bool            EVUnset     = DATA[i].d_isUnset;

                if (veryVerbose) bsl::cout << "\t\t" << DESCRIPTION
                                           << bsl::endl;

                LOOP_ASSERT(LINE, EVNull  == EREFS->d_CER.isNonNull());
                LOOP_ASSERT(LINE, EVNull  != EREFS->d_CER.isNull());
                LOOP_ASSERT(LINE, EVUnset == isUnset(EREFS->d_CER));
                LOOP_ASSERT(LINE, EVNull  == EREFS->d_cCER.isNonNull());
                LOOP_ASSERT(LINE, EVNull  != EREFS->d_cCER.isNull());
                LOOP_ASSERT(LINE, EVUnset == isUnset(EREFS->d_cCER));
                LOOP_ASSERT(LINE, EVNull  == EREFS->d_ER.isNonNull());
                LOOP_ASSERT(LINE, EVNull  != EREFS->d_ER.isNull());
                LOOP_ASSERT(LINE, EVUnset == isUnset(EREFS->d_ER));
                LOOP_ASSERT(LINE, EVNull  == EREFS->d_cER.isNonNull());
                LOOP_ASSERT(LINE, EVNull  != EREFS->d_cER.isNull());
                LOOP_ASSERT(LINE, EVUnset == isUnset(EREFS->d_cER));
            }
        }

        if (verbose) bsl::cout << "\tisNull/isNonNull" << bsl::endl;
        {
            const struct {
                int             d_line;
                const char     *d_description;
                const ELEMREFS *d_elemRefs;
                bool            d_expectedValue;
            } DATA[] = {
                // Line        Description               ERefs   EV
                // ==== ===============================  =====  =====
                { L_, "null (a)",                      &NA,   true,  },
                { L_, "null (b)",                      &NB,   true,  },
                { L_, "unset (non-nullable, a)",       &UA,   false, },
                { L_, "unset (non-nullable, b)",       &UB,   false, },
                { L_, "unset (nullable, non-null, a)", &UNA,  false, },
                { L_, "unset (nullable, non-null, b)", &UNB,  false, },
                { L_, "unset (nullable, null, a)",     &UNNA, true,  },
                { L_, "unset (nullable, null, b)",     &UNNB, true,  },
                { L_, "set (non-nullable, a1)",        &SA1,  false, },
                { L_, "set (non-nullable, a2)",        &SA2,  false, },
                { L_, "set (non-nullable, b1)",        &SB1,  false, },
                { L_, "set (non-nullable, b2)",        &SB2,  false, },
                { L_, "set (nullable, a1)",            &SNA1, false, },
                { L_, "set (nullable, a2)",            &SNA2, false, },
                { L_, "set (nullable, b1)",            &SNB1, false, },
                { L_, "set (nullable, b2)",            &SNB2, false, }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE        = DATA[i].d_line;
                const char     *const DESCRIPTION = DATA[i].d_description;
                const ELEMREFS *const EREFS       = DATA[i].d_elemRefs;
                const bool            EV          = DATA[i].d_expectedValue;

                if (veryVerbose) bsl::cout << "\t\t" << DESCRIPTION
                                           << bsl::endl;

                LOOP_ASSERT(LINE, EV == EREFS->d_CER.isNull());
                LOOP_ASSERT(LINE, EV != EREFS->d_CER.isNonNull());
                LOOP_ASSERT(LINE, EV == EREFS->d_cCER.isNull());
                LOOP_ASSERT(LINE, EV != EREFS->d_cCER.isNonNull());
                LOOP_ASSERT(LINE, EV == EREFS->d_ER.isNull());
                LOOP_ASSERT(LINE, EV != EREFS->d_ER.isNonNull());
                LOOP_ASSERT(LINE, EV == EREFS->d_cER.isNull());
                LOOP_ASSERT(LINE, EV != EREFS->d_cER.isNonNull());
            }
        }

        if (verbose) bsl::cout << "\top==/op!=" << bsl::endl;

        int         lDataNullBits = controlBits;
        char        lDataA1       = controlA1;
        char        lDataA2       = controlA2;
        char        lDataAU       = controlAU;
        bsl::string lDataB1       = controlB1;
        bsl::string lDataB2       = controlB2;
        bsl::string lDataBU       = controlBU;

        // Local 'ELEMREFS' used to test op==/op!=.  These 'ELEMREFS' point to
        // different memory then the global 'ELEMREFS', but they have the same
        // respective value.
        ELEMREFS LNA  = {
            bdlmxxx::ConstElemRef(&lDataAU, &charDescriptor, &lDataNullBits, 2),
            bdlmxxx::ConstElemRef(&lDataAU, &charDescriptor, &lDataNullBits, 2),
                 bdlmxxx::ElemRef(&lDataAU, &charDescriptor, &lDataNullBits, 2),
                 bdlmxxx::ElemRef(&lDataAU, &charDescriptor, &lDataNullBits, 2)
        };
        ELEMREFS LNB  = {
            bdlmxxx::ConstElemRef(&lDataBU, &stringDescriptor, &lDataNullBits, 3),
            bdlmxxx::ConstElemRef(&lDataBU, &stringDescriptor, &lDataNullBits, 3),
                 bdlmxxx::ElemRef(&lDataBU, &stringDescriptor, &lDataNullBits, 3),
                 bdlmxxx::ElemRef(&lDataBU, &stringDescriptor, &lDataNullBits, 3)
        };

        ELEMREFS LUA  = {
            bdlmxxx::ConstElemRef(&lDataAU, &charDescriptor),
            bdlmxxx::ConstElemRef(&lDataAU, &charDescriptor),
                 bdlmxxx::ElemRef(&lDataAU, &charDescriptor),
                 bdlmxxx::ElemRef(&lDataAU, &charDescriptor)
        };
        ELEMREFS LUB  = {
            bdlmxxx::ConstElemRef(&lDataBU, &stringDescriptor),
            bdlmxxx::ConstElemRef(&lDataBU, &stringDescriptor),
                 bdlmxxx::ElemRef(&lDataBU, &stringDescriptor),
                 bdlmxxx::ElemRef(&lDataBU, &stringDescriptor)
        };

        ELEMREFS LUNA  = {
            bdlmxxx::ConstElemRef(&lDataAU, &charDescriptor, &lDataNullBits, 4),
            bdlmxxx::ConstElemRef(&lDataAU, &charDescriptor, &lDataNullBits, 4),
                 bdlmxxx::ElemRef(&lDataAU, &charDescriptor, &lDataNullBits, 4),
                 bdlmxxx::ElemRef(&lDataAU, &charDescriptor, &lDataNullBits, 4)
        };
        ELEMREFS LUNB  = {
            bdlmxxx::ConstElemRef(&lDataBU, &stringDescriptor, &lDataNullBits, 5),
            bdlmxxx::ConstElemRef(&lDataBU, &stringDescriptor, &lDataNullBits, 5),
                 bdlmxxx::ElemRef(&lDataBU, &stringDescriptor, &lDataNullBits, 5),
                 bdlmxxx::ElemRef(&lDataBU, &stringDescriptor, &lDataNullBits, 5)
        };

        // same memory and nullability information as na, NA, etc ...
        ELEMREFS LUNNA  = {
            bdlmxxx::ConstElemRef(LNA.d_CER),
            bdlmxxx::ConstElemRef(LNA.d_cCER),
                 bdlmxxx::ElemRef(LNA.d_ER),
                 bdlmxxx::ElemRef(LNA.d_cER)
        };
        ELEMREFS LUNNB  = {
            bdlmxxx::ConstElemRef(LNB.d_CER),
            bdlmxxx::ConstElemRef(LNB.d_cCER),
                bdlmxxx::ElemRef(LNB.d_ER),
                bdlmxxx::ElemRef(LNB.d_cER)
        };

        ELEMREFS LSA1  = {
            bdlmxxx::ConstElemRef(&lDataA1, &charDescriptor),
            bdlmxxx::ConstElemRef(&lDataA1, &charDescriptor),
                 bdlmxxx::ElemRef(&lDataA1, &charDescriptor),
                 bdlmxxx::ElemRef(&lDataA1, &charDescriptor)
        };
        ELEMREFS LSNA1  = {
            bdlmxxx::ConstElemRef(&lDataA1, &charDescriptor, &lDataNullBits, 6),
            bdlmxxx::ConstElemRef(&lDataA1, &charDescriptor, &lDataNullBits, 6),
                 bdlmxxx::ElemRef(&lDataA1, &charDescriptor, &lDataNullBits, 6),
                 bdlmxxx::ElemRef(&lDataA1, &charDescriptor, &lDataNullBits, 6)
        };

        ELEMREFS LSA2  = {
            bdlmxxx::ConstElemRef(&lDataA2, &charDescriptor),
            bdlmxxx::ConstElemRef(&lDataA2, &charDescriptor),
                 bdlmxxx::ElemRef(&lDataA2, &charDescriptor),
                 bdlmxxx::ElemRef(&lDataA2, &charDescriptor)
        };
        ELEMREFS LSNA2  = {
            bdlmxxx::ConstElemRef(&lDataA2, &charDescriptor, &lDataNullBits, 7),
            bdlmxxx::ConstElemRef(&lDataA2, &charDescriptor, &lDataNullBits, 7),
                 bdlmxxx::ElemRef(&lDataA2, &charDescriptor, &lDataNullBits, 7),
                 bdlmxxx::ElemRef(&lDataA2, &charDescriptor, &lDataNullBits, 7)
        };

        ELEMREFS LSB1  = {
            bdlmxxx::ConstElemRef(&lDataB1, &stringDescriptor),
            bdlmxxx::ConstElemRef(&lDataB1, &stringDescriptor),
                 bdlmxxx::ElemRef(&lDataB1, &stringDescriptor),
                 bdlmxxx::ElemRef(&lDataB1, &stringDescriptor)
        };
        ELEMREFS LSNB1  = {
            bdlmxxx::ConstElemRef(&lDataB1, &stringDescriptor, &lDataNullBits, 8),
            bdlmxxx::ConstElemRef(&lDataB1, &stringDescriptor, &lDataNullBits, 8),
                 bdlmxxx::ElemRef(&lDataB1, &stringDescriptor, &lDataNullBits, 8),
                 bdlmxxx::ElemRef(&lDataB1, &stringDescriptor, &lDataNullBits, 8)
        };

        ELEMREFS LSB2  = {
            bdlmxxx::ConstElemRef(&lDataB2, &stringDescriptor),
            bdlmxxx::ConstElemRef(&lDataB2, &stringDescriptor),
                 bdlmxxx::ElemRef(&lDataB2, &stringDescriptor),
                 bdlmxxx::ElemRef(&lDataB2, &stringDescriptor)
        };
        ELEMREFS LSNB2  = {
            bdlmxxx::ConstElemRef(&lDataB2, &stringDescriptor, &lDataNullBits, 9),
            bdlmxxx::ConstElemRef(&lDataB2, &stringDescriptor, &lDataNullBits, 9),
                 bdlmxxx::ElemRef(&lDataB2, &stringDescriptor, &lDataNullBits, 9),
                 bdlmxxx::ElemRef(&lDataB2, &stringDescriptor, &lDataNullBits, 9)
        };

        {
            const struct {
                int             d_line;
                const char     *d_description;
                const ELEMREFS *d_elemRefs1;
                const ELEMREFS *d_elemRefs2;
                bool            d_expectedValue;
            } DATA[] = {
//|<------------^
// Line        Description                        ERefs1     ERefs2    EV
// ==== ===============================          =========  ========= =====
// unbound/unbound
  { L_, "unbound/unbound",                       &UNBOUND,  &UNBOUND,  true  },
  { L_, "unbound/unbound",                       &UNBOUND,  &UNBOUNDN, true  },
  { L_, "unbound/unbound",                       &UNBOUNDN, &UNBOUNDN, true  },

  // unbound/bound
  { L_, "unbound/null",                          &UNBOUND,  &NA,       false },
  { L_, "unbound/null",                          &UNBOUND,  &NB,       false },
  { L_, "unbound/unset (non-nullable, a)",       &UNBOUND,  &UA,       false },
  { L_, "unbound/unset (non-nullable, b)",       &UNBOUND,  &UB,       false },
  { L_, "unbound/unset (nullable, non-null, a)", &UNBOUND,  &UNA,      false },
  { L_, "unbound/unset (nullable, non-null, a)", &UNBOUND,  &UNB,      false },
  { L_, "unbound/unset (nullable, null, a)",     &UNBOUND,  &UNNA,     false },
  { L_, "unbound/unset (nullable, null, b)",     &UNBOUND,  &UNNB,     false },
  { L_, "unbound/set (non-nullable, a1)",        &UNBOUND,  &SA1,      false },
  { L_, "unbound/set (non-nullable, a2)",        &UNBOUND,  &SA2,      false },
  { L_, "unbound/set (non-nullable, b1)",        &UNBOUND,  &SB1,      false },
  { L_, "unbound/set (non-nullable, b2)",        &UNBOUND,  &SB2,      false },
  { L_, "unbound/set (nullable, a1)",            &UNBOUND,  &SNA1,     false },
  { L_, "unbound/set (nullable, a2)",            &UNBOUND,  &SNA2,     false },
  { L_, "unbound/set (nullable, b1)",            &UNBOUND,  &SNB1,     false },
  { L_, "unbound/set (nullable, b2)",            &UNBOUND,  &SNB2,     false },

  // null/null
  { L_, "null (a)/null (a)",                     &NA,       &NA,       true  },
  { L_, "null (a)/null (l, a)",                  &NA,       &LNA,      true  },
  { L_, "null (a)/null (b)",                     &NA,       &NB,       false },
  { L_, "null (a)/unset (nullable, null, a)",    &NA,       &UNNA,     true  },
  { L_, "null (a)/unset (l, nullable, null, a)", &NA,       &LUNNA,    true  },
  { L_, "null (a)/unset (nullable, null, b)",    &NA,       &UNNB,     false },
  // TBD redundant?
  { L_, "null (b)/null (b)",                     &NB,       &NB,       true  },
  { L_, "null (b)/null (l, b)",                  &NB,       &LNB,      true  },
  { L_, "null (b)/unset (nullable, null, a)",    &NB,       &UNNA,     false },
  { L_, "null (b)/unset (nullable, null, b)",    &NB,       &UNNB,     true  },
  { L_, "null (b)/unset (l, nullable, null, b)", &NB,       &LUNNB,    true  },

  // null/non-null
  { L_, "null (a)/unset (non-nullable, a)",      &NA,       &UA,       false },
  { L_, "null (a)/unset (non-nullable, b)",      &NA,       &UB,       false },
  { L_, "null (a)/unset (nullable, non-null, a)",
                                                 &NA,       &UNA,      false },
  { L_, "null (a)/unset (nullable, non-null, b)",
                                                 &NA,       &UNB,      false },
  { L_, "null (a)/set (non-nulable, a1)",        &NA,       &SA1,      false },
  { L_, "null (a)/set (non-nulable, a2)",        &NA,       &SA2,      false },
  { L_, "null (a)/set (non-nulable, b1)",        &NA,       &SB1,      false },
  { L_, "null (a)/set (non-nulable, b2)",        &NA,       &SB2,      false },
  { L_, "null (a)/set (nullable, a1)",           &NA,       &SNA1,     false },
  { L_, "null (a)/set (nullable, a2)",           &NA,       &SNA2,     false },
  { L_, "null (a)/set (nullable, b1)",           &NA,       &SNB1,     false },
  { L_, "null (a)/set (nullable, b2)",           &NA,       &SNB2,     false },
  // TBD redundant?
  { L_, "null (b)/unset (non-nullable, a)",      &NB,       &UA,       false },
  { L_, "null (b)/unset (non-nullable, b)",      &NB,       &UB,       false },
  { L_, "null (b)/unset (nullable, non-null, a)",
                                                 &NB,       &UNA,      false },
  { L_, "null (b)/unset (nullable, non-null, b)",
                                                 &NB,       &UNB,      false },
  { L_, "null (b)/set (non-nulable, a1)",        &NB,       &SA1,      false },
  { L_, "null (b)/set (non-nulable, a2)",        &NB,       &SA2,      false },
  { L_, "null (b)/set (non-nulable, b1)",        &NB,       &SB1,      false },
  { L_, "null (b)/set (non-nulable, b2)",        &NB,       &SB2,      false },
  { L_, "null (b)/set (nullable, a1)",           &NB,       &SNA1,     false },
  { L_, "null (b)/set (nullable, a2)",           &NB,       &SNA2,     false },
  { L_, "null (b)/set (nullable, b1)",           &NB,       &SNB1,     false },
  { L_, "null (b)/set (nullable, b2)",           &NB,       &SNB2,     false },

  // unset/unset
  { L_, "unset (non-nullable, a)/unset (non-nullable, a)",
                                                 &UA,       &UA,       true  },
  { L_, "unset (non-nullable, a)/unset (l, non-nullable, a)",
                                                 &UA,       &LUA,      true  },
  { L_, "unset (non-nullable, a)/unset (non-nullable, b)",
                                                 &UA,       &UB,       false },
  { L_, "unset (non-nullable, a)/unset (nullable, non-null, a)",
                                                 &UA,       &UNA,      true  },
  { L_, "unset (non-nullable, a)/unset (l, nullable, non-null, a)",
                                                 &UA,       &LUNA,     true  },
  { L_, "unset (non-nullable, a)/unset (nullable, non-null, b)",
                                                 &UA,       &UNB,      false },
  { L_, "unset (non-nullable, a)/unset (nullable, null, a)",
                                                 &UA,       &UNNA,     false },
  { L_, "unset (non-nullable, a)/unset (nullable, null, b)",
                                                 &UA,       &UNNB,     false },
  { L_, "unset (non-nullable, b)/unset (non-nullable, b)",
                                                 &UB,       &UB,       true  },
  { L_, "unset (non-nullable, b)/unset (l, non-nullable, b)",
                                                 &UB,       &LUB,      true  },
  { L_, "unset (non-nullable, b)/unset (nullable, non-null, b)",
                                                 &UB,       &UNB,      true  },
  { L_, "unset (non-nullable, b)/unset (l, nullable, non-null, b)",
                                                 &UB,       &LUNB,     true  },
  { L_, "unset (non-nullable, b)/unset (nullable, null, b)",
                                                 &UB,       &UNNB,     false },

  // unset/set
  { L_, "unset (non-nullable, a)/set (non-nullable, a1)",
                                                 &UA,       &SA1,      false },
  { L_, "unset (non-nullable, a)/set (non-nullable, a2)",
                                                 &UA,       &SA2,      false },
  { L_, "unset (non-nullable, a)/set (non-nullable, b1)",
                                                 &UA,       &SB1,      false },
  { L_, "unset (non-nullable, a)/set (non-nullable, b2)",
                                                 &UA,       &SB2,      false },
  { L_, "unset (non-nullable, a)/set (nullable, a1)",
                                                 &UA,       &SNA1,     false },
  { L_, "unset (non-nullable, a)/set (nullable, a2)",
                                                 &UA,       &SNA2,     false },
  { L_, "unset (non-nullable, a)/set (nullable, b1)",
                                                 &UA,       &SNB1,     false },
  { L_, "unset (non-nullable, a)/set (nullable, b2)",
                                                 &UA,       &SNB2,     false },

  // TBD redundant?
  { L_, "unset (non-nullable, b)/set (non-nullable, a1)",
                                                 &UB,       &SA1,      false },
  { L_, "unset (non-nullable, b)/set (non-nullable, a2)",
                                                 &UB,       &SA2,      false },
  { L_, "unset (non-nullable, b)/set (non-nullable, b1)",
                                                 &UB,       &SB1,      false },
  { L_, "unset (non-nullable, b)/set (non-nullable, b2)",
                                                 &UB,       &SB2,      false },
  { L_, "unset (non-nullable, b)/set (nullable, a1)",
                                                 &UB,       &SNA1,     false },
  { L_, "unset (non-nullable, b)/set (nullable, a2)",
                                                 &UB,       &SNA2,     false },
  { L_, "unset (non-nullable, b)/set (nullable, b1)",
                                                 &UB,       &SNB1,     false },
  { L_, "unset (non-nullable, b)/set (nullable, b2)",
                                                 &UB,       &SNB2,     false },

  // set/set
  { L_, "set (non-nullable, a1)/set (non-nullable, a1)",
                                                 &SA1,      &SA1,      true  },
  { L_, "set (non-nullable, a1)/set (l, non-nullable, a1)",
                                                 &SA1,      &LSA1,     true  },
  { L_, "set (non-nullable, a1)/set (non-nullable, a2)",
                                                 &SA1,      &SA2,      false },
  { L_, "set (non-nullable, a1)/set (non-nullable, b1)",
                                                 &SA1,      &SB1,      false },
  { L_, "set (non-nullable, a1)/set (non-nullable, b2)",
                                                 &SA1,      &SB2,      false },
  { L_, "set (non-nullable, a1)/set (nullable, a1)",
                                                 &SA1,      &SNA1,     true  },
  { L_, "set (non-nullable, a1)/set (l, nullable, a1)",
                                                 &SA1,      &LSNA1,    true  },
  { L_, "set (non-nullable, a1)/set (nullable, a2)",
                                                 &SA1,      &SNA2,     false },
  { L_, "set (non-nullable, a1)/set (nullable, b1)",
                                                 &SA1,      &SNB1,     false },
  { L_, "set (non-nullable, a1)/set (nullable, b2)",
                                                 &SA1,      &SNB2,     false },

  // TBD redundant?
  { L_, "set (non-nullable, a2)/set (non-nullable, a2)",
                                                 &SA2,      &SA2,      true  },
  { L_, "set (non-nullable, a2)/set (l, non-nullable, a2)",
                                                 &SA2,      &LSA2,     true  },
  { L_, "set (non-nullable, a2)/set (non-nullable, b1)",
                                                 &SA2,      &SB1,      false },
  { L_, "set (non-nullable, a2)/set (non-nullable, b2)",
                                                 &SA2,      &SB2,      false },
  { L_, "set (non-nullable, a2)/set (nullable, a1)",
                                                 &SA2,      &SNA1,     false },
  { L_, "set (non-nullable, a2)/set (l, nullable, a2)",
                                                 &SA2,      &LSNA2,    true  },
  { L_, "set (non-nullable, a2)/set (nullable, a2)",
                                                 &SA2,      &SNA2,     true  },
  { L_, "set (non-nullable, a2)/set (nullable, b1)",
                                                 &SA2,      &SNB1,     false },
  { L_, "set (non-nullable, a2)/set (nullable, b2)",
                                                 &SA2,      &SNB2,     false },
  // set/set
  { L_, "set (non-nullable, b1)/set (non-nullable, b1)",
                                                 &SB1,      &SB1,      true  },
  { L_, "set (non-nullable, b1)/set (l, non-nullable, b1)",
                                                 &SB1,      &LSB1,     true  },
  { L_, "set (non-nullable, b1)/set (non-nullable, b2)",
                                                 &SB1,      &SB2,      false },
  { L_, "set (non-nullable, b1)/set (nullable, b1)",
                                                 &SB1,      &SNB1,     true  },
  { L_, "set (non-nullable, b1)/set (l, nullable, b1)",
                                                 &SB1,      &LSNB1,    true  },
  { L_, "set (non-nullable, b1)/set (nullable, b2)",
                                                 &SB1,      &SNB2,     false },

  // TBD redundant?
  { L_, "set (non-nullable, b2)/set (non-nullable, b2)",
                                                 &SB2,      &SB2,      true  },
  { L_, "set (non-nullable, b2)/set (l, non-nullable, b2)",
                                                 &SB2,      &LSB2,     true  },
  { L_, "set (non-nullable, b2)/set (nullable, b1)",
                                                 &SB2,      &SNB1,     false },
  { L_, "set (non-nullable, b2)/set (l, nullable, b2)",
                                                 &SB2,      &LSNB2,    true  },
  { L_, "set (non-nullable, b2)/set (nullable, b2)",
                                                 &SB2,      &SNB2,     true  },
  { L_, "set (non-nullable, b2)/set (nullable, b1)",
                                                 &SB2,      &SNB1,     false },
  { L_, "set (non-nullable, b2)/set (nullable, b2)",
                                                 &SB2,      &SNA2,     false },
//^------------>|
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE        = DATA[i].d_line;
                const char     *const DESCRIPTION = DATA[i].d_description;
                const ELEMREFS *const EREFS1      = DATA[i].d_elemRefs1;
                const ELEMREFS *const EREFS2      = DATA[i].d_elemRefs2;
                const bool            EV          = DATA[i].d_expectedValue;

                if (veryVerbose) bsl::cout << "\t\t" << DESCRIPTION
                                           << bsl::endl;

                LOOP_ASSERT(LINE, EV == ( EREFS1->d_CER == EREFS2->d_CER ));
                LOOP_ASSERT(LINE, EV == ( EREFS1->d_CER == EREFS2->d_cCER));
                LOOP_ASSERT(LINE, EV == ( EREFS1->d_CER == EREFS2->d_ER  ));
                LOOP_ASSERT(LINE, EV == ( EREFS1->d_CER == EREFS2->d_cER ));

                LOOP_ASSERT(LINE, EV == (EREFS1->d_cCER == EREFS2->d_CER ));
                LOOP_ASSERT(LINE, EV == (EREFS1->d_cCER == EREFS2->d_cCER));
                LOOP_ASSERT(LINE, EV == (EREFS1->d_cCER == EREFS2->d_ER  ));
                LOOP_ASSERT(LINE, EV == (EREFS1->d_cCER == EREFS2->d_cER ));

                LOOP_ASSERT(LINE, EV == (  EREFS1->d_ER == EREFS2->d_CER ));
                LOOP_ASSERT(LINE, EV == (  EREFS1->d_ER == EREFS2->d_cCER));
                LOOP_ASSERT(LINE, EV == (  EREFS1->d_ER == EREFS2->d_ER  ));
                LOOP_ASSERT(LINE, EV == (  EREFS1->d_ER == EREFS2->d_cER ));

                LOOP_ASSERT(LINE, EV == ( EREFS1->d_cER == EREFS2->d_CER ));
                LOOP_ASSERT(LINE, EV == ( EREFS1->d_cER == EREFS2->d_cCER));
                LOOP_ASSERT(LINE, EV == ( EREFS1->d_cER == EREFS2->d_ER  ));
                LOOP_ASSERT(LINE, EV == ( EREFS1->d_cER == EREFS2->d_cER ));

                LOOP_ASSERT(LINE, EV != ( EREFS1->d_CER != EREFS2->d_CER ));
                LOOP_ASSERT(LINE, EV != ( EREFS1->d_CER != EREFS2->d_cCER));
                LOOP_ASSERT(LINE, EV != ( EREFS1->d_CER != EREFS2->d_ER  ));
                LOOP_ASSERT(LINE, EV != ( EREFS1->d_CER != EREFS2->d_cER ));

                LOOP_ASSERT(LINE, EV != (EREFS1->d_cCER != EREFS2->d_CER ));
                LOOP_ASSERT(LINE, EV != (EREFS1->d_cCER != EREFS2->d_cCER));
                LOOP_ASSERT(LINE, EV != (EREFS1->d_cCER != EREFS2->d_ER  ));
                LOOP_ASSERT(LINE, EV != (EREFS1->d_cCER != EREFS2->d_cER ));

                LOOP_ASSERT(LINE, EV != (  EREFS1->d_ER != EREFS2->d_CER ));
                LOOP_ASSERT(LINE, EV != (  EREFS1->d_ER != EREFS2->d_cCER));
                LOOP_ASSERT(LINE, EV != (  EREFS1->d_ER != EREFS2->d_ER  ));
                LOOP_ASSERT(LINE, EV != (  EREFS1->d_ER != EREFS2->d_cER ));

                LOOP_ASSERT(LINE, EV != ( EREFS1->d_cER != EREFS2->d_CER ));
                LOOP_ASSERT(LINE, EV != ( EREFS1->d_cER != EREFS2->d_cCER));
                LOOP_ASSERT(LINE, EV != ( EREFS1->d_cER != EREFS2->d_ER  ));
                LOOP_ASSERT(LINE, EV != ( EREFS1->d_cER != EREFS2->d_cER ));

                LOOP_ASSERT(LINE, EV == ( EREFS2->d_CER ==  EREFS1->d_CER));
                LOOP_ASSERT(LINE, EV == (EREFS2->d_cCER ==  EREFS1->d_CER));
                LOOP_ASSERT(LINE, EV == (  EREFS2->d_ER ==  EREFS1->d_CER));
                LOOP_ASSERT(LINE, EV == ( EREFS2->d_cER ==  EREFS1->d_CER));

                LOOP_ASSERT(LINE, EV == ( EREFS2->d_CER == EREFS1->d_cCER));
                LOOP_ASSERT(LINE, EV == (EREFS2->d_cCER == EREFS1->d_cCER));
                LOOP_ASSERT(LINE, EV == (  EREFS2->d_ER == EREFS1->d_cCER));
                LOOP_ASSERT(LINE, EV == ( EREFS2->d_cER == EREFS1->d_cCER));

                LOOP_ASSERT(LINE, EV == ( EREFS2->d_CER ==   EREFS1->d_ER));
                LOOP_ASSERT(LINE, EV == (EREFS2->d_cCER ==   EREFS1->d_ER));
                LOOP_ASSERT(LINE, EV == (  EREFS2->d_ER ==   EREFS1->d_ER));
                LOOP_ASSERT(LINE, EV == ( EREFS2->d_cER ==   EREFS1->d_ER));

                LOOP_ASSERT(LINE, EV == ( EREFS2->d_CER ==  EREFS1->d_cER));
                LOOP_ASSERT(LINE, EV == (EREFS2->d_cCER ==  EREFS1->d_cER));
                LOOP_ASSERT(LINE, EV == (  EREFS2->d_ER ==  EREFS1->d_cER));
                LOOP_ASSERT(LINE, EV == ( EREFS2->d_cER ==  EREFS1->d_cER));

                LOOP_ASSERT(LINE, EV != ( EREFS2->d_CER !=  EREFS1->d_CER));
                LOOP_ASSERT(LINE, EV != (EREFS2->d_cCER !=  EREFS1->d_CER));
                LOOP_ASSERT(LINE, EV != (  EREFS2->d_ER !=  EREFS1->d_CER));
                LOOP_ASSERT(LINE, EV != ( EREFS2->d_cER !=  EREFS1->d_CER));

                LOOP_ASSERT(LINE, EV != ( EREFS2->d_CER != EREFS1->d_cCER));
                LOOP_ASSERT(LINE, EV != (EREFS2->d_cCER != EREFS1->d_cCER));
                LOOP_ASSERT(LINE, EV != (  EREFS2->d_ER != EREFS1->d_cCER));
                LOOP_ASSERT(LINE, EV != ( EREFS2->d_cER != EREFS1->d_cCER));

                LOOP_ASSERT(LINE, EV != ( EREFS2->d_CER !=   EREFS1->d_ER));
                LOOP_ASSERT(LINE, EV != (EREFS2->d_cCER !=   EREFS1->d_ER));
                LOOP_ASSERT(LINE, EV != (  EREFS2->d_ER !=   EREFS1->d_ER));
                LOOP_ASSERT(LINE, EV != ( EREFS2->d_cER !=   EREFS1->d_ER));

                LOOP_ASSERT(LINE, EV != ( EREFS2->d_CER !=  EREFS1->d_cER));
                LOOP_ASSERT(LINE, EV != (EREFS2->d_cCER !=  EREFS1->d_cER));
                LOOP_ASSERT(LINE, EV != (  EREFS2->d_ER !=  EREFS1->d_cER));
                LOOP_ASSERT(LINE, EV != ( EREFS2->d_cER !=  EREFS1->d_cER));
            }
        }

        if (verbose) cout << "\toperator<</print" << endl;
        {
            const struct {
                int             d_line;
                const char     *d_description;
                const ELEMREFS *d_elemRefs;
                const char     *d_expectedValue;
            } DATA[] = {
//      |<------^
        // Line          Description               ERefs          EV
        // ==== ===============================  =========  ==============
        { L_, "unbound",                       &UNBOUND,  "unbound"      },
        { L_, "unbound (nullable)",            &UNBOUNDN, "unbound"      },
        { L_, "null (a)",                      &NA,       "NULL"         },
        { L_, "null (b)",                      &NB,       "NULL"         },
        { L_, "unset (non-nullable, a)",       &UA,       ""             },
        { L_, "unset (non-nullable, b)",       &UB,       ""             },
        { L_, "unset (nullable, non-null, a)", &UNA,      ""             },
        { L_, "unset (nullable, non-null, b)", &UNB,      ""             },
        { L_, "unset (nullable, null, a)",     &UNNA,     "NULL"         },
        { L_, "unset (nullable, null, b)",     &UNNB,     "NULL"         },
        { L_, "a1 (non-nullable)",             &SA1,      &dataA1        },
        { L_, "a2 (non-nullable)",             &SA2,      &dataA2        },
        { L_, "b1 (non-nullable)",             &SB1,      dataB1.c_str() },
        { L_, "b2 (non-nullable)",             &SB2,      dataB2.c_str() },
        { L_, "a1 (nullable)",                 &SNA1,     &dataA1        },
        { L_, "a2 (nullable)",                 &SNA2,     &dataA2        },
        { L_, "b1 (nullable)",                 &SNB1,     dataB1.c_str() },
        { L_, "b2 (nullable)",                 &SNB2,     dataB2.c_str() }
//      ^------>|
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            char buf[4096];

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE        = DATA[i].d_line;
                const char     *const DESCRIPTION = DATA[i].d_description;
                const ELEMREFS *const EREFS       = DATA[i].d_elemRefs;
                const char     *const EV          = DATA[i].d_expectedValue;

                if (veryVerbose) bsl::cout << "\t\t" << DESCRIPTION
                                           << bsl::endl;

                const int numERefs = 4;
                const bdlmxxx::ConstElemRef *eRefs[numERefs] = {
                    &EREFS->d_CER, &EREFS->d_cCER, &EREFS->d_ER, &EREFS->d_cER
                };

                for (int j = 0; j < numERefs; ++j) {
                    ostrstream out(buf, sizeof buf);
                    out << *eRefs[j] << ends;
                    LOOP3_ASSERT(LINE, EV, buf,
                                 0 == strncmp(EV, buf, strlen(buf)));
                }

                for (int j = 0; j < numERefs; ++j) {
                    ostrstream out(buf, sizeof buf);
                    eRefs[j]->print(out, 0, 0) << ends;
                    LOOP3_ASSERT(LINE, EV, buf,
                                 0 == strncmp(EV, buf, strlen(buf)));
                }
            }
        }

        if (veryVerbose)
            bsl::cout << "\t\ttest (check callback args explicitly)"
                      << bsl::endl;
        {
            const int numERefs = 4;
            const bdlmxxx::ConstElemRef *eRefs[numERefs] = {
                &TEST.d_CER, &TEST.d_cCER, &TEST.d_ER, &TEST.d_cER
            };

            ostream *STREAM     = (ostream *) 0x0;
            int      LEVEL            = 1;
            int      SPACES_PER_LEVEL = 2;
            ostream *RESULT     = (ostream *) 0x3;

            for (int i = 0; i < numERefs; ++i) {
                globalPrintTest_result = RESULT;
                ostream *const result =
                    &(eRefs[i]->print(*STREAM, LEVEL, SPACES_PER_LEVEL));
                ASSERT(RESULT           == result);
                ASSERT(STREAM           == globalPrintTest_stream);
                ASSERT(LEVEL            == globalPrintTest_level);
                ASSERT(SPACES_PER_LEVEL == globalPrintTest_spacesPerLevel);
                STREAM += 0x4;
                LEVEL  += 4;
                SPACES_PER_LEVEL += 4;
                RESULT += 0x4;
            }
        }

        if (veryVerbose)
            bsl::cout << "\t\ttestn (check callback args explicitly)"
                      << bsl::endl;
        {
            const int numERefs = 4;
            const bdlmxxx::ConstElemRef *eRefs[numERefs] = {
                &TESTN.d_CER, &TESTN.d_cCER, &TESTN.d_ER, &TESTN.d_cER
            };

            ostream *STREAM     = (ostream *) 0x0;
            int      LEVEL            = 1;
            int      SPACES_PER_LEVEL = 2;
            ostream *RESULT     = (ostream *) 0x3;

            for (int i = 0; i < numERefs; ++i) {
                globalPrintTest_result = RESULT;
                ostream *const result =
                    &(eRefs[i]->print(*STREAM, LEVEL, SPACES_PER_LEVEL));
                ASSERT(RESULT           == result);
                ASSERT(STREAM           == globalPrintTest_stream);
                ASSERT(LEVEL            == globalPrintTest_level);
                ASSERT(SPACES_PER_LEVEL == globalPrintTest_spacesPerLevel);
                STREAM += 0x4;
                LEVEL  += 4;
                SPACES_PER_LEVEL += 4;
                RESULT += 0x4;
            }
        }

        if (verbose) cout << "\tmakeNull" << endl;
        {
            const struct {
                int             d_line;
                const char     *d_description;
                const ELEMREFS *d_elemRefs;
            } DATA[] = {
                // Line          Description             ERefs
                // ==== ===============================  =====
                { L_, "null (a)",                      &NA    },
                { L_, "null (a)",                      &NA    },
                { L_, "null (a)",                      &NA    },
                { L_, "null (b)",                      &NB    },
                { L_, "null (b)",                      &NB    },
                { L_, "null (b)",                      &NB    },
                { L_, "unset (non-nullable, a)",       &UA    },
                { L_, "unset (non-nullable, b)",       &UB    },
                { L_, "unset (nullable, non-null, a)", &UNA   },
                { L_, "unset (nullable, non-null, b)", &UNB   },
                { L_, "unset (nullable, null, a)",     &UNNA  },
                { L_, "unset (nullable, null, b)",     &UNNB  },
                { L_, "a1 (non-nullable)",             &SA1   },
                { L_, "a2 (non-nullable)",             &SA2   },
                { L_, "b1 (non-nullable)",             &SB1   },
                { L_, "b2 (non-nullable)",             &SB2   },
                { L_, "a1 (nullable)",                 &SNA1  },
                { L_, "a2 (nullable)",                 &SNA2  },
                { L_, "b1 (nullable)",                 &SNB1  },
                { L_, "b2 (nullable)",                 &SNB2  }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE        = DATA[i].d_line;
                const char     *const DESCRIPTION = DATA[i].d_description;
                const ELEMREFS *const EREFS       = DATA[i].d_elemRefs;

                if (veryVerbose) bsl::cout << "\t\t" << DESCRIPTION
                                           << bsl::endl;

                // Initial values for isNull/NonNull have been tested above.

                // EREFS->d_CER.makeNull();       // should not compile
                // EREFS->d_cCER.makeNull();      // should not compile
                EREFS->d_ER.makeNull();

                if (EREFS->d_CER.isNullable()) {
                    LOOP_ASSERT(LINE, 1 == EREFS->d_CER.isNull());
                    LOOP_ASSERT(LINE, 1 == isUnset(EREFS->d_CER));
                    LOOP_ASSERT(LINE, 1 == EREFS->d_cCER.isNull());
                    LOOP_ASSERT(LINE, 1 == isUnset(EREFS->d_cCER));
                    LOOP_ASSERT(LINE, 1 == EREFS->d_ER.isNull());
                    LOOP_ASSERT(LINE, 1 == isUnset(EREFS->d_ER));
                    LOOP_ASSERT(LINE, 1 == EREFS->d_cER.isNull());
                    LOOP_ASSERT(LINE, 1 == isUnset(EREFS->d_cER));
                    LOOP_ASSERT(LINE, 0 == EREFS->d_CER.isNonNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_cCER.isNonNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_ER.isNonNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_cER.isNonNull());
                }

                // EREFS->d_CER.makeNull();       // should not compile
                // EREFS->d_cCER.makeNull();      // should not compile
                EREFS->d_ER.makeNull();

                if (EREFS->d_CER.isNullable()) {
                    LOOP_ASSERT(LINE, 1 == EREFS->d_CER.isNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_cCER.isNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_ER.isNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_cER.isNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_CER.isNonNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_cCER.isNonNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_ER.isNonNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_cER.isNonNull());
                }
                else {
                    LOOP_ASSERT(LINE, 0 == EREFS->d_CER.isNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_cCER.isNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_ER.isNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_cER.isNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_CER.isNonNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_cCER.isNonNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_ER.isNonNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_cER.isNonNull());
                }

                // We are concerned that makeNull works on a
                // 'const bdlmxxx::ElemRef', so we test that here.  However, the
                // value is already null so to test that the call actually
                // worked we test isNonNull as makeNull forces non-null.
                EREFS->d_cER.makeNull();

                if (EREFS->d_CER.isNullable()) {
                    LOOP_ASSERT(LINE, 1 == EREFS->d_CER.isNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_cCER.isNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_ER.isNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_cER.isNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_CER.isNonNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_cCER.isNonNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_ER.isNonNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_cER.isNonNull());
                    LOOP_ASSERT(LINE, 1 == isUnset(EREFS->d_CER));
                    LOOP_ASSERT(LINE, 1 == isUnset(EREFS->d_cCER));
                    LOOP_ASSERT(LINE, 1 == isUnset(EREFS->d_ER));
                    LOOP_ASSERT(LINE, 1 == isUnset(EREFS->d_cER));
                }
                else {
                    LOOP_ASSERT(LINE, 0 == EREFS->d_CER.isNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_cCER.isNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_ER.isNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_cER.isNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_CER.isNonNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_cCER.isNonNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_ER.isNonNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_cER.isNonNull());
                }

                // We are concerned that makeNull works on a
                // 'const bdlmxxx::ElemRef', so we test that here.  The value is
                // non-null from the makeNull call above.
                EREFS->d_cER.makeNull();

                if (EREFS->d_CER.isNullable()) {
                    LOOP_ASSERT(LINE, 1 == EREFS->d_CER.isNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_cCER.isNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_ER.isNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_cER.isNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_CER.isNonNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_cCER.isNonNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_ER.isNonNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_cER.isNonNull());
                }
                else {
                    LOOP_ASSERT(LINE, 0 == EREFS->d_CER.isNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_cCER.isNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_ER.isNull());
                    LOOP_ASSERT(LINE, 0 == EREFS->d_cER.isNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_CER.isNonNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_cCER.isNonNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_ER.isNonNull());
                    LOOP_ASSERT(LINE, 1 == EREFS->d_cER.isNonNull());
                }
            }
        }

        // reset memory from controls
        resetDataFromControls();

        if (verbose) cout << "\treplaceValue" << endl;
        {
            const struct {
                int             d_line;
                const char     *d_description;
                const ELEMREFS *d_elemRefs1;
                const ELEMREFS *d_elemRefs2;
                const ELEMREFS *d_expectedElemRefs;
            } DATA[] = {
//|<------------^
// Line          Description                       ERefs1 ERefs2 EErefs
// ==== ===============================            ====== ====== ======
// null/null
  { L_, "null (a)/null (a)",                       &NA,   &NA,   &NA,   },
  { L_, "null (b)/null (b)",                       &NB,   &NB,   &NB,   },
  { L_, "null (a)/unset (nullable, null, a)",      &NA,   &UNNA, &UNNA, },
  { L_, "null (b)/unset (nullable, null, b)",      &NB,   &UNNB, &UNNB, },

  // null/unset
  { L_, "null (a)/unset (non-nullable, a)",        &NA,   &UA,   &UA,   },
  { L_, "null (a)/unset (nullable, non-null, a)",  &NA,   &UNA,  &UNA,  },
  { L_, "null (a)/unset (nullable, null, a)",      &NA,   &UNNA, &UNNA, },
  { L_, "null (b)/unset (non-nullable, b)",        &NB,   &UB,   &UB,   },
  { L_, "null (b)/unset (nullable, non-null, b)",  &NB,   &UNB,  &UNB,  },
  { L_, "null (b)/unset (nullable, null, b)",      &NB,   &UNNB, &UNNB, },

  // null/set
  { L_, "null (a)/set (non-nullable, a1)",         &NA,   &SA1,  &SA1,  },
  { L_, "null (a)/set (nullable, a1)",             &NA,   &SNA1, &SNA1, },
  { L_, "null (a)/set (non-nullable, a2)",         &NA,   &SA2,  &SA2,  },
  { L_, "null (a)/set (nullable, a2)",             &NA,   &SNA2, &SNA2, },
  { L_, "null (b)/set (non-nullable, b1)",         &NB,   &SB1,  &SB1,  },
  { L_, "null (b)/set (nullable, b1)",             &NB,   &SNB1, &SNB1, },
  { L_, "null (b)/set (non-nullable, b2)",         &NB,   &SB2,  &SB2,  },
  { L_, "null (b)/set (nullable, b2)",             &NB,   &SNB2, &SNB2, },

  // unset/null
  { L_, "unset (non-nullable, a)/null (a)",        &UA,   &NA,   &UA,   },
  { L_, "unset (nullable, non-null, a)/null (a)",  &UNA,  &NA,   &NA,   },
  { L_, "unset (nullable, null, a)/null (a)",      &UNNA, &NA,   &NA,   },
  { L_, "unset (non-nullable, b)/null (b)",        &UB,   &NB,   &UB,   },
  { L_, "unset (nullable, non-null, b)/null (b)",  &UNB,  &NB,   &NB,   },
  { L_, "unset (nullable, null, b)/null (b)",      &UNNB, &NB,   &NB,   },

  // unset/unset
  { L_, "unset (non-nullable, a)/unset (non-nullable, a)",
                                                   &UA,   &UA,   &UA,   },
  { L_, "unset (non-nullable, a)/unset (nullable, non-null, a)",
                                                   &UA,   &UNA,  &UNA,  },
  { L_, "unset (non-nullable, a)/unset (nullable, null, a)",
                                                   &UA,   &UNNA, &UA,   },
  { L_, "unset (non-nullable, b)/unset (non-nullable, b)",
                                                   &UB,   &UB,   &UB,   },
  { L_, "unset (non-nullable, b)/unset (nullable, non-null, b)",
                                                   &UB,   &UNB,  &UNB,  },
  { L_, "unset (non-nullable, b)/unset (nullable, null, b)",
                                                   &UB,   &UNNB, &UB,   },

  { L_, "unset (nullable, non-null, a)/unset (non-nullable, a)",
                                                   &UNA,  &UA,   &UA,   },
  { L_, "unset (nullable, null, a)/unset (non-nullable, a)",
                                                   &UNNA, &UA,   &UA,   },
  { L_, "unset (non-nullable, b)/unset (non-nullable, b)",
                                                   &UB,   &UB,   &UB,   },
  { L_, "unset (nullable, non-null, b/)unset (non-nullable, b)",
                                                   &UNB,  &UB,   &UB,   },
  { L_, "unset (nullable, null, b)/unset (non-nullable, b)",
                                                   &UNNB, &UB,   &UB,   },

  // unset/set
  { L_, "unset (non-nullable, a)/set (non-nullable, a1)",
                                                   &UA,   &SA1,  &SA1,  },
  { L_, "unset (non-nullable, a)/set (nullable, a1)",
                                                   &UA,   &SNA1, &SNA1, },
  { L_, "unset (nullable, non-null a)/set (non-nullable, a1)",
                                                   &UNA,  &SA1,  &SA1,  },
  { L_, "unset (nullable, non-null, a)/set (nullable, a1)",
                                                   &UNA,  &SNA1, &SNA1, },
  { L_, "unset (nullable, null a)/set (non-nullable, a1)",
                                                   &UNNA, &SA1,  &SA1,  },
  { L_, "unset (nullable, null, a)/set (nullable, a1)",
                                                   &UNNA, &SNA1, &SNA1, },

  { L_, "unset (non-nullable, a)/set (non-nullable, a2)",
                                                   &UA,   &SA2,  &SA2,  },
  { L_, "unset (non-nullable, a)/set (nullable, a1)",
                                                   &UA,   &SNA2, &SNA2, },
  { L_, "unset (nullable, non-null a)/set (non-nullable, a2)",
                                                   &UNA,  &SA2,  &SA2,  },
  { L_, "unset (nullable, non-null, a)/set (nullable, a2)",
                                                   &UNA,  &SNA2, &SNA2, },
  { L_, "unset (nullable, null a)/set (non-nullable, a2)",
                                                   &UNNA, &SA2,  &SA2,  },
  { L_, "unset (nullable, null, a)/set (nullable, a2)",
                                                   &UNNA, &SNA2, &SNA2, },

  { L_, "unset (non-nullable, b)/set (non-nullable, b1)",
                                                   &UB,   &SB1,  &SB1,  },
  { L_, "unset (non-nullable, b)/set (nullable, b1)",
                                                   &UB,   &SNB1, &SNB1, },
  { L_, "unset (nullable, non-null b)/set (non-nullable, b1)",
                                                   &UNB,  &SB1,  &SB1,  },
  { L_, "unset (nullable, non-null, b)/set (nullable, b1)",
                                                   &UNB,  &SNB1, &SNB1, },
  { L_, "unset (nullable, null b)/set (non-nullable, b1)",
                                                   &UNNB, &SB1,  &SB1,  },
  { L_, "unset (nullable, null, b)/set (nullable, b1)",
                                                   &UNNB, &SNB1, &SNB1, },

  { L_, "unset (non-nullable, b)/set (non-nullable, b2)",
                                                   &UB,   &SB2,  &SB2,  },
  { L_, "unset (non-nullable, b)/set (nullable, b1)",
                                                   &UB,   &SNB2, &SNB2, },
  { L_, "unset (nullable, non-null b)/set (non-nullable, b2)",
                                                   &UNB,  &SB2,  &SB2,  },
  { L_, "unset (nullable, non-null, b)/set (nullable, b2)",
                                                   &UNB,  &SNB2, &SNB2, },
  { L_, "unset (nullable, null b)/set (non-nullable, b2)",
                                                   &UNNB, &SB2,  &SB2,  },
  { L_, "unset (nullable, null, b)/set (nullable, b2)",
                                                   &UNNB, &SNB2, &SNB2, },

  // set/null
  { L_, "set (non-nullable, a1)/null (a)", &SA1,  &NA,   &UA,   },
  { L_, "set (nullable, a1)/null (a)",     &SNA1, &NA,   &NA,   },
  { L_, "set (non-nullable, a2)/null (a)", &SA2,  &NA,   &UA,   },
  { L_, "set (nullable, a2)/null (a)",     &SNA2, &NA,   &NA,   },
  { L_, "set (non-nullable, b1)/null (b)", &SB1,  &NB,   &UB,   },
  { L_, "set (nullable, b1)/null (b)",     &SNB1, &NB,   &NB,   },
  { L_, "set (non-nullable, b2)/null (b)", &SB2,  &NB,   &UB,   },
  { L_, "set (nullable, b2)/null (b)",     &SNB2, &NB,   &NB,   },

  // set/unset
  { L_, "set (non-nullable, a1)/unset (non-nullable, a)",
                                                   &SA1,  &UA,   &UA,   },
  { L_, "set (nullable, a1)/unset (non-nullable, a)",
                                                   &SNA1, &UA,   &UA,   },
  { L_, "set (non-nullable, a1)/unset (nullable, non-null a)",
                                                   &SA1,  &UNA,  &UNA,  },
  { L_, "set (nullable, a1)/unset (nullable, non-null, a)",
                                                   &SNA1, &UNA,  &UNA,  },
  { L_, "set (non-nullable, a1)/unset (nullable, null a)",
                                                   &SA1,  &UNNA, &UA,   },
  { L_, "set (nullable, a1)/unset (nullable, null, a)",
                                                   &SNA1, &UNNA, &UNNA, },

  { L_, "set (non-nullable, a2)/unset (non-nullable, a)",
                                                   &SA2,  &UA,   &UA,   },
  { L_, "set (nullable, a2)/unset (non-nullable, a)",
                                                   &SNA2, &UA,   &UA,   },
  { L_, "set (non-nullable, a2)/unset (nullable, non-null a)",
                                                   &SA2,  &UNA,  &UNA,  },
  { L_, "set (nullable, a2)/unset (nullable, non-null, a)",
                                                   &SNA2, &UNA,  &UNA,  },
  { L_, "set (non-nullable, a2)/unset (nullable, null a)",
                                                   &SA2,  &UNNA, &UA,   },
  { L_, "set (nullable, a2)/unset (nullable, null, a)",
                                                   &SNA2, &UNNA, &UNNA, },

  { L_, "set (non-nullable, b1)/unset (non-nullable, b)",
                                                   &SB1,  &UB,   &UB,   },
  { L_, "set (nullable, b1)/unset (non-nullable, b)",
                                                   &SNB1, &UB,   &UB,   },
  { L_, "set (non-nullable, b1)/unset (nullable, non-null b)",
                                                   &SB1,  &UNB,  &UNB,  },
  { L_, "set (nullable, b1)/unset (nullable, non-null, b)",
                                                   &SNB1, &UNB,  &UNB,  },
  { L_, "set (non-nullable, b1)/unset (nullable, null b)",
                                                   &SB1,  &UNNB, &UB,   },
  { L_, "set (nullable, b1)/unset (nullable, null, b)",
                                                   &SNB1, &UNNB, &UNNB, },

  { L_, "set (non-nullable, b2)/unset (non-nullable, b)",
                                                   &SB2,  &UB,   &UB,   },
  { L_, "set (nullable, b2)/unset (non-nullable, b)",
                                                   &SNB2, &UB,   &UB,   },
  { L_, "set (non-nullable, b2)/unset (nullable, non-null b)",
                                                   &SB2,  &UNB,  &UNB,  },
  { L_, "set (nullable, b2)/unset (nullable, non-null, b)",
                                                   &SNB2, &UNB,  &UNB,  },
  { L_, "set (non-nullable, b2)/unset (nullable, null b)",
                                                   &SB2,  &UNNB, &UB,   },
  { L_, "set (nullable, b2)/unset (nullable, null, b)",
                                                   &SNB2, &UNNB, &UNNB, },

  // set/set
  { L_, "set (non-nullable, a1)/set (non-nullable, a1)",
                                                   &SA1,  &SA1,  &SA1,  },
  { L_, "set (non-nullable, a1)/set (nullable, a1)",
                                                   &SA1,  &SNA1, &SNA1, },
  { L_, "set (non-nullable, a1)/set (non-nullable, a2)",
                                                   &SA1,  &SA2,  &SA2,  },
  { L_, "set (non-nullable, a1)/set (nullable, a2)",
                                                   &SA1,  &SNA2, &SNA2, },
  { L_, "set (nullable, a1)/set (non-nullable, a1)",
                                                   &SNA1, &SA1,  &SA1,  },
  { L_, "set (nullable, a1)/set (nullable, a1)",
                                                   &SNA1, &SNA1, &SNA1, },
  { L_, "set (nullable, a1)/set (non-nullable, a2)",
                                                   &SNA1, &SA2,  &SA2,  },
  { L_, "set (nullable, a1)/set (nullable, a2)",
                                                   &SNA1, &SNA2, &SNA2, },

  { L_, "set (non-nullable, a2)/set (non-nullable, a1)",
                                                   &SA2,  &SA1,  &SA1,  },
  { L_, "set (non-nullable, a2)/set (nullable, a1)",
                                                   &SA2,  &SNA1, &SNA1, },
  { L_, "set (non-nullable, a2)/set (non-nullable, a2)",
                                                   &SA2,  &SA2,  &SA2,  },
  { L_, "set (non-nullable, a2)/set (nullable, a2)",
                                                   &SA2,  &SNA2, &SNA2, },
  { L_, "set (nullable, a2)/set (non-nullable, a1)",
                                                   &SNA2, &SA1,  &SA1,  },
  { L_, "set (nullable, a2)/set (nullable, a1)",
                                                   &SNA2, &SNA1, &SNA1, },
  { L_, "set (nullable, a2)/set (non-nullable, a2)",
                                                   &SNA2, &SA2,  &SA2,  },
  { L_, "set (nullable, a2)/set (nullable, a2)",
                                                   &SNA2, &SNA2, &SNA2, },

  { L_, "set (non-nullable, b1)/set (non-nullable, b1)",
                                                   &SB1,  &SB1,  &SB1,  },
  { L_, "set (non-nullable, b1)/set (nullable, b1)",
                                                   &SB1,  &SNB1, &SNB1, },
  { L_, "set (non-nullable, b1)/set (non-nullable, b2)",
                                                   &SB1,  &SB2,  &SB2,  },
  { L_, "set (non-nullable, b1)/set (nullable, b2)",
                                                   &SB1,  &SNB2, &SNB2, },
  { L_, "set (nullable, b1)/set (non-nullable, b1)",
                                                   &SNB1, &SB1,  &SB1,  },
  { L_, "set (nullable, b1)/set (nullable, b1)",
                                                   &SNB1, &SNB1, &SNB1, },
  { L_, "set (nullable, b1)/set (non-nullable, b2)",
                                                   &SNB1, &SB2,  &SB2,  },
  { L_, "set (nullable, b1)/set (nullable, b2)",
                                                   &SNB1, &SNB2, &SNB2, },

  { L_, "set (non-nullable, b2)/set (non-nullable, b1)",
                                                   &SB2,  &SB1,  &SB1,  },
  { L_, "set (non-nullable, b2)/set (nullable, b1)",
                                                   &SB2,  &SNB1, &SNB1, },
  { L_, "set (non-nullable, b2)/set (non-nullable, b2)",
                                                   &SB2,  &SB2,  &SB2,  },
  { L_, "set (non-nullable, b2)/set (nullable, b2)",
                                                   &SB2,  &SNB2, &SNB2, },
  { L_, "set (nullable, b2)/set (non-nullable, b1)",
                                                   &SNB2, &SB1,  &SB1,  },
  { L_, "set (nullable, b2)/set (nullable, b1)",
                                                   &SNB2, &SNB1, &SNB1, },
  { L_, "set (nullable, b2)/set (non-nullable, b2)",
                                                   &SNB2, &SB2,  &SB2,  },
  { L_, "set (nullable, b2)/set (nullable, b2)",
                                                   &SNB2, &SNB2, &SNB2, },
//^------------>|
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE        = DATA[i].d_line;
                const char     *const DESCRIPTION = DATA[i].d_description;
                const ELEMREFS *const EREFS1      = DATA[i].d_elemRefs1;
                const ELEMREFS *const EREFS2      = DATA[i].d_elemRefs2;
                const ELEMREFS *const EEREFS      = DATA[i].d_expectedElemRefs;

                if (veryVerbose) bsl::cout << "\t\t" << DESCRIPTION
                                           << bsl::endl;

                // should not compile
                // EREFS1->d_CER.replaceValue(EREFS2->d_ER);
                // EREFS1->d_cCER.replaceValue(EREFS2->d_cER);
                EREFS1->d_ER.replaceValue(EREFS2->d_ER);

                LOOP3_ASSERT(LINE, EREFS1->d_CER, EEREFS->d_CER,
                             EREFS1->d_CER == EEREFS->d_CER);
                LOOP3_ASSERT(LINE, EREFS1->d_CER, EEREFS->d_CER,
                             EREFS1->d_CER == EEREFS->d_cCER);
                LOOP3_ASSERT(LINE, EREFS1->d_CER, EEREFS->d_CER,
                             EREFS1->d_CER == EEREFS->d_ER);
                LOOP3_ASSERT(LINE, EREFS1->d_CER, EEREFS->d_CER,
                             EREFS1->d_CER == EEREFS->d_cER);

                // reset elements to control values and try again with
                // 'const bdem::Elemref'
                resetDataFromControls();

                EREFS1->d_cER.replaceValue(EREFS2->d_ER);

                LOOP3_ASSERT(LINE, EREFS1->d_CER, EEREFS->d_CER,
                             EREFS1->d_CER == EEREFS->d_CER);
                LOOP3_ASSERT(LINE, EREFS1->d_CER, EEREFS->d_CER,
                             EREFS1->d_CER == EEREFS->d_cCER);
                LOOP3_ASSERT(LINE, EREFS1->d_CER, EEREFS->d_CER,
                             EREFS1->d_CER == EEREFS->d_ER);
                LOOP3_ASSERT(LINE, EREFS1->d_CER, EEREFS->d_CER,
                             EREFS1->d_CER == EEREFS->d_cER);

                  // reset elements to control values
                  resetDataFromControls();
              }
          }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // NON-DESCRIPTOR-BASED METHODS
        //   This case verifies remaining functions that do not require
        //   descriptors.
        //
        // Concerns:
        //   - the static method 'unboundElemRef' produces an unbound
        //     reference as defined by 'isBound'.
        //   - interoperability of 'const' and non-'const' methods for both
        //     base and derived objects.
        //
        // Plan:
        //   - Verify that we can create and verify unset instances
        //     of both 'bdlmxxx::ConstElemRef' and 'bdlmxxx::ElemRef'.
        //
        // Tactics:
        //   - Ad-Hoc test data selection method
        //   - Brute Force implementation technique
        //
        // Testing:
        //                      bdlmxxx::ConstElemRef
        //                      ~~~~~~~~~~~~~~~~~
        //   static bdlmxxx::ConstElemRef unboundElemRef();
        //   bool isBound() const;
        //
        //                      bdlmxxx::ElemRef
        //                      ~~~~~~~~~~~~
        //   static bdlmxxx::ElemRef unboundElemRef();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "NON-DESCRIPTOR-BASED METHODS" << endl
                                  << "============================" << endl;

        if (veryVerbose) printERefs();

        if (verbose) bsl::cout << "\nisBound" << bsl::endl;
        {
            const struct {
                int             d_line;
                const char     *d_description;
                const ELEMREFS *d_elemRefs;
                bool            d_expectedValue;
            } DATA[] = {
                // Line        Description             ERefs     EV
                // ==== ===========================  =========  =====
                { L_, "unbound",                   &UNBOUND,  false },
                { L_, "unbound (nullable)",        &UNBOUNDN, false },
                { L_, "null (a)",                  &NA,       true  },
                { L_, "unset (non-nullable, a)",   &UA,       true  },
                { L_, "unset (nullable, a)",       &UNA,      true  },
                { L_, "unset (nullable, null, a)", &UNNA,     true  },
                { L_, "set (non-nullable, a1)",    &SA1,      true  },
                { L_, "set (non-nullable, a2)",    &SA2,      true  },
                { L_, "set (nullable, a1)",        &SNA1,     true  },
                { L_, "set (nullable, a2)",        &SNA2,     true  },

                { L_, "null (b)",                  &NB,       true  },
                { L_, "unset (non-nullable, b)",   &UB,       true  },
                { L_, "unset (nullable, b)",       &UNB,      true  },
                { L_, "unset (nullable, null, b)", &UNNB,     true  },
                { L_, "set (non-nullable, b1)",    &SB1,      true  },
                { L_, "set (non-nullable, b2)",    &SB2,      true  },
                { L_, "set (nullable, b1)",        &SNB1,     true  },
                { L_, "set (nullable, b2)",        &SNB2,     true  }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE        = DATA[i].d_line;
                const char     *const DESCRIPTION = DATA[i].d_description;
                const ELEMREFS *const EREFS       = DATA[i].d_elemRefs;
                const bool            EV          = DATA[i].d_expectedValue;

                if (veryVerbose) bsl::cout << "\t" << DESCRIPTION << bsl::endl;

                LOOP_ASSERT(LINE, EV == EREFS->d_CER.isBound());
                LOOP_ASSERT(LINE, EV == EREFS->d_cCER.isBound());
                LOOP_ASSERT(LINE, EV == EREFS->d_ER.isBound());
                LOOP_ASSERT(LINE, EV == EREFS->d_cER.isBound());
            }
        }

        if (verbose) cout << "\n'unboundElemRef'" << endl;

        const bdlmxxx::ConstElemRef UCER = bdlmxxx::ConstElemRef::unboundElemRef();
              bdlmxxx::ConstElemRef ucer = bdlmxxx::ConstElemRef::unboundElemRef();
        const bdlmxxx::ElemRef      UER  = bdlmxxx::ElemRef::unboundElemRef();
              bdlmxxx::ElemRef      uer  = bdlmxxx::ElemRef::unboundElemRef();

        ASSERT(   0 == UCER.isBound());
        ASSERT(UCER == UNBOUND.d_cCER);
        ASSERT(UCER == UNBOUNDN.d_cCER);

        ASSERT(   0 == ucer.isBound());
        ASSERT(ucer == UNBOUND.d_CER);
        ASSERT(ucer == UNBOUNDN.d_CER);

        ASSERT(   0 == UER.isBound());
        ASSERT(UER == UNBOUND.d_cER);
        ASSERT(UER == UNBOUNDN.d_cER);

        ASSERT(   0 == uer.isBound());
        ASSERT(UCER == UNBOUND.d_ER);
        ASSERT(UCER == UNBOUNDN.d_ER);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // MODIFIABLE REFERENCES
        //
        // Concerns:
        //   - We must not define full descriptors for bdlmxxx::List, bdlmxxx::Choice,
        //     bdlmxxx::Table, or bdlmxxx::ChoiceArray since that would necessitate
        //     defining them, which would introduce a circular dependency.
        //   - We can only test isNull() on the other types.
        //
        // Plan:
        //   - Test 'theModifiable()' in two modes, one for all type below
        //     bdlmxxx::ElemRef, and another for the undefined type above it.
        //
        // Testing:
        //                     bdlmxxx::ElemRef
        //                     ~~~~~~~~~~~~
        // bool& theModifiableBool() const;
        // char& theModifiableChar() const;
        // short& theModifiableShort() const;
        // int& theModifiableInt() const;
        // bsls::Types::Int64& theModifiableInt64() const;
        // float& theModifiableFloat() const;
        // double& theModifiableDouble() const;
        // bsl::string& theModifiableString() const;
        // bdlt::Datetime& theModifiableDatetime() const;
        // bdlt::DatetimeTz& theModifiableDateTimeTz() const;
        // bdlt::Date& theModifiableDate() const;
        // bdlt::DateTz& theModifiableDateTz() const;
        // bdlt::Time& theModifiableTime() const;
        // bdlt::TimeTz& theModifiableTimeTz() const;
        // bsl::vector<bool>& theModifiableBoolArray() const;
        // bsl::vector<char>& theModifiableCharArray() const;
        // bsl::vector<short>& theModifiableShortArray() const;
        // bsl::vector<int>& theModifiableIntArray() const;
        // bsl::vector<bsls::Types::Int64>& theModifiableInt64Array() const;
        // bsl::vector<float>& theModifiableFloatArray() const;
        // bsl::vector<double>& theModifiableDoubleArray() const;
        // bsl::vector<bsl::string>& theModifiableStringArray() const;
        // bsl::vector<bdlt::Datetime>& theModifiableDatetimeArray() const;
        // bsl::vector<bdlt::DatetimeTz>& theModifiableDatetimeTzArray() const;
        // bsl::vector<bdlt::Date>& theModifiableDateArray() const;
        // bsl::vector<bdlt::DateTz>& theModifiableDateTzArray() const;
        // bsl::vector<bdlt::Time>& theModifiableTimeArray() const;
        // bsl::vector<bdlt::TimeTz>& theModifiableTimeTzArray() const;
        // bdlmxxx::List& theModifiableList() const;
        // bdlmxxx::Table& theModifiableTable() const;
        // bdlmxxx::Choice& theModifiableChoice() const;
        // bdlmxxx::ChoiceArray& theModifiableChoiceArray() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nREFERENCES TO MODIFIABLE OBJECTS" << endl <<
                               "================================" << endl;

#define TEST_THEMODIFIABLE(ETYPE, ELTYPE, TYPE)                             \
    {                                                                       \
        if (veryVerbose) bsl::cout << "\ttheModifiable" << #ETYPE           \
                                   << bsl::endl;                            \
                                                                            \
        LOOP2_ASSERT(ELTYPE, #ELTYPE, ELTYPE < DESCRIPTOR_LOOKUP_LENGTH);   \
        TYPE  data = 0;                                                     \
        TYPE *pType;                                                        \
        const bdlmxxx::Descriptor *desc = descriptorLookup[ELTYPE].d_desc;      \
        const bdlmxxx::ElemRef ER(&data, desc);                                 \
        ASSERT(!ER.isNullable());                                           \
        ASSERT(!ER.isNull());                                               \
        ASSERT( ER.isNonNull());                                            \
        ASSERT(!isUnset(ER));                                               \
                                                                            \
        ER.makeNull();                                                      \
        ASSERT(!ER.isNull());                                               \
        ASSERT( ER.isNonNull());                                            \
        ASSERT( isUnset(ER));                                               \
                                                                            \
        data = 0;                                                           \
        int nullBits = 0xfffffff7;                                          \
        const bdlmxxx::ElemRef NER(&data, desc, &nullBits, 3);                  \
        ASSERT(!((1 << 3) & nullBits));                                     \
        ASSERT( NER.isNullable());                                          \
        ASSERT(!NER.isNull());                                              \
        ASSERT( NER.isNonNull());                                           \
        ASSERT(!isUnset(NER));                                              \
                                                                            \
        pType = const_cast<TYPE *>(&NER.the ## ETYPE());                    \
        ASSERT(&data == pType);                                             \
        ASSERT(!((1 << 3) & nullBits));                                     \
        ASSERT(!NER.isNull());                                              \
        ASSERT( NER.isNonNull());                                           \
        ASSERT(!isUnset(NER));                                              \
                                                                            \
        pType = &NER.theModifiable ## ETYPE();                              \
        ASSERT(&data == pType);                                             \
        ASSERT(!((1 << 3) & nullBits));                                     \
        ASSERT(!NER.isNull());                                              \
        ASSERT( NER.isNonNull());                                           \
        ASSERT(!isUnset(NER));                                              \
                                                                            \
        NER.makeNull();                                                     \
        ASSERT((1 << 3) & nullBits);                                        \
        ASSERT( NER.isNull());                                              \
        ASSERT(!NER.isNonNull());                                           \
        ASSERT( isUnset(NER));                                              \
                                                                            \
        pType = const_cast<TYPE *>(&NER.the ## ETYPE());                    \
        ASSERT(&data == pType);                                             \
        ASSERT((1 << 3) & nullBits);                                        \
        ASSERT( NER.isNull());                                              \
        ASSERT(!NER.isNonNull());                                           \
        ASSERT( isUnset(NER));                                              \
                                                                            \
        pType = &NER.theModifiable ## ETYPE();                              \
        ASSERT(&data == pType);                                             \
        ASSERT(!((1 << 3) & nullBits));                                     \
        ASSERT(!NER.isNull());                                              \
        ASSERT( NER.isNonNull());                                           \
        ASSERT( isUnset(NER));                                              \
    }

        //                 ETYPE            ELTYPE                TYPE
        //                 =====            ======                ===========
        TEST_THEMODIFIABLE(Short          , ET::BDEM_SHORT,       short);
        TEST_THEMODIFIABLE(Int            , ET::BDEM_INT,         int);
        TEST_THEMODIFIABLE(Double         , ET::BDEM_DOUBLE,      double);

#undef TEST_THEMODIFIABLE

        // Test the types that are ABOVE 'bdlmxxx_elemref'.  These types are
        // declared but not defined in this component (with the exception of
        // 'bdlmxxx::Table', which is faked above), so we can't set up a
        // fully-functional descriptor for them, meaning most operations,
        // including 'makeNull()', won't work.

#define TEST_THEMODIFIABLE_ABOVE(CTYPE, LCASETYPE, UCASETYPE)             \
    {                                                                     \
        if (veryVerbose) bsl::cout << "\ttheModifiable" << #UCASETYPE     \
                                   << bsl::endl;                          \
                                                                          \
        double data[10];                                                  \
        const bdlmxxx::ElemRef ER(data, & LCASETYPE ## Descriptor);           \
        CTYPE *pType = &ER.theModifiable ## UCASETYPE();                  \
        ASSERT((CTYPE *) data == pType);                                  \
        pType = 0;                                                        \
                                                                          \
        int nullBits = 0xffffffff;                                        \
        const bdlmxxx::ElemRef NER(data,                                      \
                                &LCASETYPE ## Descriptor, &nullBits, 3);  \
        ASSERT((1 << 3) & nullBits);                                      \
        pType = const_cast<CTYPE *>(&NER.the ## UCASETYPE());             \
        ASSERT((CTYPE *) data == pType);                                  \
        ASSERT((1 << 3) & nullBits);                                      \
        pType = 0;                                                        \
        pType = &NER.theModifiable ## UCASETYPE();                        \
        ASSERT((CTYPE *) data == pType);                                  \
        ASSERT(!((1 << 3) & nullBits));                                   \
    }

        TEST_THEMODIFIABLE_ABOVE(bdlmxxx::List,        list,        List);
        TEST_THEMODIFIABLE_ABOVE(bdlmxxx::Table,       table,       Table);
        TEST_THEMODIFIABLE_ABOVE(bdlmxxx::Choice,      choice,      Choice);
        TEST_THEMODIFIABLE_ABOVE(bdlmxxx::ChoiceArray, choiceArray, ChoiceArray);

#undef TEST_THEMODIFIABLE_ABOVE

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ELEMENT ACCESSORS
        //   This case verifies all element accessors.
        //
        // Concerns:
        //   - That a reference of appropriate type is returned.
        //   - That a reference of appropriate 'const'-ness is returned.
        //   - That a reference is to the memory specified at construction.
        //   - That the C++ 'const'-ness of the reference object has no effect.
        //
        // Plan:
        //   - Create at least two modifiable instances of each type.
        //   - Create const references to these respective instances.
        //   - Verify the type, 'const'-ness, and value of each value returned.
        //
        // Tactics:
        //   - Ad-Hoc test data selection method
        //   - Brute Force implementation technique
        //
        // Testing:
        //                     bdlmxxx::ConstElemRef
        //                     ~~~~~~~~~~~~~~~~~
        //   const bool& theBool() const;
        //   const char& theChar() const;
        //   const short& theShort() const;
        //   const int& theInt() const;
        //   const bsls::Types::Int64& theInt64() const;
        //   const float& theFloat() const;
        //   const double& theDouble() const;
        //   const bsl::string& theString() const;
        //   const bdlt::Datetime& theDatetime() const;
        //   const bdlt::DatetimeTz& theDatetimeTz() const;
        //   const bdlt::Date& theDate() const;
        //   const bdlt::DateTz& theDateTz() const;
        //   const bdlt::Time& theTime() const;
        //   const bdlt::TimeTz& theTimeTz() const;
        //   const vector<bool>& theBoolArray() const;
        //   const vector<char>& theCharArray() const;
        //   const vector<short>& theShortArray() const;
        //   const vector<int>& theIntArray() const;
        //   const vector<bsls::Types::Int64>& theInt64Array() const;
        //   const vector<float>& theFloatArray() const;
        //   const vector<double>& theDoubleArray() const;
        //   const vector<bsl::string>& theStringArray() const;
        //   const vector<bdlt::Datetime>& theDatetimeArray() const;
        //   const vector<bdlt::DatetimeTz>& theDatetimeTzArray() const;
        //   const vector<bdlt::Date>& theDateArray() const;
        //   const vector<bdlt::DateTz>& theDateTzArray() const;
        //   const vector<bdlt::Time>& theTimeArray() const;
        //   const vector<bdlt::TimeTz>& theTimeTzArray() const;
        //   const bdlmxxx::Row& theRow() const;
        //   const bdlmxxx::List& theList() const;
        //   const bdlmxxx::Table& theTable() const;
        //   const bdlmxxx::ChoiceArrayItem& theChoiceArrayItem() const;
        //   const bdlmxxx::Choice& theChoice() const;
        //   const bdlmxxx::ChoiceArray& theChoiceArray() const;
        //
        //                     bdlmxxx::ElemRef
        //                     ~~~~~~~~~~~~
        //   const bool& theBool() const;
        //   const char& theChar() const;
        //   const short& theShort() const;
        //   const int& theInt() const;
        //   const bsls::Types::Int64& theInt64() const;
        //   const float& theFloat() const;
        //   const double& theDouble() const;
        //   const bsl::string& theString() const;
        //   const bdlt::Datetime& theDatetime() const;
        //   const bdlt::DatetimeTz& theDatetimeTz() const;
        //   const bdlt::Date& theDate() const;
        //   const bdlt::DateTz& theDateTz() const;
        //   const bdlt::Time& theTime() const;
        //   const bdlt::TimeTz& theTimeTz() const;
        //   const vector<bool>& theBoolArray() const;
        //   const vector<char>& theCharArray() const;
        //   const vector<short>& theShortArray() const;
        //   const vector<int>& theIntArray() const;
        //   const vector<bsls::Types::Int64>& theInt64Array() const;
        //   const vector<float>& theFloatArray() const;
        //   const vector<double>& theDoubleArray() const;
        //   const vector<bsl::string>& theStringArray() const;
        //   const vector<bdlt::Datetime>& theDatetimeArray() const;
        //   const vector<bdlt::DatetimeTz>& theDatetimeTzArray() const;
        //   const vector<bdlt::Date>& theDateArray() const;
        //   const vector<bdlt::DateTz>& theDateTzArray() const;
        //   const vector<bdlt::Time>& theTimeArray() const;
        //   const vector<bdlt::TimeTz>& theTimeTzArray() const;
        //   const bdlmxxx::Row& theRow() const;
        //   const bdlmxxx::List& theList() const;
        //   const bdlmxxx::Table& theTable() const;
        //   const bdlmxxx::ChoiceArrayItem& theChoiceArrayItem() const;
        //   const bdlmxxx::Choice& theChoice() const;
        //   const bdlmxxx::ChoiceArray& theChoiceArray() const;
        //
        //   bool& theModifiableBool() const;
        //   char& theModifiableChar() const;
        //   short& theModifiableShort() const;
        //   int& theModifiableInt() const;
        //   bsls::Types::Int64& theModifiableInt64() const;
        //   float& theModifiableFloat() const;
        //   double& theModifiableDouble() const;
        //   bsl::string& theModifiableString() const;
        //   bdlt::Datetime& theModifiableDatetime() const;
        //   bdlt::DatetimeTz& theModifiableDatetimeTz() const;
        //   bdlt::Date& theModifiableDate() const;
        //   bdlt::DateTz& theModifiableDateTz() const;
        //   bdlt::Time& theModifiableTime() const;
        //   bdlt::TimeTz& theModifiableTimeTz() const;
        //   vector<bool>& theModifiableBoolArray() const;
        //   vector<char>& theModifiableCharArray() const;
        //   vector<short>& theModifiableShortArray() const;
        //   vector<int>& theModifiableIntArray() const;
        //   vector<bsls::Types::Int64>& theModifiableInt64Array() const;
        //   vector<float>& theModifiableFloatArray() const;
        //   vector<double>& theModifiableDoubleArray() const;
        //   vector<bdlt::Datetime>& theModifiableDatetimeArray() const;
        //   vector<bdlt::DatetimeTz>& theModifiableDatetimeTzArray() const;
        //   vector<bdlt::Date>& theModifiableDateArray() const;
        //   vector<bdlt::DateTz>& theModifiableDateTzArray() const;
        //   vector<bdlt::Time>& theModifiableTimeArray() const;
        //   vector<bdlt::TimeTz>& theModifiableTimeTzArray() const;
        //   bdlmxxx::Row& theModifiableRow() const;
        //   bdlmxxx::List& theModifiableList() const;
        //   bdlmxxx::Table& theModifiableTable() const;
        //   bdlmxxx::ChoiceArrayItem& theModifiableChoiceArrayItem() const;
        //   bdlmxxx::Choice& theModifiableChoice() const;
        //   bdlmxxx::ChoiceArray& theModifiableChoiceArray() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "ELEMENT ACCESSORS" << endl
                                  << "=================" << endl;

              void *d1 = reinterpret_cast<void *>(0x1);
        const void *D1 = d1;

              void *d2 = reinterpret_cast<void *>(0x2);
        const void *D2 = d2;

        int tIdx = -3;  // help ensure all types are covered

        if (verbose) cout <<
            "\nVerify type, value, and 'const'-ness for each type.\n";
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &choiceArrayItemDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef bdlmxxx::ChoiceArrayItem Type; Type *rp; const Type *crp;
            crp = &cer1.theChoiceArrayItem();         ASSERT(D1 == crp);
            crp = &CER1.theChoiceArrayItem();         ASSERT(D1 == crp);
            crp = &cer2.theChoiceArrayItem();         ASSERT(D2 == crp);
            crp = &CER2.theChoiceArrayItem();         ASSERT(D2 == crp);

             rp = &er1.theModifiableChoiceArrayItem();  ASSERT(D1 ==  rp);
            crp = &ER1.theChoiceArrayItem();            ASSERT(D1 == crp);
             rp = &er2.theModifiableChoiceArrayItem();  ASSERT(D2 ==  rp);
            crp = &ER2.theChoiceArrayItem();            ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl;
        tIdx += 2;
        {
            const bdlmxxx::Descriptor *A = &rowDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef bdlmxxx::Row Type; Type *rp; const Type *crp;
            crp = &cer1.theRow();              ASSERT(D1 == crp);
            crp = &CER1.theRow();              ASSERT(D1 == crp);
            crp = &cer2.theRow();              ASSERT(D2 == crp);
            crp = &CER2.theRow();              ASSERT(D2 == crp);

             rp = &er1.theModifiableRow();     ASSERT(D1 ==  rp);
            crp = &ER1.theRow();               ASSERT(D1 == crp);
             rp = &er2.theModifiableRow();     ASSERT(D2 ==  rp);
            crp = &ER2.theRow();               ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &charDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef char Type; Type *rp; const Type *crp;
            crp = &cer1.theChar();              ASSERT(D1 == crp);
            crp = &CER1.theChar();              ASSERT(D1 == crp);
            crp = &cer2.theChar();              ASSERT(D2 == crp);
            crp = &CER2.theChar();              ASSERT(D2 == crp);

             rp = &er1.theModifiableChar();     ASSERT(D1 ==  rp);
            crp = &ER1.theChar();               ASSERT(D1 == crp);
             rp = &er2.theModifiableChar();     ASSERT(D2 ==  rp);
            crp = &ER2.theChar();               ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &shortDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef short Type; Type *rp; const Type *crp;
            crp = &cer1.theShort();             ASSERT(D1 == crp);
            crp = &CER1.theShort();             ASSERT(D1 == crp);
            crp = &cer2.theShort();             ASSERT(D2 == crp);
            crp = &CER2.theShort();             ASSERT(D2 == crp);

             rp = &er1.theModifiableShort();    ASSERT(D1 ==  rp);
            crp = &ER1.theShort();              ASSERT(D1 == crp);
             rp = &er2.theModifiableShort();    ASSERT(D2 ==  rp);
            crp = &ER2.theShort();              ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &intDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef int Type; Type *rp; const Type *crp;
            crp = &cer1.theInt();               ASSERT(D1 == crp);
            crp = &CER1.theInt();               ASSERT(D1 == crp);
            crp = &cer2.theInt();               ASSERT(D2 == crp);
            crp = &CER2.theInt();               ASSERT(D2 == crp);

             rp = &er1.theModifiableInt();      ASSERT(D1 ==  rp);
            crp = &ER1.theInt();                ASSERT(D1 == crp);
             rp = &er2.theModifiableInt();      ASSERT(D2 ==  rp);
            crp = &ER2.theInt();                ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &int64Descriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef Int64 Type; Type *rp; const Type *crp;
            crp = &cer1.theInt64();             ASSERT(D1 == crp);
            crp = &CER1.theInt64();             ASSERT(D1 == crp);
            crp = &cer2.theInt64();             ASSERT(D2 == crp);
            crp = &CER2.theInt64();             ASSERT(D2 == crp);

             rp = &er1.theModifiableInt64();    ASSERT(D1 ==  rp);
            crp = &ER1.theInt64();              ASSERT(D1 == crp);
             rp = &er2.theModifiableInt64();    ASSERT(D2 ==  rp);
            crp = &ER2.theInt64();              ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &floatDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef float Type; Type *rp; const Type *crp;
            crp = &cer1.theFloat();             ASSERT(D1 == crp);
            crp = &CER1.theFloat();             ASSERT(D1 == crp);
            crp = &cer2.theFloat();             ASSERT(D2 == crp);
            crp = &CER2.theFloat();             ASSERT(D2 == crp);

             rp = &er1.theModifiableFloat();    ASSERT(D1 ==  rp);
            crp = &ER1.theFloat();              ASSERT(D1 == crp);
             rp = &er2.theModifiableFloat();    ASSERT(D2 ==  rp);
            crp = &ER2.theFloat();              ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &doubleDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef double Type; Type *rp; const Type *crp;
            crp = &cer1.theDouble();            ASSERT(D1 == crp);
            crp = &CER1.theDouble();            ASSERT(D1 == crp);
            crp = &cer2.theDouble();            ASSERT(D2 == crp);
            crp = &CER2.theDouble();            ASSERT(D2 == crp);

             rp = &er1.theModifiableDouble();   ASSERT(D1 ==  rp);
            crp = &ER1.theDouble();             ASSERT(D1 == crp);
             rp = &er2.theModifiableDouble();   ASSERT(D2 ==  rp);
            crp = &ER2.theDouble();             ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &stringDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef bsl::string Type; Type *rp; const Type *crp;
            crp = &cer1.theString();            ASSERT(D1 == crp);
            crp = &CER1.theString();            ASSERT(D1 == crp);
            crp = &cer2.theString();            ASSERT(D2 == crp);
            crp = &CER2.theString();            ASSERT(D2 == crp);

             rp = &er1.theModifiableString();   ASSERT(D1 ==  rp);
            crp = &ER1.theString();             ASSERT(D1 == crp);
             rp = &er2.theModifiableString();   ASSERT(D2 ==  rp);
            crp = &ER2.theString();             ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &datetimeDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef bdlt::Datetime Type; Type *rp; const Type *crp;
            crp = &cer1.theDatetime();            ASSERT(D1 == crp);
            crp = &CER1.theDatetime();            ASSERT(D1 == crp);
            crp = &cer2.theDatetime();            ASSERT(D2 == crp);
            crp = &CER2.theDatetime();            ASSERT(D2 == crp);

             rp = &er1.theModifiableDatetime(); ASSERT(D1 ==  rp);
            crp = &ER1.theDatetime();           ASSERT(D1 == crp);
             rp = &er2.theModifiableDatetime(); ASSERT(D2 ==  rp);
            crp = &ER2.theDatetime();           ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &dateDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef bdlt::Date Type; Type *rp; const Type *crp;
            crp = &cer1.theDate();              ASSERT(D1 == crp);
            crp = &CER1.theDate();              ASSERT(D1 == crp);
            crp = &cer2.theDate();              ASSERT(D2 == crp);
            crp = &CER2.theDate();              ASSERT(D2 == crp);

             rp = &er1.theModifiableDate();     ASSERT(D1 ==  rp);
            crp = &ER1.theDate();               ASSERT(D1 == crp);
             rp = &er2.theModifiableDate();     ASSERT(D2 ==  rp);
            crp = &ER2.theDate();               ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &timeDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef bdlt::Time Type; Type *rp; const Type *crp;
            crp = &cer1.theTime();              ASSERT(D1 == crp);
            crp = &CER1.theTime();              ASSERT(D1 == crp);
            crp = &cer2.theTime();              ASSERT(D2 == crp);
            crp = &CER2.theTime();              ASSERT(D2 == crp);

             rp = &er1.theModifiableTime();     ASSERT(D1 ==  rp);
            crp = &ER1.theTime();               ASSERT(D1 == crp);
             rp = &er2.theModifiableTime();     ASSERT(D2 ==  rp);
            crp = &ER2.theTime();               ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &charArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef vector<char> Type; Type *rp; const Type *crp;
            crp = &cer1.theCharArray();         ASSERT(D1 == crp);
            crp = &CER1.theCharArray();         ASSERT(D1 == crp);
            crp = &cer2.theCharArray();         ASSERT(D2 == crp);
            crp = &CER2.theCharArray();         ASSERT(D2 == crp);

             rp = &er1.theModifiableCharArray();  ASSERT(D1 ==  rp);
            crp = &ER1.theCharArray();            ASSERT(D1 == crp);
             rp = &er2.theModifiableCharArray();  ASSERT(D2 ==  rp);
            crp = &ER2.theCharArray();            ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &shortArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef vector<short> Type; Type *rp; const Type *crp;
            crp = &cer1.theShortArray();        ASSERT(D1 == crp);
            crp = &CER1.theShortArray();        ASSERT(D1 == crp);
            crp = &cer2.theShortArray();        ASSERT(D2 == crp);
            crp = &CER2.theShortArray();        ASSERT(D2 == crp);

             rp = &er1.theModifiableShortArray(); ASSERT(D1 ==  rp);
            crp = &ER1.theShortArray();           ASSERT(D1 == crp);
             rp = &er2.theModifiableShortArray(); ASSERT(D2 ==  rp);
            crp = &ER2.theShortArray();           ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &intArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef vector<int> Type; Type *rp; const Type *crp;
            crp = &cer1.theIntArray();          ASSERT(D1 == crp);
            crp = &CER1.theIntArray();          ASSERT(D1 == crp);
            crp = &cer2.theIntArray();          ASSERT(D2 == crp);
            crp = &CER2.theIntArray();          ASSERT(D2 == crp);

             rp = &er1.theModifiableIntArray(); ASSERT(D1 ==  rp);
            crp = &ER1.theIntArray();           ASSERT(D1 == crp);
             rp = &er2.theModifiableIntArray(); ASSERT(D2 ==  rp);
            crp = &ER2.theIntArray();           ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &int64ArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef vector<Int64> Type; Type *rp; const Type *crp;
            crp = &cer1.theInt64Array();        ASSERT(D1 == crp);
            crp = &CER1.theInt64Array();        ASSERT(D1 == crp);
            crp = &cer2.theInt64Array();        ASSERT(D2 == crp);
            crp = &CER2.theInt64Array();        ASSERT(D2 == crp);

             rp = &er1.theModifiableInt64Array();         ASSERT(D1 ==  rp);
            crp = &ER1.theInt64Array();         ASSERT(D1 == crp);
             rp = &er2.theModifiableInt64Array();         ASSERT(D2 ==  rp);
            crp = &ER2.theInt64Array();         ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &floatArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef vector<float> Type; Type *rp; const Type *crp;
            crp = &cer1.theFloatArray();        ASSERT(D1 == crp);
            crp = &CER1.theFloatArray();        ASSERT(D1 == crp);
            crp = &cer2.theFloatArray();        ASSERT(D2 == crp);
            crp = &CER2.theFloatArray();        ASSERT(D2 == crp);

             rp = &er1.theModifiableFloatArray();         ASSERT(D1 ==  rp);
            crp = &ER1.theFloatArray();         ASSERT(D1 == crp);
             rp = &er2.theModifiableFloatArray();         ASSERT(D2 ==  rp);
            crp = &ER2.theFloatArray();         ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &doubleArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef vector<double> Type; Type *rp; const Type *crp;
            crp = &cer1.theDoubleArray();       ASSERT(D1 == crp);
            crp = &CER1.theDoubleArray();       ASSERT(D1 == crp);
            crp = &cer2.theDoubleArray();       ASSERT(D2 == crp);
            crp = &CER2.theDoubleArray();       ASSERT(D2 == crp);

             rp = &er1.theModifiableDoubleArray();        ASSERT(D1 ==  rp);
            crp = &ER1.theDoubleArray();        ASSERT(D1 == crp);
             rp = &er2.theModifiableDoubleArray();        ASSERT(D2 ==  rp);
            crp = &ER2.theDoubleArray();        ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &stringArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef vector<bsl::string> Type; Type *rp; const Type *crp;
            crp = &cer1.theStringArray();       ASSERT(D1 == crp);
            crp = &CER1.theStringArray();       ASSERT(D1 == crp);
            crp = &cer2.theStringArray();       ASSERT(D2 == crp);
            crp = &CER2.theStringArray();       ASSERT(D2 == crp);

             rp = &er1.theModifiableStringArray();        ASSERT(D1 ==  rp);
            crp = &ER1.theStringArray();        ASSERT(D1 == crp);
             rp = &er2.theModifiableStringArray();        ASSERT(D2 ==  rp);
            crp = &ER2.theStringArray();        ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &datetimeArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef vector<bdlt::Datetime> Type; Type *rp; const Type *crp;
            crp = &cer1.theDatetimeArray();     ASSERT(D1 == crp);
            crp = &CER1.theDatetimeArray();     ASSERT(D1 == crp);
            crp = &cer2.theDatetimeArray();     ASSERT(D2 == crp);
            crp = &CER2.theDatetimeArray();     ASSERT(D2 == crp);

             rp = &er1.theModifiableDatetimeArray();      ASSERT(D1 ==  rp);
            crp = &ER1.theDatetimeArray();      ASSERT(D1 == crp);
             rp = &er2.theModifiableDatetimeArray();      ASSERT(D2 ==  rp);
            crp = &ER2.theDatetimeArray();      ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &dateArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef vector<bdlt::Date> Type; Type *rp; const Type *crp;
            crp = &cer1.theDateArray();         ASSERT(D1 == crp);
            crp = &CER1.theDateArray();         ASSERT(D1 == crp);
            crp = &cer2.theDateArray();         ASSERT(D2 == crp);
            crp = &CER2.theDateArray();         ASSERT(D2 == crp);

             rp = &er1.theModifiableDateArray();  ASSERT(D1 ==  rp);
            crp = &ER1.theDateArray();            ASSERT(D1 == crp);
             rp = &er2.theModifiableDateArray();  ASSERT(D2 ==  rp);
            crp = &ER2.theDateArray();            ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &timeArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef vector<bdlt::Time> Type; Type *rp; const Type *crp;
            crp = &cer1.theTimeArray();         ASSERT(D1 == crp);
            crp = &CER1.theTimeArray();         ASSERT(D1 == crp);
            crp = &cer2.theTimeArray();         ASSERT(D2 == crp);
            crp = &CER2.theTimeArray();         ASSERT(D2 == crp);

             rp = &er1.theModifiableTimeArray();  ASSERT(D1 ==  rp);
            crp = &ER1.theTimeArray();            ASSERT(D1 == crp);
             rp = &er2.theModifiableTimeArray();  ASSERT(D2 ==  rp);
            crp = &ER2.theTimeArray();            ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &listDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef bdlmxxx::List Type; Type *rp; const Type *crp;
            crp = &cer1.theList();              ASSERT(D1 == crp);
            crp = &CER1.theList();              ASSERT(D1 == crp);
            crp = &cer2.theList();              ASSERT(D2 == crp);
            crp = &CER2.theList();              ASSERT(D2 == crp);

             rp = &er1.theModifiableList();     ASSERT(D1 ==  rp);
            crp = &ER1.theList();               ASSERT(D1 == crp);
             rp = &er2.theModifiableList();     ASSERT(D2 ==  rp);
            crp = &ER2.theList();               ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &tableDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef bdlmxxx::Table Type; Type *rp; const Type *crp;
            crp = &cer1.theTable();             ASSERT(D1 == crp);
            crp = &CER1.theTable();             ASSERT(D1 == crp);
            crp = &cer2.theTable();             ASSERT(D2 == crp);
            crp = &CER2.theTable();             ASSERT(D2 == crp);

             rp = &er1.theModifiableTable();    ASSERT(D1 ==  rp);
            crp = &ER1.theTable();              ASSERT(D1 == crp);
             rp = &er2.theModifiableTable();    ASSERT(D2 ==  rp);
            crp = &ER2.theTable();              ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &boolDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef bool Type; Type *rp; const Type *crp;
            crp = &cer1.theBool();              ASSERT(D1 == crp);
            crp = &CER1.theBool();              ASSERT(D1 == crp);
            crp = &cer2.theBool();              ASSERT(D2 == crp);
            crp = &CER2.theBool();              ASSERT(D2 == crp);

             rp = &er1.theModifiableBool();     ASSERT(D1 ==  rp);
            crp = &ER1.theBool();               ASSERT(D1 == crp);
             rp = &er2.theModifiableBool();     ASSERT(D2 ==  rp);
            crp = &ER2.theBool();               ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &datetimeTzDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef bdlt::DatetimeTz Type; Type *rp; const Type *crp;
            crp = &cer1.theDatetimeTz();          ASSERT(D1 == crp);
            crp = &CER1.theDatetimeTz();          ASSERT(D1 == crp);
            crp = &cer2.theDatetimeTz();          ASSERT(D2 == crp);
            crp = &CER2.theDatetimeTz();          ASSERT(D2 == crp);

             rp = &er1.theModifiableDatetimeTz(); ASSERT(D1 ==  rp);
            crp = &ER1.theDatetimeTz();           ASSERT(D1 == crp);
             rp = &er2.theModifiableDatetimeTz(); ASSERT(D2 ==  rp);
            crp = &ER2.theDatetimeTz();           ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &dateTzDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef bdlt::DateTz Type; Type *rp; const Type *crp;
            crp = &cer1.theDateTz();              ASSERT(D1 == crp);
            crp = &CER1.theDateTz();              ASSERT(D1 == crp);
            crp = &cer2.theDateTz();              ASSERT(D2 == crp);
            crp = &CER2.theDateTz();              ASSERT(D2 == crp);

             rp = &er1.theModifiableDateTz();     ASSERT(D1 ==  rp);
            crp = &ER1.theDateTz();               ASSERT(D1 == crp);
             rp = &er2.theModifiableDateTz();     ASSERT(D2 ==  rp);
            crp = &ER2.theDateTz();               ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &timeTzDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef bdlt::TimeTz Type; Type *rp; const Type *crp;
            crp = &cer1.theTimeTz();              ASSERT(D1 == crp);
            crp = &CER1.theTimeTz();              ASSERT(D1 == crp);
            crp = &cer2.theTimeTz();              ASSERT(D2 == crp);
            crp = &CER2.theTimeTz();              ASSERT(D2 == crp);

             rp = &er1.theModifiableTimeTz();     ASSERT(D1 ==  rp);
            crp = &ER1.theTimeTz();               ASSERT(D1 == crp);
             rp = &er2.theModifiableTimeTz();     ASSERT(D2 ==  rp);
            crp = &ER2.theTimeTz();               ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &boolArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef vector<bool> Type; Type *rp; const Type *crp;
            crp = &cer1.theBoolArray();         ASSERT(D1 == crp);
            crp = &CER1.theBoolArray();         ASSERT(D1 == crp);
            crp = &cer2.theBoolArray();         ASSERT(D2 == crp);
            crp = &CER2.theBoolArray();         ASSERT(D2 == crp);

             rp = &er1.theModifiableBoolArray();  ASSERT(D1 ==  rp);
            crp = &ER1.theBoolArray();            ASSERT(D1 == crp);
             rp = &er2.theModifiableBoolArray();  ASSERT(D2 ==  rp);
            crp = &ER2.theBoolArray();            ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &datetimeTzArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef vector<bdlt::DatetimeTz> Type; Type *rp; const Type *crp;
            crp = &cer1.theDatetimeTzArray();     ASSERT(D1 == crp);
            crp = &CER1.theDatetimeTzArray();     ASSERT(D1 == crp);
            crp = &cer2.theDatetimeTzArray();     ASSERT(D2 == crp);
            crp = &CER2.theDatetimeTzArray();     ASSERT(D2 == crp);

             rp = &er1.theModifiableDatetimeTzArray();      ASSERT(D1 ==  rp);
            crp = &ER1.theDatetimeTzArray();      ASSERT(D1 == crp);
             rp = &er2.theModifiableDatetimeTzArray();      ASSERT(D2 ==  rp);
            crp = &ER2.theDatetimeTzArray();      ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &dateTzArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef vector<bdlt::DateTz> Type; Type *rp; const Type *crp;
            crp = &cer1.theDateTzArray();         ASSERT(D1 == crp);
            crp = &CER1.theDateTzArray();         ASSERT(D1 == crp);
            crp = &cer2.theDateTzArray();         ASSERT(D2 == crp);
            crp = &CER2.theDateTzArray();         ASSERT(D2 == crp);

             rp = &er1.theModifiableDateTzArray();  ASSERT(D1 ==  rp);
            crp = &ER1.theDateTzArray();            ASSERT(D1 == crp);
             rp = &er2.theModifiableDateTzArray();  ASSERT(D2 ==  rp);
            crp = &ER2.theDateTzArray();            ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &timeTzArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef vector<bdlt::TimeTz> Type; Type *rp; const Type *crp;
            crp = &cer1.theTimeTzArray();         ASSERT(D1 == crp);
            crp = &CER1.theTimeTzArray();         ASSERT(D1 == crp);
            crp = &cer2.theTimeTzArray();         ASSERT(D2 == crp);
            crp = &CER2.theTimeTzArray();         ASSERT(D2 == crp);

             rp = &er1.theModifiableTimeTzArray();  ASSERT(D1 ==  rp);
            crp = &ER1.theTimeTzArray();            ASSERT(D1 == crp);
             rp = &er2.theModifiableTimeTzArray();  ASSERT(D2 ==  rp);
            crp = &ER2.theTimeTzArray();            ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &choiceDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef bdlmxxx::Choice Type; Type *rp; const Type *crp;
            crp = &cer1.theChoice();              ASSERT(D1 == crp);
            crp = &CER1.theChoice();              ASSERT(D1 == crp);
            crp = &cer2.theChoice();              ASSERT(D2 == crp);
            crp = &CER2.theChoice();              ASSERT(D2 == crp);

             rp = &er1.theModifiableChoice();     ASSERT(D1 ==  rp);
            crp = &ER1.theChoice();               ASSERT(D1 == crp);
             rp = &er2.theModifiableChoice();     ASSERT(D2 ==  rp);
            crp = &ER2.theChoice();               ASSERT(D2 == crp);
        }
        if (verbose) cout << '\t' << (bdlmxxx::ElemType::Type)tIdx << endl; ++tIdx;
        {
            const bdlmxxx::Descriptor *A = &choiceArrayDescriptor;

                  bdlmxxx::ConstElemRef cer1(d1, A);
            const bdlmxxx::ConstElemRef CER1(d1, A);

                  bdlmxxx::ConstElemRef cer2(d2, A);
            const bdlmxxx::ConstElemRef CER2(d2, A);

                  bdlmxxx::ElemRef er1(d1, A);
            const bdlmxxx::ElemRef ER1(d1, A);

                  bdlmxxx::ElemRef er2(d2, A);
            const bdlmxxx::ElemRef ER2(d2, A);

            typedef bdlmxxx::ChoiceArray Type; Type *rp; const Type *crp;
            crp = &cer1.theChoiceArray();         ASSERT(D1 == crp);
            crp = &CER1.theChoiceArray();         ASSERT(D1 == crp);
            crp = &cer2.theChoiceArray();         ASSERT(D2 == crp);
            crp = &CER2.theChoiceArray();         ASSERT(D2 == crp);

             rp = &er1.theModifiableChoiceArray();  ASSERT(D1 ==  rp);
            crp = &ER1.theChoiceArray();            ASSERT(D1 == crp);
             rp = &er2.theModifiableChoiceArray();  ASSERT(D2 ==  rp);
            crp = &ER2.theChoiceArray();            ASSERT(D2 == crp);
        }
        ASSERT(32 == tIdx);  // ensure exactly 32 blocks

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY CONSTRUCTORS & BASIC ACCESSORS  (THERE ARE NO MANIPULATORS.)
        //   This case verifies that the methods designated as being able to
        //   bring a 'bdlmxxx::ConstElemRef' or 'bdlmxxx::ElemRef' into any
        //   attainable state behave as expected.
        //
        // Primary constructors:
        //   - bdlmxxx::ConstElemRef::bdlmxxx::ConstElemRef(data, descriptor);
        //   - bdlmxxx::ConstElemRef::bdlmxxx::ConstElemRef(data, descriptor,
        //                                          bits, offset);
        //   - bdlmxxx::ElemRef::bdlmxxx::ElemRef(data, descriptor);
        //   - bdlmxxx::ElemRef::bdlmxxx::ElemRef(data, descriptor, bits, offset);
        //
        // Primary Manipulators: (none)
        //
        // Basic Accessors:
        //   - bdlmxxx::ConstElemRef::descriptor() const
        //   - bdlmxxx::ConstElemRef::data() const
        //   - bdlmxxx::ElemRef::data() const
        //   - bdlmxxx::ElemRef::isNullable() const
        //
        // Concerns:
        //   - That the 'data' and 'descriptor' values are installed properly.
        //   - That the 'data' and 'descriptor' values are recalled properly.
        //   - That all base-class methods also work with a derived object.
        //   - That const accessors work on both const and non-const objects.
        //   - That only "const values" are returned from 'const' objects.
        //   - That 'data' returned from a 'bdlmxxx::ElemRef' is non-const.
        //   - That isNullable returns correctly for nullable and non-nullable
        //     object.
        //
        // Plan:
        //   - Create instances of both types with at least two different
        //     data and descriptor values.
        //   - Verify, using all basic accessors, that the data and descriptor
        //     pointers were installed properly.
        //
        // Tactics:
        //   - Ad-Hoc test data selection method
        //   - Brute Force implementation technique
        //
        // Testing:
        //                       bdlmxxx::ConstElemRef
        //                       ~~~~~~~~~~~~~~~~~
        //   bdlmxxx::ConstElemRef(const void *data, const bdlmxxx::Descriptor *desc);
        //   bdlmxxx::ConstElemRef(const void            *data,
        //                     const bdlmxxx::Descriptor *desc,
        //                     const int             *nullnessWord,
        //                           int              nullnessBitOffset);
        //   ~bdlmxxx::ConstElemRef();
        //
        //   const bdlmxxx::Descriptor *descriptor() const;
        //   const void *data() const;
        //         bool isNullable() const;
        //
        //                       bdlmxxx::ElemRef
        //                       ~~~~~~~~~~~~
        //   bdlmxxx::ElemRef(void *data, const bdlmxxx::Descriptor *desc);
        //   bdlmxxx::ElemRef(void *data,
        //                const bdlmxxx::Descriptor *desc,
        //                const int             *nullnessWord,
        //                      int              nullnessBitOffset);
        //   ~bdlmxxx::ElemRef();
        //
        //   void *data() const;
        //   bool isNullable() const
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY CONSTRUCTORS & BASIC ACCESSORS" << endl
                          << "======================================" << endl;

        typedef bdlmxxx::Descriptor Attr;

        char    d1 = '1';  Attr& a1 = *const_cast<Attr *>(&charDescriptor);
        short   d2 = 2;    Attr& a2 = *const_cast<Attr *>(&shortDescriptor);
        int     d3 = 3;    Attr& a3 = *const_cast<Attr *>(&intDescriptor);
        double  d4 = 4;    Attr& a4 = *const_cast<Attr *>(&doubleDescriptor);

        const char&   D1 = d1;   const Attr& A1 = a1;
        const short&  D2 = d2;   const Attr& A2 = a2;
        const int&    D3 = d3;
        const double& D4 = d4;

        if (veryVerbose) { P_(D1) P_(D2) P_(D3) P(D4) };

        if (verbose) cout <<
            "\nVerify data and descriptor are installed properly." << endl;

        if (verbose) cout <<
            "\tData and descriptor values are correct." << endl;
        // type char
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            typedef char Type;
                  Type& d = d1;       Attr& a = a1;
            const Type& D = d;  const Attr& A = a;

                  bdlmxxx::ConstElemRef  cer(&D, &A);
            const bdlmxxx::ConstElemRef& CER = cer;
            ASSERT(&D == cer.data());
            ASSERT(&D == CER.data());
            ASSERT(&A == cer.descriptor());
            ASSERT(&A == CER.descriptor());
            ASSERT( 0 == cer.isNull());
            ASSERT( 0 == CER.isNull());
            ASSERT( 0 == cer.isNullable());
            ASSERT( 0 == CER.isNullable());

                  bdlmxxx::ElemRef  er(&d, &A);
            const bdlmxxx::ElemRef& ER = er;
            ASSERT(&D == er.data());
            ASSERT(&D == ER.data());
            ASSERT(&A == er.descriptor());
            ASSERT(&A == ER.descriptor());
            ASSERT( 0 == er.isNull());
            ASSERT( 0 == ER.isNull());
            ASSERT( 0 == er.isNullable());
            ASSERT( 0 == ER.isNullable());

            if (veryVerbose) { P_(CER) P(ER) }
        }
        {
            typedef char Type;
                  Type& d = d1;       Attr& a = a1;
            const Type& D = d;  const Attr& A = a;

            int nullBits = 0;

                  bdlmxxx::ConstElemRef  cer(&D, &A, &nullBits, 3);
            const bdlmxxx::ConstElemRef& CER = cer;
            ASSERT(&D == cer.data());
            ASSERT(&D == CER.data());
            ASSERT(&A == cer.descriptor());
            ASSERT(&A == CER.descriptor());
            ASSERT( 0 == cer.isNull());
            ASSERT( 0 == CER.isNull());
            ASSERT( 1 == cer.isNullable());
            ASSERT( 1 == CER.isNullable());

                  bdlmxxx::ElemRef  er(&d, &A, &nullBits, 3);
            const bdlmxxx::ElemRef& ER = er;
            ASSERT(&D == er.data());
            ASSERT(&D == ER.data());
            ASSERT(&A == er.descriptor());
            ASSERT(&A == ER.descriptor());
            ASSERT( 0 == er.isNull());
            ASSERT( 0 == ER.isNull());
            ASSERT( 1 == er.isNullable());
            ASSERT( 1 == ER.isNullable());

            er.makeNull();
            ASSERT(8 == nullBits);

            if (veryVerbose) { P_(CER) P(ER) }
        }
        {
            typedef char Type;
                  Type  d = '\0';     Attr& a = a1;
            const Type& D = d;  const Attr& A = a;

            int nullBits = 8;
            a.makeUnset(&d);

                  bdlmxxx::ConstElemRef  cer(&D, &A, &nullBits, 3);
            const bdlmxxx::ConstElemRef& CER = cer;
            ASSERT(&D == cer.data());
            ASSERT(&D == CER.data());
            ASSERT(&A == cer.descriptor());
            ASSERT(&A == CER.descriptor());
            ASSERT( 1 == cer.isNull());
            ASSERT( 1 == CER.isNull());
            ASSERT( 1 == cer.isNullable());
            ASSERT( 1 == CER.isNullable());

                  bdlmxxx::ElemRef  er(&d, &A, &nullBits, 3);
            const bdlmxxx::ElemRef& ER = er;
            ASSERT(&D == er.dataRaw());
            ASSERT(&D == ER.dataRaw());
            ASSERT(&A == er.descriptor());
            ASSERT(&A == ER.descriptor());
            ASSERT( 1 == er.isNull());
            ASSERT( 1 == ER.isNull());
            ASSERT( 1 == er.isNullable());
            ASSERT( 1 == ER.isNullable());

            er.makeNull();
            ASSERT(0x8 == nullBits);

            if (veryVerbose) { P_(CER) P(ER) }
        }
        // type short
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            typedef short Type;
                  Type& d = d2;       Attr& a = a2;
            const Type& D = d;  const Attr& A = a;

                  bdlmxxx::ConstElemRef  cer(&D, &A);
            const bdlmxxx::ConstElemRef& CER = cer;
            ASSERT(&D == cer.data());
            ASSERT(&D == CER.data());
            ASSERT(&A == cer.descriptor());
            ASSERT(&A == CER.descriptor());
            ASSERT( 0 == cer.isNull());
            ASSERT( 0 == CER.isNull());
            ASSERT( 0 == cer.isNullable());
            ASSERT( 0 == CER.isNullable());

                  bdlmxxx::ElemRef  er(&d, &A);
            const bdlmxxx::ElemRef& ER = er;
            ASSERT(&D == er.data());
            ASSERT(&D == ER.data());
            ASSERT(&A == er.descriptor());
            ASSERT(&A == ER.descriptor());
            ASSERT( 0 == er.isNull());
            ASSERT( 0 == ER.isNull());
            ASSERT( 0 == er.isNullable());
            ASSERT( 0 == ER.isNullable());

            if (veryVerbose) { P_(CER) P(ER) }
        }
        {
            typedef short Type;
                  Type& d = d2;       Attr& a = a2;
            const Type& D = d;  const Attr& A = a;

            int nullBits = 0;

                  bdlmxxx::ConstElemRef  cer(&D, &A, &nullBits, 3);
            const bdlmxxx::ConstElemRef& CER = cer;
            ASSERT(&D == cer.data());
            ASSERT(&D == CER.data());
            ASSERT(&A == cer.descriptor());
            ASSERT(&A == CER.descriptor());
            ASSERT( 0 == cer.isNull());
            ASSERT( 0 == CER.isNull());
            ASSERT( 1 == cer.isNullable());
            ASSERT( 1 == CER.isNullable());

                  bdlmxxx::ElemRef  er(&d, &A, &nullBits, 3);
            const bdlmxxx::ElemRef& ER = er;
            ASSERT(&D == er.data());
            ASSERT(&D == ER.data());
            ASSERT(&A == er.descriptor());
            ASSERT(&A == ER.descriptor());
            ASSERT( 0 == er.isNull());
            ASSERT( 0 == ER.isNull());
            ASSERT( 1 == er.isNullable());
            ASSERT( 1 == ER.isNullable());

            er.makeNull();
            ASSERT(8 == nullBits);

            if (veryVerbose) { P_(CER) P(ER) }
        }
        {
            typedef short Type;
                  Type  d = 0;        Attr& a = a2;
            const Type& D = d;  const Attr& A = a;

            int nullBits = 8;
            a.makeUnset(&d);

                  bdlmxxx::ConstElemRef  cer(&D, &A, &nullBits, 3);
            const bdlmxxx::ConstElemRef& CER = cer;
            ASSERT(&D == cer.data());
            ASSERT(&D == CER.data());
            ASSERT(&A == cer.descriptor());
            ASSERT(&A == CER.descriptor());
            ASSERT( 1 == cer.isNull());
            ASSERT( 1 == CER.isNull());
            ASSERT( 1 == cer.isNullable());
            ASSERT( 1 == CER.isNullable());

                  bdlmxxx::ElemRef  er(&d, &A, &nullBits, 3);
            const bdlmxxx::ElemRef& ER = er;
            ASSERT(&D == er.dataRaw());
            ASSERT(&D == ER.dataRaw());
            ASSERT(&A == er.descriptor());
            ASSERT(&A == ER.descriptor());
            ASSERT( 1 == er.isNull());
            ASSERT( 1 == ER.isNull());
            ASSERT( 1 == er.isNullable());
            ASSERT( 1 == ER.isNullable());

            er.makeNull();
            ASSERT(0x8 == nullBits);

            if (veryVerbose) { P_(CER) P(ER) }
        }
        // type int
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            typedef int Type;
                  Type& d = d3;       Attr& a = a3;
            const Type& D = d;  const Attr& A = a;

                  bdlmxxx::ConstElemRef  cer(&D, &A);
            const bdlmxxx::ConstElemRef& CER = cer;
            ASSERT(&D == cer.data());
            ASSERT(&D == CER.data());
            ASSERT(&A == cer.descriptor());
            ASSERT(&A == CER.descriptor());
            ASSERT( 0 == cer.isNull());
            ASSERT( 0 == CER.isNull());
            ASSERT( 0 == cer.isNullable());
            ASSERT( 0 == CER.isNullable());

                  bdlmxxx::ElemRef  er(&d, &A);
            const bdlmxxx::ElemRef& ER = er;
            ASSERT(&D == er.data());
            ASSERT(&D == ER.data());
            ASSERT(&A == er.descriptor());
            ASSERT(&A == ER.descriptor());
            ASSERT( 0 == er.isNull());
            ASSERT( 0 == ER.isNull());
            ASSERT( 0 == er.isNullable());
            ASSERT( 0 == ER.isNullable());

            if (veryVerbose) { P_(CER) P(ER) }
        }
        {
            typedef int Type;
                  Type& d = d3;       Attr& a = a3;
            const Type& D = d;  const Attr& A = a;

            int nullBits = 0;

                  bdlmxxx::ConstElemRef  cer(&D, &A, &nullBits, 3);
            const bdlmxxx::ConstElemRef& CER = cer;
            ASSERT(&D == cer.data());
            ASSERT(&D == CER.data());
            ASSERT(&A == cer.descriptor());
            ASSERT(&A == CER.descriptor());
            ASSERT( 0 == cer.isNull());
            ASSERT( 0 == CER.isNull());
            ASSERT( 1 == cer.isNullable());
            ASSERT( 1 == CER.isNullable());

                  bdlmxxx::ElemRef  er(&d, &A, &nullBits, 3);
            const bdlmxxx::ElemRef& ER = er;
            ASSERT(&D == er.data());
            ASSERT(&D == ER.data());
            ASSERT(&A == er.descriptor());
            ASSERT(&A == ER.descriptor());
            ASSERT( 0 == er.isNull());
            ASSERT( 0 == ER.isNull());
            ASSERT( 1 == er.isNullable());
            ASSERT( 1 == ER.isNullable());

            er.makeNull();
            ASSERT(8 == nullBits);

            if (veryVerbose) { P_(CER) P(ER) }
        }
        {
            typedef int Type;
                  Type  d = 0;     Attr& a = a3;
            const Type& D = d;  const Attr& A = a;

            int nullBits = 8;
            a.makeUnset(&d);

                  bdlmxxx::ConstElemRef  cer(&D, &A, &nullBits, 3);
            const bdlmxxx::ConstElemRef& CER = cer;
            ASSERT(&D == cer.data());
            ASSERT(&D == CER.data());
            ASSERT(&A == cer.descriptor());
            ASSERT(&A == CER.descriptor());
            ASSERT( 1 == cer.isNull());
            ASSERT( 1 == CER.isNull());
            ASSERT( 1 == cer.isNullable());
            ASSERT( 1 == CER.isNullable());

                  bdlmxxx::ElemRef  er(&d, &A, &nullBits, 3);
            const bdlmxxx::ElemRef& ER = er;
            ASSERT(&D == er.dataRaw());
            ASSERT(&D == ER.dataRaw());
            ASSERT(&A == er.descriptor());
            ASSERT(&A == ER.descriptor());
            ASSERT( 1 == er.isNull());
            ASSERT( 1 == ER.isNull());
            ASSERT( 1 == er.isNullable());
            ASSERT( 1 == ER.isNullable());

            er.makeNull();
            ASSERT(0x8 == nullBits);

            if (veryVerbose) { P_(CER) P(ER) }
        }
        // type double
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            typedef double Type;
                  Type& d = d4;       Attr& a = a4;
            const Type& D = d;  const Attr& A = a;

                  bdlmxxx::ConstElemRef  cer(&D, &A);
            const bdlmxxx::ConstElemRef& CER = cer;
            ASSERT(&D == cer.data());
            ASSERT(&D == CER.data());
            ASSERT(&A == cer.descriptor());
            ASSERT(&A == CER.descriptor());
            ASSERT( 0 == cer.isNull());
            ASSERT( 0 == CER.isNull());
            ASSERT( 0 == cer.isNullable());
            ASSERT( 0 == CER.isNullable());

                  bdlmxxx::ElemRef  er(&d, &A);
            const bdlmxxx::ElemRef& ER = er;
            ASSERT(&D == er.data());
            ASSERT(&D == ER.data());
            ASSERT(&A == er.descriptor());
            ASSERT(&A == ER.descriptor());
            ASSERT( 0 == er.isNull());
            ASSERT( 0 == ER.isNull());
            ASSERT( 0 == er.isNullable());
            ASSERT( 0 == ER.isNullable());

            if (veryVerbose) { P_(CER) P(ER) }
        }
        {
            typedef double Type;
                  Type& d = d4;       Attr& a = a4;
            const Type& D = d;  const Attr& A = a;

            int nullBits = 0;

                  bdlmxxx::ConstElemRef  cer(&D, &A, &nullBits, 3);
            const bdlmxxx::ConstElemRef& CER = cer;
            ASSERT(&D == cer.data());
            ASSERT(&D == CER.data());
            ASSERT(&A == cer.descriptor());
            ASSERT(&A == CER.descriptor());
            ASSERT( 0 == cer.isNull());
            ASSERT( 0 == CER.isNull());
            ASSERT( 1 == cer.isNullable());
            ASSERT( 1 == CER.isNullable());

                  bdlmxxx::ElemRef  er(&d, &A, &nullBits, 3);
            const bdlmxxx::ElemRef& ER = er;
            ASSERT(&D == er.data());
            ASSERT(&D == ER.data());
            ASSERT(&A == er.descriptor());
            ASSERT(&A == ER.descriptor());
            ASSERT( 0 == er.isNull());
            ASSERT( 0 == ER.isNull());
            ASSERT( 1 == er.isNullable());
            ASSERT( 1 == ER.isNullable());

            er.makeNull();
            ASSERT(8 == nullBits);

            if (veryVerbose) { P_(CER) P(ER) }
        }
        {
            typedef double Type;
                  Type  d = 0;     Attr& a = a4;
            const Type& D = d;  const Attr& A = a;

            int nullBits = 8;
            a.makeUnset(&d);

                  bdlmxxx::ConstElemRef  cer(&D, &A, &nullBits, 3);
            const bdlmxxx::ConstElemRef& CER = cer;
            ASSERT(&D == cer.data());
            ASSERT(&D == CER.data());
            ASSERT(&A == cer.descriptor());
            ASSERT(&A == CER.descriptor());
            ASSERT( 1 == cer.isNull());
            ASSERT( 1 == CER.isNull());
            ASSERT( 1 == cer.isNullable());
            ASSERT( 1 == CER.isNullable());

                  bdlmxxx::ElemRef  er(&d, &A, &nullBits, 3);
            const bdlmxxx::ElemRef& ER = er;
            ASSERT(&D == er.dataRaw());
            ASSERT(&D == ER.dataRaw());
            ASSERT(&A == er.descriptor());
            ASSERT(&A == ER.descriptor());
            ASSERT( 1 == er.isNull());
            ASSERT( 1 == ER.isNull());
            ASSERT( 1 == er.isNullable());
            ASSERT( 1 == ER.isNullable());

            er.makeNull();
            ASSERT(0x8 == nullBits);

            if (veryVerbose) { P_(CER) P(ER) }
        }

        if (verbose) cout <<
            "\tInput types can be 'const' or non-'const'." << endl;
        {
                  bdlmxxx::ConstElemRef cer(&d1, &a1);
            const bdlmxxx::ConstElemRef CER(&d1, &a1);

                  bdlmxxx::ElemRef er(&d1, &a1);
            const bdlmxxx::ElemRef ER(&d1, &a1);

            if (veryVerbose) { P_(cer) P_(CER) P_(er) P(ER) }
        }
        {
                  bdlmxxx::ConstElemRef cer(&d1, &A1);
            const bdlmxxx::ConstElemRef CER(&d1, &A1);

                  bdlmxxx::ElemRef er(&d1, &A1);
            const bdlmxxx::ElemRef ER(&d1, &A1);

            if (veryVerbose) { P_(cer) P_(CER) P_(er) P(ER) }
        }
        {
                  bdlmxxx::ConstElemRef cer(&D1, &a1);
            const bdlmxxx::ConstElemRef CER(&D1, &a1);

            //X         bdlmxxx::ElemRef er(&D1, &a1);        // should not compile
            //X   const bdlmxxx::ElemRef ER(&D1, &a1);        // should not compile

            if (veryVerbose) { P_(cer) P(CER) }
        }
        {
                  bdlmxxx::ConstElemRef cer(&D1, &A1);
            const bdlmxxx::ConstElemRef CER(&D1, &A1);

            //X         bdlmxxx::ElemRef er(&D1, &a1);        // should not compile
            //X   const bdlmxxx::ElemRef ER(&D1, &a1);        // should not compile

            if (veryVerbose) { P_(cer) P(CER) }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
            "\nVerify data and descriptor are recalled properly." << endl;

        if (verbose) cout <<
            "\tData and descriptor values are correct." << endl;
        {
            // Note that we already thoroughly demonstrated this above.
        }

        if (verbose) cout <<
            "\tReturn types are properly 'const'/non-'const'." << endl;

        {
            void            *vp = 0;         const void            *cvp = 0;
            bdlmxxx::Descriptor *dp = 0;         const bdlmxxx::Descriptor *cdp = 0;
            {
                      bdlmxxx::ConstElemRef cer(&D1, &A1);
                const bdlmxxx::ConstElemRef& CER = cer;

                //X      vp = cer.data();             // should not compile
                cvp = cer.data();
                //X      vp = CER.data();             // should not compile
                cvp = CER.data();

                //X      dp = cer.descriptor();       // should not compile
                cdp = cer.descriptor();
                //X      dp = CER.descriptor();       // should not compile
                cdp = CER.descriptor();

                ASSERT(&D1 == cvp);        ASSERT(&A1 == cdp);
                ASSERT(  0 ==  vp);        ASSERT(  0 ==  dp);
           }
           {
                     bdlmxxx::ElemRef er(&d2, &A2);
               const bdlmxxx::ElemRef& ER = er;

               vp  = er.data();
               cvp = er.data();
               vp  = ER.data();
               cvp = ER.data();

               //X  dp = er.descriptor();                // should not compile
               cdp = er.descriptor();
               //X  dp = ER.descriptor();                // should not compile
               cdp = ER.descriptor();

               ASSERT(&D2 == cvp);        ASSERT(&A2 == cdp);
               ASSERT(&D2 ==  vp);        ASSERT(  0 ==  dp);
           }
        }

#ifdef CORE_DUMP_GCC_COMPILER
        // Note: this block of code core-dumps the g++ compiler under Cygwin
        "\n'data' and 'descriptor' values are installed properly."
        "\n'data' and 'descriptor' values are recalled properly"
        "\nThat all base-class methods also work with a derived object."
        "\nThat const accessors work on both const and non-const objects."
        "\nThat only "const values" are returned from 'const' objects."
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case is provided to facilitate development.  Note that this
        //   breathing test exercises basic functionality, but tests nothing.
        //   Note also that testing for 'const'-correctness of copy
        //   construction and assuring compile-time errors for re-binding
        //   (assignment) are negative tests that currently must be performed
        //   manually by selectively uncommenting code.
        //
        // Concerns:
        //   We are concerned with developing a fairly new kind of type.
        //
        // Plan:
        //   Do what ever is necessary to bring this component to a level
        //   where it can be tested thoroughly.
        //
        // Tactics:
        //   - Ad-Hoc test data selection method
        //   - Brute-Force and Loop-Based implementation techniques
        //
        // Testing:
        //   BREATHING TEST
        //   CONST CORRECTNESS BETWEEN CLASSES (NEGATIVE)
        //   ATTEMPTED RE-BINDING =) "COMPILE" (NOT "LINK") ERROR (NEGATIVE)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        typedef bdlmxxx::Descriptor Attr;

        int         d0 = INT_MIN;    const Attr *A0 = &intDescriptor;
        double      d1 = 2.25;       const Attr *A1 = &doubleDescriptor;
        bsl::string d2 = "corndog";  const Attr *A2 = &stringDescriptor;

        if (verbose) cout << "\nPrinting values" << endl;
        if (verbose) { T_ P_(d0) P_(d1) P(d2) };

        bdlmxxx::ConstElemRef cer0(&d0, A0);
        bdlmxxx::ConstElemRef cer1(&d1, A1);
        bdlmxxx::ConstElemRef cer2(&d2, A2);

        if (verbose) {
            T_ P_(cer0) P_(cer1); P(cer2);
        }

        const bdlmxxx::ConstElemRef CER0(&d0, A0);
        const bdlmxxx::ConstElemRef CER1(&d1, A1);
        const bdlmxxx::ConstElemRef CER2(&d2, A2);

        if (verbose) {
            T_ P_(CER0) P_(CER1); P(CER2);
        }

        bdlmxxx::ElemRef er0(&d0, A0);
        bdlmxxx::ElemRef er1(&d1, A1);
        bdlmxxx::ElemRef er2(&d2, A2);

        if (verbose) {
            T_ P_(er0) P_(er1) P(er2);
        }

        const bdlmxxx::ElemRef ER0(&d0, A0);
        const bdlmxxx::ElemRef ER1(&d1, A1);
        const bdlmxxx::ElemRef ER2(&d2, A2);

        if (verbose) {
            T_ P_(ER0) P_(ER1) P(ER2);
        }

        // nullable ElemRefs
        int         nullBits  = 1;
        int         nd0       = INT_MIN;
        double      nd1       = 2.25;
        bsl::string nd2       = "corndog";

        if (verbose) { T_ P_(nd0) P_(nd1) P(nd2) };

        bdlmxxx::ConstElemRef ncer0(&nd0, A0, &nullBits, 0);
        bdlmxxx::ConstElemRef ncer1(&nd1, A1, &nullBits, 1);
        bdlmxxx::ConstElemRef ncer2(&nd2, A2, &nullBits, 2);

        if (verbose) {
            T_ P_(ncer0) P_(ncer1); P(ncer2);
        }

        const bdlmxxx::ConstElemRef NCER0(&nd0, A0, &nullBits, 0);
        const bdlmxxx::ConstElemRef NCER1(&nd1, A1, &nullBits, 1);
        const bdlmxxx::ConstElemRef NCER2(&nd2, A2, &nullBits, 2);

        if (verbose) {
            T_ P_(NCER0) P_(NCER1); P(NCER2);
        }

        bdlmxxx::ElemRef ner0(&nd0, A0, &nullBits, 0);
        bdlmxxx::ElemRef ner1(&nd1, A1, &nullBits, 1);
        bdlmxxx::ElemRef ner2(&nd2, A2, &nullBits, 2);

        if (verbose) {
            T_ P_(ner0) P_(ner1) P(ner2);
        }

        const bdlmxxx::ElemRef NER0(&nd0, A0, &nullBits, 0);
        const bdlmxxx::ElemRef NER1(&nd1, A1, &nullBits, 1);
        const bdlmxxx::ElemRef NER2(&nd2, A2, &nullBits, 2);

        if (verbose) {
            T_ P_(NER0) P_(NER1) P(NER2);
        }

        if (verbose) cout << "\nisUnset" << endl;

        ASSERT(1 == isUnset(CER0));
        ASSERT(0 == isUnset(CER1));
        ASSERT(0 == isUnset(CER2));

        ASSERT(1 == isUnset(NCER0));
        ASSERT(0 == isUnset(NCER1));
        ASSERT(0 == isUnset(NCER2));

        if (verbose) cout << "\nisNullable" << endl;

        ASSERT(0 == CER0.isNullable());
        ASSERT(0 == CER1.isNullable());
        ASSERT(0 == CER2.isNullable());

        ASSERT(1 == NCER0.isNullable());
        ASSERT(1 == NCER1.isNullable());
        ASSERT(1 == NCER2.isNullable());

        if (verbose) cout << "\nisNull/isNonNull" << endl;

        ASSERT(0 == CER0.isNull());         ASSERT(1 == CER0.isNonNull());
        ASSERT(0 == CER1.isNull());         ASSERT(1 == CER1.isNonNull());
        ASSERT(0 == CER2.isNull());         ASSERT(1 == CER2.isNonNull());

        ASSERT(1 == NCER0.isNull());        ASSERT(0 == NCER0.isNonNull());
        ASSERT(0 == NCER1.isNull());        ASSERT(1 == NCER1.isNonNull());
        ASSERT(0 == NCER2.isNull());        ASSERT(1 == NCER2.isNonNull());

        if (verbose) cout << "\ngetting/setting individual values" << endl;

        ASSERT(INT_MIN ==  CER0.theInt());
        ASSERT(INT_MIN ==   ER0.theInt());
        ASSERT(INT_MIN == NCER0.theInt());
        ASSERT(INT_MIN ==  NER0.theInt());

        ASSERT(2.25 ==  CER1.theDouble());
        ASSERT(2.25 ==   ER1.theDouble());
        ASSERT(2.25 == NCER1.theDouble());
        ASSERT(2.25 ==  NER1.theDouble());

        ASSERT("corndog" ==  CER2.theString());
        ASSERT("corndog" ==   ER2.theString());
        ASSERT("corndog" == NCER2.theString());
        ASSERT("corndog" ==  NER2.theString());

        ER0.theModifiableInt()    = 127;

        ASSERT(127 ==  CER0.theInt());
        ASSERT(127 ==   ER0.theInt());

        ER1.theModifiableDouble() = 1.5;

        ASSERT(1.5 ==  CER1.theDouble());
        ASSERT(1.5 ==   ER1.theDouble());

        ER2.theModifiableString() = "bozo";

        ASSERT("bozo" ==  CER2.theString());
        ASSERT("bozo" ==   ER2.theString());

        NER0.theModifiableInt() = 130;

        ASSERT(130 == NCER0.theInt());
        ASSERT(130 ==  NER0.theInt());
        ASSERT(0   == NCER0.isNull());      ASSERT(1 == NCER0.isNonNull());

        NER1.theModifiableDouble() = 2.0;

        ASSERT(2.0 == NCER1.theDouble());
        ASSERT(2.0 ==  NER1.theDouble());

        NER2.theModifiableString() = "clown";

        ASSERT("clown" == NCER2.theString());
        ASSERT("clown" ==  NER2.theString());
#if 1
        // this section is to be deprecated
        ER0.theModifiableInt()    = 127;
        ER1.theModifiableDouble() = 1.5;
        ER2.theModifiableString() = "beeblebrox";

        ASSERT(127 == CER0.theInt());
        ASSERT(127 ==  ER0.theInt());

        ASSERT(1.5 == CER1.theDouble());
        ASSERT(1.5 ==  ER1.theDouble());

        ASSERT("beeblebrox" == CER2.theString());
        ASSERT("beeblebrox" ==  ER2.theString());
#endif

        ER0.theModifiableInt()    = 123;
        ER1.theModifiableDouble() = 0.5;
        ER2.theModifiableString() = "bozo";

        ASSERT(123 == CER0.theInt());
        ASSERT(123 ==  ER0.theInt());

        ASSERT(0.5 == CER1.theDouble());
        ASSERT(0.5 ==  ER1.theDouble());

        ASSERT("bozo" == CER2.theString());
        ASSERT("bozo" ==  ER2.theString());

        if (verbose) cout << "\nCopy construction" << endl;

        ASSERT(&d0 == CER0.data());
        ASSERT(&d1 == CER1.data());
        ASSERT(&d2 == CER2.data());

        const bdlmxxx::ConstElemRef CER0_COPY_FROM_CONST(CER0);
        const bdlmxxx::ConstElemRef CER1_COPY_FROM_CONST(CER1);
        const bdlmxxx::ConstElemRef CER2_COPY_FROM_CONST(CER2);

        ASSERT(&d0 == CER0_COPY_FROM_CONST.data());
        ASSERT(&d1 == CER1_COPY_FROM_CONST.data());
        ASSERT(&d2 == CER2_COPY_FROM_CONST.data());

        const bdlmxxx::ConstElemRef CER0_COPY(ER0);
        const bdlmxxx::ConstElemRef CER1_COPY(ER1);
        const bdlmxxx::ConstElemRef CER2_COPY(ER2);

        ASSERT(&d0 == CER0_COPY.data());
        ASSERT(&d1 == CER1_COPY.data());
        ASSERT(&d2 == CER2_COPY.data());

//#define THIS_SHOULD_NOT_COMPILE
#ifdef THIS_SHOULD_NOT_COMPILE
        const bdlmxxx::ElemRef ER0_COPY_FROM_CONST(CER0);
        const bdlmxxx::ElemRef ER1_COPY_FROM_CONST(CER1);
        const bdlmxxx::ElemRef ER2_COPY_FROM_CONST(CER2);
#endif

        const bdlmxxx::ElemRef ER0_COPY(ER0);
        const bdlmxxx::ElemRef ER1_COPY(ER1);
        const bdlmxxx::ElemRef ER2_COPY(ER2);

        ASSERT(&d0 == ER0_COPY.data());
        ASSERT(&d1 == ER1_COPY.data());
        ASSERT(&d2 == ER2_COPY.data());

        {
            if (verbose) cout << "\nUnbound References" << endl;

            ASSERT(1 == er0.isBound());
            ASSERT(1 == cer1.isBound());
            ASSERT(1 == ER1.isBound());
            ASSERT(1 == CER2.isBound());

                  bdlmxxx::ElemRef       er = bdlmxxx::ElemRef::unboundElemRef();
                  bdlmxxx::ConstElemRef cer = bdlmxxx::ElemRef::unboundElemRef();
            const bdlmxxx::ElemRef       ER = bdlmxxx::ElemRef::unboundElemRef();
            const bdlmxxx::ConstElemRef CER = bdlmxxx::ElemRef::unboundElemRef();

            ASSERT(0 == er.isBound());
            ASSERT(0 == cer.isBound());
            ASSERT(0 == ER.isBound());
            ASSERT(0 == CER.isBound());

            if (verbose) cout << "\nAssignment (Negative Test)" << endl;

            // MANUAL NEGATIVE TEST:
            // (None of the following if uncommented should compile.)

//#define THIS_SHOULD_NOT_COMPILE
#ifdef THIS_SHOULD_NOT_COMPILE
            er = er;         // compile-time error - private
            er = cer;        // compile-time error - private
            er = ER;         // compile-time error - private
            er = CER;        // compile-time error - private

            cer = er;        // compile-time error - private
            cer = cer;       // compile-time error - private
            cer = ER;        // compile-time error - private
            cer = CER;       // compile-time error - private

            ER = er;         // compile-time error - private & discards qual
            ER = cer;        // compile-time error - private & discards qual
            ER = ER;         // compile-time error - private & discards qual
            ER = CER;        // compile-time error - private & discards qual

            CER = er;        // compile-time error - private & discards qual
            CER = cer;       // compile-time error - private & discards qual
            CER = ER;        // compile-time error - private & discards qual
            CER = CER;       // compile-time error - private & discards qual
#endif
        }

        if (veryVerbose) { T_ P_(CER0) P_(CER1); P(CER2); }

        ASSERT(123 == CER0.theInt());
        ASSERT(123 ==  ER0.theInt());

        ASSERT(0.5 == CER1.theDouble());
        ASSERT(0.5 ==  ER1.theDouble());

        ASSERT("bozo" == CER2.theString());
        ASSERT("bozo" ==  ER2.theString());

        if (verbose) cout << "\nmakeNull" << endl;

        ASSERT(0 == isUnset(ER0));
        ASSERT(0 == isUnset(ER1));
        ASSERT(0 == isUnset(ER2));

        ASSERT(1 == ER0.isNonNull());
        ASSERT(1 == ER1.isNonNull());
        ASSERT(1 == ER2.isNonNull());

        ER0.makeNull();
        ER1.makeNull();
        ER2.makeNull();

        ASSERT(1 == isUnset(ER0));
        ASSERT(1 == isUnset(ER1));
        ASSERT(1 == isUnset(ER2));

        ASSERT(0 == ER0.isNull());
        ASSERT(0 == ER1.isNull());
        ASSERT(0 == ER2.isNull());

        if (veryVerbose) { T_ P_(CER0) P_(CER1); P(CER2); }

        if (verbose) cout << "\ntype" << endl;

        ASSERT(bdlmxxx::ElemType::BDEM_INT    == CER0.type());
        ASSERT(bdlmxxx::ElemType::BDEM_INT    ==  ER0.type());

        ASSERT(bdlmxxx::ElemType::BDEM_DOUBLE == CER1.type());
        ASSERT(bdlmxxx::ElemType::BDEM_DOUBLE ==  ER1.type());

        ASSERT(bdlmxxx::ElemType::BDEM_STRING == CER2.type());
        ASSERT(bdlmxxx::ElemType::BDEM_STRING ==  ER2.type());

        if (verbose) cout << "\nmakeNull" << endl;

        ASSERT(1 == NER0.isNonNull());
        ASSERT(1 == NER1.isNonNull());
        ASSERT(1 == NER2.isNonNull());

        ASSERT(0 == isUnset(NER0));
        ASSERT(0 == isUnset(NER1));
        ASSERT(0 == isUnset(NER2));

        NER0.makeNull();
        NER1.makeNull();
        NER2.makeNull();

        ASSERT(1 == NER0.isNull());
        ASSERT(1 == NER1.isNull());
        ASSERT(1 == NER2.isNull());

        ASSERT(1 == isUnset(NER0));
        ASSERT(1 == isUnset(NER1));
        ASSERT(1 == isUnset(NER2));

        if (verbose) cout << "\nVerify dummy descriptors work" << endl;
        {
            static void *D[22] = {
                (void *)900, (void *)901, (void *)902, (void *)903,
                (void *)904, (void *)905, (void *)906, (void *)907,
                (void *)908, (void *)909, (void *)910, (void *)911,
                (void *)912, (void *)913, (void *)914, (void *)915,
                (void *)916, (void *)917, (void *)918, (void *)919,
                (void *)920, (void *)921
            };

            static const bdlmxxx::Descriptor *A[22] = {
                &charDescriptor,                &shortDescriptor,
                &intDescriptor,                 &int64Descriptor,
                &floatDescriptor,               &doubleDescriptor,
                &stringDescriptor,              &datetimeDescriptor,
                &dateDescriptor,                &timeDescriptor,
                &charArrayDescriptor,           &shortArrayDescriptor,
                &intArrayDescriptor,            &int64ArrayDescriptor,
                &floatArrayDescriptor,          &doubleArrayDescriptor,
                &stringArrayDescriptor,         &datetimeArrayDescriptor,
                &dateArrayDescriptor,           &timeArrayDescriptor,
                &listDescriptor,                &tableDescriptor
            };

            for (int i = 0; i < 22; ++i) {
                if (veryVerbose) { P_(i); P(A[i]->d_elemEnum); }

                LOOP_ASSERT(i, (void *)(900 + i) == D[i]);
                LOOP_ASSERT(i, i == A[i]->d_elemEnum);

                //bdlmxxx::ElemRef elemRef(D[i], A[i]); // parse error on Cygwin

                      bdlmxxx::ElemRef      elemRef(D[i], A[i]);
                      bdlmxxx::ConstElemRef constElemRef(D[i], A[i]);
                const bdlmxxx::ElemRef      ELEM_REF(D[i], A[i]);
                const bdlmxxx::ConstElemRef CONST_ELEM_REF(D[i], A[i]);

                LOOP_ASSERT(i, i            == elemRef.type());
                LOOP_ASSERT(i, (void *)D[i] == elemRef.data());

                LOOP_ASSERT(i, i            == constElemRef.type());
                LOOP_ASSERT(i, (void *)D[i] == constElemRef.data());

                LOOP_ASSERT(i, i            == ELEM_REF.type());
                LOOP_ASSERT(i, (void *)D[i] == ELEM_REF.data());

                LOOP_ASSERT(i, i            == CONST_ELEM_REF.type());
                LOOP_ASSERT(i, (void *)D[i] == CONST_ELEM_REF.data());
            }
        }

        if (verbose) cout << "\nVerify bdlmxxx::Table (as float) works" << endl;

        float tableData1 = 1.25;
        float tableData2 = 2.50;
        float tableData3 = 3.75;
        int   tableNullWord1 = 0, tableNullWord2 = 0, tableNullWord3 = 0;
        const int tableNullOffset = 0;
        bdlmxxx::ElemRef table1(&tableData1,
                            &tableDescriptor,
                            &tableNullWord1,
                            tableNullOffset);
        bdlmxxx::ElemRef table2(&tableData2,
                            &tableDescriptor,
                            &tableNullWord2,
                            tableNullOffset);
        bdlmxxx::ElemRef table3(&tableData3,
                            &tableDescriptor,
                            &tableNullWord3,
                            tableNullOffset);

        ASSERT(1 == table1.isNonNull());
        ASSERT(1 == table1.isNullable());

        bdlmxxx::ConstElemRef& constTable1 = table1;
        bdlmxxx::ConstElemRef& constTable2 = table2;
        bdlmxxx::ConstElemRef& constTable3 = table3;

        if (verbose) cout << "\tequality operators" << endl;

        if (veryVerbose) { T_ T_ P_(table1) P_(table2) P(table3) }

        ASSERT(1.25 == constTable1.theTable());
        ASSERT(2.5  == constTable2.theTable());
        ASSERT(3.75 == constTable3.theTable());

        ASSERT(table1 == table1);
        ASSERT(table1 != table2);
        ASSERT(table1 != table3);

        ASSERT(table2 != table1);
        ASSERT(table2 == table2);
        ASSERT(table2 != table3);

        ASSERT(table3 != table1);
        ASSERT(table3 != table2);
        ASSERT(table3 == table3);

        if (verbose) cout << "\treplaceValue" << endl;

        table1.replaceValue(table2);

        if (veryVerbose) { T_ T_ P_(table1) P_(table2) P(table3) }

        ASSERT(table1 == table1);
        ASSERT(table1 == table2);
        ASSERT(table1 != table3);

        table1.replaceValue(table3);

        if (veryVerbose) { T_ T_ P_(table1) P_(table2) P(table3) }

        ASSERT(table1 == table1);
        ASSERT(table1 != table2);
        ASSERT(table1 == table3);

        if (verbose) cout << "\tset/get" << endl;

        table1.theModifiableTable() = 10.125;
        table2.theModifiableTable() = 20.375;
        table3.theModifiableTable() = 30.625;

        if (veryVerbose) { T_ T_ P_(table1) P_(table2) P(table3) }

        ASSERT(10.125 == constTable1.theTable());
        ASSERT(20.375 == constTable2.theTable());
        ASSERT(30.625 == constTable3.theTable());

        if (verbose) cout << "\tmakeNull/isNonNull/isUnset/isNull" << endl;

        ASSERT(1 == constTable1.isNonNull());
        ASSERT(0 == isUnset(constTable1));
        ASSERT(0 == constTable1.isNull());

        ASSERT(1 == constTable2.isNonNull());
        ASSERT(0 == isUnset(constTable2));
        ASSERT(0 == constTable2.isNull());

        ASSERT(1 == constTable3.isNonNull());
        ASSERT(0 == isUnset(constTable3));
        ASSERT(0 == constTable3.isNull());

        table1.makeNull();
        if (veryVerbose) { T_ T_ P_(table1) P_(table2) P(table3) }

        ASSERT(0 == constTable1.isNonNull());
        ASSERT(1 == isUnset(constTable1));
        ASSERT(1 == constTable1.isNull());

        ASSERT(1 == constTable2.isNonNull());
        ASSERT(0 == isUnset(constTable2));
        ASSERT(0 == constTable2.isNull());

        ASSERT(1 == constTable3.isNonNull());
        ASSERT(0 == isUnset(constTable3));
        ASSERT(0 == constTable3.isNull());

        table2.makeNull();
        if (veryVerbose) { T_ T_ P_(table1) P_(table2) P(table3) }

        ASSERT(0 == constTable1.isNonNull());
        ASSERT(1 == isUnset(constTable1));
        ASSERT(1 == constTable1.isNull());

        ASSERT(0 == constTable2.isNonNull());
        ASSERT(1 == isUnset(constTable2));
        ASSERT(1 == constTable2.isNull());

        ASSERT(1 == constTable3.isNonNull());
        ASSERT(0 == isUnset(constTable3));
        ASSERT(0 == constTable3.isNull());

        table3.makeNull();
        if (veryVerbose) { T_ T_ P_(table1) P_(table2) P(table3) }

        ASSERT(0 == constTable1.isNonNull());
        ASSERT(1 == isUnset(constTable1));
        ASSERT(1 == constTable1.isNull());

        ASSERT(0 == constTable2.isNonNull());
        ASSERT(1 == isUnset(constTable2));
        ASSERT(1 == constTable2.isNull());

        ASSERT(0 == constTable3.isNonNull());
        ASSERT(1 == isUnset(constTable3));
        ASSERT(1 == constTable3.isNull());

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error: non-zero test status = " << testStatus << "."
             << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
