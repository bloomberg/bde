// bdem_bdemencoderutil.t.cpp                                         -*-C++-*-

#include <bdem_bdemencoderutil.h>

#include <bdem_elemtype.h>                // for testing only
#include <bdem_list.h>                    // for testing only
#include <bdem_row.h>                     // for testing only
#include <bdem_table.h>                   // for testing only

#include <bdex_byteinstreamformatter.h>   // for testing only
#include <bdex_byteinstreamraw.h>         // for testing only
#include <bdex_byteoutstreamraw.h>        // for testing only
#include <bdex_instreamfunctions.h>       // for testing only
#include <bdex_outstreamfunctions.h>

#include <bdesb_memoutstreambuf.h>
#include <bdesb_fixedmeminstreambuf.h>

#include <bdeat_attributeinfo.h>
#include <bdeat_selectioninfo.h>
#include <bdeat_typetraits.h>

#include <bdet_date.h>                    // for testing only
#include <bdet_datetime.h>                // for testing only
#include <bdet_time.h>                    // for testing only

#include <bdeu_print.h>                   // for testing only
#include <bdeu_printmethods.h>            // for testing only
#include <bdeu_string.h>                  // for testing only

#include <bslalg_typetraits.h>            // for testing only

#include <bsls_platform.h>                // for testing only
#include <bsls_types.h>

#ifdef BSLS_PLATFORM_OS_LINUX
#define __STDC_LIMIT_MACROS 1
#endif

#include <bsl_cctype.h>
#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>
#include <bsl_string.h>

#include <bsl_c_stdio.h>       // 'snprintf'
#include <bslfwd_bslma_allocator.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//
//-----------------------------------------------------------------------------
// [ 1] 'bdem' base types
// [ 2] vectors of 'bdem' base types
// [ 3] proxied types
// [ 4] vectors of proxied types
// [ 5] extended types
// [ 6] vectors of extended types
// [ 7] Sequences
// [ 8] vectors of Sequences
// [ 9] Choices
// [10] vectors of Choices
// [11] Enumerations
// [12] vectors of Enumerations
// [13] CustomizedType
// [14] vectors of CustomizedType
// [15] NullableValue
//-----------------------------------------------------------------------------
// [16] USAGE EXAMPLE

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
#define P(X) { bsl::cout << #X " = ";                          \
               bdeu_PrintMethods::print(bsl::cout, X, -1, -1); \
               bsl::cout << bsl::endl; }
#define P_(X) { bsl::cout << #X " = ";                         \
               bdeu_PrintMethods::print(bsl::cout, X, -1, -1); \
               bsl::cout << ", " << bsl::flush; }

#define Q(X)  bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.

#define L_ __LINE__                                      // current Line number
#define T_ bsl::cout << "\t" << bsl::flush;              // Print tab w/o '\n'.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdem_BdemEncoderUtil Encoder;  // alias for 'struct' under test

namespace OSF = bdex_OutStreamFunctions;
namespace ISF = bdex_InStreamFunctions;
namespace  VF = bdex_VersionFunctions;

//=============================================================================
//                          CLASSES FOR TESTING
//-----------------------------------------------------------------------------

enum Enum {
    e1, e2, e3
};

// test_customizedstring.h   -*-C++-*-
#ifndef INCLUDED_TEST_CUSTOMIZEDSTRING
#define INCLUDED_TEST_CUSTOMIZEDSTRING

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: CustomizedString
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_VERSIONFUNCTIONS
#include <bdex_versionfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace test {

class CustomizedString {

  private:
    // PRIVATE DATA MEMBERS
    bsl::string d_value;  // stored value

    // FRIENDS
    friend bool operator==(const CustomizedString&  lhs,
                           const CustomizedString&  rhs);
    friend bool operator!=(const CustomizedString&  lhs,
                           const CustomizedString&  rhs);

  public:
    // TYPES
    typedef bsl::string BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "CustomizedString")

    // CREATORS
    explicit CustomizedString(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator
        // is used.

    CustomizedString(const CustomizedString&  original,
                     bslma_Allocator         *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit CustomizedString(const bsl::string&  value,
                              bslma_Allocator    *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the specified
        // 'value'.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~CustomizedString();
        // Destroy this object.

    // MANIPULATORS
    CustomizedString& operator=(const CustomizedString& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    int fromString(const bsl::string& value);
        // Convert from the specified 'value' to this type.  Return 0 if
        // successful and non-zero otherwise.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    int maxSupportedBdexVersion() const;
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    const bsl::string& toString() const;
        // Convert this value to 'bsl::string'.
};

// FREE OPERATORS
inline
bool operator==(const CustomizedString& lhs, const CustomizedString& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const CustomizedString& lhs, const CustomizedString& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const CustomizedString& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CREATORS

inline
CustomizedString::CustomizedString(bslma_Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
CustomizedString::CustomizedString(const CustomizedString&  original,
                                   bslma_Allocator         *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
CustomizedString::CustomizedString(const bsl::string&  value,
                                   bslma_Allocator    *basicAllocator)
: d_value(value, basicAllocator)
{
}

inline
CustomizedString::~CustomizedString()
{
}

// MANIPULATORS

inline
CustomizedString& CustomizedString::operator=(const CustomizedString& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

template <class STREAM>
STREAM& CustomizedString::bdexStreamIn(STREAM& stream, int version)
{
    bsl::string temp;

    streamIn(stream, temp, version);

    if (!stream) {
        return stream;
    }

    if (fromString(temp)!=0) {
        stream.invalidate();
    }

    return stream;
}

inline
void CustomizedString::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_value);
}

inline
int CustomizedString::fromString(const bsl::string& value)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (5 < value.size()) {
        return FAILURE;
    }

    d_value = value;

    return SUCCESS;
}

// ACCESSORS

template <class STREAM>
STREAM& CustomizedString::bdexStreamOut(STREAM& stream, int version) const
{
    return streamOut(stream, d_value, version);
}

inline
int CustomizedString::maxSupportedBdexVersion() const
{
    return bdex_VersionFunctions::maxSupportedVersion(d_value);
}

inline
bsl::ostream& CustomizedString::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdeu_PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const bsl::string& CustomizedString::toString() const
{
    return d_value;
}

}  // close namespace test;

// TRAITS

BDEAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(test::CustomizedString)

// FREE OPERATORS

inline
bool test::operator==(const test::CustomizedString& lhs,
                                 const test::CustomizedString& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool test::operator!=(const test::CustomizedString& lhs,
                                 const test::CustomizedString& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& test::operator<<(bsl::ostream& stream,
                                          const test::CustomizedString& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_customizedstring.cpp  -*-C++-*-
#include <bsl_string.h>
namespace BloombergLP {
namespace test {
                               // ---------
                               // CONSTANTS
                               // ---------
const char CustomizedString::CLASS_NAME[] = "CustomizedString";
    // the name of this class
                                // -------------
                                // CLASS METHODS
                                // -------------
                                // --------
                                // CREATORS
                                // --------
                                // ------------
                                // MANIPULATORS
                                // ------------
                                // ---------
                                // ACCESSORS
                                // ---------
}  // close namespace test;
}  // close namespace BloombergLP;
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_mychoice.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYCHOICE
#define INCLUDED_TEST_MYCHOICE
//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: MyChoice
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation
#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif
#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif
#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_SELECTIONINFO
#include <bdeat_selectioninfo.h>
#endif
#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif
#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif
#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif
#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif
#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif
#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif
#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif
namespace BloombergLP {
namespace test {
class MyChoice {
  private:
    union {
        bsls_ObjectBuffer< int > d_selection1;
            // todo: provide annotation
        bsls_ObjectBuffer< bsl::string > d_selection2;
            // todo: provide annotation
    };
    int                 d_selectionId;
    bslma_Allocator    *d_allocator_p;
  public:
    // TYPES
    enum {
        NUM_SELECTIONS = 2 // the number of selections in this class
    };
    enum {
        SELECTION_INDEX_SELECTION1 = 0,
            // index for "Selection1" selection
        SELECTION_INDEX_SELECTION2 = 1
            // index for "Selection2" selection
    };
    enum {
        SELECTION_ID_UNDEFINED = -1,
        SELECTION_ID_SELECTION1 = 0,
            // id for "Selection1" selection
        SELECTION_ID_SELECTION2 = 1
            // id for "Selection2" selection
    };
  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MyChoice")
    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];
        // selection information for each selection
  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.
    static const bdeat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.
    static const bdeat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.
    // CREATORS
    explicit MyChoice(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MyChoice' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.
    MyChoice(const MyChoice& original, bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MyChoice' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.
    ~MyChoice();
        // Destroy this object.
    // MANIPULATORS
    MyChoice& operator=(const MyChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).
    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).
    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).
    void makeSelection1();
    void makeSelection1(int value);
        // Set the value of this object to be a "Selection1" value.
        // Optionally specify the 'value' of the "Selection1".  If
        // 'value' is not specified, the default "Selection1" value is
        // used.
    void makeSelection2();
    void makeSelection2(const bsl::string& value);
        // Set the value of this object to be a "Selection2" value.
        // Optionally specify the 'value' of the "Selection2".  If
        // 'value' is not specified, the default "Selection2" value is
        // used.
    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.
    int& selection1();
        // Return a reference to the modifiable "Selection1" selection
        // of this object if "Selection1" is the current selection.
        // The behavior is undefined unless "Selection1" is the
        // selection of this object.
    bsl::string& selection2();
        // Return a reference to the modifiable "Selection2" selection
        // of this object if "Selection2" is the current selection.
        // The behavior is undefined unless "Selection2" is the
        // selection of this object.
    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.
    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.
    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.
    const int& selection1() const;
        // Return a reference to the non-modifiable "Selection1"
        // selection of this object if "Selection1" is the current
        // selection.  The behavior is undefined unless "Selection1"
        // is the selection of this object.
    const bsl::string& selection2() const;
        // Return a reference to the non-modifiable "Selection2"
        // selection of this object if "Selection2" is the current
        // selection.  The behavior is undefined unless "Selection2"
        // is the selection of this object.
};
// FREE OPERATORS
inline
bool operator==(const MyChoice& lhs, const MyChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MyChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.
inline
bool operator!=(const MyChoice& lhs, const MyChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const MyChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.
// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================
// The following inlined functions are invoked from other inline functions.
inline
int MyChoice::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}
inline
void MyChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION2: {
        typedef bsl::string Type;
        d_selection2.object().~Type();
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }
    d_selectionId = SELECTION_ID_UNDEFINED;
}
inline
void MyChoice::makeSelection1()
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer()) int;
        d_selectionId = SELECTION_ID_SELECTION1;
    }
}
inline
void MyChoice::makeSelection1(int value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer()) int(value);
        d_selectionId = SELECTION_ID_SELECTION1;
    }
}
inline
void MyChoice::makeSelection2()
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer()) bsl::string(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION2;
    }
}
inline
void MyChoice::makeSelection2(const bsl::string& value)
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer()) bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION2;
    }
}
// ---------------------------------------------------------------------------
// CREATORS
inline
MyChoice::MyChoice(bslma_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}
inline
MyChoice::MyChoice(
    const MyChoice& original,
    bslma_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            int(original.d_selection1.object());
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            bsl::string(original.d_selection2.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
inline
MyChoice::~MyChoice()
{
    reset();
}
// MANIPULATORS
inline
MyChoice&
MyChoice::operator=(const MyChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
          } break;
          case SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
          } break;
          default:
            BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}
template <class STREAM>
inline
STREAM& MyChoice::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;                                      // RETURN
            }
            switch (selectionId) {
              case SELECTION_ID_SELECTION1: {
                makeSelection1();
                streamIn(
                    stream, d_selection1.object(), 1);
              } break;
              case SELECTION_ID_SELECTION2: {
                makeSelection2();
                streamIn(
                    stream, d_selection2.object(), 1);
              } break;
              case SELECTION_ID_UNDEFINED: {
                reset();
              } break;
              default:
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}
inline
int MyChoice::makeSelection(int selectionId)
{
    enum { NOT_FOUND = -1, SUCCESS = 0 };
    switch (selectionId) {
      case SELECTION_ID_SELECTION1: {
        makeSelection1();
      } break;
      case SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return NOT_FOUND;                                           // RETURN
    }
    return SUCCESS;
}
inline
int MyChoice::makeSelection(const char *name, int nameLength)
{
    enum { NOT_FOUND = -1 };
    const bdeat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return NOT_FOUND;                                            // RETURN
    }
    return makeSelection(selectionInfo->d_id);
}
template <class MANIPULATOR>
inline
int MyChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    enum { FAILURE = -1, SUCCESS = 0 };
    switch (d_selectionId) {
      case MyChoice::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
                                                                      // RETURN

      case MyChoice::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
                                                                      // RETURN

      default:
        BSLS_ASSERT_SAFE(MyChoice::SELECTION_ID_UNDEFINED ==
                     d_selectionId);
        return FAILURE;
    }
}
inline
int& MyChoice::selection1()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}
inline
bsl::string& MyChoice::selection2()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}
// ACCESSORS
template <class STREAM>
inline
STREAM& MyChoice::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_SELECTION1: {
                streamOut(
                    stream, d_selection1.object(), 1);
              } break;
              case SELECTION_ID_SELECTION2: {
                streamOut(
                    stream, d_selection2.object(), 1);
              } break;
              default:
                BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}
inline
int MyChoice::selectionId() const
{
    return d_selectionId;
}
template <class ACCESSOR>
inline
int MyChoice::accessSelection(ACCESSOR& accessor) const
{
    enum { FAILURE = -1, SUCCESS = 0 };
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
                                                                      // RETURN

      case SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
                                                                      // RETURN

      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;
    }
}
inline
const int& MyChoice::selection1() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}
inline
const bsl::string& MyChoice::selection2() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}
}  // close namespace test;
// TRAITS
BDEAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(test::MyChoice)
// FREE OPERATORS
inline
bool test::operator==(const test::MyChoice& lhs, const test::MyChoice& rhs)
{
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case test::MyChoice::SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
                                                                    // RETURN
          case test::MyChoice::SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
                                                                    // RETURN
          default:
            BSLS_ASSERT_SAFE(test::MyChoice::SELECTION_ID_UNDEFINED
                            == rhs.selectionId());
            return true;                                            // RETURN
        }
    }
    else {
        return false;
   }
}
inline
bool test::operator!=(const test::MyChoice& lhs, const test::MyChoice& rhs)
{
    return !(lhs == rhs);
}
inline
bsl::ostream& test::operator<<(bsl::ostream& stream, const test::MyChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}
}  // close namespace BloombergLP;
#endif
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_mychoice.cpp  -*-C++-*-
#include <bsl_iostream.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>
#include <bsl_string.h>
namespace BloombergLP {
namespace test {
                               // ---------
                               // CONSTANTS
                               // ---------
const char MyChoice::CLASS_NAME[] = "MyChoice";
    // the name of this class
const bdeat_SelectionInfo MyChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION1,
        "Selection1",                         // name
        sizeof("Selection1") - 1,             // name length
        "todo: provide annotation",    // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        SELECTION_ID_SELECTION2,
        "Selection2",                         // name
        sizeof("Selection2") - 1,             // name length
        "todo: provide annotation",    // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};
                               // -------------
                               // CLASS METHODS
                               // -------------
const bdeat_SelectionInfo *MyChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='E'
             && bdeu_CharType::toUpper(name[2])=='L'
             && bdeu_CharType::toUpper(name[3])=='E'
             && bdeu_CharType::toUpper(name[4])=='C'
             && bdeu_CharType::toUpper(name[5])=='T'
             && bdeu_CharType::toUpper(name[6])=='I'
             && bdeu_CharType::toUpper(name[7])=='O'
             && bdeu_CharType::toUpper(name[8])=='N') {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return
                          &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
                    } break;
                    case '2': {
                        return
                          &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
                    } break;
                }
            }
        } break;
    }
    return 0;
}
const bdeat_SelectionInfo *MyChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
      case SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
      default:
        return 0;
    }
}
                                // --------
                                // CREATORS
                                // --------
                                // ------------
                                // MANIPULATORS
                                // ------------
                                // ---------
                                // ACCESSORS
                                // ---------
bsl::ostream& MyChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }
    int levelPlus1 = level + 1;
    if (0 <= spacesPerLevel) {
        // multiline
        stream << "[\n";
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        switch (d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdeu_PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdeu_PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line
        stream << "[ ";
        switch (d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdeu_PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdeu_PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }
        stream << " ]";
    }
    return stream << bsl::flush;
}
}  // close namespace test;
}  // close namespace BloombergLP;
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_myenumeration.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYENUMERATION
#define INCLUDED_TEST_MYENUMERATION
//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: MyEnumeration
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation
#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif
#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_ENUMERATORINFO
#include <bdeat_enumeratorinfo.h>
#endif
#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif
#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif
#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif
#ifndef INCLUDED_BDEX_VERSIONFUNCTIONS
#include <bdex_versionfunctions.h>
#endif
#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif
namespace BloombergLP {
namespace test {
struct MyEnumeration {
  public:
    // TYPES
    enum Value {
        VALUE1 = 0,
            // todo: provide annotation
        VALUE2 = 1
            // todo: provide annotation
    };
    enum {
        NUM_ENUMERATORS = 2 // the number of enumerators in the 'Value'
                            // enumeration
    };
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MyEnumeration")
    static const bdeat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];
        // enumerator information for each enumerator
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.
    static const char *toString(Value value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.
    static int fromString(Value        *result,
                          const char   *string,
                          int           stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' of the specified 'stringLength'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' otherwise
        // (i.e., 'string' does not match any enumerator).
    static int fromInt(Value *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'number' does not match any
        // enumerator).
    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&  stream,
                                Value&   value,
                                int      version);
        // Assign to the specified 'value' the value read from the specified
        // input 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, the 'value' is valid, but its value is
        // undefined.  If the specified 'version' is not supported, 'stream' is
        // marked invalid, but 'value' is unaltered.  Note that no version is
        // read from 'stream'.  (See the package-group-level documentation for
        // more information on 'bdex' streaming of container types.)
    static bsl::ostream& print(bsl::ostream& stream, Value value);
        // Write to the specified 'stream' the string representation of
        // the specified enumeration 'value'.  Return a reference to
        // the modifiable 'stream'.
    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&  stream,
                                 Value    value,
                                 int      version);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Optionally specify
        // an explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If 'version'
        // is specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).
};
// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, MyEnumeration::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.
// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================
// The following inlined functions are invoked from other inline functions.
inline
int MyEnumeration::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}
inline
int MyEnumeration::fromInt(MyEnumeration::Value *result, int number)
{
    enum { SUCCESS = 0, NOT_FOUND = 1 };
    switch (number) {
      case MyEnumeration::VALUE1:
      case MyEnumeration::VALUE2:
        *result = (MyEnumeration::Value)number;
        return SUCCESS;                                         // RETURN
      default:
        return NOT_FOUND;
    }
}
inline
bsl::ostream& MyEnumeration::print(bsl::ostream&      stream,
                                 MyEnumeration::Value value)
{
    return stream << toString(value);
}
// ---------------------------------------------------------------------------
// CLASS METHODS
inline
const char *MyEnumeration::toString(MyEnumeration::Value value)
{
    switch (value) {
      case VALUE1: {
        return "VALUE1";
      } break;
      case VALUE2: {
        return "VALUE2";
      } break;
      default:
        BSLS_ASSERT_SAFE(!"encountered out-of-bound enumerated value");
    }
    return 0;
}
template <class STREAM>
inline
STREAM& MyEnumeration::bdexStreamIn(STREAM&             stream,
                                  MyEnumeration::Value& value,
                                  int                 version)
{
    switch(version) {
      case 1: {
        int readValue;
        stream.getInt32(readValue);
        if (stream) {
            if (fromInt(&value, readValue)) {
               stream.invalidate();   // bad value in stream
            }
        }
      } break;
      default: {
        stream.invalidate();          // unrecognized version number
      } break;
    }
    return stream;
}
template <class STREAM>
inline
STREAM& MyEnumeration::bdexStreamOut(STREAM&              stream,
                                     MyEnumeration::Value value,
                                     int                version)
{
    switch (version) {
      case 1: {
        stream.putInt32(value);  // Write the value as an int
      } break;
    }
    return stream;
}

template <typename STREAM>
inline
STREAM& streamIn(STREAM&                              stream,
                 test::MyEnumeration::Value& value,
                 int                                  version)
{
    return test::MyEnumeration::bdexStreamIn(stream, value, version);
}

inline
int maxSupportedVersion(test::MyEnumeration::Value)
{
    return test::MyEnumeration::maxSupportedBdexVersion();
}

template <typename STREAM>
inline
STREAM& streamOut(STREAM& stream,
                  const test::MyEnumeration::Value& value,
                  int     version)
{
    return test::MyEnumeration::bdexStreamOut(stream, value, version);
}

}  // close namespace test;

// TRAITS
BDEAT_DECL_ENUMERATION_TRAITS(test::MyEnumeration)
// FREE OPERATORS
inline
bsl::ostream& test::operator<<(bsl::ostream&               stream,
                               test::MyEnumeration::Value  rhs)
{
    return test::MyEnumeration::print(stream, rhs);
}
}  // close namespace BloombergLP;
#endif
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_myenumeration.cpp  -*-C++-*-
#include <bsl_iostream.h>
#include <bsls_assert.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
namespace BloombergLP {
namespace test {
                               // ---------
                               // CONSTANTS
                               // ---------
const char MyEnumeration::CLASS_NAME[] = "MyEnumeration";
    // the name of this class
const bdeat_EnumeratorInfo MyEnumeration::ENUMERATOR_INFO_ARRAY[] = {
    {
        MyEnumeration::VALUE1,
        "VALUE1",                      // name
        sizeof("VALUE1") - 1,          // name length
        "todo: provide annotation"  // annotation
    },
    {
        MyEnumeration::VALUE2,
        "VALUE2",                      // name
        sizeof("VALUE2") - 1,          // name length
        "todo: provide annotation"  // annotation
    }
};
                               // -------------
                               // CLASS METHODS
                               // -------------
int MyEnumeration::fromString(MyEnumeration::Value *result,
                            const char         *string,
                            int                 stringLength)
{
    enum { SUCCESS = 0, NOT_FOUND = 1 };
    switch(stringLength) {
        case 6: {
            if (bdeu_CharType::toUpper(string[0])=='V'
             && bdeu_CharType::toUpper(string[1])=='A'
             && bdeu_CharType::toUpper(string[2])=='L'
             && bdeu_CharType::toUpper(string[3])=='U'
             && bdeu_CharType::toUpper(string[4])=='E') {
                switch(bdeu_CharType::toUpper(string[5])) {
                    case '1': {
                        *result = MyEnumeration::VALUE1;
                        return SUCCESS;                               // RETURN

                    } break;
                    case '2': {
                        *result = MyEnumeration::VALUE2;
                        return SUCCESS;                               // RETURN

                    } break;
                }
            }
        } break;
    }
    return NOT_FOUND;
}
                                // --------
                                // CREATORS
                                // --------
                                // ------------
                                // MANIPULATORS
                                // ------------
                                // ---------
                                // ACCESSORS
                                // ---------
}  // close namespace test;
}  // close namespace BloombergLP;
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_mysequence.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSEQUENCE
#define INCLUDED_TEST_MYSEQUENCE
//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: MySequence
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation
#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif
#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif
#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif
#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif
#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif
#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif
#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif
#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif
namespace BloombergLP {
namespace test {
class MySequence {
  private:
    int d_attribute1; // todo: provide annotation
    bsl::string d_attribute2; // todo: provide annotation
  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 2 // the number of attributes in this class
    };
    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 1
            // index for "Attribute2" attribute
    };
    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 1
            // id for "Attribute2" attribute
    };
  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequence")
    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute
  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.
    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.
    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.
    // CREATORS
    explicit MySequence(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequence' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.
    MySequence(const MySequence&  original,
               bslma_Allocator   *basicAllocator = 0);
        // Create an object of type 'MySequence' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.
    ~MySequence();
        // Destroy this object.
    // MANIPULATORS
    MySequence& operator=(const MySequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).
    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.
    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.
    bsl::string& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.
    const int& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1"
        // attribute of this object.
    const bsl::string& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2"
        // attribute of this object.
};
// FREE OPERATORS
inline
bool operator==(const MySequence& lhs, const MySequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.
inline
bool operator!=(const MySequence& lhs, const MySequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.
// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================
// The following inlined functions are invoked from other inline functions.
inline
int MySequence::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}
// ---------------------------------------------------------------------------
// CREATORS
inline
MySequence::MySequence(bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(bslma_Default::allocator(basicAllocator))
{
}
inline
MySequence::MySequence(
    const MySequence& original,
    bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, bslma_Default::allocator(basicAllocator))
{
}
inline
MySequence::~MySequence()
{
}
// MANIPULATORS
inline
MySequence&
MySequence::operator=(const MySequence& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
    }
    return *this;
}
template <class STREAM>
inline
STREAM& MySequence::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            streamIn(
                  stream, d_attribute1, 1);
            streamIn(
                  stream, d_attribute2, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}
inline
void MySequence::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
}
template <class MANIPULATOR>
inline
int MySequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;
    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    return ret;
}
template <class MANIPULATOR>
inline
int MySequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}
template <class MANIPULATOR>
inline
int MySequence::manipulateAttribute(MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };
    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
    }
    return manipulateAttribute(manipulator, attributeInfo->d_id);
}
inline
int& MySequence::attribute1()
{
    return d_attribute1;
}
inline
bsl::string& MySequence::attribute2()
{
    return d_attribute2;
}
// ACCESSORS
template <class STREAM>
inline
STREAM& MySequence::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        streamOut(stream, d_attribute1, 1);
        streamOut(stream, d_attribute2, 1);
      } break;
    }
    return stream;
}
template <class ACCESSOR>
inline
int MySequence::accessAttributes(ACCESSOR& accessor) const
{
    int ret;
    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    return ret;
}
template <class ACCESSOR>
inline
int MySequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}
template <class ACCESSOR>
inline
int MySequence::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };
     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
     }
     return accessAttribute(accessor, attributeInfo->d_id);
}
inline
const int& MySequence::attribute1() const
{
    return d_attribute1;
}
inline
const bsl::string& MySequence::attribute2() const
{
    return d_attribute2;
}
}  // close namespace test;
// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequence)
// FREE OPERATORS
inline
bool test::operator==(const test::MySequence& lhs, const test::MySequence& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2();
}
inline
bool test::operator!=(const test::MySequence& lhs, const test::MySequence& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2();
}
inline
bsl::ostream& test::operator<<(bsl::ostream&            stream,
                               const test::MySequence&  rhs)
{
    return rhs.print(stream, 0, -1);
}
}  // close namespace BloombergLP;
#endif
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_mysequence.cpp  -*-C++-*-
#include <bsl_iostream.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>
#include <bsl_string.h>
namespace BloombergLP {
namespace test {
                               // ---------
                               // CONSTANTS
                               // ---------
const char MySequence::CLASS_NAME[] = "MySequence";
    // the name of this class
const bdeat_AttributeInfo MySequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                     // name
        sizeof("Attribute1") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                     // name
        sizeof("Attribute2") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};
                               // -------------
                               // CLASS METHODS
                               // -------------
const bdeat_AttributeInfo *MySequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='R'
             && bdeu_CharType::toUpper(name[4])=='I'
             && bdeu_CharType::toUpper(name[5])=='B'
             && bdeu_CharType::toUpper(name[6])=='U'
             && bdeu_CharType::toUpper(name[7])=='T'
             && bdeu_CharType::toUpper(name[8])=='E') {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return
                          &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                    } break;
                    case '2': {
                        return
                          &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                    } break;
                }
            }
        } break;
    }
    return 0;
}
const bdeat_AttributeInfo *MySequence::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      default:
        return 0;
    }
}
                                // --------
                                // CREATORS
                                // --------
                                // ------------
                                // MANIPULATORS
                                // ------------
                                // ---------
                                // ACCESSORS
                                // ---------
