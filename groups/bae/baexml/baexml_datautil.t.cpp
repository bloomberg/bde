// baexml_datautil.t.cpp                                              -*-C++-*-
#include <baexml_datautil.h>

#include <baexml_errorinfo.h>
#include <baexml_minireader.h>
#include <baexml_decoder.h>
#include <baexml_decoderoptions.h>
#include <baexml_schemaparser.h>

#include <baexml_formatter.h>

#include <bdeat_formattingmode.h>

#include <bdem_elemref.h>                    // for testing only
#include <bdem_list.h>
#include <bdem_schema.h>
#include <bdem_schemaaggregateutil.h>

#include <bdesb_fixedmeminstreambuf.h>

#include <bdet_datetime.h>                   // for testing only
#include <bdet_datetimetz.h>                 // for testing only
#include <bdet_date.h>                       // for testing only
#include <bdet_datetz.h>                     // for testing only
#include <bdet_time.h>                       // for testing only
#include <bdet_timetz.h>                     // for testing only

#include <bdetu_unset.h>                     // for testing only

#include <bsls_platformutil.h>               // for testing only

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_stack.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_cctype.h>      // isupper
#include <bsl_cmath.h>       // fabs
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen()

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// Testing 'baexml_DataUtil' is divided into 2 parts (apart from
// breathing test and usage example).
//-----------------------------------------------------------------------------
// CREATORS
// [  ]
// [  ]
//
// MANIPULATORS
// [  ]
// [  ]
//
// ACCESSORS
// [  ]
// [  ]
//-----------------------------------------------------------------------------
// [  ] BREATHING TEST
// [  ]
// [  ]
// [  ] USAGE EXAMPLE
//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
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
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << #M << ": " <<  \
       M << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline
#define N_ bsl::cout << "\n" << bsl::flush;  // Print a newline.

//=============================================================================
//          GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
namespace {

int verbose;
int veryVerbose;
int veryVeryVerbose;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef baexml_MiniReader Reader;
typedef baexml_ErrorInfo  ErrorInfo;

typedef bdem_ElemRef             ERef;
typedef bdem_ElemType            EType;
typedef bdem_FieldSpec           FieldSpec;
typedef bdem_FieldDefAttributes  FieldAttr;
typedef bdem_RecordDef           Record;
typedef bsls_PlatformUtil::Int64 Int64;
typedef bdem_SchemaAggregateUtil SAU;

const char TESTNAMESPACE[] = "http://bloomberg.com/schemas/test";

    // Create Three Distinct Exemplars For Each Scalar Element Type
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const char            A00 = 'A';
const char            B00 = 'B';
const char            N00 = bdetu_Unset<char>::unsetValue();

const short           A01 = -1;
const short           B01 = -2;
const short           N01 = bdetu_Unset<short>::unsetValue();

const int             A02 = 10;
const int             B02 = 20;
const int             N02 = bdetu_Unset<int>::unsetValue();

const Int64           A03 = -100;
const Int64           B03 = -200;
const Int64           N03 =
                           bdetu_Unset<bsls_PlatformUtil::Int64>::unsetValue();

const float           A04 = -1.5;
const float           B04 = -2.5;
const float           N04 = bdetu_Unset<float>::unsetValue();

const double          A05 = 10.5;
const double          B05 = 20.5;
const double          N05 = bdetu_Unset<double>::unsetValue();

const bsl::string     A06 = "one";
const bsl::string     B06 = "two";
const bsl::string     N06 = bdetu_Unset<bsl::string>::unsetValue();

// Note: bdet_Datetime x07 implemented in terms of x08 and x09.

const bdet_Date       A08(2000,  1, 1);
const bdet_Date       B08(9999, 12,31);
const bdet_Date       N08 = bdetu_Unset<bdet_Date>::unsetValue();

const bdet_Time       A09(0, 1, 2, 3);
const bdet_Time       B09(4, 5, 6, 789);
const bdet_Time       N09 = bdetu_Unset<bdet_Time>::unsetValue();

const bdet_Datetime   A07(A08, A09);
const bdet_Datetime   B07(B08, B09);
const bdet_Datetime   N07 = bdetu_Unset<bdet_Datetime>::unsetValue();

const bool            A22 = true;
const bool            B22 = false;
const bool            N22 = bdetu_Unset<bool>::unsetValue();

const bdet_DatetimeTz A23(A07, -7);
const bdet_DatetimeTz B23(B07, 6);
const bdet_DatetimeTz N23 = bdetu_Unset<bdet_DatetimeTz>::unsetValue();

const bdet_DateTz     A24(A08, -5);
const bdet_DateTz     B24(B08, -4);
const bdet_DateTz     N24 = bdetu_Unset<bdet_DateTz>::unsetValue();

const bdet_TimeTz     A25(A09, 3);
const bdet_TimeTz     B25(B09, 11);
const bdet_TimeTz     N25 = bdetu_Unset<bdet_TimeTz>::unsetValue();

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------
enum TestFlags {
    TF_ERR_INFO     = 0x0001,  // Specify Error Info
    TF_DIAG_STREAM  = 0x0002,  // Specify Diagnostic Stream
    TF_VALIDATION   = 0x0004,  // Use Validating Reader
    TF_OPTIONS      = 0x0008,  // Use Decoder options
    TF_SKIP_UNK_EL  = 0x0010,  // Set SkipUnknownElements
    TF_ALLOCATOR    = 0x0020   // Use Test Allocator
};

class TestValidatingReader : public baexml_ValidatingReader
{
private:

    struct SchemaInfo;
    friend struct SchemaInfo;
    struct SchemaInfo
    {
        bsl::string               d_location;
        bsl::streambuf           *d_streambuf;
        bsl::streambuf::pos_type  d_startPos;
    };
    typedef bsl::vector<SchemaInfo>  SchemasCache;

    baexml_Reader      *d_reader;         // Actual reader
    bool                d_flgValidation;  // Validation mode
    SchemasCache        d_schemasCache;   // The cache of schemas

public:

    // PUBLIC CREATORS
    TestValidatingReader(baexml_Reader *reader);
    virtual ~TestValidatingReader(void);

    baexml_Reader *implementationReader()
    {
        return d_reader;
    }

    //------------------------------------------------
    // INTERFACE baexml_ValidatingReader
    //------------------------------------------------

    // MANIPULATORS
    virtual void enableValidation(bool validationFlag);
    virtual int addSchema(const char *location, bsl::streambuf *schema);
    virtual void  removeSchemas();

    // ACCESSORS
    virtual bool validationFlag() const;

    //------------------------------------------------
    // INTERFACE baexml_gReader
    //------------------------------------------------

    // MANIPULATORS - SETUP METHODS
    virtual void setResolver(XmlResolverFunctor resolver)
    {
        d_reader->setResolver(resolver);
    }

    virtual void setPrefixStack(baexml_PrefixStack *prefixes)
    {
        d_reader->setPrefixStack(prefixes);
    }

    // MANIPULATORS - OPEN/CLOSE AND NAVIGATION METHODS
    virtual int open(const char *filename,
                     const char *encoding = 0)
    {
        return d_reader->open(filename, encoding);
    }

    virtual int open(const char *buffer,
                     size_t      size,
                     const char *url = 0,
                     const char *encoding = 0)
    {
        return d_reader->open(buffer, size, url, encoding);
    }

    virtual int open(bsl::streambuf *stream,
                     const char     *url = 0,
                     const char     *encoding = 0)
    {
        return d_reader->open(stream, url, encoding);
    }

    virtual void close()
    {
        d_reader->close();
    }

    virtual int advanceToNextNode()
    {
        return d_reader->advanceToNextNode();
    }

    virtual int lookupAttribute(baexml_ElementAttribute *attribute,
                                int                      index) const
    {
        return d_reader->lookupAttribute(attribute, index);
    }

    virtual int lookupAttribute(baexml_ElementAttribute  *attribute,
                                const char               *qname) const
    {
        return d_reader->lookupAttribute(attribute, qname);
    }

    virtual int lookupAttribute(baexml_ElementAttribute  *attribute,
                                const char               *localName,
                                const char               *namespaceUri) const
    {
        return d_reader->lookupAttribute(attribute, localName, namespaceUri);
    }

    virtual int lookupAttribute(baexml_ElementAttribute  *attribute,
                                const char               *localName,
                                int                       namespaceId) const
    {
        return d_reader->lookupAttribute(attribute, localName, namespaceId);
    }

    virtual void setOptions(unsigned int flags)
    {
        d_reader->setOptions(flags);
    }

    // ACCESSORS
    virtual const char *documentEncoding() const
    {
        return d_reader->documentEncoding();
    }

    virtual XmlResolverFunctor resolver() const
    {
        return d_reader->resolver();
    }

    virtual bool isOpen() const
    {
        return d_reader->isOpen();
    }

    virtual const baexml_ErrorInfo& errorInfo() const
    {
        return d_reader->errorInfo();
    }

    virtual int getLineNumber() const
    {
        return d_reader->getLineNumber();
    }

    virtual int getColumnNumber() const
    {
        return d_reader->getColumnNumber();
    }

    virtual baexml_PrefixStack *prefixStack() const
    {
        return d_reader->prefixStack();
    }

    virtual NodeType nodeType() const
    {
        return d_reader->nodeType();
    }

    virtual const char *nodeName() const
    {
        return d_reader->nodeName();
    }

    virtual const char *nodeLocalName() const
    {
        return d_reader->nodeLocalName();
    }

    virtual const char *nodePrefix() const
    {
        return d_reader->nodePrefix();
    }

    virtual int nodeNamespaceId() const
    {
        return d_reader->nodeNamespaceId();
    }

    virtual const char *nodeNamespaceUri() const
    {
        return d_reader->nodeNamespaceUri();
    }

    virtual const char *nodeBaseUri() const
    {
        return d_reader->nodeBaseUri();
    }

    virtual bool nodeHasValue() const
    {
        return d_reader->nodeHasValue();
    }

    virtual const char *nodeValue() const
    {
        return d_reader->nodeValue();
    }

    virtual int nodeDepth() const
    {
        return d_reader->nodeDepth();
    }

    virtual int numAttributes() const
    {
        return d_reader->numAttributes();
    }

    virtual bool isEmptyElement() const
    {
        return d_reader->isEmptyElement();
    }

    virtual unsigned int options() const
    {
        return d_reader->options();
    }

};

TestValidatingReader::TestValidatingReader(baexml_Reader *reader)
: d_reader(reader)
, d_flgValidation(false)
, d_schemasCache()
{
}

TestValidatingReader::~TestValidatingReader(void)
{
}

// ---------------------------------------------------------------------------
//   baexml_ValidatingReader interface
// ---------------------------------------------------------------------------
bool
TestValidatingReader::validationFlag() const
{
    return d_flgValidation;
}

void
TestValidatingReader::enableValidation(bool validationFlag)
{
    d_flgValidation = validationFlag;
}

void
TestValidatingReader::removeSchemas()
{
    d_schemasCache.clear();
}

int
TestValidatingReader::addSchema(const char *location, bsl::streambuf *schema)
{
    SchemaInfo info;

    info.d_location  = location;
    info.d_streambuf = schema;
    info.d_startPos  = schema->pubseekoff(0,
                                          bsl::ios_base::cur,
                                          bsl::ios::in);

    bsl::vector<SchemaInfo>::iterator it1 = d_schemasCache.begin ();
    bsl::vector<SchemaInfo>::iterator it2 = d_schemasCache.end ();

    for (; it1 != it2; ++it1)
    {
        if (info.d_location == (*it1).d_location)
        {
            *it1 = info;
            return 0;
        }
    }

    d_schemasCache.push_back (info);
    return 0;
}

//-------------------------------------------------------------
//
//-------------------------------------------------------------

class ScalarData {
  private:
    EType::Type     d_type;
    char            d_char;
    short           d_short;
    int             d_int;
    Int64           d_int64;
    float           d_float;
    double          d_double;
    bsl::string     d_string;
    bdet_Datetime   d_datetime;
    bdet_Date       d_date;
    bdet_Time       d_time;
    bool            d_bool;
    bdet_DatetimeTz d_datetimeTz;
    bdet_DateTz     d_dateTz;
    bdet_TimeTz     d_timeTz;

  public:
    enum { REPEAT = 3 }; // REPEAT times to make an array
    ScalarData(char c) : d_type(EType::BDEM_CHAR), d_char(c) {}
    ScalarData(short s) : d_type(EType::BDEM_SHORT), d_short(s) {}
    ScalarData(int i) : d_type(EType::BDEM_INT), d_int(i) {}
    ScalarData(const Int64& i) : d_type(EType::BDEM_INT64), d_int64(i) {}
    ScalarData(float f) : d_type(EType::BDEM_FLOAT), d_float(f) {}
    ScalarData(double d) : d_type(EType::BDEM_DOUBLE), d_double(d) {}
    ScalarData(const bsl::string& s) : d_type(EType::BDEM_STRING), d_string(s)
    {}
    ScalarData(const bdet_Datetime& d)
    : d_type(EType::BDEM_DATETIME), d_datetime(d) {}
    ScalarData(const bdet_Date& d) : d_type(EType::BDEM_DATE), d_date(d) {}
    ScalarData(const bdet_Time& t) : d_type(EType::BDEM_TIME), d_time(t) {}
    ScalarData(bool b) : d_type(EType::BDEM_BOOL), d_bool(b) {}
    ScalarData(const bdet_DatetimeTz& d)
    : d_type(EType::BDEM_DATETIMETZ), d_datetimeTz(d) {}
    ScalarData(const bdet_DateTz& d) : d_type(EType::BDEM_DATETZ), d_dateTz(d)
    {}
    ScalarData(const bdet_TimeTz& t) : d_type(EType::BDEM_TIMETZ), d_timeTz(t)
    {}

