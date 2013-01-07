// bdeat_choicefunctions.t.cpp                  -*-C++-*-

#include <bdeat_choicefunctions.h>

#include <bdeat_formattingmode.h>
#include <bdeat_selectioninfo.h>
#include <bdeat_typetraits.h>

#include <bslalg_typetraits.h>

#include <bsls_platformutil.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD doc
//-----------------------------------------------------------------------------
// [ 3] int lookupSelectionInfo(*info, object, *name, nameLength);
// [ 3] int lookupSelectionInfo(*info, object, id);
// [ 2] bdeat_SelectionInfo Obj::selectionInfo(const TYPE&, int);
// [ 2] const char *Obj::className(const TYPE&);
// [ 2] int Obj::numSelections(const TYPE&);
//-----------------------------------------------------------------------------
// [ 1] METHOD FORWARDING TEST
// [ 2] INFO ACCESS TEST
// [ 4] USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
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
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace Obj = bdeat_ChoiceFunctions;

//=============================================================================
//                           CLASSES FOR TESTING
//-----------------------------------------------------------------------------

static int globalFlag = 0;

namespace geom {

class Figure {

  public:
    // TYPES
    enum {
        NUM_SELECTIONS = 2 // the number of selections in this class
    };

    enum {
        SELECTION_INDEX_CIRCLE = 0,
            // index for 'Circle' selection
        SELECTION_INDEX_POLYGON = 1
            // index for 'Polygon' selection
    };

    enum {
        SELECTION_ID_UNDEFINED = 0,

        SELECTION_ID_CIRCLE = 1,
            // id for 'Circle' selection
        SELECTION_ID_POLYGON = 2
            // id for 'Polygon' selection
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Figure")

    static const double DEFAULT_X;
        // default value of 'X' selection
    static const double DEFAULT_Y;
        // default value of 'Y' selection

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];
        // selection info for each selection

  public:
    // TYPE TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Figure, bdeat_TypeTraitBasicChoice);

  private:
    double d_x; // X coordinate
    double d_y; // Y coordinate

  public:
    // CLASS METHODS
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
    Figure()
        // Create an instance having default values, when appropriate.
    : d_x(0)
    , d_y(0)
    {
    }

    Figure(const Figure& original)
        // Create an instance having the value of the specified
        // 'original' object.
    : d_x(original.d_x)
    , d_y(original.d_y)
    {
    }

    ~Figure()
       // Destroy this object.
    {
    }

    // MANIPULATORS
    Figure& operator=(const Figure& rhs)
        // Assign to this object the value of the specified 'rhs'
        // object.
    {
        if (this != &rhs) {
            d_x = rhs.d_x;
            d_y = rhs.d_y;
        }
        return *this;
    }

    int makeSelection(int selectionId)
    {
        globalFlag = 1;
        return globalFlag;
    }

    int makeSelection(const char *name, int nameLength)
    {
        globalFlag = 2;
        return globalFlag;
    }

    template<typename MANIPULATOR>
    int manipulateSelection(MANIPULATOR& m)
        // visits modifiable selection
    {
        globalFlag = 3;
        return globalFlag;
    }

    double& x()
         // Return a reference to the modifiable x coordinate
    {
        return d_x;
    }

    double& y()
         // Return a reference to the modifiable y coordinate
    {
        return d_y;
    }

    // ACCESSORS
    template<typename ACCESSOR>
    int accessSelection(ACCESSOR& a) const
        // visits non-modifiable selection
    {
        globalFlag = 4;
        return globalFlag;
    }

    int selectionId() const
    {
        globalFlag = 5;
        return globalFlag;
    }

    double x() const
    {
        return d_x;
    }
         // Return the x coordinate
    double y() const
    {
        return d_y;
    }
         // Return the y coordinate
};

const char Figure::CLASS_NAME[] = "Figure";

const bdeat_SelectionInfo Figure::SELECTION_INFO_ARRAY[] = {
    {SELECTION_ID_CIRCLE, "Circle", sizeof("Circle") -
1, "Circle"},
    {SELECTION_ID_POLYGON, "Polygon", sizeof("Polygon"
) - 1, "Polygon"}
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdeat_SelectionInfo *Figure::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 6: {
            if (0 == bsl::strncmp(name, "Circle", 6)) {
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CIRCLE];
            }
        } break;
        case 7: {
            if (0 == bsl::strncmp(name, "Polygon", 7)) {
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_POLYGON];
            }
        } break;
    }
    return 0;
}

