// bdem_beruniversaltagnumber.t.cpp                                   -*-C++-*-
#include <bdem_beruniversaltagnumber.h>

#include <bdeat_arrayfunctions.h>

#include <bsls_types.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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
}

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
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
#define T_ cout << "\t" << flush; // tab

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

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

class bslma_Allocator;

namespace test {

class CustomizedString {

  private:
    // PRIVATE DATA MEMBERS
    bsl::string d_value;  // stored value

    // FRIENDS
    friend bool operator==(const CustomizedString& lhs,
                           const CustomizedString& rhs);
    friend bool operator!=(const CustomizedString& lhs,
                           const CustomizedString& rhs);

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

    CustomizedString(const CustomizedString& original,
                     bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit CustomizedString(const bsl::string& value,
                              bslma_Allocator *basicAllocator = 0);
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
CustomizedString::CustomizedString(const CustomizedString& original,
                                   bslma_Allocator *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
CustomizedString::CustomizedString(const bsl::string& value,
                                   bslma_Allocator *basicAllocator)
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
//      Property of Bloomberg L.P.  (BLP)
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
//      Property of Bloomberg L.P.  (BLP)
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

class bslma_Allocator;

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
//      Property of Bloomberg L.P.  (BLP)
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
//      Property of Bloomberg L.P.  (BLP)
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
        VALUE1 = 1,
            // todo: provide annotation
        VALUE2 = 2
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
bsl::ostream& test::operator<<(bsl::ostream& stream,
                               test::MyEnumeration::Value rhs)
{
    return test::MyEnumeration::print(stream, rhs);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
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
                        return SUCCESS;
                                                                      // RETURN
                    } break;
                    case '2': {
                        *result = MyEnumeration::VALUE2;
                        return SUCCESS;
                                                                      // RETURN
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
//      Property of Bloomberg L.P.  (BLP)
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

class bslma_Allocator;

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

    MySequence(const MySequence& original,
               bslma_Allocator *basicAllocator = 0);
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
bsl::ostream& test::operator<<(bsl::ostream& stream,
                               const test::MySequence& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
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
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

namespace BloombergLP {
namespace test {

// Declare a dynamic type
class MyDynamicType {
    int d_category;

  public:
    enum Value { ENUM_VAL };

    MyDynamicType(int category) : d_category(category) { }
    int category() const { return d_category; }
};

bdeat_TypeCategory::Value bdeat_typeCategorySelect(const MyDynamicType& obj)
{
    return bdeat_TypeCategory::Value(obj.category());
}

}  // close namespace test;

namespace bdeat_SequenceFunctions {

    template <>
    struct IsSequence<test::MyDynamicType> : bslmf_MetaInt<1> { };
}

namespace bdeat_ChoiceFunctions {

    template <>
    struct IsChoice<test::MyDynamicType> : bslmf_MetaInt<1> { };

}  // close namespace bdeat_ChoiceFunctions

namespace bdeat_ArrayFunctions {

    template <>
    struct IsArray<test::MyDynamicType> : bslmf_MetaInt<1> { };

    template <>
    struct ElementType<test::MyDynamicType> { typedef int Type; };

}  // close namespace bdeat_ArrayFunctions

namespace bdeat_EnumFunctions {

    template <>
    struct IsEnumeration<test::MyDynamicType> : bslmf_MetaInt<1> { };

} // close namespace bdeat_EnumFunctions

namespace bdeat_NullableValueFunctions {

    template <>
    struct IsNullableValue<test::MyDynamicType> : bslmf_MetaInt<1> { };

    template <>
    struct ValueType<test::MyDynamicType> { typedef int Type; };

}  // close namespace bdeat_NullableValueFunctions

}  // close namespace BloombergLP;

typedef bdem_BerUniversalTagNumber        Class;
typedef bdem_BerUniversalTagNumber::Value Enum;
const int                                 NUM_ENUMS = Class::BDEM_LENGTH;
typedef bdeat_FormattingMode              FM;
typedef bdeat_TypeCategory                TC;

//=============================================================================
//                       HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#define TEST_SELECT(type, formattingMode, expected) {                         \
        type object;                                                          \
        const bdem_BerUniversalTagNumber::Value expectedResult = expected;    \
        bdem_BerUniversalTagNumber::Value result =                            \
            bdem_BerUniversalTagNumber::select(object, formattingMode);       \
        LOOP2_ASSERT(expectedResult, result, expectedResult == result);       \
        bdem_BerUniversalTagNumber::Value result2 =                           \
            bdem_BerUniversalTagNumber::select(object,                        \
                                        formattingMode | FM::BDEAT_UNTAGGED); \
        LOOP2_ASSERT(expectedResult, result2, expectedResult == result2);     \
    }
    // Test select() function, both with unadorned 'formattingMode' and with
    // an extra bit set in 'formattingMode'.

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
//    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE1
        //   This will test 'Usage 1'.
        //
        // Concerns:
        //   The usage example must work as expected.
        //
        // Plan:
        //   Copy the usage example from the header file, replacing 'assert'
        //   with 'ASSERT', and verify that it compiles and runs as expected.
        //
        // Testing:
        //   Usage 1
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Usage 1"
                               << "\n===============" << bsl::endl;

        bdem_BerUniversalTagNumber::Value tagNumber
                                    = bdem_BerUniversalTagNumber::BDEM_BER_INT;

        const char *rep = bdem_BerUniversalTagNumber::toString(tagNumber);
        ASSERT(0 == strcmp(rep, "INT"));

        bsl::stringstream ss;
        ss << tagNumber;
        ASSERT("INT" == ss.str());

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING SELECT FUNCTION
        //   This will test the 'Select<TYPE>::value()' function.
        //
        // Concerns:
        //   The function must return the correct universal tag number.
        //
        // Plan:
        //   Use a table of types/formatting mode -> result and check all of
        //   them.
        //
        // Testing:
        //   Enum Select<TYPE>::value(FormattingMode);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting SELECT FUNCTION"
                          << "\n=======================" << endl;

        using namespace test;

        typedef signed char schar;
        typedef unsigned char uchar;
        typedef unsigned short ushort;
        typedef unsigned int uint;
        typedef bsls_Types::Int64 int64;
        typedef bsls_Types::Uint64 uint64;
        typedef bsl::vector<char> vectorChar;
        typedef test::CustomizedString CustString;
        typedef test::MyEnumeration::Value MyEnum;

        if (veryVerbose) cout << "Testing non-dynamic types" << endl;

        //          type         formatting mode   expected result
        //          ----         ---------------   ---------------
        TEST_SELECT(bool,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_BOOL           );
        TEST_SELECT(bool,
                    FM::BDEAT_TEXT,    Class::BDEM_BER_BOOL           );
        TEST_SELECT(bool,
                    FM::BDEAT_DEC,     Class::BDEM_BER_BOOL           );
        TEST_SELECT(char,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_INT            );
        TEST_SELECT(char,
                    FM::BDEAT_DEC,     Class::BDEM_BER_INT            );
        TEST_SELECT(char,
                    FM::BDEAT_TEXT,    Class::BDEM_BER_UTF8_STRING    );
        TEST_SELECT(schar,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_INT            );
        TEST_SELECT(schar,
                    FM::BDEAT_DEC,     Class::BDEM_BER_INT            );
        TEST_SELECT(schar,
                    FM::BDEAT_TEXT,    Class::BDEM_BER_UTF8_STRING    );
        TEST_SELECT(uchar,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_INT            );
        TEST_SELECT(uchar,
                    FM::BDEAT_DEC,     Class::BDEM_BER_INT            );
        TEST_SELECT(short,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_INT            );
        TEST_SELECT(short,
                    FM::BDEAT_DEC,     Class::BDEM_BER_INT            );
        TEST_SELECT(ushort,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_INT            );
        TEST_SELECT(ushort,
                    FM::BDEAT_DEC,     Class::BDEM_BER_INT            );
        TEST_SELECT(int,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_INT            );
        TEST_SELECT(int,
                    FM::BDEAT_DEC,     Class::BDEM_BER_INT            );
        TEST_SELECT(uint,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_INT            );
        TEST_SELECT(uint,
                    FM::BDEAT_DEC,     Class::BDEM_BER_INT            );
        TEST_SELECT(long,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_INT            );
        TEST_SELECT(long,
                    FM::BDEAT_DEC,     Class::BDEM_BER_INT            );
        TEST_SELECT(unsigned long,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_INT            );
        TEST_SELECT(unsigned long,
                    FM::BDEAT_DEC,     Class::BDEM_BER_INT            );
        TEST_SELECT(int64,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_INT            );
        TEST_SELECT(int64,
                    FM::BDEAT_DEC,     Class::BDEM_BER_INT            );
        TEST_SELECT(uint64,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_INT            );
        TEST_SELECT(uint64,
                    FM::BDEAT_DEC,     Class::BDEM_BER_INT            );
        TEST_SELECT(float,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_REAL           );
        TEST_SELECT(double,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_REAL           );
        TEST_SELECT(string,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_UTF8_STRING    );
        TEST_SELECT(string,
                    FM::BDEAT_TEXT,    Class::BDEM_BER_UTF8_STRING    );
        TEST_SELECT(string,
                    FM::BDEAT_BASE64,  Class::BDEM_BER_OCTET_STRING   );
        TEST_SELECT(string,
                    FM::BDEAT_HEX,     Class::BDEM_BER_OCTET_STRING   );
        TEST_SELECT(bdet_Date,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_VISIBLE_STRING );
        TEST_SELECT(bdet_DateTz,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_VISIBLE_STRING );
        TEST_SELECT(bdet_Datetime,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_VISIBLE_STRING );
        TEST_SELECT(bdet_DatetimeTz,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_VISIBLE_STRING );
        TEST_SELECT(bdet_Time,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_VISIBLE_STRING );
        TEST_SELECT(bdet_TimeTz,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_VISIBLE_STRING );
        TEST_SELECT(vectorChar,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_OCTET_STRING   );
        TEST_SELECT(vectorChar,
                    FM::BDEAT_BASE64,  Class::BDEM_BER_OCTET_STRING   );
        TEST_SELECT(vectorChar,
                    FM::BDEAT_HEX,     Class::BDEM_BER_OCTET_STRING   );
        TEST_SELECT(vectorChar,
                    FM::BDEAT_TEXT,    Class::BDEM_BER_UTF8_STRING    );
        TEST_SELECT(CustString,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_UTF8_STRING    );
        TEST_SELECT(CustString,
                    FM::BDEAT_TEXT,    Class::BDEM_BER_UTF8_STRING    );
        TEST_SELECT(CustString,
                    FM::BDEAT_BASE64,  Class::BDEM_BER_OCTET_STRING   );
        TEST_SELECT(CustString,
                    FM::BDEAT_HEX,     Class::BDEM_BER_OCTET_STRING   );
        TEST_SELECT(MyEnum,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_ENUMERATION    );
        TEST_SELECT(MyEnum,
                    FM::BDEAT_DEC,     Class::BDEM_BER_ENUMERATION    );
        TEST_SELECT(MyEnum,
                    FM::BDEAT_TEXT,    Class::BDEM_BER_ENUMERATION    );
        TEST_SELECT(MySequence,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_SEQUENCE       );
        TEST_SELECT(MyChoice,
                    FM::BDEAT_DEFAULT, Class::BDEM_BER_SEQUENCE       );

        if (veryVerbose) cout << "Testing dynamic types" << endl;

        static const struct {
            TC::Value    d_category;
            Class::Value d_expectedResult;
        } DATA[] = {
            { TC::BDEAT_ARRAY_CATEGORY         , Class::BDEM_BER_SEQUENCE    },
            { TC::BDEAT_CHOICE_CATEGORY        , Class::BDEM_BER_SEQUENCE    },
//            { TC::CUSTOMIZED_TYPE_CATEGORY , ? },
            { TC::BDEAT_ENUMERATION_CATEGORY   , Class::BDEM_BER_ENUMERATION },
//            { TC::NULLABLE_VALUE_CATEGORY  ,  ? },
            { TC::BDEAT_SEQUENCE_CATEGORY      , Class::BDEM_BER_SEQUENCE    },
        };

        static const int DATA_SIZE = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < DATA_SIZE; ++i) {
            const TC::Value    CATEGORY        = DATA[i].d_category;
            const Class::Value EXPECTED_RESULT = DATA[i].d_expectedResult;

            MyDynamicType object(CATEGORY);

            bdem_BerUniversalTagNumber::Value result =
                bdem_BerUniversalTagNumber::select(object, FM::BDEAT_DEFAULT);

            LOOP2_ASSERT(EXPECTED_RESULT, result, EXPECTED_RESULT == result);

        }

        if (verbose) cout << "\nEnd of test." << endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING MAX-SUPPORTED-VERSION
        // Concerns:
        //   Our concern is that the correct value is returned.
        //
        // Plan:
        //   Retrieve the version number and ensure that it is the correct
        //   value.  Note that this version number is hard-coded into the .h,
        //   and therefore this test case needs to be updated whenever that
        //   number changes.
        //
        // Testing:
        //   int maxSupportedBdexVersion();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting maxSupportedBdexVersion"
                          << "\n===========================" << endl;

        const int EXPECTED_VERSION = 1;
        int currentVersion = Class::maxSupportedBdexVersion();
        ASSERT(currentVersion == EXPECTED_VERSION);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // VALUE TEST:
        //   Verify that the 'toString' function produces strings that are
        //   identical to their respective enumerator symbols.  Verify that the
        //   output operator produces the same respective string values that
        //   would be produced by 'toString' (note that this is testing streams
        //   convertible to standard 'ostream' streams and the 'print' method).
        //
        // Testing:
        //   enum Value { ... }
        //   enum { BDEM_LENGTH = ... }
        //   const char *toString(Value value);
        //   int fromString(Value *value, const char *string, int length);
        //  ^print(bsl::ostream& stream, Enum rhs);
        //
        //   Note: '^' indicates tested indirectly
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "VALUE TEST" << endl
                                  << "==========" << endl;

        static const struct {
            Enum         d_enum;   // enumerated Value
            const char  *d_ascii;  // string Representation
        } DATA[] = {
            // Enumerated Value                 String Representation
            // --------------------------       --------------------------
            { Class::BDEM_BER_BOOL,             "BOOL"                      },
            { Class::BDEM_BER_INT,              "INT"                       },
            { Class::BDEM_BER_OCTET_STRING,     "OCTET_STRING"              },
            { Class::BDEM_BER_REAL,             "REAL"                      },
            { Class::BDEM_BER_ENUMERATION,      "ENUMERATION"               },
            { Class::BDEM_BER_UTF8_STRING,      "UTF8_STRING"               },
            { Class::BDEM_BER_SEQUENCE,         "SEQUENCE"                  },
            { Class::BDEM_BER_VISIBLE_STRING,   "VISIBLE_STRING"            },
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;

        int i; // loop index variable -- keeps MS compiler from complaining

        if (verbose) cout << "\nVerify table length is correct." << endl;

        ASSERT(DATA_LENGTH == NUM_ENUMS);

        if (verbose) cout << "\nVerify the toString function." << endl;

        for (i = 0; i < DATA_LENGTH; ++i) {
            const char *const FMT = DATA[i].d_ascii;

            if (veryVerbose) cout << "EXPECTED FORMAT: " << FMT << endl;
            const char *const ACT = Class::toString(DATA[i].d_enum);
            if (veryVerbose) cout << "  ACTUAL FORMAT: " << ACT << endl <<endl;

            LOOP_ASSERT(i, 0 == strcmp(FMT, ACT));
            for (int j = 0; j < i; ++j) {  // Make sure ALL strings are unique.
                LOOP2_ASSERT(i, j, 0 != strcmp(DATA[j].d_ascii, FMT));
            }
        }

        if (verbose) cout << "\nVerify the toString function." << endl;

        for (i = 0; i < DATA_LENGTH; ++i) {
            const Enum        EXP = DATA[i].d_enum;
            const char *const FMT = DATA[i].d_ascii;

            if (veryVerbose) cout << "EXPECTED VALUE: " << EXP << endl;
            Enum ACT;
            int status = Class::fromString(&ACT, FMT, strlen(FMT));
            if (veryVerbose) cout << "  ACTUAL VALUE: " << ACT << endl <<endl;

            LOOP_ASSERT(i,   0 == status);
            LOOP_ASSERT(i, EXP == ACT);

            // prepend to invalidate
            {
                bsl::string mS(FMT);  const bsl::string& S = mS;
                mS.insert(mS.begin(), 'x');

                Enum ACT = Class::BDEM_BER_INT;
                int status = Class::fromString(&ACT, S.c_str(), S.size());
                LOOP_ASSERT(i, 0 != status);
                LOOP_ASSERT(i, Class::BDEM_BER_INT == ACT);
            }

            // append to invalidate
            {
                bsl::string mS(FMT);  const bsl::string& S = mS;
                mS.push_back('z');

                Enum ACT = Class::BDEM_BER_INT;
                int status = Class::fromString(&ACT, S.c_str(), S.size());
                LOOP_ASSERT(i, 0 != status);
                LOOP_ASSERT(i, Class::BDEM_BER_INT == ACT);
            }
        }

        if (verbose) cout << "\nVerify the output (<<) operator." << endl;

        const int SIZE = 1000;   // Must be big enough to hold output string.
        const char XX = (char) 0xff;  // Value used to represent an unset char.
        char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
        const char *CTRL_BUF = mCtrlBuf; // Used to check for extra characters.

        for (i = 0; i < DATA_LENGTH; ++i) {
            char buf[SIZE];
            memcpy(buf, CTRL_BUF, SIZE);  // Preset buf to 'unset' char values.

            const char *const FMT = DATA[i].d_ascii;

            if (veryVerbose) cout << "EXPECTED FORMAT: " << FMT << endl;
            ostrstream out(buf, sizeof buf); out << DATA[i].d_enum << ends;
            if (veryVerbose) cout << "  ACTUAL FORMAT: " << buf << endl <<endl;

            const int SZ = strlen(FMT) + 1;
            LOOP_ASSERT(i, SZ < SIZE);  // Check buffer is large enough.
            LOOP_ASSERT(i, XX == buf[SIZE - 1]);  // Check for overrun.
            LOOP_ASSERT(i, 0 == memcmp(buf, FMT, SZ));
            LOOP_ASSERT(i, 0 == memcmp(buf + SZ, CTRL_BUF + SZ, SIZE - SZ));
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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