    // ACCESSORS
    EType::Type type() const { return d_type; }
    EType::Type arrayType() const;
    void appendToList(bdem_List *list) const;
    void appendArrayToList(bdem_List *list, int arrSize = REPEAT) const;
    void addToRow(bdem_Row *row, int index) const;
    void addArrayToRow(bdem_Row *row, int index, int arrSize = REPEAT) const;
};

EType::Type ScalarData::arrayType() const
{
    switch (d_type) {
      case EType::BDEM_CHAR:       return EType::BDEM_CHAR_ARRAY;
      case EType::BDEM_SHORT:      return EType::BDEM_SHORT_ARRAY;
      case EType::BDEM_INT:        return EType::BDEM_INT_ARRAY;;
      case EType::BDEM_INT64:      return EType::BDEM_INT64_ARRAY;
      case EType::BDEM_FLOAT:      return EType::BDEM_FLOAT_ARRAY;
      case EType::BDEM_DOUBLE:     return EType::BDEM_DOUBLE_ARRAY;
      case EType::BDEM_STRING:     return EType::BDEM_STRING_ARRAY;
      case EType::BDEM_DATETIME:   return EType::BDEM_DATETIME_ARRAY;
      case EType::BDEM_DATE:       return EType::BDEM_DATE_ARRAY;
      case EType::BDEM_TIME:       return EType::BDEM_TIME_ARRAY;
      case EType::BDEM_BOOL:       return EType::BDEM_BOOL_ARRAY;
      case EType::BDEM_DATETIMETZ: return EType::BDEM_DATETIMETZ_ARRAY;
      case EType::BDEM_DATETZ:     return EType::BDEM_DATETZ_ARRAY;
      case EType::BDEM_TIMETZ:     return EType::BDEM_TIMETZ_ARRAY;
      default: {
        P(d_type);
        ASSERT("Invalid element type used in 'ScalarData::arrayType'" && 0);
      } break;
    }

    return EType::BDEM_CHAR;
}

void ScalarData::appendToList(bdem_List *list) const
{
    switch (d_type) {
      case EType::BDEM_CHAR: {
        list->appendChar(d_char);
      } break;
      case EType::BDEM_SHORT: {
        list->appendShort(d_short);
      } break;
      case EType::BDEM_INT: {
        list->appendInt(d_int);
      } break;
      case EType::BDEM_INT64: {
        list->appendInt64(d_int64);
      } break;
      case EType::BDEM_FLOAT: {
        list->appendFloat(d_float);
      } break;
      case EType::BDEM_DOUBLE: {
        list->appendDouble(d_double);
      } break;
      case EType::BDEM_STRING: {
        list->appendString(d_string);
      } break;
      case EType::BDEM_DATETIME: {
        list->appendDatetime(d_datetime);
      } break;
      case EType::BDEM_DATE: {
        list->appendDate(d_date);
      } break;
      case EType::BDEM_TIME: {
        list->appendTime(d_time);
      } break;
      case EType::BDEM_BOOL: {
        list->appendBool(d_bool);
      } break;
      case EType::BDEM_DATETIMETZ: {
        list->appendDatetimeTz(d_datetimeTz);
      } break;
      case EType::BDEM_DATETZ: {
        list->appendDateTz(d_dateTz);
      } break;
      case EType::BDEM_TIMETZ: {
        list->appendTimeTz(d_timeTz);
      } break;
      default: {
        P(d_type);
        ASSERT("Invalid element type used in 'ScalarData::appendToList'" && 0);
      } break;
    }
}

void ScalarData::appendArrayToList(bdem_List *list, int arrSize) const
{
    switch(d_type) {
      case EType::BDEM_CHAR: {
        bsl::vector<char> charArray;
        for (int i = 0; i < arrSize; ++i) {
            charArray.push_back(d_char);
        }
        list->appendCharArray(charArray);
      } break;
      case EType::BDEM_SHORT: {
        bsl::vector<short> shortArray;
        for (int i = 0; i < arrSize; ++i) {
            shortArray.push_back(d_short);
        }
        list->appendShortArray(shortArray);
      } break;
      case EType::BDEM_INT: {
        bsl::vector<int> intArray;
        for (int i = 0; i < arrSize; ++i) {
            intArray.push_back(d_int);
        }
        list->appendIntArray(intArray);
      } break;
      case EType::BDEM_INT64: {
        bsl::vector<Int64> int64Array;
        for (int i = 0; i < arrSize; ++i) {
            int64Array.push_back(d_int64);
        }
        list->appendInt64Array(int64Array);
      } break;
      case EType::BDEM_FLOAT: {
        bsl::vector<float> floatArray;
        for (int i = 0; i < arrSize; ++i) {
            floatArray.push_back(d_float);
        }
        list->appendFloatArray(floatArray);
      } break;
      case EType::BDEM_DOUBLE: {
        bsl::vector<double> doubleArray;
        for (int i = 0; i < arrSize; ++i) {
            doubleArray.push_back(d_double);
        }
        list->appendDoubleArray(doubleArray);
      } break;
      case EType::BDEM_STRING: {
        bsl::vector<bsl::string> stringArray;
        for (int i = 0; i < arrSize; ++i) {
            stringArray.push_back(d_string);
        }
        list->appendStringArray(stringArray);
      } break;
      case EType::BDEM_DATETIME: {
        bsl::vector<bdet_Datetime> datetimeArray;
        for (int i = 0; i < arrSize; ++i) {
            datetimeArray.push_back(d_datetime);
        }
        list->appendDatetimeArray(datetimeArray);
      } break;
      case EType::BDEM_DATE: {
        bsl::vector<bdet_Date> dateArray;
        for (int i = 0; i < arrSize; ++i) {
            dateArray.push_back(d_date);
        }
        list->appendDateArray(dateArray);
      } break;
      case EType::BDEM_TIME: {
        bsl::vector<bdet_Time> timeArray;
        for (int i = 0; i < arrSize; ++i) {
            timeArray.push_back(d_time);
        }
        list->appendTimeArray(timeArray);
      } break;
      case EType::BDEM_BOOL: {
          bsl::vector<bool> boolArray;
          for (int i = 0; i < arrSize; ++i) {
              boolArray.push_back(d_bool);
          }
          list->appendBoolArray(boolArray);
      } break;
      case EType::BDEM_DATETIMETZ: {
          bsl::vector<bdet_DatetimeTz> datetimeTzArray;
          for (int i = 0; i < arrSize; ++i) {
              datetimeTzArray.push_back(d_datetimeTz);
          }
          list->appendDatetimeTzArray(datetimeTzArray);
      } break;
      case EType::BDEM_DATETZ: {
          bsl::vector<bdet_DateTz> dateTzArray;
          for (int i = 0; i < arrSize; ++i) {
              dateTzArray.push_back(d_dateTz);
          }
          list->appendDateTzArray(dateTzArray);
      } break;
      case EType::BDEM_TIMETZ: {
          bsl::vector<bdet_TimeTz> timeTzArray;
          for (int i = 0; i < arrSize; ++i) {
              timeTzArray.push_back(d_timeTz);
          }
          list->appendTimeTzArray(timeTzArray);
      } break;
      default: {
        P(d_type);
        ASSERT("Invalid element type used in 'ScalarData::appendArrayToList'"
            && 0);
      } break;
    }
}

void ScalarData::addToRow(bdem_Row *row, int index) const
{
    ASSERT(row->elemType(index) == d_type);

    switch (d_type) {
      case EType::BDEM_CHAR: {
        row->theModifiableChar(index) = d_char;
      } break;
      case EType::BDEM_SHORT: {
        row->theModifiableShort(index) = d_short;
      } break;
      case EType::BDEM_INT: {
        row->theModifiableInt(index) = d_int;
      } break;
      case EType::BDEM_INT64: {
        row->theModifiableInt64(index) = d_int64;
      } break;
      case EType::BDEM_FLOAT: {
        row->theModifiableFloat(index) = d_float;
      } break;
      case EType::BDEM_DOUBLE: {
        row->theModifiableDouble(index) = d_double;
      } break;
      case EType::BDEM_STRING: {
        row->theModifiableString(index) = d_string;
      } break;
      case EType::BDEM_DATETIME: {
        row->theModifiableDatetime(index) = d_datetime;
      } break;
      case EType::BDEM_DATE: {
        row->theModifiableDate(index) = d_date;
      } break;
      case EType::BDEM_TIME: {
        row->theModifiableTime(index) = d_time;
      } break;
      case EType::BDEM_BOOL: {
        row->theModifiableBool(index) = d_bool;
      } break;
      case EType::BDEM_DATETIMETZ: {
        row->theModifiableDatetimeTz(index) = d_datetimeTz;
      } break;
      case EType::BDEM_DATETZ: {
        row->theModifiableDateTz(index) = d_dateTz;
      } break;
      case EType::BDEM_TIMETZ: {
        row->theModifiableTimeTz(index) = d_timeTz;
      } break;
      default: {
        P(d_type);
        ASSERT("Invalid element type used in 'ScalarData::addToRow'" && 0);
      } break;
    }
}

void ScalarData::addArrayToRow(bdem_Row *row, int index, int arrSize) const
{
    ASSERT(row->elemType(index) == d_type + 10);  // + 10 to become ARRAY

    switch(d_type) {
      case EType::BDEM_CHAR: {
        bsl::vector<char> charArray;
        for (int i = 0; i < arrSize; ++i) {
            charArray.push_back(d_char);
        }
        row->theModifiableCharArray(index) = charArray;
      } break;
      case EType::BDEM_SHORT: {
        bsl::vector<short> shortArray;
        for (int i = 0; i < arrSize; ++i) {
            shortArray.push_back(d_short);
        }
        row->theModifiableShortArray(index) = shortArray;
      } break;
      case EType::BDEM_INT: {
        bsl::vector<int> intArray;
        for (int i = 0; i < arrSize; ++i) {
            intArray.push_back(d_int);
        }
        row->theModifiableIntArray(index) = intArray;
      } break;
      case EType::BDEM_INT64: {
        bsl::vector<Int64> int64Array;
        for (int i = 0; i < arrSize; ++i) {
            int64Array.push_back(d_int64);
        }
        row->theModifiableInt64Array(index) = int64Array;
      } break;
      case EType::BDEM_FLOAT: {
        bsl::vector<float> floatArray;
        for (int i = 0; i < arrSize; ++i) {
            floatArray.push_back(d_float);
        }
        row->theModifiableFloatArray(index) = floatArray;
      } break;
      case EType::BDEM_DOUBLE: {
        bsl::vector<double> doubleArray;
        for (int i = 0; i < arrSize; ++i) {
            doubleArray.push_back(d_double);
        }
        row->theModifiableDoubleArray(index) = doubleArray;
      } break;
      case EType::BDEM_STRING: {
        bsl::vector<bsl::string> stringArray;
        for (int i = 0; i < arrSize; ++i) {
            stringArray.push_back(d_string);
        }
        row->theModifiableStringArray(index) = stringArray;
      } break;
      case EType::BDEM_DATETIME: {
        bsl::vector<bdet_Datetime> datetimeArray;
        for (int i = 0; i < arrSize; ++i) {
            datetimeArray.push_back(d_datetime);
        }
        row->theModifiableDatetimeArray(index) = datetimeArray;
      } break;
      case EType::BDEM_DATE: {
        bsl::vector<bdet_Date> dateArray;
        for (int i = 0; i < arrSize; ++i) {
            dateArray.push_back(d_date);
        }
        row->theModifiableDateArray(index) = dateArray;
      } break;
      case EType::BDEM_TIME: {
        bsl::vector<bdet_Time> timeArray;
        for (int i = 0; i < arrSize; ++i) {
            timeArray.push_back(d_time);
        }
        row->theModifiableTimeArray(index) = timeArray;
      } break;
      case EType::BDEM_BOOL: {
          bsl::vector<bool> boolArray;
          for (int i = 0; i < arrSize; ++i) {
              boolArray.push_back(d_bool);
          }
          row->theModifiableBoolArray(index) = boolArray;
      } break;
      case EType::BDEM_DATETIMETZ: {
          bsl::vector<bdet_DatetimeTz> datetimeTzArray;
          for (int i = 0; i < arrSize; ++i) {
              datetimeTzArray.push_back(d_datetimeTz);
          }
          row->theModifiableDatetimeTzArray(index) = datetimeTzArray;
      } break;
      case EType::BDEM_DATETZ: {
          bsl::vector<bdet_DateTz> dateTzArray;
          for (int i = 0; i < arrSize; ++i) {
              dateTzArray.push_back(d_dateTz);
          }
          row->theModifiableDateTzArray(index) = dateTzArray;
      } break;
      case EType::BDEM_TIMETZ: {
          bsl::vector<bdet_TimeTz> timeTzArray;
          for (int i = 0; i < arrSize; ++i) {
              timeTzArray.push_back(d_timeTz);
          }
          row->theModifiableTimeTzArray(index) = timeTzArray;
      } break;
      default: {
        P(d_type);
        ASSERT("Invalid element type used in 'ScalarData::addArrayToRow'"
            && 0);
      } break;
    }
}

const char *NAMEDNAMES[] = {
    "Apple",         // A
    "Banana",        // B
    "Catapult",      // C
    "Daddy",         // D
    "Effervescent",  // E
    "Ferocious",     // F
    "Good",          // G
    "Holiday",       // H
    "Interesting",   // I
    "Jump",          // J
    "Kangaroo",      // K
    "Largo",         // L
    "Mandatory",     // M
    "Nemo",          // N
    "Opulent",       // O
    "Perk",          // P
    "Query",         // Q
    "Radar",         // R
    "Snow",          // S
    "Temporary",     // T
    "Unit",          // U
    "Vector",        // V
    "Walrus",        // W
    "Xenon",         // X
    "Yack",          // Y
    "Zipper"         // Z
};

const int NUM_NAMEDNAMES = sizeof NAMEDNAMES / sizeof *NAMEDNAMES;

const char *ANONYMOUSNAMES[] = {
    "RECORD_0",      // A
    "RECORD_1",      // B
    "RECORD_2",      // C
    "RECORD_3",      // D
    "RECORD_4",      // E
    "RECORD_5",      // F
    "RECORD_6",      // G
    "RECORD_7",      // H
    "RECORD_8",      // I
    "RECORD_9",      // J
    "RECORD_10",     // K
    "RECORD_11",     // L
    "RECORD_12",     // M
    "FIELD_0",       // N
    "FIELD_1",       // O
    "FIELD_2",       // P
    "FIELD_3",       // Q
    "FIELD_4",       // R
    "FIELD_5",       // S
    "FIELD_6",       // T
    "FIELD_7",       // U
    "FIELD_8",       // V
    "FIELD_9",       // W
    "FIELD_10",      // X
    "FIELD_11",      // Y
    "FIELD_12"       // Z
};

const int NUM_ANONYMOUSNAMES = sizeof ANONYMOUSNAMES / sizeof *ANONYMOUSNAMES;

EType::Type gType(char typeCode)
    // Return the 'bdem' element type corresponding to the specified
    // 'typeCode'.
    //..
    //  'a' => CHAR        'k' => CHAR_ARRAY
    //  'b' => SHORT       'l' => SHORT_ARRAY
    //  'c' => INT         'm' => INT_ARRAY
    //  'd' => INT64       'n' => INT64_ARRAY
    //  'e' => FLOAT       'o' => FLOAT_ARRAY
    //  'f' => DOUBLE      'p' => DOUBLE_ARRAY
    //  'g' => STRING      'q' => STRING_ARRAY
    //  'h' => DATETIME    'r' => DATETIME_ARRAY
    //  'i' => DATE        's' => DATE_ARRAY
    //  'j' => TIME        't' => TIME_ARRAY
    //
    //  'u' => LIST        'v' => TABLE
    //
    //  'w' => BOOL        'A' => BOOL_ARRAY
    //  'x' => DATETIMETZ  'B' => DATETIMETZ_ARRAY
    //  'y' => DATETZ      'C' => DATETZ_ARRAY
    //  'z' => TIMETZ      'D' => TIMETZ_ARRAY
    //
    //  'E' => CHOICE      'F' => CHOICE_ARRAY
    //..
{
    switch (typeCode) {
      case 'a': return EType::BDEM_CHAR;
      case 'b': return EType::BDEM_SHORT;
      case 'c': return EType::BDEM_INT;
      case 'd': return EType::BDEM_INT64;
      case 'e': return EType::BDEM_FLOAT;
      case 'f': return EType::BDEM_DOUBLE;
      case 'g': return EType::BDEM_STRING;
      case 'h': return EType::BDEM_DATETIME;
      case 'i': return EType::BDEM_DATE;
      case 'j': return EType::BDEM_TIME;
      case 'k': return EType::BDEM_CHAR_ARRAY;
      case 'l': return EType::BDEM_SHORT_ARRAY;
      case 'm': return EType::BDEM_INT_ARRAY;
      case 'n': return EType::BDEM_INT64_ARRAY;
      case 'o': return EType::BDEM_FLOAT_ARRAY;
      case 'p': return EType::BDEM_DOUBLE_ARRAY;
      case 'q': return EType::BDEM_STRING_ARRAY;
      case 'r': return EType::BDEM_DATETIME_ARRAY;
      case 's': return EType::BDEM_DATE_ARRAY;
      case 't': return EType::BDEM_TIME_ARRAY;
      case 'u': return EType::BDEM_LIST;
      case 'v': return EType::BDEM_TABLE;
      case 'w': return EType::BDEM_BOOL;
      case 'x': return EType::BDEM_DATETIMETZ;
      case 'y': return EType::BDEM_DATETZ;
      case 'z': return EType::BDEM_TIMETZ;
      case 'A': return EType::BDEM_BOOL_ARRAY;
      case 'B': return EType::BDEM_DATETIMETZ_ARRAY;
      case 'C': return EType::BDEM_DATETZ_ARRAY;
      case 'D': return EType::BDEM_TIMETZ_ARRAY;
      case 'E': return EType::BDEM_CHOICE;
      case 'F': return EType::BDEM_CHOICE_ARRAY;

      default: {
        P(typeCode);  ASSERT("Invalid element type used in 'gType'" && 0);
      } break;
    }

    return EType::BDEM_INT;
}

void gFormattingMode(int *formattingMode, char code)
    // Or into the specified 'bdeat' 'formattingMode' the value corresponding
    // to the specified 'code'.  Note that this function is used by
    // 'gBdemSchema'.
{
    switch (code) {
       case '0': {  // zero (e.g., "off" or no explicit mode specified)
         *formattingMode |= bdeat_FormattingMode::BDEAT_DEFAULT;
       } break;
       case 'B': {
         *formattingMode |= bdeat_FormattingMode::BDEAT_BASE64;
       } break;
       case 'D': {
         *formattingMode |= bdeat_FormattingMode::BDEAT_DEC;
       } break;
       case 'L': {
         *formattingMode |= bdeat_FormattingMode::BDEAT_LIST;
       } break;
       case 'T': {
         *formattingMode |= bdeat_FormattingMode::BDEAT_TEXT;
       } break;
       case 'X': {
         *formattingMode |= bdeat_FormattingMode::BDEAT_HEX;
       } break;
       case 'n': {
         *formattingMode |= bdeat_FormattingMode::BDEAT_NILLABLE;
       } break;
       default: {
         ASSERT("Invalid code passed to 'gFormattingMode'" && 0);
       } break;
    }
}

bool gNullability(char nullCode, EType::Type type)
    // Return the nullability value corresponding to the specified 'nullCode'
    // and 'bdem' element 'type'.  Note that 'type' is ignored unless the
    // default nullability is specified.
    //..
    //  'D' => default for 'type'
    //  'F' => false
    //  'T' => true
    //..
{
    switch (nullCode) {
      case 'D': return EType::BDEM_LIST == type ? false : true;
      case 'F': return false;
      case 'T': return true;
      default: {
        P(nullCode);
        ASSERT("Invalid nullability used in 'gNullability' script" && 0);
      } break;
    }

    return true;
}

const ScalarData VALUES[] = {
    (char)0x41,                                  // a  0x41 is for 'A'
    (short)-1,                                   // b
    (int)10,                                     // c
    (Int64)-100,                                 // d
    (float)-1.5,                                 // e
    (double)10.5,                                // f
    bsl::string("one"),                          // g
    bdet_Datetime(2000, 1, 1, 0, 1, 2, 3),       // h
    bdet_Date(2000, 1, 1),                       // i
    bdet_Time(0, 1, 2, 3),                       // j
    (char)0x41,                                  // k array repeat REPEAT times
    (short)-1,                                   // l array repeat REPEAT times
    (int)10,                                     // m array repeat REPEAT times
    (Int64)-100,                                 // n array repeat REPEAT times
    (float)-1.5,                                 // o array repeat REPEAT times
    (double)10.5,                                // p array repeat REPEAT times
    bsl::string("one"),                          // q array repeat REPEAT times
    bdet_Datetime(2000, 1, 1, 0, 1, 2, 3),       // r array repeat REPEAT times
    bdet_Date(2000, 1, 1),                       // s array repeat REPEAT times
    bdet_Time(0, 1, 2, 3),                       // t array repeat REPEAT times
};

const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

const char *TEXTVALUES[] = {
    "65",                                  // a  65 is decimal for 'A'
    "-1",                                  // b
    "10",                                  // c
    "-100",                                // d
    "-1.5",                                // e
    "10.5",                                // f
    "one",                                 // g
    "2000-01-01T00:01:02.003",             // h
    "2000-01-01",                          // i
    "00:01:02.003",                        // j
    "65 65 65",                            // k  65 is decimal for 'A'
    "-1 -1 -1",                            // l
    "10 10 10",                            // m
    "-100 -100 -100",                      // n
    "-1.5 -1.5 -1.5",                      // o
    "10.5 10.5 10.5",                      // p
    "one",                                            // q, this needs to be
                                                      //    repeated REPEAT
                                                      //     times in gXml
    "2000-01-01T00:01:02.003 2000-01-01T00:01:02.003" // r
    " 2000-01-01T00:01:02.003",
    "2000-01-01 2000-01-01 2000-01-01",               // s
    "00:01:02.003 00:01:02.003 00:01:02.003",         // t
};

const int NUM_TEXTVALUES = sizeof TEXTVALUES / sizeof *TEXTVALUES;

void parseFieldAttributes(int                          *defaultIndex,
                          int                          *formattingMode,
                          bool                         *isNullable,
                          EType::Type                   fieldType,
                          bsl::string::const_iterator&  iter)
    // Parse field attributes for the specified 'fieldType' from the specified
    // 'iter'.  Note that 'iter' is passed by reference and is advanced by this
    // function!
{
    // Reset attributes to their default values.

    *defaultIndex   = -1;
    *formattingMode = bdeat_FormattingMode::BDEAT_DEFAULT;
    *isNullable     = false;

    // Each field attribute specification is of the form '@av' where 'a'
    // denotes the attribute and 'v' encodes its value.

    while ('@' == *(iter + 1)) {
        ++iter;  // advance to '@'
        ++iter;  // advance to attribute code
        const char attributeCode = *iter;
        ++iter;  // advance to value
        const char attributeValue = *iter;
        switch (attributeCode) {
          case 'D': {
            ASSERT('0' <= attributeValue && attributeValue <= '1');
            *defaultIndex = attributeValue - '0';
          } break;
          case 'F': {
            gFormattingMode(formattingMode, attributeValue);
          } break;
          case 'N': {
            *isNullable = gNullability(attributeValue, fieldType);
          } break;
          default: {
            P(attributeCode);
            ASSERT("Invalid code passed to 'parseFieldAttributes'" && 0);
          } break;
        }
    }

    if (*formattingMode & bdeat_FormattingMode::BDEAT_NILLABLE
     && !bdem_ElemType::isArrayType(fieldType)) {
        *isNullable = true;
    }
}

void setDefaultValueForType(ERef ref, bool other = false)
    // Set the value of the element referenced by 'ref' to a non-null value
    // appropriate for its type.  If the optionally-specified 'other' flag is
    // 'true', set the value of the element referenced by 'ref' to an alternate
    // non-null value.  The behavior is undefined unless the type of the
    // element referenced by 'ref' is a scalar 'bdem' type.
{
    switch (ref.type()) {
      case EType::BDEM_CHAR: {
        ref.theModifiableChar()       = other ? B00 : A00;
      } break;
      case EType::BDEM_SHORT: {
        ref.theModifiableShort()      = other ? B01 : A01;
      } break;
      case EType::BDEM_INT: {
        ref.theModifiableInt()        = other ? B02 : A02;
      } break;
      case EType::BDEM_INT64: {
        ref.theModifiableInt64()      = other ? B03 : A03;
      } break;
      case EType::BDEM_FLOAT: {
        ref.theModifiableFloat()      = other ? B04 : A04;
      } break;
      case EType::BDEM_DOUBLE: {
        ref.theModifiableDouble()     = other ? B05 : A05;
      } break;
      case EType::BDEM_STRING: {
        ref.theModifiableString()     = other ? B06 : A06;
      } break;
      case EType::BDEM_DATETIME: {
        ref.theModifiableDatetime()   = other ? B07 : A07;
      } break;
      case EType::BDEM_DATE: {
        ref.theModifiableDate()       = other ? B08 : A08;
      } break;
      case EType::BDEM_TIME: {
        ref.theModifiableTime()       = other ? B09 : A09;
      } break;
      case EType::BDEM_BOOL: {
        ref.theModifiableBool()       = other ? B22 : A22;
      } break;
      case EType::BDEM_DATETIMETZ: {
        ref.theModifiableDatetimeTz() = other ? B23 : A23;
      } break;
      case EType::BDEM_DATETZ: {
        ref.theModifiableDateTz()     = other ? B24 : A24;
      } break;
      case EType::BDEM_TIMETZ: {
        ref.theModifiableTimeTz()     = other ? B25 : A25;
      } break;
      default: {
        ASSERT("Invalid element passed to 'setDefaultValueForType'" && 0);
      } break;
    }
}

// Generate a 'bdem_Schema' from a compact specification string.
//
// Each record in the schema is specified using an upper-case letter, which
// corresponds to a name in the 'NAMES' array.  An '*' is used to specify an
// unnamed record.  The record name code is followed by an open curly brace
// ({) followed by zero or more sets of two or three-letter field
// definitions.
//
// Each field definition consists of a capital letter, again referring to the
// 'NAMES' array, with '*' used for nameless fields.  The field name code is
// followed by a lower-case or upper-case letter that specifies one of the 32
// types as per the 'gType' function.  If the type code is 'u' (LIST), 'v'
// (TABLE), 'E' (CHOICE), or 'F' (CHOICE_ARRAY), then it must be followed by
// another upper-case letter specifying the name of the constraint record, or a
// digit (0-9) specifying the index of an unnamed record.
//
// The following examples refer to the 'ELEMENTS' and 'NAMES' arrays, and the
// 'gType' function defined above:
//
// Input spec           Output bdem_Schema
// ----------           ------------------
// A{BaDl}S{WvAKb}      {
//                          RECORD "Apple" {
//                              CHAR "Banana";
//                              SHORT_ARRAY "Daddy";
//                          }
//
//                          RECORD "Snow" {
//                              TABLE<"Apple"> "Walrus";
//                              SHORT "Kangaroo";
//                          }
//                      }
void gBdemSchema(bdem_Schema *schema, const bsl::string& spec,
                 bool useAnonymousNames = false)
{
    schema->removeAll();

    const char **NAMES = useAnonymousNames ? ANONYMOUSNAMES : NAMEDNAMES;
    const int NUM_NAMES = useAnonymousNames ? NUM_ANONYMOUSNAMES
                                            : NUM_NAMEDNAMES;

    enum Expectation { RECNAME, OPENCURLY, FIELDNAME, FIELDTYPE, CONSTRAINT };
    enum Pass { CREATE_RECORDS = 0, APPEND_FIELDS = 1 };

    // The 'schema' must be constructed in two passes to accommodate forward
    // references to records.  Empty records are created in the first pass.
    // The records are populated in the second pass.

    for (int currentPass = CREATE_RECORDS; currentPass <= APPEND_FIELDS;
                                                               ++currentPass) {

        Expectation expected = RECNAME;
        Record *currRecord = 0;
        bdem_FieldDef *field = 0;
        const char *fieldName = 0;
        int recordIndex = 0;  // needed for '*' records in 'APPEND_FIELDS' pass
        EType::Type fieldType;
        int defaultIndex;
        int formattingMode;
        bool isNullable;

        for (bsl::string::const_iterator i = spec.begin(); i != spec.end();
                                                                         ++i) {
            switch (expected) {
              case RECNAME: {
                ASSERT(bsl::isupper(*i) || '*' == *i);
                if (CREATE_RECORDS == currentPass) {
                    currRecord = schema->createRecord('*' == *i
                                                      ? 0 : NAMES[*i - 'A']);
                }
                else {
                    const Record *constRecord;
                    if (bsl::isupper(*i)) {
                        constRecord = schema->lookupRecord(NAMES[*i - 'A']);
                    }
                    else {  // '*'
                        constRecord = &schema->record(recordIndex);
                    }
                    currRecord = const_cast<Record *>(constRecord);
                }
                ASSERT(currRecord);
                ++recordIndex;
                expected = OPENCURLY;
              } break;
              case OPENCURLY: {
                ASSERT('{' == *i);
                expected = FIELDNAME;
              } break;
              case FIELDNAME: {
                if ('}' == *i) {
                    expected = RECNAME;
                    break;
                }
                ASSERT(bsl::isupper(*i) || '*' == *i);
                fieldName = '*' == *i ? 0 : NAMES[*i - 'A'];
                expected = FIELDTYPE;
              } break;
              case FIELDTYPE: {
                fieldType = gType(*i);
                if (EType::isAggregateType(fieldType)) {
                    expected = CONSTRAINT;
                }
                else {
                    parseFieldAttributes(&defaultIndex,
                                         &formattingMode,
                                         &isNullable,
                                         fieldType,
                                         i);
                    if (APPEND_FIELDS == currentPass) {
                        ASSERT(currRecord);

                        FieldAttr fieldAttr(fieldType,
                                            isNullable,
                                            formattingMode);

                        if (-1 != defaultIndex) {
                            ASSERT(EType::isScalarType(fieldType));
                            setDefaultValueForType(fieldAttr.defaultValue(),
                                                   defaultIndex);
                        }
                        currRecord->appendField(fieldAttr, fieldName);
                    }
                    expected = FIELDNAME;
                }
              } break;
              case CONSTRAINT: {
                ASSERT(bsl::isupper(*i) || bsl::isdigit(*i));

                if (APPEND_FIELDS == currentPass) {
                    const Record *constraint = 0;
                    if (bsl::isupper(*i)) {
                        constraint = schema->lookupRecord(NAMES[*i - 'A']);
                    }
                    else {  // '0' .. '9'
                        constraint = &schema->record(*i - '0');
                    }

                    ASSERT(constraint);
                    ASSERT(EType::isAggregateType(fieldType));
                    ASSERT(currRecord);

                    parseFieldAttributes(&defaultIndex,
                                         &formattingMode,
                                         &isNullable,
                                         fieldType,
                                         i);

                    FieldAttr fieldAttr(fieldType,
                                        isNullable,
                                        formattingMode);

                    currRecord->appendField(fieldAttr, constraint, fieldName);
                }
                else {
                    parseFieldAttributes(&defaultIndex,
                                         &formattingMode,
                                         &isNullable,
                                         fieldType,
                                         i);
                }
                expected = FIELDNAME;
              } break;
              default: {
                ASSERT(0);
              } break;
            }
        }
    }
}

class Container {
    // A class that encapsulates the three bdem container types into one, such
    // that an object of this class that points to one container object can be
    // stored in a stack during the construction of bdem_List from scratch.
    // This is used in generator gBdemList.
  public:
    enum Type { IS_LIST, IS_TABLE, IS_ROW } d_type;
  private:
    union {
        bdem_List  *d_list_p;
        bdem_Table *d_table_p;
        bdem_Row   *d_row_p;
    }                     d_container;
    int                   d_currentIndex;
    bsl::vector<EType::Type> d_elemTypes; // only meaningful when IS_TABLE
    // DISABLED
    Container& operator=(const Container&);
        // Not implemented
  public:
    // CREATORS
    Container(bdem_List *bdemList)
    : d_type(IS_LIST)
    , d_currentIndex(-1)
    {
        d_container.d_list_p = bdemList;
    }