bsl::ostream& MySequence::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }
    int levelPlus1 = level + 1;
    if (0 <= spacesPerLevel) {
        // multiline
        stream << "[\n";
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line
        stream << '[';
        stream << ' ';
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                             -levelPlus1, spacesPerLevel);
        stream << " ]";
    }
    return stream << bsl::flush;
}
}  // close namespace test;
}  // close namespace BloombergLP;
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_mysequencewithnullables.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSEQUENCEWITHNULLABLES
#define INCLUDED_TEST_MYSEQUENCEWITHNULLABLES
//@PURPOSE: todo: provide purpose
//
//@CLASSES:
// test::MySequenceWithNullables
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
// todo: provide annotation
//
///Usage Examples
///--------------
//
//..
//..
#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif
#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif
#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif
#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif
#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif
#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif
#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif
#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif
#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif
#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif
namespace BloombergLP {
namespace test { class MySequenceWithNullables; }
// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithNullables)
namespace test {
                    // =============================
                    // class MySequenceWithNullables
                    // =============================
class MySequenceWithNullables {
    // todo: provide annotation
  private:
    // TODO: provide annotations for members.
    bdeut_NullableValue<int> d_attribute1;
    bdeut_NullableValue<bsl::string> d_attribute2;
    bdeut_NullableValue<MySequence> d_attribute3;
    bdeut_NullableValue<bsl::vector<int> > d_attribute4;
    bdeut_NullableValue<MyEnumeration::Value> d_attribute5;
  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 5 // the number of attributes in this class
    };
    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 1,
            // index for "Attribute2" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE3 = 2,
            // index for "Attribute3" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE4 = 3,
            // index for "Attribute4" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE5 = 4
            // index for "Attribute5" attribute
    };
    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 1,
            // id for "Attribute2" attribute
        ATTRIBUTE_ID_ATTRIBUTE3 = 2,
            // id for "Attribute3" attribute
        ATTRIBUTE_ID_ATTRIBUTE4 = 3,
            // id for "Attribute4" attribute
        ATTRIBUTE_ID_ATTRIBUTE5 = 4
            // id for "Attribute5" attribute
    };
  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequenceWithNullables")
    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute
  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.
    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.
    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.
    // CREATORS
    explicit MySequenceWithNullables(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNullables' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
    MySequenceWithNullables(
                    const MySequenceWithNullables&  original,
                    bslma_Allocator                *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNullables' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.
    ~MySequenceWithNullables();
        // Destroy this object.
    // MANIPULATORS
    MySequenceWithNullables& operator=(const MySequenceWithNullables& rhs);
        // Assign to this object the value of the specified 'rhs' object.
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).
    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.
    bdeut_NullableValue<int>& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.
    bdeut_NullableValue<bsl::string>& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
        // object.
    bdeut_NullableValue<MySequence>& attribute3();
        // Return a reference to the modifiable "Attribute3" attribute of this
        // object.
    bdeut_NullableValue<bsl::vector<int> >& attribute4();
        // Return a reference to the modifiable "Attribute4" attribute of this
        // object.
    bdeut_NullableValue<MyEnumeration::Value>& attribute5();
        // Return a reference to the modifiable "Attribute5" attribute of this
        // object.
    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.
    const bdeut_NullableValue<int>& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.
    const bdeut_NullableValue<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.
    const bdeut_NullableValue<MySequence>& attribute3() const;
        // Return a reference to the non-modifiable "Attribute3" attribute of
        // this object.
    const bdeut_NullableValue<bsl::vector<int> >& attribute4() const;
        // Return a reference to the non-modifiable "Attribute4" attribute of
        // this object.
    const bdeut_NullableValue<MyEnumeration::Value>& attribute5() const;
        // Return a reference to the non-modifiable "Attribute5" attribute of
        // this object.
};
// FREE OPERATORS
inline
bool operator==(const MySequenceWithNullables&  lhs,
                const MySequenceWithNullables&  rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.
inline
bool operator!=(const MySequenceWithNullables&  lhs,
                const MySequenceWithNullables&  rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.
inline
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const MySequenceWithNullables&  rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.
// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================
                    // -----------------------------
                    // class MySequenceWithNullables
                    // -----------------------------
// CLASS METHODS
inline
int MySequenceWithNullables::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}
// CREATORS
inline
MySequenceWithNullables::MySequenceWithNullables(
                                               bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(bslma_Default::allocator(basicAllocator))
, d_attribute3(bslma_Default::allocator(basicAllocator))
, d_attribute4(bslma_Default::allocator(basicAllocator))
, d_attribute5()
{
}
inline
MySequenceWithNullables::MySequenceWithNullables(
        const MySequenceWithNullables& original,
        bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, bslma_Default::allocator(basicAllocator))
, d_attribute3(original.d_attribute3, bslma_Default::allocator(basicAllocator))
, d_attribute4(original.d_attribute4, bslma_Default::allocator(basicAllocator))
, d_attribute5(original.d_attribute5)
{
}
inline
MySequenceWithNullables::~MySequenceWithNullables()
{
}
// MANIPULATORS
inline
MySequenceWithNullables&
MySequenceWithNullables::operator=(const MySequenceWithNullables& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
        d_attribute3 = rhs.d_attribute3;
        d_attribute4 = rhs.d_attribute4;
        d_attribute5 = rhs.d_attribute5;
    }
    return *this;
}
template <class STREAM>
inline
STREAM& MySequenceWithNullables::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            streamIn(stream, d_attribute1, 1);
            streamIn(stream, d_attribute2, 1);
            streamIn(stream, d_attribute3, 1);
            streamIn(stream, d_attribute4, 1);
            streamIn(stream, d_attribute5, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}
inline
void MySequenceWithNullables::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
    bdeat_ValueTypeFunctions::reset(&d_attribute3);
    bdeat_ValueTypeFunctions::reset(&d_attribute4);
    bdeat_ValueTypeFunctions::reset(&d_attribute5);
}
template <class MANIPULATOR>
inline
int MySequenceWithNullables::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;
    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute3,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute4,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute5,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    return ret;
}
template <class MANIPULATOR>
inline
int MySequenceWithNullables::manipulateAttribute(MANIPULATOR&  manipulator,
                                                 int           id)
{
    enum { NOT_FOUND = -1 };
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE3: {
        return manipulator(&d_attribute3,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE4: {
        return manipulator(&d_attribute4,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE5: {
        return manipulator(&d_attribute5,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}
template <class MANIPULATOR>
inline
int MySequenceWithNullables::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };
    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
    }
    return manipulateAttribute(manipulator, attributeInfo->d_id);
}
inline
bdeut_NullableValue<int>& MySequenceWithNullables::attribute1()
{
    return d_attribute1;
}
inline
bdeut_NullableValue<bsl::string>& MySequenceWithNullables::attribute2()
{
    return d_attribute2;
}
inline
bdeut_NullableValue<MySequence>& MySequenceWithNullables::attribute3()
{
    return d_attribute3;
}
inline
bdeut_NullableValue<bsl::vector<int> >& MySequenceWithNullables::attribute4()
{
    return d_attribute4;
}
inline
bdeut_NullableValue<MyEnumeration::Value>&
                                          MySequenceWithNullables::attribute5()
{
    return d_attribute5;
}
// ACCESSORS
template <class STREAM>
inline
STREAM& MySequenceWithNullables::bdexStreamOut(STREAM&  stream,
                                               int      version) const
{
    switch (version) {
      case 1: {
        streamOut(stream, d_attribute1, 1);
        streamOut(stream, d_attribute2, 1);
        streamOut(stream, d_attribute3, 1);
        streamOut(stream, d_attribute4, 1);
        streamOut(stream, d_attribute5, 1);
      } break;
    }
    return stream;
}
template <class ACCESSOR>
inline
int MySequenceWithNullables::accessAttributes(ACCESSOR& accessor) const
{
    int ret;
    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute3,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute4,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute5,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    return ret;
}
template <class ACCESSOR>
inline
int MySequenceWithNullables::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE3: {
        return accessor(d_attribute3,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE4: {
        return accessor(d_attribute4,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE5: {
        return accessor(d_attribute5,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}
template <class ACCESSOR>
inline
int MySequenceWithNullables::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };
     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
     }
     return accessAttribute(accessor, attributeInfo->d_id);
}
inline
const bdeut_NullableValue<int>& MySequenceWithNullables::attribute1() const
{
    return d_attribute1;
}
inline
const bdeut_NullableValue<bsl::string>&
                                    MySequenceWithNullables::attribute2() const
{
    return d_attribute2;
}
inline
const bdeut_NullableValue<MySequence>&
                                    MySequenceWithNullables::attribute3() const
{
    return d_attribute3;
}
inline
const bdeut_NullableValue<bsl::vector<int> >&
                                    MySequenceWithNullables::attribute4() const
{
    return d_attribute4;
}
inline
const bdeut_NullableValue<MyEnumeration::Value>&
                                    MySequenceWithNullables::attribute5() const
{
    return d_attribute5;
}
}  // close namespace test;
// FREE FUNCTIONS
inline
bool test::operator==(const test::MySequenceWithNullables&  lhs,
                      const test::MySequenceWithNullables&  rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2()
         && lhs.attribute3() == rhs.attribute3()
         && lhs.attribute4() == rhs.attribute4()
         && lhs.attribute5() == rhs.attribute5();
}
inline
bool test::operator!=(const test::MySequenceWithNullables&  lhs,
                      const test::MySequenceWithNullables&  rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2()
         || lhs.attribute3() != rhs.attribute3()
         || lhs.attribute4() != rhs.attribute4()
         || lhs.attribute5() != rhs.attribute5();
}
inline
bsl::ostream& test::operator<<(bsl::ostream&                         stream,
                               const test::MySequenceWithNullables&  rhs)
{
    return rhs.print(stream, 0, -1);
}
}  // close namespace BloombergLP;
#endif
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
// test_mysequencewithnullables.cpp   -*-C++-*-

#include <bdeat_formattingmode.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeut_nullablevalue.h>

#include <bsl_cctype.h>
#include <bsl_iomanip.h>
namespace BloombergLP {
namespace test {
                    // -----------------------------
                    // class MySequenceWithNullables
                    // -----------------------------
// CONSTANTS
const char MySequenceWithNullables::CLASS_NAME[] = "MySequenceWithNullables";
    // the name of this class
const bdeat_AttributeInfo MySequenceWithNullables::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                     // name
        sizeof("Attribute1") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                     // name
        sizeof("Attribute2") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE3,
        "Attribute3",                     // name
        sizeof("Attribute3") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE4,
        "Attribute4",                     // name
        sizeof("Attribute4") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE5,
        "Attribute5",                     // name
        sizeof("Attribute5") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    }
};
// CLASS METHODS
const bdeat_AttributeInfo *MySequenceWithNullables::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='R'
             && bdeu_CharType::toUpper(name[4])=='I'
             && bdeu_CharType::toUpper(name[5])=='B'
             && bdeu_CharType::toUpper(name[6])=='U'
             && bdeu_CharType::toUpper(name[7])=='T'
             && bdeu_CharType::toUpper(name[8])=='E') {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return
                          &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                    } break;
                    case '2': {
                        return
                          &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                    } break;
                    case '3': {
                        return
                          &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3];
                    } break;
                    case '4': {
                        return
                          &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4];
                    } break;
                    case '5': {
                        return
                          &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5];
                    } break;
                }
            }
        } break;
    }
    return 0;
}
const bdeat_AttributeInfo *MySequenceWithNullables::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      case ATTRIBUTE_ID_ATTRIBUTE3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3];
      case ATTRIBUTE_ID_ATTRIBUTE4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4];
      case ATTRIBUTE_ID_ATTRIBUTE5:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5];
      default:
        return 0;
    }
}
// ACCESSORS
bsl::ostream& MySequenceWithNullables::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }
    int levelPlus1 = level + 1;
    if (0 <= spacesPerLevel) {
        // multiline
        stream << "[\n";
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute3 = ";
        bdeu_PrintMethods::print(stream, d_attribute3,
                                 -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute4 = ";
        bdeu_PrintMethods::print(stream, d_attribute4,
                                 -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute5 = ";
        bdeu_PrintMethods::print(stream, d_attribute5,
                                 -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line
        stream << '[';
        stream << ' ';
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute3 = ";
        bdeu_PrintMethods::print(stream, d_attribute3,
                                 -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute4 = ";
        bdeu_PrintMethods::print(stream, d_attribute4,
                                 -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute5 = ";
        bdeu_PrintMethods::print(stream, d_attribute5,
                                 -levelPlus1, spacesPerLevel);
        stream << " ]";
    }
    return stream << bsl::flush;
}
}  // close namespace test;
}  // close namespace BloombergLP;
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
// test_mysequencewithnullablescontainer.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSEQUENCEWITHNULLABLESCONTAINER
#define INCLUDED_TEST_MYSEQUENCEWITHNULLABLESCONTAINER
//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: MySequenceWithNullablesContainer
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation
#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif
#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif
#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif
#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif
#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif
#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif
#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif
namespace BloombergLP {
namespace test {
class MySequenceWithNullablesContainer {
  private:
    MySequenceWithNullables d_contained; // todo: provide annotation
  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 1 // the number of attributes in this class
    };
    enum {
        ATTRIBUTE_INDEX_CONTAINED = 0
            // index for "Contained" attribute
    };
    enum {
        ATTRIBUTE_ID_CONTAINED = 0
            // id for "Contained" attribute
    };
  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequenceWithNullablesContainer")
    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute
  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.
    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.
    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.
    // CREATORS
    explicit MySequenceWithNullablesContainer(
                                          bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNullablesContainer' having
        // the default value.  Use the optionally specified 'basicAllocator'
        // to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.
    MySequenceWithNullablesContainer(
                  const MySequenceWithNullablesContainer&  original,
                  bslma_Allocator                         *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNullablesContainer' having
        // the value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator'
        // is 0, the currently installed default allocator is used.
    ~MySequenceWithNullablesContainer();
        // Destroy this object.
    // MANIPULATORS
    MySequenceWithNullablesContainer& operator=(
                                 const MySequenceWithNullablesContainer&  rhs);
        // Assign to this object the value of the specified 'rhs' object.
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).
    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.
    MySequenceWithNullables& contained();
        // Return a reference to the modifiable "Contained" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.
    const MySequenceWithNullables& contained() const;
        // Return a reference to the non-modifiable "Contained"
        // attribute of this object.
};
// FREE OPERATORS
inline
bool operator==(const MySequenceWithNullablesContainer&  lhs,
                const MySequenceWithNullablesContainer&  rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.
inline
bool operator!=(const MySequenceWithNullablesContainer&  lhs,
                const MySequenceWithNullablesContainer&  rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.
inline
bsl::ostream& operator<<(bsl::ostream&                            stream,
                         const MySequenceWithNullablesContainer&  rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.
// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================
// The following inlined functions are invoked from other inline functions.
inline
int MySequenceWithNullablesContainer::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}
// ---------------------------------------------------------------------------
// CREATORS
inline
MySequenceWithNullablesContainer::MySequenceWithNullablesContainer(
                                               bslma_Allocator *basicAllocator)
: d_contained(bslma_Default::allocator(basicAllocator))
{
}
inline
MySequenceWithNullablesContainer::MySequenceWithNullablesContainer(
    const MySequenceWithNullablesContainer& original,
    bslma_Allocator *basicAllocator)
: d_contained(original.d_contained, bslma_Default::allocator(basicAllocator))
{
}
inline
MySequenceWithNullablesContainer::~MySequenceWithNullablesContainer()
{
}
// MANIPULATORS
inline
MySequenceWithNullablesContainer&
MySequenceWithNullablesContainer::operator=(
                                   const MySequenceWithNullablesContainer& rhs)
{
    if (this != &rhs) {
        d_contained = rhs.d_contained;
    }
    return *this;
}
template <class STREAM>
inline
STREAM& MySequenceWithNullablesContainer::bdexStreamIn(STREAM&  stream,
                                                       int      version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            streamIn(
                  stream, d_contained, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}
inline
void MySequenceWithNullablesContainer::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_contained);
}
template <class MANIPULATOR>
inline
int MySequenceWithNullablesContainer::manipulateAttributes(
                                                      MANIPULATOR& manipulator)
{
    int ret;
    ret = manipulator(&d_contained,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTAINED]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    return ret;
}
template <class MANIPULATOR>
inline
int MySequenceWithNullablesContainer::manipulateAttribute(
                                                     MANIPULATOR&  manipulator,
                                                     int           id)
{
    enum { NOT_FOUND = -1 };
    switch (id) {
      case ATTRIBUTE_ID_CONTAINED: {
        return manipulator(&d_contained,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTAINED]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}
template <class MANIPULATOR>
inline
int MySequenceWithNullablesContainer::manipulateAttribute(
                                     MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };
    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
    }
    return manipulateAttribute(manipulator, attributeInfo->d_id);
}
inline
MySequenceWithNullables& MySequenceWithNullablesContainer::contained()
{
    return d_contained;
}
// ACCESSORS
template <class STREAM>
inline
STREAM& MySequenceWithNullablesContainer::bdexStreamOut(STREAM&  stream,
                                                        int      version) const
{
    switch (version) {
      case 1: {
        streamOut(stream, d_contained, 1);
      } break;
    }
    return stream;
}
template <class ACCESSOR>
inline
int MySequenceWithNullablesContainer::accessAttributes(ACCESSOR& accessor)
                                                                          const
{
    int ret;
    ret = accessor(d_contained,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTAINED]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    return ret;
}
template <class ACCESSOR>
inline
int MySequenceWithNullablesContainer::accessAttribute(ACCESSOR&  accessor,
                                                      int        id) const
{
    enum { NOT_FOUND = -1 };
    switch (id) {
      case ATTRIBUTE_ID_CONTAINED: {
        return accessor(d_contained,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTAINED]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}
template <class ACCESSOR>
inline
int MySequenceWithNullablesContainer::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };
     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
     }
     return accessAttribute(accessor, attributeInfo->d_id);
}
inline
const MySequenceWithNullables&
MySequenceWithNullablesContainer::contained() const
{
    return d_contained;
}
}  // close namespace test
// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(
                                        test::MySequenceWithNullablesContainer)
// FREE OPERATORS
inline
bool test::operator==(const test::MySequenceWithNullablesContainer&  lhs,
                      const test::MySequenceWithNullablesContainer&  rhs)
{
    return  lhs.contained() == rhs.contained();
}
inline
bool test::operator!=(const test::MySequenceWithNullablesContainer&  lhs,
                      const test::MySequenceWithNullablesContainer&  rhs)
{
    return  lhs.contained() != rhs.contained();
}
inline
bsl::ostream& test::operator<<(
                        bsl::ostream&                                  stream,
                        const test::MySequenceWithNullablesContainer&  rhs)
{
    return rhs.print(stream, 0, -1);
}
}  // close namespace BloombergLP;
#endif
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_mysequencewithnullablescontainer.cpp  -*-C++-*-
#include <bsl_iostream.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>
namespace BloombergLP {
namespace test {
                               // ---------
                               // CONSTANTS
                               // ---------
const char MySequenceWithNullablesContainer::CLASS_NAME[]
                                          = "MySequenceWithNullablesContainer";
    // the name of this class
const bdeat_AttributeInfo
  MySequenceWithNullablesContainer::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_CONTAINED,
        "Contained",                     // name
        sizeof("Contained") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    }
};
                               // -------------
                               // CLASS METHODS
                               // -------------
const bdeat_AttributeInfo *
MySequenceWithNullablesContainer::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 9: {
            if (bdeu_CharType::toUpper(name[0])=='C'
             && bdeu_CharType::toUpper(name[1])=='O'
             && bdeu_CharType::toUpper(name[2])=='N'
             && bdeu_CharType::toUpper(name[3])=='T'
             && bdeu_CharType::toUpper(name[4])=='A'
             && bdeu_CharType::toUpper(name[5])=='I'
             && bdeu_CharType::toUpper(name[6])=='N'
             && bdeu_CharType::toUpper(name[7])=='E'
             && bdeu_CharType::toUpper(name[8])=='D') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTAINED];
            }
        } break;
    }
    return 0;
}
const bdeat_AttributeInfo *
MySequenceWithNullablesContainer::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_CONTAINED:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTAINED];
      default:
        return 0;
    }
}
                                // --------
                                // CREATORS
                                // --------
                                // ------------
                                // MANIPULATORS
                                // ------------
                                // ---------
                                // ACCESSORS
                                // ---------
