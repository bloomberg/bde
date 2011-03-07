// baexml_schemaparser.cpp                                            -*-C++-*-
#include <baexml_schemaparser.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_schemaparser_cpp,"$Id$ $CSID$")

#include <baexml_elementattribute.h>
#include <baexml_minireader.h>
#include <baexml_namespaceregistry.h>
#include <baexml_prefixstack.h>

#include <baexml_typesparserutil.h>

#include <bdeat_formattingmode.h>
#include <bdem_elemref.h>
#include <bdem_elemtype.h>
#include <bdem_schema.h>
#include <bdema_sequentialallocator.h>

#include <bdeu_print.h>
#include <bdetu_unset.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_newdeleteallocator.h>

#include <bsls_assert.h>
#include <bsls_objectbuffer.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#include <bsl_algorithm.h>
#include <bsl_functional.h>
#include <bsl_iterator.h>
#include <bsl_list.h>
#include <bsl_map.h>
#include <bsl_ostream.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_stack.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {

namespace {

const int ANON_NAMESPACE = baexml_NamespaceRegistry::BAEXML_PREDEF_MIN - 1000;
    // Special namespace ID for the anonymous namespace used for generated
    // qualified symbols.

typedef bsl::pair<int, bsl::string> QualifiedSymbol;
    // A fully-qualified symbol consisting of a namespace ID (first) and
    // local name (second).

const int NULL_NSID = -1;
    // Null namespace ID

static inline
bdem_ElemType::Type bdemArrayType(bdem_ElemType::Type orig)
    // Given a bdem element type, 'orig', return the corresponding array
    // type.  The array type for 'LIST' is 'TABLE'.  This operation is
    // idempotent -- if 'orig' is already an array type, then return
    // 'orig'.  Return 'orig' unchanged if it is not a valid element type.
{
    bdem_ElemType::Type result = bdem_ElemType::toArrayType(orig);
    return bdem_ElemType::BDEM_VOID == result ? orig : result;
}

static
int parseDefaultValue(bdem_ElemRef elemRef, const bsl::string& defaultValue)
    // Replace the value in the specified 'elemRef' with the value parsed from
    // the specified 'defaultValue'.  Return 0 on success, and a non-zero value
    // otherwise.
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    int status = BAEXML_FAILURE;

    const char *cStrValue = defaultValue.c_str();
    const int   len       = static_cast<int>(defaultValue.size());

    switch (elemRef.type()) {
      case bdem_ElemType::BDEM_CHAR: {
        char value;
        if (! baexml_TypesParserUtil::parseDefault(&value, cStrValue, len)) {
            elemRef.theModifiableChar() = value;
            status = BAEXML_SUCCESS;
        }
      } break;
      case bdem_ElemType::BDEM_SHORT: {
        short value;
        if (! baexml_TypesParserUtil::parseDefault(&value, cStrValue, len)) {
            elemRef.theModifiableShort() = value;
            status = BAEXML_SUCCESS;
        }
      } break;
      case bdem_ElemType::BDEM_INT: {
        int value;
        if (! baexml_TypesParserUtil::parseDefault(&value, cStrValue, len)) {
            elemRef.theModifiableInt() = value;
            status = BAEXML_SUCCESS;
        }
      } break;
      case bdem_ElemType::BDEM_INT64: {
        typedef bsls_PlatformUtil::Int64 Int64;
        Int64 value;
        if (! baexml_TypesParserUtil::parseDefault(&value, cStrValue, len)) {
            elemRef.theModifiableInt64() = value;
            status = BAEXML_SUCCESS;
        }
      } break;
      case bdem_ElemType::BDEM_FLOAT: {
        float value;
        if (! baexml_TypesParserUtil::parseDefault(&value, cStrValue, len)) {
            elemRef.theModifiableFloat() = value;
            status = BAEXML_SUCCESS;
        }
      } break;
      case bdem_ElemType::BDEM_DOUBLE: {
        double value;
        if (! baexml_TypesParserUtil::parseDefault(&value, cStrValue, len)) {
            elemRef.theModifiableDouble() = value;
            status = BAEXML_SUCCESS;
        }
      } break;
      case bdem_ElemType::BDEM_STRING: {
        bsl::string value;
        if (! baexml_TypesParserUtil::parseDefault(&value, cStrValue, len)) {
            elemRef.theModifiableString() = value;
            status = BAEXML_SUCCESS;
        }
      } break;
      case bdem_ElemType::BDEM_DATETIME: {
        bdet_Datetime value;
        if (! baexml_TypesParserUtil::parseDefault(&value, cStrValue, len)) {
            elemRef.theModifiableDatetime() = value;
            status = BAEXML_SUCCESS;
        }
      } break;
      case bdem_ElemType::BDEM_DATE: {
        bdet_Date value;
        if (! baexml_TypesParserUtil::parseDefault(&value, cStrValue, len)) {
            elemRef.theModifiableDate() = value;
            status = BAEXML_SUCCESS;
        }
      } break;
      case bdem_ElemType::BDEM_TIME: {
        bdet_Time value;
        if (! baexml_TypesParserUtil::parseDefault(&value, cStrValue, len)) {
            elemRef.theModifiableTime() = value;
            status = BAEXML_SUCCESS;
        }
      } break;
      case bdem_ElemType::BDEM_BOOL: {
        bool value;
        if (! baexml_TypesParserUtil::parseDefault(&value, cStrValue, len)) {
            elemRef.theModifiableBool() = value;
            status = BAEXML_SUCCESS;
        }
      } break;
      case bdem_ElemType::BDEM_DATETIMETZ: {
        bdet_DatetimeTz value;
        if (! baexml_TypesParserUtil::parseDefault(&value, cStrValue, len)) {
            elemRef.theModifiableDatetimeTz() = value;
            status = BAEXML_SUCCESS;
        }
      } break;
      case bdem_ElemType::BDEM_DATETZ: {
        bdet_DateTz value;
        if (! baexml_TypesParserUtil::parseDefault(&value, cStrValue, len)) {
            elemRef.theModifiableDateTz() = value;
            status = BAEXML_SUCCESS;
        }
      } break;
      case bdem_ElemType::BDEM_TIMETZ: {
        bdet_TimeTz value;
        if (! baexml_TypesParserUtil::parseDefault(&value, cStrValue, len)) {
            elemRef.theModifiableTimeTz() = value;
            status = BAEXML_SUCCESS;
        }
      } break;
      default: {
        BSLS_ASSERT(0);  // Should *not* reach here!
      } break;
    }

    return status;
}

////////////////////////////////////////////////////////////////////////

struct XMLTypeDescriptor {
    // Statically-initializable struct holding attributes of primitive XML
    // types.  All members are public.

    // TYPES
    typedef const char          *TypeId;

    // PUBLIC MEMBER DATA
    TypeId              d_xmlName;         // name of built-in XML schema type
    bdem_ElemType::Type d_bdemType;        // 'bdem' equivalent type
    bsls_Types::Uint64  d_maxValue;        // max value or length
    int                 d_formattingMode;  // bdeat_FormattingMode enum value

    // CREATORS

    // This is a POD type that can be statically initialized.
    //
    // Use compiler-generated default constructor, copy constructor,
    // destructor, and assignment operator.

    // ACCESSORS
    bdem_ElemType::Type bdemType() const;
        // Return the 'bdem' element type corresponding this XML type
        // descriptor.

    int formattingMode() const;
        // Return the formatting mode corresponding this XML type descriptor.
};

// Abbreviations to make 'KNOWN_TYPES' table (below) more compact:
typedef bdem_ElemType        EType;
typedef bdeat_FormattingMode FM;

// Array of descriptors for each of the pre-defined types defined in the XML
// schema specification (http://www.w3.org/TR/xmlschema-2/#built-in-datatypes).
// Each XSD type is mapped to the closest bdem type capable of holding its
// value.  Constraint information is lost in this translation.  Note that
// unsigned types must be represented in 'bdem' by the next larger signed type.
//
// The following built-in XML types are not supported and are mapped to
// 'bdem_ElemType::BDEM_VOID':
//
//   anyType, anySimpleType, duration, gYearMonth, gYear, gMonthDay, gDay,
//   gMonth, NOTATION
//
const XMLTypeDescriptor KNOWN_TYPES[] = {
    { "ENTITIES",           EType::BDEM_STRING_ARRAY, ULONG_MAX,
                                                      FM::BDEAT_LIST    },
    { "ENTITY",             EType::BDEM_STRING,       ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "ID",                 EType::BDEM_STRING,       ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "IDREF",              EType::BDEM_STRING,       ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "IDREFS",             EType::BDEM_STRING_ARRAY, ULONG_MAX,
                                                      FM::BDEAT_LIST    },
    { "NCName",             EType::BDEM_STRING,       ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "NMTOKEN",            EType::BDEM_STRING,       ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "NMTOKENS",           EType::BDEM_STRING_ARRAY, ULONG_MAX,
                                                      FM::BDEAT_LIST    },
    { "NOTATION",           EType::BDEM_VOID,         0,
                                                      FM::BDEAT_DEFAULT },
    { "QName",              EType::BDEM_STRING,       ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "anySimpleType",      EType::BDEM_VOID,         0,
                                                      FM::BDEAT_DEFAULT },
    { "anyType",            EType::BDEM_VOID,         0,
                                                      FM::BDEAT_DEFAULT },
    { "anyURI",             EType::BDEM_STRING,       ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "base64Binary",       EType::BDEM_CHAR_ARRAY,   INT_MAX,
                                                      FM::BDEAT_BASE64  },
    { "boolean",            EType::BDEM_BOOL,         1,
                                                      FM::BDEAT_DEFAULT },
    { "byte",               EType::BDEM_CHAR,         127,
                                                      FM::BDEAT_DEC     },
    { "date",               EType::BDEM_DATETZ,       INT_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "dateTime",           EType::BDEM_DATETIMETZ,   INT_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "decimal",            EType::BDEM_DOUBLE,       INT_MAX,
                                                      FM::BDEAT_DEC     },
    { "double",             EType::BDEM_DOUBLE,       INT_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "duration",           EType::BDEM_VOID,         0,
                                                      FM::BDEAT_DEFAULT },
    { "float",              EType::BDEM_FLOAT,        INT_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "gDay",               EType::BDEM_VOID,         0,
                                                      FM::BDEAT_DEFAULT },
    { "gMonth",             EType::BDEM_VOID,         0,
                                                      FM::BDEAT_DEFAULT },
    { "gMonthDay",          EType::BDEM_VOID,         0,
                                                      FM::BDEAT_DEFAULT },
    { "gYear",              EType::BDEM_VOID,         0,
                                                      FM::BDEAT_DEFAULT },
    { "gYearMonth",         EType::BDEM_VOID,         0,
                                                      FM::BDEAT_DEFAULT },
    { "hexBinary",          EType::BDEM_CHAR_ARRAY,   INT_MAX,
                                                      FM::BDEAT_HEX     },
    { "int",                EType::BDEM_INT,          INT_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "integer",            EType::BDEM_INT64,        ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "language",           EType::BDEM_STRING,       ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "long",               EType::BDEM_INT64,        ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "Name",               EType::BDEM_STRING,       ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "negativeInteger",    EType::BDEM_INT64,        ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "nonNegativeInteger", EType::BDEM_INT64,        ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "nonPositiveInteger", EType::BDEM_INT64,        ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "normalizedString",   EType::BDEM_STRING,       ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "positiveInteger",    EType::BDEM_INT64,        ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "short",              EType::BDEM_SHORT,        SHRT_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "string",             EType::BDEM_STRING,       ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "time",               EType::BDEM_TIMETZ,       INT_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "token",              EType::BDEM_STRING,       ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "unsignedByte",       EType::BDEM_SHORT,        255,
                                                      FM::BDEAT_DEFAULT },
    { "unsignedInt",        EType::BDEM_INT64,        ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "unsignedLong",       EType::BDEM_INT64,        ULONG_MAX,
                                                      FM::BDEAT_DEFAULT },
    { "unsignedShort",      EType::BDEM_INT,          USHRT_MAX,
                                                      FM::BDEAT_DEFAULT }
};

const XMLTypeDescriptor *const KNOWN_TYPES_END =
    KNOWN_TYPES + (sizeof(KNOWN_TYPES) / sizeof(XMLTypeDescriptor));

const XMLTypeDescriptor COMPLEX_TYPE_DESCRIPTOR =
    { "complexType", EType::BDEM_LIST, INT_MAX, FM::BDEAT_DEFAULT };
    // 'COMPLEX_TYPE_DESCRIPTOR' is the type descriptor ascribed to all XML
    // complex types.  Note that the type of 'bdem_RecordDef' created for an
    // XML complex type will be 'BDEM_CHOICE_RECORD' or 'BDEM_SEQUENCE_RECORD'
    // according to whether the top-level elements in the complex type are
    // constrained to be a '<choice>' or '<sequence>', respectively.

// TBD currently not used
#if 0
const XMLTypeDescriptor ANY_TYPE_DESCRIPTOR =
    { "anyType",            EType::BDEM_CHOICE,     INT_MAX,   FM::DEFAULT };
#endif

inline
bdem_ElemType::Type XMLTypeDescriptor::bdemType() const
{
    return d_bdemType;
}

inline
int XMLTypeDescriptor::formattingMode() const
{
    return d_formattingMode;
}

////////////////////////////////////////////////////////////////////////
//                      PRIVATE CLASSES
////////////////////////////////////////////////////////////////////////

// Forward declaration
class SchemaType;

                        // -------------------
                        // class SchemaElement
                        // -------------------

class SchemaElement {
    // Internal representation of an <element> or <attribute> entity in the
    // schema.  Objects of this type are stored in the 'd_topLevelElements' and
    // 'd_topLevelAttributes' symbol tables in the 'SchemaContentHandler'
    // object.  If a forward reference to an element is encountered during the
    // parse, then an empty 'SchemaElement' object is inserted into the symbol
    // table.  The contents of this empty object are filled in when the
    // definition for the element or attribute is parsed.

  private:
    // PRIVATE DATA MEMBERS

    // An element's type can be assigned directly via the 'type' attribute or
    // via an inline type declaration, or indirectly via the 'ref' attribute.
    // Thus, only one of the following two pointers will be non-null.
    SchemaType    *d_directType;
    SchemaElement *d_indirectType;

    int            d_formattingMode; // Enum value from 'bdeat_FormattingMode'
    bsl::string    d_default;        // default value
    int            d_id;             // value of "bdem:id" attribute
    int            d_minOccurs;      // value of "minOccurs" attribute
    int            d_maxOccurs;      // value of "maxOccurs" attribute

  public:
    // CREATORS
    explicit SchemaElement(bslma_Allocator *basicAllocator = 0);

    // MANIPULATORS
    void setDefaultValue(const bdeut_StringRef& defaultValue);
    void setId(int id);
    void setMinOccurs(int min);
    void setMaxOccurs(int max);
    void setType(SchemaType *type);
    void setElementRef(SchemaElement *ref);

    void addFormattingFlags(int flags);
    void removeFormattingFlags(int flags);

    SchemaType *type() const;
        // Returns the type of this element, following any element references,
        // or null if unassigned.