    Container(bdem_Table *bdemTable)
    : d_type(IS_TABLE)
    , d_currentIndex(-1)
    {
        d_container.d_table_p = bdemTable;
    }

    Container(bdem_Row *bdemRow)
    : d_type(IS_ROW)
    , d_currentIndex(-1)
    {
        d_container.d_row_p = bdemRow;
    }

    // Container(const Container&); // use synthetic copy constructor
    ~Container() {}

    // MANIPULATORS
    bdem_List *list()
    {
        return d_container.d_list_p ? d_container.d_list_p : 0;
    }

    bdem_Table *table()
    {
        return d_container.d_table_p ? d_container.d_table_p : 0;
    }

    bdem_Row *row()
    {
        return d_container.d_row_p ? d_container.d_row_p : 0;
    }

    void incrementCurrentIndex()
    {
        ++d_currentIndex;
    }

    void pushBackElemType(EType::Type elemType)
    {
        ASSERT(IS_TABLE == d_type);
        d_elemTypes.push_back(elemType);
    }

    // ACCESSORS
    Type type() const
    {
        return d_type;
    }

    int currentIndex() const
    {
        return d_currentIndex;
    }

    const bsl::vector<EType::Type>& elemTypes() const
    {
        ASSERT(IS_TABLE == d_type);
        return d_elemTypes;
    }
};

// Synopsis:
// (ab0c[adk(a0dk)(adk)(adk0)]mn()[adk])
// * lowercase letters [a-t]: bdem_ElemType scalar or array type
//   a0 - t0: scalar or array field is null
// * bdem_ElemType LIST and TABLE
//   lowercase u and v: there's only the following usage:
//                    v: a place holder declaring a table in the column
//                       specification of a table
//                    u: a place holder declaring a list in the column
//                       specification of a table
//                    v0: table field is null, instead of the [] below
//                    u0: list field is null, instead of the () below
// * []: non-null table field:
//                letters must follow the '[' first, defining column types
//                after letters, each row is enclosed in ()
// * (): non-null list field:
//                list type or table's row type.  Can be empty: it contains
//                                                              null values.
// The value for all these scalar/array types are canned in VALUES.
void gBdemList(bdem_List *bdemList, const bsl::string& spec)
{
    ASSERT('(' == spec[0]);

    bdemList->removeAll();

    bsl::stack<Container> containerStack;

    containerStack.push(Container(bdemList));

    enum Expect {
        OPEN_PAREN,
        CLOSE_PAREN,
        OPEN_SQUARE,
        CLOSE_SQUARE,
        LOWERCASE,
        ZERO,
        NULLNESS
    };

    Expect expected = LOWERCASE;
    bool isInColumnDef = false; // is it in the process of column definition
                                // for a table?
    bsl::string::const_iterator i = spec.begin();
    for (++i; i != spec.end(); ++i) {

        // start from the 1st (not 0th) character
        switch(expected) {
          case OPEN_PAREN: {
            ASSERT('(' == *i);
            // see current stack top:
            // if it's table, then open a row and place it on stack
            // if it's row, then look at its current index and theList
            // assignment at this position and place list on stack
            // if it's list, then appendList
            Container& currentContainer = containerStack.top();

            switch (currentContainer.type()) {
              case Container::IS_TABLE: {
                // this '(' means a new row
                bdem_Table *currentTable = currentContainer.table();
                ASSERT(currentTable);

                if (-1 == currentContainer.currentIndex()) {
                    int elemTypeArraySize =
                          currentContainer.elemTypes().size();
                    EType::Type *elemTypeArray =
                                            new EType::Type[elemTypeArraySize];
                    for (int idx = 0; idx < elemTypeArraySize; ++idx) {
                        elemTypeArray[idx] = currentContainer.elemTypes()[idx];
                    }

                    currentTable->reset(elemTypeArray, elemTypeArraySize);

                    delete [] elemTypeArray;
                    isInColumnDef = false;
                }
                currentTable->appendNullRow();
                currentContainer.incrementCurrentIndex();
                int indexOfTheRowInTable = currentContainer.currentIndex();
                ASSERT(indexOfTheRowInTable == currentTable->numRows() - 1);

                Container newContainer( // newContainer is the new row
                    &((*currentTable).theModifiableRow(indexOfTheRowInTable)));
                containerStack.push(newContainer);
              } break;
              case Container::IS_ROW: {
                // this '(' means a new list
                bdem_Row *currentRow = currentContainer.row();
                ASSERT(currentRow);

                currentContainer.incrementCurrentIndex();
                int indexOfTheListInRow = currentContainer.currentIndex();
                ASSERT(
                     EType::BDEM_LIST ==
                                    currentRow->elemType(indexOfTheListInRow));

                Container newContainer( // newContainer is the empty list
                    &(currentRow->theModifiableList(indexOfTheListInRow)));
                containerStack.push(newContainer);
              }
              case Container::IS_LIST: {
                // this '(' means a new list
                bdem_List *currentList = currentContainer.list();
                ASSERT(currentList);

                currentList->appendNullList();
                currentContainer.incrementCurrentIndex();
                int indexOfTheListInList = currentContainer.currentIndex();

                Container newContainer( // newContainer is the empty list
                    &(currentList->theModifiableList(indexOfTheListInList)));
                containerStack.push(newContainer);
              } break;
              default:
                ASSERT("Should *not* reach here" && 0);
            }
            expected = LOWERCASE;
          } break;
          case LOWERCASE: {
            switch (*i) {
              case '0': { // this field is unset
                --i; // roll back
                expected = ZERO;
              } break;
              case 'N': { // this field is null
                --i; // roll back
                expected = NULLNESS;
              } break;
              case '(': {
                --i; // roll back
                expected = OPEN_PAREN;
              } break;
              case ')': {
                --i; // roll back
                expected = CLOSE_PAREN;
              } break;
              case '[': {
                --i; // roll back
                expected = OPEN_SQUARE;
              } break;
              case ']': {
                --i; // roll back
                expected = CLOSE_SQUARE;
              } break;
              default: {
                EType::Type currentBdemType = gType(*i);
                Container& currentContainer = containerStack.top();

                if (isInColumnDef) {
                    // add this type to the column of the current table on
                    // the top of stack
                    ASSERT(currentContainer.type() == Container::IS_TABLE);

                    currentContainer.pushBackElemType(currentBdemType);
                }
                else { // !isInColumnDef
                    // add this field to the current container (list or
                    // row but not table)
                    ASSERT(currentContainer.type() != Container::IS_TABLE);

                    if (EType::isScalarType(currentBdemType)) {
                        if (currentContainer.type() == Container::IS_LIST) {
                            bdem_List& theList = *(currentContainer.list());

                            VALUES[*i - 'a'].appendToList(&theList);
                            currentContainer.incrementCurrentIndex();

                            ASSERT(currentContainer.currentIndex() ==
                                                         theList.length() - 1);
                        }
                        else { // IS_ROW
                            bdem_Row& theRow = *(currentContainer.row());
                            currentContainer.incrementCurrentIndex();
                            // must increment index before using it
                            int currentIndex = currentContainer.currentIndex();
                            ASSERT(currentIndex < theRow.length());
                            VALUES[*i - 'a'].addToRow(&theRow, currentIndex);
                        }
                        expected = LOWERCASE;
                    }
                    else if (EType::isArrayType(currentBdemType)
                            && ! EType::isAggregateType(currentBdemType)) {
                        if (currentContainer.type() == Container::IS_LIST) {
                            bdem_List& theList = *(currentContainer.list());

                            VALUES[*i - 'a'].appendArrayToList(&theList);
                            currentContainer.incrementCurrentIndex();

                            ASSERT(currentContainer.currentIndex() ==
                                                         theList.length() - 1);
                        }
                        else { // IS_ROW
                            bdem_Row& theRow = *(currentContainer.row());
                            currentContainer.incrementCurrentIndex();
                            // must increment index before using it
                            int currentIndex = currentContainer.currentIndex();
                            ASSERT(currentIndex < theRow.length());
                            VALUES[*i - 'a'].addArrayToRow(&theRow,
                                                           currentIndex);
                        }
                        expected = LOWERCASE;
                    }
                    else {
                        ASSERT(EType::isAggregateType(currentBdemType));
                        if (currentContainer.type() == Container::IS_LIST) {
                            bdem_List& theList = *(currentContainer.list());

                            if (EType::BDEM_LIST == currentBdemType) {
                                theList.appendNullList();
                            }
                            else {
                                theList.appendNullTable();
                            }
                            currentContainer.incrementCurrentIndex();

                            ASSERT(currentContainer.currentIndex() ==
                                   theList.length() - 1);
                        }
                        else { // IS_ROW
                            bdem_Row& theRow = *(currentContainer.row());
                            currentContainer.incrementCurrentIndex();
                            // must increment index before using it
                            int currentIndex = currentContainer.currentIndex();
                            ASSERT(currentIndex < theRow.length());
                            ASSERT(theRow.elemType(currentIndex) ==
                                   currentBdemType);
                            // don't need to unset the list or table
                        }
                        // unless 'u' or 'v' is followed by '0', they
                        // can't appear when !isInColumnDef
                        if (*(i + 1) == '0') {
                            expected = ZERO;
                        }
                        else {
                            expected = NULLNESS;
                        }
                    }
                }
              }
            }
          } break;
          case ZERO: {
            ASSERT('0' == *i);
            ASSERT(!isInColumnDef);
            Container& currentContainer = containerStack.top();
            ASSERT(currentContainer.type() != Container::IS_TABLE);
            // null current element
            int currentIndex = currentContainer.currentIndex();
            if (Container::IS_LIST == currentContainer.type()) {
                bdem_List& theList = *(currentContainer.list());
                theList[currentIndex].makeNull();
            }
            else { // IS_ROW
                bdem_Row& theRow = *(currentContainer.row());
                theRow[currentIndex].makeNull();
            }
            expected = LOWERCASE;
          } break;
          case NULLNESS: {
            ASSERT('N' == *i);
            Container& currentContainer = containerStack.top();
            ASSERT(currentContainer.type() != Container::IS_TABLE);
            // null current element
            int currentIndex = currentContainer.currentIndex();
            if (Container::IS_LIST == currentContainer.type()) {
                bdem_List& theList = *(currentContainer.list());
                theList[currentIndex].makeNull();
            }
            else { // IS_ROW
                bdem_Row& theRow = *(currentContainer.row());
                theRow[currentIndex].makeNull();
            }
            expected = LOWERCASE;
          } break;
          case CLOSE_PAREN: {
            ASSERT(')' == *i);
            ASSERT(!isInColumnDef);
            // pop current stack top (must be a row or a list, not a table)
            Container& currentContainer = containerStack.top();
            ASSERT(currentContainer.type() != Container::IS_TABLE);
            containerStack.pop();
            expected = LOWERCASE;
          } break;
          case OPEN_SQUARE: {
            ASSERT('[' == *i);
            ASSERT(!isInColumnDef);
            Container& currentContainer = containerStack.top();
            // current stack top can't be a table
            ASSERT(currentContainer.type() != Container::IS_TABLE);
            // prepare a new table on the stack

            if (Container::IS_LIST == currentContainer.type()) {
                bdem_List& theList = *(currentContainer.list());
                theList.appendNullTable();
                currentContainer.incrementCurrentIndex();
                int currentIndex = currentContainer.currentIndex();
                ASSERT(theList.length() - 1 == currentIndex);
                Container newContainer(&(theList.theModifiableTable(
                                                               currentIndex)));
                containerStack.push(newContainer);
            }
            else { // IS_ROW
                bdem_Row& theRow = *(currentContainer.row());
                currentContainer.incrementCurrentIndex();
                int currentIndex = currentContainer.currentIndex();
                // Do nothing.  theRow.theTable(currentIndex) is already
                // there, although null
                ASSERT(theRow.elemType(currentIndex) == EType::BDEM_TABLE);
                ASSERT(theRow[currentIndex].isNull());
                Container newContainer(&(theRow.theModifiableTable(
                                                               currentIndex)));
                containerStack.push(newContainer);
            }
            isInColumnDef = true;
            expected = LOWERCASE;
          } break;
          case CLOSE_SQUARE: {
            ASSERT(']' == *i);
            Container& currentContainer = containerStack.top();
            ASSERT(Container::IS_TABLE == currentContainer.type());

            if (-1 == currentContainer.currentIndex()) {
                // this table does not have any row, need to finish column
                // type definition here
                int elemTypeArraySize = currentContainer.elemTypes().size();
                EType::Type *elemTypeArray =
                                            new EType::Type[elemTypeArraySize];
                for (int idx = 0; idx < elemTypeArraySize; ++idx) {
                    elemTypeArray[idx] = currentContainer.elemTypes()[idx];
                }

                bdem_Table *currentTable = currentContainer.table();
                ASSERT(currentTable);

                currentTable->reset(elemTypeArray, elemTypeArraySize);

                delete [] elemTypeArray;
                isInColumnDef = false;
            }

            containerStack.pop();
            isInColumnDef = false;
            expected = LOWERCASE;
          } break;
          default:
            ASSERT("Should *not* reach here" && 0);
        }
    }
}

// Synopsis:
// valid examples: A , A{} , A{B}, A{BaCDbT{OPcQd}} ,
//                 A{B{CcDE{FGhK}E{FiGhKk}}Lm} ,
// invalid examples: Aa , A{a}, AB
// caret '^' may be present in the specification string
// * uppercase letters [A-Z]: names canned in NAMES.
//   - if followed by '{': there's nested element
//   - if followed by lowercase letter: there's only non-nested data
//   - if followed by other uppercase letter: the element is empty: <element/>
//   - if followed by '}': the enclosing element is ended
//   - if it's the first letter in the script, it has to be either followed by
//         '{', or followed by nothing.  The last rule follows the notion that
//         the root element itself cannot contain data value directly, it must
//         at least have one level of nested elements.  This notion comes to
//         be because the bdem_Schema structure maps record name to the root
//         element, and any enclosed fields to nested elements.
// * {: indicates existence of nested element(s).
//   - must be followed by uppercase letters, but not by others
// * lowercase letters: textual values canned in TEXTVALUES
//   - must be followed by '}' or uppercase letters, but not by others
// * }: indicates the end of element enclosing others
//   - must be followed by '}' or uppercase letters, but not by others
// Return first caret's line position (0 if no caret is present)
int  gXml(bsl::string *xmlString, const bsl::string& spec,
          bool useAnonymousNames = false)
{
    ASSERT(bsl::isupper(spec[0]));

    int caret = -1;

    const char **NAMES  = useAnonymousNames ? ANONYMOUSNAMES : NAMEDNAMES;
    const int NUM_NAMES = useAnonymousNames ? NUM_ANONYMOUSNAMES
                                            : NUM_NAMEDNAMES;

    bsl::stack<char> elementStack;

    enum Expect {
        UPPERCASE,   // tag name
        LOWERCASE,   // textual values
        OPEN_CURLY,  // tag start
        CLOSE_CURLY  // tag end
    };

    Expect expected = OPEN_CURLY; // tag start

    bsl::ostringstream oss;
    baexml_Formatter formatter(oss, 0, 4, INT_MAX);
    formatter.addHeader();
    formatter.openElement(NAMES[spec[0] - 'A']);
    formatter.addAttribute("xmlns", TESTNAMESPACE);

    elementStack.push(spec[0]);

    bsl::string::const_iterator i = spec.begin();
    for (++i; i != spec.end(); ++i) {
        switch (expected) {
          case OPEN_CURLY: {
            // prepare for nested element
            if ('^' == *i && -1 == caret) {
                caret = oss.str().length();
                expected = OPEN_CURLY;
            }
            else {
                ASSERT('{' == *i);
                formatter.flush();
                expected = UPPERCASE;
            }
          } break;
          case UPPERCASE: {
            if ('^' == *i && -1 == caret) {
                caret = oss.str().length();
                expected = UPPERCASE;
            }
            else if ('}' == *i) {
                // close enclosing element
                char currentElement = elementStack.top();
                formatter.closeElement(NAMES[currentElement - 'A']);
                elementStack.pop();
                expected = UPPERCASE;
            }
            else {
                // open new element for current i
                ASSERT(bsl::isupper(*i));
                formatter.openElement(NAMES[*i - 'A']);
                elementStack.push(*i);
                expected = LOWERCASE;
            }
          } break;
          case LOWERCASE: {
            if ('^' == *i && -1 == caret) {
                caret = oss.str().length();
                expected = LOWERCASE;
            }
            else if ('{' == *i) {
                // prepare for more nested element(s)
                formatter.flush();
                expected = UPPERCASE;
            }
            else if (bsl::isupper(*i)) {
                // close the empty element
                // open new element for current i
                char currentElement = elementStack.top();
                formatter.closeElement(NAMES[currentElement - 'A']);
                elementStack.pop();

                formatter.openElement(NAMES[*i - 'A']);
                elementStack.push(*i);
                expected = LOWERCASE;
            }
            else if ('}' == *i) {
                // close the empty element
                char currentElement = elementStack.top();
                formatter.closeElement(NAMES[currentElement - 'A']);
                elementStack.pop();
                // close enclosing element
                currentElement = elementStack.top();
                formatter.closeElement(NAMES[currentElement - 'A']);
                elementStack.pop();
                expected = UPPERCASE;
            }
            else { // lowercase letters
                char currentElement = elementStack.top();
                if (*i != 'q') {  // STRING_ARRAY data need to be duplicated
                                  // REPEAT times.
                    formatter.addData(TEXTVALUES[*i - 'a']);
                    // close this element with just added data
                    formatter.closeElement(NAMES[currentElement - 'A']);
                }
                else { // 'q', STRING_ARRAY data "hello"
                    for (int idx = 0; idx < ScalarData::REPEAT; ++idx) {
                        formatter.addData(TEXTVALUES[*i - 'a']);
                        formatter.closeElement(NAMES[currentElement - 'A']);
                        if (idx < ScalarData::REPEAT - 1) {
                            formatter.openElement(NAMES[currentElement - 'A']);
                        }
                    }
                }
                elementStack.pop();
                expected = UPPERCASE;
            }
          } break;
          default:
            ASSERT("Should *not* reach here" && 0);
        }
    }

    if (spec.length() == 1) {
        char currentElement = elementStack.top();
        formatter.closeElement(NAMES[currentElement - 'A']);
        elementStack.pop();
    }
    ASSERT(0 == elementStack.size());

    *xmlString = oss.str();

    return -1 < caret ? bsl::count(xmlString->begin(),
                                   xmlString->begin() + caret, '\n') + 1
                      : 0;
}

//=============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
//=============================================================================
struct Data {
    int         d_line;
    const char *d_bdemSchemaSpec;
    const char *d_xmlSpec;
    const char *d_expectedListSpec;
};

void testData(const Data DATA[], int NUM_DATA,
              int veryVerbose, int veryVeryVerbose)
    // Driver for test Data array.  Generate bdem_Schema and expected
    // bdem_List, and xml document.  Parse the document and get a bdem_List
    // and assert the list binds to the last record in the schema and is the
    // same as the expected bdem_List.
{
    for (int i = 0; i < NUM_DATA; ++i) {
        const int LINE = DATA[i].d_line;
        const bsl::string SCHEMASPEC = DATA[i].d_bdemSchemaSpec;
        const bsl::string XMLSPEC    = DATA[i].d_xmlSpec;
        const bsl::string LISTSPEC   = DATA[i].d_expectedListSpec;

        bdem_Schema schema;
        bcema_SharedPtr<bdem_Schema>  schemaPtr(
                                         &schema,
                                         bcema_SharedPtrNilDeleter(),
                                         0);

        gBdemSchema(&schema, SCHEMASPEC);

        bsl::string xml;
        if ("" != XMLSPEC) {
            gXml(&xml, XMLSPEC);
        }

        bdem_List expectedList;
        if ("" != LISTSPEC) {
            gBdemList(&expectedList, LISTSPEC);
        }

        if (veryVerbose) {
            P(LINE);
            P(schema);
            P(xml);
        }

        if ("" != LISTSPEC) {
            // make sure the expected list is correct
            ASSERT(SAU::isListConformant(expectedList,
                                      schema.record(schema.numRecords() - 1)));

            bsl::istringstream xmlStream(xml);

            bcem_Aggregate  aggregate;
            ErrorInfo       errInfo;
            Reader          reader;

            int status = baexml_DataUtil::createAggregateFromDocument(
                        &aggregate,
                        xmlStream.rdbuf(),
                        schemaPtr,
                        &reader,
                        &errInfo,
                        &bsl::cerr);

            const bdem_List& LIST = aggregate.asElemRef().theList();
            int index = aggregate.recordDef().recordIndex();

            if (veryVerbose) { P(LINE);  P(aggregate);
                               P(expectedList);  P(LIST); }

            LOOP_ASSERT(LINE, status >= 0);
            LOOP3_ASSERT(LINE, LIST, expectedList, expectedList == LIST);
            if (status >= 0) {
                LOOP_ASSERT(LINE, index == schema.numRecords() - 1);
            }
            else if (veryVerbose) {
                bsl::cout << errInfo;
            }
        }
    }
}

//---------------------------------------------------------------------------
// Create Aggregate for given XML document 'xmlData'
// and given XSD schema 'xmlSchema'.
// The parameter 'flags' defines variety of parameters
// for the 'baexml_DataUtil::createAggregateFromDocument'
//---------------------------------------------------------------------------

int doTest(bcem_Aggregate   *aggregate,
           const char       *xmlData,
           const char       *xmlSchema,
           bslma_Allocator  *allocator,
           int               flags)

{
    aggregate->reset();

    bdesb_FixedMemInStreamBuf xmlDataStream(xmlData,
                                            bsl::strlen(xmlData));
    bdesb_FixedMemInStreamBuf xmlSchemaStream(xmlSchema,
                                              bsl::strlen(xmlSchema));

    baexml_ErrorInfo      errorInfo;
    baexml_MiniReader     reader;
    TestValidatingReader  vreader(&reader);
    baexml_DecoderOptions options;

    options.setSkipUnknownElements((flags & TF_SKIP_UNK_EL) ? true : false);

    baexml_ErrorInfo *parmErrInfo = (flags & TF_ERR_INFO) ? &errorInfo : 0;

    bsl::ostream     *parmDiagStream =
        ((flags & TF_DIAG_STREAM) && veryVerbose) ? &bsl::cout : 0;

    int rc = 0;

    if (flags & TF_VALIDATION) {

        if (flags & TF_OPTIONS) {

            rc = baexml_DataUtil::createAggregateFromDocument(
                        aggregate,
                        &xmlDataStream,
                        &xmlSchemaStream,
                        &vreader,
                        &options,
                        parmErrInfo,
                        parmDiagStream,
                        allocator);
        }
        else {
            rc = baexml_DataUtil::createAggregateFromDocument(
                        aggregate,
                        &xmlDataStream,
                        &xmlSchemaStream,
                        &vreader,
                        parmErrInfo,
                        parmDiagStream,
                        allocator);
        }
    }
    else {
        if (flags & TF_OPTIONS) {

            rc = baexml_DataUtil::createAggregateFromDocument(
                        aggregate,
                        &xmlDataStream,
                        &xmlSchemaStream,
                        &reader,
                        &options,
                        parmErrInfo,
                        parmDiagStream,
                        allocator);
        }
        else {
            rc = baexml_DataUtil::createAggregateFromDocument(
                        aggregate,
                        &xmlDataStream,
                        &xmlSchemaStream,
                        &reader,
                        parmErrInfo,
                        parmDiagStream,
                        allocator);
        }
    }
    return rc;
}

//---------------------------------------------------------------------------
// Create Aggregate for given XML document 'xmlData'
// and given BDEM schema 'bdemSchema'.
// The parameter 'flags' defines variety of parameters
// for the 'baexml_DataUtil::createAggregateFromDocument'
//---------------------------------------------------------------------------
int doTest2(bcem_Aggregate                      *aggregate,
            const char                          *xmlData,
            const bcema_SharedPtr<bdem_Schema>&  bdemSchema,
            bslma_Allocator                     *allocator,
            int                                  flags)

{
    aggregate->reset();

    bdesb_FixedMemInStreamBuf xmlDataStream(xmlData, bsl::strlen(xmlData));

    baexml_ErrorInfo      errorInfo;
    baexml_MiniReader     reader;
    baexml_DecoderOptions options;

    options.setSkipUnknownElements(
                (flags & TF_SKIP_UNK_EL) ? true : false);

    baexml_ErrorInfo *parmErrInfo =
        (flags & TF_ERR_INFO) ? &errorInfo : 0;

    bsl::ostream     *parmDiagStream =
        ((flags & TF_DIAG_STREAM) && veryVerbose) ? &bsl::cout : 0;

    int rc = 0;

    if (flags & TF_OPTIONS) {

        rc = baexml_DataUtil::createAggregateFromDocument(
                    aggregate,
                    &xmlDataStream,
                    bdemSchema,
                    &reader,
                    &options,
                    parmErrInfo,
                    parmDiagStream,
                    allocator);
    }
    else {
        rc = baexml_DataUtil::createAggregateFromDocument(
                    aggregate,
                    &xmlDataStream,
                    bdemSchema,
                    &reader,
                    parmErrInfo,
                    parmDiagStream,
                    allocator);
    }
    return rc;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------

bcema_SharedPtr<bdem_Schema> buildBdemSchema(
                const char         *xmlSchema,
                bslma_Allocator    *allocator,
                int                 flags)
{
    bdesb_FixedMemInStreamBuf xmlSchemaStream(xmlSchema,
                                              bsl::strlen(xmlSchema));

    baexml_ErrorInfo      errorInfo;
    baexml_MiniReader     reader;

    baexml_ErrorInfo *parmErrInfo =
        (flags & TF_ERR_INFO) ? &errorInfo : 0;

    bsl::ostream     *parmDiagStream =
        ((flags & TF_DIAG_STREAM) && veryVerbose) ? &bsl::cout : 0;

    //
    // Create a BDEM schema on heap and keep it by shared pointer
    //
    bcema_SharedPtr<bdem_Schema> outSchemaPtr;
    outSchemaPtr.createInplace(allocator, allocator);

    //
    // Parse XSD schema and load result into BDEM schema
    //
    baexml_SchemaParser schemaParser(&reader,
                                     parmErrInfo,
                                     parmDiagStream);

    bsl::string targetNamespace;

    int rc = schemaParser.parse(&xmlSchemaStream,
                                outSchemaPtr.ptr(),
                                &targetNamespace,
                                "Schema");

    if (rc != 0) {

        if (parmDiagStream && parmErrInfo) {

            *parmDiagStream << *parmErrInfo << bsl::endl;
        }

        outSchemaPtr.clear();
    }
    return outSchemaPtr;
}
//---------------------------------------------------------------------
//   Wrapper around doTest.  It calls doTest and doTest2
//   in the loop with different flags:
//   with/without parameter ErrorInfo,
//   with/without parameter DiagnosticStream,
//   with baexml_Reader and with baexml_ValidatingReader,
//   with/without parameter DecoderOptions,
//   with/without bslma_Allocator.
//   Ensure the results for all calls are identical
// --------------------------------------------------------------------
int doAllTests(bcem_Aggregate   *aggregate,
               const char       *xmlData,
               const char       *xmlSchema,
               bslma_Allocator  *allocator)
{
    // Don't let 'allocator' change after start of this function.
    allocator = bslma_Default::allocator(allocator);

    bslma_TestAllocator da(veryVeryVerbose);
    bslma_DefaultAllocatorGuard guard(&da);

    int rc = 0;
    rc = doTest(aggregate, xmlData, xmlSchema, allocator, 0);
    ASSERT(0 == da.numBytesInUse());

    int maxFlags = TF_ERR_INFO |
                   TF_DIAG_STREAM |
                   TF_VALIDATION |
                   TF_OPTIONS |
                   TF_SKIP_UNK_EL;

    for (int i=1; i < maxFlags; ++i) {

        rc = doTest(aggregate, xmlData, xmlSchema, allocator, i);
        ASSERT(rc == rc);
        ASSERT(0 == da.numBytesInUse());

        bcema_SharedPtr<bdem_Schema> bdemSchema =
            buildBdemSchema(xmlSchema, allocator, i);

        ASSERT(bdemSchema);
        rc = doTest2(aggregate, xmlData, bdemSchema, allocator, i);
        ASSERT(rc == rc);
        ASSERT(0 == da.numBytesInUse());
    }

    return rc;
}

}  // close unnamed namespace

//-----------------------------------------------------------------------------
//                         CASE 2 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase2 {
const char SCHEMA[] =
    "<?xml version='1.0' encoding='UTF-8' ?>\n"
    "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
    "    xmlns:emp='http://bloomberg.com/schemas/employee'\n"
    "    targetNamespace='http://bloomberg.com/schemas/employee'\n"
    "    elementFormDefault='qualified'>\n"
    "    <xs:include schemaLocation='location1'/>\n"
    "    <xs:element name='employee' type='emp:EmployeeType'/>\n"
    "</xs:schema>\n";

const char SCHEMA_INC[] =
    "<?xml version='1.0' encoding='UTF-8' ?>\n"
    "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
    "    elementFormDefault='qualified'>\n"
    "    <xs:complexType name='EmployeeType'>\n"
    "        <xs:sequence>\n"
    "            <xs:element name='name' type='xs:string'/>\n"
    "            <xs:element name='salary' type='xs:int'/>\n"
    "            <xs:element name='married' type='xs:boolean'/>\n"
    "        </xs:sequence>\n"
    "    </xs:complexType>\n"
    "</xs:schema>\n";

const char DATA[] =
    "<?xml version='1.0' encoding='UTF-8' ?>\n"
    "<employee xmlns='http://bloomberg.com/schemas/employee'\n"
    "     xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>\n"
    "        <name>TOM</name>\n"
    "        <salary>100</salary>\n"
    "        <married> true </married>\n"
    "</employee>\n";

class ResolverFunctor
{
  public:
    typedef baexml_Reader::StreamBufPtr    StreamBufPtr;