bsl::ostream& MySequenceWithNullablesContainer::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }
    int levelPlus1 = level + 1;
    if (0 <= spacesPerLevel) {
        // multiline
        stream << "[\n";
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Contained = ";
        bdeu_PrintMethods::print(stream, d_contained,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line
        stream << '[';
        stream << ' ';
        stream << "Contained = ";
        bdeu_PrintMethods::print(stream, d_contained,
                             -levelPlus1, spacesPerLevel);
        stream << " ]";
    }
    return stream << bsl::flush;
}
}  // close namespace test;
}  // close namespace BloombergLP;
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_mysequencewithmanynullables.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSEQUENCEWITHMANYNULLABLES
#define INCLUDED_TEST_MYSEQUENCEWITHMANYNULLABLES
//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: MySequenceWithManyNullables
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation
#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif
#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif
#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif
#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif
#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif
#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif
#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif
namespace BloombergLP {
namespace test {
class MySequenceWithManyNullables {
  private:
    bdeut_NullableValue<int> d_attribute01; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute02; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute03; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute04; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute05; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute06; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute07; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute08; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute09; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute10; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute11; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute12; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute13; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute14; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute15; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute16; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute17; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute18; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute19; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute20; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute21; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute22; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute23; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute24; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute25; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute26; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute27; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute28; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute29; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute30; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute31; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute32; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute33; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute34; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute35; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute36; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute37; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute38; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute39; // todo: provide annotation
    bdeut_NullableValue<int> d_attribute40; // todo: provide annotation
  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 40 // the number of attributes in this class
    };
    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE01 = 0,
            // index for "Attribute01" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE02 = 1,
            // index for "Attribute02" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE03 = 2,
            // index for "Attribute03" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE04 = 3,
            // index for "Attribute04" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE05 = 4,
            // index for "Attribute05" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE06 = 5,
            // index for "Attribute06" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE07 = 6,
            // index for "Attribute07" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE08 = 7,
            // index for "Attribute08" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE09 = 8,
            // index for "Attribute09" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE10 = 9,
            // index for "Attribute10" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE11 = 10,
            // index for "Attribute11" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE12 = 11,
            // index for "Attribute12" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE13 = 12,
            // index for "Attribute13" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE14 = 13,
            // index for "Attribute14" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE15 = 14,
            // index for "Attribute15" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE16 = 15,
            // index for "Attribute16" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE17 = 16,
            // index for "Attribute17" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE18 = 17,
            // index for "Attribute18" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE19 = 18,
            // index for "Attribute19" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE20 = 19,
            // index for "Attribute20" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE21 = 20,
            // index for "Attribute21" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE22 = 21,
            // index for "Attribute22" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE23 = 22,
            // index for "Attribute23" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE24 = 23,
            // index for "Attribute24" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE25 = 24,
            // index for "Attribute25" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE26 = 25,
            // index for "Attribute26" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE27 = 26,
            // index for "Attribute27" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE28 = 27,
            // index for "Attribute28" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE29 = 28,
            // index for "Attribute29" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE30 = 29,
            // index for "Attribute30" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE31 = 30,
            // index for "Attribute31" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE32 = 31,
            // index for "Attribute32" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE33 = 32,
            // index for "Attribute33" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE34 = 33,
            // index for "Attribute34" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE35 = 34,
            // index for "Attribute35" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE36 = 35,
            // index for "Attribute36" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE37 = 36,
            // index for "Attribute37" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE38 = 37,
            // index for "Attribute38" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE39 = 38,
            // index for "Attribute39" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE40 = 39
            // index for "Attribute40" attribute
    };
    enum {
        ATTRIBUTE_ID_ATTRIBUTE01 = 0,
            // id for "Attribute01" attribute
        ATTRIBUTE_ID_ATTRIBUTE02 = 1,
            // id for "Attribute02" attribute
        ATTRIBUTE_ID_ATTRIBUTE03 = 2,
            // id for "Attribute03" attribute
        ATTRIBUTE_ID_ATTRIBUTE04 = 3,
            // id for "Attribute04" attribute
        ATTRIBUTE_ID_ATTRIBUTE05 = 4,
            // id for "Attribute05" attribute
        ATTRIBUTE_ID_ATTRIBUTE06 = 5,
            // id for "Attribute06" attribute
        ATTRIBUTE_ID_ATTRIBUTE07 = 6,
            // id for "Attribute07" attribute
        ATTRIBUTE_ID_ATTRIBUTE08 = 7,
            // id for "Attribute08" attribute
        ATTRIBUTE_ID_ATTRIBUTE09 = 8,
            // id for "Attribute09" attribute
        ATTRIBUTE_ID_ATTRIBUTE10 = 9,
            // id for "Attribute10" attribute
        ATTRIBUTE_ID_ATTRIBUTE11 = 10,
            // id for "Attribute11" attribute
        ATTRIBUTE_ID_ATTRIBUTE12 = 11,
            // id for "Attribute12" attribute
        ATTRIBUTE_ID_ATTRIBUTE13 = 12,
            // id for "Attribute13" attribute
        ATTRIBUTE_ID_ATTRIBUTE14 = 13,
            // id for "Attribute14" attribute
        ATTRIBUTE_ID_ATTRIBUTE15 = 14,
            // id for "Attribute15" attribute
        ATTRIBUTE_ID_ATTRIBUTE16 = 15,
            // id for "Attribute16" attribute
        ATTRIBUTE_ID_ATTRIBUTE17 = 16,
            // id for "Attribute17" attribute
        ATTRIBUTE_ID_ATTRIBUTE18 = 17,
            // id for "Attribute18" attribute
        ATTRIBUTE_ID_ATTRIBUTE19 = 18,
            // id for "Attribute19" attribute
        ATTRIBUTE_ID_ATTRIBUTE20 = 19,
            // id for "Attribute20" attribute
        ATTRIBUTE_ID_ATTRIBUTE21 = 20,
            // id for "Attribute21" attribute
        ATTRIBUTE_ID_ATTRIBUTE22 = 21,
            // id for "Attribute22" attribute
        ATTRIBUTE_ID_ATTRIBUTE23 = 22,
            // id for "Attribute23" attribute
        ATTRIBUTE_ID_ATTRIBUTE24 = 23,
            // id for "Attribute24" attribute
        ATTRIBUTE_ID_ATTRIBUTE25 = 24,
            // id for "Attribute25" attribute
        ATTRIBUTE_ID_ATTRIBUTE26 = 25,
            // id for "Attribute26" attribute
        ATTRIBUTE_ID_ATTRIBUTE27 = 26,
            // id for "Attribute27" attribute
        ATTRIBUTE_ID_ATTRIBUTE28 = 27,
            // id for "Attribute28" attribute
        ATTRIBUTE_ID_ATTRIBUTE29 = 28,
            // id for "Attribute29" attribute
        ATTRIBUTE_ID_ATTRIBUTE30 = 29,
            // id for "Attribute30" attribute
        ATTRIBUTE_ID_ATTRIBUTE31 = 30,
            // id for "Attribute31" attribute
        ATTRIBUTE_ID_ATTRIBUTE32 = 31,
            // id for "Attribute32" attribute
        ATTRIBUTE_ID_ATTRIBUTE33 = 32,
            // id for "Attribute33" attribute
        ATTRIBUTE_ID_ATTRIBUTE34 = 33,
            // id for "Attribute34" attribute
        ATTRIBUTE_ID_ATTRIBUTE35 = 34,
            // id for "Attribute35" attribute
        ATTRIBUTE_ID_ATTRIBUTE36 = 35,
            // id for "Attribute36" attribute
        ATTRIBUTE_ID_ATTRIBUTE37 = 36,
            // id for "Attribute37" attribute
        ATTRIBUTE_ID_ATTRIBUTE38 = 37,
            // id for "Attribute38" attribute
        ATTRIBUTE_ID_ATTRIBUTE39 = 38,
            // id for "Attribute39" attribute
        ATTRIBUTE_ID_ATTRIBUTE40 = 39
            // id for "Attribute40" attribute
    };
  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequenceWithManyNullables")
    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute
  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.
    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.
    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.
    // CREATORS
    MySequenceWithManyNullables();
        // Create an object of type 'MySequenceWithManyNullables' having the
        // default value.
    MySequenceWithManyNullables(const MySequenceWithManyNullables& original);
        // Create an object of type 'MySequenceWithManyNullables' having the
        // value of the specified 'original' object.
    ~MySequenceWithManyNullables();
        // Destroy this object.
    // MANIPULATORS
    MySequenceWithManyNullables& operator=(
                                       const MySequenceWithManyNullables& rhs);
        // Assign to this object the value of the specified 'rhs' object.
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).
    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.
    bdeut_NullableValue<int>& attribute01();
        // Return a reference to the modifiable "Attribute01" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute02();
        // Return a reference to the modifiable "Attribute02" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute03();
        // Return a reference to the modifiable "Attribute03" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute04();
        // Return a reference to the modifiable "Attribute04" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute05();
        // Return a reference to the modifiable "Attribute05" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute06();
        // Return a reference to the modifiable "Attribute06" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute07();
        // Return a reference to the modifiable "Attribute07" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute08();
        // Return a reference to the modifiable "Attribute08" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute09();
        // Return a reference to the modifiable "Attribute09" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute10();
        // Return a reference to the modifiable "Attribute10" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute11();
        // Return a reference to the modifiable "Attribute11" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute12();
        // Return a reference to the modifiable "Attribute12" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute13();
        // Return a reference to the modifiable "Attribute13" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute14();
        // Return a reference to the modifiable "Attribute14" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute15();
        // Return a reference to the modifiable "Attribute15" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute16();
        // Return a reference to the modifiable "Attribute16" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute17();
        // Return a reference to the modifiable "Attribute17" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute18();
        // Return a reference to the modifiable "Attribute18" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute19();
        // Return a reference to the modifiable "Attribute19" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute20();
        // Return a reference to the modifiable "Attribute20" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute21();
        // Return a reference to the modifiable "Attribute21" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute22();
        // Return a reference to the modifiable "Attribute22" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute23();
        // Return a reference to the modifiable "Attribute23" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute24();
        // Return a reference to the modifiable "Attribute24" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute25();
        // Return a reference to the modifiable "Attribute25" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute26();
        // Return a reference to the modifiable "Attribute26" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute27();
        // Return a reference to the modifiable "Attribute27" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute28();
        // Return a reference to the modifiable "Attribute28" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute29();
        // Return a reference to the modifiable "Attribute29" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute30();
        // Return a reference to the modifiable "Attribute30" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute31();
        // Return a reference to the modifiable "Attribute31" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute32();
        // Return a reference to the modifiable "Attribute32" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute33();
        // Return a reference to the modifiable "Attribute33" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute34();
        // Return a reference to the modifiable "Attribute34" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute35();
        // Return a reference to the modifiable "Attribute35" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute36();
        // Return a reference to the modifiable "Attribute36" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute37();
        // Return a reference to the modifiable "Attribute37" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute38();
        // Return a reference to the modifiable "Attribute38" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute39();
        // Return a reference to the modifiable "Attribute39" attribute of
        // this object.
    bdeut_NullableValue<int>& attribute40();
        // Return a reference to the modifiable "Attribute40" attribute of
        // this object.
    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.
    const bdeut_NullableValue<int>& attribute01() const;
        // Return a reference to the non-modifiable "Attribute01"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute02() const;
        // Return a reference to the non-modifiable "Attribute02"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute03() const;
        // Return a reference to the non-modifiable "Attribute03"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute04() const;
        // Return a reference to the non-modifiable "Attribute04"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute05() const;
        // Return a reference to the non-modifiable "Attribute05"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute06() const;
        // Return a reference to the non-modifiable "Attribute06"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute07() const;
        // Return a reference to the non-modifiable "Attribute07"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute08() const;
        // Return a reference to the non-modifiable "Attribute08"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute09() const;
        // Return a reference to the non-modifiable "Attribute09"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute10() const;
        // Return a reference to the non-modifiable "Attribute10"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute11() const;
        // Return a reference to the non-modifiable "Attribute11"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute12() const;
        // Return a reference to the non-modifiable "Attribute12"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute13() const;
        // Return a reference to the non-modifiable "Attribute13"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute14() const;
        // Return a reference to the non-modifiable "Attribute14"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute15() const;
        // Return a reference to the non-modifiable "Attribute15"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute16() const;
        // Return a reference to the non-modifiable "Attribute16"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute17() const;
        // Return a reference to the non-modifiable "Attribute17"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute18() const;
        // Return a reference to the non-modifiable "Attribute18"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute19() const;
        // Return a reference to the non-modifiable "Attribute19"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute20() const;
        // Return a reference to the non-modifiable "Attribute20"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute21() const;
        // Return a reference to the non-modifiable "Attribute21"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute22() const;
        // Return a reference to the non-modifiable "Attribute22"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute23() const;
        // Return a reference to the non-modifiable "Attribute23"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute24() const;
        // Return a reference to the non-modifiable "Attribute24"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute25() const;
        // Return a reference to the non-modifiable "Attribute25"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute26() const;
        // Return a reference to the non-modifiable "Attribute26"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute27() const;
        // Return a reference to the non-modifiable "Attribute27"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute28() const;
        // Return a reference to the non-modifiable "Attribute28"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute29() const;
        // Return a reference to the non-modifiable "Attribute29"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute30() const;
        // Return a reference to the non-modifiable "Attribute30"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute31() const;
        // Return a reference to the non-modifiable "Attribute31"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute32() const;
        // Return a reference to the non-modifiable "Attribute32"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute33() const;
        // Return a reference to the non-modifiable "Attribute33"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute34() const;
        // Return a reference to the non-modifiable "Attribute34"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute35() const;
        // Return a reference to the non-modifiable "Attribute35"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute36() const;
        // Return a reference to the non-modifiable "Attribute36"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute37() const;
        // Return a reference to the non-modifiable "Attribute37"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute38() const;
        // Return a reference to the non-modifiable "Attribute38"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute39() const;
        // Return a reference to the non-modifiable "Attribute39"
        // attribute of this object.
    const bdeut_NullableValue<int>& attribute40() const;
        // Return a reference to the non-modifiable "Attribute40"
        // attribute of this object.
};
// FREE OPERATORS
inline
bool operator==(const MySequenceWithManyNullables&  lhs,
                const MySequenceWithManyNullables&  rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.
inline
bool operator!=(const MySequenceWithManyNullables&  lhs,
                const MySequenceWithManyNullables&  rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.
inline
bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const MySequenceWithManyNullables&  rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.
// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================
// The following inlined functions are invoked from other inline functions.
inline
int MySequenceWithManyNullables::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}
// ---------------------------------------------------------------------------
// CREATORS
inline
MySequenceWithManyNullables::MySequenceWithManyNullables()
: d_attribute01()
, d_attribute02()
, d_attribute03()
, d_attribute04()
, d_attribute05()
, d_attribute06()
, d_attribute07()
, d_attribute08()
, d_attribute09()
, d_attribute10()
, d_attribute11()
, d_attribute12()
, d_attribute13()
, d_attribute14()
, d_attribute15()
, d_attribute16()
, d_attribute17()
, d_attribute18()
, d_attribute19()
, d_attribute20()
, d_attribute21()
, d_attribute22()
, d_attribute23()
, d_attribute24()
, d_attribute25()
, d_attribute26()
, d_attribute27()
, d_attribute28()
, d_attribute29()
, d_attribute30()
, d_attribute31()
, d_attribute32()
, d_attribute33()
, d_attribute34()
, d_attribute35()
, d_attribute36()
, d_attribute37()
, d_attribute38()
, d_attribute39()
, d_attribute40()
{
}
inline
MySequenceWithManyNullables::MySequenceWithManyNullables(
                                   const MySequenceWithManyNullables& original)
: d_attribute01(original.d_attribute01)
, d_attribute02(original.d_attribute02)
, d_attribute03(original.d_attribute03)
, d_attribute04(original.d_attribute04)
, d_attribute05(original.d_attribute05)
, d_attribute06(original.d_attribute06)
, d_attribute07(original.d_attribute07)
, d_attribute08(original.d_attribute08)
, d_attribute09(original.d_attribute09)
, d_attribute10(original.d_attribute10)
, d_attribute11(original.d_attribute11)
, d_attribute12(original.d_attribute12)
, d_attribute13(original.d_attribute13)
, d_attribute14(original.d_attribute14)
, d_attribute15(original.d_attribute15)
, d_attribute16(original.d_attribute16)
, d_attribute17(original.d_attribute17)
, d_attribute18(original.d_attribute18)
, d_attribute19(original.d_attribute19)
, d_attribute20(original.d_attribute20)
, d_attribute21(original.d_attribute21)
, d_attribute22(original.d_attribute22)
, d_attribute23(original.d_attribute23)
, d_attribute24(original.d_attribute24)
, d_attribute25(original.d_attribute25)
, d_attribute26(original.d_attribute26)
, d_attribute27(original.d_attribute27)
, d_attribute28(original.d_attribute28)
, d_attribute29(original.d_attribute29)
, d_attribute30(original.d_attribute30)
, d_attribute31(original.d_attribute31)
, d_attribute32(original.d_attribute32)
, d_attribute33(original.d_attribute33)
, d_attribute34(original.d_attribute34)
, d_attribute35(original.d_attribute35)
, d_attribute36(original.d_attribute36)
, d_attribute37(original.d_attribute37)
, d_attribute38(original.d_attribute38)
, d_attribute39(original.d_attribute39)
, d_attribute40(original.d_attribute40)
{
}
inline
MySequenceWithManyNullables::~MySequenceWithManyNullables()
{
}
// MANIPULATORS
inline
MySequenceWithManyNullables&
MySequenceWithManyNullables::operator=(const MySequenceWithManyNullables& rhs)
{
    if (this != &rhs) {
        d_attribute01 = rhs.d_attribute01;
        d_attribute02 = rhs.d_attribute02;
        d_attribute03 = rhs.d_attribute03;
        d_attribute04 = rhs.d_attribute04;
        d_attribute05 = rhs.d_attribute05;
        d_attribute06 = rhs.d_attribute06;
        d_attribute07 = rhs.d_attribute07;
        d_attribute08 = rhs.d_attribute08;
        d_attribute09 = rhs.d_attribute09;
        d_attribute10 = rhs.d_attribute10;
        d_attribute11 = rhs.d_attribute11;
        d_attribute12 = rhs.d_attribute12;
        d_attribute13 = rhs.d_attribute13;
        d_attribute14 = rhs.d_attribute14;
        d_attribute15 = rhs.d_attribute15;
        d_attribute16 = rhs.d_attribute16;
        d_attribute17 = rhs.d_attribute17;
        d_attribute18 = rhs.d_attribute18;
        d_attribute19 = rhs.d_attribute19;
        d_attribute20 = rhs.d_attribute20;
        d_attribute21 = rhs.d_attribute21;
        d_attribute22 = rhs.d_attribute22;
        d_attribute23 = rhs.d_attribute23;
        d_attribute24 = rhs.d_attribute24;
        d_attribute25 = rhs.d_attribute25;
        d_attribute26 = rhs.d_attribute26;
        d_attribute27 = rhs.d_attribute27;
        d_attribute28 = rhs.d_attribute28;
        d_attribute29 = rhs.d_attribute29;
        d_attribute30 = rhs.d_attribute30;
        d_attribute31 = rhs.d_attribute31;
        d_attribute32 = rhs.d_attribute32;
        d_attribute33 = rhs.d_attribute33;
        d_attribute34 = rhs.d_attribute34;
        d_attribute35 = rhs.d_attribute35;
        d_attribute36 = rhs.d_attribute36;
        d_attribute37 = rhs.d_attribute37;
        d_attribute38 = rhs.d_attribute38;
        d_attribute39 = rhs.d_attribute39;
        d_attribute40 = rhs.d_attribute40;
    }
    return *this;
}
template <class STREAM>
inline
STREAM& MySequenceWithManyNullables::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            streamIn(
                  stream, d_attribute01, 1);
            streamIn(
                  stream, d_attribute02, 1);
            streamIn(
                  stream, d_attribute03, 1);
            streamIn(
                  stream, d_attribute04, 1);
            streamIn(
                  stream, d_attribute05, 1);
            streamIn(
                  stream, d_attribute06, 1);
            streamIn(
                  stream, d_attribute07, 1);
            streamIn(
                  stream, d_attribute08, 1);
            streamIn(
                  stream, d_attribute09, 1);
            streamIn(
                  stream, d_attribute10, 1);
            streamIn(
                  stream, d_attribute11, 1);
            streamIn(
                  stream, d_attribute12, 1);
            streamIn(
                  stream, d_attribute13, 1);
            streamIn(
                  stream, d_attribute14, 1);
            streamIn(
                  stream, d_attribute15, 1);
            streamIn(
                  stream, d_attribute16, 1);
            streamIn(
                  stream, d_attribute17, 1);
            streamIn(
                  stream, d_attribute18, 1);
            streamIn(
                  stream, d_attribute19, 1);
            streamIn(
                  stream, d_attribute20, 1);
            streamIn(
                  stream, d_attribute21, 1);
            streamIn(
                  stream, d_attribute22, 1);
            streamIn(
                  stream, d_attribute23, 1);
            streamIn(
                  stream, d_attribute24, 1);
            streamIn(
                  stream, d_attribute25, 1);
            streamIn(
                  stream, d_attribute26, 1);
            streamIn(
                  stream, d_attribute27, 1);
            streamIn(
                  stream, d_attribute28, 1);
            streamIn(
                  stream, d_attribute29, 1);
            streamIn(
                  stream, d_attribute30, 1);
            streamIn(
                  stream, d_attribute31, 1);
            streamIn(
                  stream, d_attribute32, 1);
            streamIn(
                  stream, d_attribute33, 1);
            streamIn(
                  stream, d_attribute34, 1);
            streamIn(
                  stream, d_attribute35, 1);
            streamIn(
                  stream, d_attribute36, 1);
            streamIn(
                  stream, d_attribute37, 1);
            streamIn(
                  stream, d_attribute38, 1);
            streamIn(
                  stream, d_attribute39, 1);
            streamIn(
                  stream, d_attribute40, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}
inline
void MySequenceWithManyNullables::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute01);
    bdeat_ValueTypeFunctions::reset(&d_attribute02);
    bdeat_ValueTypeFunctions::reset(&d_attribute03);
    bdeat_ValueTypeFunctions::reset(&d_attribute04);
    bdeat_ValueTypeFunctions::reset(&d_attribute05);
    bdeat_ValueTypeFunctions::reset(&d_attribute06);
    bdeat_ValueTypeFunctions::reset(&d_attribute07);
    bdeat_ValueTypeFunctions::reset(&d_attribute08);
    bdeat_ValueTypeFunctions::reset(&d_attribute09);
    bdeat_ValueTypeFunctions::reset(&d_attribute10);
    bdeat_ValueTypeFunctions::reset(&d_attribute11);
    bdeat_ValueTypeFunctions::reset(&d_attribute12);
    bdeat_ValueTypeFunctions::reset(&d_attribute13);
    bdeat_ValueTypeFunctions::reset(&d_attribute14);
    bdeat_ValueTypeFunctions::reset(&d_attribute15);
    bdeat_ValueTypeFunctions::reset(&d_attribute16);
    bdeat_ValueTypeFunctions::reset(&d_attribute17);
    bdeat_ValueTypeFunctions::reset(&d_attribute18);
    bdeat_ValueTypeFunctions::reset(&d_attribute19);
    bdeat_ValueTypeFunctions::reset(&d_attribute20);
    bdeat_ValueTypeFunctions::reset(&d_attribute21);
    bdeat_ValueTypeFunctions::reset(&d_attribute22);
    bdeat_ValueTypeFunctions::reset(&d_attribute23);
    bdeat_ValueTypeFunctions::reset(&d_attribute24);
    bdeat_ValueTypeFunctions::reset(&d_attribute25);
    bdeat_ValueTypeFunctions::reset(&d_attribute26);
    bdeat_ValueTypeFunctions::reset(&d_attribute27);
    bdeat_ValueTypeFunctions::reset(&d_attribute28);
    bdeat_ValueTypeFunctions::reset(&d_attribute29);
    bdeat_ValueTypeFunctions::reset(&d_attribute30);
    bdeat_ValueTypeFunctions::reset(&d_attribute31);
    bdeat_ValueTypeFunctions::reset(&d_attribute32);
    bdeat_ValueTypeFunctions::reset(&d_attribute33);
    bdeat_ValueTypeFunctions::reset(&d_attribute34);
    bdeat_ValueTypeFunctions::reset(&d_attribute35);
    bdeat_ValueTypeFunctions::reset(&d_attribute36);
    bdeat_ValueTypeFunctions::reset(&d_attribute37);
    bdeat_ValueTypeFunctions::reset(&d_attribute38);
    bdeat_ValueTypeFunctions::reset(&d_attribute39);
    bdeat_ValueTypeFunctions::reset(&d_attribute40);
}
template <class MANIPULATOR>
inline
int MySequenceWithManyNullables::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;
    ret = manipulator(&d_attribute01,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE01]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute02,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE02]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute03,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE03]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute04,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE04]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute05,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE05]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute06,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE06]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute07,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE07]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute08,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE08]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute09,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE09]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute10,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE10]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute11,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE11]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute12,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE12]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute13,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE13]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute14,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE14]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute15,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE15]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute16,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE16]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute17,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE17]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute18,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE18]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute19,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE19]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute20,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE20]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute21,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE21]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute22,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE22]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute23,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE23]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute24,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE24]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute25,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE25]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute26,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE26]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute27,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE27]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute28,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE28]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute29,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE29]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute30,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE30]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute31,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE31]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute32,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE32]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute33,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE33]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute34,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE34]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute35,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE35]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute36,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE36]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute37,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE37]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute38,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE38]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute39,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE39]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_attribute40,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE40]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    return ret;
}
template <class MANIPULATOR>
inline
int MySequenceWithManyNullables::manipulateAttribute(MANIPULATOR&  manipulator,
                                                     int           id)
{
    enum { NOT_FOUND = -1 };
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE01: {
        return manipulator(&d_attribute01,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE01]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE02: {
        return manipulator(&d_attribute02,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE02]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE03: {
        return manipulator(&d_attribute03,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE03]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE04: {
        return manipulator(&d_attribute04,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE04]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE05: {
        return manipulator(&d_attribute05,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE05]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE06: {
        return manipulator(&d_attribute06,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE06]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE07: {
        return manipulator(&d_attribute07,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE07]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE08: {
        return manipulator(&d_attribute08,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE08]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE09: {
        return manipulator(&d_attribute09,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE09]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE10: {
        return manipulator(&d_attribute10,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE10]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE11: {
        return manipulator(&d_attribute11,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE11]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE12: {
        return manipulator(&d_attribute12,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE12]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE13: {
        return manipulator(&d_attribute13,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE13]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE14: {
        return manipulator(&d_attribute14,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE14]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE15: {
        return manipulator(&d_attribute15,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE15]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE16: {
        return manipulator(&d_attribute16,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE16]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE17: {
        return manipulator(&d_attribute17,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE17]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE18: {
        return manipulator(&d_attribute18,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE18]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE19: {
        return manipulator(&d_attribute19,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE19]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE20: {
        return manipulator(&d_attribute20,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE20]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE21: {
        return manipulator(&d_attribute21,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE21]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE22: {
        return manipulator(&d_attribute22,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE22]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE23: {
        return manipulator(&d_attribute23,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE23]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE24: {
        return manipulator(&d_attribute24,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE24]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE25: {
        return manipulator(&d_attribute25,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE25]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE26: {
        return manipulator(&d_attribute26,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE26]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE27: {
        return manipulator(&d_attribute27,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE27]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE28: {
        return manipulator(&d_attribute28,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE28]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE29: {
        return manipulator(&d_attribute29,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE29]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE30: {
        return manipulator(&d_attribute30,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE30]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE31: {
        return manipulator(&d_attribute31,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE31]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE32: {
        return manipulator(&d_attribute32,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE32]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE33: {
        return manipulator(&d_attribute33,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE33]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE34: {
        return manipulator(&d_attribute34,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE34]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE35: {
        return manipulator(&d_attribute35,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE35]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE36: {
        return manipulator(&d_attribute36,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE36]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE37: {
        return manipulator(&d_attribute37,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE37]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE38: {
        return manipulator(&d_attribute38,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE38]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE39: {
        return manipulator(&d_attribute39,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE39]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE40: {
        return manipulator(&d_attribute40,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE40]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}
template <class MANIPULATOR>
inline
int MySequenceWithManyNullables::manipulateAttribute(MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };
    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
    }
    return manipulateAttribute(manipulator, attributeInfo->d_id);
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute01()
{
    return d_attribute01;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute02()
{
    return d_attribute02;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute03()
{
    return d_attribute03;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute04()
{
    return d_attribute04;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute05()
{
    return d_attribute05;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute06()
{
    return d_attribute06;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute07()
{
    return d_attribute07;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute08()
{
    return d_attribute08;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute09()
{
    return d_attribute09;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute10()
{
    return d_attribute10;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute11()
{
    return d_attribute11;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute12()
{
    return d_attribute12;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute13()
{
    return d_attribute13;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute14()
{
    return d_attribute14;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute15()
{
    return d_attribute15;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute16()
{
    return d_attribute16;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute17()
{
    return d_attribute17;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute18()
{
    return d_attribute18;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute19()
{
    return d_attribute19;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute20()
{
    return d_attribute20;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute21()
{
    return d_attribute21;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute22()
{
    return d_attribute22;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute23()
{
    return d_attribute23;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute24()
{
    return d_attribute24;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute25()
{
    return d_attribute25;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute26()
{
    return d_attribute26;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute27()
{
    return d_attribute27;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute28()
{
    return d_attribute28;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute29()
{
    return d_attribute29;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute30()
{
    return d_attribute30;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute31()
{
    return d_attribute31;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute32()
{
    return d_attribute32;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute33()
{
    return d_attribute33;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute34()
{
    return d_attribute34;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute35()
{
    return d_attribute35;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute36()
{
    return d_attribute36;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute37()
{
    return d_attribute37;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute38()
{
    return d_attribute38;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute39()
{
    return d_attribute39;
}
inline
bdeut_NullableValue<int>& MySequenceWithManyNullables::attribute40()
{
    return d_attribute40;
}
// ACCESSORS
template <class STREAM>
inline
STREAM& MySequenceWithManyNullables::bdexStreamOut(STREAM&  stream,
                                                   int      version) const
{
    switch (version) {
      case 1: {
        streamOut(stream, d_attribute01, 1);
        streamOut(stream, d_attribute02, 1);
        streamOut(stream, d_attribute03, 1);
        streamOut(stream, d_attribute04, 1);
        streamOut(stream, d_attribute05, 1);
        streamOut(stream, d_attribute06, 1);
        streamOut(stream, d_attribute07, 1);
        streamOut(stream, d_attribute08, 1);
        streamOut(stream, d_attribute09, 1);
        streamOut(stream, d_attribute10, 1);
        streamOut(stream, d_attribute11, 1);
        streamOut(stream, d_attribute12, 1);
        streamOut(stream, d_attribute13, 1);
        streamOut(stream, d_attribute14, 1);
        streamOut(stream, d_attribute15, 1);
        streamOut(stream, d_attribute16, 1);
        streamOut(stream, d_attribute17, 1);
        streamOut(stream, d_attribute18, 1);
        streamOut(stream, d_attribute19, 1);
        streamOut(stream, d_attribute20, 1);
        streamOut(stream, d_attribute21, 1);
        streamOut(stream, d_attribute22, 1);
        streamOut(stream, d_attribute23, 1);
        streamOut(stream, d_attribute24, 1);
        streamOut(stream, d_attribute25, 1);
        streamOut(stream, d_attribute26, 1);
        streamOut(stream, d_attribute27, 1);
        streamOut(stream, d_attribute28, 1);
        streamOut(stream, d_attribute29, 1);
        streamOut(stream, d_attribute30, 1);
        streamOut(stream, d_attribute31, 1);
        streamOut(stream, d_attribute32, 1);
        streamOut(stream, d_attribute33, 1);
        streamOut(stream, d_attribute34, 1);
        streamOut(stream, d_attribute35, 1);
        streamOut(stream, d_attribute36, 1);
        streamOut(stream, d_attribute37, 1);
        streamOut(stream, d_attribute38, 1);
        streamOut(stream, d_attribute39, 1);
        streamOut(stream, d_attribute40, 1);
      } break;
    }
    return stream;
}
template <class ACCESSOR>
inline
int MySequenceWithManyNullables::accessAttributes(ACCESSOR& accessor) const
{
    int ret;
    ret = accessor(d_attribute01,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE01]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute02,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE02]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute03,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE03]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute04,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE04]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute05,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE05]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute06,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE06]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute07,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE07]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute08,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE08]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute09,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE09]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute10,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE10]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute11,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE11]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute12,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE12]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute13,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE13]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute14,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE14]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute15,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE15]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute16,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE16]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute17,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE17]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute18,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE18]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute19,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE19]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute20,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE20]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute21,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE21]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute22,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE22]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute23,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE23]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute24,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE24]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute25,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE25]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute26,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE26]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute27,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE27]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute28,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE28]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute29,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE29]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute30,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE30]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute31,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE31]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute32,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE32]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute33,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE33]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute34,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE34]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute35,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE35]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute36,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE36]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute37,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE37]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute38,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE38]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute39,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE39]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_attribute40,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE40]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    return ret;
}
template <class ACCESSOR>
inline
int MySequenceWithManyNullables::accessAttribute(ACCESSOR&  accessor,
                                                 int        id) const
{
    enum { NOT_FOUND = -1 };
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE01: {
        return accessor(d_attribute01,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE01]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE02: {
        return accessor(d_attribute02,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE02]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE03: {
        return accessor(d_attribute03,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE03]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE04: {
        return accessor(d_attribute04,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE04]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE05: {
        return accessor(d_attribute05,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE05]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE06: {
        return accessor(d_attribute06,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE06]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE07: {
        return accessor(d_attribute07,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE07]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE08: {
        return accessor(d_attribute08,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE08]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE09: {
        return accessor(d_attribute09,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE09]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE10: {
        return accessor(d_attribute10,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE10]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE11: {
        return accessor(d_attribute11,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE11]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE12: {
        return accessor(d_attribute12,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE12]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE13: {
        return accessor(d_attribute13,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE13]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE14: {
        return accessor(d_attribute14,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE14]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE15: {
        return accessor(d_attribute15,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE15]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE16: {
        return accessor(d_attribute16,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE16]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE17: {
        return accessor(d_attribute17,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE17]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE18: {
        return accessor(d_attribute18,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE18]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE19: {
        return accessor(d_attribute19,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE19]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE20: {
        return accessor(d_attribute20,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE20]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE21: {
        return accessor(d_attribute21,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE21]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE22: {
        return accessor(d_attribute22,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE22]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE23: {
        return accessor(d_attribute23,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE23]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE24: {
        return accessor(d_attribute24,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE24]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE25: {
        return accessor(d_attribute25,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE25]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE26: {
        return accessor(d_attribute26,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE26]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE27: {
        return accessor(d_attribute27,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE27]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE28: {
        return accessor(d_attribute28,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE28]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE29: {
        return accessor(d_attribute29,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE29]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE30: {
        return accessor(d_attribute30,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE30]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE31: {
        return accessor(d_attribute31,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE31]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE32: {
        return accessor(d_attribute32,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE32]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE33: {
        return accessor(d_attribute33,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE33]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE34: {
        return accessor(d_attribute34,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE34]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE35: {
        return accessor(d_attribute35,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE35]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE36: {
        return accessor(d_attribute36,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE36]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE37: {
        return accessor(d_attribute37,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE37]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE38: {
        return accessor(d_attribute38,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE38]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE39: {
        return accessor(d_attribute39,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE39]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE40: {
        return accessor(d_attribute40,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE40]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}
template <class ACCESSOR>
inline
int MySequenceWithManyNullables::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };
     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
     }
     return accessAttribute(accessor, attributeInfo->d_id);
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute01() const
{
    return d_attribute01;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute02() const
{
    return d_attribute02;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute03() const
{
    return d_attribute03;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute04() const
{
    return d_attribute04;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute05() const
{
    return d_attribute05;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute06() const
{
    return d_attribute06;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute07() const
{
    return d_attribute07;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute08() const
{
    return d_attribute08;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute09() const
{
    return d_attribute09;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute10() const
{
    return d_attribute10;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute11() const
{
    return d_attribute11;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute12() const
{
    return d_attribute12;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute13() const
{
    return d_attribute13;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute14() const
{
    return d_attribute14;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute15() const
{
    return d_attribute15;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute16() const
{
    return d_attribute16;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute17() const
{
    return d_attribute17;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute18() const
{
    return d_attribute18;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute19() const
{
    return d_attribute19;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute20() const
{
    return d_attribute20;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute21() const
{
    return d_attribute21;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute22() const
{
    return d_attribute22;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute23() const
{
    return d_attribute23;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute24() const
{
    return d_attribute24;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute25() const
{
    return d_attribute25;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute26() const
{
    return d_attribute26;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute27() const
{
    return d_attribute27;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute28() const
{
    return d_attribute28;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute29() const
{
    return d_attribute29;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute30() const
{
    return d_attribute30;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute31() const
{
    return d_attribute31;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute32() const
{
    return d_attribute32;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute33() const
{
    return d_attribute33;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute34() const
{
    return d_attribute34;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute35() const
{
    return d_attribute35;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute36() const
{
    return d_attribute36;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute37() const
{
    return d_attribute37;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute38() const
{
    return d_attribute38;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute39() const
{
    return d_attribute39;
}
inline
const bdeut_NullableValue<int>&
                               MySequenceWithManyNullables::attribute40() const
{
    return d_attribute40;
}
}  // close namespace test;
// TRAITS
BDEAT_DECL_SEQUENCE_TRAITS(test::MySequenceWithManyNullables)
// FREE OPERATORS
inline
bool test::operator==(const test::MySequenceWithManyNullables&  lhs,
                      const test::MySequenceWithManyNullables&  rhs)
{
    return  lhs.attribute01() == rhs.attribute01()
         && lhs.attribute02() == rhs.attribute02()
         && lhs.attribute03() == rhs.attribute03()
         && lhs.attribute04() == rhs.attribute04()
         && lhs.attribute05() == rhs.attribute05()
         && lhs.attribute06() == rhs.attribute06()
         && lhs.attribute07() == rhs.attribute07()
         && lhs.attribute08() == rhs.attribute08()
         && lhs.attribute09() == rhs.attribute09()
         && lhs.attribute10() == rhs.attribute10()
         && lhs.attribute11() == rhs.attribute11()
         && lhs.attribute12() == rhs.attribute12()
         && lhs.attribute13() == rhs.attribute13()
         && lhs.attribute14() == rhs.attribute14()
         && lhs.attribute15() == rhs.attribute15()
         && lhs.attribute16() == rhs.attribute16()
         && lhs.attribute17() == rhs.attribute17()
         && lhs.attribute18() == rhs.attribute18()
         && lhs.attribute19() == rhs.attribute19()
         && lhs.attribute20() == rhs.attribute20()
         && lhs.attribute21() == rhs.attribute21()
         && lhs.attribute22() == rhs.attribute22()
         && lhs.attribute23() == rhs.attribute23()
         && lhs.attribute24() == rhs.attribute24()
         && lhs.attribute25() == rhs.attribute25()
         && lhs.attribute26() == rhs.attribute26()
         && lhs.attribute27() == rhs.attribute27()
         && lhs.attribute28() == rhs.attribute28()
         && lhs.attribute29() == rhs.attribute29()
         && lhs.attribute30() == rhs.attribute30()
         && lhs.attribute31() == rhs.attribute31()
         && lhs.attribute32() == rhs.attribute32()
         && lhs.attribute33() == rhs.attribute33()
         && lhs.attribute34() == rhs.attribute34()
         && lhs.attribute35() == rhs.attribute35()
         && lhs.attribute36() == rhs.attribute36()
         && lhs.attribute37() == rhs.attribute37()
         && lhs.attribute38() == rhs.attribute38()
         && lhs.attribute39() == rhs.attribute39()
         && lhs.attribute40() == rhs.attribute40();
}
inline
bool test::operator!=(const test::MySequenceWithManyNullables&  lhs,
                      const test::MySequenceWithManyNullables&  rhs)
{
    return  lhs.attribute01() != rhs.attribute01()
         || lhs.attribute02() != rhs.attribute02()
         || lhs.attribute03() != rhs.attribute03()
         || lhs.attribute04() != rhs.attribute04()
         || lhs.attribute05() != rhs.attribute05()
         || lhs.attribute06() != rhs.attribute06()
         || lhs.attribute07() != rhs.attribute07()
         || lhs.attribute08() != rhs.attribute08()
         || lhs.attribute09() != rhs.attribute09()
         || lhs.attribute10() != rhs.attribute10()
         || lhs.attribute11() != rhs.attribute11()
         || lhs.attribute12() != rhs.attribute12()
         || lhs.attribute13() != rhs.attribute13()
         || lhs.attribute14() != rhs.attribute14()
         || lhs.attribute15() != rhs.attribute15()
         || lhs.attribute16() != rhs.attribute16()
         || lhs.attribute17() != rhs.attribute17()
         || lhs.attribute18() != rhs.attribute18()
         || lhs.attribute19() != rhs.attribute19()
         || lhs.attribute20() != rhs.attribute20()
         || lhs.attribute21() != rhs.attribute21()
         || lhs.attribute22() != rhs.attribute22()
         || lhs.attribute23() != rhs.attribute23()
         || lhs.attribute24() != rhs.attribute24()
         || lhs.attribute25() != rhs.attribute25()
         || lhs.attribute26() != rhs.attribute26()
         || lhs.attribute27() != rhs.attribute27()
         || lhs.attribute28() != rhs.attribute28()
         || lhs.attribute29() != rhs.attribute29()
         || lhs.attribute30() != rhs.attribute30()
         || lhs.attribute31() != rhs.attribute31()
         || lhs.attribute32() != rhs.attribute32()
         || lhs.attribute33() != rhs.attribute33()
         || lhs.attribute34() != rhs.attribute34()
         || lhs.attribute35() != rhs.attribute35()
         || lhs.attribute36() != rhs.attribute36()
         || lhs.attribute37() != rhs.attribute37()
         || lhs.attribute38() != rhs.attribute38()
         || lhs.attribute39() != rhs.attribute39()
         || lhs.attribute40() != rhs.attribute40();
}
inline
bsl::ostream& test::operator<<(
                              bsl::ostream&                             stream,
                              const test::MySequenceWithManyNullables&  rhs)
{
    return rhs.print(stream, 0, -1);
}
}  // close namespace BloombergLP;
#endif
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_mysequencewithmanynullables.cpp  -*-C++-*-
#include <bsl_iostream.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>
#include <bdeut_nullablevalue.h>
namespace BloombergLP {
namespace test {
                               // ---------
                               // CONSTANTS
                               // ---------
const char MySequenceWithManyNullables::CLASS_NAME[]
                                               = "MySequenceWithManyNullables";
    // the name of this class
const bdeat_AttributeInfo
  MySequenceWithManyNullables::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE01,
        "Attribute01",                     // name
        sizeof("Attribute01") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE02,
        "Attribute02",                     // name
        sizeof("Attribute02") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE03,
        "Attribute03",                     // name
        sizeof("Attribute03") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE04,
        "Attribute04",                     // name
        sizeof("Attribute04") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE05,
        "Attribute05",                     // name
        sizeof("Attribute05") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE06,
        "Attribute06",                     // name
        sizeof("Attribute06") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE07,
        "Attribute07",                     // name
        sizeof("Attribute07") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE08,
        "Attribute08",                     // name
        sizeof("Attribute08") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE09,
        "Attribute09",                     // name
        sizeof("Attribute09") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE10,
        "Attribute10",                     // name
        sizeof("Attribute10") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE11,
        "Attribute11",                     // name
        sizeof("Attribute11") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE12,
        "Attribute12",                     // name
        sizeof("Attribute12") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE13,
        "Attribute13",                     // name
        sizeof("Attribute13") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE14,
        "Attribute14",                     // name
        sizeof("Attribute14") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE15,
        "Attribute15",                     // name
        sizeof("Attribute15") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE16,
        "Attribute16",                     // name
        sizeof("Attribute16") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE17,
        "Attribute17",                     // name
        sizeof("Attribute17") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE18,
        "Attribute18",                     // name
        sizeof("Attribute18") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE19,
        "Attribute19",                     // name
        sizeof("Attribute19") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE20,
        "Attribute20",                     // name
        sizeof("Attribute20") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE21,
        "Attribute21",                     // name
        sizeof("Attribute21") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE22,
        "Attribute22",                     // name
        sizeof("Attribute22") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE23,
        "Attribute23",                     // name
        sizeof("Attribute23") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE24,
        "Attribute24",                     // name
        sizeof("Attribute24") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE25,
        "Attribute25",                     // name
        sizeof("Attribute25") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE26,
        "Attribute26",                     // name
        sizeof("Attribute26") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE27,
        "Attribute27",                     // name
        sizeof("Attribute27") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE28,
        "Attribute28",                     // name
        sizeof("Attribute28") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE29,
        "Attribute29",                     // name
        sizeof("Attribute29") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE30,
        "Attribute30",                     // name
        sizeof("Attribute30") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE31,
        "Attribute31",                     // name
        sizeof("Attribute31") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE32,
        "Attribute32",                     // name
        sizeof("Attribute32") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE33,
        "Attribute33",                     // name
        sizeof("Attribute33") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE34,
        "Attribute34",                     // name
        sizeof("Attribute34") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE35,
        "Attribute35",                     // name
        sizeof("Attribute35") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE36,
        "Attribute36",                     // name
        sizeof("Attribute36") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE37,
        "Attribute37",                     // name
        sizeof("Attribute37") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE38,
        "Attribute38",                     // name
        sizeof("Attribute38") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE39,
        "Attribute39",                     // name
        sizeof("Attribute39") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE40,
        "Attribute40",                     // name
        sizeof("Attribute40") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    }
};
                               // -------------
                               // CLASS METHODS
                               // -------------
const bdeat_AttributeInfo *MySequenceWithManyNullables::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 11: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='R'
             && bdeu_CharType::toUpper(name[4])=='I'
             && bdeu_CharType::toUpper(name[5])=='B'
             && bdeu_CharType::toUpper(name[6])=='U'
             && bdeu_CharType::toUpper(name[7])=='T'
             && bdeu_CharType::toUpper(name[8])=='E') {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '0': {
                        switch(bsl::toupper(name[10])) {
                            case '1': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE01
                                                             ];
                            } break;
                            case '2': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE02
                                                             ];
                            } break;
                            case '3': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE03
                                                             ];
                            } break;
                            case '4': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE04
                                                             ];
                            } break;
                            case '5': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE05
                                                             ];
                            } break;
                            case '6': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE06
                                                             ];
                            } break;
                            case '7': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE07
                                                             ];
                            } break;
                            case '8': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE08
                                                             ];
                            } break;
                            case '9': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE09
                                                             ];
                            } break;
                        }
                    } break;
                    case '1': {
                        switch(bdeu_CharType::toUpper(name[10])) {
                            case '0': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE10
                                                             ];
                            } break;
                            case '1': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE11
                                                             ];
                            } break;
                            case '2': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE12
                                                             ];
                            } break;
                            case '3': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE13
                                                             ];
                            } break;
                            case '4': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE14
                                                             ];
                            } break;
                            case '5': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE15
                                                             ];
                            } break;
                            case '6': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE16
                                                             ];
                            } break;
                            case '7': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE17
                                                             ];
                            } break;
                            case '8': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE18
                                                             ];
                            } break;
                            case '9': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE19
                                                             ];
                            } break;
                        }
                    } break;
                    case '2': {
                        switch(bdeu_CharType::toUpper(name[10])) {
                            case '0': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE20
                                                             ];
                            } break;
                            case '1': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE21
                                                             ];
                            } break;
                            case '2': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE22
                                                             ];
                            } break;
                            case '3': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE23
                                                             ];
                            } break;
                            case '4': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE24
                                                             ];
                            } break;
                            case '5': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE25
                                                             ];
                            } break;
                            case '6': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE26
                                                             ];
                            } break;
                            case '7': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE27
                                                             ];
                            } break;
                            case '8': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE28
                                                             ];
                            } break;
                            case '9': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE29
                                                             ];
                            } break;
                        }
                    } break;
                    case '3': {
                        switch(bdeu_CharType::toUpper(name[10])) {
                            case '0': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE30
                                                             ];
                            } break;
                            case '1': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE31
                                                             ];
                            } break;
                            case '2': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE32
                                                             ];
                            } break;
                            case '3': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE33
                                                             ];
                            } break;
                            case '4': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE34
                                                             ];
                            } break;
                            case '5': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE35
                                                             ];
                            } break;
                            case '6': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE36
                                                             ];
                            } break;
                            case '7': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE37
                                                             ];
                            } break;
                            case '8': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE38
                                                             ];
                            } break;
                            case '9': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE39
                                                             ];
                            } break;
                        }
                    } break;
                    case '4': {
                        if (bsl::toupper(name[10])=='0') {
                            return &ATTRIBUTE_INFO_ARRAY[
                                                    ATTRIBUTE_INDEX_ATTRIBUTE40
                                                         ];
                        }
                    } break;
                }
            }
        } break;
    }
    return 0;
}
const bdeat_AttributeInfo *MySequenceWithManyNullables::lookupAttributeInfo(
                                                                       int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE01:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE01];
      case ATTRIBUTE_ID_ATTRIBUTE02:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE02];
      case ATTRIBUTE_ID_ATTRIBUTE03:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE03];
      case ATTRIBUTE_ID_ATTRIBUTE04:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE04];
      case ATTRIBUTE_ID_ATTRIBUTE05:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE05];
      case ATTRIBUTE_ID_ATTRIBUTE06:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE06];
      case ATTRIBUTE_ID_ATTRIBUTE07:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE07];
      case ATTRIBUTE_ID_ATTRIBUTE08:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE08];
      case ATTRIBUTE_ID_ATTRIBUTE09:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE09];
      case ATTRIBUTE_ID_ATTRIBUTE10:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE10];
      case ATTRIBUTE_ID_ATTRIBUTE11:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE11];
      case ATTRIBUTE_ID_ATTRIBUTE12:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE12];
      case ATTRIBUTE_ID_ATTRIBUTE13:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE13];
      case ATTRIBUTE_ID_ATTRIBUTE14:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE14];
      case ATTRIBUTE_ID_ATTRIBUTE15:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE15];
      case ATTRIBUTE_ID_ATTRIBUTE16:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE16];
      case ATTRIBUTE_ID_ATTRIBUTE17:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE17];
      case ATTRIBUTE_ID_ATTRIBUTE18:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE18];
      case ATTRIBUTE_ID_ATTRIBUTE19:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE19];
      case ATTRIBUTE_ID_ATTRIBUTE20:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE20];
      case ATTRIBUTE_ID_ATTRIBUTE21:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE21];
      case ATTRIBUTE_ID_ATTRIBUTE22:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE22];
      case ATTRIBUTE_ID_ATTRIBUTE23:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE23];
      case ATTRIBUTE_ID_ATTRIBUTE24:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE24];
      case ATTRIBUTE_ID_ATTRIBUTE25:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE25];
      case ATTRIBUTE_ID_ATTRIBUTE26:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE26];
      case ATTRIBUTE_ID_ATTRIBUTE27:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE27];
      case ATTRIBUTE_ID_ATTRIBUTE28:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE28];
      case ATTRIBUTE_ID_ATTRIBUTE29:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE29];
      case ATTRIBUTE_ID_ATTRIBUTE30:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE30];
      case ATTRIBUTE_ID_ATTRIBUTE31:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE31];
      case ATTRIBUTE_ID_ATTRIBUTE32:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE32];
      case ATTRIBUTE_ID_ATTRIBUTE33:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE33];
      case ATTRIBUTE_ID_ATTRIBUTE34:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE34];
      case ATTRIBUTE_ID_ATTRIBUTE35:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE35];
      case ATTRIBUTE_ID_ATTRIBUTE36:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE36];
      case ATTRIBUTE_ID_ATTRIBUTE37:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE37];
      case ATTRIBUTE_ID_ATTRIBUTE38:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE38];
      case ATTRIBUTE_ID_ATTRIBUTE39:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE39];
      case ATTRIBUTE_ID_ATTRIBUTE40:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE40];
      default:
        return 0;
    }
}
                                // --------
                                // CREATORS
                                // --------
                                // ------------
                                // MANIPULATORS
                                // ------------
                                // ---------
                                // ACCESSORS
                                // ---------