const bdeat_SelectionInfo *Figure::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_CIRCLE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CIRCLE];
      case SELECTION_ID_POLYGON:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_POLYGON];
      default:
        return 0;
    }
}

}  // end namespace geom

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// Suppose you had a 'union' embedded inside a 'struct'.  The 'struct' also
// contains a 'd_selectionId' member that specifies which member of the 'union'
// is selected.  The default constructor of the 'struct' makes the selection
// undefined:
//..
#include <bdeat_choicefunctions.h>
#include <bdeat_selectioninfo.h>
#include <bsls_assert.h>
#include <bdeu_string.h>

namespace BloombergLP {

namespace mine {

struct MyChoice {
    // This struct represents a choice between a 'char' value, an 'int'
    // value, and a 'float' value.

    // CONSTANTS
    enum {
        UNDEFINED_SELECTION_ID = -1,
        CHAR_SELECTION_ID      = 0,
        INT_SELECTION_ID       = 1,
        FLOAT_SELECTION_ID     = 2
    };

    // DATA MEMBERS
    union {
        char  d_charValue;
        int   d_intValue;
        float d_floatValue;
    };
    int d_selectionId;

    // CREATORS
    MyChoice()
    : d_selectionId(UNDEFINED_SELECTION_ID)
    {
    }
};
//..
// We can now make 'MyChoice' expose "choice" behavior by implementing
// 'bdeat_ChoiceFunctions' for 'MyChoice'.  First, we should forward declare
// all the functions that we will implement inside the 'mine' namespace:
//..
    // MANIPULATORS
    int bdeat_choiceMakeSelection(MyChoice *object, int selectionId);
        // Set the value of the specified 'object' to be the default for
        // the selection indicated by the specified 'selectionId'.  Return
        // 0 on success, and non-zero value otherwise (i.e., the selection
        // is not found).

    int bdeat_choiceMakeSelection(MyChoice   *object,
                                  const char *selectionName,
                                  int         selectionNameLength);
        // Set the value of the specified 'object' to be the default for
        // the selection indicated by the specified 'selectionName' of the
        // specified 'selectionNameLength'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    template <typename MANIPULATOR>
    int bdeat_choiceManipulateSelection(MyChoice     *object,
                                        MANIPULATOR&  manipulator);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) selection of the specified 'object', supplying
        // 'manipulator' with the corresponding selection information
        // structure.  Return -1 if the selection is undefined, and the
        // value returned from the invocation of 'manipulator' otherwise.

    // ACCESSORS
    template <typename ACCESSOR>
    int bdeat_choiceAccessSelection(const MyChoice& object,
                                    ACCESSOR&       accessor);
        // Invoke the specified 'accessor' on the (non-modifiable)
        // selection of the specified 'object', supplying 'accessor' with
        // the corresponding selection information structure.  Return -1 if
        // the selection is undefined, and the value returned from the
        // invocation of 'accessor' otherwise.

    int bdeat_choiceSelectionId(const MyChoice& object);
        // Return the id of the current selection if the selection is
        // defined, and 0 otherwise.

}  // close namespace mine
//..
// Next, we provide the definitions for each of these functions:
//..
// MANIPULATORS

inline
int mine::bdeat_choiceMakeSelection(MyChoice *object,
                                    int       selectionId)
{
    enum { SUCCESS = 0, NOT_FOUND = -1 };

    switch (selectionId) {
      case MyChoice::CHAR_SELECTION_ID: {
        object->d_selectionId = selectionId;
        object->d_charValue   = 0;

        return SUCCESS;
      }
      case MyChoice::INT_SELECTION_ID: {
        object->d_selectionId = selectionId;
        object->d_intValue    = 0;

        return SUCCESS;
      }
      case MyChoice::FLOAT_SELECTION_ID: {
        object->d_selectionId = selectionId;
        object->d_floatValue  = 0;

        return SUCCESS;
      }
      case MyChoice::UNDEFINED_SELECTION_ID: {
        object->d_selectionId = selectionId;

        return SUCCESS;
      }
      default: {
        return NOT_FOUND;
      }
    }
}