    StreamBufPtr  operator()(const char *schemaLocation,
                             const char *targetNamespace)
    {

        StreamBufPtr ret;

        if (bsl::strcmp(schemaLocation,  "location1") == 0) {
            ret.load(new bdesb_FixedMemInStreamBuf(SCHEMA_INC,
                                                   sizeof(SCHEMA_INC) - 1));
        }

        return ret;
    }
};

}  // close namespace TestCase2
//=============================================================================
//                         CASE 1 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase1 {
const char SCHEMA[] =
    "<?xml version='1.0' encoding='UTF-8' ?>\n"
    "<schema xmlns='http://www.w3.org/2001/XMLSchema'\n"
    "    targetNamespace='http://bloomberg.com/schemas/employee'\n"
    "    xmlns:emp='http://bloomberg.com/schemas/employee'\n"
    "    elementFormDefault='qualified'>\n"
    "    <complexType name='EmployeeType'>\n"
    "        <sequence>\n"
    "            <element name='name' type='string'/>\n"
    "            <element name='salary' type='int'/>\n"
    "            <element name='married' type='boolean'/>\n"
    "        </sequence>\n"
    "    </complexType>\n"
    "    <element name='employee' type='emp:EmployeeType'/>\n"
    "</schema>\n";

const char DATA[] =
    "<?xml version='1.0' encoding='UTF-8' ?>\n"
    "<employee xmlns='http://bloomberg.com/schemas/employee'\n"
    "     xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>\n"
    "        <name>TOM</name>\n"
    "        <salary>100</salary>\n"
    "        <married> true </married>\n"
    "</employee>\n";
}  // close namespace TestCase1

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nUSAGE EXAMPLE"
                      << "\n=============" << bsl::endl;
        }