bsl::ostream& MySequenceWithManyNullables::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }
    int levelPlus1 = level + 1;
    if (0 <= spacesPerLevel) {
        // multiline
        stream << "[\n";
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute01 = ";
        bdeu_PrintMethods::print(stream, d_attribute01,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute02 = ";
        bdeu_PrintMethods::print(stream, d_attribute02,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute03 = ";
        bdeu_PrintMethods::print(stream, d_attribute03,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute04 = ";
        bdeu_PrintMethods::print(stream, d_attribute04,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute05 = ";
        bdeu_PrintMethods::print(stream, d_attribute05,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute06 = ";
        bdeu_PrintMethods::print(stream, d_attribute06,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute07 = ";
        bdeu_PrintMethods::print(stream, d_attribute07,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute08 = ";
        bdeu_PrintMethods::print(stream, d_attribute08,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute09 = ";
        bdeu_PrintMethods::print(stream, d_attribute09,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute10 = ";
        bdeu_PrintMethods::print(stream, d_attribute10,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute11 = ";
        bdeu_PrintMethods::print(stream, d_attribute11,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute12 = ";
        bdeu_PrintMethods::print(stream, d_attribute12,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute13 = ";
        bdeu_PrintMethods::print(stream, d_attribute13,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute14 = ";
        bdeu_PrintMethods::print(stream, d_attribute14,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute15 = ";
        bdeu_PrintMethods::print(stream, d_attribute15,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute16 = ";
        bdeu_PrintMethods::print(stream, d_attribute16,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute17 = ";
        bdeu_PrintMethods::print(stream, d_attribute17,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute18 = ";
        bdeu_PrintMethods::print(stream, d_attribute18,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute19 = ";
        bdeu_PrintMethods::print(stream, d_attribute19,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute20 = ";
        bdeu_PrintMethods::print(stream, d_attribute20,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute21 = ";
        bdeu_PrintMethods::print(stream, d_attribute21,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute22 = ";
        bdeu_PrintMethods::print(stream, d_attribute22,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute23 = ";
        bdeu_PrintMethods::print(stream, d_attribute23,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute24 = ";
        bdeu_PrintMethods::print(stream, d_attribute24,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute25 = ";
        bdeu_PrintMethods::print(stream, d_attribute25,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute26 = ";
        bdeu_PrintMethods::print(stream, d_attribute26,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute27 = ";
        bdeu_PrintMethods::print(stream, d_attribute27,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute28 = ";
        bdeu_PrintMethods::print(stream, d_attribute28,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute29 = ";
        bdeu_PrintMethods::print(stream, d_attribute29,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute30 = ";
        bdeu_PrintMethods::print(stream, d_attribute30,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute31 = ";
        bdeu_PrintMethods::print(stream, d_attribute31,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute32 = ";
        bdeu_PrintMethods::print(stream, d_attribute32,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute33 = ";
        bdeu_PrintMethods::print(stream, d_attribute33,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute34 = ";
        bdeu_PrintMethods::print(stream, d_attribute34,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute35 = ";
        bdeu_PrintMethods::print(stream, d_attribute35,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute36 = ";
        bdeu_PrintMethods::print(stream, d_attribute36,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute37 = ";
        bdeu_PrintMethods::print(stream, d_attribute37,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute38 = ";
        bdeu_PrintMethods::print(stream, d_attribute38,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute39 = ";
        bdeu_PrintMethods::print(stream, d_attribute39,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute40 = ";
        bdeu_PrintMethods::print(stream, d_attribute40,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line
        stream << '[';
        stream << ' ';
        stream << "Attribute01 = ";
        bdeu_PrintMethods::print(stream, d_attribute01,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute02 = ";
        bdeu_PrintMethods::print(stream, d_attribute02,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute03 = ";
        bdeu_PrintMethods::print(stream, d_attribute03,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute04 = ";
        bdeu_PrintMethods::print(stream, d_attribute04,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute05 = ";
        bdeu_PrintMethods::print(stream, d_attribute05,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute06 = ";
        bdeu_PrintMethods::print(stream, d_attribute06,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute07 = ";
        bdeu_PrintMethods::print(stream, d_attribute07,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute08 = ";
        bdeu_PrintMethods::print(stream, d_attribute08,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute09 = ";
        bdeu_PrintMethods::print(stream, d_attribute09,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute10 = ";
        bdeu_PrintMethods::print(stream, d_attribute10,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute11 = ";
        bdeu_PrintMethods::print(stream, d_attribute11,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute12 = ";
        bdeu_PrintMethods::print(stream, d_attribute12,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute13 = ";
        bdeu_PrintMethods::print(stream, d_attribute13,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute14 = ";
        bdeu_PrintMethods::print(stream, d_attribute14,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute15 = ";
        bdeu_PrintMethods::print(stream, d_attribute15,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute16 = ";
        bdeu_PrintMethods::print(stream, d_attribute16,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute17 = ";
        bdeu_PrintMethods::print(stream, d_attribute17,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute18 = ";
        bdeu_PrintMethods::print(stream, d_attribute18,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute19 = ";
        bdeu_PrintMethods::print(stream, d_attribute19,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute20 = ";
        bdeu_PrintMethods::print(stream, d_attribute20,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute21 = ";
        bdeu_PrintMethods::print(stream, d_attribute21,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute22 = ";
        bdeu_PrintMethods::print(stream, d_attribute22,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute23 = ";
        bdeu_PrintMethods::print(stream, d_attribute23,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute24 = ";
        bdeu_PrintMethods::print(stream, d_attribute24,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute25 = ";
        bdeu_PrintMethods::print(stream, d_attribute25,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute26 = ";
        bdeu_PrintMethods::print(stream, d_attribute26,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute27 = ";
        bdeu_PrintMethods::print(stream, d_attribute27,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute28 = ";
        bdeu_PrintMethods::print(stream, d_attribute28,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute29 = ";
        bdeu_PrintMethods::print(stream, d_attribute29,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute30 = ";
        bdeu_PrintMethods::print(stream, d_attribute30,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute31 = ";
        bdeu_PrintMethods::print(stream, d_attribute31,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute32 = ";
        bdeu_PrintMethods::print(stream, d_attribute32,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute33 = ";
        bdeu_PrintMethods::print(stream, d_attribute33,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute34 = ";
        bdeu_PrintMethods::print(stream, d_attribute34,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute35 = ";
        bdeu_PrintMethods::print(stream, d_attribute35,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute36 = ";
        bdeu_PrintMethods::print(stream, d_attribute36,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute37 = ";
        bdeu_PrintMethods::print(stream, d_attribute37,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute38 = ";
        bdeu_PrintMethods::print(stream, d_attribute38,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute39 = ";
        bdeu_PrintMethods::print(stream, d_attribute39,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Attribute40 = ";
        bdeu_PrintMethods::print(stream, d_attribute40,
                             -levelPlus1, spacesPerLevel);
        stream << " ]";
    }
    return stream << bsl::flush;
}
}  // close namespace test;
}  // close namespace BloombergLP;
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_address.h   -*-C++-*-
#ifndef INCLUDED_TEST_ADDRESS
#define INCLUDED_TEST_ADDRESS
//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: Address
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation
#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif
#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif
#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif
#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif
#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif
#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif
#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif
#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif
namespace BloombergLP {
namespace test {
class Address {
  private:
    bsl::string d_street; // todo: provide annotation
    bsl::string d_city; // todo: provide annotation
    bsl::string d_state; // todo: provide annotation
  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };
    enum {
        ATTRIBUTE_INDEX_STREET = 0,
            // index for "Street" attribute
        ATTRIBUTE_INDEX_CITY = 1,
            // index for "City" attribute
        ATTRIBUTE_INDEX_STATE = 2
            // index for "State" attribute
    };
    enum {
        ATTRIBUTE_ID_STREET = 0,
            // id for "Street" attribute
        ATTRIBUTE_ID_CITY = 1,
            // id for "City" attribute
        ATTRIBUTE_ID_STATE = 2
            // id for "State" attribute
    };
  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Address")
    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute
  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.
    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.
    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.
    // CREATORS
    explicit Address(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Address' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.
    Address(const Address& original, bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Address' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.
    ~Address();
        // Destroy this object.
    // MANIPULATORS
    Address& operator=(const Address& rhs);
        // Assign to this object the value of the specified 'rhs' object.
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).
    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.
    bsl::string& street();
        // Return a reference to the modifiable "Street" attribute of this
        // object.
    bsl::string& city();
        // Return a reference to the modifiable "City" attribute of this
        // object.
    bsl::string& state();
        // Return a reference to the modifiable "State" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.
    const bsl::string& street() const;
        // Return a reference to the non-modifiable "Street"
        // attribute of this object.
    const bsl::string& city() const;
        // Return a reference to the non-modifiable "City"
        // attribute of this object.
    const bsl::string& state() const;
        // Return a reference to the non-modifiable "State"
        // attribute of this object.
};
// FREE OPERATORS
inline
bool operator==(const Address& lhs, const Address& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.
inline
bool operator!=(const Address& lhs, const Address& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const Address& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.
// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================
// The following inlined functions are invoked from other inline functions.
inline
int Address::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}
// ---------------------------------------------------------------------------
// CREATORS
inline
Address::Address(bslma_Allocator *basicAllocator)
: d_street(bslma_Default::allocator(basicAllocator))
, d_city(bslma_Default::allocator(basicAllocator))
, d_state(bslma_Default::allocator(basicAllocator))
{
}
inline
Address::Address(
    const Address& original,
    bslma_Allocator *basicAllocator)
: d_street(original.d_street, bslma_Default::allocator(basicAllocator))
, d_city(original.d_city, bslma_Default::allocator(basicAllocator))
, d_state(original.d_state, bslma_Default::allocator(basicAllocator))
{
}
inline
Address::~Address()
{
}
// MANIPULATORS
inline
Address&
Address::operator=(const Address& rhs)
{
    if (this != &rhs) {
        d_street = rhs.d_street;
        d_city = rhs.d_city;
        d_state = rhs.d_state;
    }
    return *this;
}
template <class STREAM>
inline
STREAM& Address::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            streamIn(
                  stream, d_street, 1);
            streamIn(
                  stream, d_city, 1);
            streamIn(
                  stream, d_state, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}
inline
void Address::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_street);
    bdeat_ValueTypeFunctions::reset(&d_city);
    bdeat_ValueTypeFunctions::reset(&d_state);
}
template <class MANIPULATOR>
inline
int Address::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;
    ret = manipulator(&d_street, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    return ret;
}
template <class MANIPULATOR>
inline
int Address::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };
    switch (id) {
      case ATTRIBUTE_ID_STREET: {
        return manipulator(&d_street,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_CITY: {
        return manipulator(&d_city,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_STATE: {
        return manipulator(&d_state,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}
template <class MANIPULATOR>
inline
int Address::manipulateAttribute(MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };
    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
    }
    return manipulateAttribute(manipulator, attributeInfo->d_id);
}
inline
bsl::string& Address::street()
{
    return d_street;
}
inline
bsl::string& Address::city()
{
    return d_city;
}
inline
bsl::string& Address::state()
{
    return d_state;
}
// ACCESSORS
template <class STREAM>
inline
STREAM& Address::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        streamOut(stream, d_street, 1);
        streamOut(stream, d_city, 1);
        streamOut(stream, d_state, 1);
      } break;
    }
    return stream;
}
template <class ACCESSOR>
inline
int Address::accessAttributes(ACCESSOR& accessor) const
{
    int ret;
    ret = accessor(d_street, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    return ret;
}
template <class ACCESSOR>
inline
int Address::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };
    switch (id) {
      case ATTRIBUTE_ID_STREET: {
        return accessor(d_street,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_CITY: {
        return accessor(d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_STATE: {
        return accessor(d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}
template <class ACCESSOR>
inline
int Address::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };
     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
     }
     return accessAttribute(accessor, attributeInfo->d_id);
}
inline
const bsl::string& Address::street() const
{
    return d_street;
}
inline
const bsl::string& Address::city() const
{
    return d_city;
}
inline
const bsl::string& Address::state() const
{
    return d_state;
}
}  // close namespace test;
// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Address)
// FREE OPERATORS
inline
bool test::operator==(const test::Address& lhs, const test::Address& rhs)
{
    return  lhs.street() == rhs.street()
         && lhs.city() == rhs.city()
         && lhs.state() == rhs.state();
}
inline
bool test::operator!=(const test::Address& lhs, const test::Address& rhs)
{
    return  lhs.street() != rhs.street()
         || lhs.city() != rhs.city()
         || lhs.state() != rhs.state();
}
inline
bsl::ostream& test::operator<<(bsl::ostream& stream, const test::Address& rhs)
{
    return rhs.print(stream, 0, -1);
}
}  // close namespace BloombergLP;
#endif
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_address.cpp  -*-C++-*-
#include <bsl_iostream.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>
#include <bsl_string.h>
namespace BloombergLP {
namespace test {
                               // ---------
                               // CONSTANTS
                               // ---------
const char Address::CLASS_NAME[] = "Address";
    // the name of this class
const bdeat_AttributeInfo Address::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_STREET,
        "street",                     // name
        sizeof("street") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_CITY,
        "city",                     // name
        sizeof("city") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_STATE,
        "state",                     // name
        sizeof("state") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};
                               // -------------
                               // CLASS METHODS
                               // -------------
const bdeat_AttributeInfo *Address::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 4: {
            if (bdeu_CharType::toUpper(name[0])=='C'
             && bdeu_CharType::toUpper(name[1])=='I'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='Y') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY];
            }
        } break;
        case 5: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='A'
             && bdeu_CharType::toUpper(name[3])=='T'
             && bdeu_CharType::toUpper(name[4])=='E') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE];
            }
        } break;
        case 6: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='R'
             && bdeu_CharType::toUpper(name[3])=='E'
             && bdeu_CharType::toUpper(name[4])=='E'
             && bdeu_CharType::toUpper(name[5])=='T') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET];
            }
        } break;
    }
    return 0;
}
const bdeat_AttributeInfo *Address::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_STREET:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET];
      case ATTRIBUTE_ID_CITY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY];
      case ATTRIBUTE_ID_STATE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE];
      default:
        return 0;
    }
}
                                // --------
                                // CREATORS
                                // --------
                                // ------------
                                // MANIPULATORS
                                // ------------
                                // ---------
                                // ACCESSORS
                                // ---------
bsl::ostream& Address::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }
    int levelPlus1 = level + 1;
    if (0 <= spacesPerLevel) {
        // multiline
        stream << "[\n";
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Street = ";
        bdeu_PrintMethods::print(stream, d_street,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "City = ";
        bdeu_PrintMethods::print(stream, d_city,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "State = ";
        bdeu_PrintMethods::print(stream, d_state,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line
        stream << '[';
        stream << ' ';
        stream << "Street = ";
        bdeu_PrintMethods::print(stream, d_street,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "City = ";
        bdeu_PrintMethods::print(stream, d_city,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "State = ";
        bdeu_PrintMethods::print(stream, d_state,
                             -levelPlus1, spacesPerLevel);
        stream << " ]";
    }
    return stream << bsl::flush;
}
}  // close namespace test;
}  // close namespace BloombergLP;
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_employee.h   -*-C++-*-
#ifndef INCLUDED_TEST_EMPLOYEE
#define INCLUDED_TEST_EMPLOYEE
//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: Employee
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation
#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif
#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif
#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif
#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif
#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif
#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif
#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif
#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif
#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif
#ifndef INCLUDED_BDET_DATETZ
#include <bdet_datetz.h>
#endif
namespace BloombergLP {
namespace test {
class Employee {
  private:
    bsl::string d_name; // todo: provide annotation
    Address d_homeAddress; // todo: provide annotation
    int d_age; // todo: provide annotation
    bdet_DateTz d_dateJoined; // todo: provide annotation
  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 4 // the number of attributes in this class
    };
    enum {
        ATTRIBUTE_INDEX_NAME = 0,
            // index for "Name" attribute
        ATTRIBUTE_INDEX_HOME_ADDRESS = 1,
            // index for "HomeAddress" attribute
        ATTRIBUTE_INDEX_AGE = 2,
            // index for "Age" attribute
        ATTRIBUTE_INDEX_DATE_JOINED = 3
            // index for "DateJoined" attribute
    };
    enum {
        ATTRIBUTE_ID_NAME = 0,
            // id for "Name" attribute
        ATTRIBUTE_ID_HOME_ADDRESS = 1,
            // id for "HomeAddress" attribute
        ATTRIBUTE_ID_AGE = 2,
            // id for "Age" attribute
        ATTRIBUTE_ID_DATE_JOINED = 3
            // id for "DateJoined" attribute
    };
  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Employee")
    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute
  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.
    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.
    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.
    // CREATORS
    explicit Employee(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Employee' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.
    Employee(const Employee& original, bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Employee' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.
    ~Employee();
        // Destroy this object.
    // MANIPULATORS
    Employee& operator=(const Employee& rhs);
        // Assign to this object the value of the specified 'rhs' object.
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).
    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.
    bsl::string& name();
        // Return a reference to the modifiable "Name" attribute of this
        // object.
    Address& homeAddress();
        // Return a reference to the modifiable "HomeAddress" attribute of this
        // object.
    int& age();
        // Return a reference to the modifiable "Age" attribute of this object.
    bdet_DateTz& dateJoined();
        // Return a reference to the modifiable "DateJoined" attribute of this
        // object.  ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.
    const bsl::string& name() const;
        // Return a reference to the non-modifiable "Name"
        // attribute of this object.
    const Address& homeAddress() const;
        // Return a reference to the non-modifiable "HomeAddress"
        // attribute of this object.
    const int& age() const;
        // Return a reference to the non-modifiable "Age"
        // attribute of this object.
    const bdet_DateTz& dateJoined() const;
        // Return a reference to the non-modifiable "DateJoined"
        // attribute of this object.
};
// FREE OPERATORS
inline
bool operator==(const Employee& lhs, const Employee& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.
inline
bool operator!=(const Employee& lhs, const Employee& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const Employee& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.
// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================
// The following inlined functions are invoked from other inline functions.
inline
int Employee::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}
// ---------------------------------------------------------------------------
// CREATORS
inline
Employee::Employee(bslma_Allocator *basicAllocator)
: d_name(bslma_Default::allocator(basicAllocator))
, d_homeAddress(bslma_Default::allocator(basicAllocator))
, d_age()
, d_dateJoined()
{
}
inline
Employee::Employee(
    const Employee& original,
    bslma_Allocator *basicAllocator)
: d_name(original.d_name, bslma_Default::allocator(basicAllocator))
, d_homeAddress(original.d_homeAddress,
                bslma_Default::allocator(basicAllocator))
, d_age(original.d_age)
, d_dateJoined(original.d_dateJoined)
{
}
inline
Employee::~Employee()
{
}
// MANIPULATORS
inline
Employee&
Employee::operator=(const Employee& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_homeAddress = rhs.d_homeAddress;
        d_age = rhs.d_age;
        d_dateJoined = rhs.d_dateJoined;
    }
    return *this;
}
template <class STREAM>
inline
STREAM& Employee::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            streamIn(
                  stream, d_name, 1);
            streamIn(
                  stream, d_homeAddress, 1);
            streamIn(
                  stream, d_age, 1);
            streamIn(
                  stream, d_dateJoined, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}
inline
void Employee::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_name);
    bdeat_ValueTypeFunctions::reset(&d_homeAddress);
    bdeat_ValueTypeFunctions::reset(&d_age);
    bdeat_ValueTypeFunctions::reset(&d_dateJoined);
}
template <class MANIPULATOR>
inline
int Employee::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;
    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_homeAddress,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = manipulator(&d_dateJoined,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATE_JOINED]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    return ret;
}
template <class MANIPULATOR>
inline
int Employee::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };
    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_HOME_ADDRESS: {
        return manipulator(&d_homeAddress,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_AGE: {
        return manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_DATE_JOINED: {
        return manipulator(&d_dateJoined,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATE_JOINED]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}
template <class MANIPULATOR>
inline
int Employee::manipulateAttribute(MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };
    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
    }
    return manipulateAttribute(manipulator, attributeInfo->d_id);
}
inline
bsl::string& Employee::name()
{
    return d_name;
}
inline
Address& Employee::homeAddress()
{
    return d_homeAddress;
}
inline
int& Employee::age()
{
    return d_age;
}
inline
bdet_DateTz& Employee::dateJoined()
{
    return d_dateJoined;
}
// ACCESSORS
template <class STREAM>
inline
STREAM& Employee::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        streamOut(stream, d_name, 1);
        streamOut(stream, d_homeAddress, 1);
        streamOut(stream, d_age, 1);
        streamOut(stream, d_dateJoined, 1);
      } break;
    }
    return stream;
}
template <class ACCESSOR>
inline
int Employee::accessAttributes(ACCESSOR& accessor) const
{
    int ret;
    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_homeAddress,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    ret = accessor(d_dateJoined,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATE_JOINED]);
    if (ret) {
        return ret;                                                 // RETURN
    }
    return ret;
}
template <class ACCESSOR>
inline
int Employee::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };
    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_HOME_ADDRESS: {
        return accessor(d_homeAddress,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_AGE: {
        return accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_DATE_JOINED: {
        return accessor(d_dateJoined,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATE_JOINED]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}
template <class ACCESSOR>
inline
int Employee::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };
     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
     }
     return accessAttribute(accessor, attributeInfo->d_id);
}
inline
const bsl::string& Employee::name() const
{
    return d_name;
}
inline
const Address& Employee::homeAddress() const
{
    return d_homeAddress;
}
inline
const int& Employee::age() const
{
    return d_age;
}
inline
const bdet_DateTz& Employee::dateJoined() const
{
    return d_dateJoined;
}
}  // close namespace test;
// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Employee)
// FREE OPERATORS
inline
bool test::operator==(const test::Employee& lhs, const test::Employee& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.homeAddress() == rhs.homeAddress()
         && lhs.age() == rhs.age()
         && lhs.dateJoined() == rhs.dateJoined();
}
inline
bool test::operator!=(const test::Employee& lhs, const test::Employee& rhs)
{
    return  lhs.name() != rhs.name()
         || lhs.homeAddress() != rhs.homeAddress()
         || lhs.age() != rhs.age()
         || lhs.dateJoined() != rhs.dateJoined();
}
inline
bsl::ostream& test::operator<<(bsl::ostream& stream, const test::Employee& rhs)
{
    return rhs.print(stream, 0, -1);
}
}  // close namespace BloombergLP;
#endif
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
// test_employee.cpp  -*-C++-*-
#include <bsl_iostream.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>
#include <bsl_string.h>
#include <bdet_datetz.h>
namespace BloombergLP {
namespace test {
                               // ---------
                               // CONSTANTS
                               // ---------
const char Employee::CLASS_NAME[] = "Employee";
    // the name of this class
const bdeat_AttributeInfo Employee::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",                     // name
        sizeof("name") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_HOME_ADDRESS,
        "homeAddress",                     // name
        sizeof("homeAddress") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_AGE,
        "age",                     // name
        sizeof("age") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_DATE_JOINED,
        "dateJoined",                     // name
        sizeof("dateJoined") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    }
};
                               // -------------
                               // CLASS METHODS
                               // -------------
const bdeat_AttributeInfo *Employee::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 3: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='G'
             && bdeu_CharType::toUpper(name[2])=='E') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
            }
        } break;
        case 4: {
            if (bdeu_CharType::toUpper(name[0])=='N'
             && bdeu_CharType::toUpper(name[1])=='A'
             && bdeu_CharType::toUpper(name[2])=='M'
             && bdeu_CharType::toUpper(name[3])=='E') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
            }
        } break;
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='D'
             && bdeu_CharType::toUpper(name[1])=='A'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='E'
             && bdeu_CharType::toUpper(name[4])=='J'
             && bdeu_CharType::toUpper(name[5])=='O'
             && bdeu_CharType::toUpper(name[6])=='I'
             && bdeu_CharType::toUpper(name[7])=='N'
             && bdeu_CharType::toUpper(name[8])=='E'
             && bdeu_CharType::toUpper(name[9])=='D') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATE_JOINED];
            }
        } break;
        case 11: {
            if (bdeu_CharType::toUpper(name[0])=='H'
             && bdeu_CharType::toUpper(name[1])=='O'
             && bdeu_CharType::toUpper(name[2])=='M'
             && bdeu_CharType::toUpper(name[3])=='E'
             && bdeu_CharType::toUpper(name[4])=='A'
             && bdeu_CharType::toUpper(name[5])=='D'
             && bdeu_CharType::toUpper(name[6])=='D'
             && bdeu_CharType::toUpper(name[7])=='R'
             && bdeu_CharType::toUpper(name[8])=='E'
             && bdeu_CharType::toUpper(name[9])=='S'
             && bdeu_CharType::toUpper(name[10])=='S') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS];
            }
        } break;
    }
    return 0;
}
const bdeat_AttributeInfo *Employee::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      case ATTRIBUTE_ID_HOME_ADDRESS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS];
      case ATTRIBUTE_ID_AGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
      case ATTRIBUTE_ID_DATE_JOINED:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATE_JOINED];
      default:
        return 0;
    }
}
                                // --------
                                // CREATORS
                                // --------
                                // ------------
                                // MANIPULATORS
                                // ------------
                                // ---------
                                // ACCESSORS
                                // ---------
bsl::ostream& Employee::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }
    int levelPlus1 = level + 1;
    if (0 <= spacesPerLevel) {
        // multiline
        stream << "[\n";
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Name = ";
        bdeu_PrintMethods::print(stream, d_name,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "HomeAddress = ";
        bdeu_PrintMethods::print(stream, d_homeAddress,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Age = ";
        bdeu_PrintMethods::print(stream, d_age,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "DateJoined = ";
        bdeu_PrintMethods::print(stream, d_dateJoined,
                             -levelPlus1, spacesPerLevel);
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line
        stream << '[';
        stream << ' ';
        stream << "Name = ";
        bdeu_PrintMethods::print(stream, d_name,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "HomeAddress = ";
        bdeu_PrintMethods::print(stream, d_homeAddress,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "Age = ";
        bdeu_PrintMethods::print(stream, d_age,
                             -levelPlus1, spacesPerLevel);
        stream << ' ';
        stream << "DateJoined = ";
        bdeu_PrintMethods::print(stream, d_dateJoined,
                             -levelPlus1, spacesPerLevel);
        stream << " ]";
    }
    return stream << bsl::flush;
}
}  // close namespace test
}  // close namespace BloombergLP
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
namespace {
bool isEqual(const bdet_DatetimeTz& lhs, const bdem_List& rhs)
{
    return 2                       == rhs.length()
        && bdem_ElemType::BDEM_DATETIME == rhs.elemType(0)
        && bdem_ElemType::BDEM_INT      == rhs.elemType(1)
        && lhs.localDatetime()     == rhs.theDatetime(0)
        && lhs.offset()            == rhs.theInt(1);
}
bool isEqual(const bdet_DateTz& lhs, const bdem_List& rhs)
{
    return 2                   == rhs.length()
        && bdem_ElemType::BDEM_DATE == rhs.elemType(0)
        && bdem_ElemType::BDEM_INT  == rhs.elemType(1)
        && lhs.localDate()     == rhs.theDate(0)
        && lhs.offset()        == rhs.theInt(1);
}
bool isEqual(const bdet_TimeTz& lhs, const bdem_List& rhs)
{
    return 2                   == rhs.length()
        && bdem_ElemType::BDEM_TIME == rhs.elemType(0)
        && bdem_ElemType::BDEM_INT  == rhs.elemType(1)
        && lhs.localTime()     == rhs.theTime(0)
        && lhs.offset()        == rhs.theInt(1);
}
bool isEqual(const test::MySequence& lhs, const bdem_List& rhs)
{
    return 2                     == rhs.length()
        && bdem_ElemType::BDEM_INT    == rhs.elemType(0)
        && bdem_ElemType::BDEM_STRING == rhs.elemType(1)
        && lhs.attribute1()      == rhs.theInt(0)
        && lhs.attribute2()      == rhs.theString(1);
}
bool isEqual(const test::MyChoice& lhs, const bdem_List& rhs)
{
    const int selectionId = lhs.selectionId();
    if (test::MyChoice::SELECTION_ID_UNDEFINED == selectionId) {
        if (1                     != rhs.length()
         || bdem_ElemType::BDEM_STRING != rhs.elemType(0)
         || bsl::string("")       != rhs.theString(0)) {
            return false;
        }
        return true;
    }
    if (2                     != rhs.length()
     || bdem_ElemType::BDEM_STRING != rhs.elemType(0)) {
        return false;
    }
    if (test::MyChoice::SELECTION_ID_SELECTION1 == selectionId) {
        if ("Selection1"       != rhs.theString(0)
         || bdem_ElemType::BDEM_INT != rhs.elemType(1)
         || lhs.selection1()   != rhs.theInt(1)) {
            return false;
        }
    }
    else {  // selection2
        if ("Selection2"          != rhs.theString(0)
         || bdem_ElemType::BDEM_STRING != rhs.elemType(1)
         || lhs.selection2()      != rhs.theString(1)) {
            return false;
        }
    }
    return true;
}
bool isEqual(const test::MySequenceWithNullables& lhs,
             const bdem_List&                     rhs)
{
    if (5                        != rhs.length()
     || bdem_ElemType::BDEM_INT       != rhs.elemType(0)
     || bdem_ElemType::BDEM_STRING    != rhs.elemType(1)
     || bdem_ElemType::BDEM_LIST      != rhs.elemType(2)
     || bdem_ElemType::BDEM_INT_ARRAY != rhs.elemType(3)
     || bdem_ElemType::BDEM_INT       != rhs.elemType(4)) {
        return false;
    }
    if ((lhs.attribute1().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(0)))
     || (!lhs.attribute1().isNull()
                               && lhs.attribute1().value() != rhs.theInt(0))) {
        return false;
    }
    if ((lhs.attribute2().isNull()
      && !bdetu_Unset<bsl::string>::isUnset(rhs.theString(1)))
     || (!lhs.attribute2().isNull()
                            && lhs.attribute2().value() != rhs.theString(1))) {
        return false;
    }
    if ((lhs.attribute3().isNull() && 0 != rhs.theList(2).length())
     || (!lhs.attribute3().isNull()
                      && !isEqual(lhs.attribute3().value(), rhs.theList(2)))) {
        return false;
    }
    if ((lhs.attribute4().isNull() && 0 != rhs.theIntArray(3).size())
     || (!lhs.attribute4().isNull()
                          && lhs.attribute4().value() != rhs.theIntArray(3))) {
        return false;
    }
    if ((lhs.attribute5().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(4)))
     || (!lhs.attribute5().isNull()
                          && int(lhs.attribute5().value()) != rhs.theInt(4))) {
        return false;
    }
    return true;
}
bool isEqual(const test::MySequenceWithNullablesContainer& lhs,
             const bdem_List&                              rhs)
{
    return (1                   == rhs.length()
         && bdem_ElemType::BDEM_LIST == rhs.elemType(0)
         && isEqual(lhs.contained(), rhs.theList(0)));
}
bool isEqual(const test::MySequenceWithManyNullables& lhs,
             const bdem_List&                         rhs)
{
    if (40                 != rhs.length()
     || bdem_ElemType::BDEM_INT != rhs.elemType(0)
     || bdem_ElemType::BDEM_INT != rhs.elemType(1)
     || bdem_ElemType::BDEM_INT != rhs.elemType(2)
     || bdem_ElemType::BDEM_INT != rhs.elemType(3)
     || bdem_ElemType::BDEM_INT != rhs.elemType(4)
     || bdem_ElemType::BDEM_INT != rhs.elemType(5)
     || bdem_ElemType::BDEM_INT != rhs.elemType(6)
     || bdem_ElemType::BDEM_INT != rhs.elemType(7)
     || bdem_ElemType::BDEM_INT != rhs.elemType(8)
     || bdem_ElemType::BDEM_INT != rhs.elemType(9)
     || bdem_ElemType::BDEM_INT != rhs.elemType(10)
     || bdem_ElemType::BDEM_INT != rhs.elemType(11)
     || bdem_ElemType::BDEM_INT != rhs.elemType(12)
     || bdem_ElemType::BDEM_INT != rhs.elemType(13)
     || bdem_ElemType::BDEM_INT != rhs.elemType(14)
     || bdem_ElemType::BDEM_INT != rhs.elemType(15)
     || bdem_ElemType::BDEM_INT != rhs.elemType(16)
     || bdem_ElemType::BDEM_INT != rhs.elemType(17)
     || bdem_ElemType::BDEM_INT != rhs.elemType(18)
     || bdem_ElemType::BDEM_INT != rhs.elemType(19)
     || bdem_ElemType::BDEM_INT != rhs.elemType(20)
     || bdem_ElemType::BDEM_INT != rhs.elemType(21)
     || bdem_ElemType::BDEM_INT != rhs.elemType(22)
     || bdem_ElemType::BDEM_INT != rhs.elemType(23)
     || bdem_ElemType::BDEM_INT != rhs.elemType(24)
     || bdem_ElemType::BDEM_INT != rhs.elemType(25)
     || bdem_ElemType::BDEM_INT != rhs.elemType(26)
     || bdem_ElemType::BDEM_INT != rhs.elemType(27)
     || bdem_ElemType::BDEM_INT != rhs.elemType(28)
     || bdem_ElemType::BDEM_INT != rhs.elemType(29)
     || bdem_ElemType::BDEM_INT != rhs.elemType(30)
     || bdem_ElemType::BDEM_INT != rhs.elemType(31)
     || bdem_ElemType::BDEM_INT != rhs.elemType(32)
     || bdem_ElemType::BDEM_INT != rhs.elemType(33)
     || bdem_ElemType::BDEM_INT != rhs.elemType(34)
     || bdem_ElemType::BDEM_INT != rhs.elemType(35)
     || bdem_ElemType::BDEM_INT != rhs.elemType(36)
     || bdem_ElemType::BDEM_INT != rhs.elemType(37)
     || bdem_ElemType::BDEM_INT != rhs.elemType(38)
     || bdem_ElemType::BDEM_INT != rhs.elemType(39)) {
        return false;
    }
    int i = 0;
    if (( lhs.attribute01().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute01().isNull()
       && lhs.attribute01().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute02().isNull()
       && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute02().isNull()
       && lhs.attribute02().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute03().isNull()
       && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute03().isNull()
       && lhs.attribute03().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute04().isNull()
       && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute04().isNull()
       && lhs.attribute04().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute05().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute05().isNull()
       && lhs.attribute05().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute06().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute06().isNull()
       && lhs.attribute06().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute07().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute07().isNull()
       && lhs.attribute07().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute08().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute08().isNull()
       && lhs.attribute08().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute09().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute09().isNull()
       && lhs.attribute09().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute10().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute10().isNull()
       && lhs.attribute10().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute11().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute11().isNull()
       && lhs.attribute11().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute12().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute12().isNull()
       && lhs.attribute12().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute13().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute13().isNull()
       && lhs.attribute13().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute14().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute14().isNull()
       && lhs.attribute14().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute15().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute15().isNull()
       && lhs.attribute15().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute16().isNull()
       && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute16().isNull()
       && lhs.attribute16().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute17().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute17().isNull()
       && lhs.attribute17().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute18().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute18().isNull()
       && lhs.attribute18().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute19().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute19().isNull()
       && lhs.attribute19().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute20().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute20().isNull()
       && lhs.attribute20().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute21().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute21().isNull()
       && lhs.attribute21().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute22().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute22().isNull()
       && lhs.attribute22().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute23().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute23().isNull()
       && lhs.attribute23().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute24().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute24().isNull()
       && lhs.attribute24().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute25().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute25().isNull()
       && lhs.attribute25().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute26().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute26().isNull()
       && lhs.attribute26().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute27().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute27().isNull()
       && lhs.attribute27().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute28().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute28().isNull()
       && lhs.attribute28().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute29().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute29().isNull()
       && lhs.attribute29().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute30().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute30().isNull()
       && lhs.attribute30().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute31().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute31().isNull()
       && lhs.attribute31().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute32().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute32().isNull()
       && lhs.attribute32().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute33().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute33().isNull()
       && lhs.attribute33().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute34().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute34().isNull()
       && lhs.attribute34().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute35().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute35().isNull()
       && lhs.attribute35().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute36().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute36().isNull()
       && lhs.attribute36().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute37().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute37().isNull()
       && lhs.attribute37().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute38().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute38().isNull()
       && lhs.attribute38().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute39().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute39().isNull()
       && lhs.attribute39().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    if (( lhs.attribute40().isNull()
      && !bdetu_Unset<int>::isUnset(rhs.theInt(i)))
     || (!lhs.attribute40().isNull()
       && lhs.attribute40().value() != rhs.theInt(i))) {
        return false;
    }
    ++i;
    ASSERT(40 == i);
    return true;
}
                   // =====================================
                   // private struct TestValueFunctions_Imp
                   // =====================================
struct TestValueFunctions_Imp {
    // This 'struct' provides some of the implementation details for the
    // functions in the 'TestValueFunctions' namespace.
    int d_index;  // index of test value to generate
    // TYPES
    enum { SUCCESS = 0, FAILURE = -1 };
        // Return values for the methods in this 'struct'.
    // CLASS METHODS
    template <typename TYPE>
    static
    int loadTestValue(TYPE *object, int index, bdeat_TypeCategory::Array);
        // Load into the specified Array 'object' a test value indicated by the
        // specified 'index'.  Return 0 on success, and a non-zero value
        // otherwise with no effect on 'object'.  The behavior is undefined
        // unless index >= 0.
    template <typename TYPE>
    static
    int loadTestValue(TYPE *object, int index, bdeat_TypeCategory::Choice);
        // Load into the specified Choice 'object' a test value indicated by
        // the specified 'index'.  Return 0 on success, and a non-zero value
        // otherwise with no effect on 'object'.  The behavior is undefined
        // unless index >= 0.
    template <typename TYPE>
    static
    int loadTestValue(TYPE *object, int index,
                      bdeat_TypeCategory::Enumeration);
        // Load into the specified Enumeration 'object' a test value indicated
        // by the specified 'index'.  Return 0 on success, and a non-zero value
        // otherwise with no effect on 'object'.  The behavior is undefined
        // unless index >= 0.
    template <typename TYPE>
    static
    int loadTestValue(TYPE *object, int index, bdeat_TypeCategory::Sequence);
        // Load into the specified Sequence 'object' a test value indicated by
        // the specified 'index'.  Return 0 on success, and a non-zero value
        // otherwise with no effect on 'object'.  The behavior is undefined
        // unless index >= 0.
    // MANIPULATORS
    template <typename TYPE, typename INFO>
    int operator()(TYPE *object, const INFO&);
        // Load a test value into the specified 'object' of parameterized
        // 'TYPE'.  Return 0 on success, and a non-zero value otherwise.
    template <typename TYPE>
    int operator()(TYPE *object);
        // Load a test value into the specified 'object' of parameterized
        // 'TYPE'.  Return 0 on success, and a non-zero value otherwise.
};
                        // ============================
                        // namespace TestValueFunctions
                        // ============================
namespace TestValueFunctions {
    // The functions provided in this 'namespace' supply test values for
    // objects of sequence, choice, and enumeration type, as well as values
    // for objects of any type that may be sequence attributes or choice
    // selections.
    // TYPES
    enum { SUCCESS = 0, FAILURE = -1 };
        // Return values for the functions in this 'namespace'.
    // MANIPULATORS
    template <typename TYPE>
    int loadTestValue(TYPE *object, int index);
        // Load into the specified 'object' a test value indicated by the
        // specified 'index'.  Return 0 on success, and a non-zero value
        // otherwise with no effect on 'object'.  The behavior is undefined
        // unless index >= 0.
    int loadTestValue(Enum *object, int index);
        // Load into the specified 'object' a test value of 'bool' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(bool *object, int index);
        // Load into the specified 'object' a test value of 'bool' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(char *object, int index);
        // Load into the specified 'object' a test value of 'char' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(signed char *object, int index);
        // Load into the specified 'object' a test value of 'signed char' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(unsigned char *object, int index);
        // Load into the specified 'object' a test value of 'unsigned char'
        // type indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(short *object, int index);
        // Load into the specified 'object' a test value of 'short' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(unsigned short *object, int index);
        // Load into the specified 'object' a test value of 'unsigned short'
        // type indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(int *object, int index);
        // Load into the specified 'object' a test value of 'int' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(unsigned int *object, int index);
        // Load into the specified 'object' a test value of 'unsigned int' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(bsls_Types::Int64 *object, int index);
        // Load into the specified 'object' a test value of 'long long' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(bsls_Types::Uint64 *object, int index);
        // Load into the specified 'object' a test value of
        // 'unsigned long long' type indicated by the specified 'index'.
        // Return 0 on success, and a non-zero value otherwise with no effect
        // on 'object'.  The behavior is undefined unless index >= 0.
    int loadTestValue(float *object, int index);
        // Load into the specified 'object' a test value of 'float' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(double *object, int index);
        // Load into the specified 'object' a test value of 'double' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(bsl::string *object, int index);
        // Load into the specified 'object' a test value of 'bsl::string' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(bdet_Date *object, int index);
        // Load into the specified 'object' a test value of 'bdet_Date' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(bdet_Datetime *object, int index);
        // Load into the specified 'object' a test value of 'bdet_Datetime'
        // type indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(bdet_Time *object, int index);
        // Load into the specified 'object' a test value of 'bdet_Time' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(bdet_DateTz *object, int index);
        // Load into the specified 'object' a test value of 'bdet_Date' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(bdet_DatetimeTz *object, int index);
        // Load into the specified 'object' a test value of 'bdet_Datetime'
        // type indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
    int loadTestValue(bdet_TimeTz *object, int index);
        // Load into the specified 'object' a test value of 'bdet_Time' type
        // indicated by the specified 'index'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on 'object'.  The behavior
        // is undefined unless index >= 0.
}  // close namespace TestValueFunctions
// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================
                   // -------------------------------------
                   // private struct TestValueFunctions_Imp
                   // -------------------------------------
// CLASS METHODS
template <typename TYPE>
inline
int TestValueFunctions_Imp::loadTestValue(TYPE *object,
                                          int   index,
                                          bdeat_TypeCategory::Array)
{
    BSLMF_ASSERT((bdeat_ArrayFunctions::IsArray<TYPE>::VALUE));
    BSLS_ASSERT_SAFE(index >= 0);
    enum { MAX_ARRAY_SIZE = 20 };
    int arraySize = index % MAX_ARRAY_SIZE;
    bdeat_ArrayFunctions::resize(object, arraySize);
    for (int i = 0; i < arraySize; ++i) {
        TestValueFunctions_Imp manipulator = { index + i };
        if (0 != bdeat_ArrayFunctions::manipulateElement(object,
                                                         manipulator,
                                                         i)) {
            return FAILURE;
        }
    }
    return SUCCESS;
}
template <typename TYPE>
inline
int TestValueFunctions_Imp::loadTestValue(TYPE *object,
                                                int   index,
                                                bdeat_TypeCategory::Choice)
{
    BSLMF_ASSERT((bdeat_ChoiceFunctions::IsChoice<TYPE>::VALUE));
    BSLS_ASSERT_SAFE(index >= 0);
    const int numSelections = TYPE::NUM_SELECTIONS;
    if (0 == index) {
        object->reset();
        return SUCCESS;
    }
    int selectionId = (index-1) % numSelections;
    if (0 != object->makeSelection(selectionId)) {
        return FAILURE;
    }
    TestValueFunctions_Imp manipulator = { index + 1 };
    return object->manipulateSelection(manipulator);
}
template <typename TYPE>
inline
int TestValueFunctions_Imp::loadTestValue(TYPE *object,
                                          int   index,
                                          bdeat_TypeCategory::Enumeration)
{
    BSLMF_ASSERT((bdeat_EnumFunctions::IsEnumeration<TYPE>::VALUE));
    BSLS_ASSERT_SAFE(index >= 0);
    typedef typename bslalg_TypeTraits<TYPE>::Wrapper Wrapper;
    index = index % Wrapper::NUM_ENUMERATORS;
    bdeat_EnumeratorInfo info = Wrapper::ENUMERATOR_INFO_ARRAY[index];
    *object = static_cast<TYPE>(info.value());
    return SUCCESS;
}
template <typename TYPE>
inline
int TestValueFunctions_Imp::loadTestValue(TYPE *object,
                                                int   index,
                                                bdeat_TypeCategory::Sequence)
{
    BSLMF_ASSERT((bdeat_SequenceFunctions::IsSequence<TYPE>::VALUE));
    BSLS_ASSERT_SAFE(index >= 0);
    for (int i = 0; i < TYPE::NUM_ATTRIBUTES; ++i) {
        TestValueFunctions_Imp manipulator = { index + i };
        if (0 != object->manipulateAttribute(manipulator, i)) {
            return FAILURE;
        }
    }
    return SUCCESS;
}
// MANIPULATORS
template <typename TYPE, typename INFO>
inline
int TestValueFunctions_Imp::operator()(TYPE *object, const INFO&)
{
    return TestValueFunctions::loadTestValue(object, d_index);
}
template <typename TYPE>
inline
int TestValueFunctions_Imp::operator()(TYPE *object)
{
    return TestValueFunctions::loadTestValue(object, d_index);
}
                        // ----------------------------
                        // namespace TestValueFunctions
                        // ----------------------------
// MANIPULATORS
template <typename TYPE>
inline
int TestValueFunctions::loadTestValue(TYPE *object, int index)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type Type;
    return TestValueFunctions_Imp::loadTestValue(object, index, Type());
}
inline
int TestValueFunctions::loadTestValue(Enum *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    *object = static_cast<Enum>(index % 3);
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(bool *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    *object = (index % 2) ? true : false;
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(char *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    if (index > UCHAR_MAX) {
        return FAILURE;                                               // RETURN
    }
    int value;
    if (index + '!' <= UCHAR_MAX) {
        value = index + '!';  // start with first printable char beyond ' '
    }
    else {
        value = index - (UCHAR_MAX - '!' + 1);
    }
    *object = static_cast<char>(value);
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(signed char *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    return loadTestValue(reinterpret_cast<char *>(object), index);
}
inline
int TestValueFunctions::loadTestValue(unsigned char *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    return loadTestValue(reinterpret_cast<char *>(object), index);
}
inline
int TestValueFunctions::loadTestValue(short *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    if (index > USHRT_MAX) {
        return FAILURE;                                               // RETURN
    }
    *object = static_cast<short>(index);
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(unsigned short *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    if (index > USHRT_MAX) {
        return FAILURE;                                               // RETURN
    }
    *object = static_cast<unsigned short>(index);
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(int *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    *object = index;
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(unsigned int *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    *object = static_cast<unsigned int>(index);
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(bsls_Types::Int64 *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    *object = index;
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(bsls_Types::Uint64 *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    *object = static_cast<bsls_Types::Uint64>(index);
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(float *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    *object = index;
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(double *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    *object = index;
    return SUCCESS;
}
#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
#endif
inline
int TestValueFunctions::loadTestValue(bsl::string *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    const int BUFSIZE = 64;
    char buffer[BUFSIZE];
    if (snprintf(buffer, BUFSIZE, "%d", index) > BUFSIZE) {
        return FAILURE;                                               // RETURN
    }
    buffer[BUFSIZE - 1] = '\0';
    *object = buffer;
    return SUCCESS;
}
#if defined(BSLS_PLATFORM_CMP_MSVC)
#undef snprintf
#endif
inline
int TestValueFunctions::loadTestValue(bdet_Date *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    // Treat 'index' as a number of days.  For simplicity, all months are
    // assumed to have 28 days.
    const int DAYS_IN_MONTH = 28;  // avoid February 29, April 31, etc.
    const int day   = index % DAYS_IN_MONTH + 1;  index /= DAYS_IN_MONTH;
    const int month = index % 12 + 1;             index /= 12;
    const int year  = index + 1753;  // avoid September 1752
    if (year > 9999) {
        return FAILURE;                                               // RETURN
    }
    BSLS_ASSERT_SAFE(bdet_Date::isValid(year, month, day));
    object->setYearMonthDay(year, month, day);
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(bdet_Datetime *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    bdet_Date date;
    if (loadTestValue(&date, index)) {
        return FAILURE;                                               // RETURN
    }
    object->setDate(date);
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(bdet_Time *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    // Treat 'index' as a number of milliseconds.
    const int milliseconds = index % 1000;  index /= 1000;
    const int seconds      = index %   60;  index /=   60;
    const int minutes      = index %   60;  index /=   60;
    const int hours        = index;
    if (hours > 23) {
        return FAILURE;                                               // RETURN
    }
    BSLS_ASSERT_SAFE(bdet_Time::isValid(hours,
                                        minutes,
                                        seconds,
                                        milliseconds));
    object->setTime(hours, minutes, seconds, milliseconds);
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(bdet_DateTz *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    bdet_Date local;
    int       offset;
    loadTestValue(&local, index);
    loadTestValue(&offset, index);
    object->setDateTz(local, offset);
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(bdet_DatetimeTz *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    bdet_Datetime local;
    int           offset;
    loadTestValue(&local, index);
    loadTestValue(&offset, index);
    object->setDatetimeTz(local, offset);
    return SUCCESS;
}
inline
int TestValueFunctions::loadTestValue(bdet_TimeTz *object, int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    bdet_Time local;
    int       offset;
    loadTestValue(&local, index);
    loadTestValue(&offset, index);
    object->setTimeTz(local, offset);
    return SUCCESS;
}
                              // ===============
                              // class SetToNull
                              // ===============
class SetToNull {
  public:
    template <typename TYPE, typename INFO_TYPE>
    int operator()(TYPE *object, const INFO_TYPE&)
    {
        BSLMF_ASSERT(
                 (bdeat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE));
        bdeat_ValueTypeFunctions::reset(object);
        return 0;
    }
};
                            // ===================
                            // class LoadTestValue
                            // ===================
class LoadTestValue {
    int d_valueIndex;
  public:
    explicit LoadTestValue(int valueIndex)
    : d_valueIndex(valueIndex)
    {
    }
    int operator()(test::MyEnumeration::Value *object)
    {
        int i = d_valueIndex % test::MyEnumeration::NUM_ENUMERATORS;
        return test::MyEnumeration::fromInt(object, i);
    }
    template <typename TYPE>
    int operator()(TYPE *object)
    {
        return TestValueFunctions::loadTestValue(object, d_valueIndex);
    }
};
                             // ==================
                             // class SetToNotNull
                             // ==================
class SetToNotNull {
    LoadTestValue d_loadTestValue;
  public:
    explicit SetToNotNull(int valueIndex)
    : d_loadTestValue(valueIndex)
    {
    }
    template <typename TYPE, typename INFO_TYPE>
    int operator()(TYPE *object, const INFO_TYPE&)
    {
        BSLMF_ASSERT(
                 (bdeat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE));
        bdeat_NullableValueFunctions::makeValue(object);
        return bdeat_NullableValueFunctions::manipulateValue(object,
                                                             d_loadTestValue);
    }
};
}  // close namespace <anonymous>
//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
// The following snippets of code illustrate the usage of this component.
// Suppose we have an XML schema inside a file called 'xsdfile.xsd':
//..
//  <?xml version='1.0' encoding='UTF-8'?>
//  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
//             xmlns:test='http://bloomberg.com/schemas/test'
//             targetNamespace='http://bloomberg.com/schemas/test'
//             elementFormDefault='unqualified'>
//
//      <xs:complexType name='Address'>
//          <xs:sequence>
//              <xs:element name='street' type='xs:string'/>
//              <xs:element name='city'   type='xs:string'/>
//              <xs:element name='state'  type='xs:string'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='Employee'>
//          <xs:sequence>
//              <xs:element name='name'        type='xs:string'/>
//              <xs:element name='homeAddress' type='test:Address'/>
//              <xs:element name='age'         type='xs:int'/>
//              <xs:element name='dateJoined'  type='date'/>
//          </xs:sequence>
//      </xs:complexType>
//
//  </xs:schema>
//..
// Using the 'bde_xsdcc.pl' tool, we can generate C++ classes for this schema:
//..
//  $ bde_xsdcc.pl -g h -g cpp -p test xsdfile.xsd
//..
// This tool will generate the header and implementation files for the
// 'test_address' and 'test_employee' components in the current directory.
//
// Now suppose we want to encode information about a particular employee in a
// 'bdem' wire format.  Note that we will use 'bdesb' stream buffers for
// in-core buffer management:
//..
//  #include <bdesb_memoutstreambuf.h>
//  #include <bdesb_fixedmeminstreambuf.h>
//  #include <bdex_byteinstreamformatter.h>
//  #include <bdem_list.h>
//
//  #include <test_employee.h>
//
//  #include <bdem_bdemencoderutil.h>
//
//  using namespace BloombergLP;
//
void usageExample()
{
    bdesb_MemOutStreamBuf osb;
    test::Employee bob;
    bob.name()                 = "Bob";
    bob.homeAddress().street() = "Some Street";
    bob.homeAddress().city()   = "Some City";
    bob.homeAddress().state()  = "Some State";
    bob.age()                  = 21;
    bob.dateJoined()           = bdet_DateTz(bdet_Date(2003, 3, 20), -300);
    int retCode = bdem_BdemEncoderUtil::encode(&osb, bob);
    ASSERT(0 == retCode);
//..
// At this point, 'osb' contains a representation of 'bob' in 'bdem' format
// (version 2).  Now we will load the contents of 'osb' into a 'bdem_List'
// object and verify its contents:
//..
    bdesb_FixedMemInStreamBuf  isb(osb.data(), osb.length());
    bdex_ByteInStreamFormatter formatter(&isb);
    bdem_List                  list;
    const int BDEM_VERSION = 2;
    list.bdexStreamIn(formatter, BDEM_VERSION);
    ASSERT(formatter);
    ASSERT(bob.name()                   == list.theString(0));
    ASSERT(bob.homeAddress().street()   == list.theList(1).theString(0));
    ASSERT(bob.homeAddress().city()     == list.theList(1).theString(1));
    ASSERT(bob.homeAddress().state()    == list.theList(1).theString(2));
    ASSERT(bob.age()                    == list.theInt(2));
    ASSERT(bob.dateJoined().localDate() == list.theList(3).theDate(0));
    ASSERT(bob.dateJoined().offset()    == list.theList(3).theInt(1));
}
//..
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;
    switch (test) { case 0:  // Zero is always the leading case.
      case 16: {
#if 0
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Usage Example"
                               << "\n=====================" << bsl::endl;
        usageExample();
#endif
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING NULLABLE TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Nullable Types"
                               << "\n======================"
                               << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (veryVerbose) bsl::cout << "\tTesting MySequenceWithNullables"
                                   << bsl::endl;
        {
            typedef test::MySequenceWithNullables Type;
            typedef bdem_List                     Proxy;
            Type DATA[test::MySequenceWithNullables::NUM_ATTRIBUTES];
            const int  NUM_DATA = sizeof DATA / sizeof *DATA;
            int tvi = 0;  // test value index
            for (int i = 0; i < NUM_DATA; ++i) {
                for (int j = 0; j < NUM_DATA; ++j) {
                    if (i == j) {
                        SetToNull setToNull;
                        LOOP2_ASSERT(
                               i, j,
                               0 == DATA[i].manipulateAttribute(setToNull, j));
                    }
                    else {
                        SetToNotNull setToNotNull(tvi);
                        ++tvi;
                        LOOP2_ASSERT(
                            i, j,
                            0 == DATA[i].manipulateAttribute(setToNotNull, j));
                    }
                }
            }
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                for (int i = 0; i < NUM_DATA; ++i) {
                    bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                    bdem_BdemEncoderOptions options;
                    options.setBdemVersion(bdemVersion);
                    Type mX = DATA[i];  const Type& X = mX;
                    LOOP2_ASSERT(bdemVersion, i,
                                 Encoder::encode(outStream, X, options));
                    LOOP2_ASSERT(bdemVersion, i, outStream);
                    if (veryVerbose) { P(outStream.length())  P(X) }
                    if (veryVeryVerbose) {
                        bdeu_Print::hexDump(bsl::cout,
                                            buffer, outStream.length());
                    }
                    bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                    Proxy mY; const Proxy& Y = mY;
                    ISF::streamIn(inStream, mY, bdemVersion);
                    LOOP2_ASSERT(bdemVersion, i, inStream);
                    LOOP2_ASSERT(bdemVersion, i,
                                 inStream.cursor() == outStream.length());
                    LOOP2_ASSERT(bdemVersion, i, isEqual(X, Y));
                    if (veryVerbose) { P(inStream.cursor())  P(Y) }
                }
            }
        }
        if (veryVerbose) bsl::cout
                                << "\tTesting MySequenceWithNullablesContainer"
                                << bsl::endl;
        {
            typedef test::MySequenceWithNullablesContainer Type;
            typedef bdem_List                              Proxy;
            Type DATA[test::MySequenceWithNullables::NUM_ATTRIBUTES];
            const int  NUM_DATA = sizeof DATA / sizeof *DATA;
            int tvi = 0;  // test value index
            for (int i = 0; i < NUM_DATA; ++i) {
                for (int j = 0; j < NUM_DATA; ++j) {
                    if (i == j) {
                        SetToNull setToNull;
                        LOOP2_ASSERT(
                               i, j,
                               0 == DATA[i].contained().manipulateAttribute(
                                                                setToNull, j));
                    }
                    else {
                        SetToNotNull setToNotNull(tvi);
                        ++tvi;
                        LOOP2_ASSERT(
                            i, j,
                            0 == DATA[i].contained().manipulateAttribute(
                                                             setToNotNull, j));
                    }
                }
            }
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                for (int i = 0; i < NUM_DATA; ++i) {
                    bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                    bdem_BdemEncoderOptions options;
                    options.setBdemVersion(bdemVersion);
                    Type mX = DATA[i];  const Type& X = mX;
                    LOOP2_ASSERT(bdemVersion, i,
                                 Encoder::encode(outStream, X, options));
                    LOOP2_ASSERT(bdemVersion, i, outStream);
                    if (veryVerbose) { P(outStream.length())  P(X) }
                    bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                    Proxy mY; const Proxy& Y = mY;
                    ISF::streamIn(inStream, mY, bdemVersion);
                    LOOP2_ASSERT(bdemVersion, i, inStream);
                    LOOP2_ASSERT(bdemVersion, i,
                                 inStream.cursor() == outStream.length());
                    LOOP2_ASSERT(bdemVersion, i, isEqual(X, Y));
                    if (veryVerbose) { P(inStream.cursor())  P(Y) }
                }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting MySequenceWithManyNullables"
                                   << bsl::endl;
        {
            typedef test::MySequenceWithManyNullables Type;
            typedef bdem_List                         Proxy;
            Type DATA[test::MySequenceWithManyNullables::NUM_ATTRIBUTES];
            const int  NUM_DATA = sizeof DATA / sizeof *DATA;
            int tvi = 0;  // test value index
            for (int i = 0; i < NUM_DATA; ++i) {
                for (int j = 0; j < NUM_DATA; ++j) {
                    if (i == j) {
                        SetToNull setToNull;
                        LOOP2_ASSERT(
                               i, j,
                               0 == DATA[i].manipulateAttribute(setToNull, j));
                    }
                    else {
                        SetToNotNull setToNotNull(tvi);
                        ++tvi;
                        LOOP2_ASSERT(
                            i, j,
                            0 == DATA[i].manipulateAttribute(setToNotNull, j));
                    }
                }
            }
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                for (int i = 0; i < NUM_DATA; ++i) {
                    bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                    bdem_BdemEncoderOptions options;
                    options.setBdemVersion(bdemVersion);
                    Type mX = DATA[i];  const Type& X = mX;
                    LOOP2_ASSERT(bdemVersion, i,
                                 Encoder::encode(outStream, X, options));
                    LOOP2_ASSERT(bdemVersion, i, outStream);
                    if (veryVerbose) { P(outStream.length())  P(X) }
                    bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                    Proxy mY; const Proxy& Y = mY;
                    ISF::streamIn(inStream, mY, bdemVersion);
                    LOOP2_ASSERT(bdemVersion, i, inStream);
                    LOOP2_ASSERT(bdemVersion, i,
                                 inStream.cursor() == outStream.length());
                    LOOP2_ASSERT(bdemVersion, i, isEqual(X, Y));
                    if (veryVerbose) { P(inStream.cursor())  P(Y) }
                }
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING VECTORS OF CUSTOMIZED TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Vectors of Customized Types"
                               << "\n==================================="
                               << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (verbose) bsl::cout << "\tCustomizedString" << bsl::endl;
        {
            typedef bsl::vector<test::CustomizedString> Type;
            typedef Type::value_type                    EType;
            typedef bsl::vector<bsl::string>            Proxy;
            const char *DATA[]   = { "", "a", "bc", "def", "ghij", "klmno" };
            const int   NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    Type mX;  const Type& X = mX;
                    for (int i = 0; i < ti; ++i) {
                        EType mE(DATA[i]);  const EType& E = mE;
                        mX.push_back(E);
                    }
                    bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                    bdem_BdemEncoderOptions options;
                    options.setBdemVersion(bdemVersion);
                    LOOP2_ASSERT(bdemVersion, ti,
                                 Encoder::encode(outStream, X, options));
                    LOOP2_ASSERT(bdemVersion, ti, outStream);
                    if (veryVerbose) { P(outStream.length())  P(X) }
                    bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                    Proxy mY;  const Proxy& Y = mY;
                    ISF::streamIn(inStream, mY, bdemVersion);
                    LOOP2_ASSERT(bdemVersion, ti, inStream);
                    LOOP2_ASSERT(bdemVersion, ti,
                                 inStream.cursor() == outStream.length());
                    LOOP2_ASSERT(bdemVersion, ti, X.size() == Y.size());
                    for (int i = 0; i < ti; ++i) {
                        EType mE;  const EType& E = mE;
                        LOOP3_ASSERT(bdemVersion, ti, i,
                                     0 == mE.fromString(Y[i]));
                        LOOP3_ASSERT(bdemVersion, ti, i, X[i] == E);
                    }
                    if (veryVerbose) { P(inStream.cursor())  P(Y) }
                }
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING CUSTOMIZED TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Customized Types"
                               << "\n========================"
                               << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (veryVerbose) bsl::cout << "\tTesting CustomizedString"
                                   << bsl::endl;
        {
            typedef test::CustomizedString Type;
            typedef bsl::string            Proxy;
            const Type DATA[]   = { Type(""),
                                    Type("a"),
                                    Type("bc"),
                                    Type("def"),
                                    Type("ghij"),
                                    Type("klmno") };
            const int  NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                for (int i = 0; i < NUM_DATA; ++i) {
                    bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                    bdem_BdemEncoderOptions options;
                    options.setBdemVersion(bdemVersion);
                    Type mX = DATA[i];  const Type& X = mX;
                    LOOP2_ASSERT(bdemVersion, i,
                                 Encoder::encode(outStream, X, options));
                    LOOP2_ASSERT(bdemVersion, i, outStream);
                    if (veryVerbose) { P(outStream.length())  P(X) }
                    bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                    Proxy mY; const Proxy& Y = mY;
                    ISF::streamIn(inStream, mY, bdemVersion);
                    LOOP2_ASSERT(bdemVersion, i, inStream);
                    LOOP2_ASSERT(bdemVersion, i,
                                 inStream.cursor() == outStream.length());
                    Type mZ;  const Type& Z = mZ;
                    LOOP2_ASSERT(bdemVersion, i, 0 == mZ.fromString(Y));
                    LOOP2_ASSERT(bdemVersion, i, X == Z);
                    if (veryVerbose) { P(inStream.cursor())  P(Y) }
                }
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING VECTORS OF ENUMERATION TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Vectors of Enumeration Types"
                               << "\n===================================="
                               << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (verbose) bsl::cout << "\tMyEnumeration" << bsl::endl;
        {
            typedef test::MyEnumeration         Wrapper;
            typedef bsl::vector<Wrapper::Value> Type;
            typedef Type::value_type            EType;
            typedef bsl::vector<int>            Proxy;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    Type mX;  const Type& X = mX;
                    for (int i = 0; i < SIZES[ti]; ++i) {
                        EType mE;  const EType& E = mE;
                        LOOP3_ASSERT(
                             bdemVersion, ti, i,
                             0 == TestValueFunctions::loadTestValue(&mE, tvi));
                        mX.push_back(E);
                        ++tvi;
                    }
                    bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                    bdem_BdemEncoderOptions options;
                    options.setBdemVersion(bdemVersion);
                    LOOP2_ASSERT(bdemVersion, ti,
                                 Encoder::encode(outStream, X, options));
                    LOOP2_ASSERT(bdemVersion, ti, outStream);
                    if (veryVerbose) { P(outStream.length())  P(X) }
                    bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                    Proxy mY;  const Proxy& Y = mY;
                    ISF::streamIn(inStream, mY, bdemVersion);
                    LOOP2_ASSERT(bdemVersion, ti, inStream);
                    LOOP2_ASSERT(bdemVersion, ti,
                                 inStream.cursor() == outStream.length());
                    LOOP2_ASSERT(bdemVersion, ti, X.size() == Y.size());
                    for (int i = 0; i < SIZES[ti]; ++i) {
                        EType mE;  const EType& E = mE;
                        LOOP3_ASSERT(bdemVersion, ti, i,
                                     0 == Wrapper::fromInt(&mE, Y[i]));
                        LOOP3_ASSERT(bdemVersion, ti, i, X[i] == E);
                    }
                    if (veryVerbose) { P(inStream.cursor())  P(Y) }
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING ENUMERATION TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Enumeration Types"
                               << "\n========================="
                               << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (veryVerbose) bsl::cout << "\tTesting MyEnumeration"
                                   << bsl::endl;
        {
            typedef test::MyEnumeration Wrapper;
            typedef Wrapper::Value      Type;
            typedef int                 Proxy;
            const Type DATA[]   = { test::MyEnumeration::VALUE1,
                                    test::MyEnumeration::VALUE2 };
            const int  NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                for (int i = 0; i < NUM_DATA; ++i) {
                    bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                    bdem_BdemEncoderOptions options;
                    options.setBdemVersion(bdemVersion);
                    Type mX = DATA[i];  const Type& X = mX;
                    LOOP2_ASSERT(bdemVersion, i,
                                 Encoder::encode(outStream, X, options));
                    LOOP2_ASSERT(bdemVersion, i, outStream);
                    if (veryVerbose) { P(outStream.length())  P(X) }
                    bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                    Proxy mY; const Proxy& Y = mY;
                    ISF::streamIn(inStream, mY, bdemVersion);
                    LOOP2_ASSERT(bdemVersion, i, inStream);
                    LOOP2_ASSERT(bdemVersion, i,
                                 inStream.cursor() == outStream.length());
                    Type mZ;  const Type& Z = mZ;
                    LOOP2_ASSERT(bdemVersion, i,
                                 0 == Wrapper::fromInt(&mZ, Y));
                    LOOP2_ASSERT(bdemVersion, i, X == Z);
                    if (veryVerbose) { P(inStream.cursor())  P(Y) }
                }
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING VECTORS OF CHOICE TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Vectors of Choice Types"
                               << "\n==============================="
                               << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (verbose) bsl::cout << "\tMyChoice" << bsl::endl;
        {
            typedef bsl::vector<test::MyChoice> Type;
            typedef Type::value_type            EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    Type mX;  const Type& X = mX;
                    for (int i = 0; i < SIZES[ti]; ++i) {
                        EType mE;  const EType& E = mE;
                        LOOP3_ASSERT(
                             bdemVersion, ti, i,
                             0 == TestValueFunctions::loadTestValue(&mE, tvi));
                        mX.push_back(E);
                        ++tvi;
                    }
                    bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                    bdem_BdemEncoderOptions options;
                    options.setBdemVersion(bdemVersion);
                    LOOP2_ASSERT(bdemVersion, ti,
                                 Encoder::encode(outStream, X, options));
                    LOOP2_ASSERT(bdemVersion, ti, outStream);
                    if (veryVerbose) { P(outStream.length())  P(X) }
                    bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                    bdem_Table mT;  const bdem_Table& T = mT;
                    mT.bdexStreamIn(inStream, bdemVersion);
                    LOOP2_ASSERT(bdemVersion, ti, inStream);
                    LOOP2_ASSERT(bdemVersion, ti,
                                 inStream.cursor() == outStream.length());
                    LOOP2_ASSERT(bdemVersion, ti,
                                 X.size()          == T.numRows());
                    for (int i = 0; i < SIZES[ti]; ++i) {
                        LOOP3_ASSERT(bdemVersion, ti, i,
                                     isEqual(X[i],
                                     bdem_List(T[i]).theList(0)));
                    }
                    if (veryVerbose) { P(inStream.cursor())  P(T) }
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING CHOICE TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Choice Types"
                               << "\n====================" << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (veryVerbose) bsl::cout << "\tTesting MyChoice with no selection"
                                   << bsl::endl;
        {
            typedef test::MyChoice Type;
            typedef bdem_List      Proxy;
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                bdem_BdemEncoderOptions options;
                options.setBdemVersion(bdemVersion);
                Type mX;  const Type& X = mX;
                LOOP_ASSERT(bdemVersion,
                            Encoder::encode(outStream, X, options));
                LOOP_ASSERT(bdemVersion, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Proxy mY; const Proxy& Y = mY;
                ISF::streamIn(inStream, mY, bdemVersion);
                LOOP_ASSERT(bdemVersion, inStream);
                LOOP_ASSERT(bdemVersion,
                            inStream.cursor() == outStream.length());
                LOOP_ASSERT(bdemVersion, isEqual(X, Y));
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting MyChoice with selection1"
                                   << bsl::endl;
        {
            typedef test::MyChoice Type;
            typedef int            SType;
            typedef bdem_List      Proxy;
            const SType DATA[]   = { 0, 1, 34, 321 };
            const int   NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                for (int i = 0; i < NUM_DATA; ++i) {
                    bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                    bdem_BdemEncoderOptions options;
                    options.setBdemVersion(bdemVersion);
                    Type mX;  const Type& X = mX;
                    mX.makeSelection1(DATA[i]);
                    LOOP2_ASSERT(bdemVersion, i,
                                 Encoder::encode(outStream, X, options));
                    LOOP2_ASSERT(bdemVersion, i, outStream);
                    if (veryVerbose) { P(outStream.length())  P(X) }
                    bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                    Proxy mY; const Proxy& Y = mY;
                    ISF::streamIn(inStream, mY, bdemVersion);
                    LOOP2_ASSERT(bdemVersion, i, inStream);
                    LOOP2_ASSERT(bdemVersion, i,
                                 inStream.cursor() == outStream.length());
                    LOOP2_ASSERT(bdemVersion, i, isEqual(X, Y));
                    if (veryVerbose) { P(inStream.cursor())  P(Y) }
                }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting MyChoice with selection2"
                                   << bsl::endl;
        {
            typedef test::MyChoice Type;
            typedef bsl::string    SType;
            typedef bdem_List      Proxy;
            const SType DATA[]   = { "", "asv", "fdf", "dffkljl" };
            const int   NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                for (int i = 0; i < NUM_DATA; ++i) {
                    bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                    bdem_BdemEncoderOptions options;
                    options.setBdemVersion(bdemVersion);
                    Type mX;  const Type& X = mX;
                    mX.makeSelection2(DATA[i]);
                    LOOP2_ASSERT(bdemVersion, i,
                                 Encoder::encode(outStream, X, options));
                    LOOP2_ASSERT(bdemVersion, i, outStream);
                    if (veryVerbose) { P(outStream.length())  P(X) }
                    bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                    Proxy mY; const Proxy& Y = mY;
                    ISF::streamIn(inStream, mY, bdemVersion);
                    LOOP2_ASSERT(bdemVersion, i, inStream);
                    LOOP2_ASSERT(bdemVersion, i,
                                 inStream.cursor() == outStream.length());
                    LOOP2_ASSERT(bdemVersion, i, isEqual(X, Y));
                    if (veryVerbose) { P(inStream.cursor())  P(Y) }
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING VECTORS OF SEQUENCE TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Vectors of Sequence Types"
                               << "\n================================="
                               << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (verbose) bsl::cout << "\tMySequence" << bsl::endl;
        {
            typedef bsl::vector<test::MySequence> Type;
            typedef Type::value_type              EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    Type mX;  const Type& X = mX;
                    for (int i = 0; i < SIZES[ti]; ++i) {
                        EType mE;  const EType& E = mE;
                        LOOP3_ASSERT(
                             bdemVersion, ti, i,
                             0 == TestValueFunctions::loadTestValue(&mE, tvi));
                        mX.push_back(E);
                        ++tvi;
                    }
                    bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                    bdem_BdemEncoderOptions options;
                    options.setBdemVersion(bdemVersion);
                    LOOP2_ASSERT(bdemVersion, ti,
                                 Encoder::encode(outStream, X, options));
                    LOOP2_ASSERT(bdemVersion, ti, outStream);
                    if (veryVerbose) { P(outStream.length())  P(X) }
                    bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                    bdem_Table mT;  const bdem_Table& T = mT;
                    mT.bdexStreamIn(inStream, bdemVersion);
                    LOOP2_ASSERT(bdemVersion, ti, inStream);
                    LOOP2_ASSERT(bdemVersion, ti,
                                 inStream.cursor() == outStream.length());
                    LOOP2_ASSERT(bdemVersion, ti,
                                 X.size()          == T.numRows());
                    for (int i = 0; i < SIZES[ti]; ++i) {
                        LOOP3_ASSERT(bdemVersion, ti, i,
                                     isEqual(X[i], bdem_List(T[i])));
                    }
                    if (veryVerbose) { P(inStream.cursor())  P(T) }
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING SEQUENCE TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Sequence Types"
                               << "\n======================" << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (veryVerbose) bsl::cout << "\tTesting MySequence" << bsl::endl;
        {
            typedef test::MySequence Type;
            typedef int              A1;
            typedef bsl::string      A2;
            typedef bdem_List        Proxy;
            static const struct {
                int d_lineNum;  // source line number
                A1  d_attr1;    // value for year
                A2  d_attr2;    // attribute2
                int d_version;  // bdem version
            } DATA[] = {
                //line  attr1   attr2    version
                //----  -----   -----    -------
                { L_,   1,      "",      1        },
                { L_,   23,     "fds",   1        },
                { L_,   -34,    "39dd",  1        },
                { L_,   1,      "",      2        },
                { L_,   23,     "fds",   2        },
                { L_,   -34,    "39dd",  2        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE    = DATA[ti].d_lineNum;
                const A1  ATTR1   = DATA[ti].d_attr1;
                const A2  ATTR2   = DATA[ti].d_attr2;
                const int VERSION = DATA[ti].d_version;
                bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                bdem_BdemEncoderOptions options;
                options.setBdemVersion(VERSION);
                Type mX;  const Type& X = mX;
                mX.attribute1() = ATTR1;
                mX.attribute2() = ATTR2;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X, options));
                LOOP_ASSERT(LINE, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Proxy mY; const Proxy& Y = mY;
                ISF::streamIn(inStream, mY, VERSION);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, isEqual(X, Y));
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
      } break;
      case 6: {
#if 0
        // --------------------------------------------------------------------
        // TESTING VECTORS OF EXTENDED TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Vectors of Extended Types"
                               << "\n================================="
                               << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (verbose) bsl::cout << "\tDatetimeTz" << bsl::endl;
        {
            typedef bsl::vector<bdet_DatetimeTz> Type;
            typedef Type::value_type             EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    Type mX;  const Type& X = mX;
                    for (int i = 0; i < SIZES[ti]; ++i) {
                        EType mE;  const EType& E = mE;
                        LOOP3_ASSERT(
                             bdemVersion, ti, i,
                             0 == TestValueFunctions::loadTestValue(&mE, tvi));
                        mX.push_back(E);
                        ++tvi;
                    }
                    bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                    bdem_BdemEncoderOptions options;
                    options.setBdemVersion(bdemVersion);
                    LOOP2_ASSERT(bdemVersion, ti,
                                 Encoder::encode(outStream, X, options));
                    LOOP2_ASSERT(bdemVersion, ti, outStream);
                    if (veryVerbose) { P(outStream.length())  P(X) }
                    bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                    bdem_Table mT;  const bdem_Table& T = mT;
                    mT.bdexStreamIn(inStream, bdemVersion);
                    LOOP2_ASSERT(bdemVersion, ti, inStream);
                    LOOP2_ASSERT(bdemVersion, ti,
                                 inStream.cursor() == outStream.length());
                    LOOP2_ASSERT(bdemVersion, ti,
                                 X.size()          == T.numRows());
                    for (int i = 0; i < SIZES[ti]; ++i) {
                        LOOP3_ASSERT(bdemVersion, ti, i,
                                     isEqual(X[i], bdem_List(T[i])));
                    }
                    if (veryVerbose) { P(inStream.cursor())  P(T) }
                }
            }
        }
        if (verbose) bsl::cout << "\tDateTz" << bsl::endl;
        {
            typedef bsl::vector<bdet_DateTz> Type;
            typedef Type::value_type         EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    Type mX;  const Type& X = mX;
                    for (int i = 0; i < SIZES[ti]; ++i) {
                        EType mE;  const EType& E = mE;
                        LOOP3_ASSERT(
                             bdemVersion, ti, i,
                             0 == TestValueFunctions::loadTestValue(&mE, tvi));
                        mX.push_back(E);
                        ++tvi;
                    }
                    bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                    bdem_BdemEncoderOptions options;
                    options.setBdemVersion(bdemVersion);
                    LOOP2_ASSERT(bdemVersion, ti,
                                 Encoder::encode(outStream, X, options));
                    LOOP2_ASSERT(bdemVersion, ti, outStream);
                    if (veryVerbose) { P(outStream.length())  P(X) }
                    bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                    bdem_Table mT;  const bdem_Table& T = mT;
                    mT.bdexStreamIn(inStream, bdemVersion);
                    LOOP2_ASSERT(bdemVersion, ti, inStream);
                    LOOP2_ASSERT(bdemVersion, ti,
                                 inStream.cursor() == outStream.length());
                    LOOP2_ASSERT(bdemVersion, ti,
                                 X.size()          == T.numRows());
                    for (int i = 0; i < SIZES[ti]; ++i) {
                        LOOP3_ASSERT(bdemVersion, ti, i,
                                     isEqual(X[i], bdem_List(T[i])));
                    }
                    if (veryVerbose) { P(inStream.cursor())  P(T) }
                }
            }
        }
        if (verbose) bsl::cout << "\tTimeTz" << bsl::endl;
        {
            typedef bsl::vector<bdet_TimeTz> Type;
            typedef Type::value_type         EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int bdemVersion = 1; bdemVersion <= 2; ++bdemVersion) {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    Type mX;  const Type& X = mX;
                    for (int i = 0; i < SIZES[ti]; ++i) {
                        EType mE;  const EType& E = mE;
                        LOOP3_ASSERT(
                             bdemVersion, ti, i,
                             0 == TestValueFunctions::loadTestValue(&mE, tvi));
                        mX.push_back(E);
                        ++tvi;
                    }
                    bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                    bdem_BdemEncoderOptions options;
                    options.setBdemVersion(bdemVersion);
                    LOOP2_ASSERT(bdemVersion, ti,
                                 Encoder::encode(outStream, X, options));
                    LOOP2_ASSERT(bdemVersion, ti, outStream);
                    if (veryVerbose) { P(outStream.length())  P(X) }
                    bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                    bdem_Table mT;  const bdem_Table& T = mT;
                    mT.bdexStreamIn(inStream, bdemVersion);
                    LOOP2_ASSERT(bdemVersion, ti, inStream);
                    LOOP4_ASSERT(bdemVersion, ti,
                                 inStream.cursor(),   outStream.length(),
                                 inStream.cursor() == outStream.length());
                    LOOP2_ASSERT(bdemVersion, ti,
                                 X.size()          == T.numRows());
                    for (int i = 0; i < SIZES[ti]; ++i) {
                        LOOP3_ASSERT(bdemVersion, ti, i,
                                     isEqual(X[i], bdem_List(T[i])));
                    }
                    if (veryVerbose) { P(inStream.cursor())  P(T) }
                }
            }
        }
#endif
      } break;
      case 5: {
#if 0
        // --------------------------------------------------------------------
        // TESTING EXTENDED TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Extended Types"
                               << "\n======================" << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (veryVerbose) bsl::cout << "\tTesting DatetimeTz" << bsl::endl;
        {
            typedef bdet_DatetimeTz Type;
            typedef bdem_List       Proxy;
            typedef bdet_Datetime   E1Type;
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // value for year
                int d_month;    // value for month
                int d_day;      // value for day
                int d_hour;     // value for hour
                int d_minute;   // value for minute
                int d_second;   // value for second
                int d_offset;   // value for offset
                int d_version;  // bdem version
            } DATA[] = {
                //line  year  month  day  hour  minute  second  offset  version
                //----  ----  -----  ---  ----  ------  ------  ------  -------
                { L_,   1999,    1,   1,   11,     45,     33,       0,   1  },
                { L_,   2005,   10,   5,   10,     32,     25,      28,   1  },
                { L_,   1999,    1,   1,   11,     45,     33,       0,   2  },
                { L_,   2005,   10,   5,   10,     32,     25,      28,   2  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE    = DATA[ti].d_lineNum;
                const int YEAR    = DATA[ti].d_year;
                const int MONTH   = DATA[ti].d_month;
                const int DAY     = DATA[ti].d_day;
                const int HOUR    = DATA[ti].d_hour;
                const int MINUTE  = DATA[ti].d_minute;
                const int SECOND  = DATA[ti].d_second;
                const int OFFSET  = DATA[ti].d_offset;
                const int VERSION = DATA[ti].d_version;
                bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                bdem_BdemEncoderOptions options;
                options.setBdemVersion(VERSION);
                Type  mX(E1Type(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND),
                         OFFSET);
                const Type&  X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X, options));
                LOOP_ASSERT(LINE, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Proxy mY; const Proxy& Y = mY;
                ISF::streamIn(inStream, mY, VERSION);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, isEqual(X, Y));
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting DateTz" << bsl::endl;
        {
            typedef bdet_DateTz Type;
            typedef bdem_List   Proxy;
            typedef bdet_Date   E1Type;
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // value for year
                int d_month;    // value for month
                int d_day;      // value for day
                int d_offset;   // value for offset
                int d_version;  // bdem version
            } DATA[] = {
                //line  year  month  day  offset  version
                //----  ----  -----  ---  ------  -------
                { L_,   1999,    1,   1,       0,   1  },
                { L_,   2005,   10,   5,      28,   1  },
                { L_,   1999,    1,   1,       0,   2  },
                { L_,   2005,   10,   5,      28,   2  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE    = DATA[ti].d_lineNum;
                const int YEAR    = DATA[ti].d_year;
                const int MONTH   = DATA[ti].d_month;
                const int DAY     = DATA[ti].d_day;
                const int OFFSET  = DATA[ti].d_offset;
                const int VERSION = DATA[ti].d_version;
                bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                bdem_BdemEncoderOptions options;
                options.setBdemVersion(VERSION);
                Type  mX(E1Type(YEAR, MONTH, DAY), OFFSET);
                const Type&  X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X, options));
                LOOP_ASSERT(LINE, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Proxy mY; const Proxy& Y = mY;
                ISF::streamIn(inStream, mY, VERSION);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, isEqual(X, Y));
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting TimeTz" << bsl::endl;
        {
            typedef bdet_TimeTz Type;
            typedef bdem_List   Proxy;
            typedef bdet_Time   E1Type;
            static const struct {
                int d_lineNum;  // source line number
                int d_hour;     // value for hour
                int d_minute;   // value for minute
                int d_second;   // value for second
                int d_offset;   // value for offset
                int d_version;  // bdem version
            } DATA[] = {
                //line  hour  minute  second  offset  version
                //----  ----  ------  ------  ------  -------
                { L_,    11,     45,     33,       0,   1  },
                { L_,    10,     32,     25,      28,   1  },
                { L_,    11,     45,     33,       0,   2  },
                { L_,    10,     32,     25,      28,   2  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE    = DATA[ti].d_lineNum;
                const int HOUR    = DATA[ti].d_hour;
                const int MINUTE  = DATA[ti].d_minute;
                const int SECOND  = DATA[ti].d_second;
                const int OFFSET  = DATA[ti].d_offset;
                const int VERSION = DATA[ti].d_version;
                bdex_ByteOutStreamRaw   outStream(buffer, BUFSIZE);
                bdem_BdemEncoderOptions options;
                options.setBdemVersion(VERSION);
                Type  mX(E1Type(HOUR, MINUTE, SECOND), OFFSET);
                const Type&  X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X, options));
                LOOP_ASSERT(LINE, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Proxy mY; const Proxy& Y = mY;
                ISF::streamIn(inStream, mY, VERSION);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, isEqual(X, Y));
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING VECTORS OF 'bdem' PROXIED TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Vectors of 'bdem' Proxied Types"
                               << "\n======================================="
                               << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (veryVerbose) bsl::cout << "\tTesting 'bsl::vector<bool>'"
                                   << bsl::endl;
        {
            typedef bsl::vector<bool> Type;
            typedef             bool  EType;
            typedef bsl::vector<char> Proxy;
            typedef             char  ProxyE;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Proxy mY;  const Proxy& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X.size() == Y.size());
                for (int i = 0; i < X.size(); ++i) {
                    LOOP2_ASSERT(ti, i, (ProxyE)X[i] == Y[i]);
                }
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting 'bsl::vector<signed char>'"
                                   << bsl::endl;
        {
            typedef bsl::vector<signed char> Type;
            typedef             signed char  EType;
            typedef bsl::vector<char>        Proxy;
            typedef             char         ProxyE;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Proxy mY;  const Proxy& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X.size() == Y.size());
                for (int i = 0; i < X.size(); ++i) {
                    LOOP2_ASSERT(ti, i, (ProxyE)X[i] == Y[i]);
                }
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting 'bsl::vector<unsigned char>'"
                                   << bsl::endl;
        {
            typedef bsl::vector<unsigned char> Type;
            typedef             unsigned char  EType;
            typedef bsl::vector<char>          Proxy;
            typedef             char           ProxyE;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Proxy mY;  const Proxy& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X.size() == Y.size());
                for (int i = 0; i < X.size(); ++i) {
                    LOOP2_ASSERT(ti, i, (ProxyE)X[i] == Y[i]);
                }
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting 'bsl::vector<unsigned short>'"
                                   << bsl::endl;
        {
            typedef bsl::vector<unsigned short> Type;
            typedef             unsigned short  EType;
            typedef bsl::vector<short>          Proxy;
            typedef             short           ProxyE;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Proxy mY;  const Proxy& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X.size() == Y.size());
                for (int i = 0; i < X.size(); ++i) {
                    LOOP2_ASSERT(ti, i, (ProxyE)X[i] == Y[i]);
                }
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting 'bsl::vector<unsigned int>'"
                                   << bsl::endl;
        {
            typedef bsl::vector<unsigned int> Type;
            typedef             unsigned int  EType;
            typedef bsl::vector<int>          Proxy;
            typedef             int           ProxyE;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Proxy mY;  const Proxy& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X.size() == Y.size());
                for (int i = 0; i < X.size(); ++i) {
                    LOOP2_ASSERT(ti, i, (ProxyE)X[i] == Y[i]);
                }
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting 'bsl::vector<Uint64>'"
                                   << bsl::endl;
        {
            typedef bsl::vector<bsls_Types::Uint64> Type;
            typedef             bsls_Types::Uint64  EType;
            typedef bsl::vector<bsls_Types::Int64>  Proxy;
            typedef             bsls_Types::Int64   ProxyE;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Proxy mY;  const Proxy& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X.size() == Y.size());
                for (int i = 0; i < X.size(); ++i) {
                    LOOP2_ASSERT(ti, i, (ProxyE)X[i] == Y[i]);
                }
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting 'bsl::vector<enum>'"
                                   << bsl::endl;
        {
            typedef bsl::vector<Enum> Type;
            typedef             Enum  EType;
            typedef bsl::vector<int>  Proxy;
            typedef             int   ProxyE;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Proxy mY;  const Proxy& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X.size() == Y.size());
                for (int i = 0; i < X.size(); ++i) {
                    LOOP2_ASSERT(ti, i, (ProxyE)X[i] == Y[i]);
                }
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'bdem' PROXIED TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting 'bdem' Proxied Types"
                               << "\n============================"
                               << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (veryVerbose) bsl::cout << "\tTesting 'bool'" << bsl::endl;
        {
            typedef bool Type;
            typedef char Proxy;
            static const struct {
                int  d_lineNum;  // source line number
                Type d_value;    // value to encode
            } DATA[] = {
                //line    value
                //----    -----
                { L_,     false    },
                { L_,     true     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const Type VALUE = DATA[ti].d_value;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX = VALUE;  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                LOOP_ASSERT(LINE, sizeof(Proxy) == outStream.length());
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Proxy mY = (Proxy)!VALUE;  const Proxy& Y = mY;
                LOOP_ASSERT(LINE, (Proxy)X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, (Proxy)X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting 'signed char'" << bsl::endl;
        {
            typedef signed char Type;
            typedef char        Proxy;
            static const struct {
                int  d_lineNum;  // source line number
                Type d_value;    // value to encode
            } DATA[] = {
                //line    value
                //----    -----
                { L_,     0           },
                { L_,     1           },
                { L_,     SCHAR_MIN   },
                { L_,     SCHAR_MAX   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const Type VALUE = DATA[ti].d_value;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX = VALUE;  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                LOOP_ASSERT(LINE, sizeof(Proxy) == outStream.length());
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Proxy mY = (Proxy)~VALUE;  const Proxy& Y = mY;
                LOOP_ASSERT(LINE, (Proxy)X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, (Proxy)X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting 'unsigned char'" << bsl::endl;
        {
            typedef unsigned char Type;
            typedef char          Proxy;
            static const struct {
                int  d_lineNum;  // source line number
                Type d_value;    // value to encode
            } DATA[] = {
                //line    value
                //----    -----
                { L_,     0           },
                { L_,     1           },
                { L_,     UCHAR_MAX   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const Type VALUE = DATA[ti].d_value;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX = VALUE;  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                LOOP_ASSERT(LINE, sizeof(Proxy) == outStream.length());
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Proxy mY = (Proxy)~VALUE;  const Proxy& Y = mY;
                LOOP_ASSERT(LINE, (Proxy)X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, (Proxy)X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(X) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting 'unsigned short'"
                                   << bsl::endl;
        {
            typedef unsigned short Type;
            typedef short          Proxy;
            static const struct {
                int  d_lineNum;  // source line number
                Type d_value;    // value to encode
            } DATA[] = {
                //line    value
                //----    -----
                { L_,     0           },
                { L_,     1           },
                { L_,     UCHAR_MAX   },
                { L_,     USHRT_MAX   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const Type VALUE = DATA[ti].d_value;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX = VALUE;  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                LOOP_ASSERT(LINE, sizeof(Proxy) == outStream.length());
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Proxy mY = (Proxy)~VALUE;  const Proxy& Y = mY;
                LOOP_ASSERT(LINE, (Proxy)X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, (Proxy)X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(X) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting 'unsigned int'" << bsl::endl;
        {
            typedef unsigned int Type;
            typedef int          Proxy;
            static const struct {
                int  d_lineNum;  // source line number
                Type d_value;    // value to encode
            } DATA[] = {
                //line    value
                //----    -----
                { L_,     0           },
                { L_,     1           },
                { L_,     UCHAR_MAX   },
                { L_,     USHRT_MAX   },
                { L_,     UINT_MAX    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const Type VALUE = DATA[ti].d_value;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX = VALUE;  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                LOOP_ASSERT(LINE, sizeof(Proxy) == outStream.length());
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Proxy mY = (Proxy)~VALUE;  const Proxy& Y = mY;
                LOOP_ASSERT(LINE, (Proxy)X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, (Proxy)X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(X) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting 'Uint64'" << bsl::endl;
        {
            typedef bsls_Types::Uint64 Type;
            typedef bsls_Types::Int64  Proxy;
            static const struct {
                int  d_lineNum;  // source line number
                Type d_value;    // value to decode
            } DATA[] = {
                //line    value
                //----    -----
                { L_,     0              },
                { L_,     1              },
                { L_,     UCHAR_MAX      },
                { L_,     USHRT_MAX      },
                { L_,     UINT_MAX       },
                { L_,     ULLONG_MAX     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const Type VALUE = DATA[ti].d_value;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX = VALUE;  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                LOOP_ASSERT(LINE, sizeof(Proxy) == outStream.length());
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Proxy mY = (Proxy)~VALUE;  const Proxy& Y = mY;
                LOOP_ASSERT(LINE, (Proxy)X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, (Proxy)X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(X) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting 'enum'" << bsl::endl;
        {
            typedef Enum Type;
            typedef int  Proxy;
            static const struct {
                int  d_lineNum;  // source line number
                Type d_value;    // value to encode
                Type d_other;    // value to overwrite
            } DATA[] = {
                //line    value
                //----    -----
                { L_,     e1,     e2     },
                { L_,     e2,     e3     },
                { L_,     e3,     e1     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const Type VALUE = DATA[ti].d_value;
                const Type OTHER = DATA[ti].d_other;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX = VALUE;  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                LOOP_ASSERT(LINE, sizeof(Proxy) == outStream.length());
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Proxy mY = (Proxy)OTHER;  const Proxy& Y = mY;
                LOOP_ASSERT(LINE, (Proxy)X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, (Proxy)X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(X) }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING VECTORS OF 'bdem' BASE TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting Vectors of 'bdem' Base Types"
                               << "\n===================================="
                               << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (veryVerbose) bsl::cout << "\tTesting CHAR_ARRAY" << bsl::endl;
        {
            typedef bsl::vector<char> Type;  // 'bdem' element type
                                             // 'CHAR_ARRAY'
            typedef             char  EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Type mY;  const Type& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting SHORT_ARRAY" << bsl::endl;
        {
            typedef bsl::vector<short> Type;  // 'bdem' element type
                                              // 'SHORT_ARRAY'
            typedef             short  EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Type mY;  const Type& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting INT_ARRAY" << bsl::endl;
        {
            typedef bsl::vector<int> Type;  // 'bdem' element type 'INT_ARRAY'
            typedef             int  EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Type mY;  const Type& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting INT64_ARRAY" << bsl::endl;
        {
            typedef bsl::vector<bsls_Types::Int64> Type;
                                           // 'bdem' element type 'INT64_ARRAY'
            typedef             bsls_Types::Int64  EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Type mY;  const Type& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting FLOAT_ARRAY" << bsl::endl;
        {
            typedef bsl::vector<float> Type;  // 'bdem' element type
                                              // 'FLOAT_ARRAY'
            typedef             float  EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Type mY;  const Type& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting DOUBLE_ARRAY" << bsl::endl;
        {
            typedef bsl::vector<double> Type;  // 'bdem' element type
                                               // 'DOUBLE_ARRAY'
            typedef             double  EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Type mY;  const Type& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting STRING_ARRAY" << bsl::endl;
        {
            typedef bsl::vector<bsl::string> Type;  // 'bdem' element type
                                                    // 'STRING_ARRAY'
            typedef             bsl::string  EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Type mY;  const Type& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting DATETIME_ARRAY" << bsl::endl;
        {
            typedef bsl::vector<bdet_Datetime> Type;  // 'bdem' element type
                                                      // 'DATETIME_ARRAY'
            typedef             bdet_Datetime  EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Type mY;  const Type& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting DATE_ARRAY" << bsl::endl;
        {
            typedef bsl::vector<bdet_Date> Type;  // 'bdem' element type
                                                  // 'DATE_ARRAY'
            typedef             bdet_Date  EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Type mY;  const Type& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting TIME_ARRAY" << bsl::endl;
        {
            typedef bsl::vector<bdet_Time> Type;  // 'bdem' element type
                                                  // 'TIME_ARRAY'
            typedef             bdet_Time  EType;
            const int SIZES[]  = { 0, 1, 2, 3, 5 };
            const int NUM_DATA = sizeof SIZES / sizeof *SIZES;
            int tvi = 0;  // test value index
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                Type mX;  const Type& X = mX;
                for (int i = 0; i < SIZES[ti]; ++i) {
                    EType mE;  const EType& E = mE;
                    LOOP2_ASSERT(ti, i, 0 ==
                           TestValueFunctions::loadTestValue(&mE, tvi));
                    mX.push_back(E);
                    ++tvi;
                }
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                LOOP_ASSERT(ti, Encoder::encode(outStream, X));
                LOOP_ASSERT(ti, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                Type mY;  const Type& Y = mY;
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(ti, inStream);
                LOOP_ASSERT(ti, inStream.cursor() == outStream.length());
                LOOP_ASSERT(ti, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'bdem' BASE TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTesting 'bdem' Base Types"
                               << "\n=========================" << bsl::endl;
        const int BUFSIZE = 1024;  char buffer[BUFSIZE];
        if (veryVerbose) bsl::cout << "\tTesting CHAR" << bsl::endl;
        {
            typedef char Type;  // 'bdem' element type 'CHAR'
            static const struct {
                int  d_lineNum;  // source line number
                Type d_value;    // value to encode
            } DATA[] = {
                //line    value
                //----    -----
                { L_,     0          },
                { L_,     1          },
                { L_,     CHAR_MIN   },
                { L_,     CHAR_MAX   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const Type VALUE = DATA[ti].d_value;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX = VALUE;  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, sizeof(Type) == outStream.length());
                LOOP_ASSERT(LINE, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Type mY = ~VALUE;  const Type& Y = mY;
                LOOP_ASSERT(LINE, X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting SHORT" << bsl::endl;
        {
            typedef short Type;  // 'bdem' element type 'SHORT'
            static const struct {
                int  d_lineNum;  // source line number
                Type d_value;    // value to encode
            } DATA[] = {
                //line    value
                //----    -----
                { L_,     0          },
                { L_,     1          },
                { L_,     CHAR_MIN   },
                { L_,     CHAR_MAX   },
                { L_,     SHRT_MIN   },
                { L_,     SHRT_MAX   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const Type VALUE = DATA[ti].d_value;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX = VALUE;  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                LOOP_ASSERT(LINE, sizeof(Type) == outStream.length());
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Type mY = ~VALUE;  const Type& Y = mY;
                LOOP_ASSERT(LINE, X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting INT" << bsl::endl;
        {
            typedef int Type;  // 'bdem' element type 'INT'
            static const struct {
                int  d_lineNum;  // source line number
                Type d_value;    // value to encode
            } DATA[] = {
                //line    value
                //----    -----
                { L_,     0          },
                { L_,     1          },
                { L_,     CHAR_MIN   },
                { L_,     CHAR_MAX   },
                { L_,     SHRT_MIN   },
                { L_,     SHRT_MAX   },
                { L_,     INT_MIN    },
                { L_,     INT_MAX    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const Type VALUE = DATA[ti].d_value;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX = VALUE;  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                LOOP_ASSERT(LINE, sizeof(Type) == outStream.length());
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Type mY = ~VALUE;  const Type& Y = mY;
                LOOP_ASSERT(LINE, X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting INT64" << bsl::endl;
        {
            typedef bsls_Types::Int64 Type;  // 'bdem' element type 'INT64'
            static const struct {
                int  d_lineNum;  // source line number
                Type d_value;    // value to encode
            } DATA[] = {
                //line    value
                //----    -----
                { L_,     0             },
                { L_,     1             },
                { L_,     CHAR_MIN      },
                { L_,     CHAR_MAX      },
                { L_,     SHRT_MIN      },
                { L_,     SHRT_MAX      },
                { L_,     INT_MIN       },
                { L_,     INT_MAX       },
                { L_,     LLONG_MIN     },
                { L_,     LLONG_MAX     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const Type VALUE = DATA[ti].d_value;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX = VALUE;  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                LOOP_ASSERT(LINE, sizeof(Type) == outStream.length());
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Type mY = ~VALUE;  const Type& Y = mY;
                LOOP_ASSERT(LINE, X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting FLOAT" << bsl::endl;
        {
            typedef float Type;  // 'bdem' element type 'FLOAT'
            static const struct {
                int  d_lineNum;  // source line number
                Type d_value;    // value to encode
            } DATA[] = {
                //line     value
                //----   ---------
                { L_,       0.0           },
                { L_,       1.25          },
                { L_,     247.66          },
                { L_,    - 77.33          },
                { L_,    -  0.0054        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const Type VALUE = DATA[ti].d_value;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX = VALUE;  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                LOOP_ASSERT(LINE, sizeof(Type) == outStream.length());
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Type mY = VALUE + 0.5;  const Type& Y = mY;
                LOOP_ASSERT(LINE, X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting DOUBLE" << bsl::endl;
        {
            typedef double Type;  // 'bdem' element type 'DOUBLE'
            static const struct {
                int  d_lineNum;  // source line number
                Type d_value;    // value to encode
            } DATA[] = {
                //line     value
                //----   ---------
                { L_,       0.0           },
                { L_,       1.25          },
                { L_,     247.66          },
                { L_,    - 77.33          },
                { L_,    -  0.0054        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const Type VALUE = DATA[ti].d_value;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX = VALUE;  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                LOOP_ASSERT(LINE, sizeof(Type) == outStream.length());
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Type mY = VALUE + 0.5;  const Type& Y = mY;
                LOOP_ASSERT(LINE, X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting STRING" << bsl::endl;
        {
            typedef bsl::string Type;  // 'bdem' element type 'STRING'
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_value;    // value to encode
                const char *d_other;    // value to overwrite
            } DATA[] = {
                //line    value    other
                //----    -----    -----
                { L_,     ""   ,   "abc"      },
                { L_,     "123",   ""         },
                { L_,     "1"  ,   "23"       },
                { L_,     "xyz",   "E"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_lineNum;
                const char *VALUE = DATA[ti].d_value;
                const char *OTHER = DATA[ti].d_other;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX(VALUE);  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Type mY(OTHER);  const Type& Y = mY;
                LOOP_ASSERT(LINE, X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting DATETIME" << bsl::endl;
        {
            typedef bdet_Datetime Type;  // 'bdem' element type 'DATETIME'
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // value for year
                int d_month;    // value for month
                int d_day;      // value for day
                int d_hour;     // value for hour
                int d_minute;   // value for minute
                int d_second;   // value for second
            } DATA[] = {
                //line  year  month  day  hour  minute  second
                //----  ----  -----  ---  ----  ------  ------
                { L_,   1999,    1,   1,   11,     45,     33,       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE   = DATA[ti].d_lineNum;
                const int YEAR   = DATA[ti].d_year;
                const int MONTH  = DATA[ti].d_month;
                const int DAY    = DATA[ti].d_day;
                const int HOUR   = DATA[ti].d_hour;
                const int MINUTE = DATA[ti].d_minute;
                const int SECOND = DATA[ti].d_second;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                      Type  mX(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND);
                const Type&  X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                      Type  mY(YEAR + 1, MONTH + 1, DAY + 1,
                               HOUR + 1, MINUTE + 1, SECOND + 1);
                const Type&  Y = mY;
                LOOP_ASSERT(LINE, X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting DATE" << bsl::endl;
        {
            typedef bdet_Date Type;  // 'bdem' element type 'DATE'
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // value for year
                int d_month;    // value for month
                int d_day;      // value for day
            } DATA[] = {
                //line  year  month  day
                //----  ----  -----  ---
                { L_,   1999,    1,   1,   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int YEAR  = DATA[ti].d_year;
                const int MONTH = DATA[ti].d_month;
                const int DAY   = DATA[ti].d_day;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX(YEAR, MONTH, DAY);  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Type mY(YEAR + 1, MONTH + 1, DAY + 1);  const Type& Y = mY;
                LOOP_ASSERT(LINE, X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
            }
        }
        if (veryVerbose) bsl::cout << "\tTesting TIME" << bsl::endl;
        {
            typedef bdet_Time Type;  // 'bdem' element type 'TIME'
            static const struct {
                int d_lineNum;  // source line number
                int d_hour;     // value for hour
                int d_minute;   // value for minute
                int d_second;   // value for second
            } DATA[] = {
                //line  hour  minute  second
                //----  ----  ------  ------
                { L_,   11,     45,     33,       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE   = DATA[ti].d_lineNum;
                const int HOUR   = DATA[ti].d_hour;
                const int MINUTE = DATA[ti].d_minute;
                const int SECOND = DATA[ti].d_second;
                bdex_ByteOutStreamRaw outStream(buffer, BUFSIZE);
                Type mX(HOUR, MINUTE, SECOND);  const Type& X = mX;
                LOOP_ASSERT(LINE, Encoder::encode(outStream, X));
                LOOP_ASSERT(LINE, outStream);
                if (veryVerbose) { P(outStream.length())  P(X) }
                bdex_ByteInStreamRaw inStream(buffer, BUFSIZE);
                Type mY(HOUR + 1, MINUTE + 1, SECOND + 1);  const Type& Y = mY;
                LOOP_ASSERT(LINE, X != Y);
                int version = VF::maxSupportedVersion(Y);
                ISF::streamIn(inStream, mY, version);
                LOOP_ASSERT(LINE, inStream);
                LOOP_ASSERT(LINE, inStream.cursor() == outStream.length());
                LOOP_ASSERT(LINE, X == Y);
                if (veryVerbose) { P(inStream.cursor())  P(Y) }
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