    // ACCESSORS
    bool isDefined() const;
        // Return 'true' if element refers to another element or to a type.

    const bsl::string& defaultValue() const;
    int id() const;
    int minOccurs() const;
    int maxOccurs() const;
    int formattingMode() const;

    bdem_ElemType::Type bdemType() const;
};

// CREATORS
inline
SchemaElement::SchemaElement(bslma_Allocator *basicAllocator)
: d_directType(0)
, d_indirectType(0)
, d_formattingMode(0)
, d_default(bslma_Default::allocator(basicAllocator))
, d_id(bdem_RecordDef::BDEM_NULL_FIELD_ID)
, d_minOccurs(1)
, d_maxOccurs(1)
{
}

// MANIPULATORS
inline
void SchemaElement::addFormattingFlags(int flags)
{
    d_formattingMode |= flags;
}

inline
void SchemaElement::removeFormattingFlags(int flags)
{
    d_formattingMode &= ~flags;
}

inline
void SchemaElement::setDefaultValue(const bdeut_StringRef& defaultValue)
{
    d_default = defaultValue;
}

inline
void SchemaElement::setId(int id)
{
    d_id = id;
}

inline
void SchemaElement::setMinOccurs(int min)
{
    d_minOccurs = min;
}

inline
void SchemaElement::setMaxOccurs(int max)
{
    d_maxOccurs = max;
}

inline
void SchemaElement::setType(SchemaType *type)
{
    d_directType = type;
}

inline
void SchemaElement::setElementRef(SchemaElement *ref)
{
    d_indirectType = ref;
}

// ACCESSORS
inline
bool SchemaElement::isDefined() const
{
    // Element is defined if it has a type or refers to another element.

    return 0 != d_directType || 0 != d_indirectType;
}

SchemaType *SchemaElement::type() const
{
    const SchemaElement *p = this;

    // Follow element reference indirection
    while (p->d_indirectType) {
        p = p->d_indirectType;
    }

    return p->d_directType;
}

inline
const bsl::string& SchemaElement::defaultValue() const
{
    return d_default;
}

inline
int SchemaElement::id() const
{
    return d_id;
}

inline
int SchemaElement::minOccurs() const
{
    return d_minOccurs;
}

inline
int SchemaElement::maxOccurs() const
{
    return d_maxOccurs;
}

inline
int SchemaElement::formattingMode() const
{
    return d_formattingMode;
}

// bdem_ElemType::Type SchemaElement::bdemType() const
// // Implementation moved below because it requires definition of
// // 'SchemaType'

                        // ----------------
                        // class SchemaType
                        // ----------------

class SchemaType {
    // Internal representation of a <complexType> or <simpleType> entity in the
    // schema.  Objects of this type are stored in the 'd_schemaTypes' symbol
    // table in the 'SchemaContentHandler'.  If a forward reference to a type
    // is encountered during the parse, then an empty 'SchemaType' object is
    // inserted into the symbol table.  The contents of this empty object are
    // filled in when the definition for the type is parsed.

  private:
    // PRIVATE TYPES
    typedef bsl::pair<bsl::string, SchemaElement*>    SchemaField;
    typedef bsl::vector<SchemaField>                  FieldVector;
    typedef bsl::vector<bsl::pair<bsl::string, int> > EnumerationVector;

    // PRIVATE DATA MEMBERS
    // This set of members is the union of all members needed to represent any
    // schema type.  The use of some of these members are mutually-exclusive.
    // For example, if 'd_fields' is non-empty, then 'd_enumerations' will be
    // empty and vice-versa.
    QualifiedSymbol          d_symbol;          // name of this type
    QualifiedSymbol          d_overrideSymbol;  // Name of top-level element
                                                // of this type, if any.
    SchemaType              *d_baseType;        // For simple types, the name
                                                // of another simple type on
                                                // which this is based.
    const XMLTypeDescriptor *d_xmlTypeDescriptor; // Description of this type
    bool                     d_isList;          // true if a sequence type
    bool                     d_isChoice;        // true if a choice type
    bsls_Types::Uint64       d_maxValue;        // max value constraint
    FieldVector              d_fields;          // list of nested fields
    bdem_RecordDef          *d_bdemRecord;      // generated bdem record
                                                // (held, not owned).
    EnumerationVector        d_enumerations;    // list of enumerators
    bdem_EnumerationDef     *d_bdemEnumeration; // generated bdem enumeration
                                                // (held, not owned)
    bool                     d_preserveEnumIds; // true to not sort enumerators

  public:
    // TYPES
    struct AttributesBeforeElementsCmp :
      public bsl::binary_function <
            const bsl::pair<bsl::string, SchemaElement*>&,
            const bsl::pair<bsl::string, SchemaElement*>&,
            bool >
    {
        // Field comparator for sorting 'FieldVector'.  The X.694 standard
        // requires that '<attribute>' tags be added before '<sequence>' or
        // '<choice>' tags, and that '<attribute>' tags be sorted by name.
        // This comparator produces an ordering whereby '<attribute>' tags
        // come before '<sequence>' or '<choice>' and are sorted by name as
        // well.

        bool operator()(
            const bsl::pair<bsl::string, SchemaElement*>& left,
            const bsl::pair<bsl::string, SchemaElement*>& right) const;
            // Return true if 'left' is an attribute and 'right' is not an
            // attribute (i.e., 'right' is an element of a sequence or
            // choice), or if 'left.first < right.first', and false otherwise.
    };

    typedef FieldVector::const_iterator FieldIterator;
    typedef EnumerationVector::const_iterator EnumerationIterator;

    // CREATORS
    SchemaType();
    explicit SchemaType(const QualifiedSymbol& symbol);

    // MANIPULATORS
    void setSymbol(const QualifiedSymbol& symbol);
    void setOverrideSymbol(const QualifiedSymbol& symbol);
    void setBaseType(SchemaType *baseType);
    void setXmlTypeDescriptor(const XMLTypeDescriptor *desc);
    void setIsList(bool isList = true);
    void setIsChoice(bool isChoice = true);
    void setMaxValue(bsls_Types::Uint64 maxValue);
    void addField(const bsl::string& name, SchemaElement *element);
    void setBdemRecord(bdem_RecordDef *record);
    void addEnumeration(const bsl::string& value, int id);
    void setPreserveEnumIds(bool val);
    void setBdemEnumeration(bdem_EnumerationDef *enumeration);
    void sortAttributesBeforeElements();

    // ACCESSORS
    const QualifiedSymbol& symbol() const;
    const QualifiedSymbol& overrideSymbol() const;
    SchemaType *baseType() const;
    const SchemaType *mostBaseType() const;
    const XMLTypeDescriptor *xmlTypeDescriptor() const;
    bool isList() const;
    bool isChoice() const;
    bdem_ElemType::Type bdemType() const;
    bsls_Types::Uint64 maxValue() const;
    bdem_RecordDef *bdemRecord() const;
    bool isDefined() const;
    bdem_EnumerationDef *bdemEnumeration() const;
    bool preserveEnumIds() const;

    FieldIterator beginFields() const;
    FieldIterator endFields() const;

    EnumerationIterator beginEnumerations() const;
    EnumerationIterator endEnumerations() const;