// We begin with a simple XSD schema document that describes a basic gift
// registry.  This XSD schema specifies constraints that are not currently
// supported by the 'bdem' messaging package (e.g., <maxLength>,
// <minExclusive> etc.).  Whether or not these constraints are enforced
// depends on the type of XML reader used, as described below.
//..
    static const char xsdString[] =
        "<?xml version='1.0' encoding='UTF-8' ?>\n"
        "<schema xmlns='http://www.w3.org/2001/XMLSchema'\n"
        "    targetNamespace='http://bloomberg.com/schemas/giftRegistry'\n"
        "    xmlns:gr='http://bloomberg.com/schemas/giftRegistry'\n"
        "    elementFormDefault='qualified'>\n"
        "    <simpleType name='GiftDescription'>\n"
        "        <restriction base='string'>\n"
        "            <maxLength value='40'/>\n"
        "        </restriction>\n"
        "    </simpleType>\n"
        "    <simpleType name='PurchaseStatus'>\n"
        "        <restriction base='string'>\n"
        "            <enumeration value='Received'/>\n"
        "            <enumeration value='Ordered'/>\n"
        "            <enumeration value='NoAction'/>\n"
        "        </restriction>\n"
        "    </simpleType>\n"
        "    <simpleType name='Price'>\n"
        "        <restriction base='float'>\n"
        "            <minExclusive value='0.00'/>\n"
        "            <maxInclusive value='10000.00'/>\n"
        "        </restriction>\n"
        "    </simpleType>\n"
        "\n"
        "    <complexType name='GiftItem'>\n"
        "        <sequence>\n"
        "            <element name='name' type='string'/>\n"
        "            <element name='price' type='gr:Price'/>\n"
        "            <element name='description' type='gr:GiftDescription'/>\n"
        "            <element name='status' type='gr:PurchaseStatus'/>\n"
        "            <element name='purchaser' type='string'/>\n"
        "        </sequence>\n"
        "    </complexType>\n"
        "    <complexType name='Registry'>\n"
        "        <sequence>\n"
        "            <element name='createdBy' type='string'/>\n"
        "            <element name='creationDate' type='date'/>\n"
        "            <element name='giftItem' type='gr:GiftItem'\n"
        "                minOccurs='0' maxOccurs='unbounded'/>\n"
        "        </sequence>\n"
        "    </complexType>\n"
        "    <element name='registry' type='gr:Registry'/>\n"
        "</schema>\n";
