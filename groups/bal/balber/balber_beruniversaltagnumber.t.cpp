// balber_beruniversaltagnumber.t.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balber_beruniversaltagnumber.h>

#include <bdlat_arrayfunctions.h>

#include <bdlsb_fixedmemoutstreambuf.h>

#include <bslalg_constructorproxy.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>

#include <bslmf_if.h>

#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_iostream.h>
#include <bsl_string.h>

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

#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

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

// test_mychoice.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYCHOICE
#define INCLUDED_TEST_MYCHOICE

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: MyChoice
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
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
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
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
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
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
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
        BSLS_ASSERT(MyChoice::SELECTION_ID_UNDEFINED ==
                     d_selectionId);
        return FAILURE;                                               // RETURN
    }
}

inline
int& MyChoice::selection1()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
bsl::string& MyChoice::selection2()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
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
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;                                               // RETURN
    }
}

inline
const int& MyChoice::selection1() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const bsl::string& MyChoice::selection2() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
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
            BSLS_ASSERT(test::MyChoice::SELECTION_ID_UNDEFINED
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

// test_myenumeration.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYENUMERATION
#define INCLUDED_TEST_MYENUMERATION

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: MyEnumeration
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
        BSLS_ASSERT(!"encountered out-of-bound enumerated value");
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

// test_mysequence.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSEQUENCE
#define INCLUDED_TEST_MYSEQUENCE

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: MySequence
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
//                        GLOBAL ENTITIES FOR TESTING
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace {
namespace u {

                         // ===========================
                         // class PartialCustomizedType
                         // ===========================

template <class BASE_TYPE>
class PartialCustomizedType {
    // This class template provides the minimum necessary functionality to test
    // 'balber::BerUniversalTagNumber::select' with an 'object' that has the
    // 'CustomizedType' 'bldat' type category.  The specified 'BASE_TYPE'
    // defines the 'BaseType' trait of the 'CustomizedType' implementation
    // provided by this class.

  public:
    // CREATORS
    PartialCustomizedType()
        // Construct a 'PartialCustomizedType' object.
    {
    }
};

// TRAITS

}  // close u namespace
}  // close unnamed namespace

namespace bdlat_CustomizedTypeFunctions {

template <class BASE_TYPE>
struct BaseType<u::PartialCustomizedType<BASE_TYPE> > {
    // This 'struct' provides a definition of the
    // 'bdlat_CustomizedTypeFucntions::BaseType' meta-function for all
    // specializations of 'PartialCustomizedType'.

    typedef BASE_TYPE Type;
};

template <class BASE_TYPE>
struct IsCustomizedType<u::PartialCustomizedType<BASE_TYPE> > {
    // This 'struct' provides a definition of the
    // 'bdlat_CustomizedTypeFunctions::IsCustomizedType' meta-function for all
    // specializations of 'PartialCustomizedType'.

    enum { VALUE = 1 };
};

} // close bdlat_CustomizedTypeFunctions

namespace {
namespace u {

                          // ========================
                          // class PartialDynamicType
                          // ========================

template <class VALUE_TYPE>
class PartialDynamicType {
    // This class template provides the minimum necessary functionality to
    // test 'balber::BerUniversalTagNumber::select' with an 'object' that
    // has the 'DynamicType' 'bdlat' type category.  The specified 'VALUE_TYPE'
    // defines the type of the underlying value of this object as well as its
    // dynamic type category, which is the type category of the 'VALUE_TYPE'.

  public:
    // TYPES
    typedef VALUE_TYPE ValueType;
        // 'ValueType' is an alias to the 'VALUE_TYPE' template argument.

  private:
    // DATA
    bslalg::ConstructorProxy<ValueType> d_value;
        // The value of the underlying type, 'ValueType', for which this object
        // is a wrapper with the 'DynamicType' category

    bdlat_TypeCategory::Value           d_category;
        // Note that this object stores its type category in a 'd_category'
        // data member. Storing this information in a data member, and in
        // particular using that data member as the value to return in the
        // 'typeCategory' method below, is *critical* to expose a bug in the
        // prior implementation of this component that did not expect
        // 'bdlat_TypeCategoryUtil::accessByCategory' to load information from
        // its 'object' argument.  This operation does, in practice, load such
        // information (as one can imagine the implementation for
        // 'bcem_Aggregate' must, and indeed, does).

  public:
    // CREATORS
    explicit PartialDynamicType(bslma::Allocator *basicAllocator = 0)
        // Construct a 'PartialDynamicType' object.  Optionally specify a
        // 'basicAllocator' used to supply memory if 'ValueType' is
        // allocator-aware, and is otherwise unused.  If 'basicAllocator' is 0
        // and 'ValueType' is allocator-aware, the currently installed default
        // allocator is used.
    : d_value(bslma::Default::allocator(basicAllocator))
    , d_category(static_cast<bdlat_TypeCategory::Value>(
          bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION))
    {
    }

    explicit PartialDynamicType(const ValueType&  value,
                                bslma::Allocator *basicAllocator = 0)
        // Construct a 'PartialDynamicType' object having the specified 'value'
        // underlying value.  Optionally specify a 'basicAllocator' used to
        // supply memory if 'ValueType' is allocator-aware, and is otherwise
        // unused.  If 'basicAllocator' is 0 and 'ValueType' is
        // allocator-aware, the currently installed default allocator is used.
    : d_value(value, bslma::Default::allocator(basicAllocator))
    , d_category(static_cast<bdlat_TypeCategory::Value>(
          bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION))
    {
    }

    PartialDynamicType(const PartialDynamicType&  original,
                       bslma::Allocator          *basicAllocator = 0)
        // Construct a 'PartialDynamicType' object having an underlying value
        // that is a copy of the underlying value of the specified 'original'
        // object.  Optionally specify a 'basicAllocator' used to supply memory
        // if 'ValueType' is allocator-aware, and is otherwise unused.  If
        // 'basicAllocator' is 0 and 'ValueType' is allocator-aware, the
        // currently installed default allocator is used.
    : d_value(original.d_value.object(),
              bslma::Default::allocator(basicAllocator))
    , d_category(original.d_category)
    {
    }

    // MANIPULATORS
    PartialDynamicType& operator=(const PartialDynamicType& original)
        // Assign to the underlying value of this object a copy of the
        // underlying value of the specified 'original' object.  Return a
        // reference providing modifiable access to this object.
    {
        d_value.object() = original.d_value.object();
        d_category       = original.d_category;

        return *this;
    }

    // ACCESSORS
    bdlat_TypeCategory::Value typeCategory() const
        // Return the *runtime* type category of this object, which is the
        // *compile-time* type category of 'ValueType'.
    {
        return d_category;
    }

    const ValueType& value() const
        // Return a non-'const' reference to the underlying value of this
        // object.
    {
        return d_value.object();
    }
};

// TRAITS
template <class VALUE_TYPE>
bdlat_TypeCategory::Value bdlat_typeCategorySelect(
                                  const PartialDynamicType<VALUE_TYPE>& object)
    // Return the *runtime* type category for the specified 'object'.
{
    return object.typeCategory();
}

}  // close u namespace
}  // close unnamed namespace

template <class VALUE_TYPE>
struct bdlat_TypeCategoryDeclareDynamic<u::PartialDynamicType<VALUE_TYPE> > {
    enum { VALUE = 1 };
};

namespace bdlat_ArrayFunctions {

// No specialization of 'ElementType' need be provided for this test.

template <class VALUE_TYPE>
struct IsArray<u::PartialDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsArray<VALUE_TYPE>::VALUE };
};

}  // close bdlat_ArrayFunctions namespace

namespace bdlat_ChoiceFunctions {

template <class VALUE_TYPE>
struct IsChoice<u::PartialDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsChoice<VALUE_TYPE>::VALUE };
};

}  // close bdlat_ChoiceFunctions namespace