    bsl::ostream& print(bsl::ostream& os,
                        int           indentLevel = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS
inline
bool operator<(const SchemaType& lhs, const SchemaType& rhs);

// CREATORS
inline
SchemaType::SchemaType()
: d_baseType(0)
, d_xmlTypeDescriptor(0)
, d_isList(false)
, d_isChoice(false)
, d_maxValue(0)
, d_bdemRecord(0)
, d_bdemEnumeration(0)
, d_preserveEnumIds(false)
{
}

inline
SchemaType::SchemaType(const QualifiedSymbol& symbol)
: d_symbol(symbol)
, d_baseType(0)
, d_xmlTypeDescriptor(0)
, d_isList(false)
, d_isChoice(false)
, d_maxValue(0)
, d_bdemRecord(0)
, d_bdemEnumeration(0)
, d_preserveEnumIds(false)
{
}

// MANIPULATORS
bool
SchemaType::AttributesBeforeElementsCmp::operator()(
        const bsl::pair<bsl::string, SchemaElement*>& left,
        const bsl::pair<bsl::string, SchemaElement*>& right) const
{
    if ((left.second->formattingMode() &
        bdeat_FormattingMode::BDEAT_ATTRIBUTE) != 0) {

        if ((right.second->formattingMode() &
            bdeat_FormattingMode::BDEAT_ATTRIBUTE) != 0) {

            return false;

            // TBD: When we have verified impact we should revert to the
            // correct behavior.
            // Left and right arguments are both attributes, return the
            // lexicographically smaller attribute name.
            // return (left.first < right.first);
        }

        // Left argument is attribute and right argument is not.
        // 'left' is less than 'right'.
        return true;
    }

    // Left argument is not an attribute (i.e., it is an element).
    // 'left' >= 'right'.
    return false;
}

inline
void SchemaType::sortAttributesBeforeElements()
{
    // Sort schema fields: attributes come first, in the order in which
    // the appear in the schema (though in the future they will be
    // sorted lexicographically).  Elements come after the attributes,
    // also in the order in which they appear in the schema.

    bsl::stable_sort(d_fields.begin(), d_fields.end(),
                     AttributesBeforeElementsCmp());
}

inline
void SchemaType::setSymbol(const QualifiedSymbol& symbol)
{
    d_symbol = symbol;
}

inline
void SchemaType::setOverrideSymbol(const QualifiedSymbol& symbol)
{
    d_overrideSymbol = symbol;
}

inline
void SchemaType::setBaseType(SchemaType *baseType)
{
    d_baseType = baseType;
}

inline
void SchemaType::setXmlTypeDescriptor(const XMLTypeDescriptor *desc)
{
    d_xmlTypeDescriptor = desc;
}

inline
void SchemaType::setIsList(bool isList)
{
    d_isList = isList;
}

inline
void SchemaType::setIsChoice(bool isChoice)
{
    d_isChoice = isChoice;
}

inline
void SchemaType::setBdemRecord(bdem_RecordDef *record)
{
    d_bdemRecord = record;
}

void SchemaType::addEnumeration(const bsl::string& value, int id)
{
   if (bdetu_Unset<int>::unsetValue() == id) {
        id = d_enumerations.empty() ? 0 :  d_enumerations.back().second + 1;
    }
    else {
        // If we obtain even one enumerator ID for this (enumeration)
        // SchemaType instance, then mark the instance as having IDs.
        d_preserveEnumIds = true;
    }

    d_enumerations.push_back(bsl::make_pair(value, id));
}

inline
void SchemaType::setPreserveEnumIds(bool val)
{
    d_preserveEnumIds = val;
}

inline
void SchemaType::setBdemEnumeration(bdem_EnumerationDef *enumeration)
{
    d_bdemEnumeration = enumeration;
}

inline
void SchemaType::setMaxValue(bsls_Types::Uint64 maxValue)
{
    d_maxValue = maxValue;
}

inline
void SchemaType::addField(const bsl::string& name, SchemaElement *element)
{
    d_fields.push_back(SchemaField(name, element));
    d_xmlTypeDescriptor = &COMPLEX_TYPE_DESCRIPTOR;
}

// ACCESSORS
inline
const QualifiedSymbol& SchemaType::symbol() const
{
    return d_symbol;
}

inline
const QualifiedSymbol& SchemaType::overrideSymbol() const
{
    return d_overrideSymbol;
}

inline
SchemaType *SchemaType::baseType() const
{
    (void) &SchemaType::baseType;  // Suppress warning about unused function
    return d_baseType;
}

const SchemaType *SchemaType::mostBaseType() const
{
    const SchemaType *result = this;

    while (0 != result->d_baseType) {
        result = result->d_baseType;
    }

    return result;
}

const XMLTypeDescriptor *
SchemaType::xmlTypeDescriptor() const
{
    // Traverse list of base types.  Return first non-null d_xmlTypeDescriptor.
    const SchemaType *scan = this;
    while (0 == scan->d_xmlTypeDescriptor && 0 != scan->d_baseType) {
        scan = scan->d_baseType;
    }

    return scan->d_xmlTypeDescriptor;
}

bool SchemaType::isList() const
{
    // Traverse list of base types.  Return first non-false d_isList.
    const SchemaType *scan = this;
    while (! scan->d_isList && 0 != scan->d_baseType) {
        scan = scan->d_baseType;
    }

    return scan->d_isList;
}

bool SchemaType::isChoice() const
{
    // Traverse list of base types.  Return first non-false d_isChoice.
    const SchemaType *scan = this;
    while (! scan->d_isChoice && 0 != scan->d_baseType) {
        scan = scan->d_baseType;
    }

    return scan->d_isChoice;
}

bdem_ElemType::Type SchemaType::bdemType() const
{
    const XMLTypeDescriptor* xmlTypeDesc = xmlTypeDescriptor();
    if (0 == xmlTypeDesc) {
        return bdem_ElemType::BDEM_VOID;
    }
    else {
        bdem_ElemType::Type result = xmlTypeDesc->bdemType();
        if (this->isList()) {
            result = bdemArrayType(result);
        }

        return result;
    }
}

inline
bdem_RecordDef *SchemaType::bdemRecord() const
{
    return d_bdemRecord;
}

inline
bsls_Types::Uint64 SchemaType::maxValue() const
{
    // Traverse list of base types.  Return first non-zero d_maxValue.
    const SchemaType *scan = this;
    while (0 == scan->d_maxValue && 0 != scan->d_baseType) {
        scan = scan->d_baseType;
    }

    return scan->d_maxValue;
}

inline
bool SchemaType::isDefined() const
{
    // Type has a definition if it either has a type descriptor or a base type
    return (0 != d_xmlTypeDescriptor || 0 != d_baseType);
}

inline
bdem_EnumerationDef *SchemaType::bdemEnumeration() const
{
    return d_bdemEnumeration;
}

inline
bool SchemaType::preserveEnumIds() const
{
    return d_preserveEnumIds;
}

inline
SchemaType::FieldIterator SchemaType::beginFields() const
{
    return d_fields.begin();
}

inline
SchemaType::FieldIterator SchemaType::endFields() const
{
    return d_fields.end();
}

inline
SchemaType::EnumerationIterator SchemaType::beginEnumerations() const
{
    return d_enumerations.begin();
}

inline
SchemaType::EnumerationIterator SchemaType::endEnumerations() const
{
    return d_enumerations.end();
}

bsl::ostream& SchemaType::print(bsl::ostream& os,
                                int           indentLevel,
                                int           spacesPerLevel) const
{
    bdeu_Print::indent(os, indentLevel, spacesPerLevel);
    os << '{';
    if (0 > indentLevel) {
        indentLevel = -indentLevel;
    }
    ++indentLevel;

    bdeu_Print::newlineAndIndent(os, indentLevel, spacesPerLevel);
    os << "Symbol    = " << symbol().first << ':' << symbol().second;

    if (! overrideSymbol().second.empty()) {
        bdeu_Print::newlineAndIndent(os, indentLevel, spacesPerLevel);
        os << "Override  = " << overrideSymbol().first << ':'
           << overrideSymbol().second;
    }

    bdeu_Print::newlineAndIndent(os, indentLevel, spacesPerLevel);
    const XMLTypeDescriptor *desc = xmlTypeDescriptor();
    os << "XML Type  = " << (desc ? desc->d_xmlName : "(null)");

    bdeu_Print::newlineAndIndent(os, indentLevel, spacesPerLevel);
    os << "bdem Type = " << bdemType();

    bdeu_Print::newlineAndIndent(os, indentLevel, spacesPerLevel);
    os << "max value = " << maxValue();

    if (! d_fields.empty()) {
        bdeu_Print::newlineAndIndent(os, indentLevel, spacesPerLevel);
        os << "Fields = [";

        for (FieldIterator i = beginFields(); i != endFields(); ++i) {
            bdeu_Print::newlineAndIndent(os, indentLevel + 1, spacesPerLevel);
            os << "Name = " << i->first;
            if (0 == i->second) {
                os << ", (null element)";
                continue;
            }

            os << ", minOccurs = " << i->second->minOccurs()
               << ", maxOccurs = " << i->second->maxOccurs()
               << ", Type = ";
            if (0 == i->second->type()) {
                os << "(null)";
            }
            else {
                const QualifiedSymbol& elemTypeSymbol =
                    i->second->type()->symbol();
                os << elemTypeSymbol.first << ':' << elemTypeSymbol.second;
            }

            os << ", defaultValue = ";
            const bsl::string& defaultValue = i->second->defaultValue();
            if (defaultValue.empty()) {
                os << "(null)";
            }
            else {
                os << defaultValue;
            }
        }

        bdeu_Print::newlineAndIndent(os, indentLevel, spacesPerLevel);
        os << ']';
    }

    --indentLevel;
    bdeu_Print::newlineAndIndent(os, indentLevel, spacesPerLevel);
    os << '}';

    return os;
}

// FREE OPERATORS
inline
bool operator<(const SchemaType& lhs, const SchemaType& rhs)
{
    return lhs.symbol() < rhs.symbol();
}

///////////////////////////////////////////////////////////////////////////
// Implementation of this element function moved here because it requires
// definition of 'SchemaType'
bdem_ElemType::Type SchemaElement::bdemType() const
{
    bdem_ElemType::Type result = type()->bdemType();
    if (d_maxOccurs > 1) {
        result = bdemArrayType(result);
    }

    return result;
}

                        // ------------------------
                        // class ExternalSchemaInfo
                        // ------------------------

class ExternalSchemaInfo {
    // Information about a subordinate schema document that was incorporated
    // by a '<include>' or '<import>' entity.

  public:
    // PUBLIC TYPE
    enum  InclusionType {
        TOP_LEVEL,      // top-level main schema
        INCLUDE,        // schema from <include> construct
        IMPORT          // schema from <import> construct
    };

  private:
    // PRIVATE DATA MEMBERS
    InclusionType        d_incType;   // inclusion Type
    ExternalSchemaInfo  *d_parent;    // pointer to the parent
                                      // schema which directly
                                      // includes this schema.
                                      // 0 for TOP_LEVEL

    bsl::string          d_location;  // schema Location

    baexml_PrefixStack   d_prefixes;
    int                  d_targetNsId;

  public:
    // CREATORS
    ExternalSchemaInfo(baexml_NamespaceRegistry *namespaces,
                       const bdeut_StringRef&    location,
                       bslma_Allocator          *allocator = 0);
        // Top Level Schema constructor

    ExternalSchemaInfo(InclusionType           iType,
                       ExternalSchemaInfo     *parent,
                       const bdeut_StringRef&  location,
                       const bdeut_StringRef&  targetNs,
                       bslma_Allocator        *allocator = 0);
        // Constructor for the included and imported schemas

    // MANIPULATORS
    bool  setTargetNamespace(const bdeut_StringRef& uri);
        // Set the target namespace for this external schema.

    baexml_PrefixStack *prefixStack();
        // Return the address of the modifiable per-schema prefix stack.

    // PUBLIC ACCESSORS
    InclusionType       inclusionType  () const;
    ExternalSchemaInfo *parent         () const;
    ExternalSchemaInfo *root           () const;
    const bsl::string&  schemaLocation () const;

    int                 defaultNsId    () const;
    int                 targetNsId     () const;

    const char          *targetNamespace() const;
    const char          *defaultNamespace() const;

    int getNamespaceIdByPrefix(const bsl::string& prefix) const;
        // Return the namespace ID of the specified 'prefix' in the per-schema
        // prefix stack or -1 if not registered.
};

inline
ExternalSchemaInfo::ExternalSchemaInfo(baexml_NamespaceRegistry *namespaces,
                                       const bdeut_StringRef&    location,
                                       bslma_Allocator          *allocator)
: d_incType   (TOP_LEVEL)
, d_parent    (0)
, d_location  (allocator)
, d_prefixes  (namespaces, allocator)
, d_targetNsId(NULL_NSID)
{
    if (location.data()) {
        d_location.assign(location.data(), location.length());
    }
}

ExternalSchemaInfo::ExternalSchemaInfo(InclusionType           iType,
                                       ExternalSchemaInfo     *parent,
                                       const bdeut_StringRef&  location,
                                       const bdeut_StringRef&  targetNs,
                                       bslma_Allocator        *allocator)
: d_incType   (iType)
, d_parent    (parent)
, d_location  (allocator)
, d_prefixes  (parent->d_prefixes.namespaceRegistry(), allocator)
, d_targetNsId(NULL_NSID)
{
    BSLS_ASSERT(d_incType != TOP_LEVEL);

    if (location.data()) {
        d_location.assign(location.data(), location.length());
    }

    setTargetNamespace(targetNs);

    d_prefixes.pushPrefix("", targetNs);
}

inline
baexml_PrefixStack *ExternalSchemaInfo::prefixStack()
{
    return &d_prefixes;
}
inline
ExternalSchemaInfo::InclusionType ExternalSchemaInfo::inclusionType() const
{
    return d_incType;
}

inline
const bsl::string& ExternalSchemaInfo::schemaLocation() const
{
    return d_location;
}

inline
ExternalSchemaInfo *ExternalSchemaInfo::parent() const
{
    return d_parent;
}

inline
ExternalSchemaInfo *ExternalSchemaInfo::root() const
{
    ExternalSchemaInfo *ret = const_cast<ExternalSchemaInfo*> (this);

    while (ret->d_parent != 0) {
        ret = ret->d_parent;
    }
    return ret;
}

inline
int ExternalSchemaInfo::targetNsId() const
{
    return d_targetNsId;
}

inline
int ExternalSchemaInfo::defaultNsId() const
{
    return d_prefixes.lookupNamespaceId("");
}

inline
bool ExternalSchemaInfo::setTargetNamespace(const bdeut_StringRef& uri)
{
    int nsId = d_prefixes.namespaceRegistry()->lookupOrRegister(uri);

    if (d_targetNsId == NULL_NSID ||  // has not been set yet
        d_targetNsId == nsId)         // or the same - it is OK
    {
        d_targetNsId = nsId;          // set it now

        return true;
    }

    return false;
}

inline
int ExternalSchemaInfo::getNamespaceIdByPrefix(const bsl::string& prefix) const
{
    return d_prefixes.lookupNamespaceId(prefix);
}

inline
const char *ExternalSchemaInfo::targetNamespace() const
{
    return d_prefixes.lookupNamespaceUri(d_targetNsId);
}

inline
const char *ExternalSchemaInfo::defaultNamespace() const
{
    return d_prefixes.lookupNamespaceUri("");
}

                        // --------------------------
                        // class SchemaContentHandler
                        // --------------------------

class SchemaContentHandler
{
    // Top-level context for schema parser.  One of these objects is created
    // for each schema parse, but it exists only while a parse is in-progress.
    // (i.e., it is destroyed before the 'parse' method returns to the user.)

  public:
    // PUBLIC TYPES
    typedef  baexml_ErrorInfo::Severity Severity;

  private:
    // PRIVATE TYPES

    // Note that the property of bsl::list "insertion of new elements
    // does not invalidate the iterators for existing elements"
    // are very important for this implementation.
    typedef bsl::list<ExternalSchemaInfo> InputSchemaList;
    typedef InputSchemaList::iterator     InputSchemaIterator;
    typedef baexml_SchemaParser::SchemaElementAttributes
                                          SchemaElementAttributes;

    struct ElementSet {
        // A set of elements.  Each element has a numeric ID, as enumerated
        // later in this file (e.g., 'XSTAG_choice' for element "<choice>").

        unsigned long long d_bits;
            // One bit per potential element.  If we ever find ourselves with
            // more than 64 schema elements, we can easily switch to an array
            // of unsigned char, terminated by an 'XSTAG_NULL'.
    };

    struct SchemaElementDescriptor;
    friend struct SchemaElementDescriptor;
    struct SchemaElementDescriptor {
        // Map an XSD entity name to the actions to perform when the start and
        // end of the entity is encounted during schema parsing.  Also, has
        // list of valid schema elements that can be nested within the named
        // entity.

        const char *d_name;
        int (SchemaContentHandler::*d_startFunction)(int currXsTag);
        int (SchemaContentHandler::*d_endFunction)(int currXsTag);

        ElementSet d_validContentTags;
            // Array of valid nested elements.  Each 'char' holds the ID of a
            // schema element.  The IDs are enumerated in the '.cpp' file.

        bool isValidContent(int contentTag) const;
            // Return true if 'contentTag' exists in the set of valid content
            // tags and false otherwise.
    };

    // PRIVATE CONSTANTS
    enum {
        NSID_XMLSCHEMA = baexml_NamespaceRegistry::BAEXML_XMLSCHEMA,
        NSID_BDEM      = baexml_NamespaceRegistry::BAEXML_BDEM
    };

    // PRIVATE CLASS DATA
    static const SchemaElementDescriptor        DESCRIPTOR_TABLE[];
    static const SchemaElementDescriptor *const DESCRIPTOR_TABLE_END;

    static int verifyDescriptorTable();
        // Basic integrity checks on descriptor table.  Return 0 on success.

    // PRIVATE DATA MEMBERS
    bdema_SequentialAllocator d_managedAllocator; // Fast allocator

    const bsl::string         d_emptyString;   // Empty string.  Instance data
                                               // avoids need for thread-safe
                                               // singleton.

    baexml_NamespaceRegistry  d_namespaces;    // namespace => ID
    InputSchemaList           d_inputSchemas;  // schemas to process
    InputSchemaIterator       d_currentSchema; // iterator to current schema

    baexml_Reader            *d_reader;        // XML reader
    baexml_ErrorInfo          d_errorInfo;     // error status
    bsl::ostream              d_verboseStream; // stream for diagnostics

    bsl::map<QualifiedSymbol, SchemaElement>     d_topLevelElements;
    bsl::map<QualifiedSymbol, SchemaElement>     d_topLevelAttributes;
    bsl::set<SchemaType>                         d_schemaTypes;
    bsl::vector<const SchemaElementDescriptor *> d_descriptorStack;
    bsl::stack<SchemaElement*>                   d_elementDefStack;
    bsl::stack<SchemaType*>                      d_typeDefStack;
    SchemaElementAttributes                      d_schemaAttributes;

    int d_skippedElementDepth;  // number skipped elems on descriptor stack
    int d_anonymousTypeCounter; // to generate unique names for anonymous types

    // PRIVATE ACCESSORS
    int currentXsTag() const;
    int parentXsTag() const;

    // PRIVATE MANIPULATORS
    int checkForExistingSchema(const bdeut_StringRef& location,
                               int                    targetNsId);
        // Check if we already have this schema in the list and return:
        // 1  schema is not in the list
        // 0  schema is in the list, but in the different trunk.
        //    (this is not a recursion, just multiple inclusion)
        // -1 schema is in the list and in the same trunk
        //    (this is recursion!)

    const char *lookupAttribute(int namespaceId, const char *localname) const;
        // Return attribute value in the current reader node associated with
        // the specified 'localname' in the specified 'namespaceId'.  If
        // the attribute does not exist, return a null pointer.

    void storeSchemaAttributes();
        // Store the attributes of the top level schema element as name-value
        // pairs in this object.

    bool getQnameAttribute(QualifiedSymbol *attributeSymbol,
                           int              attributeNamespaceId,
                           const char      *attributeName);
        // Look up the attribute specified by 'attributeNamespaceId' and
        // 'attributeName' in the current reader node and convert the
        // attribute value from a qualified name to a symbol, which is stored
        // at the address specified by 'attributeSymbol'.  Return 'true' if
        // the attribute is found and return 'false' (and leave
        // 'attributeSymbol' unchanged) if the attribute is not found.

    void loadPrimtiveTypes();
        // Add primitive types to 'd_schemaTypes' symbol table.

    QualifiedSymbol generateLocalSymbol();
        // Generate a unique type symbol with the 'ANON_NAMESPACE' namespace
        // and an illegal name starting with a space.

    void createAnonymousUntaggedRecord(SchemaType     *parentType,
                                       SchemaElement **retElem,
                                       SchemaType    **retType);
        // Within the specified 'parentType', create an anonymous, untagged,
        // field of anonymous record type.  Set pointer at the specified
        // 'retElem' address to the newly-created field and the pointer at the
        // specified 'retType' address to the newly-crated record type.
        // '*retElem' and '*retType' are pushed onto the element and type
        // stacks, respectively, before returning.

    SchemaType& getType(const QualifiedSymbol& typeSymbol);
        // Look up 'typeSymbol' in 'd_schemaTypes' and return a modifiable
        // reference to the found 'SchemaType' object.  If symbol is not
        // found, insert an empty 'SchemaType' object into 'd_schemaTypes'.
        // If the symbol is not found but 'typeSymbol' is not in the target
        // namespace, then set the error state.  (It is a common error in .xsd
        // files to refer to a custom type without specifying the target
        // namespace.)  This function can be used to look up, but not to
        // insert, the built-in types (because built-in types are in the w3
        // namespace, not the target namespace).

    void generateTypeTree(bdem_Schema *outputSchema, SchemaType *type);
        // Generate the 'bdem_RecordDef' (recursively) and 'bdem_RecordDef'
        // objects described by 'type'.  This function inspects 'type' and
        // delegates to 'generateRecord' and 'generateEnumeration' functions
        // as appropriate.

    void generateRecord(bdem_Schema *outputSchema, SchemaType *type);
        // Recursively generate the 'bdem_RecordDef' object described by
        // 'type'.

    void generateEnumeration(bdem_Schema *outputSchema, SchemaType *type);
        // Generate the 'bdem_EnumerationDef' object described by
        // 'type'.

    int checkElement(const bsl::string& recordName,
                     const bsl::string& elementName,
                     SchemaElement     *element);
        // Additional checks for the element at the process of generation of
        // bdem_Schema.  Return zero in case of success; non zero otherwise.

    int startAttrDef(int currXsTag);
    int startBadDef(int currXsTag);
    int startElementDef(int currXsTag);
    int startEnumerationDef(int currXsTag);
    int startGroupDef(int currXsTag);
    int startIncludeDef(int currXsTag);
    int startImportDef(int currXsTag);
    int startLengthDef(int currXsTag);
    int startListDef(int currXsTag);
    int startNoopDef(int currXsTag);
    int startRecordDef(int currXsTag);
    int startRestrictionDef(int currXsTag);
    int startSchemaDef(int currXsTag);
    int startSkipDef(int currXsTag);
    int startTypeDef(int currXsTag);

    int endAttrDef(int currXsTag);
    int endElementDef(int currXsTag);
    int endSchemaDef(int currXsTag);
    int endTypeDef(int currXsTag);
    int endRecordDef(int currXsTag);
    int endOtherDef(int currXsTag);

    void dispatchStartElement();
    void dispatchEndElement();

    // NOT IMPLEMENTED
    SchemaContentHandler(const SchemaContentHandler& original);
    SchemaContentHandler&operator=(const SchemaContentHandler& rhs);

  public:
    // CREATORS
    SchemaContentHandler(baexml_Reader          *reader,
                         const bdeut_StringRef&  location);
        // Construct the context parsing an XSD document at the specified
        // 'location' from the specified 'reader'.  The 'location' is used for
        // error reporting and is is normally the URI or file name of the input
        // XSD document.

    ~SchemaContentHandler();
        // Destroy this object.

    // MANIPULATORS
    int  generateBdemSchema(bdem_Schema *outputSchema);
        // Generate output Bdem Schema after processing all input XSD schemas.
        // The implementation was removed from the 'endSchemaDef' methods and
        // and converted into the 'generateBdemSchema' method.

    bool advanceToNextSchema();
        // Move internal iterator to the next input schema and
        // prepare to parse it.

    void parseCurrentSchema(bsl::streambuf  *inputStream);
       // Parse the current schema from 'inputStream'

    void
    setError(const bsl::string& msg,
             Severity           severity = baexml_ErrorInfo::BAEXML_ERROR);
        // Set an error message.  Overwrite the current error message only if
        // a message of the same or greater severity has not already been set.
        // If line and column are not set, they are obtained from the current
        // locator object.

    void setVerbose(bsl::streambuf *buf);
        // TEMPORARY function to set verbose/debug output stream buffer.

    // ACCESSORS
    bsl::ostream& verboseStream();
        // Return a reference to verbose stream

    const SchemaElementAttributes& schemaAttributes() const;
        // Return a reference to the non-modifiable schema element attributes
        // stored by this schema content handler.

    const char *targetNamespace() const;
        // Return targetNamespace from the top level XSD schema.

    const baexml_ErrorInfo&  errorInfo() const;
        // Return ErrorInfo object

    ExternalSchemaInfo *currentSchemaInfo();
    const ExternalSchemaInfo *currentSchemaInfo() const;
        // Return the current input schema info.

    ExternalSchemaInfo *topSchemaInfo();
    const ExternalSchemaInfo *topSchemaInfo() const;
        // Return the current input schema info.
};

enum {
    // Enumeration of schema tags, numbered so that they can be used to index
    // elements in the CH::DESCRIPTOR_TABLE, below.  We use mixed-case
    // enumeration names so that the ASCII order exactly matches the sort
    // order of the corresponding element name strings (except for
    // 'XSTAG_NULL' and 'XSTAG_NUM_TAGS').  IMPORTANT: Except for
    // 'XSTAG_NULL', 'XSTAG_WILDCARD', and 'XSTAG_NUM_TAGS', these tags MUST
    // be kept in alphabetical order and MUST correspond one-to-one with the
    // entries in 'SchemaContentHandler', below.
    XSTAG_NULL,
    XSTAG_all,
    XSTAG_annotation,
    XSTAG_any,
    XSTAG_anyAttribute,
    XSTAG_appinfo,
    XSTAG_attribute,
    XSTAG_attributeGroup,
    XSTAG_choice,
    XSTAG_complexContent,
    XSTAG_complexType,
    XSTAG_documentation,
    XSTAG_element,
    XSTAG_enumeration,
    XSTAG_extension,
    XSTAG_field,
    XSTAG_fractionDigits,
    XSTAG_group,
    XSTAG_import,
    XSTAG_include,
    XSTAG_key,
    XSTAG_keyref,
    XSTAG_length,
    XSTAG_list,
    XSTAG_maxExclusive,
    XSTAG_maxInclusive,
    XSTAG_maxLength,
    XSTAG_minExclusive,
    XSTAG_minInclusive,
    XSTAG_minLength,
    XSTAG_notation,
    XSTAG_pattern,
    XSTAG_redefine,
    XSTAG_restriction,
    XSTAG_schema,
    XSTAG_selector,
    XSTAG_sequence,
    XSTAG_simpleContent,
    XSTAG_simpleType,
    XSTAG_totalDigits,
    XSTAG_union,
    XSTAG_unique,
    XSTAG_whiteSpace,
    XSTAG_WILDCARD,  // Does not correspond to a 'DESCRIPTOR_TABLE' entry
    XSTAG_NUM_TAGS = XSTAG_WILDCARD
};

#define XS_CONTENT(TAG1, TAG2, TAG3, TAG4) \
    (1ULL << unsigned(XSTAG_ ## TAG1) |    \
     1ULL << unsigned(XSTAG_ ## TAG2) |    \
     1ULL << unsigned(XSTAG_ ## TAG3) |    \
     1ULL << unsigned(XSTAG_ ## TAG4) | 1)
    // Return an unsigned long long with the appropriate bit set for up to
    // four tags.  XSTAG_NULL is always part of the resulting set.

// PRIVATE CLASS DATA

// Abbreviation for 'SchemaContentHandler' keeps table compact.
typedef class SchemaContentHandler CH;

// Dispatch table of for mapping XSD entities to parser actions.  IMPORTANT:
// The entries in this array MUST be kept in alphabetical order by name and
// MUST correspond one-to-one with the 'XSTAG' enumerations, above.
const CH::SchemaElementDescriptor CH::DESCRIPTOR_TABLE[] = {
    { "", /* NULL */    &CH::startBadDef,         &CH::endOtherDef,
        { XS_CONTENT(schema,      NULL,        NULL,        NULL         ) } },
    { "all",            &CH::startNoopDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  element,     NULL,        NULL         ) } },
    { "annotation",     &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(appinfo,     documentation, NULL,      NULL         ) } },
    { "any",            &CH::startBadDef,         &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "anyAttribute",   &CH::startBadDef,         &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "appinfo",        &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(WILDCARD,    NULL,        NULL,        NULL         ) } },
    { "attribute",      &CH::startAttrDef,        &CH::endAttrDef,
        { XS_CONTENT(annotation,  simpleType,  NULL,        NULL         ) } },
    { "attributeGroup", &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  attribute, attributeGroup, anyAttribute) } },
    { "choice",         &CH::startRecordDef,      &CH::endRecordDef,
        { XS_CONTENT(annotation,  element,     group,       choice       ) |
          XS_CONTENT(sequence,    any,         NULL,        NULL         ) } },
    { "complexContent", &CH::startNoopDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  restriction, extension,   NULL         ) } },
    { "complexType",    &CH::startTypeDef,        &CH::endTypeDef,
        { XS_CONTENT(annotation, simpleContent, complexContent, group    ) |
          XS_CONTENT(all,         choice,      sequence,    attribute    ) |
          XS_CONTENT(attributeGroup, anyAttribute, NULL,    NULL         ) } },
    { "documentation",  &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(WILDCARD,    NULL,        NULL,        NULL         ) } },
    { "element",        &CH::startElementDef,     &CH::endElementDef,
        { XS_CONTENT(annotation,  simpleType,  complexType, unique       ) |
          XS_CONTENT(key,         keyref,      NULL,        NULL         ) } },
    { "enumeration",    &CH::startEnumerationDef, &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
#ifdef ALLOW_UNSUPPORTED_FEATURES
    { "extension",      &CH::startRestrictionDef, &CH::endOtherDef,
        { XS_CONTENT(annotation,  group,       all,         choice       ) |
          XS_CONTENT(sequence,    attribute, attributeGroup, anyAttribute) } },
#else
    { "extension",      &CH::startBadDef,         &CH::endOtherDef,
        { XS_CONTENT(annotation,  group,       all,         choice       ) |
          XS_CONTENT(sequence,    attribute, attributeGroup, anyAttribute) } },
#endif
    { "field",          &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "fractionDigits", &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "group",          &CH::startGroupDef,       &CH::endOtherDef,
        { XS_CONTENT(annotation,  all,         choice,      sequence     ) } },
    { "import",         &CH::startImportDef,      &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "include",        &CH::startIncludeDef,     &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "key",            &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  selector,    field,       NULL         ) } },
    { "keyref",         &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  selector,    field,       NULL         ) } },
    { "length",         &CH::startLengthDef,      &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "list",           &CH::startListDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  simpleType,  NULL,        NULL         ) } },
    { "maxExclusive",   &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "maxInclusive",   &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "maxLength",      &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "minExclusive",   &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "minInclusive",   &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "minLength",      &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "notation",       &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "pattern",        &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "redefine",       &CH::startBadDef,         &CH::endOtherDef,
        { XS_CONTENT(annotation,  simpleType,  complexType, group        ) |
          XS_CONTENT(attributeGroup, NULL,     NULL,        NULL         ) } },
    { "restriction",    &CH::startRestrictionDef, &CH::endOtherDef,
        { XS_CONTENT(annotation,  group,       all,         choice       ) |
          XS_CONTENT(sequence,    attribute, attributeGroup, anyAttribute) |
          XS_CONTENT(minExclusive,minInclusive,maxExclusive,maxInclusive ) |
          XS_CONTENT(totalDigits,fractionDigits,length,     minLength    ) |
          XS_CONTENT(maxLength,   enumeration, whiteSpace,  pattern      ) } },
    { "schema",         &CH::startSchemaDef,      &CH::endSchemaDef,
        { XS_CONTENT(include,     import,      redefine,    annotation   ) |
          XS_CONTENT(simpleType,  complexType, group,      attributeGroup) |
          XS_CONTENT(element,     attribute,   notation,    NULL         ) } },
    { "selector",       &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "sequence",       &CH::startRecordDef,      &CH::endRecordDef,
        { XS_CONTENT(annotation,  element,     group,       choice       ) |
          XS_CONTENT(sequence,    any,         NULL,        NULL         ) } },
    { "simpleContent",  &CH::startNoopDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  restriction, extension,   NULL         ) } },
    { "simpleType",     &CH::startTypeDef,        &CH::endTypeDef,
        { XS_CONTENT(annotation,  restriction, list,        union        ) } },
    { "totalDigits",    &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } },
    { "union",          &CH::startBadDef,         &CH::endOtherDef,
        { XS_CONTENT(annotation,  simpleType,  NULL,        NULL         ) } },
    { "unique",         &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  selector,    field,       NULL         ) } },
    { "whiteSpace",     &CH::startSkipDef,        &CH::endOtherDef,
        { XS_CONTENT(annotation,  NULL,        NULL,        NULL         ) } }
};