//..
// Now we create an XML data document that conforms to (and references) the
// above schema.  Although the document specifies a 'schemaLocation' of
// "giftRegistry.xml", the 'createAggregateFromDocument' function will ignore
// this hint and instead use the explicitly-supplied schema document.
//..
    const char xmlString[] =
        "<?xml version='1.0' encoding='UTF-8' ?>\n"
        "<registry xmlns='http://bloomberg.com/schemas/giftRegistry'\n"
        "     xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'\n"
        "     xsi:schemaLocation='http://bloomberg.com/schemas/giftRegistry "
        "giftRegistry.xsd'>\n"
        "    <createdBy>Garfield The FatCat</createdBy>\n"
        "    <creationDate>2004-11-24</creationDate>\n"
        "    <giftItem>\n"
        "        <name>turkey flavored donuts</name>\n"
        "        <price>4.97</price>\n"
        "        <description>Only available at Drunken Donuts</description>\n"
        "        <status>Received</status>\n"
        "        <purchaser>Odie</purchaser>\n"
        "    </giftItem>\n"
        "    <giftItem>\n"
        "        <name>King size cat bed</name>\n"
        "        <price>299.99</price>\n"
        "        <description>Please go to Beds R Cats</description>\n"
        "        <status>Ordered</status>\n"
        "        <purchaser>Jon</purchaser>\n"
        "    </giftItem>\n"
        "</registry>\n";
//..
// In order to parse the schema and data documents, we must present them as
// streams.  The 'bdesb_FixedMemInStreamBuf' class is well suited for
// presenting a fixed-length string as a stream:
//..
    bdesb_FixedMemInStreamBuf xmlDoc(xmlString, bsl::strlen(xmlString));
    bdesb_FixedMemInStreamBuf xsdDoc(xsdString, bsl::strlen(xsdString));
//..
// Now we must create a reader object, an aggregate object to receive the
// output, and an error object to capture error messages.  In this example we
// are using 'baexml_MiniReader', which is a fast, but non-validating reader.
// The XSD constraints such as <maxLength>, <minExclusive> are ignored by
// non-validating readers.  If we were to use a validating reader instead,
// then the XML data would be validated against the original schema, enforcing
// all of the constraints.
//..
    baexml_MiniReader reader;
    bcem_Aggregate    aggregate;
    baexml_ErrorInfo  errInfo;
//..
// Using the XSD schema and XML document as input, we use 'baexml_DataUtil' to
// populate the aggregate object:
//..
    int status = baexml_DataUtil::createAggregateFromDocument(&aggregate,
                                                              &xmlDoc,
                                                              &xsdDoc,
                                                              &reader,
                                                              &errInfo);

    ASSERT(0 == status);
    if (status) {
        bsl::cerr << "Data error: " << errInfo;
        return -2;
    }
//..
// Finally, we verify that the aggregate contains the data from the XML
// document:
//..
    ASSERT(aggregate["createdBy"].asString() == "Garfield The FatCat");
    ASSERT(aggregate["creationDate"].asDateTz().localDate() ==
           bdet_Date(2004,11,24));
    ASSERT(aggregate["giftItem"].length() == 2);

    bcem_Aggregate item = aggregate["giftItem"][0];
    ASSERT(item["name"].asString() == "turkey flavored donuts");
    ASSERT(bsl::fabs(item["price"].asFloat() - 4.97) < 0.001);
    ASSERT(item["description"].asString()=="Only available at Drunken Donuts");
    ASSERT(item["status"].asInt() == 2); // Received
    ASSERT(item["purchaser"].asString() == "Odie");

    item = aggregate["giftItem"][1];
    ASSERT(item["name"].asString() == "King size cat bed");
    ASSERT(bsl::fabs(item["price"].asFloat() - 299.99) < 0.001);
    ASSERT(item["description"].asString()=="Please go to Beds R Cats");
    ASSERT(item["status"].asInt() == 1); // Ordered
    ASSERT(item["purchaser"].asString() == "Jon");