namespace {
namespace u {

template <
    class VALUE_TYPE,
    int = bdlat_CustomizedTypeFunctions::IsCustomizedType<VALUE_TYPE>::VALUE>
struct PartialDynamicType_BaseTypeImpl {
    // This meta-function 'struct' provides the calculation of the
    // 'BaseType' for the 'CustomizedType' category of a 'PartialDynamicType'
    // if its specified 'VALUE_TYPE' is a 'CustomizedType'.

    typedef typename bdlat_CustomizedTypeFunctions::BaseType<VALUE_TYPE>::Type
        Type;
};

template <class VALUE_TYPE>
struct PartialDynamicType_BaseTypeImpl<VALUE_TYPE, 0> {
    // This meta-function 'struct' provides the calculation of the
    // 'BaseType' for the 'CustomizedType' category of a 'PartialDynamicType'
    // if its specified 'VALUE_TYPE' is not a 'CustomizedType'.

    typedef bslmf::Nil Type;
};

}  // close u namespace
}  // close unnamed namespace

namespace bdlat_CustomizedTypeFunctions {

template <class VALUE_TYPE>
struct BaseType<u::PartialDynamicType<VALUE_TYPE> > {
    typedef typename u::PartialDynamicType_BaseTypeImpl<VALUE_TYPE>::Type Type;
};

template <class VALUE_TYPE>
struct IsCustomizedType<u::PartialDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsCustomizedType<VALUE_TYPE>::VALUE };
};

}  // close bdlat_CustomizedTypeFunctions namespace

namespace bdlat_EnumFunctions {

template <class VALUE_TYPE>
struct IsEnumeration<u::PartialDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsEnumeration<VALUE_TYPE>::VALUE };
};

}  // close bdlat_EnumFunctions namespace

namespace {
namespace u {

template <class VALUE_TYPE,
          int =
              bdlat_NullableValueFunctions::IsNullableValue<VALUE_TYPE>::VALUE>
struct PartialDynamicType_ValueTypeImpl {
    // This meta-function 'struct' provides the calculation of the
    // 'ValueType' for the 'NullableValue' category of a 'PartialDynamicType'
    // if its specified 'VALUE_TYPE' has the 'NullableValue' category.

    typedef typename bdlat_NullableValueFunctions::ValueType<VALUE_TYPE>::Type
        Type;
};

template <class VALUE_TYPE>
struct PartialDynamicType_ValueTypeImpl<VALUE_TYPE, 0> {
    // This meta-function 'struct' provides the calculation of the 'ValueType'
    // for the 'NullableValue' category of a 'PartialDynamicType' if its
    // specified 'VALUE_TYPE' does not have the 'NullableValue' category.

    typedef bslmf::Nil Type;
};

}  // close u namespace
}  // close unnamed namespace

namespace bdlat_NullableValueFunctions {

template <class VALUE_TYPE>
struct IsNullableValue<u::PartialDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsNullableValue<VALUE_TYPE>::VALUE };
};

template <class VALUE_TYPE>
struct ValueType<u::PartialDynamicType<VALUE_TYPE> > {
    typedef
        typename u::PartialDynamicType_ValueTypeImpl<VALUE_TYPE>::Type Type;
};

}  // close bdlat_NullableValueFunctions namespace

namespace bdlat_SequenceFunctions {

template <class VALUE_TYPE>
struct IsSequence<u::PartialDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsSequence<VALUE_TYPE>::VALUE };
};

}  // close bdlat_SequenceFunctions namespace

