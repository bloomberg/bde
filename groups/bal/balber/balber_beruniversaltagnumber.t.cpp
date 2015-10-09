// balber_beruniversaltagnumber.t.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balber_beruniversaltagnumber.h>

#include <bdlat_arrayfunctions.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>

#include <bsls_types.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

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

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// test_customizedstring.h   -*-C++-*-
#ifndef INCLUDED_TEST_CUSTOMIZEDSTRING
#define INCLUDED_TEST_CUSTOMIZEDSTRING

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: CustomizedString
//
//@DESCRIPTION:
//  todo: provide annotation

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#include <bdlat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
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
    explicit CustomizedString(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator
        // is used.

    CustomizedString(const CustomizedString& original,
                     bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit CustomizedString(const bsl::string& value,
                              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the specified
        // 'value'.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~CustomizedString();
        // Destroy this object.

    // MANIPULATORS
    CustomizedString& operator=(const CustomizedString& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    int fromString(const bsl::string& value);
        // Convert from the specified 'value' to this type.  Return 0 if
        // successful and non-zero otherwise.

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

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS

inline
CustomizedString::CustomizedString(bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
CustomizedString::CustomizedString(const CustomizedString& original,
                                   bslma::Allocator *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
CustomizedString::CustomizedString(const bsl::string& value,
                                   bslma::Allocator *basicAllocator)
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

inline
void CustomizedString::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

inline
int CustomizedString::fromString(const bsl::string& value)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (5 < value.size()) {
        return FAILURE;                                               // RETURN
    }

    d_value = value;

    return SUCCESS;
}

// ACCESSORS

inline
bsl::ostream& CustomizedString::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const bsl::string& CustomizedString::toString() const
{
    return d_value;
}

}  // close namespace test

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(test::CustomizedString)

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

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------

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

}  // close namespace test
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------

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

#ifndef INCLUDED_BDLAT_SELECTIONINFO
#include <bdlat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#include <bdlat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {
namespace test {

class MyChoice {

  private:
    union {
        bsls::ObjectBuffer< int > d_selection1;
            // todo: provide annotation
        bsls::ObjectBuffer< bsl::string > d_selection2;
            // todo: provide annotation
    };

    int                  d_selectionId;

    bslma::Allocator    *d_allocator_p;

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

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];
        // selection information for each selection

  public:
    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit MyChoice(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MyChoice' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    MyChoice(const MyChoice& original, bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MyChoice' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~MyChoice();
        // Destroy this object.

    // MANIPULATORS
    MyChoice& operator=(const MyChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

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
        bdlat_ValueTypeFunctions::reset(&d_selection1.object());
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
        bdlat_ValueTypeFunctions::reset(&d_selection2.object());
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

// ----------------------------------------------------------------------------

// CREATORS
inline
MyChoice::MyChoice(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
MyChoice::MyChoice(
    const MyChoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
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
        return NOT_FOUND;                                             // RETURN
    }
    return SUCCESS;
}

inline
int MyChoice::makeSelection(const char *name, int nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return NOT_FOUND;                                              // RETURN
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
        return FAILURE;                                               // RETURN
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
        return FAILURE;                                               // RETURN
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

}  // close namespace test

// TRAITS
BDLAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(test::MyChoice)

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
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
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

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------

// test_mychoice.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_chartype.h>
#include <bdlat_formattingmode.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MyChoice::CLASS_NAME[] = "MyChoice";
    // the name of this class

const bdlat_SelectionInfo MyChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION1,
        "Selection1",                         // name
        sizeof("Selection1") - 1,             // name length
        "todo: provide annotation",    // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        SELECTION_ID_SELECTION2,
        "Selection2",                         // name
        sizeof("Selection2") - 1,             // name length
        "todo: provide annotation",    // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdlat_SelectionInfo *MyChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdlb::CharType::toUpper(name[0])=='S'
             && bdlb::CharType::toUpper(name[1])=='E'
             && bdlb::CharType::toUpper(name[2])=='L'
             && bdlb::CharType::toUpper(name[3])=='E'
             && bdlb::CharType::toUpper(name[4])=='C'
             && bdlb::CharType::toUpper(name[5])=='T'
             && bdlb::CharType::toUpper(name[6])=='I'
             && bdlb::CharType::toUpper(name[7])=='O'
             && bdlb::CharType::toUpper(name[8])=='N') {
                switch(bdlb::CharType::toUpper(name[9])) {
                    case '1': {
                        return
                             &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return
                             &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *MyChoice::lookupSelectionInfo(int id)
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
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);

        switch (d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace test
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------

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

#ifndef INCLUDED_BDLAT_ENUMERATORINFO
#include <bdlat_enumeratorinfo.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
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

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];
        // enumerator information for each enumerator

    // CLASS METHODS
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

    static bsl::ostream& print(bsl::ostream& stream, Value value);
        // Write to the specified 'stream' the string representation of
        // the specified enumeration 'value'.  Return a reference to
        // the modifiable 'stream'.
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, MyEnumeration::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int MyEnumeration::fromInt(MyEnumeration::Value *result, int number)
{
    enum { SUCCESS = 0, NOT_FOUND = 1 };

    switch (number) {
      case MyEnumeration::VALUE1:
      case MyEnumeration::VALUE2:
        *result = (MyEnumeration::Value)number;
        return SUCCESS;                                               // RETURN
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

inline
bsl::ostream& MyEnumeration::print(bsl::ostream&      stream,
                                 MyEnumeration::Value value)
{
    return stream << toString(value);
}

// ----------------------------------------------------------------------------

// CLASS METHODS
inline
const char *MyEnumeration::toString(MyEnumeration::Value value)
{
    switch (value) {
      case VALUE1: {
        return "VALUE1";                                              // RETURN
      } break;
      case VALUE2: {
        return "VALUE2";                                              // RETURN
      } break;
      default:
        BSLS_ASSERT_SAFE(!"encountered out-of-bound enumerated value");
    }

    return 0;
}

}  // close namespace test

// TRAITS
BDLAT_DECL_ENUMERATION_TRAITS(test::MyEnumeration)

// FREE OPERATORS
inline
bsl::ostream& test::operator<<(bsl::ostream& stream,
                               test::MyEnumeration::Value rhs)
{
    return test::MyEnumeration::print(stream, rhs);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------

// test_myenumeration.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bsls_assert.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_chartype.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MyEnumeration::CLASS_NAME[] = "MyEnumeration";
    // the name of this class

const bdlat_EnumeratorInfo MyEnumeration::ENUMERATOR_INFO_ARRAY[] = {
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
            if (bdlb::CharType::toUpper(string[0])=='V'
             && bdlb::CharType::toUpper(string[1])=='A'
             && bdlb::CharType::toUpper(string[2])=='L'
             && bdlb::CharType::toUpper(string[3])=='U'
             && bdlb::CharType::toUpper(string[4])=='E') {
                switch(bdlb::CharType::toUpper(string[5])) {
                    case '1': {
                        *result = MyEnumeration::VALUE1;
                        return SUCCESS;                               // RETURN
                                                                      // RETURN
                    } break;
                    case '2': {
                        *result = MyEnumeration::VALUE2;
                        return SUCCESS;                               // RETURN
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

}  // close namespace test
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------

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

#ifndef INCLUDED_BDLAT_ATTRIBUTEINFO
#include <bdlat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#include <bdlat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
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

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySequence(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MySequence' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    MySequence(const MySequence& original,
               bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MySequence' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~MySequence();
        // Destroy this object.

    // MANIPULATORS
    MySequence& operator=(const MySequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
inline
MySequence::MySequence(bslma::Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(bslma::Default::allocator(basicAllocator))
{
}

inline
MySequence::MySequence(
    const MySequence& original,
    bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2,
               bslma::Default::allocator(basicAllocator))
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

inline
void MySequence::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
}

template <class MANIPULATOR>
inline
int MySequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
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
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int MySequence::manipulateAttribute(MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
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
template <class ACCESSOR>
inline
int MySequence::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
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
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int MySequence::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };

     const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
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

}  // close namespace test

// TRAITS
BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequence)

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

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------

// test_mysequence.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_chartype.h>
#include <bdlat_formattingmode.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MySequence::CLASS_NAME[] = "MySequence";
    // the name of this class

const bdlat_AttributeInfo MySequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                     // name
        sizeof("Attribute1") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                     // name
        sizeof("Attribute2") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdlat_AttributeInfo *MySequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdlb::CharType::toUpper(name[0])=='A'
             && bdlb::CharType::toUpper(name[1])=='T'
             && bdlb::CharType::toUpper(name[2])=='T'
             && bdlb::CharType::toUpper(name[3])=='R'
             && bdlb::CharType::toUpper(name[4])=='I'
             && bdlb::CharType::toUpper(name[5])=='B'
             && bdlb::CharType::toUpper(name[6])=='U'
             && bdlb::CharType::toUpper(name[7])=='T'
             && bdlb::CharType::toUpper(name[8])=='E') {
                switch(bdlb::CharType::toUpper(name[9])) {
                    case '1': {
                        return
                             &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return
                             &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *MySequence::lookupAttributeInfo(int id)
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
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                             -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace test
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------

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

bdlat_TypeCategory::Value bdlat_typeCategorySelect(const MyDynamicType& obj)
{
    return bdlat_TypeCategory::Value(obj.category());
}

}  // close namespace test

namespace bdlat_SequenceFunctions {

    template <>
    struct IsSequence<test::MyDynamicType> : bslmf::MetaInt<1> { };
}  // close namespace bdlat_SequenceFunctions

namespace bdlat_ChoiceFunctions {

    template <>
    struct IsChoice<test::MyDynamicType> : bslmf::MetaInt<1> { };

}  // close namespace bdlat_ChoiceFunctions

namespace bdlat_ArrayFunctions {

    template <>
    struct IsArray<test::MyDynamicType> : bslmf::MetaInt<1> { };

    template <>
    struct ElementType<test::MyDynamicType> { typedef int Type; };

}  // close namespace bdlat_ArrayFunctions

namespace bdlat_EnumFunctions {

    template <>
    struct IsEnumeration<test::MyDynamicType> : bslmf::MetaInt<1> { };

}  // close namespace bdlat_EnumFunctions

namespace bdlat_NullableValueFunctions {

    template <>
    struct IsNullableValue<test::MyDynamicType> : bslmf::MetaInt<1> { };

    template <>
    struct ValueType<test::MyDynamicType> { typedef int Type; };

}  // close namespace bdlat_NullableValueFunctions

}  // close enterprise namespace

typedef balber::BerUniversalTagNumber        Class;
typedef balber::BerUniversalTagNumber::Value Enum;
const int                                 NUM_ENUMS = Class::k_LENGTH;
typedef bdlat_FormattingMode              FM;
typedef bdlat_TypeCategory                TC;

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

#define TEST_SELECT_WITH_OPTIONS(type, formattingMode, expected, options) {   \
        type object;                                                          \
        const balber::BerUniversalTagNumber::Value expectedResult = expected; \
        balber::BerUniversalTagNumber::Value result =                         \
                                balber::BerUniversalTagNumber::select(        \
                                                               object,        \
                                                               formattingMode,\
                                                               options);      \
        LOOP2_ASSERT(expectedResult, result, expectedResult == result);       \
        balber::BerUniversalTagNumber::Value result2 =                        \
            balber::BerUniversalTagNumber::select(object,                     \
                                         formattingMode | FM::e_UNTAGGED,     \
                                               options);                      \
        LOOP2_ASSERT(expectedResult, result2, expectedResult == result2);     \
    }
    // Test select() function, both with unadorned 'formattingMode' and with
    // an extra bit set in 'formattingMode'.

#define TEST_SELECT_WITH_ALT_TAG(type, formattingMode, expected, otherTag) {  \
        type object;                                                          \
        int  altTag = -1;                                                     \
        const balber::BerUniversalTagNumber::Value expectedResult = expected; \
        balber::BerUniversalTagNumber::Value result =                         \
        balber::BerUniversalTagNumber::select(object,                         \
                                              formattingMode,                 \
                                              &altTag);                       \
        LOOP2_ASSERT(expectedResult, result, expectedResult == result);       \
        balber::BerUniversalTagNumber::Value result2 =                        \
            balber::BerUniversalTagNumber::select(object,                     \
                                         formattingMode | FM::e_UNTAGGED, \
                                         &altTag);                            \
        LOOP2_ASSERT(expectedResult, result2, expectedResult == result2);     \
        LOOP2_ASSERT(*otherTag, altTag, *otherTag == altTag);                 \
    }
    // Test 'select' function, both with unadorned 'formattingMode' and with an
    // extra bit set in 'formattingMode'.

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int         test = argc > 1 ? atoi(argv[1]) : 0;
    bool     verbose = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Usage 1"
                               << "\n===============" << bsl::endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Exercise1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'balber::BerUniversalTagNumber' operation.
//
// First, create a variable 'tagNumber' of type
// 'balber::BerUniversalTagNumber::Value' and initialize it to the value
// 'balber::BerUniversalTagNumber::e_BER_INT':
//..
    balber::BerUniversalTagNumber::Value tagNumber
                                    = balber::BerUniversalTagNumber::e_BER_INT;
//..
// Next, store its representation in a variable 'rep' of type 'const char *':
//..
    const char *rep = balber::BerUniversalTagNumber::toString(tagNumber);
    ASSERT(0 == strcmp(rep, "INT"));
//..
// Finally, print the value of 'tagNumber' to 'bsl::cout':
//..
    bsl::cout << tagNumber << bsl::endl;
//..
// This statement produces the following output on 'bsl::cout':
//..
//  INT
//..
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
        typedef bsls::Types::Int64 int64;
        typedef bsls::Types::Uint64 uint64;
        typedef bsl::vector<char> vectorChar;
        typedef test::CustomizedString CustString;
        typedef test::MyEnumeration::Value MyEnum;
        typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz>       DateVariant;
        typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz>       TimeVariant;
        typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                                                               DatetimeVariant;

        if (veryVerbose) cout << "Testing non-dynamic types with options"
                              << endl;

        int otherTag = -1;

        //          type         formatting mode   expected result
        //          ----         ---------------   ---------------
        TEST_SELECT_WITH_ALT_TAG(bool,
                                 FM::e_DEFAULT,
                                 Class::e_BER_BOOL,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(bool,
                                 FM::e_TEXT,
                                 Class::e_BER_BOOL,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(bool,
                                 FM::e_DEC,
                                 Class::e_BER_BOOL,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(char,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(char,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(char,
                                 FM::e_TEXT,
                                 Class::e_BER_UTF8_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(schar,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(schar,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(schar,
                                 FM::e_TEXT,
                                 Class::e_BER_UTF8_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(uchar,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(uchar,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(short,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(short,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(ushort,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(ushort,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(int,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(int,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(uint,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(uint,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(long,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(long,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(unsigned long,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(unsigned long,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(int64,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(int64,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(uint64,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(uint64,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(float,
                                 FM::e_DEFAULT,
                                 Class::e_BER_REAL,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(double,
                                 FM::e_DEFAULT,
                                 Class::e_BER_REAL,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(string,
                                 FM::e_DEFAULT,
                                 Class::e_BER_UTF8_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(string,
                                 FM::e_TEXT,
                                 Class::e_BER_UTF8_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(string,
                                 FM::e_BASE64,
                                 Class::e_BER_OCTET_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(string,
                                 FM::e_HEX,
                                 Class::e_BER_OCTET_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(vectorChar,
                                 FM::e_DEFAULT,
                                 Class::e_BER_OCTET_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(vectorChar,
                                 FM::e_BASE64,
                                 Class::e_BER_OCTET_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(vectorChar,
                                 FM::e_HEX,
                                 Class::e_BER_OCTET_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(vectorChar,
                                 FM::e_TEXT,
                                 Class::e_BER_UTF8_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(CustString,
                                 FM::e_DEFAULT,
                                 Class::e_BER_UTF8_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(CustString,
                                 FM::e_TEXT,
                                 Class::e_BER_UTF8_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(CustString,
                                 FM::e_BASE64,
                                 Class::e_BER_OCTET_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(CustString,
                                 FM::e_HEX,
                                 Class::e_BER_OCTET_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(MyEnum,
                                 FM::e_DEFAULT,
                                 Class::e_BER_ENUMERATION,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(MyEnum,
                                 FM::e_DEC,
                                 Class::e_BER_ENUMERATION,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(MyEnum,
                                 FM::e_TEXT,
                                 Class::e_BER_ENUMERATION,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(MySequence,
                                 FM::e_DEFAULT,
                                 Class::e_BER_SEQUENCE,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(MyChoice,
                                 FM::e_DEFAULT,
                                 Class::e_BER_SEQUENCE,
                                 &otherTag);

        otherTag = Class::e_BER_OCTET_STRING;
        TEST_SELECT_WITH_ALT_TAG(bdlt::Date,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(bdlt::DateTz,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(bdlt::Datetime,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(bdlt::DatetimeTz,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(bdlt::Time,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(bdlt::TimeTz,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(DateVariant,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(TimeVariant,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &otherTag);
        TEST_SELECT_WITH_ALT_TAG(DatetimeVariant,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &otherTag);

        if (veryVerbose) cout << "Testing dynamic types" << endl;

        static const struct {
            TC::Value    d_category;
            Class::Value d_expectedResult;
        } DATA[] = {
            { TC::e_ARRAY_CATEGORY         , Class::e_BER_SEQUENCE    },
            { TC::e_CHOICE_CATEGORY        , Class::e_BER_SEQUENCE    },
//            { TC::CUSTOMIZED_TYPE_CATEGORY , ? },
            { TC::e_ENUMERATION_CATEGORY   , Class::e_BER_ENUMERATION },
//            { TC::NULLABLE_VALUE_CATEGORY  ,  ? },
            { TC::e_SEQUENCE_CATEGORY      , Class::e_BER_SEQUENCE    },
        };

        static const int DATA_SIZE = sizeof DATA / sizeof DATA[0];
        otherTag = -1;

        for (int i = 0; i < DATA_SIZE; ++i) {
            const TC::Value    CATEGORY        = DATA[i].d_category;
            const Class::Value EXPECTED_RESULT = DATA[i].d_expectedResult;

            MyDynamicType object(CATEGORY);

            balber::BerUniversalTagNumber::Value result =
                balber::BerUniversalTagNumber::select(object,
                                                   FM::e_DEFAULT,
                                                   &otherTag);

            LOOP2_ASSERT(EXPECTED_RESULT, result, EXPECTED_RESULT == result);

        }

        if (verbose) cout << "\nEnd of test." << endl;
      } break;
      case 2: {
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
        typedef bsls::Types::Int64 int64;
        typedef bsls::Types::Uint64 uint64;
        typedef bsl::vector<char> vectorChar;
        typedef test::CustomizedString CustString;
        typedef test::MyEnumeration::Value MyEnum;
        typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz>       DateVariant;
        typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz>       TimeVariant;
        typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                                                               DatetimeVariant;

        if (veryVerbose) cout << "Testing non-dynamic types with options"
                              << endl;

        balber::BerEncoderOptions options;

        //          type         formatting mode   expected result
        //          ----         ---------------   ---------------
        TEST_SELECT_WITH_OPTIONS(bool,
                                 FM::e_DEFAULT,
                                 Class::e_BER_BOOL,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(bool,
                                 FM::e_TEXT,
                                 Class::e_BER_BOOL,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(bool,
                                 FM::e_DEC,
                                 Class::e_BER_BOOL,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(char,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(char,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(char,
                                 FM::e_TEXT,
                                 Class::e_BER_UTF8_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(schar,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(schar,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(schar,
                                 FM::e_TEXT,
                                 Class::e_BER_UTF8_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(uchar,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(uchar,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(short,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(short,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(ushort,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(ushort,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(int,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(int,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(uint,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(uint,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(long,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(long,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(unsigned long,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(unsigned long,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(int64,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(int64,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(uint64,
                                 FM::e_DEFAULT,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(uint64,
                                 FM::e_DEC,
                                 Class::e_BER_INT,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(float,
                                 FM::e_DEFAULT,
                                 Class::e_BER_REAL,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(double,
                                 FM::e_DEFAULT,
                                 Class::e_BER_REAL,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(string,
                                 FM::e_DEFAULT,
                                 Class::e_BER_UTF8_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(string,
                                 FM::e_TEXT,
                                 Class::e_BER_UTF8_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(string,
                                 FM::e_BASE64,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(string,
                                 FM::e_HEX,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(bdlt::Date,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(bdlt::DateTz,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(bdlt::Datetime,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(bdlt::DatetimeTz,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(bdlt::Time,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(bdlt::TimeTz,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(DateVariant,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(TimeVariant,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(DatetimeVariant,
                                 FM::e_DEFAULT,
                                 Class::e_BER_VISIBLE_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(vectorChar,
                                 FM::e_DEFAULT,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(vectorChar,
                                 FM::e_BASE64,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(vectorChar,
                                 FM::e_HEX,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(vectorChar,
                                 FM::e_TEXT,
                                 Class::e_BER_UTF8_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(CustString,
                                 FM::e_DEFAULT,
                                 Class::e_BER_UTF8_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(CustString,
                                 FM::e_TEXT,
                                 Class::e_BER_UTF8_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(CustString,
                                 FM::e_BASE64,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(CustString,
                                 FM::e_HEX,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(MyEnum,
                                 FM::e_DEFAULT,
                                 Class::e_BER_ENUMERATION,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(MyEnum,
                                 FM::e_DEC,
                                 Class::e_BER_ENUMERATION,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(MyEnum,
                                 FM::e_TEXT,
                                 Class::e_BER_ENUMERATION,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(MySequence,
                                 FM::e_DEFAULT,
                                 Class::e_BER_SEQUENCE,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(MyChoice,
                                 FM::e_DEFAULT,
                                 Class::e_BER_SEQUENCE,
                                 &options);

        options.setEncodeDateAndTimeTypesAsBinary(true);

        TEST_SELECT_WITH_OPTIONS(bdlt::Date,
                                 FM::e_DEFAULT,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(bdlt::DateTz,
                                 FM::e_DEFAULT,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(bdlt::Datetime,
                                 FM::e_DEFAULT,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(bdlt::DatetimeTz,
                                 FM::e_DEFAULT,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(bdlt::Time,
                                 FM::e_DEFAULT,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(bdlt::TimeTz,
                                 FM::e_DEFAULT,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(DateVariant,
                                 FM::e_DEFAULT,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(TimeVariant,
                                 FM::e_DEFAULT,
                                 Class::e_BER_OCTET_STRING,
                                 &options);
        TEST_SELECT_WITH_OPTIONS(DatetimeVariant,
                                 FM::e_DEFAULT,
                                 Class::e_BER_OCTET_STRING,
                                 &options);

        if (veryVerbose) cout << "Testing dynamic types" << endl;

        static const struct {
            TC::Value    d_category;
            Class::Value d_expectedResult;
        } DATA[] = {
            { TC::e_ARRAY_CATEGORY         , Class::e_BER_SEQUENCE    },
            { TC::e_CHOICE_CATEGORY        , Class::e_BER_SEQUENCE    },
//            { TC::CUSTOMIZED_TYPE_CATEGORY , ? },
            { TC::e_ENUMERATION_CATEGORY   , Class::e_BER_ENUMERATION },
//            { TC::NULLABLE_VALUE_CATEGORY  ,  ? },
            { TC::e_SEQUENCE_CATEGORY      , Class::e_BER_SEQUENCE    },
        };

        static const int DATA_SIZE = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < DATA_SIZE; ++i) {
            const TC::Value    CATEGORY        = DATA[i].d_category;
            const Class::Value EXPECTED_RESULT = DATA[i].d_expectedResult;

            MyDynamicType object(CATEGORY);

            balber::BerUniversalTagNumber::Value result =
                balber::BerUniversalTagNumber::select(object,
                                                   FM::e_DEFAULT,
                                                   &options);

            LOOP2_ASSERT(EXPECTED_RESULT, result, EXPECTED_RESULT == result);

        }

        if (verbose) cout << "\nEnd of test." << endl;
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
            { Class::e_BER_BOOL,             "BOOL"                      },
            { Class::e_BER_INT,              "INT"                       },
            { Class::e_BER_OCTET_STRING,     "OCTET_STRING"              },
            { Class::e_BER_REAL,             "REAL"                      },
            { Class::e_BER_ENUMERATION,      "ENUMERATION"               },
            { Class::e_BER_UTF8_STRING,      "UTF8_STRING"               },
            { Class::e_BER_SEQUENCE,         "SEQUENCE"                  },
            { Class::e_BER_VISIBLE_STRING,   "VISIBLE_STRING"            },
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

                Enum ACT = Class::e_BER_INT;
                int status = Class::fromString(&ACT, S.c_str(), S.size());
                LOOP_ASSERT(i, 0 != status);
                LOOP_ASSERT(i, Class::e_BER_INT == ACT);
            }

            // append to invalidate
            {
                bsl::string mS(FMT);  const bsl::string& S = mS;
                mS.push_back('z');

                Enum ACT = Class::e_BER_INT;
                int status = Class::fromString(&ACT, S.c_str(), S.size());
                LOOP_ASSERT(i, 0 != status);
                LOOP_ASSERT(i, Class::e_BER_INT == ACT);
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

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