inline
int mine::bdeat_choiceMakeSelection(MyChoice   *object,
                                    const char *selectionName,
                                    int         selectionNameLength)
{
    enum { NOT_FOUND = -1 };

    if (bdeu_String::areEqualCaseless("charValue",
                                      selectionName,
                                      selectionNameLength)) {
        return bdeat_choiceMakeSelection(object, MyChoice::CHAR_SELECTION_ID);
    }

    if (bdeu_String::areEqualCaseless("intValue",
                                      selectionName,
                                      selectionNameLength)) {
        return bdeat_choiceMakeSelection(object, MyChoice::INT_SELECTION_ID);
    }

    if (bdeu_String::areEqualCaseless("floatValue",
                                      selectionName,
                                      selectionNameLength)) {
        return bdeat_choiceMakeSelection(object, MyChoice::FLOAT_SELECTION_ID);
    }

    return NOT_FOUND;
}
//..
// For the 'manipulateSelection' and 'accessSelection' functions, we need to
// create a temporary 'bdeat_SelectionInfo' object and pass it along when
// invoking the manipulator or accessor.  See the 'bdeat_selectioninfo'
// component-level documentation for more information.  The implementation of
// the remaining functions are as follows:
//..
template <typename MANIPULATOR>
int mine::bdeat_choiceManipulateSelection(MyChoice     *object,
                                          MANIPULATOR&  manipulator)
{
    switch (object->d_selectionId) {
      case MyChoice::CHAR_SELECTION_ID: {
        bdeat_SelectionInfo info;

        info.annotation()     = "Char Selection";
        info.formattingMode() = bdeat_FormattingMode::BDEAT_DEFAULT;
        info.id()             = MyChoice::CHAR_SELECTION_ID;
        info.name()           = "charValue";
        info.nameLength()     = 9;

        return manipulator(&object->d_charValue, info);
      }
      case MyChoice::INT_SELECTION_ID: {
        bdeat_SelectionInfo info;

        info.annotation()     = "Int Selection";
        info.formattingMode() = bdeat_FormattingMode::BDEAT_DEFAULT;
        info.id()             = MyChoice::INT_SELECTION_ID;
        info.name()           = "intValue";
        info.nameLength()     = 8;

        return manipulator(&object->d_intValue, info);
      }
      case MyChoice::FLOAT_SELECTION_ID: {
        bdeat_SelectionInfo info;

        info.annotation()     = "Float Selection";
        info.formattingMode() = bdeat_FormattingMode::BDEAT_DEFAULT;
        info.id()             = MyChoice::FLOAT_SELECTION_ID;
        info.name()           = "floatValue";
        info.nameLength()     = 10;

        return manipulator(&object->d_floatValue, info);
      }
      default:
        BSLS_ASSERT_SAFE(!"Invalid selection!");
    }
    return 0;
}

// ACCESSORS

template <typename ACCESSOR>
int mine::bdeat_choiceAccessSelection(const MyChoice& object,
                                      ACCESSOR&       accessor)
{
    switch (object.d_selectionId) {
      case MyChoice::CHAR_SELECTION_ID: {
        bdeat_SelectionInfo info;

        info.annotation()     = "Char Selection";
        info.formattingMode() = bdeat_FormattingMode::BDEAT_DEFAULT;
        info.id()             = MyChoice::CHAR_SELECTION_ID;
        info.name()           = "charValue";
        info.nameLength()     = 9;

        return accessor(object.d_charValue, info);
      }
      case MyChoice::INT_SELECTION_ID: {
        bdeat_SelectionInfo info;

        info.annotation()     = "Int Selection";
        info.formattingMode() = bdeat_FormattingMode::BDEAT_DEFAULT;
        info.id()             = MyChoice::INT_SELECTION_ID;
        info.name()           = "intValue";
        info.nameLength()     = 8;

        return accessor(object.d_intValue, info);
      }
      case MyChoice::FLOAT_SELECTION_ID: {
        bdeat_SelectionInfo info;

        info.annotation()     = "Float Selection";
        info.formattingMode() = bdeat_FormattingMode::BDEAT_DEFAULT;
        info.id()             = MyChoice::FLOAT_SELECTION_ID;
        info.name()           = "floatValue";
        info.nameLength()     = 10;

        return accessor(object.d_floatValue, info);
      }
      default:
        BSLS_ASSERT_SAFE(!"Invalid selection!");
    }
    return 0;
}

inline
int mine::bdeat_choiceSelectionId(const MyChoice& object)
{
    return object.d_selectionId;
}
//..
// Finally, we need to specialize the 'IsChoice' meta-function in the
// 'bdeat_ChoiceFunctions' namespace for the 'mine::MyChoice' type.  This
// makes the 'bdeat' infrastructure recognize 'mine::MyChoice' as a choice
// abstraction:
//..
namespace bdeat_ChoiceFunctions {