namespace {
namespace u {

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_typeCategoryAccessArray(
                             const u::PartialDynamicType<VALUE_TYPE>& object,
                             ACCESSOR&                                accessor)
    // Invoke the specified 'accessor', passing it an appropriate
    // representation of the specified 'object' as the first argument, and
    // either a 'bdlat_TypeCategory::Array' tag object as the second argument
    // if the first argument can be used with 'bdlat_arrayfunctions', or a
    // 'bslmf::Nil' tag object otherwise.  Return the result from the
    // invocation of 'accessor'.
{
    typedef
        typename bslmf::If<bdlat_ArrayFunctions::IsArray<VALUE_TYPE>::VALUE,
                           bdlat_TypeCategory::Array,
                           bslmf::Nil>::Type Tag;

    return accessor(object.value(), Tag());
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_typeCategoryAccessChoice(
                             const u::PartialDynamicType<VALUE_TYPE>& object,
                             ACCESSOR&                                accessor)
    // Invoke the specified 'accessor', passing it an appropriate
    // representation of the specified 'object' as the first argument, and
    // either a 'bdlat_TypeCategory::Choice' tag object as the second argument
    // if the first argument can be used with 'bdlat_choicefunctions', or a
    // 'bslmf::Nil' tag object otherwise.  Return the result from the
    // invocation of 'accessor'.
{
    typedef
        typename bslmf::If<bdlat_ChoiceFunctions::IsChoice<VALUE_TYPE>::VALUE,
                           bdlat_TypeCategory::Choice,
                           bslmf::Nil>::Type Tag;

    return accessor(object.value(), Tag());
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_typeCategoryAccessCustomizedType(
                             const u::PartialDynamicType<VALUE_TYPE>& object,
                             ACCESSOR&                                accessor)
    // Invoke the specified 'accessor', passing it an appropriate
    // representation of the specified 'object' as the first argument, and
    // either a 'bdlat_TypeCategory::CustomizedType' tag object as the second
    // argument if the first argument can be used with
    // 'bdlat_customizedtypefunctions', or a 'bslmf::Nil' tag object otherwise.
    // Return the result from the invocation of 'accessor'.
{
    typedef typename bslmf::If<
        bdlat_CustomizedTypeFunctions::IsCustomizedType<VALUE_TYPE>::VALUE,
        bdlat_TypeCategory::CustomizedType,
        bslmf::Nil>::Type Tag;

    return accessor(object.value(), Tag());
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_typeCategoryAccessEnumeration(
                             const u::PartialDynamicType<VALUE_TYPE>& object,
                             ACCESSOR&                                accessor)
    // Invoke the specified 'accessor', passing it an appropriate
    // representation of the specified 'object' as the first argument, and
    // either a 'bdlat_TypeCategory::Enumeration' tag object as the second
    // argument if the first argument can be used with 'bdlat_enumfunctions',
    // or a 'bslmf::Nil' tag object otherwise.  Return the result from the
    // invocation of 'accessor'.
{
    typedef typename bslmf::If<
        bdlat_EnumFunctions::IsEnumeration<VALUE_TYPE>::VALUE,
        bdlat_TypeCategory::Enumeration,
        bslmf::Nil>::Type Tag;

    return accessor(object.value(), Tag());
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_typeCategoryAccessNullableValue(
                             const u::PartialDynamicType<VALUE_TYPE>& object,
                             ACCESSOR&                                accessor)
    // Invoke the specified 'accessor', passing it an appropriate
    // representation of the specified 'object' as the first argument, and
    // either a 'bdlat_TypeCategory::NullableValue' tag object as the second
    // argument if the first argument can be used with
    // 'bdlat_nullablevaluefunctions', or a 'bslmf::Nil' tag object otherwise.
    // Return the result from the invocation of 'accessor'.
{
    typedef typename bslmf::If<
        bdlat_NullableValueFunctions::IsNullableValue<VALUE_TYPE>::VALUE,
        bdlat_TypeCategory::NullableValue,
        bslmf::Nil>::Type Tag;

    return accessor(object.value(), Tag());
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_typeCategoryAccessSequence(
                             const u::PartialDynamicType<VALUE_TYPE>& object,
                             ACCESSOR&                                accessor)
    // Invoke the specified 'accessor', passing it an appropriate
    // representation of the specified 'object' as the first argument, and
    // either a 'bdlat_TypeCategory::Sequence' tag object as the second
    // argument if the first argument can be used with
    // 'bdlat_sequencefunctions', or a 'bslmf::Nil' tag object otherwise.
    // Return the result from the invocation of 'accessor'.
{
    typedef typename bslmf::If<
        bdlat_SequenceFunctions::IsSequence<VALUE_TYPE>::VALUE,
        bdlat_TypeCategory::Sequence,
        bslmf::Nil>::Type Tag;

    return accessor(object.value(), Tag());
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_typeCategoryAccessSimple(
                             const u::PartialDynamicType<VALUE_TYPE>& object,
                             ACCESSOR&                                accessor)
    // Invoke the specified 'accessor', passing it an appropriate
    // representation of the specified 'object' as the first argument, and
    // either a 'bdlat_TypeCategory::Simple' tag object as the second argument
    // if the first argument can be used as a simple type, or a 'bslmf::Nil'
    // tag object otherwise.  Return the result from the invocation of
    // 'accessor'.
{
    typedef typename bslmf::If<
        bdlat_TypeCategory::e_SIMPLE_CATEGORY ==
            static_cast<bdlat_TypeCategory::Value>(
                bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION),
        bdlat_TypeCategory::Simple,
        bslmf::Nil>::Type Tag;
    // If the type category of 'VALUE_TYPE' is 'Simple', then 'Tag' is
    // 'bdlat_TypeCategory::Simple', and 'bslmf::Nil' otherwise.  This
    // detection is done differently than for the complex type categories
    // because 'Simple' types do not specialize a type trait to opt-in to being
    // 'Simple'.

    return accessor(object.value(), Tag());
}

                         // =========================
                         // struct TestCase4_ImplUtil
                         // =========================

struct TestCase4_ImpUtil {
    // This utility 'struct' provides a namespace for a suite of constants and
    // functions used in the implementation of test case 4.

    // CLASS DATA
    enum {
        k_NONE_TAG_NUMBER = -2 // sentinel value used to indicate that the
                               // 'alternateTagNumber' output parameter of
                               // 'select' should be unmodified upon return
    };

    // CLASS METHODS
    template <class TYPE>
    static void verifySelect(
               int                                  LINE,
               const TYPE&                          object,
               int                                  formattingMode,
               balber::BerUniversalTagNumber::Value expectedTagNumber,
               int                                  expectedAlternateTagNumber)
        // Compare the results of invoking
        // 'balber::BerUniversalTagNumber::select' with the specified 'object'
        // and 'formattingMode' with the specified 'expectedTagNumber' and
        // 'expectedAlternateTagNumber'.  If identical, do nothing; otherwise,
        // log an error message and increment 'testStatus'.
    {
        int alternateTagNumber = k_NONE_TAG_NUMBER;

        const balber::BerUniversalTagNumber::Value tagNumber =
            balber::BerUniversalTagNumber::select(
                object, formattingMode, &alternateTagNumber);

        ASSERTV(L_, LINE, tagNumber, tagNumber == expectedTagNumber);

        ASSERTV(L_,
                LINE,
                alternateTagNumber,
                alternateTagNumber == expectedAlternateTagNumber);
    }

    template <class TYPE>
    static void verifySelect(
                       int                                   LINE,
                       const TYPE&                           object,
                       int                                   formattingMode,
                       const balber::BerEncoderOptions      *options,
                       balber::BerUniversalTagNumber::Value  expectedTagNumber)
        // Compare the results of invoking
        // 'balber::BerUniversalTagNumber::select' with the specified 'object',
        // 'formattingMode', and 'options' with the specified
        // 'expectedTagNumber' and 'expectedAlternateTagNumber'.  If identical,
        // do nothing; otherwise, log an error message and increment
        // 'testStatus'.
    {
        const balber::BerUniversalTagNumber::Value tagNumber =
        balber::BerUniversalTagNumber::select(object, formattingMode, options);

        ASSERTV(L_, LINE, tagNumber, tagNumber == expectedTagNumber);
    }

    template <class TYPE>
    static void verifySelect(const TYPE& object, int formattingMode)
        // Invoke 'balber::BerUniversalTagNumber::select' with the specified
        // 'object' and 'formattingMode'.  Note that this function is intended
        // to be used for negative testing of 'select'.
    {
        int alternateTagNumber = k_NONE_TAG_NUMBER;

        balber::BerUniversalTagNumber::select(
            object, formattingMode, &alternateTagNumber);
    }

    template <class TYPE>
    static void verifySelect(const TYPE&                      object,
                             int                              formattingMode,
                             const balber::BerEncoderOptions *options)
        // Invoke 'balber::BerUniversalTagNumber::select' with the specified
        // 'object', 'formattingMode', and 'options'.  Note that this function
        // is intended to be used for negative testing of 'select'.
    {
        balber::BerUniversalTagNumber::select(object, formattingMode, options);
    }
};

}  // close u namespace
}  // close unnamed namespace
}  // close enterprise namespace

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
      case 5: {
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
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ALL IN AND OUT OF CONTRACT 'SELECT' CALLS
        //   This case tests the entire input-output space of 'select',
        //   including the out-of-contract calls, to verify quality of
        //   implementation.
        //
        // Concerns:
        //: 1 The calculated universal tag number for each supported type is
        //:   equal to the tag number from the table for said type in the
        //:   component documentation.  There are 3 exceptions to this rule.
        //:   The 3 "real number" types: 'float', 'double', and
        //:   'bdldfp::Decimal64' provide undocumented support for the
        //:   'bdlat_FormattingMode::e_DEC' formatting mode.  The universal tag
        //:   number for this formatting mode is the same as for their default
        //:   formatting mode: 'balber::BerUniversalTagNumber::e_REAL'.
        //:
        //: 2 As a quality of implementation detail, 'select' asserts on all
        //:   arguments that violate its contract.
        //:
        //: 3 The default universal tag number for date and time types is
        //:   'e_BER_VISIBLE_STRING'.
        //:
        //: 4 The alternate tag number for date and time types is
        //:   'e_BER_OCTET_STRING'.
        //:
        //: 5 The universal tag number for date and time types when encoder
        //:   options are specified and the 'encodeDateAndTimeTypesAsBinary'
        //:   flag is 'false' is 'e_BER_VISIBLE_STRING'.
        //:
        //: 6 The universal tag number for date and time types when encoder
        //:   options are specified and the 'encodeDateAndTimeTypesAsBinary'
        //:   flag is 'true' is 'e_BER_OCTET_STRING'.
        //
        // Plan:
        //: 1 Enumerate every value of the input space considered different by
        //:   'select' and verify each property specified in "Concerns" holds.
        //
        // Testing:
        //  Value select(const TYPE& object, int fmtMode, int *altTag);
        //  Value select(const TYPE& object, int fmtMode, const Options *opts);
        // --------------------------------------------------------------------

        typedef bdlt::Date       Date;
        typedef bdlt::DateTz     DateTz;
        typedef bdlt::Datetime   Datetime;
        typedef bdlt::DatetimeTz DatetimeTz;
        typedef bdlt::Time       Time;
        typedef bdlt::TimeTz     TimeTz;
            // Convenient aliases for the date and time types.

        const bool                                 boolVal          = false;
        const char                                 charVal          = 'a';
        const signed char                          signedCharVal    = 'a';
        const unsigned char                        unsignedCharVal  = 'a';
        const short                                shortVal         = 0;
        const unsigned short                       unsignedShortVal = 0;
        const int                                  intVal           = 0;
        const unsigned int                         unsignedIntVal   = 0;
        const long                                 longVal          = 0;
        const unsigned long                        unsignedLongVal  = 0;
        const bsls::Types::Int64                   int64Val         = 0;
        const bsls::Types::Uint64                  uint64Val        = 0;
        const float                                floatVal         = 0.f;
        const double                               doubleVal        = 0.0;
        const bdldfp::Decimal64                    decimal64Val;
        const bsl::string                          stringVal;
        const Date                                 dateVal;
        const DateTz                               dateTzVal;
        const Datetime                             datetimeVal;
        const DatetimeTz                           datetimeTzVal;
        const Time                                 timeVal;
        const TimeTz                               timeTzVal;
        const bdlb::Variant2<Date, DateTz>         dateVariantVal;
        const bdlb::Variant2<Datetime, DatetimeTz> datetimeVariantVal;
        const bdlb::Variant2<Time, TimeTz>         timeVariantVal;
        const bsl::vector<char>                    charVectorVal;
            // Convenient aliases for a suite of values for all supported
            // 'bdlat' 'Simple' types, as well as 'bsl::vector<char>' (which is
            // an 'Array' type).

        enum {
            // This enumeration provides a set of convenient aliases for the
            // 'bdlat_TypeCategory::Value' enumerators.

            ARRAY_C           = bdlat_TypeCategory::e_ARRAY_CATEGORY,
            CHOICE_C          = bdlat_TypeCategory::e_CHOICE_CATEGORY,
            CUSTOMIZED_TYPE_C = bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY,
            ENUMERATION_C     = bdlat_TypeCategory::e_ENUMERATION_CATEGORY,
            NULLABLE_VALUE_C  = bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY,
            SEQUENCE_C        = bdlat_TypeCategory::e_SEQUENCE_CATEGORY,
            SIMPLE_C          = bdlat_TypeCategory::e_SIMPLE_CATEGORY
        };

        typedef bsl::vector<test::MySequence> Array;
        typedef test::MyChoice                Choice;
        typedef test::MyEnumeration::Value    Enumeration;
        typedef test::MyEnumeration           EnumerationUtil;
        typedef test::MySequence              Sequence;
        typedef int                           SimpleInt;
            // Convenient aliases for types satisfying one of the 'bdlat' type
            // categories and that are not parameterized by an underlying type.
            // These are the "level 0" complex 'bdlat' types used in this test
            // (except for 'SimpleInt', which is not a complex type, but a
            // simple type.) 'EnumerationUtil' is used below to get a value for
            // an enumerator.

        typedef u::PartialCustomizedType<Array>       CustomizedArray;
        typedef u::PartialCustomizedType<Choice>      CustomizedChoice;
        typedef u::PartialCustomizedType<Enumeration> CustomizedEnumeration;
        typedef u::PartialCustomizedType<Sequence>    CustomizedSequence;
        typedef u::PartialCustomizedType<SimpleInt>   CustomizedSimpleInt;
            // Convenient aliases for 'CustomizedType' types having 'BaseType'
            // types that provide different "level 0" 'bdlat' type categories.
            // These are the "level 1" 'CustomizedType' types.

        typedef u::PartialDynamicType<Array>       DynamicArray;
        typedef u::PartialDynamicType<Choice>      DynamicChoice;
        typedef u::PartialDynamicType<Enumeration> DynamicEnumeration;
        typedef u::PartialDynamicType<Sequence>    DynamicSequence;
        typedef u::PartialDynamicType<SimpleInt>   DynamicSimpleInt;
            // Convenient aliases for 'DynamicType' types having underlying
            // types that provide different "level 0" 'bdlat' type categories.
            // These are the "level 1" 'DynamicType' types.

        typedef bdlb::NullableValue<Array>       NullableArray;
        typedef bdlb::NullableValue<Choice>      NullableChoice;
        typedef bdlb::NullableValue<Enumeration> NullableEnumeration;
        typedef bdlb::NullableValue<Sequence>    NullableSequence;
        typedef bdlb::NullableValue<SimpleInt>   NullableSimpleInt;
            // Convenient aliases for 'NullableValue' types having 'ValueType'
            // types that provide different "level 0" 'bdlat' type categories.
            // These are the "level 1" 'NullableValue' types.

        typedef u::PartialCustomizedType<DynamicSimpleInt>
                                                   CustomizedDynamicSimpleInt;
        typedef u::PartialCustomizedType<NullableSimpleInt>
                                                   CustomizedNullableSimpleInt;
            // Convenient aliases for 'CustomizedType' types having 'BaseType'
            // types that provide different "level 1" 'bdlat' type categories.
            // These are the "level 2" 'CustomizedType' types.

        typedef u::PartialDynamicType<CustomizedSimpleInt>
                                                    DynamicCustomizedSimpleInt;
        typedef u::PartialDynamicType<NullableSimpleInt>
                                                    DynamicNullableSimpleInt;
            // Convenient aliases for 'DynamicType' types having underlying
            // types that provide different "level 1" 'bdlat' type categories.
            // these are the "level 2" 'DynamicType' types.

        typedef bdlb::NullableValue<CustomizedSimpleInt>
                                                   NullableCustomizedSimpleInt;
        typedef bdlb::NullableValue<DynamicSimpleInt>
                                                   NullableDynamicSimpleInt;
            // Convenient aliases for 'NullableValue' types having 'ValueType'
            // types that provide different "level 1" 'bdlat' type categories.
            // These are the "level 2" 'NullableValue' types.

        typedef u::PartialDynamicType<CustomizedDynamicSimpleInt>
                                             DynamicCustomizedDynamicSimpleInt;
        typedef u::PartialDynamicType<NullableDynamicSimpleInt>
                                             DynamicNullableDynamicSimpleInt;
            // Convenient aliases for 'DynamicType' types having underlying
            // types that provide different "level 2" 'bdlat' type categories.
            // These are the "level 3" 'DynamicType' types.

        const Array                             arrayVal;
        const Choice                            choiceVal;
        const CustomizedArray                   custArrayVal;
        const CustomizedChoice                  custChoiceVal;
        const CustomizedDynamicSimpleInt        custDynIntVal;
        const CustomizedEnumeration             custEnumVal;
        const CustomizedNullableSimpleInt       custNullIntVal;
        const CustomizedSequence                custSeqVal;
        const CustomizedSimpleInt               custIntVal;
        const DynamicArray                      dynArrayVal;
        const DynamicChoice                     dynChoiceVal;
        const DynamicCustomizedDynamicSimpleInt dynCustDynIntVal;
        const DynamicCustomizedSimpleInt        dynCustIntVal;
        const DynamicEnumeration                dynEnumVal;
        const DynamicNullableDynamicSimpleInt   dynNullDynIntVal;
        const DynamicNullableSimpleInt          dynNullIntVal;
        const DynamicSequence                   dynSeqVal;
        const DynamicSimpleInt                  dynIntVal;
        const Enumeration                 enumVal(EnumerationUtil::VALUE1);
        const NullableArray               nullArrayVal;
        const NullableChoice              nullChoiceVal;
        const NullableCustomizedSimpleInt nullCustIntVal;
        const NullableDynamicSimpleInt    nullDynIntVal;
        const NullableEnumeration         nullEnumVal;
        const NullableSequence            nullSeqVal;
        const NullableSimpleInt           nullIntVal;
        const Sequence                    seqVal;
            // These are Complex-typed values used in the below test table.

        const int BASE64  = bdlat_FormattingMode::e_BASE64;
        const int DEC     = bdlat_FormattingMode::e_DEC;
        const int DEFAULT = bdlat_FormattingMode::e_DEFAULT;
        const int HEX     = bdlat_FormattingMode::e_HEX;
        const int TEXT    = bdlat_FormattingMode::e_TEXT;
            // Convenient aliases for the enumerators of the valid formatting
            // modes.

        typedef u::TestCase4_ImpUtil TestUtil;
            // Convenient alias for the utility 'struct' used to provide
            // the implementation of the 'PASS' and 'FAIL' macros below.

        typedef balber::BerUniversalTagNumber TagNumberUtil;
        typedef TagNumberUtil::Value          TagNumber;
            // Convenient aliases for the class under test, and the
            // tag number enumeration.

        const int       NONE           = TestUtil::k_NONE_TAG_NUMBER;
        const TagNumber BOOL           = TagNumberUtil::e_BER_BOOL;
        const TagNumber INT            = TagNumberUtil::e_BER_INT;
        const TagNumber OCTET_STRING   = TagNumberUtil::e_BER_OCTET_STRING;
        const TagNumber REAL           = TagNumberUtil::e_BER_REAL;
        const TagNumber ENUMERATION    = TagNumberUtil::e_BER_ENUMERATION;
        const TagNumber UTF8_STRING    = TagNumberUtil::e_BER_UTF8_STRING;
        const TagNumber SEQUENCE       = TagNumberUtil::e_BER_SEQUENCE;
        const TagNumber VISIBLE_STRING = TagNumberUtil::e_BER_VISIBLE_STRING;
            // Aliases for the enumerators of
            // 'balber::BerUniversalTagNumber::Value', and 'NONE', which is a
            // negative number not equal to the numerical values of any of the
            // valid enumerators.  This value is used to verify that the
            // 'alternateTag' output argument of 'select' is unmodified when
            // the given type does not have an alternate tag.

        const bsls::AssertTestHandlerGuard G;
            // 'G' installs a throwing assertion failure handler for the
            // duration of this test case, in order to perform negative
            // testing.  This failure handler is required to be installed while
            // either 'PASS' or 'FAIL', defined below, are invoked.

#define PASS(...)                                                             \
    ASSERT_PASS(                                                              \
      ::BloombergLP::u::TestCase4_ImpUtil::verifySelect(__LINE__, __VA_ARGS__))
    // Verify that a particular set of argument and result values are
    // in-contract for 'select'.  This macro supports one of two overloads,
    // having the following specifications:
    //..
    //  template <class ANY_TYPE>
    //  void PASS(
    //        const ANY_TYPE&                      object,
    //        int                                  formattingMode,
    //        balber::BerUniversalTagNumber::Value expectedTagNumber,
    //        int                                  expectedAlternateTagNumber);
    //      // Compare the results of invoking
    //      // 'balber::BerUniversalTagNumber::select' with the specified
    //      // 'object' and 'formattingMode' with the specified
    //      // 'expectedTagNumber' and 'expectedAlternateTagNumber'.  If
    //      // identical, do nothing; otherwise log an error message and
    //      // increment 'testStatus'.  If the 'object' and 'formattingMode'
    //      // violate the contract of 'select', increment 'testStatus' and
    //      // log an error message.
    //..
    // and,
    //..
    //  template <class ANY_TYPE>
    //  void PASS(const ANY_TYPE&                       object,
    //            int                                   formattingMode,
    //            const balber::BerEncoderOptions      *encoderOptions,
    //            balber::BerUniversalTagNumber::Value  expectedTagNumber);
    //      // Compare the results of invoking
    //      // 'balber::BerUniversalTagNumber::select' with the specified
    //      // 'object', 'formattingMode', and 'options' with the specified
    //      // 'expectedTagNumber' and 'expectedAlternateTagNumber'.  If
    //      // identical, do nothing; otherwise, log an error message and
    //      // increment 'testStatus'.  If the 'object' and 'formattingMode'
    //      // violate the contract of 'select', increment 'testStatus' and
    //      // log an error message.
    //..

#define FAIL(...)                                                             \
    ASSERT_SAFE_FAIL(                                                         \
                ::BloombergLP::u::TestCase4_ImpUtil::verifySelect(__VA_ARGS__))
#define FAIL_OPT(...)                                                         \
    ASSERT_OPT_FAIL(                                                          \
                ::BloombergLP::u::TestCase4_ImpUtil::verifySelect(__VA_ARGS__))
    // Verify that a particular set of argument values are out-of-contract
    // for 'select'.  This macro supports one of two overloads, having the
    // following specifications:
    //..
    //  template <class ANY_TYPE>
    //  void FAIL(const ANY_TYPE& object, int formattingMode);
    //      // Invoke 'balber::BerUniversalTagNumber::select' with the
    //      // specified 'object' and 'formattingMode'.  Do nothing if the
    //      // invocation violates the contract of 'select'; otherwise,
    //      // increment 'testStatus' and log an error message.
    //..
    // and,
    //..
    //  template <class ANY_TYPE>
    //  void FAIL(const ANY_TYPE&                  object,
    //            int                              formattingMode,
    //            const balber::BerEncoderOptions *options);
    //      // Invoke 'balber::BerUniversalTagNumber::select' with the
    //      // specified 'object', 'formattingMode', and 'options'.  Do nothing
    //      // if the invocation violates the contract of 'select'; otherwise,
    //      // increment 'testStatus' and log an error message.
    //..

        // The following test table verifies that 'select' behaves according
        // to the specification in the component documentation.
        //
        // In this test table, rows prefixed with a comment: /* ! */ indicate
        // that the successfully returned tag number is not in the component
        // documentation.  These undocumented results contradict the
        // documentation, which states that such usage is out of contract.
        // However, this behavior is likely relied upon in practice.
        //
        //         OBJECT       FMT MODE   TAG NUMBER    ALT TAG NUMBER
        //   ------------------ -------- --------------- ---------------
        FAIL(boolVal           , BASE64                                 );
        PASS(boolVal           , DEC    , BOOL          , NONE          );
        PASS(boolVal           , DEFAULT, BOOL          , NONE          );
        FAIL(boolVal           , HEX                                    );
        PASS(boolVal           , TEXT   , BOOL          , NONE          );

        FAIL(charVal           , BASE64                                 );
        PASS(charVal           , DEC    , INT           , NONE          );
        PASS(charVal           , DEFAULT, INT           , NONE          );
        FAIL(charVal           , HEX                                    );
        PASS(charVal           , TEXT   , UTF8_STRING   , NONE          );

        FAIL(signedCharVal     , BASE64                                 );
        PASS(signedCharVal     , DEC    , INT           , NONE          );
        PASS(signedCharVal     , DEFAULT, INT           , NONE          );
        FAIL(signedCharVal     , HEX                                    );
        PASS(signedCharVal     , TEXT   , UTF8_STRING   , NONE          );

        FAIL(unsignedCharVal   , BASE64                                 );
        PASS(unsignedCharVal   , DEC    , INT           , NONE          );
        PASS(unsignedCharVal   , DEFAULT, INT           , NONE          );
        FAIL(unsignedCharVal   , HEX                                    );
        FAIL(unsignedCharVal   , TEXT                                   );

        FAIL(shortVal          , BASE64                                 );
        PASS(shortVal          , DEC    , INT           , NONE          );
        PASS(shortVal          , DEFAULT, INT           , NONE          );
        FAIL(shortVal          , HEX                                    );
        FAIL(shortVal          , TEXT                                   );

        FAIL(unsignedShortVal  , BASE64                                 );
        PASS(unsignedShortVal  , DEC    , INT           , NONE          );
        PASS(unsignedShortVal  , DEFAULT, INT           , NONE          );
        FAIL(unsignedShortVal  , HEX                                    );
        FAIL(unsignedShortVal  , TEXT                                   );

        FAIL(intVal            , BASE64                                 );
        PASS(intVal            , DEC    , INT           , NONE          );
        PASS(intVal            , DEFAULT, INT           , NONE          );
        FAIL(intVal            , HEX                                    );
        FAIL(intVal            , TEXT                                   );

        FAIL(unsignedIntVal    , BASE64                                 );
        PASS(unsignedIntVal    , DEC    , INT           , NONE          );
        PASS(unsignedIntVal    , DEFAULT, INT           , NONE          );
        FAIL(unsignedIntVal    , HEX                                    );
        FAIL(unsignedIntVal    , TEXT                                   );

        FAIL(longVal           , BASE64                                 );
        PASS(longVal           , DEC    , INT           , NONE          );
        PASS(longVal           , DEFAULT, INT           , NONE          );
        FAIL(longVal           , HEX                                    );
        FAIL(longVal           , TEXT                                   );

        FAIL(unsignedLongVal   , BASE64                                 );
        PASS(unsignedLongVal   , DEC    , INT           , NONE          );
        PASS(unsignedLongVal   , DEFAULT, INT           , NONE          );
        FAIL(unsignedLongVal   , HEX                                    );
        FAIL(unsignedLongVal   , TEXT                                   );

        FAIL(int64Val          , BASE64                                 );
        PASS(int64Val          , DEC    , INT           , NONE          );
        PASS(int64Val          , DEFAULT, INT           , NONE          );
        FAIL(int64Val          , HEX                                    );
        FAIL(int64Val          , TEXT                                   );

        FAIL(uint64Val         , BASE64                                 );
        PASS(uint64Val         , DEC    , INT           , NONE          );
        PASS(uint64Val         , DEFAULT, INT           , NONE          );
        FAIL(uint64Val         , HEX                                    );
        FAIL(uint64Val         , TEXT                                   );

        FAIL(floatVal          , BASE64                                 );
/* ! */ PASS(floatVal          , DEC    , REAL          , NONE          );
        PASS(floatVal          , DEFAULT, REAL          , NONE          );
        FAIL(floatVal          , HEX                                    );
        FAIL(floatVal          , TEXT                                   );

        FAIL(doubleVal         , BASE64                                 );
/* ! */ PASS(doubleVal         , DEC    , REAL          , NONE          );
        PASS(doubleVal         , DEFAULT, REAL          , NONE          );
        FAIL(doubleVal         , HEX                                    );
        FAIL(doubleVal         , TEXT                                   );

        FAIL(decimal64Val      , BASE64                                 );
/* ! */ PASS(decimal64Val      , DEC    , OCTET_STRING  , NONE          );
        PASS(decimal64Val      , DEFAULT, OCTET_STRING  , NONE          );
        FAIL(decimal64Val      , HEX                                    );
        FAIL(decimal64Val      , TEXT                                   );

        PASS(stringVal         , BASE64 , OCTET_STRING  , NONE          );
        FAIL(stringVal         , DEC                                    );
        PASS(stringVal         , DEFAULT, UTF8_STRING   , NONE          );
        PASS(stringVal         , HEX    , OCTET_STRING  , NONE          );
        PASS(stringVal         , TEXT   , UTF8_STRING   , NONE          );

        FAIL(dateVal           , BASE64                                 );
        FAIL(dateVal           , DEC                                    );
        PASS(dateVal           , DEFAULT, VISIBLE_STRING, OCTET_STRING  );
        FAIL(dateVal           , HEX                                    );
        FAIL(dateVal           , TEXT                                   );

        FAIL(dateTzVal         , BASE64                                 );
        FAIL(dateTzVal         , DEC                                    );
        PASS(dateTzVal         , DEFAULT, VISIBLE_STRING, OCTET_STRING  );
        FAIL(dateTzVal         , HEX                                    );
        FAIL(dateTzVal         , TEXT                                   );

        FAIL(dateVariantVal    , BASE64                                 );
        FAIL(dateVariantVal    , DEC                                    );
        PASS(dateVariantVal    , DEFAULT, VISIBLE_STRING, OCTET_STRING  );
        FAIL(dateVariantVal    , HEX                                    );
        FAIL(dateVariantVal    , TEXT                                   );

        FAIL(datetimeVal       , BASE64                                 );
        FAIL(datetimeVal       , DEC                                    );
        PASS(datetimeVal       , DEFAULT, VISIBLE_STRING, OCTET_STRING  );
        FAIL(datetimeVal       , HEX                                    );
        FAIL(datetimeVal       , TEXT                                   );

        FAIL(datetimeTzVal     , BASE64                                 );
        FAIL(datetimeTzVal     , DEC                                    );
        PASS(datetimeTzVal     , DEFAULT, VISIBLE_STRING, OCTET_STRING  );
        FAIL(datetimeTzVal     , HEX                                    );
        FAIL(datetimeTzVal     , TEXT                                   );

        FAIL(datetimeVariantVal, BASE64                                 );
        FAIL(datetimeVariantVal, DEC                                    );
        PASS(datetimeVariantVal, DEFAULT, VISIBLE_STRING, OCTET_STRING  );
        FAIL(datetimeVariantVal, HEX                                    );
        FAIL(datetimeVariantVal, TEXT                                   );

        FAIL(timeVal           , BASE64                                 );
        FAIL(timeVal           , DEC                                    );
        PASS(timeVal           , DEFAULT, VISIBLE_STRING, OCTET_STRING  );
        FAIL(timeVal           , HEX                                    );
        FAIL(timeVal           , TEXT                                   );

        FAIL(timeTzVal         , BASE64                                 );
        FAIL(timeTzVal         , DEC                                    );
        PASS(timeTzVal         , DEFAULT, VISIBLE_STRING, OCTET_STRING  );
        FAIL(timeTzVal         , HEX                                    );
        FAIL(timeTzVal         , TEXT                                   );

        FAIL(timeVariantVal    , BASE64                                 );
        FAIL(timeVariantVal    , DEC                                    );
        PASS(timeVariantVal    , DEFAULT, VISIBLE_STRING, OCTET_STRING  );
        FAIL(timeVariantVal    , HEX                                    );
        FAIL(timeVariantVal    , TEXT                                   );

        PASS(charVectorVal     , BASE64 , OCTET_STRING  , NONE          );
        FAIL(charVectorVal     , DEC                                    );
        PASS(charVectorVal     , DEFAULT, OCTET_STRING  , NONE          );
        PASS(charVectorVal     , HEX    , OCTET_STRING  , NONE          );
        PASS(charVectorVal     , TEXT   , UTF8_STRING   , NONE          );

        FAIL(arrayVal          , BASE64                                 );
        FAIL(arrayVal          , DEC                                    );
        PASS(arrayVal          , DEFAULT, SEQUENCE      , NONE          );
        FAIL(arrayVal          , HEX                                    );
        FAIL(arrayVal          , TEXT                                   );

        FAIL(choiceVal         , BASE64                                 );
        FAIL(choiceVal         , DEC                                    );
        PASS(choiceVal         , DEFAULT, SEQUENCE      , NONE          );
        FAIL(choiceVal         , HEX                                    );
        FAIL(choiceVal         , TEXT                                   );

        FAIL(custArrayVal      , BASE64                                 );
        FAIL(custArrayVal      , DEC                                    );
        PASS(custArrayVal      , DEFAULT, SEQUENCE      , NONE          );
        FAIL(custArrayVal      , HEX                                    );
        FAIL(custArrayVal      , TEXT                                   );

        FAIL(custChoiceVal     , BASE64                                 );
        FAIL(custChoiceVal     , DEC                                    );
        PASS(custChoiceVal     , DEFAULT, SEQUENCE      , NONE          );
        FAIL(custChoiceVal     , HEX                                    );
        FAIL(custChoiceVal     , TEXT                                   );

        FAIL_OPT(custDynIntVal , BASE64                                 );
        FAIL_OPT(custDynIntVal , DEC                                    );
        FAIL_OPT(custDynIntVal , DEFAULT                                );
        FAIL_OPT(custDynIntVal , HEX                                    );
        FAIL_OPT(custDynIntVal , TEXT                                   );

        FAIL(custEnumVal       , BASE64                                 );
        PASS(custEnumVal       , DEC    , ENUMERATION   , NONE          );
        PASS(custEnumVal       , DEFAULT, ENUMERATION   , NONE          );
        FAIL(custEnumVal       , HEX                                    );
        PASS(custEnumVal       , TEXT   , ENUMERATION   , NONE          );

        FAIL(custNullIntVal    , BASE64                                 );
        PASS(custNullIntVal    , DEC    , INT           , NONE          );
        PASS(custNullIntVal    , DEFAULT, INT           , NONE          );
        FAIL(custNullIntVal    , HEX                                    );
        FAIL(custNullIntVal    , TEXT                                   );

        FAIL(custSeqVal        , BASE64                                 );
        FAIL(custSeqVal        , DEC                                    );
        PASS(custSeqVal        , DEFAULT, SEQUENCE      , NONE          );
        FAIL(custSeqVal        , HEX                                    );
        FAIL(custSeqVal        , TEXT                                   );

        FAIL(custIntVal        , BASE64                                 );
        PASS(custIntVal        , DEC    , INT           , NONE          );
        PASS(custIntVal        , DEFAULT, INT           , NONE          );
        FAIL(custIntVal        , HEX                                    );
        FAIL(custIntVal        , TEXT                                   );

        FAIL(dynArrayVal       , BASE64                                 );
        FAIL(dynArrayVal       , DEC                                    );
        PASS(dynArrayVal       , DEFAULT, SEQUENCE      , NONE          );
        FAIL(dynArrayVal       , HEX                                    );
        FAIL(dynArrayVal       , TEXT                                   );

        FAIL(dynChoiceVal      , BASE64                                 );
        FAIL(dynChoiceVal      , DEC                                    );
        PASS(dynChoiceVal      , DEFAULT, SEQUENCE      , NONE          );
        FAIL(dynChoiceVal      , HEX                                    );
        FAIL(dynChoiceVal      , TEXT                                   );

        FAIL_OPT(dynCustDynIntVal, BASE64                               );
        FAIL_OPT(dynCustDynIntVal, DEC                                  );
        FAIL_OPT(dynCustDynIntVal, DEFAULT                              );
        FAIL_OPT(dynCustDynIntVal, HEX                                  );
        FAIL_OPT(dynCustDynIntVal, TEXT                                 );

        FAIL(dynCustIntVal     , BASE64                                 );
        PASS(dynCustIntVal     , DEC    , INT           , NONE          );
        PASS(dynCustIntVal     , DEFAULT, INT           , NONE          );
        FAIL(dynCustIntVal     , HEX                                    );
        FAIL(dynCustIntVal     , TEXT                                   );

        FAIL(dynEnumVal        , BASE64                                 );
        PASS(dynEnumVal        , DEC    , ENUMERATION   , NONE          );
        PASS(dynEnumVal        , DEFAULT, ENUMERATION   , NONE          );
        FAIL(dynEnumVal        , HEX                                    );
        PASS(dynEnumVal        , TEXT   , ENUMERATION   , NONE          );

        FAIL_OPT(dynNullDynIntVal, BASE64                               );
        FAIL_OPT(dynNullDynIntVal, DEC                                  );
        FAIL_OPT(dynNullDynIntVal, DEFAULT                              );
        FAIL_OPT(dynNullDynIntVal, HEX                                  );
        FAIL_OPT(dynNullDynIntVal, TEXT                                 );

        FAIL(dynNullIntVal     , BASE64                                 );
        PASS(dynNullIntVal     , DEC    , INT           , NONE          );
        PASS(dynNullIntVal     , DEFAULT, INT           , NONE          );
        FAIL(dynNullIntVal     , HEX                                    );
        FAIL(dynNullIntVal     , TEXT                                   );

        FAIL(dynSeqVal         , BASE64                                 );
        FAIL(dynSeqVal         , DEC                                    );
        PASS(dynSeqVal         , DEFAULT, SEQUENCE      , NONE          );
        FAIL(dynSeqVal         , HEX                                    );
        FAIL(dynSeqVal         , TEXT                                   );

        FAIL(dynIntVal         , BASE64                                 );
        PASS(dynIntVal         , DEC    , INT           , NONE          );
        PASS(dynIntVal         , DEFAULT, INT           , NONE          );
        FAIL(dynIntVal         , HEX                                    );
        FAIL(dynIntVal         , TEXT                                   );

        FAIL(enumVal           , BASE64                                 );
        PASS(enumVal           , DEC    , ENUMERATION   , NONE          );
        PASS(enumVal           , DEFAULT, ENUMERATION   , NONE          );
        FAIL(enumVal           , HEX                                    );
        PASS(enumVal           , TEXT   , ENUMERATION   , NONE          );

        FAIL(nullArrayVal      , BASE64                                 );
        FAIL(nullArrayVal      , DEC                                    );
        PASS(nullArrayVal      , DEFAULT, SEQUENCE      , NONE          );
        FAIL(nullArrayVal      , HEX                                    );
        FAIL(nullArrayVal      , TEXT                                   );

        FAIL(nullChoiceVal     , BASE64                                 );
        FAIL(nullChoiceVal     , DEC                                    );
        PASS(nullChoiceVal     , DEFAULT, SEQUENCE      , NONE          );
        FAIL(nullChoiceVal     , HEX                                    );
        FAIL(nullChoiceVal     , TEXT                                   );

        FAIL(nullCustIntVal    , BASE64                                 );
        PASS(nullCustIntVal    , DEC    , INT           , NONE          );
        PASS(nullCustIntVal    , DEFAULT, INT           , NONE          );
        FAIL(nullCustIntVal    , HEX                                    );
        FAIL(nullCustIntVal    , TEXT                                   );

        FAIL_OPT(nullDynIntVal , BASE64                                 );
        FAIL_OPT(nullDynIntVal , DEC                                    );
        FAIL_OPT(nullDynIntVal , DEFAULT                                );
        FAIL_OPT(nullDynIntVal , HEX                                    );
        FAIL_OPT(nullDynIntVal , TEXT                                   );

        FAIL(nullEnumVal       , BASE64                                 );
        PASS(nullEnumVal       , DEC    , ENUMERATION   , NONE          );
        PASS(nullEnumVal       , DEFAULT, ENUMERATION   , NONE          );
        FAIL(nullEnumVal       , HEX                                    );
        PASS(nullEnumVal       , TEXT   , ENUMERATION   , NONE          );

        FAIL(nullSeqVal        , BASE64                                 );
        FAIL(nullSeqVal        , DEC                                    );
        PASS(nullSeqVal        , DEFAULT, SEQUENCE      , NONE          );
        FAIL(nullSeqVal        , HEX                                    );
        FAIL(nullSeqVal        , TEXT                                   );

        FAIL(nullIntVal        , BASE64                                 );
        PASS(nullIntVal        , DEC    , INT           , NONE          );
        PASS(nullIntVal        , DEFAULT, INT           , NONE          );
        FAIL(nullIntVal        , HEX                                    );
        FAIL(nullIntVal        , TEXT                                   );

        FAIL(seqVal            , BASE64                                 );
        FAIL(seqVal            , DEC                                    );
        PASS(seqVal            , DEFAULT, SEQUENCE      , NONE          );
        FAIL(seqVal            , HEX                                    );
        FAIL(seqVal            , TEXT                                   );

        // The following table tests the overload of
        // 'balber::BerUniversalTagNumber::select' taking a
        // 'balber::BerEncoderOptions'.  Note that this overload only differs
        // from the primary overload for the date and time types.

        balber::BerEncoderOptions txtTime;
            // An encoder options value that specifies date and time types
            // should be encoded using their textual representation

        balber::BerEncoderOptions binTime;
            // An encoder options value that specifies date and time types
            // should be encoded using their binary representation
        binTime.setEncodeDateAndTimeTypesAsBinary(true);

        const balber::BerEncoderOptions *TXT_TIME = &txtTime;
        const balber::BerEncoderOptions *BIN_TIME = &binTime;
            // Convenient aliases for the above options values

        //         OBJECT       FMT MODE  OPTIONS       TAG NUMBER
        //   ------------------ -------- --------- ---------------------
        FAIL(dateVal           , BASE64 , TXT_TIME                      );
        FAIL(dateVal           , BASE64 , BIN_TIME                      );
        FAIL(dateVal           , DEC    , TXT_TIME                      );
        FAIL(dateVal           , DEC    , BIN_TIME                      );
        PASS(dateVal           , DEFAULT, TXT_TIME, VISIBLE_STRING      );
        PASS(dateVal           , DEFAULT, BIN_TIME, OCTET_STRING        );
        FAIL(dateVal           , HEX    , TXT_TIME                      );
        FAIL(dateVal           , HEX    , BIN_TIME                      );
        FAIL(dateVal           , TEXT   , TXT_TIME                      );
        FAIL(dateVal           , TEXT   , BIN_TIME                      );

        FAIL(dateTzVal         , BASE64 , TXT_TIME                      );
        FAIL(dateTzVal         , BASE64 , BIN_TIME                      );
        FAIL(dateTzVal         , DEC    , TXT_TIME                      );
        FAIL(dateTzVal         , DEC    , BIN_TIME                      );
        PASS(dateTzVal         , DEFAULT, TXT_TIME, VISIBLE_STRING      );
        PASS(dateTzVal         , DEFAULT, BIN_TIME, OCTET_STRING        );
        FAIL(dateTzVal         , HEX    , TXT_TIME                      );
        FAIL(dateTzVal         , HEX    , BIN_TIME                      );
        FAIL(dateTzVal         , TEXT   , TXT_TIME                      );
        FAIL(dateTzVal         , TEXT   , BIN_TIME                      );

        FAIL(dateVariantVal    , BASE64 , TXT_TIME                      );
        FAIL(dateVariantVal    , BASE64 , BIN_TIME                      );
        FAIL(dateVariantVal    , DEC    , TXT_TIME                      );
        FAIL(dateVariantVal    , DEC    , BIN_TIME                      );
        PASS(dateVariantVal    , DEFAULT, TXT_TIME, VISIBLE_STRING      );
        PASS(dateVariantVal    , DEFAULT, BIN_TIME, OCTET_STRING        );
        FAIL(dateVariantVal    , HEX    , TXT_TIME                      );
        FAIL(dateVariantVal    , HEX    , BIN_TIME                      );
        FAIL(dateVariantVal    , TEXT   , TXT_TIME                      );
        FAIL(dateVariantVal    , TEXT   , BIN_TIME                      );

        FAIL(datetimeVal       , BASE64 , TXT_TIME                      );
        FAIL(datetimeVal       , BASE64 , BIN_TIME                      );
        FAIL(datetimeVal       , DEC    , TXT_TIME                      );
        FAIL(datetimeVal       , DEC    , BIN_TIME                      );
        PASS(datetimeVal       , DEFAULT, TXT_TIME, VISIBLE_STRING      );
        PASS(datetimeVal       , DEFAULT, BIN_TIME, OCTET_STRING        );
        FAIL(datetimeVal       , HEX    , TXT_TIME                      );
        FAIL(datetimeVal       , HEX    , BIN_TIME                      );
        FAIL(datetimeVal       , TEXT   , TXT_TIME                      );
        FAIL(datetimeVal       , TEXT   , BIN_TIME                      );

        FAIL(datetimeTzVal     , BASE64 , TXT_TIME                      );
        FAIL(datetimeTzVal     , BASE64 , BIN_TIME                      );
        FAIL(datetimeTzVal     , DEC    , TXT_TIME                      );
        FAIL(datetimeTzVal     , DEC    , BIN_TIME                      );
        PASS(datetimeTzVal     , DEFAULT, TXT_TIME, VISIBLE_STRING      );
        PASS(datetimeTzVal     , DEFAULT, BIN_TIME, OCTET_STRING        );
        FAIL(datetimeTzVal     , HEX    , TXT_TIME                      );
        FAIL(datetimeTzVal     , HEX    , BIN_TIME                      );
        FAIL(datetimeTzVal     , TEXT   , TXT_TIME                      );
        FAIL(datetimeTzVal     , TEXT   , BIN_TIME                      );

        FAIL(datetimeVariantVal, BASE64 , TXT_TIME                      );
        FAIL(datetimeVariantVal, BASE64 , BIN_TIME                      );
        FAIL(datetimeVariantVal, DEC    , TXT_TIME                      );
        FAIL(datetimeVariantVal, DEC    , BIN_TIME                      );
        PASS(datetimeVariantVal, DEFAULT, TXT_TIME, VISIBLE_STRING      );
        PASS(datetimeVariantVal, DEFAULT, BIN_TIME, OCTET_STRING        );
        FAIL(datetimeVariantVal, HEX    , TXT_TIME                      );
        FAIL(datetimeVariantVal, HEX    , BIN_TIME                      );
        FAIL(datetimeVariantVal, TEXT   , TXT_TIME                      );
        FAIL(datetimeVariantVal, TEXT   , BIN_TIME                      );

        FAIL(timeVal           , BASE64 , TXT_TIME                      );
        FAIL(timeVal           , BASE64 , BIN_TIME                      );
        FAIL(timeVal           , DEC    , TXT_TIME                      );
        FAIL(timeVal           , DEC    , BIN_TIME                      );
        PASS(timeVal           , DEFAULT, TXT_TIME, VISIBLE_STRING      );
        PASS(timeVal           , DEFAULT, BIN_TIME, OCTET_STRING        );
        FAIL(timeVal           , HEX    , TXT_TIME                      );
        FAIL(timeVal           , HEX    , BIN_TIME                      );
        FAIL(timeVal           , TEXT   , TXT_TIME                      );
        FAIL(timeVal           , TEXT   , BIN_TIME                      );

        FAIL(timeTzVal         , BASE64 , TXT_TIME                      );
        FAIL(timeTzVal         , BASE64 , BIN_TIME                      );
        FAIL(timeTzVal         , DEC    , TXT_TIME                      );
        FAIL(timeTzVal         , DEC    , BIN_TIME                      );
        PASS(timeTzVal         , DEFAULT, TXT_TIME, VISIBLE_STRING      );
        PASS(timeTzVal         , DEFAULT, BIN_TIME, OCTET_STRING        );
        FAIL(timeTzVal         , HEX    , TXT_TIME                      );
        FAIL(timeTzVal         , HEX    , BIN_TIME                      );
        FAIL(timeTzVal         , TEXT   , TXT_TIME                      );
        FAIL(timeTzVal         , TEXT   , BIN_TIME                      );

        FAIL(timeVariantVal    , BASE64 , TXT_TIME                      );
        FAIL(timeVariantVal    , BASE64 , BIN_TIME                      );
        FAIL(timeVariantVal    , DEC    , TXT_TIME                      );
        FAIL(timeVariantVal    , DEC    , BIN_TIME                      );
        PASS(timeVariantVal    , DEFAULT, TXT_TIME, VISIBLE_STRING      );
        PASS(timeVariantVal    , DEFAULT, BIN_TIME, OCTET_STRING        );
        FAIL(timeVariantVal    , HEX    , TXT_TIME                      );
        FAIL(timeVariantVal    , HEX    , BIN_TIME                      );
        FAIL(timeVariantVal    , TEXT   , TXT_TIME                      );
        FAIL(timeVariantVal    , TEXT   , BIN_TIME                      );

#undef FAIL
#undef PASS

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
        TEST_SELECT_WITH_ALT_TAG(bdldfp::Decimal64,
                                 FM::e_DEFAULT,
                                 Class::e_BER_OCTET_STRING,
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
        TEST_SELECT_WITH_OPTIONS(bdldfp::Decimal64,
                                 FM::e_DEFAULT,
                                 Class::e_BER_OCTET_STRING,
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
            bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
            bsl::ostream out(&obuf);
            out << DATA[i].d_enum << ends;
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
