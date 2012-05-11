// bdeat_typename.t.cpp    -*-C++-*-

#include <bdeat_typename.h>

#include <bdet_date.h>
#include <bdet_datetz.h>
#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdet_time.h>
#include <bdet_timetz.h>
#include <bsl_string.h>

#include <bsl_cctype.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bslfwd_bslma_allocator.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::atoi;
using bsl::flush;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//-----------------------------------------------------------------------------
//
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//-----------------------------------------------------------------------------
//

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
//                  USAGE EXAMPLE
//-----------------------------------------------------------------------------

#include <bdeat_typename.h>
#include <bdet_date.h>
#include <bdet_datetz.h>
#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdet_time.h>
#include <bdet_timetz.h>
#include <bsl_string.h>

// We begin by creating abbreviations for formatting modes and by declaring
// objects of a number of types:
//..
    int usageExample1() {

        static const int DEFAULT = bdeat_FormattingMode::BDEAT_DEFAULT;
        static const int DEC     = bdeat_FormattingMode::BDEAT_DEC;
//        static const int HEX     = bdeat_FormattingMode::BDEAT_HEX;
//        static const int BASE64  = bdeat_FormattingMode::BDEAT_BASE64;
        static const int TEXT    = bdeat_FormattingMode::BDEAT_TEXT;

        short                    theShort;
        unsigned                 theUint;
        float                    theFloat;
        const char              *theCharPtr;
        bsl::string              theString;

        bdet_Date                theDate;
        bdet_DatetimeTz          theDatetime;
        bsl::vector<char>        theCharVector;
        bsl::vector<bsl::string> theStrVector;
//..
// None of these types are generated types with metadata, so 'className' will
// return a null pointer for each of them:
//..
        ASSERT(0 == bdeat_TypeName::className(theShort));
        ASSERT(0 == bdeat_TypeName::className(theUint));
        ASSERT(0 == bdeat_TypeName::className(theFloat));
        ASSERT(0 == bdeat_TypeName::className(theCharPtr));
        ASSERT(0 == bdeat_TypeName::className(theString));

        ASSERT(0 == bdeat_TypeName::className(theDate));
        ASSERT(0 == bdeat_TypeName::className(theDatetime));
        ASSERT(0 == bdeat_TypeName::className(theCharVector));
        ASSERT(0 == bdeat_TypeName::className(theStrVector));
//..
// The 'name' function will never return a null pointer.  For each of the
// fundamental and vocabulary types, it returns the known type name.  For
// vector types, it returns the appropriate "vector<X>" string:
//..
        ASSERT(0 == bsl::strcmp("short", bdeat_TypeName::name(theShort)));
        ASSERT(0 == bsl::strcmp("unsigned int",
                                bdeat_TypeName::name(theUint)));
        ASSERT(0 == bsl::strcmp("float", bdeat_TypeName::name(theFloat)));
        ASSERT(0 == bsl::strcmp("const char*",
                                bdeat_TypeName::name(theCharPtr)));

        ASSERT(0 == bsl::strcmp("string", bdeat_TypeName::name(theString)));
        ASSERT(0 == bsl::strcmp("bdet_Date", bdeat_TypeName::name(theDate)));
        ASSERT(0 == bsl::strcmp("bdet_DatetimeTz",
                                bdeat_TypeName::name(theDatetime)));
        ASSERT(0 == bsl::strcmp("vector<char>",
                                bdeat_TypeName::name(theCharVector)));
        ASSERT(0 == bsl::strcmp("vector<string>",
                                bdeat_TypeName::name(theStrVector)));
//..
// Each of the above types except 'vector<string>' has one or more
// corresponding XSD types.  The XSD type is affected by a formatting mode so
// that, for example, a 'vector<char>' can be represented as a text string
// (formatting mode 'TEXT') or as a sequence of binary bytes (formatting mode
// 'HEX' or 'BASE64').
//..
        ASSERT(0 == bsl::strcmp("short",
                                bdeat_TypeName::xsdName(theShort, DEFAULT)));
        ASSERT(0 == bsl::strcmp("unsignedInt",
                                bdeat_TypeName::xsdName(theUint, DEFAULT)));
        ASSERT(0 == bsl::strcmp("float",
                                bdeat_TypeName::xsdName(theFloat, DEFAULT)));
        ASSERT(0 == bsl::strcmp("decimal",
                                bdeat_TypeName::xsdName(theFloat, DEC)));
        ASSERT(0 == bsl::strcmp("base64Binary",
                             bdeat_TypeName::xsdName(theCharVector, DEFAULT)));
        ASSERT(0 == bsl::strcmp("string",
                                bdeat_TypeName::xsdName(theCharVector, TEXT)));
//..
// For types that have not corresponding XSD type, 'xsdName' returns
// "anyType", regardless of formatting mode:
//..
        ASSERT(0 == bsl::strcmp("anyType",
                              bdeat_TypeName::xsdName(theStrVector, DEFAULT)));

        return 0;
    }
