// bdlat_symbolicconverter.t.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlat_symbolicconverter.h>

#include <bslim_testutil.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_enumeratorinfo.h>
#include <bdlat_formattingmode.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>

#include <bdlb_nullablevalue.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bsl_cctype.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iosfwd.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bslalg_typetraits.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_objectbuffer.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//                                  TBD doc
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
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

struct ConvertibleType {
    int d_value;
};

// test_choicea.h   -*-C++-*-
#ifndef INCLUDED_TEST_CHOICEA
#define INCLUDED_TEST_CHOICEA

//@PURPOSE: TBD: provide purpose.
//
//@CLASSES:
//  ChoiceA: A choice class
//
//@DESCRIPTION:
//  TBD: provide annotation for 'ChoiceA'

namespace BloombergLP {

namespace test {

class ChoiceA {

  private:
    union {
        bsls::ObjectBuffer< int > d_selection1;
            // TBD: provide annotation
        bsls::ObjectBuffer< bsl::string > d_selection2;
            // TBD: provide annotation
    };

    int                 d_selectionId;

    bslma::Allocator   *d_allocator_p;

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
        // the name of this class (i.e., "ChoiceA")

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];
        // selection information for each selection

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

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
    explicit ChoiceA(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'ChoiceA' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ChoiceA(const ChoiceA& original, bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'ChoiceA' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~ChoiceA();
        // Destroy this object.

    // MANIPULATORS
    ChoiceA& operator=(const ChoiceA& rhs);
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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

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
        // Set the value of this object to be a "Selection1" value.  Optionally
        // specify the 'value' of the "Selection1".  If 'value' is not
        // specified, the default "Selection1" value is used.

    void makeSelection2();
    void makeSelection2(const bsl::string& value);
        // Set the value of this object to be a "Selection2" value.  Optionally
        // specify the 'value' of the "Selection2".  If 'value' is not
        // specified, the default "Selection2" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    int& selection1();
        // Return a reference to the modifiable "Selection1" selection of this
        // object if "Selection1" is the current selection.  The behavior is
        // undefined unless "Selection1" is the selection of this object.

    bsl::string& selection2();
        // Return a reference to the modifiable "Selection2" selection of this
        // object if "Selection2" is the current selection.  The behavior is
        // undefined unless "Selection2" is the selection of this object.

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
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

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
        // Return a reference to the non-modifiable "Selection1" selection of
        // this object if "Selection1" is the current selection.  The behavior
        // is undefined unless "Selection1" is the selection of this object.

    const bsl::string& selection2() const;
        // Return a reference to the non-modifiable "Selection2" selection of
        // this object if "Selection2" is the current selection.  The behavior
        // is undefined unless "Selection2" is the selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const ChoiceA& lhs, const ChoiceA& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'ChoiceA' objects have the same value
    // if either the selections in both objects have the same ids and the same
    // values, or both selections are undefined.

inline
bool operator!=(const ChoiceA& lhs, const ChoiceA& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const ChoiceA& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int ChoiceA::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

inline
void ChoiceA::reset()
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
void ChoiceA::makeSelection1()
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
void ChoiceA::makeSelection1(int value)
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
void ChoiceA::makeSelection2()
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
void ChoiceA::makeSelection2(const bsl::string& value)
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
ChoiceA::ChoiceA(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
ChoiceA::ChoiceA(const ChoiceA& original, bslma::Allocator *basicAllocator)
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
ChoiceA::~ChoiceA()
{
    reset();
}

// MANIPULATORS
inline
ChoiceA&
ChoiceA::operator=(const ChoiceA& rhs)
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
STREAM& ChoiceA::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;                                        // RETURN
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
int ChoiceA::makeSelection(int selectionId)
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
int ChoiceA::makeSelection(const char *name, int nameLength)
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
int ChoiceA::manipulateSelection(MANIPULATOR& manipulator)
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case ChoiceA::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
                                                                      // RETURN
      case ChoiceA::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
                                                                      // RETURN
      default:
        BSLS_ASSERT_SAFE(ChoiceA::SELECTION_ID_UNDEFINED ==
                     d_selectionId);
        return FAILURE;                                               // RETURN
    }
}

inline
int& ChoiceA::selection1()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
bsl::string& ChoiceA::selection2()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

// ACCESSORS
template <class STREAM>
inline
STREAM& ChoiceA::bdexStreamOut(STREAM& stream, int version) const
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
int ChoiceA::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
inline
int ChoiceA::accessSelection(ACCESSOR& accessor) const
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
const int& ChoiceA::selection1() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const bsl::string& ChoiceA::selection2() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

}  // close namespace test

// TRAITS
BDLAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(test::ChoiceA)

// FREE OPERATORS
inline
bool test::operator==(const test::ChoiceA& lhs, const test::ChoiceA& rhs)
{
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case test::ChoiceA::SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
                                                                    // RETURN
          case test::ChoiceA::SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
                                                                    // RETURN
          default:
            BSLS_ASSERT_SAFE(test::ChoiceA::SELECTION_ID_UNDEFINED
                            == rhs.selectionId());
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
   }
}

inline
bool test::operator!=(const test::ChoiceA& lhs, const test::ChoiceA& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& test::operator<<(bsl::ostream& stream, const test::ChoiceA& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_choicea.cpp  -*-C++-*-

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char ChoiceA::CLASS_NAME[] = "ChoiceA";
    // the name of this class

const bdlat_SelectionInfo ChoiceA::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION1,
        "Selection1",              // name
        sizeof("Selection1") - 1,  // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        SELECTION_ID_SELECTION2,
        "Selection2",              // name
        sizeof("Selection2") - 1,  // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdlat_SelectionInfo *ChoiceA::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bsl::toupper(name[0])=='S'
             && bsl::toupper(name[1])=='E'
             && bsl::toupper(name[2])=='L'
             && bsl::toupper(name[3])=='E'
             && bsl::toupper(name[4])=='C'
             && bsl::toupper(name[5])=='T'
             && bsl::toupper(name[6])=='I'
             && bsl::toupper(name[7])=='O'
             && bsl::toupper(name[8])=='N') {
                switch(bsl::toupper(name[9])) {
                    case '1': {
                      return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
                                                                      // RETURN
                    } break;
                    case '2': {
                      return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *ChoiceA::lookupSelectionInfo(int id)
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

bsl::ostream& ChoiceA::print(
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
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_choiceb.h   -*-C++-*-
#ifndef INCLUDED_TEST_CHOICEB
#define INCLUDED_TEST_CHOICEB

//@PURPOSE: TBD: provide purpose.
//
//@CLASSES:
//  ChoiceB: A choice class
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TBD: provide annotation for 'ChoiceB'


namespace BloombergLP {

namespace test {

class ChoiceB {

  private:
    union {
        bsls::ObjectBuffer< bsl::string > d_selection2;
            // TBD: provide annotation
        bsls::ObjectBuffer< int >         d_selection1;
            // TBD: provide annotation
    };

    int                 d_selectionId;

    bslma::Allocator   *d_allocator_p;

  public:
    // TYPES
    enum {
        NUM_SELECTIONS = 2 // the number of selections in this class
    };

    enum {
        SELECTION_INDEX_SELECTION2 = 0,
            // index for "Selection2" selection
        SELECTION_INDEX_SELECTION1 = 1
            // index for "Selection1" selection
    };

    enum {
        SELECTION_ID_UNDEFINED = -1,

        SELECTION_ID_SELECTION2 = 0,
            // id for "Selection2" selection
        SELECTION_ID_SELECTION1 = 1
            // id for "Selection1" selection
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "ChoiceB")

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];
        // selection information for each selection

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

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
    explicit ChoiceB(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'ChoiceB' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ChoiceB(const ChoiceB& original, bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'ChoiceB' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~ChoiceB();
        // Destroy this object.

    // MANIPULATORS
    ChoiceB& operator=(const ChoiceB& rhs);
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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    void makeSelection2();
    void makeSelection2(const bsl::string& value);
        // Set the value of this object to be a "Selection2" value.  Optionally
        // specify the 'value' of the "Selection2".  If 'value' is not
        // specified, the default "Selection2" value is used.

    void makeSelection1();
    void makeSelection1(int value);
        // Set the value of this object to be a "Selection1" value.  Optionally
        // specify the 'value' of the "Selection1".  If 'value' is not
        // specified, the default "Selection1" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    bsl::string& selection2();
        // Return a reference to the modifiable "Selection2" selection of this
        // object if "Selection2" is the current selection.  The behavior is
        // undefined unless "Selection2" is the selection of this object.

    int& selection1();
        // Return a reference to the modifiable "Selection1" selection of this
        // object if "Selection1" is the current selection.  The behavior is
        // undefined unless "Selection1" is the selection of this object.

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
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const bsl::string& selection2() const;
        // Return a reference to the non-modifiable "Selection2" selection of
        // this object if "Selection2" is the current selection.  The behavior
        // is undefined unless "Selection2" is the selection of this object.

    const int& selection1() const;
        // Return a reference to the non-modifiable "Selection1" selection of
        // this object if "Selection1" is the current selection.  The behavior
        // is undefined unless "Selection1" is the selection of this object.

};

// FREE OPERATORS
inline
bool operator==(const ChoiceB& lhs, const ChoiceB& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'ChoiceB' objects have the same value
    // if either the selections in both objects have the same ids and the same
    // values, or both selections are undefined.

inline
bool operator!=(const ChoiceB& lhs, const ChoiceB& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const ChoiceB& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int ChoiceB::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

inline
void ChoiceB::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION2: {
        typedef bsl::string Type;
        d_selection2.object().~Type();
      } break;
      case SELECTION_ID_SELECTION1: {
        // no destruction required
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

inline
void ChoiceB::makeSelection2()
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
void ChoiceB::makeSelection2(const bsl::string& value)
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

inline
void ChoiceB::makeSelection1()
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
void ChoiceB::makeSelection1(int value)
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

// ----------------------------------------------------------------------------

// CREATORS
inline
ChoiceB::ChoiceB(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
ChoiceB::ChoiceB(const ChoiceB& original, bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            bsl::string(original.d_selection2.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            int(original.d_selection1.object());
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

inline
ChoiceB::~ChoiceB()
{
    reset();
}

// MANIPULATORS
inline
ChoiceB&
ChoiceB::operator=(const ChoiceB& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
          } break;
          case SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
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
STREAM& ChoiceB::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;                                        // RETURN
            }
            switch (selectionId) {
              case SELECTION_ID_SELECTION2: {
                makeSelection2();
                streamIn(
                    stream, d_selection2.object(), 1);
              } break;
              case SELECTION_ID_SELECTION1: {
                makeSelection1();
                streamIn(
                    stream, d_selection1.object(), 1);
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
int ChoiceB::makeSelection(int selectionId)
{
    enum { NOT_FOUND = -1, SUCCESS = 0 };

    switch (selectionId) {
      case SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case SELECTION_ID_SELECTION1: {
        makeSelection1();
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
int ChoiceB::makeSelection(const char *name, int nameLength)
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
int ChoiceB::manipulateSelection(MANIPULATOR& manipulator)
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case ChoiceB::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
                                                                      // RETURN
      case ChoiceB::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
                                                                      // RETURN
      default:
        BSLS_ASSERT_SAFE(ChoiceB::SELECTION_ID_UNDEFINED ==
                     d_selectionId);
        return FAILURE;                                               // RETURN
    }
}

inline
bsl::string& ChoiceB::selection2()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
int& ChoiceB::selection1()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

// ACCESSORS
template <class STREAM>
inline
STREAM& ChoiceB::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_SELECTION2: {
                streamOut(
                    stream, d_selection2.object(), 1);
              } break;
              case SELECTION_ID_SELECTION1: {
                streamOut(
                    stream, d_selection1.object(), 1);
              } break;
              default:
                BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}

inline
int ChoiceB::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
inline
int ChoiceB::accessSelection(ACCESSOR& accessor) const
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
                                                                      // RETURN
      case SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
                                                                      // RETURN
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;                                               // RETURN
    }
}

inline
const bsl::string& ChoiceB::selection2() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const int& ChoiceB::selection1() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

}  // close namespace test

// TRAITS
BDLAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(test::ChoiceB)

// FREE OPERATORS
inline
bool test::operator==(const test::ChoiceB& lhs, const test::ChoiceB& rhs)
{
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case test::ChoiceB::SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();              // RETURN
          case test::ChoiceB::SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();              // RETURN
          default:
            BSLS_ASSERT_SAFE(test::ChoiceB::SELECTION_ID_UNDEFINED
                            == rhs.selectionId());
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
   }
}

inline
bool test::operator!=(const test::ChoiceB& lhs, const test::ChoiceB& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& test::operator<<(bsl::ostream& stream, const test::ChoiceB& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_choiceb.cpp  -*-C++-*-

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char ChoiceB::CLASS_NAME[] = "ChoiceB";
    // the name of this class

const bdlat_SelectionInfo ChoiceB::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION2,
        "Selection2",              // name
        sizeof("Selection2") - 1,  // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        SELECTION_ID_SELECTION1,
        "Selection1",              // name
        sizeof("Selection1") - 1,  // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdlat_SelectionInfo *ChoiceB::lookupSelectionInfo(const char *name,
                                                        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bsl::toupper(name[0])=='S'
             && bsl::toupper(name[1])=='E'
             && bsl::toupper(name[2])=='L'
             && bsl::toupper(name[3])=='E'
             && bsl::toupper(name[4])=='C'
             && bsl::toupper(name[5])=='T'
             && bsl::toupper(name[6])=='I'
             && bsl::toupper(name[7])=='O'
             && bsl::toupper(name[8])=='N') {
                switch(bsl::toupper(name[9])) {
                    case '1': {
                      return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
                                                                      // RETURN
                    } break;
                    case '2': {
                      return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *ChoiceB::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
      case SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
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

bsl::ostream& ChoiceB::print(bsl::ostream& stream,
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
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
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
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
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
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_customizeda.h   -*-C++-*-
#ifndef INCLUDED_TEST_CUSTOMIZEDA
#define INCLUDED_TEST_CUSTOMIZEDA

//@PURPOSE: TBD: provide purpose.
//
//@CLASSES:
//  CustomizedA: a customized class
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TBD: provide annotation for 'CustomizedA'

namespace BloombergLP {

namespace test {

class CustomizedA {

  private:
    // PRIVATE DATA MEMBERS
    bsl::string d_value;  // stored value

    // FRIENDS
    friend bool operator==(const CustomizedA& lhs, const CustomizedA& rhs);
    friend bool operator!=(const CustomizedA& lhs, const CustomizedA& rhs);

  public:
    // TYPES
    typedef bsl::string BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "CustomizedA")

    // CREATORS
    explicit CustomizedA(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'CustomizedA' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    CustomizedA(const CustomizedA&  original,
                bslma::Allocator   *basicAllocator = 0);
        // Create an object of type 'CustomizedA' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    explicit CustomizedA(const bsl::string&  value,
                         bslma::Allocator   *basicAllocator = 0);
        // Create an object of type 'CustomizedA' having the specified 'value'.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~CustomizedA();
        // Destroy this object.

    // MANIPULATORS
    CustomizedA& operator=(const CustomizedA& rhs);
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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int fromString(const bsl::string& value);
        // Convert from the specified 'value' to this type.  Return 0 if
        // successful and non-zero otherwise.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

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
bool operator==(const CustomizedA& lhs, const CustomizedA& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const CustomizedA& lhs, const CustomizedA& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const CustomizedA& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// CREATORS

inline
CustomizedA::CustomizedA(bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
CustomizedA::CustomizedA(const CustomizedA&  original,
                         bslma::Allocator   *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
CustomizedA::CustomizedA(const bsl::string&  value,
                         bslma::Allocator   *basicAllocator)
: d_value(value, basicAllocator)
{
}

inline
CustomizedA::~CustomizedA()
{
}

// MANIPULATORS

inline
CustomizedA& CustomizedA::operator=(const CustomizedA& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

template <class STREAM>
STREAM& CustomizedA::bdexStreamIn(STREAM& stream, int version)
{
    bsl::string temp;

    streamIn(stream, temp, version);

    if (!stream) {
        return stream;                                                // RETURN
    }

    if (fromString(temp) != 0) {
        stream.invalidate();
    }

    return stream;
}

inline
void CustomizedA::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

inline
int CustomizedA::fromString(const bsl::string& value)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (5 < value.size()) {
        return FAILURE;                                               // RETURN
    }

    d_value = value;

    return SUCCESS;
}

// ACCESSORS

template <class STREAM>
STREAM& CustomizedA::bdexStreamOut(STREAM& stream, int version) const
{
    return streamOut(stream, d_value, version);
}

inline
int CustomizedA::maxSupportedBdexVersion() const
{
    return bslx::VersionFunctions::maxSupportedBdexVersion(&d_value, 0);
}

inline
bsl::ostream& CustomizedA::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const bsl::string& CustomizedA::toString() const
{
    return d_value;
}

}  // close namespace test

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(test::CustomizedA)

// FREE OPERATORS

inline
bool test::operator==(const test::CustomizedA& lhs,
                      const test::CustomizedA& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool test::operator!=(const test::CustomizedA& lhs,
                      const test::CustomizedA& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& test::operator<<(bsl::ostream&            stream,
                               const test::CustomizedA& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_customizeda.cpp  -*-C++-*-

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char CustomizedA::CLASS_NAME[] = "CustomizedA";
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
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_customizedb.h   -*-C++-*-
#ifndef INCLUDED_TEST_CUSTOMIZEDB
#define INCLUDED_TEST_CUSTOMIZEDB

//@PURPOSE: TBD: provide purpose.
//
//@CLASSES:
//  CustomizedB: a customized class
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TBD: provide annotation for 'CustomizedB'

namespace BloombergLP {

namespace test {

class CustomizedB {

  private:
    // PRIVATE DATA MEMBERS
    bsl::string d_value;  // stored value

    // FRIENDS
    friend bool operator==(const CustomizedB& lhs, const CustomizedB& rhs);
    friend bool operator!=(const CustomizedB& lhs, const CustomizedB& rhs);

  public:
    // TYPES
    typedef bsl::string BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "CustomizedB")

    // CREATORS
    explicit CustomizedB(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'CustomizedB' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    CustomizedB(const CustomizedB&  original,
                bslma::Allocator   *basicAllocator = 0);
        // Create an object of type 'CustomizedB' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    explicit CustomizedB(const bsl::string&  value,
                         bslma::Allocator   *basicAllocator = 0);
        // Create an object of type 'CustomizedB' having the specified 'value'.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~CustomizedB();
        // Destroy this object.

    // MANIPULATORS
    CustomizedB& operator=(const CustomizedB& rhs);
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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int fromString(const bsl::string& value);
        // Convert from the specified 'value' to this type.  Return 0 if
        // successful and non-zero otherwise.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

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
bool operator==(const CustomizedB& lhs, const CustomizedB& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const CustomizedB& lhs, const CustomizedB& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const CustomizedB& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS

inline
CustomizedB::CustomizedB(bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
CustomizedB::CustomizedB(const CustomizedB&  original,
                         bslma::Allocator   *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
CustomizedB::CustomizedB(const bsl::string&  value,
                         bslma::Allocator   *basicAllocator)
: d_value(value, basicAllocator)
{
}

inline
CustomizedB::~CustomizedB()
{
}

// MANIPULATORS

inline
CustomizedB& CustomizedB::operator=(const CustomizedB& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

template <class STREAM>
STREAM& CustomizedB::bdexStreamIn(STREAM& stream, int version)
{
    bsl::string temp;

    streamIn(stream, temp, version);

    if (!stream) {
        return stream;                                                // RETURN
    }

    if (fromString(temp)!=0) {
        stream.invalidate();
    }

    return stream;
}

inline
void CustomizedB::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

inline
int CustomizedB::fromString(const bsl::string& value)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (10 < value.size()) {
        return FAILURE;                                               // RETURN
    }

    d_value = value;

    return SUCCESS;
}

// ACCESSORS

template <class STREAM>
STREAM& CustomizedB::bdexStreamOut(STREAM& stream, int version) const
{
    return streamOut(stream, d_value, version);
}

inline
int CustomizedB::maxSupportedBdexVersion() const
{
    return bslx::VersionFunctions::maxSupportedBdexVersion(&d_value, 0);
}

inline
bsl::ostream& CustomizedB::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const bsl::string& CustomizedB::toString() const
{
    return d_value;
}

}  // close namespace test

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(test::CustomizedB)

// FREE OPERATORS

inline
bool test::operator==(const test::CustomizedB& lhs,
                      const test::CustomizedB& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool test::operator!=(const test::CustomizedB& lhs,
                      const test::CustomizedB& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& test::operator<<(bsl::ostream&            stream,
                               const test::CustomizedB& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_customizedb.cpp  -*-C++-*-

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char CustomizedB::CLASS_NAME[] = "CustomizedB";
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
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_enumerationa.h   -*-C++-*-
#ifndef INCLUDED_TEST_ENUMERATIONA
#define INCLUDED_TEST_ENUMERATIONA

//@PURPOSE: TBD: provide purpose.
//
//@CLASSES:
//  EnumerationA: an enumeration class
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TBD: provide annotation for 'EnumerationA'

namespace BloombergLP {

namespace test {

struct EnumerationA {

  public:
    // TYPES
    enum Value {
        VALUE1 = 1,
            // TBD: provide annotation
        VALUE2 = 2
            // TBD: provide annotation
    };

    enum {
        NUM_ENUMERATORS = 2 // the number of enumerators in the 'Value'
                            // enumeration
    };

    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "EnumerationA")

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];
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
        // undefined.  If 'version' is not supported, 'stream' is marked
        // invalid, but 'value' is unaltered.  Note that no version is read
        // from 'stream'.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    static bsl::ostream& print(bsl::ostream& stream, Value value);
        // Write to the specified 'stream' the string representation of the
        // specified enumeration 'value'.  Return a reference to the modifiable
        // 'stream'.

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
bsl::ostream& operator<<(bsl::ostream& stream, EnumerationA::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int EnumerationA::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

inline
int EnumerationA::fromInt(EnumerationA::Value *result, int number)
{
    enum { SUCCESS = 0, NOT_FOUND = 1 };

    switch (number) {
      case EnumerationA::VALUE1:
      case EnumerationA::VALUE2:
        *result = (EnumerationA::Value)number;
        return SUCCESS;                                               // RETURN
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

inline
bsl::ostream& EnumerationA::print(bsl::ostream&       stream,
                                  EnumerationA::Value value)
{
    return stream << toString(value);
}

// ----------------------------------------------------------------------------

// CLASS METHODS
inline
const char *EnumerationA::toString(EnumerationA::Value value)
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

template <class STREAM>
inline
STREAM& EnumerationA::bdexStreamIn(STREAM&              stream,
                                   EnumerationA::Value& value,
                                   int                  version)
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
STREAM& EnumerationA::bdexStreamOut(STREAM&             stream,
                                    EnumerationA::Value value,
                                    int                 version)
{
    switch (version) {
      case 1: {
        stream.putInt32(value);  // Write the value as an int
      } break;
    }
    return stream;
}

template <class STREAM>
inline
STREAM& streamIn(STREAM& stream, test::EnumerationA::Value& value, int version)
{
    return test::EnumerationA::bdexStreamIn(stream, value, version);
}

inline
int maxSupportedVersion(test::EnumerationA::Value)
{
    return test::EnumerationA::maxSupportedBdexVersion();
}

template <class STREAM>
inline
STREAM& streamOut(STREAM&                          stream,
                  const test::EnumerationA::Value& value,
                  int                              version)
{
    return test::EnumerationA::bdexStreamOut(stream, value, version);
}

}  // close namespace test

// TRAITS
BDLAT_DECL_ENUMERATION_TRAITS(test::EnumerationA)

// FREE OPERATORS
inline
bsl::ostream& test::operator<<(bsl::ostream&             stream,
                               test::EnumerationA::Value rhs)
{
    return test::EnumerationA::print(stream, rhs);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_enumerationa.cpp  -*-C++-*-

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char EnumerationA::CLASS_NAME[] = "EnumerationA";
    // the name of this class

const bdlat_EnumeratorInfo EnumerationA::ENUMERATOR_INFO_ARRAY[] = {
    {
        EnumerationA::VALUE1,
        "VALUE1",                 // name
        sizeof("VALUE1") - 1,     // name length
        "TBD: provide annotation" // annotation
    },
    {
        EnumerationA::VALUE2,
        "VALUE2",                  // name
        sizeof("VALUE2") - 1,      // name length
        "TBD: provide annotation"  // annotation
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

int EnumerationA::fromString(EnumerationA::Value *result,
                             const char          *string,
                             int                  stringLength)
{

    enum { SUCCESS = 0, NOT_FOUND = 1 };

    switch(stringLength) {
        case 6: {
            if (bsl::toupper(string[0])=='V'
             && bsl::toupper(string[1])=='A'
             && bsl::toupper(string[2])=='L'
             && bsl::toupper(string[3])=='U'
             && bsl::toupper(string[4])=='E') {
                switch(bsl::toupper(string[5])) {
                    case '1': {
                        *result = EnumerationA::VALUE1;
                        return SUCCESS;                               // RETURN
                    } break;
                    case '2': {
                        *result = EnumerationA::VALUE2;
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

}  // close namespace test
}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_enumerationb.h   -*-C++-*-
#ifndef INCLUDED_TEST_ENUMERATIONB
#define INCLUDED_TEST_ENUMERATIONB

//@PURPOSE: TBD: provide purpose.
//
//@CLASSES:
//  EnumerationB: an enumeration class
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TBD: provide annotation for 'EnumerationB'

namespace BloombergLP {

namespace test {

struct EnumerationB {

  public:
    // TYPES
    enum Value {
        VALUE1 = 2,
            // TBD: provide annotation
        VALUE2 = 1
            // TBD: provide annotation
    };

    enum {
        NUM_ENUMERATORS = 2 // the number of enumerators in the 'Value'
                            // enumeration
    };

    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "EnumerationB")

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];
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
        // undefined.  If 'version' is not supported, 'stream' is marked
        // invalid, but 'value' is unaltered.  Note that no version is read
        // from 'stream'.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    static bsl::ostream& print(bsl::ostream& stream, Value value);
        // Write to the specified 'stream' the string representation of the
        // specified enumeration 'value'.  Return a reference to the modifiable
        // 'stream'.

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
bsl::ostream& operator<<(bsl::ostream& stream, EnumerationB::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int EnumerationB::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

inline
int EnumerationB::fromInt(EnumerationB::Value *result, int number)
{
    enum { SUCCESS = 0, NOT_FOUND = 1 };

    switch (number) {
      case EnumerationB::VALUE1:
      case EnumerationB::VALUE2:
        *result = (EnumerationB::Value)number;
        return SUCCESS;                                               // RETURN
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

inline
bsl::ostream& EnumerationB::print(bsl::ostream&       stream,
                                  EnumerationB::Value value)
{
    return stream << toString(value);
}

// ----------------------------------------------------------------------------

// CLASS METHODS
inline
const char *EnumerationB::toString(EnumerationB::Value value)
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

template <class STREAM>
inline
STREAM& EnumerationB::bdexStreamIn(STREAM&              stream,
                                   EnumerationB::Value& value,
                                   int                  version)
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
STREAM& EnumerationB::bdexStreamOut(STREAM&             stream,
                                    EnumerationB::Value value,
                                    int                 version)
{
    switch (version) {
      case 1: {
        stream.putInt32(value);  // Write the value as an int
      } break;
    }
    return stream;
}

template <class STREAM>
inline
STREAM& streamIn(STREAM& stream, test::EnumerationB::Value& value, int version)
{
    return test::EnumerationB::bdexStreamIn(stream, value, version);
}

inline
int maxSupportedVersion(test::EnumerationB::Value)
{
    return test::EnumerationB::maxSupportedBdexVersion();
}

template <class STREAM>
inline
STREAM& streamOut(STREAM&                          stream,
                  const test::EnumerationB::Value& value,
                  int                              version)
{
    return test::EnumerationB::bdexStreamOut(stream, value, version);
}

}  // close namespace test

// TRAITS
BDLAT_DECL_ENUMERATION_TRAITS(test::EnumerationB)

// FREE OPERATORS
inline
bsl::ostream& test::operator<<(bsl::ostream&             stream,
                               test::EnumerationB::Value rhs)
{
    return test::EnumerationB::print(stream, rhs);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_enumerationb.cpp  -*-C++-*-

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char EnumerationB::CLASS_NAME[] = "EnumerationB";
    // the name of this class

const bdlat_EnumeratorInfo EnumerationB::ENUMERATOR_INFO_ARRAY[] = {
    {
        EnumerationB::VALUE1,
        "VALUE1",                  // name
        sizeof("VALUE1") - 1,      // name length
        "TBD: provide annotation"  // annotation
    },
    {
        EnumerationB::VALUE2,
        "VALUE2",                  // name
        sizeof("VALUE2") - 1,      // name length
        "TBD: provide annotation"  // annotation
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

int EnumerationB::fromString(EnumerationB::Value *result,
                             const char          *string,
                             int                  stringLength)
{

    enum { SUCCESS = 0, NOT_FOUND = 1 };

    switch(stringLength) {
        case 6: {
            if (bsl::toupper(string[0])=='V'
             && bsl::toupper(string[1])=='A'
             && bsl::toupper(string[2])=='L'
             && bsl::toupper(string[3])=='U'
             && bsl::toupper(string[4])=='E') {
                switch(bsl::toupper(string[5])) {
                    case '1': {
                        *result = EnumerationB::VALUE1;
                        return SUCCESS;                               // RETURN
                    } break;
                    case '2': {
                        *result = EnumerationB::VALUE2;
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

}  // close namespace test
}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_sequencea.h   -*-C++-*-
#ifndef INCLUDED_TEST_SEQUENCEA
#define INCLUDED_TEST_SEQUENCEA

//@PURPOSE: TBD: provide purpose.
//
//@CLASSES:
//  SequenceA: a sequence class
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TBD: provide annotation for 'SequenceA'

namespace BloombergLP {

namespace test {

class SequenceA {

  private:
    int d_attribute1; // TBD: provide annotation
    bsl::string d_attribute2; // TBD: provide annotation

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
        // the name of this class (i.e., "SequenceA")

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

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
    explicit SequenceA(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceA' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    SequenceA(const SequenceA& original, bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceA' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~SequenceA();
        // Destroy this object.

    // MANIPULATORS
    SequenceA& operator=(const SequenceA& rhs);
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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'manipulator' (i.e., the invocation that terminated
        // the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'id', supplying
        // 'manipulator' with the corresponding attribute information
        // structure.  Return the value returned from the invocation of
        // 'manipulator' if 'id' identifies an attribute of this class, and -1
        // otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'name' of the
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
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor' with
        // the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'accessor' (i.e., the invocation that terminated the
        // sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const int& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const bsl::string& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.

};

// FREE OPERATORS
inline
bool operator==(const SequenceA& lhs, const SequenceA& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const SequenceA& lhs, const SequenceA& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const SequenceA& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int SequenceA::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ----------------------------------------------------------------------------

// CREATORS
inline
SequenceA::SequenceA(bslma::Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(bslma::Default::allocator(basicAllocator))
{
}

inline
SequenceA::SequenceA(const SequenceA&  original,
                     bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2,
               bslma::Default::allocator(basicAllocator))
{
}

inline
SequenceA::~SequenceA()
{
}

// MANIPULATORS
inline
SequenceA&
SequenceA::operator=(const SequenceA& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& SequenceA::bdexStreamIn(STREAM& stream, int version)
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
void SequenceA::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
}

template <class MANIPULATOR>
inline
int SequenceA::manipulateAttributes(MANIPULATOR& manipulator)
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
int SequenceA::manipulateAttribute(MANIPULATOR& manipulator, int id)
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
int SequenceA::manipulateAttribute(MANIPULATOR&  manipulator,
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
int& SequenceA::attribute1()
{
    return d_attribute1;
}

inline
bsl::string& SequenceA::attribute2()
{
    return d_attribute2;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& SequenceA::bdexStreamOut(STREAM& stream, int version) const
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
int SequenceA::accessAttributes(ACCESSOR& accessor) const
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
int SequenceA::accessAttribute(ACCESSOR& accessor, int id) const
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
int SequenceA::accessAttribute(ACCESSOR&   accessor,
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
const int& SequenceA::attribute1() const
{
    return d_attribute1;
}

inline
const bsl::string& SequenceA::attribute2() const
{
    return d_attribute2;
}

}  // close namespace test

// TRAITS
BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::SequenceA)

// FREE OPERATORS
inline
bool test::operator==(const test::SequenceA& lhs, const test::SequenceA& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2();
}

inline
bool test::operator!=(const test::SequenceA& lhs, const test::SequenceA& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2();
}

inline
bsl::ostream& test::operator<<(bsl::ostream&          stream,
                               const test::SequenceA& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_sequencea.cpp  -*-C++-*-

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char SequenceA::CLASS_NAME[] = "SequenceA";
    // the name of this class

const bdlat_AttributeInfo SequenceA::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",              // name
        sizeof("Attribute1") - 1,  // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",              // name
        sizeof("Attribute2") - 1,  // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdlat_AttributeInfo *SequenceA::lookupAttributeInfo(
                                                        const char *name,
                                                        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bsl::toupper(name[0])=='A'
             && bsl::toupper(name[1])=='T'
             && bsl::toupper(name[2])=='T'
             && bsl::toupper(name[3])=='R'
             && bsl::toupper(name[4])=='I'
             && bsl::toupper(name[5])=='B'
             && bsl::toupper(name[6])=='U'
             && bsl::toupper(name[7])=='T'
             && bsl::toupper(name[8])=='E') {
                switch(bsl::toupper(name[9])) {
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

const bdlat_AttributeInfo *SequenceA::lookupAttributeInfo(int id)
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

bsl::ostream& SequenceA::print(bsl::ostream& stream,
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
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_sequenceb.h   -*-C++-*-
#ifndef INCLUDED_TEST_SEQUENCEB
#define INCLUDED_TEST_SEQUENCEB

//@PURPOSE: TBD: provide purpose.
//
//@CLASSES:
//  SequenceB: a sequence class
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TBD: provide annotation for 'SequenceB'

namespace BloombergLP {

namespace test {

class SequenceB {

  private:
    bsl::string d_attribute2; // TBD: provide annotation
    int d_attribute1; // TBD: provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 2 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 0,
            // index for "Attribute2" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 1
            // index for "Attribute1" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE2 = 0,
            // id for "Attribute2" attribute
        ATTRIBUTE_ID_ATTRIBUTE1 = 1
            // id for "Attribute1" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "SequenceB")

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

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
    explicit SequenceB(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceB' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    SequenceB(const SequenceB& original, bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceB' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~SequenceB();
        // Destroy this object.

    // MANIPULATORS
    SequenceB& operator=(const SequenceB& rhs);
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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'manipulator' (i.e., the invocation that terminated
        // the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'id', supplying
        // 'manipulator' with the corresponding attribute information
        // structure.  Return the value returned from the invocation of
        // 'manipulator' if 'id' identifies an attribute of this class, and -1
        // otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bsl::string& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
        // object.

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
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
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor' with
        // the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'accessor' (i.e., the invocation that terminated the
        // sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bsl::string& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.

    const int& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

};

// FREE OPERATORS
inline
bool operator==(const SequenceB& lhs, const SequenceB& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const SequenceB& lhs, const SequenceB& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const SequenceB& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int SequenceB::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ----------------------------------------------------------------------------

// CREATORS
inline
SequenceB::SequenceB(bslma::Allocator *basicAllocator)
: d_attribute2(bslma::Default::allocator(basicAllocator))
, d_attribute1()
{
}

inline
SequenceB::SequenceB(const SequenceB&  original,
                     bslma::Allocator *basicAllocator)
: d_attribute2(original.d_attribute2,
               bslma::Default::allocator(basicAllocator))
, d_attribute1(original.d_attribute1)
{
}

inline
SequenceB::~SequenceB()
{
}

// MANIPULATORS
inline
SequenceB&
SequenceB::operator=(const SequenceB& rhs)
{
    if (this != &rhs) {
        d_attribute2 = rhs.d_attribute2;
        d_attribute1 = rhs.d_attribute1;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& SequenceB::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            streamIn(
                  stream, d_attribute2, 1);
            streamIn(
                  stream, d_attribute1, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void SequenceB::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
}

template <class MANIPULATOR>
inline
int SequenceB::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int SequenceB::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int SequenceB::manipulateAttribute(MANIPULATOR&  manipulator,
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
bsl::string& SequenceB::attribute2()
{
    return d_attribute2;
}

inline
int& SequenceB::attribute1()
{
    return d_attribute1;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& SequenceB::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        streamOut(stream, d_attribute2, 1);
        streamOut(stream, d_attribute1, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int SequenceB::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int SequenceB::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int SequenceB::accessAttribute(ACCESSOR&   accessor,
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
const bsl::string& SequenceB::attribute2() const
{
    return d_attribute2;
}

inline
const int& SequenceB::attribute1() const
{
    return d_attribute1;
}

}  // close namespace test

// TRAITS
BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::SequenceB)

// FREE OPERATORS
inline
bool test::operator==(const test::SequenceB& lhs, const test::SequenceB& rhs)
{
    return  lhs.attribute2() == rhs.attribute2()
         && lhs.attribute1() == rhs.attribute1();
}

inline
bool test::operator!=(const test::SequenceB& lhs, const test::SequenceB& rhs)
{
    return  lhs.attribute2() != rhs.attribute2()
         || lhs.attribute1() != rhs.attribute1();
}

inline
bsl::ostream& test::operator<<(bsl::ostream&          stream,
                               const test::SequenceB& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_sequenceb.cpp  -*-C++-*-

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char SequenceB::CLASS_NAME[] = "SequenceB";
    // the name of this class

const bdlat_AttributeInfo SequenceB::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",              // name
        sizeof("Attribute2") - 1,  // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",              // name
        sizeof("Attribute1") - 1,  // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdlat_AttributeInfo *SequenceB::lookupAttributeInfo(
                                                        const char *name,
                                                        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bsl::toupper(name[0])=='A'
             && bsl::toupper(name[1])=='T'
             && bsl::toupper(name[2])=='T'
             && bsl::toupper(name[3])=='R'
             && bsl::toupper(name[4])=='I'
             && bsl::toupper(name[5])=='B'
             && bsl::toupper(name[6])=='U'
             && bsl::toupper(name[7])=='T'
             && bsl::toupper(name[8])=='E') {
                switch(bsl::toupper(name[9])) {
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

const bdlat_AttributeInfo *SequenceB::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
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

bsl::ostream& SequenceB::print(bsl::ostream& stream,
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
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                             -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace test
}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_employee.h   -*-C++-*-
#ifndef INCLUDED_TEST_EMPLOYEE
#define INCLUDED_TEST_EMPLOYEE

//@PURPOSE: TBD: provide purpose.
//
//@CLASSES:
//  Employee: an employee class
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TBD: provide annotation for 'Employee'

namespace BloombergLP {

namespace test {

class Employee {

  private:
    bsl::string d_name; // TBD: provide annotation
    bsl::string d_dept; // TBD: provide annotation
    int d_age; // TBD: provide annotation
    float d_salary; // TBD: provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 4 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_NAME = 0,
            // index for "Name" attribute
        ATTRIBUTE_INDEX_DEPT = 1,
            // index for "Dept" attribute
        ATTRIBUTE_INDEX_AGE = 2,
            // index for "Age" attribute
        ATTRIBUTE_INDEX_SALARY = 3
            // index for "Salary" attribute
    };

    enum {
        ATTRIBUTE_ID_NAME = 0,
            // id for "Name" attribute
        ATTRIBUTE_ID_DEPT = 1,
            // id for "Dept" attribute
        ATTRIBUTE_ID_AGE = 2,
            // id for "Age" attribute
        ATTRIBUTE_ID_SALARY = 3
            // id for "Salary" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Employee")

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

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
    explicit Employee(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Employee' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Employee(const Employee& original, bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Employee' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'manipulator' (i.e., the invocation that terminated
        // the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'id', supplying
        // 'manipulator' with the corresponding attribute information
        // structure.  Return the value returned from the invocation of
        // 'manipulator' if 'id' identifies an attribute of this class, and -1
        // otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bsl::string& name();
        // Return a reference to the modifiable "Name" attribute of this
        // object.

    bsl::string& dept();
        // Return a reference to the modifiable "Dept" attribute of this
        // object.

    int& age();
        // Return a reference to the modifiable "Age" attribute of this object.

    float& salary();
        // Return a reference to the modifiable "Salary" attribute of this
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
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor' with
        // the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'accessor' (i.e., the invocation that terminated the
        // sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bsl::string& name() const;
        // Return a reference to the non-modifiable "Name" attribute of this
        // object.

    const bsl::string& dept() const;
        // Return a reference to the non-modifiable "Dept" attribute of this
        // object.

    const int& age() const;
        // Return a reference to the non-modifiable "Age" attribute of this
        // object.

    const float& salary() const;
        // Return a reference to the non-modifiable "Salary" attribute of this
        // object.

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
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int Employee::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ----------------------------------------------------------------------------

// CREATORS
inline
Employee::Employee(bslma::Allocator *basicAllocator)
: d_name(bslma::Default::allocator(basicAllocator))
, d_dept(bslma::Default::allocator(basicAllocator))
, d_age()
, d_salary()
{
}

inline
Employee::Employee(const Employee&   original,
                   bslma::Allocator *basicAllocator)
: d_name(original.d_name, bslma::Default::allocator(basicAllocator))
, d_dept(original.d_dept, bslma::Default::allocator(basicAllocator))
, d_age(original.d_age)
, d_salary(original.d_salary)
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
        d_dept = rhs.d_dept;
        d_age = rhs.d_age;
        d_salary = rhs.d_salary;
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
                  stream, d_dept, 1);
            streamIn(
                  stream, d_age, 1);
            streamIn(
                  stream, d_salary, 1);
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
    bdlat_ValueTypeFunctions::reset(&d_name);
    bdlat_ValueTypeFunctions::reset(&d_dept);
    bdlat_ValueTypeFunctions::reset(&d_age);
    bdlat_ValueTypeFunctions::reset(&d_salary);
}

template <class MANIPULATOR>
inline
int Employee::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_dept, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEPT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_salary, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SALARY]);
    if (ret) {
        return ret;                                                   // RETURN
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
      case ATTRIBUTE_ID_DEPT: {
        return manipulator(&d_dept,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEPT]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_AGE: {
        return manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_SALARY: {
        return manipulator(&d_salary,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SALARY]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int Employee::manipulateAttribute(MANIPULATOR&  manipulator,
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
bsl::string& Employee::name()
{
    return d_name;
}

inline
bsl::string& Employee::dept()
{
    return d_dept;
}

inline
int& Employee::age()
{
    return d_age;
}

inline
float& Employee::salary()
{
    return d_salary;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& Employee::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        streamOut(stream, d_name, 1);
        streamOut(stream, d_dept, 1);
        streamOut(stream, d_age, 1);
        streamOut(stream, d_salary, 1);
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
        return ret;                                                   // RETURN
    }

    ret = accessor(d_dept, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEPT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_salary, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SALARY]);
    if (ret) {
        return ret;                                                   // RETURN
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
      case ATTRIBUTE_ID_DEPT: {
        return accessor(d_dept, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEPT]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_AGE: {
        return accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_SALARY: {
        return accessor(d_salary,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SALARY]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int Employee::accessAttribute(ACCESSOR&   accessor,
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
const bsl::string& Employee::name() const
{
    return d_name;
}

inline
const bsl::string& Employee::dept() const
{
    return d_dept;
}

inline
const int& Employee::age() const
{
    return d_age;
}

inline
const float& Employee::salary() const
{
    return d_salary;
}

}  // close namespace test

// TRAITS
BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Employee)

// FREE OPERATORS
inline
bool test::operator==(const test::Employee& lhs, const test::Employee& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.dept() == rhs.dept()
         && lhs.age() == rhs.age()
         && lhs.salary() == rhs.salary();
}

inline
bool test::operator!=(const test::Employee& lhs, const test::Employee& rhs)
{
    return  lhs.name() != rhs.name()
         || lhs.dept() != rhs.dept()
         || lhs.age() != rhs.age()
         || lhs.salary() != rhs.salary();
}

inline
bsl::ostream& test::operator<<(bsl::ostream& stream, const test::Employee& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_employee.cpp  -*-C++-*-

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char Employee::CLASS_NAME[] = "Employee";
    // the name of this class

const bdlat_AttributeInfo Employee::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "Name",                    // name
        sizeof("Name") - 1,        // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_DEPT,
        "Dept",                    // name
        sizeof("Dept") - 1,        // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_AGE,
        "Age",                     // name
        sizeof("Age") - 1,         // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_SALARY,
        "Salary",                     // name
        sizeof("Salary") - 1,         // name length
        "TBD: provide annotation",    // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdlat_AttributeInfo *Employee::lookupAttributeInfo(
                                                        const char *name,
                                                        int         nameLength)
{
    switch(nameLength) {
        case 3: {
            if (bsl::toupper(name[0])=='A'
             && bsl::toupper(name[1])=='G'
             && bsl::toupper(name[2])=='E') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];    // RETURN
            }
        } break;
        case 4: {
            switch(bsl::toupper(name[0])) {
                case 'D': {
                    if (bsl::toupper(name[1])=='E'
                     && bsl::toupper(name[2])=='P'
                     && bsl::toupper(name[3])=='T') {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEPT];
                                                                      // RETURN
                    }
                } break;
                case 'N': {
                    if (bsl::toupper(name[1])=='A'
                     && bsl::toupper(name[2])=='M'
                     && bsl::toupper(name[3])=='E') {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
                                                                      // RETURN
                    }
                } break;
            }
        } break;
        case 6: {
            if (bsl::toupper(name[0])=='S'
             && bsl::toupper(name[1])=='A'
             && bsl::toupper(name[2])=='L'
             && bsl::toupper(name[3])=='A'
             && bsl::toupper(name[4])=='R'
             && bsl::toupper(name[5])=='Y') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SALARY]; // RETURN
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Employee::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      case ATTRIBUTE_ID_DEPT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEPT];
      case ATTRIBUTE_ID_AGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
      case ATTRIBUTE_ID_SALARY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SALARY];
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

bsl::ostream& Employee::print(bsl::ostream& stream,
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
        stream << "Name = ";
        bdlb::PrintMethods::print(stream, d_name,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Dept = ";
        bdlb::PrintMethods::print(stream, d_dept,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Age = ";
        bdlb::PrintMethods::print(stream, d_age,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Salary = ";
        bdlb::PrintMethods::print(stream, d_salary,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Name = ";
        bdlb::PrintMethods::print(stream, d_name,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Dept = ";
        bdlb::PrintMethods::print(stream, d_dept,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Age = ";
        bdlb::PrintMethods::print(stream, d_age,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Salary = ";
        bdlb::PrintMethods::print(stream, d_salary,
                             -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace test
}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_trainee.h   -*-C++-*-
#ifndef INCLUDED_TEST_TRAINEE
#define INCLUDED_TEST_TRAINEE

//@PURPOSE: TBD: provide purpose.
//
//@CLASSES:
//  Trainee: a trainee class
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TBD: provide annotation for 'Trainee'

namespace BloombergLP {

namespace test {

class Trainee {

  private:
    bsl::string d_name; // TBD: provide annotation
    bsl::string d_dept; // TBD: provide annotation
    int d_age; // TBD: provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_NAME = 0,
            // index for "Name" attribute
        ATTRIBUTE_INDEX_DEPT = 1,
            // index for "Dept" attribute
        ATTRIBUTE_INDEX_AGE = 2
            // index for "Age" attribute
    };

    enum {
        ATTRIBUTE_ID_NAME = 0,
            // id for "Name" attribute
        ATTRIBUTE_ID_DEPT = 1,
            // id for "Dept" attribute
        ATTRIBUTE_ID_AGE = 2
            // id for "Age" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Trainee")

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

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
    explicit Trainee(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Trainee' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Trainee(const Trainee& original, bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Trainee' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Trainee();
        // Destroy this object.

    // MANIPULATORS
    Trainee& operator=(const Trainee& rhs);
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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'manipulator' (i.e., the invocation that terminated
        // the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'id', supplying
        // 'manipulator' with the corresponding attribute information
        // structure.  Return the value returned from the invocation of
        // 'manipulator' if 'id' identifies an attribute of this class, and -1
        // otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bsl::string& name();
        // Return a reference to the modifiable "Name" attribute of this
        // object.

    bsl::string& dept();
        // Return a reference to the modifiable "Dept" attribute of this
        // object.

    int& age();
        // Return a reference to the modifiable "Age" attribute of this object.

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
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor' with
        // the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'accessor' (i.e., the invocation that terminated the
        // sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bsl::string& name() const;
        // Return a reference to the non-modifiable "Name" attribute of this
        // object.

    const bsl::string& dept() const;
        // Return a reference to the non-modifiable "Dept" attribute of this
        // object.

    const int& age() const;
        // Return a reference to the non-modifiable "Age" attribute of this
        // object.

};

// FREE OPERATORS
inline
bool operator==(const Trainee& lhs, const Trainee& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Trainee& lhs, const Trainee& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Trainee& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int Trainee::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ----------------------------------------------------------------------------

// CREATORS
inline
Trainee::Trainee(bslma::Allocator *basicAllocator)
: d_name(bslma::Default::allocator(basicAllocator))
, d_dept(bslma::Default::allocator(basicAllocator))
, d_age()
{
}

inline
Trainee::Trainee(const Trainee& original, bslma::Allocator *basicAllocator)
: d_name(original.d_name, bslma::Default::allocator(basicAllocator))
, d_dept(original.d_dept, bslma::Default::allocator(basicAllocator))
, d_age(original.d_age)
{
}

inline
Trainee::~Trainee()
{
}

// MANIPULATORS
inline
Trainee&
Trainee::operator=(const Trainee& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_dept = rhs.d_dept;
        d_age = rhs.d_age;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& Trainee::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            streamIn(
                  stream, d_name, 1);
            streamIn(
                  stream, d_dept, 1);
            streamIn(
                  stream, d_age, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void Trainee::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_name);
    bdlat_ValueTypeFunctions::reset(&d_dept);
    bdlat_ValueTypeFunctions::reset(&d_age);
}

template <class MANIPULATOR>
inline
int Trainee::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_dept, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEPT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int Trainee::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_DEPT: {
        return manipulator(&d_dept,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEPT]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_AGE: {
        return manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int Trainee::manipulateAttribute(MANIPULATOR&  manipulator,
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
bsl::string& Trainee::name()
{
    return d_name;
}

inline
bsl::string& Trainee::dept()
{
    return d_dept;
}

inline
int& Trainee::age()
{
    return d_age;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& Trainee::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        streamOut(stream, d_name, 1);
        streamOut(stream, d_dept, 1);
        streamOut(stream, d_age, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int Trainee::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_dept, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEPT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Trainee::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_DEPT: {
        return accessor(d_dept, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEPT]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_AGE: {
        return accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int Trainee::accessAttribute(ACCESSOR&   accessor,
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
const bsl::string& Trainee::name() const
{
    return d_name;
}

inline
const bsl::string& Trainee::dept() const
{
    return d_dept;
}

inline
const int& Trainee::age() const
{
    return d_age;
}

}  // close namespace test

// TRAITS
BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Trainee)

// FREE OPERATORS
inline
bool test::operator==(const test::Trainee& lhs, const test::Trainee& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.dept() == rhs.dept()
         && lhs.age() == rhs.age();
}

inline
bool test::operator!=(const test::Trainee& lhs, const test::Trainee& rhs)
{
    return  lhs.name() != rhs.name()
         || lhs.dept() != rhs.dept()
         || lhs.age() != rhs.age();
}

inline
bsl::ostream& test::operator<<(bsl::ostream& stream, const test::Trainee& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_trainee.cpp  -*-C++-*-

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char Trainee::CLASS_NAME[] = "Trainee";
    // the name of this class

const bdlat_AttributeInfo Trainee::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "Name",                    // name
        sizeof("Name") - 1,        // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_DEPT,
        "Dept",                    // name
        sizeof("Dept") - 1,        // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_AGE,
        "Age",                     // name
        sizeof("Age") - 1,         // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdlat_AttributeInfo *Trainee::lookupAttributeInfo(const char *name,
                                                        int         nameLength)
{
    switch(nameLength) {
        case 3: {
            if (bsl::toupper(name[0])=='A'
             && bsl::toupper(name[1])=='G'
             && bsl::toupper(name[2])=='E') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];    // RETURN
            }
        } break;
        case 4: {
            switch(bsl::toupper(name[0])) {
                case 'D': {
                    if (bsl::toupper(name[1])=='E'
                     && bsl::toupper(name[2])=='P'
                     && bsl::toupper(name[3])=='T') {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEPT];
                                                                      // RETURN
                    }
                } break;
                case 'N': {
                    if (bsl::toupper(name[1])=='A'
                     && bsl::toupper(name[2])=='M'
                     && bsl::toupper(name[3])=='E') {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
                                                                      // RETURN
                    }
                } break;
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Trainee::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      case ATTRIBUTE_ID_DEPT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEPT];
      case ATTRIBUTE_ID_AGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
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

bsl::ostream& Trainee::print(bsl::ostream& stream,
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
        stream << "Name = ";
        bdlb::PrintMethods::print(stream, d_name,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Dept = ";
        bdlb::PrintMethods::print(stream, d_dept,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Age = ";
        bdlb::PrintMethods::print(stream, d_age,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Name = ";
        bdlb::PrintMethods::print(stream, d_name,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Dept = ";
        bdlb::PrintMethods::print(stream, d_dept,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Age = ";
        bdlb::PrintMethods::print(stream, d_age,
                             -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace test
}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_mixedsequence.h   -*-C++-*-
#ifndef INCLUDED_TEST_MIXEDSEQUENCE
#define INCLUDED_TEST_MIXEDSEQUENCE

//@PURPOSE: TBD: provide purpose.
//
//@CLASSES:
//  MixedSequence: a mixed sequence
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TBD: provide annotation for 'MixedSequence'

namespace BloombergLP {

namespace test {

class MixedSequence {

  private:
    ChoiceA d_attribute1; // TBD: provide annotation
    CustomizedA d_attribute2; // TBD: provide annotation
    bdlb::NullableValue<int> d_attribute3; // TBD: provide annotation
    EnumerationA::Value d_attribute4; // TBD: provide annotation
    int d_attribute5; // TBD: provide annotation
    bsl::string d_attribute6; // TBD: provide annotation
    SequenceA d_attribute7; // TBD: provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 7 // the number of attributes in this class
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
        ATTRIBUTE_INDEX_ATTRIBUTE5 = 4,
            // index for "Attribute5" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE6 = 5,
            // index for "Attribute6" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE7 = 6
            // index for "Attribute7" attribute
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
        ATTRIBUTE_ID_ATTRIBUTE5 = 4,
            // id for "Attribute5" attribute
        ATTRIBUTE_ID_ATTRIBUTE6 = 5,
            // id for "Attribute6" attribute
        ATTRIBUTE_ID_ATTRIBUTE7 = 6
            // id for "Attribute7" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MixedSequence")

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

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
    explicit MixedSequence(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MixedSequence' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    MixedSequence(const MixedSequence&  original,
                  bslma::Allocator     *basicAllocator = 0);
        // Create an object of type 'MixedSequence' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MixedSequence();
        // Destroy this object.

    // MANIPULATORS
    MixedSequence& operator=(const MixedSequence& rhs);
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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'manipulator' (i.e., the invocation that terminated
        // the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'id', supplying
        // 'manipulator' with the corresponding attribute information
        // structure.  Return the value returned from the invocation of
        // 'manipulator' if 'id' identifies an attribute of this class, and -1
        // otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    ChoiceA& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    CustomizedA& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
        // object.

    bdlb::NullableValue<int>& attribute3();
        // Return a reference to the modifiable "Attribute3" attribute of this
        // object.

    EnumerationA::Value& attribute4();
        // Return a reference to the modifiable "Attribute4" attribute of this
        // object.

    int& attribute5();
        // Return a reference to the modifiable "Attribute5" attribute of this
        // object.

    bsl::string& attribute6();
        // Return a reference to the modifiable "Attribute6" attribute of this
        // object.

    SequenceA& attribute7();
        // Return a reference to the modifiable "Attribute7" attribute of this
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
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor' with
        // the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'accessor' (i.e., the invocation that terminated the
        // sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const ChoiceA& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const CustomizedA& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.

    const bdlb::NullableValue<int>& attribute3() const;
        // Return a reference to the non-modifiable "Attribute3" attribute of
        // this object.

    const EnumerationA::Value& attribute4() const;
        // Return a reference to the non-modifiable "Attribute4" attribute of
        // this object.

    const int& attribute5() const;
        // Return a reference to the non-modifiable "Attribute5" attribute of
        // this object.

    const bsl::string& attribute6() const;
        // Return a reference to the non-modifiable "Attribute6" attribute of
        // this object.

    const SequenceA& attribute7() const;
        // Return a reference to the non-modifiable "Attribute7" attribute of
        // this object.

};

// FREE OPERATORS
inline
bool operator==(const MixedSequence& lhs, const MixedSequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MixedSequence& lhs, const MixedSequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MixedSequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int MixedSequence::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ----------------------------------------------------------------------------

// CREATORS
inline
MixedSequence::MixedSequence(bslma::Allocator *basicAllocator)
: d_attribute1(bslma::Default::allocator(basicAllocator))
, d_attribute2(bslma::Default::allocator(basicAllocator))
, d_attribute3()
, d_attribute4(static_cast<EnumerationA::Value>(0))
, d_attribute5()
, d_attribute6(bslma::Default::allocator(basicAllocator))
, d_attribute7(bslma::Default::allocator(basicAllocator))
{
}

inline
MixedSequence::MixedSequence(const MixedSequence&  original,
                             bslma::Allocator     *basicAllocator)
: d_attribute1(original.d_attribute1,
               bslma::Default::allocator(basicAllocator))
, d_attribute2(original.d_attribute2,
               bslma::Default::allocator(basicAllocator))
, d_attribute3(original.d_attribute3)
, d_attribute4(original.d_attribute4)
, d_attribute5(original.d_attribute5)
, d_attribute6(original.d_attribute6,
               bslma::Default::allocator(basicAllocator))
, d_attribute7(original.d_attribute7,
               bslma::Default::allocator(basicAllocator))
{
}

inline
MixedSequence::~MixedSequence()
{
}

// MANIPULATORS
inline
MixedSequence&
MixedSequence::operator=(const MixedSequence& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
        d_attribute3 = rhs.d_attribute3;
        d_attribute4 = rhs.d_attribute4;
        d_attribute5 = rhs.d_attribute5;
        d_attribute6 = rhs.d_attribute6;
        d_attribute7 = rhs.d_attribute7;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& MixedSequence::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            streamIn(
                  stream, d_attribute1, 1);
            streamIn(
                  stream, d_attribute2, 1);
            streamIn(
                  stream, d_attribute3, 1);
            streamIn(
                  stream, d_attribute4, 1);
            streamIn(
                  stream, d_attribute5, 1);
            streamIn(
                  stream, d_attribute6, 1);
            streamIn(
                  stream, d_attribute7, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MixedSequence::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
    bdlat_ValueTypeFunctions::reset(&d_attribute3);
    bdlat_ValueTypeFunctions::reset(&d_attribute4);
    bdlat_ValueTypeFunctions::reset(&d_attribute5);
    bdlat_ValueTypeFunctions::reset(&d_attribute6);
    bdlat_ValueTypeFunctions::reset(&d_attribute7);
}

template <class MANIPULATOR>
inline
int MixedSequence::manipulateAttributes(MANIPULATOR& manipulator)
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

    ret = manipulator(&d_attribute3,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute4,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute5,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute6,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute7,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MixedSequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
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
      case ATTRIBUTE_ID_ATTRIBUTE6: {
        return manipulator(&d_attribute6,
                          ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE7: {
        return manipulator(&d_attribute7,
                          ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int MixedSequence::manipulateAttribute(MANIPULATOR&  manipulator,
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
ChoiceA& MixedSequence::attribute1()
{
    return d_attribute1;
}

inline
CustomizedA& MixedSequence::attribute2()
{
    return d_attribute2;
}

inline
bdlb::NullableValue<int>& MixedSequence::attribute3()
{
    return d_attribute3;
}

inline
EnumerationA::Value& MixedSequence::attribute4()
{
    return d_attribute4;
}

inline
int& MixedSequence::attribute5()
{
    return d_attribute5;
}

inline
bsl::string& MixedSequence::attribute6()
{
    return d_attribute6;
}

inline
SequenceA& MixedSequence::attribute7()
{
    return d_attribute7;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& MixedSequence::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        streamOut(stream, d_attribute1, 1);
        streamOut(stream, d_attribute2, 1);
        streamOut(stream, d_attribute3, 1);
        streamOut(stream, d_attribute4, 1);
        streamOut(stream, d_attribute5, 1);
        streamOut(stream, d_attribute6, 1);
        streamOut(stream, d_attribute7, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int MixedSequence::accessAttributes(ACCESSOR& accessor) const
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

    ret = accessor(d_attribute3,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute4,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute5,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute6,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute7,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MixedSequence::accessAttribute(ACCESSOR& accessor, int id) const
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
      case ATTRIBUTE_ID_ATTRIBUTE6: {
        return accessor(d_attribute6,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE7: {
        return accessor(d_attribute7,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int MixedSequence::accessAttribute(ACCESSOR&   accessor,
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
const ChoiceA& MixedSequence::attribute1() const
{
    return d_attribute1;
}

inline
const CustomizedA& MixedSequence::attribute2() const
{
    return d_attribute2;
}

inline
const bdlb::NullableValue<int>& MixedSequence::attribute3() const
{
    return d_attribute3;
}

inline
const EnumerationA::Value& MixedSequence::attribute4() const
{
    return d_attribute4;
}

inline
const int& MixedSequence::attribute5() const
{
    return d_attribute5;
}

inline
const bsl::string& MixedSequence::attribute6() const
{
    return d_attribute6;
}

inline
const SequenceA& MixedSequence::attribute7() const
{
    return d_attribute7;
}

}  // close namespace test

// TRAITS
BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MixedSequence)

// FREE OPERATORS
inline
bool test::operator==(const test::MixedSequence& lhs,
                      const test::MixedSequence& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2()
         && lhs.attribute3() == rhs.attribute3()
         && lhs.attribute4() == rhs.attribute4()
         && lhs.attribute5() == rhs.attribute5()
         && lhs.attribute6() == rhs.attribute6()
         && lhs.attribute7() == rhs.attribute7();
}

inline
bool test::operator!=(const test::MixedSequence& lhs,
                      const test::MixedSequence& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2()
         || lhs.attribute3() != rhs.attribute3()
         || lhs.attribute4() != rhs.attribute4()
         || lhs.attribute5() != rhs.attribute5()
         || lhs.attribute6() != rhs.attribute6()
         || lhs.attribute7() != rhs.attribute7();
}

inline
bsl::ostream& test::operator<<(bsl::ostream&              stream,
                               const test::MixedSequence& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_mixedsequence.cpp  -*-C++-*-

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MixedSequence::CLASS_NAME[] = "MixedSequence";
    // the name of this class

const bdlat_AttributeInfo MixedSequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                 // name
        sizeof("Attribute1") - 1,     // name length
        "TBD: provide annotation",    // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                 // name
        sizeof("Attribute2") - 1,     // name length
        "TBD: provide annotation",    // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE3,
        "Attribute3",              // name
        sizeof("Attribute3") - 1,  // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE4,
        "Attribute4",                 // name
        sizeof("Attribute4") - 1,     // name length
        "TBD: provide annotation",    // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE5,
        "Attribute5",              // name
        sizeof("Attribute5") - 1,  // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE6,
        "Attribute6",              // name
        sizeof("Attribute6") - 1,  // name length
        "TBD: provide annotation", // annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE7,
        "Attribute7",                 // name
        sizeof("Attribute7") - 1,     // name length
        "TBD: provide annotation",    // annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdlat_AttributeInfo *MixedSequence::lookupAttributeInfo(
                                                        const char *name,
                                                        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bsl::toupper(name[0])=='A'
             && bsl::toupper(name[1])=='T'
             && bsl::toupper(name[2])=='T'
             && bsl::toupper(name[3])=='R'
             && bsl::toupper(name[4])=='I'
             && bsl::toupper(name[5])=='B'
             && bsl::toupper(name[6])=='U'
             && bsl::toupper(name[7])=='T'
             && bsl::toupper(name[8])=='E') {
                switch(bsl::toupper(name[9])) {
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
                    case '3': {
                        return
                             &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return
                             &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4];
                                                                      // RETURN
                    } break;
                    case '5': {
                        return
                             &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5];
                                                                      // RETURN
                    } break;
                    case '6': {
                        return
                             &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6];
                                                                      // RETURN
                    } break;
                    case '7': {
                        return
                             &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *MixedSequence::lookupAttributeInfo(int id)
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
      case ATTRIBUTE_ID_ATTRIBUTE6:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6];
      case ATTRIBUTE_ID_ATTRIBUTE7:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7];
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

bsl::ostream& MixedSequence::print(bsl::ostream& stream,
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

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute3 = ";
        bdlb::PrintMethods::print(stream, d_attribute3,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute4 = ";
        bdlb::PrintMethods::print(stream, d_attribute4,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute5 = ";
        bdlb::PrintMethods::print(stream, d_attribute5,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute6 = ";
        bdlb::PrintMethods::print(stream, d_attribute6,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute7 = ";
        bdlb::PrintMethods::print(stream, d_attribute7,
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

        stream << ' ';
        stream << "Attribute3 = ";
        bdlb::PrintMethods::print(stream, d_attribute3,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute4 = ";
        bdlb::PrintMethods::print(stream, d_attribute4,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute5 = ";
        bdlb::PrintMethods::print(stream, d_attribute5,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute6 = ";
        bdlb::PrintMethods::print(stream, d_attribute6,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute7 = ";
        bdlb::PrintMethods::print(stream, d_attribute7,
                             -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace test
}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// The following snippets of code illustrate the usage of this component.  This
// component can be used with types supported by the 'bdeat' framework.  In
// particular, types generated by the 'bde_xsdcc.pl' tool can be used.  For
// example, suppose we have the following XML schema inside a file called
// 'xsdfile.xsd':
//..
//  <?xml version='1.0' encoding='UTF-8'?>
//  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
//             xmlns:bdem='http://bloomberg.com/schemas/bdem'
//             elementFormDefault='unqualified'>
//
//      <xs:complexType name='Employee'>
//          <xs:sequence>
//              <xs:element name='Name'   type='string'/>
//              <xs:element name='Dept'   type='string'/>
//              <xs:element name='Age'    type='int'/>
//              <xs:element name='Salary' type='float'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='Trainee'>
//          <xs:sequence>
//              <xs:element name='Name' type='string'/>
//              <xs:element name='Dept' type='string'/>
//              <xs:element name='Age'  type='int'/>
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
// 'test_employee' and 'test_trainee' components in the current directory.
//
// Now suppose we want to create a 'hireTrainee' function, that converts a
// trainee to an employee.  Such a function could be written as follows:
//..
//  #include <test_employee.h>
//  #include <test_trainee.h>
//
//  #include <bdlat_symbolicconverter.h>
//
//  using namespace BloombergLP;
//
int hireTrainee(test::Employee       *result,
                const test::Trainee&  trainee,
                float                 salary)
{
    int retCode = bdlat_SymbolicConverter::convert(result, trainee);

    result->salary() = salary;

    return retCode;
}
//..
//  The 'hireTrainee' function can be used as follows:
//..
void usageExample()
{
    test::Trainee trainee;

    trainee.name() = "Bob";
    trainee.dept() = "RnD";
    trainee.age()  = 24;

    test::Employee employee;

    int result = hireTrainee(&employee, trainee, 20000.00f);

    ASSERT(0         == result);
    ASSERT("Bob"     == employee.name());
    ASSERT("RnD"     == employee.dept());
    ASSERT(24        == employee.age());
    ASSERT(20000.00f == employee.salary());
}
//..
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
//  int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 16: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

        usageExample();
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING MIXED SEQUENCE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Mixed Sequence"
                          << "\n======================"
                          << endl;

        typedef test::MixedSequence Type;

        Type mX;  const Type& X = mX;

        mX.attribute1().makeSelection1();
        mX.attribute1().selection1() = 123;
        mX.attribute2().fromString("def");
        mX.attribute3().makeValue(843);
        mX.attribute4() = test::EnumerationA::VALUE2;
        mX.attribute5() = 927;
        mX.attribute6() = "test string";
        mX.attribute7().attribute1() = 102;
        mX.attribute7().attribute2() = "another test string";

        Type mY;  const Type& Y = mY;
        LOOP2_ASSERT(X, Y, X != Y);

        int ret = bdlat_SymbolicConverter::convert(&mY, X);
        LOOP_ASSERT(ret, 0     == ret);
        LOOP2_ASSERT(X, Y, X == Y);

        if (verbose) cout << "\nEnd of test." << bsl::endl;
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING TO NON-CUSTOMIZED FROM CUSTOMIZED
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting To Non-Customized From Customized"
                          << "\n========================================="
                          << endl;

        typedef bsl::string BaseType;

        const BaseType VALUE = "Hello!!";

        BaseType          a;
        test::CustomizedB b;
        b.fromString(VALUE);

        int ret = bdlat_SymbolicConverter::convert(&a, b);
        LOOP_ASSERT(ret, 0     == ret);
        LOOP_ASSERT(a,   VALUE == a);

        if (verbose) cout << "\nEnd of test." << bsl::endl;
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING TO CUSTOMIZED FROM NON-CUSTOMIZED
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting To Customized From Non-Customized"
                          << "\n========================================="
                          << endl;

        typedef bsl::string BaseType;

        const BaseType VALUE1 = "Hello";
        const BaseType VALUE2 = "World!!";

        if (verbose) cout << "\nSource fits into destination." << endl;
        {
            test::CustomizedA a;
            BaseType          b = VALUE1;

            LOOP_ASSERT(a.toString(), a.toString().empty());
            LOOP_ASSERT(b,            VALUE1 == b);

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(a.toString(), VALUE1 == a.toString());
        }

        if (verbose) cout << "\nSource does not fit into destination." << endl;
        {
            test::CustomizedA a;
            BaseType          b = VALUE2;

            LOOP_ASSERT(a.toString(), a.toString().empty());
            LOOP_ASSERT(b,            VALUE2 == b);

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 != ret);
        }

        if (verbose) cout << "\nEnd of test." << bsl::endl;
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING TO CUSTOMIZED FROM CUSTOMIZED
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting To Customized From Customized"
                          << "\n=====================================" << endl;

        typedef bsl::string BaseType;

        const BaseType VALUE1 = "Hello";
        const BaseType VALUE2 = "World!!";

        if (verbose) cout << "\nSource fits into destination." << endl;
        {
            test::CustomizedA a;
            test::CustomizedB b;
            b.fromString(VALUE1);

            LOOP_ASSERT(a.toString(), a.toString().empty());
            LOOP_ASSERT(b.toString(), VALUE1 == b.toString());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(a.toString(), VALUE1 == a.toString());
        }

        if (verbose) cout << "\nSource does not fit into destination." << endl;
        {
            test::CustomizedA a;
            test::CustomizedB b;
            b.fromString(VALUE2);

            LOOP_ASSERT(a.toString(), a.toString().empty());
            LOOP_ASSERT(b.toString(), VALUE2 == b.toString());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 != ret);
        }

        if (verbose) cout << "\nEnd of test." << bsl::endl;
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING TO NON-NULLABLE FROM NULLABLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting To Non-Nullable From Nullable"
                          << "\n=====================================" << endl;

        typedef int                            ValueType;
        typedef bdlb::NullableValue<ValueType> NullableType;

        const ValueType VALUE1 = 20;
        const ValueType VALUE2 = 34;

        if (verbose) cout << "\nRhs is null." << endl;
        {
            ValueType    a = VALUE1;
            NullableType b;

            ASSERT(b.isNull());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(a, ValueType() == a);
        }

        if (verbose) cout << "\nRhs is not null." << endl;
        {
            ValueType    a = VALUE1;
            NullableType b = VALUE2;

            ASSERT(!b.isNull());

            LOOP2_ASSERT(a, b.value(), a != b.value());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP2_ASSERT(a, b.value(), a == b.value());
        }

        if (verbose) cout << "\nEnd of test." << bsl::endl;
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING TO NULLABLE FROM NON-NULLABLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting To Nullable From Non-Nullable"
                          << "\n=====================================" << endl;

        typedef int                            ValueType;
        typedef bdlb::NullableValue<ValueType> NullableType;

        const ValueType VALUE1 = 20;
        const ValueType VALUE2 = 34;

        if (verbose) cout << "\nLhs is null." << endl;
        {
            NullableType a;
            ValueType    b = VALUE2;

            ASSERT(a.isNull());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            ASSERT(!a.isNull());
            LOOP_ASSERT(a.value(), VALUE2 == a.value());
        }

        if (verbose) cout << "\nLhs is not null." << endl;
        {
            NullableType a = VALUE1;
            ValueType    b = VALUE2;

            ASSERT(!a.isNull());

            LOOP2_ASSERT(a.value(), b, a.value() != b);

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            ASSERT(!a.isNull());
            LOOP2_ASSERT(a.value(), b, a.value() == b);
        }

        if (verbose) cout << "\nEnd of test." << bsl::endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING TO NULLABLE FROM NULLABLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting To Nullable From Nullable"
                          << "\n=================================" << endl;

        typedef int                            ValueType;
        typedef bdlb::NullableValue<ValueType> NullableType;

        const ValueType VALUE1 = 20;
        const ValueType VALUE2 = 34;

        if (verbose) cout << "\nLhs and Rhs are null." << endl;
        {
            NullableType a;
            NullableType b;

            ASSERT(a.isNull());
            ASSERT(b.isNull());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            ASSERT(a.isNull());
        }

        if (verbose) cout << "\nOnly Lhs is null." << endl;
        {
            NullableType a;
            NullableType b = VALUE2;

            ASSERT(a.isNull());
            ASSERT(!b.isNull());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            ASSERT(!a.isNull());
            LOOP_ASSERT(a.value(), VALUE2 == a.value());
        }

        if (verbose) cout << "\nOnly Rhs is null." << endl;
        {
            NullableType a = VALUE1;
            NullableType b;

            ASSERT(!a.isNull());
            ASSERT(b.isNull());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            ASSERT(a.isNull());
        }

        if (verbose) cout << "\nNeither Lhs nor Rhs is null." << endl;
        {
            NullableType a = VALUE1;
            NullableType b = VALUE2;

            ASSERT(!a.isNull());
            ASSERT(!b.isNull());

            LOOP2_ASSERT(a.value(), b.value(), a.value() != b.value());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            ASSERT(!a.isNull());
            LOOP2_ASSERT(a.value(), b.value(), a.value() == b.value());
        }

        if (verbose) cout << "\nEnd of test." << bsl::endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING TO ENUMERATION FROM STRING AND VICE VERSA
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting To Enumeration From String And "
                          << "Vice Versa"
                          << "\n======================================="
                          << "=========="
                          << endl;

        if (verbose) cout << "To Enumeration From String." << endl;
        {
            const bsl::string VALUE1 = "VALUE1";

            test::EnumerationA::Value a = test::EnumerationA::VALUE2;
            int ret = bdlat_SymbolicConverter::convert(&a, VALUE1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(a, test::EnumerationA::VALUE1 == a);
        }

        if (verbose) cout << "To String From Enumeration." << endl;
        {
            const bsl::string VALUE1 = "VALUE1";
            const bsl::string VALUE2 = "VALUE2";
            test::EnumerationA::Value a = test::EnumerationA::VALUE1;
            bsl::string value = VALUE2;

            int ret = bdlat_SymbolicConverter::convert(&value, a);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(value, VALUE1 == value);
        }

        if (verbose) cout << "\nEnd of test." << bsl::endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING TO ENUMERATION FROM NUMBER AND VICE VERSA
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting To Enumeration From Number And "
                          << "Vice Versa"
                          << "\n======================================="
                          << "=========="
                          << endl;

        if (verbose) cout << "To Enumeration From Number." << endl;
        {
            typedef char Number;

            const Number VALUE1 = 1;
            BSLMF_ASSERT(VALUE1 == test::EnumerationA::VALUE1);

            test::EnumerationA::Value a = test::EnumerationA::VALUE2;
            int ret = bdlat_SymbolicConverter::convert(&a, VALUE1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(a, test::EnumerationA::VALUE1 == a);
        }
        {
            typedef short Number;

            const Number VALUE1 = 1;
            BSLMF_ASSERT(VALUE1 == test::EnumerationA::VALUE1);

            test::EnumerationA::Value a = test::EnumerationA::VALUE2;
            int ret = bdlat_SymbolicConverter::convert(&a, VALUE1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(a, test::EnumerationA::VALUE1 == a);
        }
        {
            typedef int Number;

            const Number VALUE1 = 1;
            BSLMF_ASSERT(VALUE1 == test::EnumerationA::VALUE1);

            test::EnumerationA::Value a = test::EnumerationA::VALUE2;
            int ret = bdlat_SymbolicConverter::convert(&a, VALUE1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(a, test::EnumerationA::VALUE1 == a);
        }

        if (verbose) cout << "To Number From Enumeration." << endl;
        {
            typedef char Number;

            const Number VALUE2 = 2;
            BSLMF_ASSERT(VALUE2 != test::EnumerationA::VALUE1);

            test::EnumerationA::Value a = test::EnumerationA::VALUE1;
            Number number = VALUE2;

            int ret = bdlat_SymbolicConverter::convert(&number, a);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(number, static_cast<Number>(a) == number);
        }
        {
            typedef short Number;

            const Number VALUE2 = 2;
            BSLMF_ASSERT(VALUE2 != test::EnumerationA::VALUE1);

            test::EnumerationA::Value a = test::EnumerationA::VALUE1;
            Number number = VALUE2;

            int ret = bdlat_SymbolicConverter::convert(&number, a);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(number, static_cast<Number>(a) == number);
        }
        {
            typedef int Number;

            const Number VALUE2 = 2;
            BSLMF_ASSERT(VALUE2 != test::EnumerationA::VALUE1);

            test::EnumerationA::Value a = test::EnumerationA::VALUE1;
            Number number = VALUE2;

            int ret = bdlat_SymbolicConverter::convert(&number, a);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(number, static_cast<Number>(a) == number);
        }

        if (verbose) cout << "\nEnd of test." << bsl::endl;
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING TO ENUMERATION FROM ENUMERATION
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting To Enumeration From Enumeration"
                          << "\n======================================="
                          << endl;

        test::EnumerationA::Value a = test::EnumerationA::VALUE1;
        test::EnumerationB::Value b = test::EnumerationB::VALUE2;

        int ret = bdlat_SymbolicConverter::convert(&a, b);
        LOOP_ASSERT(ret, 0 == ret);
        LOOP_ASSERT(a, test::EnumerationA::VALUE2 == a);

        if (verbose) cout << "\nEnd of test." << bsl::endl;
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING TO ARRAY FROM ARRAY
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting To Array From Array"
                          << "\n===========================" << endl;

        typedef int                  Element;
        typedef bsl::vector<Element> Array;

        const Element ELEMENT1 = 1;
        const Element ELEMENT2 = 2;
        const Element ELEMENT3 = 3;

        if (verbose) cout << "\nLhs and Rhs are empty." << endl;
        {
            Array a;
            Array b;

            LOOP_ASSERT(a.size(), 0 == a.size());
            LOOP_ASSERT(b.size(), 0 == b.size());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(a.size(), 0 == a.size());
        }

        if (verbose) cout << "\nOnly Lhs is empty." << endl;
        {
            Array a;
            Array b;

            b.push_back(ELEMENT1);
            b.push_back(ELEMENT2);
            b.push_back(ELEMENT3);

            LOOP_ASSERT(a.size(), 0 == a.size());
            LOOP_ASSERT(b.size(), 3 == b.size());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret,      0        == ret);
            LOOP_ASSERT(a.size(), 3        == a.size());
            LOOP_ASSERT(a[0],     ELEMENT1 == a[0]);
            LOOP_ASSERT(a[1],     ELEMENT2 == a[1]);
            LOOP_ASSERT(a[2],     ELEMENT3 == a[2]);
        }

        if (verbose) cout << "\nOnly Rhs is empty." << endl;
        {
            Array a;
            Array b;

            a.push_back(ELEMENT1);
            a.push_back(ELEMENT2);
            a.push_back(ELEMENT3);

            LOOP_ASSERT(a.size(), 3 == a.size());
            LOOP_ASSERT(b.size(), 0 == b.size());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret,      0 == ret);
            LOOP_ASSERT(a.size(), 0 == a.size());
        }

        if (verbose) cout << "\nNeither Lhs nor Rhs is empty." << endl;
        {
            if (veryVerbose) cout << "\tBoth arrays have same number of "
                                  << "elements." << endl;

            Array a;
            Array b;

            a.push_back(ELEMENT1);
            a.push_back(ELEMENT2);
            a.push_back(ELEMENT3);

            b.push_back(ELEMENT2);
            b.push_back(ELEMENT3);
            b.push_back(ELEMENT1);

            LOOP_ASSERT(a.size(), 3 == a.size());
            LOOP_ASSERT(b.size(), 3 == b.size());

            LOOP2_ASSERT(a[0], b[0], a[0] != b[0]);
            LOOP2_ASSERT(a[1], b[1], a[1] != b[1]);
            LOOP2_ASSERT(a[2], b[2], a[2] != b[2]);

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret,      0 == ret);
            LOOP_ASSERT(a.size(), 3 == a.size());
            LOOP2_ASSERT(a[0], b[0], a[0] == b[0]);
            LOOP2_ASSERT(a[1], b[1], a[1] == b[1]);
            LOOP2_ASSERT(a[2], b[2], a[2] == b[2]);
        }
        {
            if (veryVerbose) cout << "\tLhs has more elements than Rhs."
                                  << endl;

            Array a;
            Array b;

            a.push_back(ELEMENT1);
            a.push_back(ELEMENT2);
            a.push_back(ELEMENT3);

            b.push_back(ELEMENT2);

            LOOP_ASSERT(a.size(), 3 == a.size());
            LOOP_ASSERT(b.size(), 1 == b.size());

            LOOP2_ASSERT(a[0], b[0], a[0] != b[0]);

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret,      0 == ret);
            LOOP_ASSERT(a.size(), 1 == a.size());
            LOOP2_ASSERT(a[0], b[0], a[0] == b[0]);
        }
        {
            if (veryVerbose) cout << "\tLhs has fewer elements than Rhs."
                                  << endl;

            Array a;
            Array b;

            a.push_back(ELEMENT1);

            b.push_back(ELEMENT2);
            b.push_back(ELEMENT3);
            b.push_back(ELEMENT1);

            LOOP_ASSERT(a.size(), 1 == a.size());
            LOOP_ASSERT(b.size(), 3 == b.size());

            LOOP2_ASSERT(a[0], b[0], a[0] != b[0]);

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret,      0 == ret);
            LOOP_ASSERT(a.size(), 3 == a.size());
            LOOP2_ASSERT(a[0], b[0], a[0] == b[0]);
            LOOP2_ASSERT(a[1], b[1], a[1] == b[1]);
            LOOP2_ASSERT(a[2], b[2], a[2] == b[2]);
        }

        if (verbose) cout << "\nEnd of test." << bsl::endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING TO CHOICE FROM CHOICE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting To Choice From Choice"
                          << "\n=============================" << endl;

        const int         SELECTION1 = 34;
        const bsl::string SELECTION2 = "Hello";

        if (verbose) cout << "\nLhs and Rhs have selections." << endl;
        {
            if (veryVerbose) cout << "\tLhs has selection1, "
                                  << "Rhs has selection2." << endl;

            test::ChoiceA a;
            test::ChoiceB b;

            a.makeSelection(test::ChoiceA::SELECTION_ID_SELECTION1);
            a.selection1() = SELECTION1;

            b.makeSelection(test::ChoiceB::SELECTION_ID_SELECTION2);
            b.selection2() = SELECTION2;

            LOOP_ASSERT(a.selectionId(),
                        test::ChoiceA::SELECTION_ID_SELECTION1
                                                           == a.selectionId());
            LOOP_ASSERT(b.selectionId(),
                        test::ChoiceB::SELECTION_ID_SELECTION2
                                                           == b.selectionId());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(a.selectionId(),
                        test::ChoiceA::SELECTION_ID_SELECTION2
                                                           == a.selectionId());
            LOOP_ASSERT(a.selection2(), SELECTION2 == a.selection2());
        }
        {
            if (veryVerbose) cout << "\tLhs has selection2, "
                                  << "Rhs has selection1." << endl;

            test::ChoiceA a;
            test::ChoiceB b;

            a.makeSelection(test::ChoiceA::SELECTION_ID_SELECTION2);
            a.selection2() = SELECTION2;

            b.makeSelection(test::ChoiceB::SELECTION_ID_SELECTION1);
            b.selection1() = SELECTION1;

            LOOP_ASSERT(a.selectionId(),
                        test::ChoiceA::SELECTION_ID_SELECTION2
                                                           == a.selectionId());
            LOOP_ASSERT(b.selectionId(),
                        test::ChoiceB::SELECTION_ID_SELECTION1
                                                           == b.selectionId());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(a.selectionId(),
                        test::ChoiceA::SELECTION_ID_SELECTION1
                                                           == a.selectionId());
            LOOP_ASSERT(a.selection1(), SELECTION1 == a.selection1());
        }
        {
            if (veryVerbose) cout << "\tLhs and Rhs have selection1." << endl;

            test::ChoiceA a;
            test::ChoiceB b;

            a.makeSelection(test::ChoiceA::SELECTION_ID_SELECTION1);
            a.selection1() = SELECTION1;

            b.makeSelection(test::ChoiceB::SELECTION_ID_SELECTION1);
            b.selection1() = SELECTION1 + 105;

            LOOP_ASSERT(a.selectionId(),
                        test::ChoiceA::SELECTION_ID_SELECTION1
                                                           == a.selectionId());
            LOOP_ASSERT(b.selectionId(),
                        test::ChoiceB::SELECTION_ID_SELECTION1
                                                           == b.selectionId());
            LOOP2_ASSERT(a.selection1(),   b.selection1(),
                         a.selection1() != b.selection1());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(a.selectionId(),
                        test::ChoiceA::SELECTION_ID_SELECTION1
                                                           == a.selectionId());
            LOOP_ASSERT(a.selection1(), b.selection1() == a.selection1());
        }

        if (verbose) cout << "\nOnly Lhs has selection." << endl;
        {
            test::ChoiceA a;
            test::ChoiceB b;

            a.makeSelection(test::ChoiceA::SELECTION_ID_SELECTION2);
            a.selection2() = SELECTION2;

            LOOP_ASSERT(a.selectionId(),
                        test::ChoiceA::SELECTION_ID_SELECTION2
                                                           == a.selectionId());
            LOOP_ASSERT(b.selectionId(),
                        test::ChoiceB::SELECTION_ID_UNDEFINED
                                                           == b.selectionId());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(a.selectionId(),
                        test::ChoiceA::SELECTION_ID_UNDEFINED
                                                           == a.selectionId());
        }

        if (verbose) cout << "\nOnly Rhs have selection." << endl;
        {
            test::ChoiceA a;
            test::ChoiceB b;

            b.makeSelection(test::ChoiceB::SELECTION_ID_SELECTION1);
            b.selection1() = SELECTION1;

            LOOP_ASSERT(a.selectionId(),
                        test::ChoiceA::SELECTION_ID_UNDEFINED
                                                           == a.selectionId());
            LOOP_ASSERT(b.selectionId(),
                        test::ChoiceB::SELECTION_ID_SELECTION1
                                                           == b.selectionId());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(a.selectionId(),
                        test::ChoiceA::SELECTION_ID_SELECTION1
                                                           == a.selectionId());
            LOOP_ASSERT(a.selection1(), SELECTION1 == a.selection1());
        }

        if (verbose) cout << "\nNeither Lhs nor Rhs has selection." << endl;
        {
            test::ChoiceA a;
            test::ChoiceB b;

            LOOP_ASSERT(a.selectionId(),
                        test::ChoiceA::SELECTION_ID_UNDEFINED
                                                           == a.selectionId());
            LOOP_ASSERT(b.selectionId(),
                        test::ChoiceB::SELECTION_ID_UNDEFINED
                                                           == b.selectionId());

            int ret = bdlat_SymbolicConverter::convert(&a, b);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(a.selectionId(),
                        test::ChoiceA::SELECTION_ID_UNDEFINED
                                                           == a.selectionId());
        }

        if (verbose) cout << "\nEnd of test." << bsl::endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING TO SEQUENCE FROM SEQUENCE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting To Sequence From Sequence"
                          << "\n=================================" << endl;

        const int         ATTRIBUTE1_A = 34;
        const bsl::string ATTRIBUTE2_A = "Hello";
        const int         ATTRIBUTE1_B = 103;
        const bsl::string ATTRIBUTE2_B = "World";

        test::SequenceA a;
        test::SequenceB b;

        a.attribute1() = ATTRIBUTE1_A;
        a.attribute2() = ATTRIBUTE2_A;
        b.attribute1() = ATTRIBUTE1_B;
        b.attribute2() = ATTRIBUTE2_B;

        LOOP_ASSERT(a.attribute1(), ATTRIBUTE1_B != a.attribute1());
        LOOP_ASSERT(a.attribute2(), ATTRIBUTE2_B != a.attribute2());

        int ret = bdlat_SymbolicConverter::convert(&a, b);
        LOOP_ASSERT(ret, 0 == ret);
        LOOP_ASSERT(a.attribute1(), ATTRIBUTE1_B == a.attribute1());
        LOOP_ASSERT(a.attribute2(), ATTRIBUTE2_B == a.attribute2());

        if (verbose) cout << "\nEnd of test." << bsl::endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TO SIMPLE FROM SIMPLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting To Simple From Simple"
                          << "\n=============================" << endl;

        if (verbose) cout << "\nTesting convertible types." << endl;
        {
            if (veryVerbose) cout << "\tTo int From int." << endl;
            {
                const int VALUEA = 1;
                const int VALUEB = 2;

                int a = VALUEA;
                int b = VALUEB;

                LOOP_ASSERT(a, VALUEB != a);

                int ret = bdlat_SymbolicConverter::convert(&a, b);

                LOOP_ASSERT(ret, 0      == ret);
                LOOP_ASSERT(a,   VALUEB == a);
            }

            if (veryVerbose) cout << "\tTo double From int." << endl;
            {
                const double VALUEA = 1.3;
                const int    VALUEB = 8;

                double a = VALUEA;
                int    b = VALUEB;

                LOOP_ASSERT(a, VALUEB != a);

                int ret = bdlat_SymbolicConverter::convert(&a, b);

                LOOP_ASSERT(ret, 0      == ret);
                LOOP_ASSERT(a,   VALUEB == a);
            }

            if (veryVerbose) cout << "\tTo string From string." << endl;
            {
                const bsl::string VALUEA = "Hello";
                const bsl::string VALUEB = "World";

                bsl::string a = VALUEA;
                bsl::string b = VALUEB;

                LOOP_ASSERT(a, VALUEB != a);

                int ret = bdlat_SymbolicConverter::convert(&a, b);

                LOOP_ASSERT(ret, 0      == ret);
                LOOP_ASSERT(a,   VALUEB == a);
            }
        }

        if (verbose) cout << "\nTesting non-convertible types." << endl;
        {
            if (veryVerbose) cout << "\tTo int From string." << endl;
            {
                const int         VALUEA = 1;
                const bsl::string VALUEB = "World";

                int         a = VALUEA;
                bsl::string b = VALUEB;

                int ret = bdlat_SymbolicConverter::convert(&a, b);

                LOOP_ASSERT(ret, 0 != ret);
            }
        }

        if (verbose) cout << "\nEnd of test." << bsl::endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n============= " << endl;

        bool boolValue = false;
        int  intValue  = 1;

        ASSERT(0 == bdlat_SymbolicConverter::convert(&boolValue, intValue));
        ASSERT(true == boolValue);

        {
            ConvertibleType a, b;
            a.d_value = 30;
            b.d_value = 100;
            ASSERT(0 == bdlat_SymbolicConverter::convert(&a, b));
            ASSERT(100 == a.d_value);
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