    template <>
    struct IsChoice<mine::MyChoice> {
        enum { VALUE = 1 };
    };

} // close namespace 'bdeat_ChoiceFunctions'
} // close namespace 'BloombergLP'
//..
// The 'bdeat' infrastructure (and any component that uses this infrastructure)
// will now recognize 'MyChoice' as a "choice" type.  For example, suppose we
// have the following XML data:
//..
//  <?xml version='1.0' encoding='UTF-8' ?>
//  <MyChoice>
//      <intValue>321</intValue>
//  </MyChoice>
//..
// Using the 'baexml_decoder' component, we can load this XML data into a
// 'MyChoice' object:
//..
//  #include <baexml_decoder.h>
//
//  void usageExample(bsl::istream& inputData)
//  {
//    using namespace BloombergLP;
//
//    MyChoice object;
//
//    assert(MyChoice::UNDEFINED_SELECTION_ID == object.d_selectionId);
//
//    baexml_DecoderOptions options;
//    baexml_MiniReader     reader;
//    baexml_ErrorInfo      errInfo;
//
//    baexml_Decoder decoder(&options, &reader, &errInfo);
//    int result = decoder.decode(inputData, &object);
//
//    assert(0                           == result);
//    assert(MyChoice::INT_SELECTION_ID  == object.d_selectionId);
//    assert(321                         == object.d_intValue);
//  }
//..
// Note that the 'bdeat' framework can be used for functionality other than
// encoding/decoding into XML.  When 'mine::MyChoice' is plugged into the
// framework, then it will be automatically usable within the framework.  For
// example, the following snippets of code will print out the selection value
// of a choice object:
//..
struct PrintSelection {
    // Print each visited object to the bound 'd_stream_p' object.

    // DATA MEMBERS
    bsl::ostream *d_stream_p;

    template <typename TYPE, typename INFO>
    int operator()(const TYPE& object, const INFO& info)
    {
      (*d_stream_p) << info.name() << ": " << object << bsl::endl;
      return 0;
    }
};

template <typename TYPE>
void printChoiceSelection(bsl::ostream& stream, const TYPE& object)
{
    using namespace BloombergLP;

    PrintSelection accessor;
    accessor.d_stream_p = &stream;

    bdeat_choiceAccessSelection(object, accessor);
}
//..
// Now we have a generic function that takes an output stream and a choice
// object, and prints out each attribute with its name and value.  We can use
// this generic function as follows:
//..
void printMyChoice(bsl::ostream& stream)
{
    using namespace BloombergLP;

    mine::MyChoice object;

    object.d_selectionId = mine::MyChoice::INT_SELECTION_ID;
    object.d_intValue    = 321;

    printChoiceSelection(stream, object);
}
//..
// The function above will print the following to provided stream:
//..
//  intValue: 321
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   That the usage example compiles and runs as expected.
        //
        // Plan:
        //   Copy-paste the example and change the assert into ASSERT.
        //
        // Testing:
        //   Usage Example
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
            << "\n=====================" << endl;

        bsl::stringstream ss;
        printMyChoice(ss);

        if (veryVerbose) {
            P(ss.str());
        }

        LOOP_ASSERT(ss.str(), "intValue: 321\n" == ss.str());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING META-FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   struct IsChoice
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting meta-functions"
                          << "\n======================" << endl;

        ASSERT(0 == Obj::IsChoice<int>::VALUE);
        ASSERT(1 == Obj::IsChoice<geom::Figure>::VALUE);
        ASSERT(1 == Obj::IsChoice<mine::MyChoice>::VALUE);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // METHOD FORWARDING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHOD FORWARDING TEST" << endl
                          << "======================" << endl;

        geom::Figure mF;  const geom::Figure& F = mF;
        int        dummySelectionId         = 1;
        const char dummySelectionName[]     = "dummy";
        int        dummySelectionNameLength = 5;
        int        dummyVisitor             = 0;

        BSLMF_ASSERT(bdeat_IsBasicChoice<geom::Figure>::value);

        globalFlag = 0;
        ASSERT(1 == bdeat_ChoiceFunctions::makeSelection(&mF,
                                                         dummySelectionId));
        ASSERT(1 == globalFlag);

        globalFlag = 0;
        ASSERT(2 == bdeat_ChoiceFunctions::makeSelection(
                                                    &mF,
                                                    dummySelectionName,
                                                    dummySelectionNameLength));
        ASSERT(2 == globalFlag);

        globalFlag = 0;
        ASSERT(3 == bdeat_ChoiceFunctions::manipulateSelection(&mF,
                                                               dummyVisitor));
        ASSERT(3 == globalFlag);

        globalFlag = 0;
        ASSERT(4 == bdeat_ChoiceFunctions::accessSelection(F, dummyVisitor));
        ASSERT(4 == globalFlag);

        globalFlag = 0;
        ASSERT(5 == bdeat_ChoiceFunctions::selectionId(F));
        ASSERT(5 == globalFlag);

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
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