const CH::SchemaElementDescriptor *const CH::DESCRIPTOR_TABLE_END =
    DESCRIPTOR_TABLE + XSTAG_NUM_TAGS;

inline
int SchemaContentHandler::verifyDescriptorTable() {
    // Verify that number of descriptors matches number of index enumerators.
    // This is a compile-time test.  BSLMF_ASSERT must be called in a member
    // function in order to have access to the private DESCRIPTOR_TABLE.
    BSLMF_ASSERT(XSTAG_NUM_TAGS == (sizeof DESCRIPTOR_TABLE /
                                    sizeof DESCRIPTOR_TABLE[0]));

    // Verify that there are fewer than 64 tags so that we can fit one tag per
    // bit in our within the ElementSet bit-set.  If this assertion fails,
    // then we must change to a different implementation of ElementSet.
    BSLMF_ASSERT(XSTAG_NUM_TAGS < sizeof(long long) * CHAR_BIT);

    return 0;
}

// PRIVATE ACCESSORS
inline
bool CH::SchemaElementDescriptor::isValidContent(int nestedXsTag) const
{
    return d_validContentTags.d_bits & (1ULL << (unsigned) nestedXsTag);
}

inline
int SchemaContentHandler::currentXsTag() const
{
    if (d_descriptorStack.empty()) {
        return XSTAG_NULL;
    }
    else {
        return static_cast<int>(d_descriptorStack.back() - DESCRIPTOR_TABLE);
    }
}

inline
int SchemaContentHandler::parentXsTag() const
{
    if (d_descriptorStack.size() < 2) {
        return XSTAG_NULL;
    }

    return static_cast<int>(
           d_descriptorStack[d_descriptorStack.size() - 2] - DESCRIPTOR_TABLE);
}

// PRIVATE MANIPULATORS
int
SchemaContentHandler::checkForExistingSchema(const bdeut_StringRef& location,
                                             int                    targetNsId)
{
    InputSchemaIterator it1 = d_inputSchemas.begin();
    InputSchemaIterator it2 = d_inputSchemas.end();

    for(; it1 != it2; ++it1) {
        if (bdeut_StringRef((*it1).schemaLocation()) == location &&
            (*it1).targetNsId() == targetNsId) {

            // Schema already is in the list
            ExternalSchemaInfo *found = &(*it1);

            // check for the same trunk
            ExternalSchemaInfo *info = &(*d_currentSchema);

            for (;info != 0; info = info->parent()) {
                if (info == found)
                    return -1;  // same trunk
            }

            return 0;
        }
    }
    return 1;
}

const char *
SchemaContentHandler::lookupAttribute(int         namespaceId,
                                      const char *localname) const
{
    baexml_ElementAttribute attr;
    if (d_reader->lookupAttribute(&attr, localname, namespaceId)) {
        return 0;
    }
    return attr.value();
}

void SchemaContentHandler::storeSchemaAttributes()
{
    const int numAttributes = d_reader->numAttributes();
    for (int i = 0; i < numAttributes; ++i) {
        baexml_ElementAttribute attribute;
        d_reader->lookupAttribute(&attribute, i);
        d_schemaAttributes[attribute.qualifiedName()] = attribute.value();
    }
}

bool SchemaContentHandler::getQnameAttribute(
    QualifiedSymbol *attributeSymbol,
    int              attributeNamespaceId,
    const char      *attributeName)
{
    const char *attrValueStr = lookupAttribute(attributeNamespaceId,
                                               attributeName);
    if (0 == attrValueStr || '\0' == attrValueStr) {
        return false;
    }

    ExternalSchemaInfo *info = currentSchemaInfo();
    BSLS_ASSERT(info != 0);

    const char *colon = bsl::strchr(attrValueStr, ':');
    if (0 == colon)
    {
        int defNsId = info->defaultNsId();
        *attributeSymbol = QualifiedSymbol(defNsId, attrValueStr);
    }
    else {
        bdeut_StringRef prefix(attrValueStr,
                               static_cast<int>(colon - attrValueStr));
        bdeut_StringRef localName(colon + 1);

        int nsId = info->getNamespaceIdByPrefix(prefix);
        *attributeSymbol = QualifiedSymbol(nsId, localName);
    }

    return true;
}