//..
// If we create our own class,
//..
    namespace MyNamespace {

        class MyClass {
            //...
        };
//..
// Then we can assign it a printable name by overloading the
// 'bdeat_TypeName_className' function in the class's namespace:
//..
        const char *bdeat_TypeName_className(const MyClass&) {
            return "MyClass";
        }

    } // Close MyNamespace
//..
// Note that 'bdeat_TypeName_className' must return a string that is
// valid and does not change for remaining duration the program.  The
// overloaded 'bdeat_TypeName_className' function is automatically used for
// 'name' and 'xsdName', as well as for 'className':
//..
    int usageExample2()
    {
        static const int DEFAULT = bdeat_FormattingMode::BDEAT_DEFAULT;

        MyNamespace::MyClass myClassObj;

        ASSERT(0 == bsl::strcmp("MyClass",
                                bdeat_TypeName::className(myClassObj)));
        ASSERT(0 == bsl::strcmp("MyClass", bdeat_TypeName::name(myClassObj)));
        ASSERT(0 == bsl::strcmp("MyClass",
                                bdeat_TypeName::xsdName(myClassObj, DEFAULT)));

        return 0;
    }
//..

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdeat_TypeName Obj;

bool streq(const char* s1, const char* s2)
    // Return true if the specified strings 's1' and 's2' are equal and false
    // otherwise.  Returns 'true' if both 's1' and 's2' are null pointers.
{
    if (s1 == s2) {
        return true;
    }
    else if (0 == s1 || 0 == s2) {
        return false;
    }
    else {
        return 0 == bsl::strcmp(s1, s2);
    }
}

#define CLASSNAME_TST(Type, exp) {                                            \
        Type object;                                                          \
        const char *expected = exp ? exp : "<null>";                          \
        const char *result = Obj::className(object);                          \
        if (0 == result) result = "<null>";                                   \
        LOOP2_ASSERT(result, expected, streq(result, expected));              \
    }

#define NAME_TST(Type, exp) {                                                 \
        Type object;                                                          \
        const char *expected = exp ? exp : "<null>";                          \
        const char *result = Obj::name(object);                               \
        if (0 == result) result = "<null>";                                   \
        LOOP2_ASSERT(result, expected, streq(result, expected));              \
    }

#define XSDNAME_TST(Type, fmt, exp) {                                         \
        Type object;                                                          \
        const char *expected = exp ? exp : "<null>";                          \
        const char *result = Obj::xsdName(object, fmt);                       \
        if (0 == result) result = "<null>";                                   \
        LOOP2_ASSERT(result, expected, streq(result, expected));              \
    }

// test_mychoice.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYCHOICE
#define INCLUDED_TEST_MYCHOICE

//@PURPOSE:
//  TBD: provide purpose
//
//@CLASSES: MyChoice
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TBD: provide annotation

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
            // TBD: provide annotation
        bsls_ObjectBuffer< bsl::string > d_selection2;
            // TBD: provide annotation
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
// *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_mychoice.cpp  -*-C++-*-

#include <bdeat_formattingmode.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>

#include <bsl_cctype.h>
#include <bsl_iostream.h>
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
        "Selection1",              // name
        sizeof("Selection1") - 1,  // name length
        "TBD: provide annotation", // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        SELECTION_ID_SELECTION2,
        "Selection2",              // name
        sizeof("Selection2") - 1,  // name length
        "TBD: provide annotation", // annotation
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
// *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_myenumeration.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYENUMERATION
#define INCLUDED_TEST_MYENUMERATION

//@PURPOSE:
//  TBD: provide purpose
//
//@CLASSES: MyEnumeration
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TBD: provide annotation

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
// *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_myenumeration.cpp  -*-C++-*-

#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bsls_assert.h>