//..

    if (verbose) P(aggregate);

    if (veryVerbose) {
        P(xsdString) P(xmlString)
    }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING Parsing of elements recursive constrained table fields (set
        // or unset)
        //
        // Plan:
        //   Create bdem_Schema's of records that contain recursive list
        //   fields, which self-reference the enclosing record, as well as
        //   cross-reference other records.  Create a baexml_DataParser for
        //   this
        //   schema and parse XML documents containing such elements.  Compare
        //   the result with expected output.
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "TEST generation of recursive constrained list"
                      << bsl::endl;
        }

        static Data DATA[] = {
{ L_, "A{BcCvA}",       "A{Bc}",                "(c[cv])"                    },
{ L_, "A{BcCvA}",       "A{BcC{BcC{Bc}}C{Bc}}",
      "(c[cv(c[cv(c[cv])])(c[cv])])"                                         },
{ L_, "A{BcCvA}",       "A{BcC{BcC{BcC{Bc}}}}",
      "(c[cv(c[cv(c[cv(c[cv])])])])"                                         },
{ L_, "A{Bc}D{EvAFvD}", "D{E{Bc}F{E{Bc}F{F{E{Bc}}F{F}}}}",
      "([c(c)][vv([c(c)][vv([c][vv([c(c)][vv])([c][vv([c][vv])])])])])"      },
{ L_, "A{Bc}D{EvA}F{GvF}", "F{G{G}}",           "([v([v([v])])])"            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        testData(DATA, NUM_DATA, veryVerbose, veryVeryVerbose);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING Parsing of elements representing unset constrained table
        // fields.
        //
        // Plan:
        //   Create bdem_Schema's of records that contain non-recursive table
        //   fields that cross reference records.  Create a baexml_DataParser
        //   for this schema and parse XML documents containing no such
        //   elements.  Notice the absence of such elements really indicates
        //   unset table fields in the bdem_List that's bound to that schema.
        //   Compare the result with expected output.
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "TEST parsing of XML documents without certain "
                      << "elements that are supposed to represent tables. "
                      << "This produces a bdem_List with unset "
                      << "table fields" << bsl::endl;
        }

        static Data DATA[] = {
{ L_, "A{Bc}D{EvA}",            "D",             "([c])"                     },
{ L_, "A{Bc}D{EvAFvA}",         "D",             "([c][c])"                  },
{ L_, "A{Bc}D{EvA}F{GvD}",      "F",             "([v])"                     },
{ L_, "A{Bc}D{EvA}F{GvD}",      "F{G}",          "([v([c])])"                },
{ L_, "A{Bc}D{EdFvAGe}",        "D{EdGe}",       "(d[c]e)"                   },
{ L_, "A{Bc}D{EdFvA}G{HvDIe}",  "G{Ie}",         "([dv]e)"                   },
{ L_, "A{Bc}D{EdFvA}G{HvDIe}",  "G{H{Ed}H{EdF{Bc}}Ie}",
                                "([dv(d[c])(d[c(c)])]e)"                     },
{ L_, "A{Bo}D{Ep@FLFvA}G{HvDIq}J{KvG}", "J{K{H{Ep}Iq}K{Iq}}",
                                     "([vq([pv(p[o])]q)([pv]q)])"            },
{ L_, "A{Bo}D{EpFvA}G{HvDIq}J{KvG}", "J",        "([vq])"                    },
{ L_, "A{Bo}D{EpFvA}G{HvDIq}J{KvG}", "J{K{Iq}}", "([vq([pv]q)])"             },
{ L_, "A{Bo}D{EpFvA}G{HvDIq}J{KvG}", "J{K}",     "([vq([pv]qN)"              },
{ L_, "A{BbCc}D{EvAFdGvAHeIf}G{HvAIgKvD}L{MvGNvDOhPiQj}",
      "L{M{H{BbCc}IgK{FdHeIf}}OhPiQj}",
      "([vgv([bc(bc)]g[vdvef([bc]d[bc]ef)])][vdvef]hij)"                     },
{ L_, "A{BbCc}D{EvAFdGvAHeIf}G{HvAIgKvD}L{MvGNvDOhPiQj}",
      "L{M{H{BbCc}Ig}OhPiQj}",
      "([vgv([bc(bc)]g[vdvef])][vdvef]hij)"                                  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        testData(DATA, NUM_DATA, veryVerbose, veryVeryVerbose);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING Parsing of elements representing non-unset constrained
        // table fields
        //
        // Plan:
        //   Create bdem_Schema's of records that contain non-recursive table
        //   fields that cross reference one another.  Create a
        //   'baexml_DataParser' for this schema and parse XML documents
        //   containing such elements.  Compare the result with expected
        //   output.
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "TEST parsing of elements representing"
                      << " non-unset constrained table" << bsl::endl;
        }

        static Data DATA[] = {
{ L_, "A{Bc}D{EvA}",        "D{E{Bc}E{Bc}E{Bc}E{Bc}}", "([c(c)(c)(c)(c)])"   },
{ L_, "A{Bc}D{EvAFvA}",     "D{E{Bc}E{Bc}F{Bc}F{Bc}F{Bc}}",
                            "([c(c)(c)][c(c)(c)(c)])"                        },
{ L_, "A{Bc}D{EvAFvA}",     "D{E{Bc}E{Bc}}",           "([c(c)(c)][c])"      },
{ L_, "A{Bc}D{EvA}F{GvD}",  "F{G{E{Bc}E{Bc}}G{E{Bc}E{Bc}E{Bc}}G{E{Bc}}}",
                            "([v([c(c)(c)])([c(c)(c)(c)])([c(c)])])"         },
{ L_, "A{Bc}D{EvA}F{GvD}",  "F{G{E{Bc}E{Bc}}G{E{Bc}}}",
                            "([v([c(c)(c)])([c(c)])]"                        },
{ L_, "A{Bc}D{EvA}F{GvA}",  "F{G{Bc}G{Bc}G{Bc}G{Bc}}", "([c(c)(c)(c)(c)])"   },
{ L_, "A{Bc}D{EdFvAGe}",    "D{EdF{Bc}F{Bc}F{Bc}Ge}",  "(d[c(c)(c)(c)]e)"    },
{ L_, "A{Bc}D{EdFvA}G{HvDIe}",
      "G{H{EdF{Bc}F{Bc}F{Bc}}H{EdF{Bc}}H{EdF{Bc}F{Bc}}Ie}",
      "([dv(d[c(c)(c)(c)])(d[c(c)])(d[c(c)(c)])]e)"                          },
{ L_, "A{Bo@FL}D{Ep@FLFvA}G{HvDIq}J{KvG}",
      "J{K{H{Ep}H{EpF{Bo}}Iq}K{H{EpF{Bo}F{Bo}}}K{H{EpF{Bo}F{Bo}F{Bo}}"
      "H{Ep}H{EpF{Bo}F{Bo}}Iq}}",
      "([vq([pv(p[o])(p[o(o)])]q)([pv(p[o(o)(o)])]qN)"
      "([pv(p[o(o)(o)(o)])(p[o])(p[o(o)(o)])]q)])"                           },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        testData(DATA, NUM_DATA, veryVerbose, veryVeryVerbose);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING Parsing of elements representing recursive constrained list
        // fields (set or unset)
        //
        // Concerns:
        //   The innermost recursive field in the bdem_List is itself unset,
        //   rather than having its elements unset.
        // Plan:
        //   Create bdem_Schema's of records that contain recursive list
        //   fields, which self-reference the enclosing record, as well as
        //   cross-reference other records.  Create a baexml_DataParser for
        //   this schema and parse XML documents containing such elements.
        //   Compare the result with expected output.
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "TEST Parsing of elements representing "
                      << "recursive constrained list" << bsl::endl;
        }

        static Data DATA[] = {
{ L_, "A{BcCuA}",             "A{BcC}",               "(c(cNuN))"            },
{ L_, "A{Bc@NTCuA}",          "A{BcC}",               "(c(cNuN))"            },
{ L_, "A{BcCuA}",             "A{BcC{BcC}}",          "(c(c(cNuN)))"         },
{ L_, "A{Bc@NTCuA}",          "A{BcC{BcC}}",          "(c(c(cNuN)))"         },
{ L_, "A{BcCuA}",             "A{BcC{BcC{BcC}}}",     "(c(c(c(cNuN))))"      },
{ L_, "A{Bc@NTCuA}",          "A{BcC{BcC{BcC}}}",     "(c(c(c(cNuN))))"      },

{ L_, "A{Bc}D{EuAFuD}",       "D{E{Bc}F{E{Bc}F{E{Bc}F{E{Bc}F}}}}",
                              "((c)((c)((c)((c)((cN)uN)))))"                 },
{ L_, "A{Bc@NT}D{EuAFuD}",    "D{E{Bc}F{E{Bc}F{E{Bc}F{E{Bc}F}}}}",
                              "((c)((c)((c)((c)((cN)uN)))))"                 },
{ L_, "A{Bc}D{EuDFuA}",       "D{E{E{E{EF{Bc}}F{Bc}}F{Bc}}F{Bc}}",
                              "(((((uN(cN))(c))(c))(c))(c))"                 },
{ L_, "A{Bc@NT}D{EuDFuA}",    "D{E{E{E{EF{Bc}}F{Bc}}F{Bc}}F{Bc}}",
                              "(((((uN(cN))(c))(c))(c))(c))"                 },
{ L_, "A{Bc}D{EuA}F{GuF}",    "F{G{G}}",              "(((uN)))"             },
{ L_, "A{BcCuA}D{EuA}F{GuA}", "F{G{BcC{BcC{BcC}}}}",  "((c(c(c(cNuN)))))"    },
{ L_, "A{Bc@NTCuA}D{EuA}F{GuA}","F{G{BcC{BcC{BcC}}}}","((c(c(c(cNuN)))))"    },
{ L_, "D{EdFuDGe}",           "D{EdF{F{EdF{EdFGe}}Ge}Ge}",
                              "(d(dN(d(d(dNuNeN)e)eN)e)e)"                   },
{ L_, "D{Ed@NTFuDGe@NT}",     "D{EdF{F{EdF{EdFGe}}Ge}Ge}",
                              "(d(dN(d(d(dNuNeN)e)eN)e)e)"                   },
{ L_, "D{EdFuD}G{HuD@NTIeJuG}",
      "G{H{EdF{EdF}}IeJ{H{EdF{EdF}}IeJ{H{EdF{EdF{EdF{EdF}}}}IeJ}}}",
      "((d(d(dNuN)))e((d(d(dNuN)))e((d(d(d(d(dNuN)))))e(uNeNuN))))"          },
{ L_, "D{Ed@NTFuD}G{HuD@NTIe@NTJuG}",
      "G{H{EdF{EdF}}IeJ{H{EdF{EdF}}IeJ{H{EdF{EdF{EdF{EdF}}}}IeJ}}}",
      "((d(d(dNuN)))e((d(d(dNuN)))e((d(d(d(d(dNuN)))))e(uNeNuN))))"          },
       };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        testData(DATA, NUM_DATA, veryVerbose, veryVeryVerbose);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING Parsing of empty elements representing unset constrained
        // list fields
        //
        // Plan:
        //   Create bdem_Schema's of records that contain non-recursive list
        //   fields that cross reference records.  Create a baexml_DataParser
        //   for this schema.  Create XML document that represent such fields
        //   as either empty elements (<element/>), or the lack of which.
        //   Parse the document and compare the result with the expected
        //   output.
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "TEST parsing of empty elements represents"
                      << " unset constrained list" << bsl::endl;
        }

        static Data DATA[] = {
{ L_, "A{Bc}D{EuA}",                  "D{E}",            "((cN))"            },
{ L_, "A{Bc@NT}D{EuA}",               "D{E}",            "((cN))"            },
{ L_, "A{Bc@NT}D{EuA@Fn}",            "D{E}",            "(uN)"              },
{ L_, "A{Bc}D{EuAFuA}",               "D{E{Bc}F}",       "((c)(cN))"         },
{ L_, "A{Bc@NT}D{EuAFuA}",            "D{E{Bc}F}",       "((c)(cN))"         },
{ L_, "A{Bc@NT}D{EuAFuA@Fn}",         "D{E{Bc}F}",       "((c)uN)"           },
{ L_, "A{Bc}D{EuAFuA}",               "D{E{Bc}F{Bc}}",   "((c)(c))"          },
{ L_, "A{Bc@NT}D{EuAFuA}",            "D{E{Bc}F}",       "((c)(cN))"         },
{ L_, "A{Bc@NT}D{EuAFuA@Fn}",         "D{E{Bc}F}",       "((c)uN)"           },
{ L_, "A{Bc}D{EuA}F{GuD}",            "F{G{E}}",         "(((cN)))"          },
{ L_, "A{Bc}D{EuA@Fn}F{GuD}",         "F{G{E}}",         "((uN))"            },
{ L_, "A{Bc}D{EuA@NT}F{GuD@NT}",      "F{G}",            "((uN))"            },
{ L_, "A{Bc}D{EuA}F{GuD@Fn}",         "F{G}",            "(uN)"              },
{ L_, "A{Bc}D{EuA@NT}F{GuD@Fn}",      "F{G}",            "(uN)"              },

{ L_, "A{Bc}D{EuA@NT}F{GuD@NT}",      "F{}",             "(uN)"              },
{ L_, "A{Bc}D{EuA}F{GuD@NT}",         "F{}",             "(uN)"              },
{ L_, "A{Bc}D{EuA}F{GuD@NT@Fn}",      "F{}",             "(uN)"              },

{ L_, "A{Bc}D{EuA@NT}F{GuD@NT}",      "F",               "(uN)"              },
{ L_, "A{Bc}D{EdFuAGe}",              "D{EdFGe}",        "(d(cN)e)"          },
{ L_, "A{Bc}D{EdFuA}G{HuDIe}",        "G{H{EdF}Ie}",     "((d(cN))e)"        },
{ L_, "A{Bc}D{EdFuA@NT}G{HuD@NTIe}",  "G{HIe}",          "((dNuN)e)"         },
{ L_, "A{Bc}D{Ed@NTFuA@NT}G{HuD@NTIe}","G{HIe}",         "((dNuN)e)"         },
{ L_, "A{Bc}D{EdFuA@NT}G{HuD@NTIe}",  "G{Ie}",           "(uNe)"             },
{ L_, "A{Bc}D{Ed@NTFuA@NT}G{HuD@NTIe}", "G{Ie}",         "(uNe)"             },
{ L_, "A{Bo}D{Ep@FLFuA@NT}G{HuD@NTIq}J{KuG@NT}",
                                      "J{K{H{EpF}Iq}}",  "(((p(oN))q))"      },
{ L_, "A{Bo}D{Ep@FLFuA@NT}G{HuD@NTIq}J{KuG@NT}",
                                      "J{K{Iq}}",        "((uNq))"           },
{ L_, "A{Bo}D{EpFuA@NT}G{HuD@NTIq}J{KuG@NT}",
                                      "J{K{HIq}}",       "(((pNuN)q))"       },
{ L_, "A{Bo}D{EpFuA@NT}G{HuD@NTIq}J{KuG@NT}",
                                      "J{K}",            "((uNqN))"          },
{ L_, "A{Bo}D{EpFuA@NT}G{HuD@NTIq}J{KuG@NT}",
                                      "J{}",             "(uN)"              },
{ L_, "A{Bo}D{EpFuA@NT}G{HuD@NTIq}J{KuG@NT}",
                                      "J",               "(uN)"              },
{ L_, "A{BbCc}D{EuA@NTFdGuA@NTHeIf}G{HuA@NTIgKuD@NT}L{MuG@NTNuD@NTOhPiQj}",
      "L{M{H{BbCc}IgK{E{BbCc}FdG{BbCc}HeIf}}NOhPiQj}",
      "(((bc)g((bc)d(bc)ef))(uNdNuNeNfN)hij)"                                },
{ L_, "A{Bc}D{EdFuA@NT}G{HuD@NTIe}J{KfLuG@NT}M{NuJ@NTOg}P{QhRuM@NT}S{TuP@NTUi}"
      "V{WjXuS@NT}Y{ZuV@NTAk@FL}",
      "Y{Z{WjX{T{QhR}Ui}}Ak}",
      "((j((h(uNgN))i))k)"                                                   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        testData(DATA, NUM_DATA, veryVerbose, veryVeryVerbose);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING Parsing of non-empty elements representing constrained list
        // fields
        //
        // Plan:
        //   Create bdem_Schema's of records that contain non-recursive list
        //   fields that cross reference one another.  Create a
        //   baexml_DataParser for the schema.  Parse an XML document
        //   containing such elements, and compare the result against the
        //   expected bdem_List.
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "TEST Parsing of non-empty elements representing "
                      << "non-unset constrained list" << bsl::endl;
        }

        static Data DATA[] = {
{ L_, "A{Bc}D{EuA}",                 "D{E{Bc}}",           "((c))"           },
{ L_, "A{Bc}D{EuAFuA}",              "D{E{Bc}F}",          "((c)(cN))"       },
{ L_, "A{Bc@NT}D{EuAFuA}",           "D{E{Bc}F}",          "((c)(cN))"       },
{ L_, "A{Bc@NT}D{EuAFuA@Fn}",        "D{E{Bc}F}",          "((c)uN)"         },
                                                      // contain one unset LIST
{ L_, "A{Bc}D{EuA}F{GuD}",           "F{G{E{Bc}}}",        "(((c)))"         },
{ L_, "A{Bc}D{EuA}F{GuA}",           "F{G{Bc}}",           "((c))"           },
{ L_, "A{Bc}D{EdFuAGe}",             "D{EdF{Bc}Ge}",       "(d(c)e)"         },
{ L_, "A{Bc}D{EdFuA}G{HuDIe}",       "G{H{EdF{Bc}}Ie}",    "((d(c))e)"       },
{ L_, "A{Bo@FL}D{Ep@FLFuA}G{HuDIq}J{KuG}",
                                     "J{K{H{EpF{Bo}}Iq}}", "(((p(o))q))"     },
{ L_,
    "A{Bc}D{EdFuA}G{HuDIe}J{KfLuG}M{NuJOg}P{QhRuM}S{TuPUi}V{WjXuS}Y{ZuVAk@FL}",
      "Y{Z{WjX{T{QhR{N{KfL{H{EdF{Bc}}Ie}}Og}}Ui}}Ak}",
      "((j((h((f((d(c))e))g))i))k)"                                          },
{ L_, "A{BbCc}D{EuAFdGuAHeIf}G{HuAIgKuD}L{MuGNuDOhPiQj}",
      "L{M{H{BbCc}IgK{E{BbCc}FdG{BbCc}HeIf}}N{E{BbCc}FdG{BbCc}HeIf}OhPiQj}",
      "(((bc)g((bc)d(bc)ef))((bc)d(bc)ef)hij)"                               },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        testData(DATA, NUM_DATA, veryVerbose, veryVeryVerbose);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING Parsing a mixture of empty and non-empty elements
        // representing unset and set scalar and array fields in one record.
        //
        // Plan:
        //   Create a bdem_Schema with a record that contains fields of all
        //   scalar/array types, and create a baexml_DataParser on this
        //   bdem_Schema.
        //   Create an XML document of elements of all types within the root
        //   and these elements are alternatingly empty.  Parse the document
        //   and verify the resulting bdem_List is as expected.
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "TEST Parsing of multiple non-empty and empty "
                      << "elements" << bsl::endl;
        }

        bsl::ostringstream bdemSchemaSpec;
        bdemSchemaSpec << "A{}B{"; // contains two records A and B
                                   // but record A is empty and record B has
                                   // no relation to record A
        bsl::ostringstream expectedListSpec;
        expectedListSpec << '(';

        // two possible way of writing XML documents should give the same
        // parsing result
        bsl::ostringstream xmlSpec1;
        xmlSpec1 << "B{";
        bsl::ostringstream xmlSpec2;
        xmlSpec2 << "B{";

        static const char BDEMTYPECHARS[] = "abcdefghijklmnopqrst";

        for (int i = 0; i < sizeof BDEMTYPECHARS - 1; ++i) {
            EType::Type type = gType(BDEMTYPECHARS[i]);
            ASSERT(! EType::isAggregateType(type));

            const char FIELDNAME_CHAR = 'A' + i;
            const char TYPE_CHAR      = BDEMTYPECHARS[i];

            bdemSchemaSpec << FIELDNAME_CHAR << TYPE_CHAR;
            if (EType::isScalarType(type)) {
                bdemSchemaSpec << "@D0";  // default value
            }
            if (EType::isArrayType(type) && EType::BDEM_STRING_ARRAY != type) {
                bdemSchemaSpec << "@FL";  // format as XML <list>
            }

            expectedListSpec << TYPE_CHAR;
            if (i % 2) {
                if (EType::isScalarType(type)) {
                    xmlSpec1 << FIELDNAME_CHAR;   // name only -> empty element
                                                  // write nothing to xmlSpec2
                }
                if (EType::isArrayType(type)) {
                    expectedListSpec << 'N';      // Null
                }
            }
            else {
                xmlSpec1 << FIELDNAME_CHAR << TYPE_CHAR; // non empty element
                xmlSpec2 << FIELDNAME_CHAR << TYPE_CHAR; // non empty element
            }
        }
        bdemSchemaSpec << '}';
        expectedListSpec << ')';
        xmlSpec1 << '}';
        xmlSpec2 << '}';

        if (veryVerbose) {
            P(bdemSchemaSpec.str());
            P(expectedListSpec.str());
            P(xmlSpec1.str());
            P(xmlSpec2.str());
        }

        bdem_Schema schema;
        bcema_SharedPtr<bdem_Schema> schemaPtr(&schema,
                                               bcema_SharedPtrNilDeleter(),
                                               0);

        gBdemSchema(&schema, bdemSchemaSpec.str());

        bdem_List expectedList;
        gBdemList(&expectedList, expectedListSpec.str());

        if (veryVerbose) { N_  P(schema);  P(expectedList); }

        bsl::string xmlString1, xmlString2;
        gXml(&xmlString1, xmlSpec1.str());
        gXml(&xmlString2, xmlSpec2.str());

        bsl::istringstream xmlStream1(xmlString1);
        bsl::istringstream xmlStream2(xmlString2);

        bcem_Aggregate  aggregate;
        ErrorInfo       errInfo;
        Reader          reader;
        //Obj dataParser(&reader,TESTNAMESPACE, &schema);

        int status = baexml_DataUtil::createAggregateFromDocument(
                        &aggregate,
                        xmlStream1.rdbuf(),
                        schemaPtr,
                        &reader,
                        &errInfo,
                        &bsl::cerr);

        const bdem_List& list1 = aggregate.asElemRef().theList();
        int index = aggregate.recordDef().recordIndex();

        if (veryVerbose) { N_  P(xmlString1);  P(list1); }
        ASSERT(status >= 0);
        ASSERT(list1 == expectedList);
        ASSERT(1 == index); // index of record B

        status = baexml_DataUtil::createAggregateFromDocument(
                        &aggregate,
                        xmlStream2.rdbuf(),
                        schemaPtr,
                        &reader,
                        &errInfo,
                        &bsl::cerr);

        const bdem_List& list2 = aggregate.asElemRef().theList();
        index = aggregate.recordDef().recordIndex();

        if (veryVerbose) { N_  P(xmlString2);  P(list2); }
        ASSERT(status >= 0);
        ASSERT(list2 == expectedList);
        ASSERT(1 == index); // index of record B
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING Parsing of empty data elements representing unset scalar
        // and array fields
        //
        // Plan:
        //   Create a bdem_Schema with a record that contains fields of all
        //   scalar/array types, and a create a baexml_DataParser on this
        //   bdem_Schema.  Create an XML document of elements of all types
        //   within the root.  These elements are empty tags.  Create another
        //   XML document of one empty root element (namely, write nothing
        //   between the root tags).  Parse these XML documents and verify that
        //   the resulting bdem_List are the same as the expected bdem_List.
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "TEST Parsing of empty elements"
                      << " representing unset scalar/array" << bsl::endl;
        }

        if (verbose) {
            bsl::cout << "Testing individual elements." << bsl::endl;
        }

        static const char BDEMTYPECHARS[] = "abcdghijklmnopqrst";

        {
            for (int i = 0; i < sizeof BDEMTYPECHARS - 1; ++i) {
                EType::Type type = gType(BDEMTYPECHARS[i]);
               ASSERT(! EType::isAggregateType(type));

                const char FIELDNAME = 'A' + i;
                const char TYPE      = BDEMTYPECHARS[i];

                bsl::ostringstream bdemSchemaSpec;
                bdemSchemaSpec << "A{}B{" << FIELDNAME << TYPE;
                if (EType::isScalarType(type)) {
                    bdemSchemaSpec << "@D0";  // default value
                }
                bdemSchemaSpec << '}';

                bsl::ostringstream expectedListSpec;
                expectedListSpec << '(' << TYPE;
                if (EType::isArrayType(type)) {
                    expectedListSpec << 'N';  // null
                }
                expectedListSpec << ')';

                bsl::ostringstream xmlSpec;
                xmlSpec << "B{";
                if (EType::isScalarType(type)) {
                    xmlSpec << FIELDNAME;
                }
                xmlSpec << '}';

                if (veryVerbose) {
                    P(bdemSchemaSpec.str());
                    P(expectedListSpec.str());
                    P(xmlSpec.str());
                }

                bdem_Schema schema;
                bcema_SharedPtr<bdem_Schema>  schemaPtr(
                                         &schema,
                                         bcema_SharedPtrNilDeleter(),
                                         0);

                gBdemSchema(&schema, bdemSchemaSpec.str());

                bdem_List expectedList;
                gBdemList(&expectedList, expectedListSpec.str());

                if (veryVerbose) { N_  P(schema);  P(expectedList); }

                bsl::string xmlString;
                gXml(&xmlString, xmlSpec.str());

                bsl::istringstream xmlStream(xmlString);

                bcem_Aggregate  aggregate;
                ErrorInfo       errInfo;
                Reader          reader;
                //Obj dataParser(&reader,TESTNAMESPACE, &schema);

                int status = baexml_DataUtil::createAggregateFromDocument(
                        &aggregate,
                        xmlStream.rdbuf(),
                        schemaPtr,
                        &reader,
                        &errInfo,
                        &bsl::cerr);

                const bdem_List& LIST = aggregate.asElemRef().theList();
                int index = aggregate.recordDef().recordIndex();

                if (veryVerbose) { N_  P(xmlString);  P(LIST); }
                ASSERT(status >= 0);
                LOOP_ASSERT(errInfo.message(), errInfo.message().empty());
                LOOP_ASSERT(errInfo.lineNumber(), 0 == errInfo.lineNumber());
                LOOP_ASSERT(errInfo.columnNumber(),
                            0 == errInfo.columnNumber());
                LOOP2_ASSERT(LIST, expectedList, LIST == expectedList);
                ASSERT(1 == index);  // index of record B
            }
        }

        bsl::ostringstream bdemSchemaSpec;
        bdemSchemaSpec << "A{}B{";  // contains two records A and B
                                    // record A is empty; record B has
                                    // no relation to record A
        bsl::ostringstream expectedListSpec;
        expectedListSpec << '(';

        // three possible XML documents all should give the same parsing result
        bsl::ostringstream xmlSpec1;
        xmlSpec1 << "B{";
        bsl::ostringstream xmlSpec2;
        xmlSpec2 << "B{";
        bsl::ostringstream xmlSpec3;
        xmlSpec3 << "B"; // that's it for xmlSpec3

        for (int i = 0; i < sizeof BDEMTYPECHARS - 1; ++i) {
            EType::Type type = gType(BDEMTYPECHARS[i]);
            ASSERT(! EType::isAggregateType(type));

            const char FIELDNAME_CHAR = 'A' + i;
            const char TYPE_CHAR      = BDEMTYPECHARS[i];

            bdemSchemaSpec << FIELDNAME_CHAR << TYPE_CHAR;

            expectedListSpec << TYPE_CHAR;

            if (EType::isScalarType(type)) {
                bdemSchemaSpec << "@D0";  // default value

                xmlSpec1 << FIELDNAME_CHAR;  // field name only -> empty
                                             // element
            }
            if (EType::isArrayType(type)) {
                expectedListSpec << 'N';  // null
            }
        }
        bdemSchemaSpec << '}';
        expectedListSpec << ')';
        xmlSpec1 << '}';
        xmlSpec2 << '}';

        if (veryVerbose) {
            P(bdemSchemaSpec.str());
            P(expectedListSpec.str());
            P(xmlSpec1.str());
            P(xmlSpec2.str());
            P(xmlSpec3.str());
        }

        bdem_Schema schema;
        bcema_SharedPtr<bdem_Schema>  schemaPtr(
                                         &schema,
                                         bcema_SharedPtrNilDeleter(),
                                         0);
        gBdemSchema(&schema, bdemSchemaSpec.str());

        bdem_List expectedList;
        gBdemList(&expectedList, expectedListSpec.str());

        if (veryVerbose) { N_  P(schema);  P(expectedList); }

        bsl::string xmlString1, xmlString2, xmlString3;
        gXml(&xmlString1, xmlSpec1.str());
        gXml(&xmlString2, xmlSpec2.str());
        gXml(&xmlString3, xmlSpec3.str());

        bsl::istringstream xmlStream1(xmlString1);
        bsl::istringstream xmlStream2(xmlString2);
        bsl::istringstream xmlStream3(xmlString3);

        bcem_Aggregate  aggregate;
        ErrorInfo       errInfo;
        Reader          reader;

        int status = baexml_DataUtil::createAggregateFromDocument(
                        &aggregate,
                        xmlStream1.rdbuf(),
                        schemaPtr,
                        &reader,
                        &errInfo,
                        &bsl::cerr);

        bdem_List& list1 = aggregate.asElemRef().theModifiableList();
        int index = aggregate.recordDef().recordIndex();

        if (veryVerbose) { N_  P(xmlString1);  P(list1); }
        ASSERT(status >= 0);
        LOOP_ASSERT(errInfo.message(), errInfo.message().empty());
        LOOP_ASSERT(errInfo.lineNumber(), 0 == errInfo.lineNumber());
        LOOP_ASSERT(errInfo.columnNumber(), 0 == errInfo.columnNumber());
        LOOP2_ASSERT(list1, expectedList, list1 == expectedList);
        ASSERT(1 == index);  // index of record B

        errInfo.reset();
        status = baexml_DataUtil::createAggregateFromDocument(
                        &aggregate,
                        xmlStream2.rdbuf(),
                        schemaPtr,
                        &reader,
                        &errInfo,
                        &bsl::cerr);

        bdem_List& list2 = aggregate.asElemRef().theModifiableList();
        index = aggregate.recordDef().recordIndex();

        if (veryVerbose) { N_  P(xmlString2);  P(list2); }
        ASSERT(status >= 0);
        LOOP_ASSERT(errInfo.message(), errInfo.message().empty());
        LOOP_ASSERT(errInfo.lineNumber(), 0 == errInfo.lineNumber());
        LOOP_ASSERT(errInfo.columnNumber(), 0 == errInfo.columnNumber());
        LOOP2_ASSERT(list2, expectedList, list2 == expectedList);
        ASSERT(1 == index);  // index of record B

        errInfo.reset();
        status = baexml_DataUtil::createAggregateFromDocument(
                        &aggregate,
                        xmlStream3.rdbuf(),
                        schemaPtr,
                        &reader,
                        &errInfo,
                        &bsl::cerr);

        bdem_List& list3 = aggregate.asElemRef().theModifiableList();
        index = aggregate.recordDef().recordIndex();

        if (veryVerbose) { P(xmlString3);  P(list3); }
        ASSERT(status >= 0);
        LOOP_ASSERT(errInfo.message(), errInfo.message().empty());
        LOOP_ASSERT(errInfo.lineNumber(), 0 == errInfo.lineNumber());
        LOOP_ASSERT(errInfo.columnNumber(), 0 == errInfo.columnNumber());
        LOOP2_ASSERT(list3, expectedList, list3 == expectedList);
        ASSERT(1 == index);  // index of record B

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING Parsing of multiple non-empty XML data elements representing
        // scalar and array fields
        //
        // Plan:
        //   Create a bdem_Schema with a record that contains fields of all
        //   scalar/array types, and create a baexml_DataParser on this
        //   bdem_Schema.
        //   Create an XML document of elements of all types within the root
        //   element.  Parse the XML element and compare the result with
        //   expected bdem_List.
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "TEST parsing of multiple non-empty scalar/array"
                      << " elements" << bsl::endl;
        }

        bsl::ostringstream bdemSchemaSpec;
        bdemSchemaSpec << "A{}B{"; // contains two records A and B
                                   // but record A is empty and record B has
                                   // no relation to record A
        bsl::ostringstream expectedListSpec;
        expectedListSpec << '(';

        bsl::ostringstream xmlSpec;
        xmlSpec << "B{";

        static const char BDEMTYPECHARS[] = "abcdefghijklmnopqrst";

        for (int i = 0; i < sizeof BDEMTYPECHARS - 1; ++i) {
            EType::Type type = gType(BDEMTYPECHARS[i]);
            ASSERT(! EType::isAggregateType(type));

            const char FIELDNAME_CHAR = 'A' + i;
            const char TYPE_CHAR      = BDEMTYPECHARS[i];

            bdemSchemaSpec << FIELDNAME_CHAR << TYPE_CHAR;
            if (EType::isArrayType(type) && EType::BDEM_STRING_ARRAY != type) {
                bdemSchemaSpec << "@FL";  // format as XML <list>
            }
            expectedListSpec << TYPE_CHAR;
            xmlSpec << FIELDNAME_CHAR << TYPE_CHAR;
        }
        bdemSchemaSpec << '}';
        expectedListSpec << ')';
        xmlSpec << '}';

        if (veryVerbose) {
            P(bdemSchemaSpec.str());
            P(expectedListSpec.str());
            P(xmlSpec.str());
        }

        bdem_Schema schema;
        bcema_SharedPtr<bdem_Schema>  schemaPtr(
                                         &schema,
                                         bcema_SharedPtrNilDeleter(),
                                         0);
        gBdemSchema(&schema, bdemSchemaSpec.str());

        bdem_List expectedList;
        gBdemList(&expectedList, expectedListSpec.str());

        bsl::string xmlString;
        gXml(&xmlString, xmlSpec.str());
        bsl::istringstream xmlStream(xmlString);

        bcem_Aggregate  aggregate;
        ErrorInfo       errInfo;
        Reader          reader;

        int status = baexml_DataUtil::createAggregateFromDocument(
                        &aggregate,
                        xmlStream.rdbuf(),
                        schemaPtr,
                        &reader,
                        &errInfo,
                        &bsl::cerr);

        bdem_List& list = aggregate.asElemRef().theModifiableList();
        int index = aggregate.recordDef().recordIndex();

        if (veryVerbose) { P(expectedList);  P(list); P(xmlString) }

        ASSERT(status >= 0);
        LOOP2_ASSERT(list, expectedList, list == expectedList);
        ASSERT(1 == index); // index of record B
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING SCHEMA WITH INCLUDES
        //
        // Concerns:
        //   That basic essential functionality is operational
        //   for one thread.
        //
        // Plan:
        //   Create xyz.
        //
        //   Create xyz.
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TEST SCHEMA WITH INCLUDES" << endl
                          << "==========================" << endl;

        using namespace TestCase2;

        bslma_TestAllocator ta(veryVeryVerbose);

        bsl::ostream * verboseStream = 0;

        if (veryVeryVerbose){
            verboseStream = &bsl::cout;
        }

        bcem_Aggregate result;
        bdesb_FixedMemInStreamBuf xmlSchemaStream(SCHEMA, sizeof(SCHEMA)-1);
        bdesb_FixedMemInStreamBuf xmlDataStream(DATA, sizeof(DATA)-1);

        ResolverFunctor resolver;
        baexml_ErrorInfo errorInfo;
        baexml_MiniReader reader;
        reader.setResolver(resolver);

        int rc = baexml_DataUtil::createAggregateFromDocument(
                        &result,
                        &xmlDataStream,
                        &xmlSchemaStream,
                        &reader,
                        &errorInfo,
                        verboseStream,
                        &ta);

        ASSERT (rc==0);

        if (verbose) {
            cout << result << endl;
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise the basic functionality.
        //
        // Concerns:
        //   That basic essential functionality is operational.
        //
        // Plan:
        //   Having correct XSD schema and corresponded XML document,
        //   build the 'bcem_Aggregate' object.  Verify that aggregate
        //   contains expected fields with expected values.
        //   Perform described action in loop with all possible
        //   combinations:  with/without parameter ErrorInfo,
        //   with/without parameter DiagnosticStream,
        //   with baexml_Reader and with baexml_ValidatingReader,
        //   with/without parameter DecoderOptions,
        //   with/without bslma_Allocator
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        using namespace TestCase1;

        {
            bcem_Aggregate result;

            int rc = doAllTests(&result, DATA, SCHEMA, 0);

            ASSERT (rc==0);
            ASSERT (result["name"].asString() == "TOM");
            ASSERT (result["salary"].asInt() == 100);
            ASSERT (result["married"].asBool() == true);

            if (verbose) {
                cout << result << endl;
            }

        }

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // INTERACTIVE TEST:
        //
        // Usage: test_prog -1 xsdFile xmlFile
        // --------------------------------------------------------------------

        ASSERT(argc >= 4);
        if (argc < 4) break;

        verbose = argc > 4;
        veryVerbose = argc > 5;
        veryVeryVerbose = argc > 6;

        bsl::ostream * verboseStream = 0;

        if (veryVeryVerbose){
            verboseStream = &bsl::cout;
        }

        bcem_Aggregate result;

        bsl::ifstream xmlSchemaStream(argv[2]);
        bsl::ifstream xmlDataStream(argv[3]);

        ASSERT(xmlSchemaStream.good());
        ASSERT(xmlDataStream.good());

        baexml_ErrorInfo errorInfo;
        baexml_MiniReader reader;
        TestValidatingReader vreader(&reader);

        int rc = baexml_DataUtil::createAggregateFromDocument(
                        &result,
                        xmlDataStream.rdbuf(),
                        xmlSchemaStream.rdbuf(),
                        &reader,
                        &errorInfo,
                        verboseStream);

        ASSERT(0 == rc);
        bsl::cout << errorInfo << bsl::endl;
        if (0 == rc) {
            P(result);
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