void SchemaContentHandler::loadPrimtiveTypes()
{
    const XMLTypeDescriptor *i;

    for (i = KNOWN_TYPES; i != KNOWN_TYPES_END; ++i) {
        QualifiedSymbol typeSymbol(NSID_XMLSCHEMA, i->d_xmlName);
        SchemaType& primType = const_cast<SchemaType&>(
            *d_schemaTypes.insert(SchemaType(typeSymbol)).first);
        primType.setXmlTypeDescriptor(i);
        primType.setMaxValue(i->d_maxValue);
    }
}

QualifiedSymbol SchemaContentHandler::generateLocalSymbol()
{
    // Name starts with a space to make it illegal.
    char name[] = " 000000_ANON";
    char *lastDigit = &name[4];

    int i = d_anonymousTypeCounter++;  BSLS_ASSERT(100000 > i);

    while (0 != i) {
        *lastDigit = static_cast<char>('0' + i % 10);
        i /= 10;
        --lastDigit;
    }

    return QualifiedSymbol(ANON_NAMESPACE, name);
}

void
SchemaContentHandler::createAnonymousUntaggedRecord(SchemaType     *parentType,
                                                    SchemaElement **retElem,
                                                    SchemaType    **retType)
{
    // Create an anonymous record type.
    QualifiedSymbol genSymbol(generateLocalSymbol());
    SchemaType *anonRecordType = &getType(genSymbol);
    anonRecordType->setXmlTypeDescriptor(&COMPLEX_TYPE_DESCRIPTOR);
    d_typeDefStack.push(anonRecordType);

    // Create an anonymous element of the anonymous type within the parent
    // record.
    SchemaElement *anonRecordElem =
        new(d_managedAllocator) SchemaElement(&d_managedAllocator);
    parentType->addField("UNTAGGED", anonRecordElem);
    anonRecordElem->setType(anonRecordType);
    anonRecordElem->addFormattingFlags(bdeat_FormattingMode::BDEAT_UNTAGGED);
    d_elementDefStack.push(anonRecordElem);

    *retElem = anonRecordElem;
    *retType = anonRecordType;
}

SchemaType& SchemaContentHandler::getType(const QualifiedSymbol& typeSymbol)
{
    ExternalSchemaInfo *info = currentSchemaInfo();
    BSLS_ASSERT(info != 0);

    bsl::pair<bsl::set<SchemaType>::iterator, bool> insertRet =
        d_schemaTypes.insert(SchemaType(typeSymbol));
    SchemaType& ret = const_cast<SchemaType&>(*insertRet.first);

    if (insertRet.second &&
        info->targetNsId() != typeSymbol.first &&
        ANON_NAMESPACE     != typeSymbol.first) {
        // We have just inserted a type that is not in the target namespace.
        // The user made an error and either misspelled a built-in data type,
        // or forgot to specify the namespace.
        setError("No such type: \"" + typeSymbol.second +
                 "\".  Missing or incorrect namespace prefix?");
    }
    else {
        const XMLTypeDescriptor *descriptor = ret.xmlTypeDescriptor();
        if (descriptor && bdem_ElemType::BDEM_VOID == descriptor->bdemType()) {
            // One of the handful of built-in XML types that we don't support.
            setError("Unsupported built-in XML type: " + typeSymbol.second);
        }
    }

    return ret;
}

int
SchemaContentHandler::checkElement(const bsl::string&  recordName,
                                   const bsl::string&  elemName,
                                   SchemaElement      *element)
{
    if (! element->isDefined()) {
        setError("No type specified for element \"" +
                 recordName + "." + elemName  + "\".");
        return -1;
    }

    SchemaType *elemType = element->type();
    if (0 == elemType) {
        setError("No type definition found for (indirect) element \"" +
                 recordName + "." + elemName  + "\".");
        return -1;
    }

     const XMLTypeDescriptor* xmlTypeDesc =
                              elemType->xmlTypeDescriptor();
     if (0 == xmlTypeDesc) {
        setError("Undefined type \"" +
                 elemType->symbol().second +
                 "\" referenced from element \"" +
                 recordName + "." + elemName  + "\".");
        return -1;
    }

    if (element->minOccurs() < 0) {
        setError("Invalid value of 'minOccurs' attribute for element \"" +
                 recordName + "." + elemName  + "\".");
        return -1;
    }
    if (element->minOccurs() > element->maxOccurs()) {
        setError("'maxOccurs' value must be greater or equal to "
                 "'minOccurs' value for element \"" +
                 recordName + "." + elemName  + "\".");
        return -1;
    }

    if (element->maxOccurs() > 1) {

        bdem_ElemType::Type bType = xmlTypeDesc->bdemType();
        if (bdem_ElemType::isArrayType(bType)) {
            setError("'maxOccurs' is not supported for array types on "
                     "element \"" + recordName + "." + elemName  + "\".");
            return -1;
        }
    }

    return 0;
}

int
SchemaContentHandler::generateBdemSchema(bdem_Schema *outputSchema)
{
    // Generate an output Bdem Schema after parsing all input XSD schemas.

    // clear the output Schema
    outputSchema->removeAll();

    //The implementation is "cut-and-paste" from old endSchemaDef() method

    // For each root (top-level) element of complex type, override the name of
    // the complex type to be the same as the element.  This is necessary so
    // that root element names appear in the constructed 'bdem_Schema', which
    // only concerns itself with record (complex) types.
    bsl::map<QualifiedSymbol, SchemaElement>::iterator elementsIter;
    for (elementsIter = d_topLevelElements.begin();
         elementsIter != d_topLevelElements.end(); ++elementsIter)
    {
        if (0 != checkElement("root",
                              elementsIter->first.second,
                              &elementsIter->second)) {
            return -1;
        }

        SchemaType *elemType = elementsIter->second.type();

        if (&COMPLEX_TYPE_DESCRIPTOR == elemType->xmlTypeDescriptor() &&
            elementsIter->first != elemType->symbol())
        {
            // Element symbol is different from type symbol.  Override type
            // symbol to match.
            if (! elemType->overrideSymbol().second.empty()) {
                // Type symbol has already been overridden.
                // Make an copy of the type.
                SchemaType elemTypeCopy = *elemType;

                QualifiedSymbol genSymbol(generateLocalSymbol());

                elemTypeCopy.setSymbol(genSymbol);

                elemTypeCopy.setOverrideSymbol(elementsIter->first);
                elemType = &const_cast<SchemaType&>(
                    *d_schemaTypes.insert(elemTypeCopy).first);
                elementsIter->second.setType(elemType);
            } // end if (type already overridden)
            elemType->setOverrideSymbol(elementsIter->first);
        } // End if (need to override symbol)
    } // End for each element

    if (d_verboseStream) {
        // Print out parse tree of types.
        bsl::set<SchemaType>::const_iterator typesIter;
        for (typesIter = d_schemaTypes.begin();
             typesIter != d_schemaTypes.end(); ++typesIter) {
            d_verboseStream << "Type ";
            typesIter->print(d_verboseStream);
            d_verboseStream << '\n' << bsl::endl;
        }
    }

    // Generate 'bdem_Schema' records for each complex top-level element.
    for (elementsIter = d_topLevelElements.begin();
         elementsIter != d_topLevelElements.end(); ++elementsIter)
    {
        if (&COMPLEX_TYPE_DESCRIPTOR ==
            elementsIter->second.type()->xmlTypeDescriptor())
        {
            generateTypeTree(outputSchema, elementsIter->second.type());
        }
    }

    return d_errorInfo.severity() >= baexml_ErrorInfo::BAEXML_ERROR ?
        -1 : 0;
}

void
SchemaContentHandler::generateTypeTree(bdem_Schema *outputSchema,
                                       SchemaType  *type)
{
    const XMLTypeDescriptor *typeDescriptor = type->xmlTypeDescriptor();

    if (0 == typeDescriptor) {
        setError("No definition for type \"" + type->symbol().second + "\".");
        return;
    }
    else if (&COMPLEX_TYPE_DESCRIPTOR == typeDescriptor) {
        type->sortAttributesBeforeElements();
        generateRecord(outputSchema, type);
    }
    else if (type->beginEnumerations() != type->endEnumerations()) {
        generateEnumeration(outputSchema, type);
    }
}

void
SchemaContentHandler::generateEnumeration(bdem_Schema *outputSchema,
                                          SchemaType  *type)
{
    // Create a 'bdem_EnumerationDef' for the specified XML simple 'type'.

    if (0 != type->bdemEnumeration()) {
        // A bdem_EnumerationDef has already been created for this simpleType.
        return;
    }

    // Extract type enumeration name, depending on whether or not the name
    // of the type has been overridden by the name of a schema-level
    // element referring to the type.
    const QualifiedSymbol& enumerationSymbol =
        type->overrideSymbol().second.empty() ? type->symbol()
                                              : type->overrideSymbol();
    const bsl::string& enumerationName = enumerationSymbol.second;

    // If enumeration name starts with a space, then it is a synthetic (and
    // invalid) name and should result in an unnamed enumeration.
    bool isUnnamedEnumeration = enumerationName.empty()
                            || ' ' == enumerationName[0]
                            || enumerationName.substr(0, 12) == "ENUMERATION_";

    const char *enumerationNameCstr = isUnnamedEnumeration
                                    ? 0 : enumerationName.c_str();

    bdem_EnumerationDef *enumeration =
        outputSchema->createEnumeration(enumerationNameCstr);
    if (0 == enumeration) {
        // The only reason 'enumeration' would be null is if a type by
        // that name already exists in the schema.  The only reason a
        // duplicate type would exist at this point in processing is if
        // a schema-level element overrides the name of a
        // type, causing it to collide with the name of a
        // different type.  For example:
        //..
        //  <simpleType name='Y'>...</simpleType>
        //  <element name='Y'><complexType>...</complexType></element>
        //..
        // The anonymous type defined by element Y inherit's its name from Y,
        // this causes a conflict with the other type named Y.
        setError("A type and a root element both have the name, \"" +
                 enumerationName + ",\" but element \"" +
                 enumerationName + "\" is not of type \"" +
                 enumerationName + ".\"");
        return;
    }

    type->setBdemEnumeration(enumeration);

    for (SchemaType::EnumerationIterator iter = type->beginEnumerations();
         iter != type->endEnumerations();
         ++iter) {
        enumeration->addEnumerator(iter->first.c_str(), iter->second);
    }

    if (!type->preserveEnumIds()) {
        // If none of the enumerators has an explicit ID, then assign the IDs
        // in alphabetical order so as to be consistent with the ASN.1
        // formula for assigning integer IDs to enumerators.
        enumeration->alphabetize();
    }
}

void
SchemaContentHandler::generateRecord(bdem_Schema *outputSchema,
                                     SchemaType  *type)
{
    // Create a 'bdem_RecordDef' for the specified XML complex 'type'.

    if (0 != type->bdemRecord()) {
        // A bdem_RecordDef has already been created for this complexType.
        // This check has the effect of stopping recursion, but is necessary
        // even if the schema is not recursive.
        return;
    }

    // Create 'bdem_RecordDef' for this XML complex 'type'.

    // Extract type record name, depending on whether or not the name
    // of the type has been overridden by the name of a schema-level
    // element referring to the type.
    const QualifiedSymbol& recordSymbol = type->overrideSymbol().second.empty()
                                        ? type->symbol()
                                        : type->overrideSymbol();
    const bsl::string& recordName = recordSymbol.second;

    // If record name starts with a space, then it is a synthetic (and invalid)
    // name and should result in an unnamed record.
    bool isUnnamedRecord = recordName.empty()
                        || ANON_NAMESPACE == recordSymbol.first
                        || recordName.substr(0, 7) == "RECORD_";
    const char *recordNameCstr = isUnnamedRecord ? 0 : recordName.c_str();

    const bdem_RecordDef::RecordType recordType = type->isChoice()
                                        ? bdem_RecordDef::BDEM_CHOICE_RECORD
                                        : bdem_RecordDef::BDEM_SEQUENCE_RECORD;
    bdem_RecordDef *thisRecord = outputSchema->createRecord(recordNameCstr,
                                                            recordType);
    if (0 == thisRecord) {
        // The only reason 'thisRecord' would be null is if a record by
        // that name already exists in the schema.  The only reason a
        // duplicate record would exist at this point in processing is if
        // a schema-level element overrides the name of a
        // <complexType>, causing it to collide with the name of a
        // different <complexType>.  For example:
        //..
        //  <complexType name='X'>...</complexType>
        //  <complexType name='Y'>...</complexType>
        //  <element name='Y' type='X'/>
        //..
        // Type X is renamed to Y because it is referenced by element
        // Y.  This causes a name collision with the other type named X.
        setError("A type and a root element both have the name, \"" +
                 recordName + ",\" but element \"" +
                 recordName + "\" is not of type \"" + recordName + ".\"");
        return;
    }

    type->setBdemRecord(thisRecord);

    // First pass: call 'generateTypeTree' recursively for each field of
    // 'type'.  Following the first pass, 'bdem' records will have been created
    // for all XML complex types reachable from 'type'.  This is necessary to
    // support forward references to records.

    for (SchemaType::FieldIterator fieldIter = type->beginFields();
         fieldIter != type->endFields(); ++fieldIter) {

        SchemaType *fieldType = fieldIter->second->type();

        if (type == fieldType) {
            continue;   // Direct recursion.
        }

        if (0 != checkElement(recordName,
                              fieldIter->first,
                              fieldIter->second)) {
            return;
        }
        generateTypeTree(outputSchema, fieldType);
    }

    // Second pass: add fields to 'thisRecord'.  Any records that constrain
    // fields within 'thisRecord' will have been created in the first pass.

    for (SchemaType::FieldIterator fieldIter = type->beginFields();
         fieldIter != type->endFields();
         ++fieldIter) {
        const bsl::string&   fieldName    = fieldIter->first;
        const SchemaElement *fieldElement = fieldIter->second;
        const SchemaType    *fieldType    = fieldElement->type();

        const bool isUnnamedField = (fieldElement->formattingMode() &
                                     bdeat_FormattingMode::BDEAT_UNTAGGED)
                                 || fieldName.empty()
                                 || fieldName.substr(0, 6) == "FIELD_";
        const char *fieldNameCstr = isUnnamedField ? 0 : fieldName.c_str();

        if (0 == fieldType) {
            continue;   // Error has already been reported.
        }

        bdem_ElemType::Type bdemType = fieldElement->bdemType();
        if (bdem_ElemType::BDEM_VOID == bdemType) {
            setError(recordName + "." + fieldName + " has no type.");
            continue;
        }

        const bdem_RecordDef *constraint = fieldType->bdemRecord();
        if (constraint) {
            BSLS_ASSERT(bdem_ElemType::isAggregateType(bdemType));

            // Fix up 'bdemType' to correspond to its constraining record and
            // the 'maxOccurs' attribute.

            const bdem_RecordDef::RecordType constraintRecordType =
                                                      constraint->recordType();
            const int maxOccurs = fieldElement->maxOccurs();

            if (bdem_RecordDef::BDEM_CHOICE_RECORD == constraintRecordType) {
                bdemType = 1 == maxOccurs ? bdem_ElemType::BDEM_CHOICE
                                          : bdem_ElemType::BDEM_CHOICE_ARRAY;
            }
            else {
                BSLS_ASSERT(
                 bdem_RecordDef::BDEM_SEQUENCE_RECORD == constraintRecordType);

                bdemType = 1 == maxOccurs ? bdem_ElemType::BDEM_LIST
                                          : bdem_ElemType::BDEM_TABLE;
            }
        }

        // Determine formatting mode for this field.

        const XMLTypeDescriptor *xmlTypeDesc = fieldType->xmlTypeDescriptor();
        if (0 == xmlTypeDesc) {
            continue;   // Error has already been reported.
        }

        int formattingMode = (fieldElement->formattingMode() |
                              xmlTypeDesc->formattingMode());

        if (fieldType->isList()) {
            formattingMode |= bdeat_FormattingMode::BDEAT_LIST;
        }

        bdem_FieldDefAttributes fieldAttr(bdemType);
        const bsl::string& defaultValue = fieldElement->defaultValue();
        if (! defaultValue.empty() &&
            ! bdem_ElemType::isAggregateType(bdemType)) {
            // Default value can be set for scalar and array types, but
            // not aggregate types
            if (parseDefaultValue(fieldAttr.defaultValue(), defaultValue)) {
                setError("Invalid default value " + defaultValue +
                         " specified for " + recordName + "." + fieldName);
            }
        }

        // An item is marked nullable if minOccurs == 0, but only if it is not
        // a repeated element and it does not have a default value.  (Repeated
        // elements are treated as arrays for which zero occurrences produces
        // a valid, empty array.  Default values prevent an element from being
        // null, even if it does not show up in the data stream.)
        const bool isNullable = (fieldElement->minOccurs() == 0 &&
                                 fieldElement->maxOccurs() <= 1 &&
                                 ! fieldAttr.hasDefaultValue());

        // TBD: Uncomment when we can support nillables
//         bool isNullable = false;
//         if (!fieldAttr.hasDefaultValue()
//          && !bdem_ElemType::isArrayType(bdemType)) {
//             if ((fieldElement->minOccurs() == 0
//               && fieldElement->maxOccurs() <= 1)
//               || formattingMode & bdeat_FormattingMode::BDEAT_NILLABLE) {
//                 isNullable = true;
//             }
//         }
        bdem_EnumerationDef *enumerationConstraint = 0;
        if (formattingMode & bdeat_FormattingMode::BDEAT_NILLABLE
         && bdem_ElemType::isArrayType(bdemType)
         && !bdem_ElemType::isAggregateType(bdemType)
         && defaultValue.empty()) {
            bdem_RecordDef *tmpRecord = outputSchema->createRecord(
                                         0,
                                         bdem_RecordDef::BDEM_SEQUENCE_RECORD);
            bdem_ElemType::Type baseType =
                                        bdem_ElemType::fromArrayType(bdemType);
            bdem_FieldDefAttributes
                            baseAttr(baseType,
                                     false,
                                     bdeat_FormattingMode::BDEAT_NILLABLE);

            const bdem_EnumerationDef *enumeration =
                                                 fieldType->bdemEnumeration();
            enumeration ? tmpRecord->appendField(baseAttr, enumeration)
                        : tmpRecord->appendField(baseAttr);
            fieldAttr.reset(bdem_ElemType::BDEM_TABLE);
            constraint = tmpRecord;
        }
        else if (fieldType->bdemEnumeration() && !constraint) {
            enumerationConstraint = fieldType->bdemEnumeration();
        }

        fieldAttr.setFormattingMode(formattingMode);
        fieldAttr.setIsNullable(isNullable);
        const bdem_FieldDef *field = enumerationConstraint
            ? thisRecord->appendField(fieldAttr,
                                      enumerationConstraint,
                                      fieldNameCstr,
                                      fieldElement->id())
            : thisRecord->appendField(fieldAttr,
                                      constraint,
                                      fieldNameCstr,
                                      fieldElement->id());
        if (0 == field) {
            setError(recordName + "." + fieldName + " is defined twice.");
        }
    }
}