#include <bsl_cctype.h>
#include <bsl_iostream.h>

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
        "VALUE1",                 // name
        sizeof("VALUE1") - 1,     // name length
        "TBD: provide annotation" // annotation
    },
    {
        MyEnumeration::VALUE2,
        "VALUE2",                 // name
        sizeof("VALUE2") - 1,     // name length
        "TBD: provide annotation" // annotation
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
            if (bsl::toupper(string[0])=='V'
             && bsl::toupper(string[1])=='A'
             && bsl::toupper(string[2])=='L'
             && bsl::toupper(string[3])=='U'
             && bsl::toupper(string[4])=='E') {
                switch(bsl::toupper(string[5])) {
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
// *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_mysequence.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSEQUENCE
#define INCLUDED_TEST_MYSEQUENCE

//@PURPOSE:
//  TBD: provide purpose
//
//@CLASSES: MySequence
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TBD: provide annotation

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
// *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_mysequence.cpp  -*-C++-*-

#include <bdeat_formattingmode.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>

#include <bsl_cctype.h>
#include <bsl_iostream.h>
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
        "Attribute1",              // name
        sizeof("Attribute1") - 1,  // name length
        "TBD: provide annotation", // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",              // name
        sizeof("Attribute2") - 1,  // name length
        "TBD: provide annotation", // annotation
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
// *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_mycustomizedtype.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYCUSTOMIZEDTYPE
#define INCLUDED_TEST_MYCUSTOMIZEDTYPE

//@PURPOSE:
//  TBD: provide purpose
//
//@CLASSES: MyCustomizedType
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TBD: provide annotation

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

class MyCustomizedType {

  private:
    // PRIVATE DATA MEMBERS
    bsl::string d_value;  // stored value

    // FRIENDS
    friend bool operator==(const MyCustomizedType& lhs,
                           const MyCustomizedType& rhs);
    friend bool operator!=(const MyCustomizedType& lhs,
                           const MyCustomizedType& rhs);

  public:
    // TYPES
    typedef bsl::string BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MyCustomizedType")

    // CREATORS
    explicit MyCustomizedType(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MyCustomizedType' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    MyCustomizedType(const MyCustomizedType& original,
                     bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MyCustomizedType' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit MyCustomizedType(const bsl::string& value,
                              bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MyCustomizedType' having the specified
        // 'value'.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~MyCustomizedType();
        // Destroy this object.

    // MANIPULATORS
    MyCustomizedType& operator=(const MyCustomizedType& rhs);
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
bool operator==(const MyCustomizedType& lhs, const MyCustomizedType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MyCustomizedType& lhs, const MyCustomizedType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MyCustomizedType& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CREATORS

inline
MyCustomizedType::MyCustomizedType(bslma_Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
MyCustomizedType::MyCustomizedType(const MyCustomizedType& original,
                                   bslma_Allocator *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
MyCustomizedType::MyCustomizedType(const bsl::string& value,
                                   bslma_Allocator *basicAllocator)
: d_value(value, basicAllocator)
{
}

inline
MyCustomizedType::~MyCustomizedType()
{
}

// MANIPULATORS

inline
MyCustomizedType& MyCustomizedType::operator=(const MyCustomizedType& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

template <class STREAM>
STREAM& MyCustomizedType::bdexStreamIn(STREAM& stream, int version)
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
void MyCustomizedType::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_value);
}

inline
int MyCustomizedType::fromString(const bsl::string& value)
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
STREAM& MyCustomizedType::bdexStreamOut(STREAM& stream, int version) const
{
    streamOut(stream, d_value, version);
}

inline
int MyCustomizedType::maxSupportedBdexVersion() const
{
    return bdex_VersionFunctions::maxSupportedVersion(d_value);
}

inline
bsl::ostream& MyCustomizedType::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdeu_PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const bsl::string& MyCustomizedType::toString() const
{
    return d_value;
}

}  // close namespace test;

// TRAITS

BDEAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(test::MyCustomizedType)

// FREE OPERATORS

inline
bool test::operator==(const test::MyCustomizedType& lhs,
                                 const test::MyCustomizedType& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool test::operator!=(const test::MyCustomizedType& lhs,
                                 const test::MyCustomizedType& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& test::operator<<(bsl::ostream& stream,
                                          const test::MyCustomizedType& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_mycustomizedtype.cpp  -*-C++-*-

#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MyCustomizedType::CLASS_NAME[] = "MyCustomizedType";
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
// *End-of-file Block removed.*
// ----------------------------------------------------------------------------

class OtherType { int a; double b; };

namespace BloombergLP {
namespace test {

class MyClassWith92ByteName {
    // This class overloads 'bdeat_TypeName_className' to produce a name
    // that's 92 bytes long.  That makes
    // 'bdeat_TypeName::className(vector<MyClassWith93ByteName>)' return a
    // 100-byte name and
    // 'bdeat_TypeName::className(vector<vector<MyClassWith93ByteName> >)'
    // return a name truncated truncated to 100 characters.
};

#define STR_92BYTENAME "MyClassWith92ByteName_" \
    "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ"

const char* bdeat_TypeName_className(const MyClassWith92ByteName&)
{
    ASSERT(92 == sizeof(STR_92BYTENAME) - 1);
    return STR_92BYTENAME;
}

class MyIntWrapper {
    // Wrapper around int.  XSD treats it as INT.
    int d_value;
  public:
    MyIntWrapper(int v = 0) : d_value(v) { }
    void setValue(int v) { d_value = v; }
    int value() const { return d_value; }
};

const char* bdeat_TypeName_name(const MyIntWrapper& object)
{
    return "MyIntWrapper";
}

const char* bdeat_TypeName_xsdName(const MyIntWrapper& object, int format)
{
    return "integer";
}

}  // close namespace test;
}  // close namespace BloombergLP;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace test;
    using namespace MyNamespace;

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
//     int veryVerbose = argc > 3;
//     int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING USAGE EXAMPLE"
                          << "\n=====================" << endl;

        usageExample1();
        usageExample2();

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'xsdName'
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'xsdName'"
                          << "\n=================" << endl;

        static const int DEFAULT = bdeat_FormattingMode::BDEAT_DEFAULT;
        static const int DEC     = bdeat_FormattingMode::BDEAT_DEC;
        static const int HEX     = bdeat_FormattingMode::BDEAT_HEX;
        static const int BASE64  = bdeat_FormattingMode::BDEAT_BASE64;
        static const int TEXT    = bdeat_FormattingMode::BDEAT_TEXT;

        XSDNAME_TST(bool                     , DEFAULT, "boolean"          );
        XSDNAME_TST(bool                     , DEC    , "boolean"          );
        XSDNAME_TST(bool                     , TEXT   , "boolean"          );
        XSDNAME_TST(char                     , DEFAULT, "byte"             );
        XSDNAME_TST(char                     , DEC    , "byte"             );
        XSDNAME_TST(char                     , TEXT   , "string"           );
        XSDNAME_TST(signed char              , DEFAULT, "byte"             );
        XSDNAME_TST(signed char              , DEC    , "byte"             );
        XSDNAME_TST(signed char              , TEXT   , "string"           );
        XSDNAME_TST(unsigned char            , DEFAULT, "unsignedByte"     );
        XSDNAME_TST(unsigned char            , DEC    , "unsignedByte"     );
        XSDNAME_TST(unsigned char            , TEXT   , "string"           );
        XSDNAME_TST(short                    , DEFAULT, "short"            );
        XSDNAME_TST(short                    , DEC    , "short"            );
        XSDNAME_TST(short                    , TEXT   , "string"           );
        XSDNAME_TST(unsigned short           , DEFAULT, "unsignedShort"    );
        XSDNAME_TST(unsigned short           , DEC    , "unsignedShort"    );
        XSDNAME_TST(int                      , DEFAULT, "int"              );
        XSDNAME_TST(int                      , DEC    , "int"              );
        XSDNAME_TST(unsigned int             , DEFAULT, "unsignedInt"      );
        XSDNAME_TST(unsigned int             , DEC    , "unsignedInt"      );
        XSDNAME_TST(long                     , DEFAULT, "int"              );
        XSDNAME_TST(long                     , DEC    , "int"              );
        XSDNAME_TST(unsigned long            , DEFAULT, "unsignedInt"      );
        XSDNAME_TST(unsigned long            , DEC    , "unsignedInt"      );
        XSDNAME_TST(bsls_PlatformUtil::Int64 , DEFAULT, "long"             );
        XSDNAME_TST(bsls_PlatformUtil::Int64 , DEC    , "long"             );
        XSDNAME_TST(bsls_PlatformUtil::Uint64, DEFAULT, "unsignedLong"     );
        XSDNAME_TST(bsls_PlatformUtil::Uint64, DEC    , "unsignedLong"     );
        XSDNAME_TST(float                    , DEFAULT, "float"            );
        XSDNAME_TST(float                    , DEC    , "decimal"          );
        XSDNAME_TST(double                   , DEFAULT, "double"           );
        XSDNAME_TST(double                   , DEC    , "decimal"          );
        XSDNAME_TST(bsl::string              , DEFAULT, "string"           );
        XSDNAME_TST(bsl::string              , TEXT   , "string"           );
        XSDNAME_TST(bsl::string              , BASE64 , "base64Binary"     );
        XSDNAME_TST(bsl::string              , HEX    , "hexBinary"        );
        XSDNAME_TST(bdet_Date                , DEFAULT, "date"             );
        XSDNAME_TST(bdet_DateTz              , DEFAULT, "date"             );
        XSDNAME_TST(bdet_Datetime            , DEFAULT, "dateTime"         );
        XSDNAME_TST(bdet_DatetimeTz          , DEFAULT, "dateTime"         );
        XSDNAME_TST(bdet_Time                , DEFAULT, "time"             );
        XSDNAME_TST(bdet_TimeTz              , DEFAULT, "time"             );
        XSDNAME_TST(bsl::vector<char>        , DEFAULT, "base64Binary"     );
        XSDNAME_TST(bsl::vector<char>        , BASE64 , "base64Binary"     );
        XSDNAME_TST(bsl::vector<char>        , TEXT   , "string"           );
        XSDNAME_TST(bsl::vector<char>        , HEX    , "hexBinary"        );
        XSDNAME_TST(bsl::vector<short>       , TEXT   , "string"           );
        XSDNAME_TST(MyChoice                 , DEFAULT, "MyChoice"         );
        XSDNAME_TST(MySequence               , DEFAULT, "MySequence"       );
        XSDNAME_TST(MyEnumeration::Value     , DEFAULT, "MyEnumeration"    );
        XSDNAME_TST(MyCustomizedType         , DEFAULT, "MyCustomizedType" );
        XSDNAME_TST(OtherType                , DEFAULT, "anyType"          );
        XSDNAME_TST(MyClassWith92ByteName    , DEFAULT, STR_92BYTENAME     );
        XSDNAME_TST(MyClass                  , DEFAULT, "MyClass"          );
        XSDNAME_TST(MyIntWrapper             , DEFAULT, "integer"          );

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'name'
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'name'"
                          << "\n==============" << endl;

        NAME_TST(bool                           , "bool"                     );
        NAME_TST(char                           , "char"                     );
        NAME_TST(signed char                    , "signed char"              );
        NAME_TST(unsigned char                  , "unsigned char"            );
        NAME_TST(short                          , "short"                    );
        NAME_TST(unsigned short                 , "unsigned short"           );
        NAME_TST(int                            , "int"                      );
        NAME_TST(unsigned int                   , "unsigned int"             );
        NAME_TST(long                           , "long"                     );
        NAME_TST(unsigned long                  , "unsigned long"            );
        NAME_TST(bsls_PlatformUtil::Int64       , "Int64"                    );
        NAME_TST(bsls_PlatformUtil::Uint64      , "Uint64"                   );
        NAME_TST(float                          , "float"                    );
        NAME_TST(double                         , "double"                   );
        NAME_TST(bsl::string                    , "string"                   );
        NAME_TST(const char*                    , "const char*"              );
        NAME_TST(const signed char*             , "const signed char*"       );
        NAME_TST(const unsigned char*           , "const unsigned char*"     );
        NAME_TST(bdet_Date                      , "bdet_Date"                );
        NAME_TST(bdet_DateTz                    , "bdet_DateTz"              );
        NAME_TST(bdet_Datetime                  , "bdet_Datetime"            );
        NAME_TST(bdet_DatetimeTz                , "bdet_DatetimeTz"          );
        NAME_TST(bdet_Time                      , "bdet_Time"                );
        NAME_TST(bdet_TimeTz                    , "bdet_TimeTz"              );

        NAME_TST(MySequence                     , "MySequence"               );
        NAME_TST(MyChoice                       , "MyChoice"                 );
        NAME_TST(MyCustomizedType               , "MyCustomizedType"         );
        NAME_TST(MyEnumeration::Value           , "MyEnumeration"            );
        NAME_TST(OtherType                      , typeid(OtherType).name()   );
        NAME_TST(MyClass                        , "MyClass"                  );
        NAME_TST(MyClassWith92ByteName          , STR_92BYTENAME             );
        NAME_TST(MyIntWrapper                   , "MyIntWrapper"             );

        NAME_TST(bsl::vector<bool>              , "vector<bool>"             );
        NAME_TST(bsl::vector<char>              , "vector<char>"             );
        NAME_TST(bsl::vector<unsigned>          , "vector<unsigned int>"     );
        NAME_TST(bsl::vector<bsl::string>       , "vector<string>"           );
        NAME_TST(bsl::vector<const char*>       , "vector<const char*>"      );

        NAME_TST(bsl::vector<MySequence>        , "vector<MySequence>"       );
        NAME_TST(bsl::vector<MyChoice>          , "vector<MyChoice>"         );
        NAME_TST(bsl::vector<MyCustomizedType>  , "vector<MyCustomizedType>" );
        NAME_TST(bsl::vector<MyEnumeration::Value>, "vector<MyEnumeration>"  );
        NAME_TST(bsl::vector<MyClass>           ,  "vector<MyClass>"         );
        NAME_TST(bsl::vector<MyClassWith92ByteName>,
                                                "vector<" STR_92BYTENAME ">" );

        NAME_TST(bsl::vector<bsl::vector<int> > , "vector<vector<int>>"      );

        {
            // Special test for truncated name
            bsl::vector<bsl::vector<MyClassWith92ByteName> > object;
            static const char *fullName = "vector<vector<" STR_92BYTENAME ">>";
            const char *name = bdeat_TypeName::name(object);
            LOOP_ASSERT(name, 100 == bsl::strlen(name));
            LOOP_ASSERT(name, 0 == bsl::strncmp(fullName, name, 100));
        }

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'className'
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'className'"
                          << "\n===================" << endl;

        CLASSNAME_TST(bool                              , 0                  );
        CLASSNAME_TST(char                              , 0                  );
        CLASSNAME_TST(signed char                       , 0                  );
        CLASSNAME_TST(unsigned char                     , 0                  );
        CLASSNAME_TST(short                             , 0                  );
        CLASSNAME_TST(unsigned short                    , 0                  );
        CLASSNAME_TST(int                               , 0                  );
        CLASSNAME_TST(unsigned int                      , 0                  );
        CLASSNAME_TST(long                              , 0                  );
        CLASSNAME_TST(unsigned long                     , 0                  );
        CLASSNAME_TST(bsls_PlatformUtil::Int64          , 0                  );
        CLASSNAME_TST(bsls_PlatformUtil::Uint64         , 0                  );
        CLASSNAME_TST(float                             , 0                  );
        CLASSNAME_TST(double                            , 0                  );
        CLASSNAME_TST(bsl::string                       , 0                  );
        CLASSNAME_TST(const char*                       , 0                  );
        CLASSNAME_TST(const signed char*                , 0                  );
        CLASSNAME_TST(const unsigned char*              , 0                  );
        CLASSNAME_TST(bdet_Date                         , 0                  );
        CLASSNAME_TST(bdet_DateTz                       , 0                  );
        CLASSNAME_TST(bdet_Datetime                     , 0                  );
        CLASSNAME_TST(bdet_DatetimeTz                   , 0                  );
        CLASSNAME_TST(bdet_Time                         , 0                  );
        CLASSNAME_TST(bdet_TimeTz                       , 0                  );

        CLASSNAME_TST(MySequence                        , "MySequence"       );
        CLASSNAME_TST(MyChoice                          , "MyChoice"         );
        CLASSNAME_TST(MyCustomizedType                  , "MyCustomizedType" );
        CLASSNAME_TST(MyEnumeration::Value              , "MyEnumeration"    );
        CLASSNAME_TST(OtherType                         , 0                  );
        CLASSNAME_TST(MyClass                           , "MyClass"          );
        CLASSNAME_TST(MyIntWrapper                      , 0                  );
        CLASSNAME_TST(MyClassWith92ByteName             , STR_92BYTENAME     );

        CLASSNAME_TST(bsl::vector<bool>                 , 0                  );
        CLASSNAME_TST(bsl::vector<char>                 , 0                  );
        CLASSNAME_TST(bsl::vector<unsigned>             , 0                  );
        CLASSNAME_TST(bsl::vector<bsl::string>          , 0                  );
        CLASSNAME_TST(bsl::vector<const char*>          , 0                  );

        CLASSNAME_TST(bsl::vector<MySequence>           , 0                  );
        CLASSNAME_TST(bsl::vector<MyChoice>             , 0                  );
        CLASSNAME_TST(bsl::vector<MyCustomizedType>     , 0                  );
        CLASSNAME_TST(bsl::vector<MyEnumeration::Value> , 0                  );
        CLASSNAME_TST(bsl::vector<MyClass>              , 0                  );
        CLASSNAME_TST(bsl::vector<MyClassWith92ByteName>, 0                  );

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
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