int SchemaContentHandler::startSchemaDef(int)
{
    d_verboseStream << "[Starting schema definition]" << bsl::endl;

    ExternalSchemaInfo *info = currentSchemaInfo();
    BSLS_ASSERT(info != 0);

    bdeut_StringRef target = lookupAttribute(NULL_NSID, "targetNamespace");
    int tnsId = d_namespaces.lookupOrRegister(target);

    int rc = 0;
    switch(info->inclusionType())
    {
      case ExternalSchemaInfo::TOP_LEVEL:
        // Store the attributes provided with the schema
        storeSchemaAttributes();
        if (tnsId != -1) {
            info->setTargetNamespace(target);
        }
        break;
      case ExternalSchemaInfo::INCLUDE:
        if (tnsId != NULL_NSID &&         // not empty and
            tnsId != info->targetNsId())  // not expected
        {
            rc = -1;
        }
        break;
      case ExternalSchemaInfo::IMPORT:
        if (tnsId != info->targetNsId())  // not expected
        {
            rc = -1;
        }
        break;
      default:
        BSLS_ASSERT(0);
        break;
    }

    if (rc < 0) {
        bsl::string msg("Bad target namespace: got '");
        msg.append(target);
        msg.append("', expected '");
        msg.append(info->targetNamespace());
        msg.append("' schemaLocation='");
        msg.append(info->schemaLocation());
        msg.append("'");
        setError(msg);
    }

    d_verboseStream << "-- Got Target namespace: '" << target
                    << "', expected '" << info->targetNamespace()
                    << "', schemaLocation '" << info->schemaLocation()
                    << "'\n";
    d_verboseStream << "-- targetNamespace ID="
                    << info->targetNsId()
                    << bsl::endl;

    return rc;
}

int SchemaContentHandler::startElementDef(int)
{
    d_verboseStream << "[Starting element definition]" << bsl::endl;

    if (d_verboseStream) {
        baexml_ElementAttribute attr;
        for (int i = 0; i < d_reader->numAttributes(); ++i) {
            d_reader->lookupAttribute(&attr, i);
            d_verboseStream << "    Attribute "
                            << attr.qualifiedName() << "=\""
                            << attr.value() << '"' << bsl::endl;
        }
    }

    ExternalSchemaInfo *info = currentSchemaInfo();
    BSLS_ASSERT(info != 0);

    int targetNsId = info->targetNsId();

    bdeut_StringRef name = lookupAttribute(NULL_NSID, "name");

    // Check if new element is at top-level.  Note that we are testing the
    // *type* stack, not the *element* stack, since an element is either at
    // the top level or nested within a type.
    bool isTopLevel = d_typeDefStack.empty();

    SchemaElement *newElement = 0;
    if (isTopLevel) {
        newElement = &d_topLevelElements[QualifiedSymbol(targetNsId, name)];
        if (newElement->isDefined()) {
            setError("Duplicate definition of element, \"" + name + "\".");
            return -1;
        }
    }
    else {
        newElement =
                    new(d_managedAllocator) SchemaElement(&d_managedAllocator);
    }
    BSLS_ASSERT(newElement);

    d_elementDefStack.push(newElement);

    QualifiedSymbol attrSymbol;
    if (getQnameAttribute(&attrSymbol, NULL_NSID, "ref")) {
        if (isTopLevel) {
            setError("Top level elements may not have \"ref\" attributes");
            return -1;
        }

        SchemaElement *refElement = &d_topLevelElements[attrSymbol];
        if (! refElement->isDefined() &&
            targetNsId != attrSymbol.first) {
            // Catch common mistake of referring to an element without
            // specifying its namespace (if the target namespace is not the
            // default namespace).
            setError("No such element: \"" + attrSymbol.second +
                     "\".  Missing or incorrect namespace prefix?");
            return -1;
        }
        newElement->setElementRef(refElement);
        name = attrSymbol.second;
    }
    else if (getQnameAttribute(&attrSymbol, NULL_NSID, "type")) {
        d_verboseStream << "   Setting type to " << attrSymbol.first << ':'
                        << attrSymbol.second << bsl::endl;
        SchemaType *type = &getType(attrSymbol);
        newElement->setType(type);
    }

    if (name.isEmpty()) {
        setError("Element is missing name");
        return -1;
    }

    const char *minOccursStr = lookupAttribute(NULL_NSID, "minOccurs");
    if (minOccursStr) {
        int minOccurs = static_cast<int>(bsl::strtol(minOccursStr, 0, 10));
        d_verboseStream << "   Setting minOccurs to " << minOccurs
                        << bsl::endl;
        newElement->setMinOccurs(minOccurs);
    }

    const char *maxOccursStr = lookupAttribute(NULL_NSID, "maxOccurs");
    if (maxOccursStr) {
        int maxOccurs = INT_MAX;
        if (0 != bsl::strcmp("unbounded", maxOccursStr)) {
            maxOccurs = static_cast<int>(bsl::strtol(maxOccursStr, 0, 10));
        }
        // set max of existing and new value
        if (maxOccurs > newElement->maxOccurs()) {
            newElement->setMaxOccurs(maxOccurs);
        }

        d_verboseStream << "   Setting maxOccurs to "
                        << newElement->maxOccurs()
                        << bsl::endl;
    }

    bdeut_StringRef nillableStr = lookupAttribute(NULL_NSID, "nillable");
    if (nillableStr.data() && ("true" == nillableStr || "1" == nillableStr)) {
        d_verboseStream << "   Setting nillable to true" << bsl::endl;
        newElement->addFormattingFlags(bdeat_FormattingMode::BDEAT_NILLABLE);
    }

    bdeut_StringRef defaultStr = lookupAttribute(NULL_NSID, "default");
    bdeut_StringRef fixedStr = lookupAttribute(NULL_NSID, "fixed");

    if (fixedStr.data()) {
        if (defaultStr.data()) {
            setError("Mutually-exclusive attributes 'default' and 'fixed' "
                     "are both specified");
        }
        else {
            defaultStr = fixedStr;
        }
    }

    if (defaultStr.data()) {
        d_verboseStream << "   Setting default to " << defaultStr << bsl::endl;
        newElement->setDefaultValue(defaultStr);

        // Always force minOccurs to zero if a default value is specified.
        // That way, even default values that match the "null" value will
        // result in an optional element.
        d_verboseStream << "   Setting minOccurs to 0" << bsl::endl;
        newElement->setMinOccurs(0);
    }

    // Look for "bdem:id" attribute:
    const char *idStr = lookupAttribute(NSID_BDEM, "id");
    if (idStr) {
        d_verboseStream << "   Setting id to " << idStr << bsl::endl;
        newElement->setId(static_cast<int>(bsl::strtol(idStr, 0, 10)));
    }

    if (! isTopLevel) {
        // Element is part of a complex type.

        d_verboseStream << "   Adding element to type." << bsl::endl;
        d_typeDefStack.top()->addField(name, newElement);
    }

    return 0;
}

int SchemaContentHandler::startAttrDef(int)
{
    d_verboseStream << "[Starting attribute definition]" << bsl::endl;

    if (d_verboseStream) {
        baexml_ElementAttribute attr;
        for (int i = 0; i < d_reader->numAttributes(); ++i) {
            d_reader->lookupAttribute(&attr, i);
            d_verboseStream << "    Attribute "
                            << attr.qualifiedName() << "=\""
                            << attr.value() << '"' << bsl::endl;
        }
    }

    ExternalSchemaInfo *info = currentSchemaInfo();
    BSLS_ASSERT(info != 0);

    int targetNsId = info->targetNsId();
    bdeut_StringRef name = lookupAttribute(NULL_NSID, "name");

    // Check if new attribute is at top-level.  Note that we are testing the
    // *type* stack, not the *element* stack, since an attribute is either at
    // the top level or nested within a type.
    bool isTopLevel = d_typeDefStack.empty();

    // An attribute is represented the same way as an element in our
    // data structure.
    SchemaElement *newAttribute = 0;
    if (isTopLevel) {
        newAttribute =
            &d_topLevelAttributes[QualifiedSymbol(targetNsId, name)];
        if (newAttribute->isDefined()) {
            setError("Duplicate definition of attribute, \"" + name + "\".");
            return -1;
        }
    }
    else {
        newAttribute =
                    new(d_managedAllocator) SchemaElement(&d_managedAllocator);
    }
    BSLS_ASSERT(newAttribute);
    newAttribute->addFormattingFlags(bdeat_FormattingMode::BDEAT_ATTRIBUTE);

    d_elementDefStack.push(newAttribute);

    QualifiedSymbol attrSymbol;
    if (getQnameAttribute(&attrSymbol, NULL_NSID, "ref")) {
        if (isTopLevel) {
            setError("Top level attributes may not have \"ref\" attributes");
            return -1;
        }

        SchemaElement *refAttribute = &d_topLevelAttributes[attrSymbol];
        if (! refAttribute->isDefined() &&
            targetNsId != attrSymbol.first) {
            // Catch common mistake of referring to an attribute without
            // specifying its namespace (if the target namespace is not the
            // default namespace).
            setError("No such attribute: \"" + attrSymbol.second +
                     "\".  Missing or incorrect namespace prefix?");
            return -1;
        }
        newAttribute->setElementRef(refAttribute);
        name = attrSymbol.second;
    }
    else if (getQnameAttribute(&attrSymbol, NULL_NSID, "type")) {
        d_verboseStream << "   Setting type to " << attrSymbol.first << ':'
                        << attrSymbol.second << bsl::endl;
        SchemaType *type = &getType(attrSymbol);
        newAttribute->setType(type);
    }

    bdeut_StringRef useStr = lookupAttribute(NULL_NSID, "use");
    if (0 == useStr.length()
        || "optional" == useStr || "prohibited" == useStr) {
        d_verboseStream << "   Setting minOccurs to 0" << bsl::endl;
        newAttribute->setMinOccurs(0);
    }
    else if ("required" == useStr) {
        d_verboseStream << "   Setting minOccurs to 1" << bsl::endl;
        newAttribute->setMinOccurs(1);
    }
    else {
        setError("'use' attribute must have value 'optional', 'prohibited',"
                 " or 'required'.");
    }

    bdeut_StringRef defaultStr = lookupAttribute(NULL_NSID, "default");
    bdeut_StringRef fixedStr = lookupAttribute(NULL_NSID, "fixed");

    if (fixedStr.data()) {
        if (defaultStr.data()) {
            setError("Mutually-exclusive attributes 'default' and 'fixed' "
                     "are both specified");
        }
        else {
            defaultStr = fixedStr;
        }
    }

    if (defaultStr.data()) {
        d_verboseStream << "   Setting default to " << defaultStr << bsl::endl;
        newAttribute->setDefaultValue(defaultStr);

        // Always force minOccurs to zero if a default value is specified.
        // That way, even default values that match the "null" value will
        // result in an optional attribute.
        d_verboseStream << "   Setting minOccurs to 0" << bsl::endl;
        newAttribute->setMinOccurs(0);
    }

    // Look for "bdem:id" attribute:
    const char *idStr = lookupAttribute(NSID_BDEM, "id");
    if (idStr) {
        d_verboseStream << "   Setting id to " << idStr << bsl::endl;
        newAttribute->setId(static_cast<int>(bsl::strtol(idStr, 0, 10)));
    }

    if (name.isEmpty()) {
        setError("Attribute is missing a name");
        return -1;
    }

    if (! isTopLevel) {
        // Attribute is part of a complex type.

        SchemaType *topType = d_typeDefStack.top();
#ifndef ALLOW_UNSUPPORTED_FEATURES
        if (topType->isChoice() || EType::isScalarType(topType->bdemType())) {
            setError("Attributes are supported only on <sequence> and <all> "
                     "types.");
            return -1;
        }
#endif
        d_verboseStream << "   Adding attribute to type." << bsl::endl;
        topType->addField(name, newAttribute);
    }

    return 0;
}

int SchemaContentHandler::startEnumerationDef(int)
{
    d_verboseStream << "[Starting enumeration definition]" << bsl::endl;

    if (d_typeDefStack.empty()) {
        setError("Attempted to add enumeration to a non-type");
        return -1;
    }

    SchemaType *thisType = d_typeDefStack.top();
    SchemaType *baseType = thisType->baseType();
    QualifiedSymbol symbol = baseType->symbol();

    if ("string" != symbol.second) {
        // Ignore enumeration constraint on non-string types.
        return 0;
    }

    if (d_verboseStream) {
        baexml_ElementAttribute attr;
        for (int i = 0; i < d_reader->numAttributes(); ++i) {
            d_reader->lookupAttribute(&attr, i);
            d_verboseStream << "    Attribute "
                            << attr.qualifiedName() << "=\""
                            << attr.value() << '"' << bsl::endl;
        }
    }

    bdeut_StringRef value = lookupAttribute(NULL_NSID, "value");
    const char *idStr = lookupAttribute(NSID_BDEM, "id");
    int id = bdetu_Unset<int>::unsetValue();
    if (idStr) {
        id = static_cast<int>(bsl::strtol(idStr, 0, 10));
    }

    thisType->addEnumeration(value, id);

    return 0;
}

int SchemaContentHandler::startTypeDef(int currXsTag)
{
    d_verboseStream << "[Starting type definition]" << bsl::endl;

    if (d_verboseStream) {
        baexml_ElementAttribute attr;
        for (int i = 0; i < d_reader->numAttributes(); ++i) {
            d_reader->lookupAttribute(&attr, i);
            d_verboseStream << "    Attribute "
                            << attr.qualifiedName() << "=\""
                            << attr.value() << '"' << bsl::endl;
        }
    }

    bdeut_StringRef name = lookupAttribute(NULL_NSID, "name");

    ExternalSchemaInfo *info = currentSchemaInfo();
    BSLS_ASSERT(info !=0);

    int targetNsId = info->targetNsId();

    // Check if new type is at top-level.  Note that we are testing the
    // *element* stack, not the *type* stack, since a type is either at the
    // top level or nested within an element.
    bool isTopLevel = d_elementDefStack.empty();

    SchemaType *newType = 0;
    if (isTopLevel) {
        if (name.isEmpty()) {
            setError("Top-level type is missing name.");
            return -1;
        }

        newType = &getType(QualifiedSymbol(targetNsId, name));
        if (newType->isDefined()) {
            setError("Duplicate definition of type, \"" + name + "\".");
            return -1;
        }
    }
    else
    {
        if (! name.isEmpty()) {
            setError("Only top-level types may have names.");
            return -1;
        }

        QualifiedSymbol genSymbol(generateLocalSymbol());

        newType = &getType(genSymbol);

        d_elementDefStack.top()->setType(newType);
    }

    if (XSTAG_complexType == currXsTag) {
        newType->setXmlTypeDescriptor(&COMPLEX_TYPE_DESCRIPTOR);
    }
    else if (XSTAG_simpleType == currXsTag) {
        bdeut_StringRef preserveEnumOrder =
            lookupAttribute(NSID_BDEM, "preserveEnumOrder");
        if (preserveEnumOrder == "true" || preserveEnumOrder == "1") {
            newType->setPreserveEnumIds(true);
        }
    }

    d_typeDefStack.push(newType);

    return 0;
}

int SchemaContentHandler::startRecordDef(int currXsTag)
{
    d_verboseStream << "[Start choice or sequence record]" << bsl::endl;

    bool isChoice = (XSTAG_choice == currXsTag);

    SchemaType *recordType = 0;
    SchemaElement *parentElement = 0;

    if (lookupAttribute(NULL_NSID, "name")) {
        // Catch a common misuse.  XSD standard allows only the
        // following attributes from schema namespace:
        // "id", "maxOccurs", "minOccurs"
        setError("<choice> or <sequence> can not have a name");
        return -1;
    }

    int minOccurs = 1;
    int maxOccurs = 1;

    const char *minOccursStr = lookupAttribute(NULL_NSID, "minOccurs");
    if (minOccursStr) {
        minOccurs = static_cast<int>(bsl::strtol(minOccursStr, 0, 10));
        d_verboseStream << "   Setting minOccurs to " << minOccurs
                        << bsl::endl;
    }

    const char *maxOccursStr = lookupAttribute(NULL_NSID, "maxOccurs");
    if (maxOccursStr) {
        maxOccurs = INT_MAX;
        if (0 != bsl::strcmp("unbounded", maxOccursStr)) {
            maxOccurs = static_cast<int>(bsl::strtol(maxOccursStr, 0, 10));
        }
        d_verboseStream << "   Setting maxOccurs to " << maxOccurs
                        << bsl::endl;
    }

    switch (parentXsTag()) {
      case XSTAG_complexType: {
        // <choice>, <sequence>, or <all> within a <complexType>

        BSLS_ASSERT(! d_typeDefStack.empty());

        recordType = d_typeDefStack.top();
        parentElement = (d_elementDefStack.empty() ?
                         0 : d_elementDefStack.top());

        if (isChoice && recordType->beginFields() != recordType->endFields()) {
            setError("Cannot define a <choice> with previously-defined "
                     "elements or attributes.");
            return -1;
        }

        if ((0 == parentElement && minOccurs < 1) || maxOccurs > 1) {
            // bdem_Schema cannot represent a top-level aggregate that is
            // nullable or is an array.  We create an extra sequence record
            // having a single, untagged element of the record type.
            SchemaType *parentType = recordType;
            createAnonymousUntaggedRecord(parentType,
                                          &parentElement, &recordType);
        }

      } break;

      case XSTAG_sequence:
      case XSTAG_choice: {
        // Anonymous <choice> or <sequence> nested within another
        // <choice> or <sequence>.

        BSLS_ASSERT(! d_typeDefStack.empty());
        SchemaType *parentType = d_typeDefStack.top();

        if (XSTAG_all == currXsTag) {
            // No such thing as an anonymous <all>.
            setError("<all> is valid only within <complexType>.");
            return -1;
        }

        // Create an anonymous element of anonymous record type within the
        // parent record.
        createAnonymousUntaggedRecord(parentType, &parentElement, &recordType);
      } break;

      default: {
        bsl::string msg("<");
        msg += DESCRIPTOR_TABLE[currXsTag].d_name;
        msg += "> is valid only in <complexType>, <sequence>, or <choice>.";
        setError(msg);
        return -1;
      }
    } // end switch

    recordType->setIsChoice(isChoice);

    if (parentElement) {
        if (minOccurs < parentElement->minOccurs()) {
            parentElement->setMinOccurs(minOccurs);
        }
        if (maxOccurs > parentElement->maxOccurs()) {
            parentElement->setMaxOccurs(maxOccurs);
        }
    }

    return 0;
}

int
SchemaContentHandler::startRestrictionDef(int)
{
    d_verboseStream << "[Starting restriction definition]" << bsl::endl;

    if (d_typeDefStack.empty()) {
        setError("Attempted to restrict a non-type");
        return -1;
    }

    QualifiedSymbol baseTypeSymbol;
    if (! getQnameAttribute(&baseTypeSymbol, NULL_NSID, "base")) {
        setError("No base type specified in extension/restriction");
        return -1;
    }

    d_verboseStream << "   Base type = " << baseTypeSymbol.first << ':'
                    << baseTypeSymbol.second << bsl::endl;

    SchemaType *thisType = d_typeDefStack.top();
    SchemaType *baseType = &getType(baseTypeSymbol);
    if (baseType == thisType || baseType->mostBaseType() == thisType) {
        setError("Recursive extension or restriction of type \"" +
                 thisType->symbol().second + "\".");
        return -1;
    }
    thisType->setBaseType(baseType);

    return 0;
}

int SchemaContentHandler::startListDef(int)
{
    d_verboseStream << "[list]" << bsl::endl;

    QualifiedSymbol itemTypeSymbol;
    if (! getQnameAttribute(&itemTypeSymbol, NULL_NSID, "itemType")) {
        setError("No itemType specified for list");
        return -1;
    }

    SchemaType *itemType = &getType(itemTypeSymbol);
    SchemaType *topType = d_typeDefStack.top();
    topType->setBaseType(itemType);
    topType->setIsList(true);

    return 0;
}

int SchemaContentHandler::startLengthDef(int currXsTag)
{
    // Skip for now.
    return startSkipDef(currXsTag);
}

int SchemaContentHandler::startGroupDef(int)
{
    setError("Groups are not yet supported");
    ++d_skippedElementDepth;        // Skip rest of this element
    return 0;
}

int SchemaContentHandler::startNoopDef(int currXsTag)
{
    d_verboseStream << "[Noop "
                    << DESCRIPTOR_TABLE[currXsTag].d_name
                    << "]" << bsl::endl;
    return 0;
}

int SchemaContentHandler::startSkipDef(int currXsTag)
{
    d_verboseStream << "[Skipping "
                    << DESCRIPTOR_TABLE[currXsTag].d_name << "]"
                    << bsl::endl;
    ++d_skippedElementDepth;        // Skip rest of this element
    return 0;
}

int SchemaContentHandler::startIncludeDef(int currXsTag)
{
    ///Processing '<include>' and '<import>' constructs
    ///-------------------------------------------------
    // The 'baexml_SchemaParser' starts by parsing initial (top-level) schema.
    // When the parser comes across the '<include>' or '<import>' construct,
    // it pushes the information (schemaLocation and targetNamespace) onto the
    // list of external schemas.  Then parser continues parsing the current
    // schema and building the internal schema model.  Note that the parser
    // supports "deferred" types and elements resolution, so it is not
    // necessary to parse included schemas immediately.  When the processing
    // of the current schema is finished, the parser checks the list of the
    // external schemas.  If there are no more schemas in the list, the parser
    // generates the 'bdem_Schema' and returns.  Otherwise the parser picks up
    // the next schema to process, resolves the schema via the
    // 'SchemaResolverFunctor', and starts processing the new schema with the
    // existing internal in-memory model.

    d_verboseStream << "["
                    << DESCRIPTOR_TABLE[currXsTag].d_name
                    << "]" << bsl::endl;

    if (!d_typeDefStack.empty() || !d_elementDefStack.empty()) {
        setError("Constuct <include> must be directly under the root");
        return -1;
    }

    bdeut_StringRef location = lookupAttribute(NULL_NSID, "schemaLocation");
    if (location.isEmpty()) {
        setError("attribute 'schemaLocation' is empty");
        return -1;
    }

    ExternalSchemaInfo *oldInfo = currentSchemaInfo();
    BSLS_ASSERT(oldInfo != 0);

    // Get current target namespace:
    int oldTargetNsId = oldInfo->targetNsId();
    const char *oldTargetNsStr = d_namespaces.lookup(oldTargetNsId);

    ExternalSchemaInfo newInfo(ExternalSchemaInfo::INCLUDE,
                               oldInfo,
                               location,
                               oldTargetNsStr,
                               &d_managedAllocator);

    if (checkForExistingSchema(location, newInfo.targetNsId()) <= 0) {

        d_verboseStream << "  Schema already included. "
                        << "Ignored schemaLocation='"
                        << location
                        << "' targetNamespace="
                        << oldTargetNsStr
                        << "'" << bsl::endl;

        return 0;
    }

    d_inputSchemas.push_back(newInfo);

    d_verboseStream << "  included schemaLocation='"
                    << location
                    << "' targetNamespace='"
                    << oldTargetNsStr
                    << "'" << bsl::endl;

    return 0;
}

int SchemaContentHandler::startImportDef(int currXsTag)
{
    ///Processing '<include>' and '<import>' constructs
    ///-------------------------------------------------
    // The 'baexml_SchemaParser' starts by parsing initial (top-level) schema.
    // When the parser comes across the '<include>' or '<import>' construct,
    // it pushes the information (schemaLocation and targetNamespace) onto the
    // list of external schemas.  Then parser continues parsing the current
    // schema and building the internal schema model.  Note that the parser
    // supports "deferred" types and elements resolution, so it is not
    // necessary to parse included schemas immediately.  When the processing
    // of the current schema is finished, the parser checks the list of the
    // external schemas.  If there are no more schemas in the list, the parser
    // generates the 'bdem_Schema' and returns.  Otherwise the parser picks up
    // the next schema to process, resolves the schema via the
    // 'SchemaResolverFunctor' and starts adding the new schema to the
    // existing internal schema model.

    d_verboseStream << "["
                    << DESCRIPTOR_TABLE[currXsTag].d_name
                    << "]" << bsl::endl;

    if (!d_typeDefStack.empty() || !d_elementDefStack.empty()) {
        setError("Constuct <import> must be directly under the root");
        return -1;
    }

    ExternalSchemaInfo *oldInfo = currentSchemaInfo();
    BSLS_ASSERT(oldInfo != 0);

    // Get the target namespace for the schema being parsed.
    int oldTargetNsId = oldInfo->targetNsId();
    const char *oldTargetNsStr = d_namespaces.lookup(oldTargetNsId);

    // Look up schema location.  For <import>, 'location' is permitted to be
    // empty.
    bdeut_StringRef location = lookupAttribute(NULL_NSID, "schemaLocation");

    bdeut_StringRef newTargetNsStr = lookupAttribute(NULL_NSID,
                                                     "namespace");

    if (newTargetNsStr.isEmpty()) {
        setError("attribute 'namespace' must be specified for <import>");
        return -1;
    }
    else if (newTargetNsStr == oldTargetNsStr) {
        setError("attribute 'namespace' may not match outer schema's "
                 "target namespace");
        return -1;
    }

    ExternalSchemaInfo newInfo(ExternalSchemaInfo::IMPORT,
                               oldInfo,
                               location,
                               newTargetNsStr,
                               &d_managedAllocator);

    if (checkForExistingSchema(location, newInfo.targetNsId()) <= 0) {

        d_verboseStream
            << "  Schema already included. Ignored schemaLocation="
            << location
            << " targetNamespace="
            << oldTargetNsStr
            << bsl::endl;

        return 0;
    }

    d_inputSchemas.push_back(newInfo);

    d_verboseStream << "  imported schemaLocation="
        << location
        << " targetNamespace="
        << newTargetNsStr
        << bsl::endl;

    return 0;
}

int SchemaContentHandler::startBadDef(int currXsTag)
{
    setError(bsl::string("Unsupported schema element: ") +
             DESCRIPTOR_TABLE[currXsTag].d_name);
    ++d_skippedElementDepth;        // Skip rest of this element
    return 0;
}

int SchemaContentHandler::endSchemaDef(int)
{
    d_verboseStream << "[Ending schema definition]" << bsl::endl;

    return 0;
}

int SchemaContentHandler::endElementDef(int)
{
    d_verboseStream << "[Ending element definition]" << bsl::endl;

    BSLS_ASSERT(d_elementDefStack.size());
    if (! d_elementDefStack.top()->isDefined()) {
        setError("Element definition has no type");
        return -1;
    }
    d_elementDefStack.pop();

    // Note that 'SchemaElement*' pointers are intentionally *not* deleted when
    // they are popped from 'd_elementDefStack'.  The 'SchemaElement' objects
    // that are referenced are either (1) owned by the 'd_topLevelElements'
    // map, or (2) constructed using 'd_managedAllocator' (which is a
    // 'bdema_SequentialAllocator').

    return 0;
}

int SchemaContentHandler::endAttrDef(int currXsTag)
{
    return endElementDef(currXsTag);
}

int SchemaContentHandler::endTypeDef(int)
{
    d_verboseStream << "[Ending type definition]" << bsl::endl;

    d_typeDefStack.pop();
    return 0;
}

int SchemaContentHandler::endRecordDef(int)
{
    d_verboseStream << "[Ending record definition]" << bsl::endl;

    if (d_elementDefStack.size() &&
        (d_elementDefStack.top()->formattingMode() &
         bdeat_FormattingMode::BDEAT_UNTAGGED)) {
        // Ending an anonymous <choice> or <sequence>
        // (not inside a <complexType> or <simpleType> construct).
        d_elementDefStack.pop();
        d_typeDefStack.pop();
    }

    return 0;
}

int SchemaContentHandler::endOtherDef(int currXsTag)
{
    d_verboseStream << "[Ending "
                    << DESCRIPTOR_TABLE[currXsTag].d_name
                    << "]" << bsl::endl;
    return 0;
}

// CREATORS
SchemaContentHandler::SchemaContentHandler(baexml_Reader          *reader,
                                           const bdeut_StringRef&  location)
: d_managedAllocator(&bslma_NewDeleteAllocator::singleton())
, d_emptyString(&d_managedAllocator)
, d_namespaces(&d_managedAllocator)
, d_inputSchemas(&d_managedAllocator)
, d_currentSchema(d_inputSchemas.end())
, d_reader(reader)
, d_errorInfo(&d_managedAllocator)
, d_verboseStream(0)
, d_topLevelElements(bsl::less<QualifiedSymbol>(), &d_managedAllocator)
, d_topLevelAttributes(bsl::less<QualifiedSymbol>(), &d_managedAllocator)
, d_schemaTypes(bsl::less<SchemaType>(), &d_managedAllocator)
// Stacks do not make efficient use of managed allocator.  Use default
// allocator for the following three stacks:
, d_descriptorStack()
, d_elementDefStack()
, d_typeDefStack()
, d_schemaAttributes(&d_managedAllocator)
, d_skippedElementDepth(0)
, d_anonymousTypeCounter(0)
{
    d_descriptorStack.reserve(32);

    loadPrimtiveTypes();

    // top Level schema
    ExternalSchemaInfo topInfo(&d_namespaces,
                               location,
                               &d_managedAllocator);
    d_inputSchemas.push_back(topInfo);
    d_currentSchema = d_inputSchemas.begin();
}

SchemaContentHandler::~SchemaContentHandler()
{
}

// MANIPULATORS
bool
SchemaContentHandler::advanceToNextSchema()
{
    if (d_currentSchema != d_inputSchemas.end())
    {
        ++d_currentSchema;
    }
    return (d_currentSchema != d_inputSchemas.end());
}

void
SchemaContentHandler::setError(const bsl::string& msg,
                               Severity           severity)
{

    ExternalSchemaInfo *schemaInfo = currentSchemaInfo();

    if (schemaInfo == 0) {
        schemaInfo = topSchemaInfo();
    }

    const bsl::string& srcId = schemaInfo
                             ? schemaInfo->schemaLocation()
                             : d_emptyString;

    d_errorInfo.setError(severity,
                         d_reader->getLineNumber(),
                         d_reader->getColumnNumber(),
                         srcId,
                         msg);

    if (d_verboseStream) {

        // d_error may contain previous high-level error
        // as setError does not override the high-level error

        baexml_ErrorInfo tmp;
        tmp.setError(severity,
                     d_reader->getLineNumber(),
                     d_reader->getColumnNumber(),
                     srcId,
                     msg);

        d_verboseStream << tmp << bsl::endl;
    }
}

inline
void SchemaContentHandler::setVerbose(bsl::streambuf *buf)
{
    d_verboseStream.rdbuf(buf);
}

void
SchemaContentHandler::dispatchEndElement()
{
    BSLS_ASSERT(d_descriptorStack.size());

    if (d_skippedElementDepth > 0) {
        // This element was skipped.
        --d_skippedElementDepth;
    }
    else {
        const SchemaElementDescriptor *tagInfo = d_descriptorStack.back();
        int tag = static_cast<int>(tagInfo - DESCRIPTOR_TABLE);
        (this->*(tagInfo->d_endFunction))(tag);
    }

    d_descriptorStack.pop_back();
}

void
SchemaContentHandler::dispatchStartElement()
{
    const char *localname = d_reader->nodeLocalName();
    if (localname == 0) {
        localname = "";
    }

    if (NSID_XMLSCHEMA != d_reader->nodeNamespaceId() &&
        0 == d_skippedElementDepth) {
        // Element is not in the 'xs' namespace.
        // Only set error if this is not a skipped element

        bsl::string msg("Unknown schema element namespace: ");
        msg.append(d_reader->nodeNamespaceUri());
        msg.append(":");
        msg.append(localname);
        setError(msg);
        return;
    }

    // Binary search for element tag in array of elements.
    // Can't use bsl::lower_bound because the value we are searching for is
    // not the same type as the contents of the array.
    // Note: skip the NULL element of DESCRIPTOR_TABLE when searching.
    const SchemaElementDescriptor *lo = DESCRIPTOR_TABLE + XSTAG_NULL + 1;
    const SchemaElementDescriptor *hi = DESCRIPTOR_TABLE_END;

    int test = 1;
    const SchemaElementDescriptor *mid = 0;
    while (0 != test && lo < hi) {
        // Invariant A: lo->d_name <= localname && localname < hi->d_name

        mid = lo + (hi - lo) / 2;
        // Invariant B: lo <= mid && mid < hi

        test = bsl::strcmp(localname, mid->d_name);
        if (test < 0) {
            hi = mid;
            // Invariant B ensures that new hi < old hi and new hi >= lo
        }
        else if (test > 0) {
            lo = mid + 1;
            // Invariant B ensures that new lo > old lo and new lo <= hi
        }
        // else we found what we were looking for.
    }

    if (0 != test) {
        // tag not found in array.

        if (0 == d_skippedElementDepth) {
            // Only set error if this is not a skipped element
            bsl::string msg("Unknown schema element tag: ");
            msg.append(localname);
            setError(msg);
            return;
        }

        mid = &DESCRIPTOR_TABLE[ XSTAG_NULL ];
    }

    // Perform error checking that '<mid>' is a legitimate child of the
    // current tag.
    int tag = static_cast<int>(mid - DESCRIPTOR_TABLE);
    const SchemaElementDescriptor *parent = (d_descriptorStack.empty()       ?
                                             &DESCRIPTOR_TABLE[ XSTAG_NULL ] :
                                             d_descriptorStack.back());
    if (! parent->isValidContent(tag) &&
        ! parent->isValidContent(XSTAG_WILDCARD)) {
        bsl::string msg("<");
        msg.append(localname);
        if (d_descriptorStack.empty()) {
            msg.append("> is not valid at the top level.  Expected <schema>.");
        }
        else {
            msg.append("> is not valid within <");
            msg.append(parent->d_name);
            msg.append(">.  Expected ");

            // Create a nicely-formatted list of expected elements:
            int numExpected = 0;
            const char *expectedTag = "NOTHING";
            const char *conjunction = "<";
            for (int i = XSTAG_NULL + 1; i < XSTAG_NUM_TAGS; ++i) {
                if (parent->isValidContent(i) &&
                    DESCRIPTOR_TABLE[i].d_startFunction != &CH::startBadDef) {
                    if (numExpected) {
                        // Append previous valid tag
                        msg.append(conjunction);
                        msg.append(expectedTag);
                        conjunction = ">, <";
                    }

                    expectedTag = DESCRIPTOR_TABLE[i].d_name;
                    ++numExpected;
                } // End if isValidContent
            } // end for i

            const char *suffix = ">.";
            switch (numExpected) {
              case 0:  conjunction = ""; suffix = "."; break;
              case 1:  conjunction = "<";              break;
              case 2:  conjunction = "> or <";         break;
              default: conjunction = ">, or <";        break;
            }

            // Append last valid tag
            msg.append(conjunction);
            msg.append(expectedTag);
            msg.append(suffix);
        }

        setError(msg);
        return;
    }

    d_descriptorStack.push_back(mid);

    if (d_skippedElementDepth > 0) {
        // Parent element is being skipped, so skip this one, too.
        ++d_skippedElementDepth;
    }
    else {
        (this->*(mid->d_startFunction))(tag);
    }
}

void
SchemaContentHandler::parseCurrentSchema(bsl::streambuf  *inputStream)
{
    ExternalSchemaInfo  *schemaInfo  = currentSchemaInfo();
    baexml_PrefixStack  *oldPrefixes = d_reader->prefixStack();

    d_reader->setPrefixStack(schemaInfo->prefixStack());

    if (0 != d_reader->open(inputStream, schemaInfo->schemaLocation().c_str()))
    {
        setError("Unable to open schema document '" +
                 schemaInfo->schemaLocation() +
                 "' for parsing");
    }

    while (d_errorInfo.severity() < baexml_ErrorInfo::BAEXML_ERROR)
    {
        int rc = d_reader->advanceToNextNode();
        if (rc < 0) {
            d_errorInfo.setError(d_reader->errorInfo());
            setError("Schema parser error");
            break;
        }
        else if (rc > 0) {
            break;  // Finished parsing
        }

        if (d_errorInfo.severity() < d_reader->errorInfo().severity()) {
            d_errorInfo.setError(d_reader->errorInfo());
        }

        baexml_Reader::NodeType nodeType = d_reader->nodeType();

        switch(nodeType)
        {
          case baexml_Reader::BAEXML_NODE_TYPE_ELEMENT:
            dispatchStartElement();
            if (d_reader->isEmptyElement() &&
                d_errorInfo.severity() < baexml_ErrorInfo::BAEXML_ERROR) {
                dispatchEndElement();
            }
            break;
          case baexml_Reader::BAEXML_NODE_TYPE_END_ELEMENT:
            dispatchEndElement();
            break;
          default:
            break;
        }
    }

    d_reader->close();
    d_reader->setPrefixStack(oldPrefixes);
}

// ACCESSORS
bsl::ostream&
SchemaContentHandler::verboseStream()
{
    return d_verboseStream;
}

inline
const baexml_SchemaParser::SchemaElementAttributes&
SchemaContentHandler::schemaAttributes() const
{
    return d_schemaAttributes;
}

inline
const char *SchemaContentHandler::targetNamespace() const
{
    const ExternalSchemaInfo *info = topSchemaInfo();

    const int nsId = 0 == info ? NULL_NSID : info->targetNsId();

    return d_namespaces.lookup(nsId);
}

inline
const baexml_ErrorInfo& SchemaContentHandler::errorInfo() const
{
    return d_errorInfo;
}

ExternalSchemaInfo *
SchemaContentHandler::currentSchemaInfo()
{
    if (d_currentSchema != d_inputSchemas.end())
        return &(*d_currentSchema);

    return 0;
}
const ExternalSchemaInfo *
SchemaContentHandler::currentSchemaInfo() const
{
    if (d_currentSchema != d_inputSchemas.end())
        return &(*d_currentSchema);

    return 0;
}

ExternalSchemaInfo *
SchemaContentHandler::topSchemaInfo()
{
    if (d_inputSchemas.empty())
        return 0;

    return &d_inputSchemas.front();
}

const ExternalSchemaInfo *
SchemaContentHandler::topSchemaInfo() const
{
    if (d_inputSchemas.empty())
        return 0;

    return &d_inputSchemas.front();
}

} // Close unnamed namespace

////////////////////////////////////////////////////////////////////////

int baexml_SchemaParser::parse(bsl::streambuf          *inputStream,
                               bdem_Schema             *schema,
                               bsl::string             *targetNamespace,
                               SchemaElementAttributes *attributes,
                               const bdeut_StringRef&   inputId)
{
    SchemaContentHandler contentHandler(reader(), inputId);
    const baexml_ErrorInfo& parseErrInfo = contentHandler.errorInfo();

    if (d_verboseStream) {
        contentHandler.setVerbose(d_verboseStream->rdbuf());
    }

    baexml_Reader::XmlResolverFunctor resolver = d_reader->resolver();

    // parse main top level schema
    contentHandler.parseCurrentSchema(inputStream);

    // So long as there are additional schemas to parse (i.e., from <import>
    // and <include> constructs, parse the next schema:
    while (parseErrInfo.severity() <= baexml_ErrorInfo::BAEXML_WARNING &&
           contentHandler.advanceToNextSchema())  {

        ExternalSchemaInfo* schemaInfo = contentHandler.currentSchemaInfo();

        baexml_Reader::StreamBufPtr streamPtr;
        if (resolver) {
            streamPtr = resolver(schemaInfo->schemaLocation().c_str(),
                                 schemaInfo->targetNamespace());
        }

        if (!streamPtr) {
            bsl::string msg("Unable to resolve external schema "
                            "for the schemaLocation = " );
            msg.append(schemaInfo->schemaLocation());

            contentHandler.verboseStream() << msg << bsl::endl;
            contentHandler.setError(msg);
        }
        else  {
            contentHandler.parseCurrentSchema(streamPtr.ptr());
        }
    }

    if (parseErrInfo.severity() <= baexml_ErrorInfo::BAEXML_WARNING) {

        if (targetNamespace) {
            *targetNamespace = contentHandler.targetNamespace();
        }

        if (attributes) {
            *attributes = contentHandler.schemaAttributes();
        }

        contentHandler.generateBdemSchema(schema);
    }

    if (d_errorInfo != 0 &&
        parseErrInfo.severity() != baexml_ErrorInfo::BAEXML_NO_ERROR) {

        d_errorInfo->setError(parseErrInfo);
    }

    if (parseErrInfo.severity() > baexml_ErrorInfo::BAEXML_WARNING) {
        return -1;
    }

    return 0;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
