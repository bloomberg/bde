// bdem_recorddef.h                                                   -*-C++-*-
#ifndef INCLUDED_BDEM_RECORDDEF
#define INCLUDED_BDEM_RECORDDEF

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a record definition characterizing a sequence of fields.
//
//@CLASSES:
//  bdem_RecordDef: record definition comprising a field-definition sequence
//
//@SEE_ALSO: bdem_fielddef, bdem_schema
//
//@AUTHOR: Tom Marshall, Henry Verschell
//
//@DESCRIPTION: This component implements a collaborative type,
// 'bdem_RecordDef', that is an integral part of a fully-value semantic schema
// object.  'bdem_RecodDef' objects serve as metadata describing the
// structure of associated records.  Each 'bdem_RecordDef' object contains a
// sequence of field definitions describing the type, nullability, default
// value, and text formatting of a field, as well as any constraint on the
// possible values of the field.  In general, clients should *not* construct a
// 'bdem_RecordDef' directly, but instead obtain a record definition from a
// 'bdem_Schema' (see 'bdem_schema' for more information).  A record
// definition provides an overloaded manipulator, 'appendField'.  Each
// appended field definition may be optionally-named and, independently, may
// also optionally have an associated 'int' id.  The field definitions can be
// accessed by index, by name, or by id.  Accessing a field definition via
// index is guaranteed to be an efficient, constant-time operation.  Note that
// both the names and ids of the field definitions are attributes of the record
// definition, and, as such, are accessed directly via the 'bdem_RecordDef'
// interface.
//
///Record Type
///-----------
// A record definition may be either a 'BDEM_SEQUENCE_RECORD' or a
// 'BDEM_CHOICE_RECORD'.   The type of record definition is specified at
// construction, and accessible via the 'recordType' method.  A
// 'BDEM_SEQUENCE_RECORD' record definition indicates that the described
// record is an (ordered) aggregation of the record's field types (similar
// to a C++ 'struct') -- this is the most common type of record.  A
// 'BDEM_CHOICE_RECORD' record definition indicates that the described record
// is a choice among the record's field types (similar to a C++ 'union').
//
///Usage
///-----
// The following usage examples demonstrate how to construct, manipulate, and
// access a record definition.  In general, clients should *not* create a
// 'bdem_RecordDef' directly, but instead obtain one from a schema (see
// 'bdem_schema').
//
///Schema Description
///- - - - - - - - - -
// In this section we configure several record definitions and an
// enumeration definition that collectively could be used to describe the
// value of a sale.  For the purposes of this example, a product can be paid
// for using either check, credit-card, or cash.  The set of definitions will
// have the following structure:
//..
//  +---------------------------------------------------------+
//  |{                                                        |
//  |    ENUMERATION "CREDIT_CARD_TYPE" {                     |
//  |        "MASTERCARD"                                     |
//  |        "VISA"                                           |
//  |        "AMEX"                                           |
//  |    }                                                    |
//  |                                                         |
//  |    RECORD "CREDIT_CARD_PAYMENT" {                       |
//  |        STRING<"CREDIT_CARD_TYPE">  "cardType";          |
//  |        STRING                      "name";              |
//  |        INT                         "cardNumber";        |
//  |    }                                                    |
//  |                                                         |
//  |    RECORD "CHECK_PAYMENT" {                             |
//  |        STRING  "bankName";                              |
//  |        INT     "routingNumber";                         |
//  |    }                                                    |
//  |                                                         |
//  |    CHOICE RECORD "PAYMENT_INFO" {                       |
//  |        LIST<"CREDIT_CARD_PAYMENT"> "creditCardPayment"; |
//  |        LIST<"CHECK_PAYMENT">       "checkPayment";      |
//  |        BOOL                        "cashPayment";       |
//  |    }                                                    |
//  |                                                         |
//  |    RECORD "SALE" {                                      |
//  |        INT                    "productId";              |
//  |        INT                    "quantity" [default: 1];  |
//  |        STRING                 "customerName";           |
//  |        DOUBLE                 "price"                   |
//  |        BDET_DATE              "date";                   |
//  |        CHOICE<"PAYMENT_INFO"> "paymentInfo";            |
//  |    }                                                    |
//  |}                                                        |
//  +---------------------------------------------------------+
//..
// The selection among 3 different payment options (credit-card, check, and
// cash) is described by the *choice* record "PAYMENT_INFO".  Choice record
// definitions (i.e., record definitions of type 'BDEM_CHOICE_RECORD')
// indicate a selection among its contained fields, and can be used to
// constrain fields of type 'CHOICE' and 'CHOICE_ARRAY'.  "PAYMENT_INFO" is
// used to constraint the choice field "paymentInfo" in the "SALE" record
// definition.
//
// The record definitions "SALE", "CREDIT_CARD_PAYMENT", and "CHECK_PAYMENT",
// are all *sequential* record definitions.  A sequence record definition
// (i.e., a record definition of type 'BDEM_SEQUENCE_RECORD') indicates that
// the record value is a collection of the indicated field
// values (rather than a choice between field values).  Sequence record
// definitions can be used to constraint fields definitions of type 'LIST' and
// 'TABLE'.  In this example, the sequence records definitions
// "CREDIT_CARD_PAYMENT" and "CHECK_PAYMENT" are used to respectively
// constrain the values in the lists "creditCardPayment" and "cashPayment" (in
// the "PAYMENT_INFO" choice record definition).  Finally, note that "SALE"
// has a "quantity" whose default value is 1.
//
// Here are a couple hypothetical record values that this schema might
// describe:
//..
//  {
//      productId:    386
//      quantity:     4
//      customerName: Jeff
//      price:        29.99
//      date:         April 22, 2010
//      paymentInfo:  {
//          cashPayment: { true }
//      }
//  }
//  {
//      productId:    486
//      customerName: Sally
//      price:        48.50
//      date:         May 1, 2010
//      paymentInfo:  {
//          creditCardPayment:  {
//              cardType:   "VISA"
//              name:       Sally Jenkins
//              cardNumber: 1111222223333
//          }
//      }
//  }
//..
// Note that the second record does not specify "quantity", thereby indicating
// the default value of 1.
//
///Directly Creating a 'bdem_RecordDef' (*Not Recommended*)
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This section demonstrates how to create a 'bdem_RecordRef' (and
// 'bdem_EnumerationDef') in isolation -- this is *not* the recommended usage
// of 'bdem_RecordDef' (or 'bdem_EnumerationDef'): obtain 'bdem_RecordDef'
// objects from a schema (see 'bdem_schema').  For the purpose of this usage
// example, we define a dummy "stand-in" type for 'bdem_Schema'.  This type is
// used *in* *name* *only* by 'bdem_RecordDef', so no particular definition is
// necessary:
//..
//  namespace BloombergLP {
//
//  class bdem_Schema {
//      // Dummy type.
//  };
//
//  }
//..
// Now we create record and enumeration definition objects matching the
// names in the example above.  We create one enumeration definition,
// "CREDIT_CARD_TYPE", one record definition, "PAYMENT_INFO", of type choice
// (i.e., 'BDEM_CHOICE_RECORD'), and three record definitions,
// "CREDIT_CARD_PAYMENT", "CHECK_PAYMENT", and "SALE", of type sequence
// (i.e., 'BDEM_SEQUENCE_RECORD'):
//..
//  bslma_Allocator           *allocator = bslma_Default::allocator();
//  bdema_SequentialAllocator  seqAllocator(allocator);
//  bdem_Schema                dummySchema;
//
//  bdem_EnumerationDef ccTypeEnumDef(&dummySchema,
//                                    0,
//                                    "CREDIT_CARD_TYPE",
//                                    &seqAllocator);
//  bdem_RecordDef ccPaymentRecDef(&dummySchema,
//                                 0,
//                                 "CREDIT_CARD_PAYMENT",
//                                 bdem_RecordDef::BDEM_SEQUENCE_RECORD,
//                                 &seqAllocator,
//                                 allocator);
//  bdem_RecordDef checkPaymentRecDef(&dummySchema,
//                                    1,
//                                    "CHECK_PAYMENT",
//                                    bdem_RecordDef::BDEM_SEQUENCE_RECORD,
//                                    &seqAllocator,
//                                    allocator);
//  bdem_RecordDef paymentInfoRecDef(&dummySchema,
//                                   2,
//                                   "PAYMENT_INFO",
//                                   bdem_RecordDef::BDEM_CHOICE_RECORD,
//                                   &seqAllocator,
//                                   allocator);
//  bdem_RecordDef saleRecDef(&dummySchema,
//                            3,
//                            "SALE",
//                            bdem_RecordDef::BDEM_SEQUENCE_RECORD,
//                            &seqAllocator,
//                            allocator);
//..
//
///Manipulating and Accessing a 'bdem_RecordDef'
///- - - - - - - - - - - - - - - - - - - - - - -
// In this section we demonstrate how to append fields to a record definition,
// and access its attributes.  For illustration purposes, we will use the
// record definitions (and enumeration definition) created in the preceding
// section.  Again note that, in practice, clients should obtain these
// definitions from a schema (see 'bdem_schema').
//
// We start by examining the attributes for the sequence record definition,
// 'ccPaymentRecDef'.  Note that the values returned for 'recordName',
// 'recordIndex', and 'recordType' are the values supplied at construction:
//..
//  assert(0 == ccPaymentRecDef.recordIndex());
//  assert(0 == bsl::strcmp(ccPaymentRecDef.recordName(),
//                          "CREDIT_CARD_PAYMENT"));
//  assert(bdem_RecordDef::BDEM_SEQUENCE_RECORD ==
//                                               ccPaymentRecDef.recordType());
//  assert(0 == ccPaymentRecDef.numFields());
//  assert(&dummySchema == &ccPaymentRecDef.schema());
//..
// We also examine the attributes for the choice record definition,
// 'paymentInfoRecDef':
//..
//  assert(2 == paymentInfoRecDef.recordIndex());
//  assert(0 == bsl::strcmp(paymentInfoRecDef.recordName(),
//                          "PAYMENT_INFO"));
//  assert(bdem_RecordDef::BDEM_CHOICE_RECORD ==
//                                             paymentInfoRecDef.recordType());
//  assert(0 == paymentInfoRecDef.numFields());
//  assert(&dummySchema == &paymentInfoRecDef.schema());
//..
// Next we add a set of values to 'ccTypeEnumDef' (see 'bdem_enumerationdef'
// for more information):
//..
//  int id1 = ccTypeEnumDef.addEnumerator("MASTERCARD");
//  int id2 = ccTypeEnumDef.addEnumerator("VISA");
//  int id3 = ccTypeEnumDef.addEnumerator("AMEX");
//  assert(0 == id1); assert(1 == id2); assert(2 == id3);
//..
// Next we append the field definitions to 'ccPaymentRecDef'.  When we add
// the field definition "cardType", we supply the enumeration definition
// 'ccTypeEnumDef' as an enumeration constraint for the values of that field:
//..
//  ccPaymentRecDef.appendField(bdem_ElemType::BDEM_STRING,
//                              &ccTypeEnumDef,
//                              "cardType");
//  ccPaymentRecDef.appendField(bdem_ElemType::BDEM_STRING,
//                              "name");
//  ccPaymentRecDef.appendField(bdem_ElemType::BDEM_INT,
//                              "cardNumber");
//..
// We will now verify the attributes of the first of the three fields we have
// just appended.  By default, a field will have a null field-id, it will not
// be nullable, it will not have a default value, and its formatting mode will
// be 'bdeat_FormattingMode::BDEAT_DEFAULT':
//..
//  assert(3 == ccPaymentRecDef.numFields());
//  const bdem_FieldDef& cardTypeFldDef = ccPaymentRecDef.field(0);
//
//  assert(0 == bsl::strcmp("cardType", cardTypeFldDef.fieldName()));
//  assert(bdem_RecordDef::BDEM_NULL_FIELD_ID == cardTypeFldDef.fieldId());
//  assert(bdem_ElemType::BDEM_STRING  == cardTypeFldDef.elemType());
//  assert(false                       == cardTypeFldDef.hasDefaultValue());
//  assert(false                       == cardTypeFldDef.isNullable());
//  assert(bdeat_FormattingMode::BDEAT_DEFAULT ==
//                                     cardTypeFldDef.formattingMode());
//  assert(&ccTypeEnumDef == cardTypeFldDef.enumerationConstraint());
//..
// Next append the two fields to 'checkPaymentRecDef':
//..
//  checkPaymentRecDef.appendField(bdem_ElemType::BDEM_STRING,
//                                 "bankName");
//  checkPaymentRecDef.appendField(bdem_ElemType::BDEM_INT,
//                                 "routingNumber");
//..
// Next we append the three fields to the choice record definition
// 'paymentInfoRecDef'.   Notice that when we add the field definition
// "creditCardPayment" we provide the (record) definition 'ccPaymentRecDef' as
// a record constraint for the values of that field.  Similarly notice that we
// provide 'checkPaymentRecDef' as a constraint for the "checkPayment" field:
//..
//  paymentInfoRecDef.appendField(bdem_ElemType::BDEM_LIST,
//                                &ccPaymentRecDef,
//                                "creditCardPayment");
//  paymentInfoRecDef.appendField(bdem_ElemType::BDEM_LIST,
//                                &checkPaymentRecDef,
//                                "checkPayment");
//  paymentInfoRecDef.appendField(bdem_ElemType::BDEM_BOOL,
//                                "cashPayment");
//..
// Finally we append three fields for the record definition 'saleRecDef'.  For
// the field "quantity" we create a 'bdem_FieldDefAttributes' object that we
// supply to 'appendField' so we can provide a default value of 1 for the
// field:
//..
//  saleRecDef.appendField(bdem_ElemType::BDEM_INT, "productId");
//
//  bdem_FieldDefAttributes quantityType(bdem_ElemType::BDEM_INT);
//  quantityType.defaultValue().theModifiableInt() = 1;
//  saleRecDef.appendField(quantityType, "quantity");
//
//  saleRecDef.appendField(bdem_ElemType::BDEM_INT,    "productId");
//  saleRecDef.appendField(bdem_ElemType::BDEM_STRING, "customerName");
//  saleRecDef.appendField(bdem_ElemType::BDEM_DOUBLE, "price");
//  saleRecDef.appendField(bdem_ElemType::BDEM_DATE,   "date");
//  saleRecDef.appendField(bdem_ElemType::BDEM_LIST,
//                         &paymentInfoRecDef,
//                         "paymentInfo");
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_FIELDDEF
#include <bdem_fielddef.h>
#endif

#ifndef INCLUDED_BDEM_FIELDSPEC
#include <bdem_fieldspec.h>
#endif

#ifndef INCLUDED_BDEAT_FORMATTINGMODE
#include <bdeat_formattingmode.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bdema_SequentialAllocator;
class bdem_Schema;

                    // ================================
                    // class bdem_RecordDef_NameCompare
                    // ================================

struct bdem_RecordDef_NameCompare
                     : bsl::binary_function<const char *, const char *, bool> {
    // Component-private struct: DO NOT USE OUTSIDE OF THIS COMPONENT.
    // This binary function object class provides a less-than operation that
    // compares 'const char' pointers as strings.  It is used for sorting
    // 'const char *' values.

    bool operator() (const char *lhs, const char *rhs) const
        // Return 'true' if the specified 'lhs' is lexicographically less
        // than the specified 'rhs', and 'false' otherwise.  The behavior is
        // undefined unless 'lhs' and 'rhs' are non-null.
    {
        return bsl::strcmp(lhs, rhs) < 0;
    }
};

                         // ====================
                         // class bdem_RecordDef
                         // ====================

class bdem_RecordDef {
    // This collaborative class represents a record definition, which is an
    // integral part a fully value-semantic schema object.  In general,
    // clients should *not* construct a 'bdem_RecordDef' directly, but instead
    // obtain a record definition from a schema (see 'bdem_schema' for more
    // information).  A record definition provides access to an indexed
    // collection of field definitions.  Each field definition may optionally
    // be named and, independently, may also optionally have an associated
    // 'int' id.  The field definitions can be accessed by index, by name, or
    // by id.  Accessing a field by index is guaranteed to be an efficient,
    // constant-time operation.  Note that both the names and ids of the fields
    // are attributes of the record definition and, as such, are accessed via
    // the record definition's interface.  Also note that the empty string ("")
    // is a valid field name, while the null string (0) indicates that the
    // field is unnamed (and need not be unique within the field definition
    // namespace).
    //
    // Finally note that *self-referential* record definitions are permitted.
    // That is, fields of aggregate type -- i.e., 'CHOICE', 'CHOICE_ARRAY',
    // 'LIST', or 'TABLE' -- in a record definition 'R' may be constrained by
    // 'R' itself, thus permitting schemas to describe recursive data
    // structures such as linked-lists and trees.  It is also possible to add
    // fields to a record that is not the most recently created record within
    // its containing schema, which allows for the creation of *mutually*
    // *recursive* record definitions suitable for describing a "GRAPH" record
    // that requires fields of a "NODE" record to be constrained by and "EDGE"
    // record and vice versa.

  public:
    // CONSTANTS
    static const int BDEM_NULL_FIELD_ID;  // This values is intended to be
                                          // distinct from that of any valid
                                          // field id.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    static const int NullFieldId;
        // The value of 'NullFieldId' is distinct from that of any valid field
        // id.  Note that the value of 'NullFieldId' is
        // 'bdetu_Unset<int>::unsetValue()'.
#endif

    // TYPES
    enum RecordType {
        // 'RecordType' enumerates the types of records that may be created
        // within a schema.

        BDEM_SEQUENCE_RECORD    = 0,
        BDEM_CHOICE_RECORD      = 1
    };

  private:
    // PRIVATE TYPES
    typedef bsl::map<const char *,
                     int,
                     bdem_RecordDef_NameCompare> CharptrIntMap;
        // Short name for string-to-integer map.

    // DATA
    const bdem_Schema            *d_schema_p;     // address of parent schema

    int                           d_recordIndex;  // index of '*this' in schema

    const char                   *d_recordName_p; // name of '*this', if any
                                                  // (*held, not owned*)

    RecordType                    d_recordType;   // type of this record
                                                  // (sequence or choice)

    bsl::vector<bdem_FieldDef *>  d_fieldDefs;    // vector of contained fields

    CharptrIntMap                 d_fieldNames;   // name-to-index map of
                                                  // fields that have names

    bsl::map<int, int>            d_fieldIds;     // id-to-index map of fields
                                                  // that have ids

  private:
    // PRIVATE MANIPULATORS
    template <typename CONSTRAINT_TYPE>
    bdem_FieldDef *append(const bdem_FieldDefAttributes&  attributes,
                          const CONSTRAINT_TYPE          *constraint,
                          const char                     *name,
                          int                             id);
        // Append, to the end of the sequence of field definitions managed by
        // this record definition, a field definition having the specified
        // 'attributes', 'constraint', 'name', and 'id'.  The templatized type
        // 'CONSTRAINT_TYPE' must be either 'bdem_EnumerationDef' or
        // 'bdem_RecordDef'.  Return the address of the newly-created,
        // non-modifiable field definition on success, and 0 if either (1)
        // 'name' or 'id' already exist in this record definition, (2)
        // 'constraint' is not a member of the schema containing this record
        // definition, or (3) 'constraint' is invalid for 'fielddefattributes'
        // -- i.e., 'constraint' is not 0 and
        // 'isValidConstraint(*constraint, attributes.elemType())' would return
        // 'false'.  If 'name' is 0, the field definition will be unnamed.
        // If 'id' is 'BDEM_NULL_FIELD_ID', the field definition will have no
        // associated explicit id.  If the field is a list recursively
        // constrained by this record, the 'isNullable' attribute will
        // be set to 'true' (irrespective of its value in 'attributes').

    // PRIVATE ACCESSORS
    bslma_Allocator *writeOnceAllocator() const;
        // Return the address of the modifiable write-once allocator
        // (established at construction).

  private:
    // NOT IMPLEMENTED
    bdem_RecordDef(const bdem_RecordDef&);
    bdem_RecordDef& operator=(const bdem_RecordDef&);

  public:
    // TYPES
    struct LowPrecedenceEnumPtr {
        // Use instead of a 'bdem_EnumerationDef *' in an overloaded
        // function signature to avoid overloading ambiguities between
        // 'bdem_EnumerationDef *' and 'bdem_RecordDef *'
        // when a literal 0 is passed as an argument.

        const bdem_EnumerationDef *d_ptr;
        LowPrecedenceEnumPtr(const bdem_EnumerationDef *p)
        : d_ptr(p)
        { }
    };

    // CLASS METHODS
    static bool isValidConstraint(const bdem_EnumerationDef& constraint,
                                  bdem_ElemType::Type        type);
        // Return 'true' if the specified 'type' is either 'INT', 'STRING',
        // 'INT_ARRAY', or 'STRING_ARRAY', and 'false' otherwise.

    static bool isValidConstraint(const bdem_RecordDef& constraint,
                                  bdem_ElemType::Type   type);
        // Return 'true' if (1) the specified 'type' is either 'CHOICE',
        // or 'CHOICE_ARRAY' and 'constraint.recordType()' is
        // 'BDEM_RECORD_CHOICE', or (2) the specified 'type' is either 'LIST'
        // or 'TABLE' and 'constraint.recordType()' is 'BDEM_RECORD_SEQUENCE';
        // otherwise return 'false'.

    // CREATORS
    bdem_RecordDef(const bdem_Schema         *schema,
                   int                        index,
                   const char                *name,
                   RecordType                 recordType,
                   bdema_SequentialAllocator *writeOnceAllocator,
                   bslma_Allocator           *basicAllocator);
        // Create a record definition having the specified 'schema', 'index',
        // 'name', and 'recordType', and using the specified
        // 'writeOnceAllocator' and 'basicAllocator' to supply memory.
        // If 'name' is 0, the record definition will be unnamed.
        // 'writeOnceAllocator' will be used to supply memory that will not be
        // released until this object is destroyed, while 'basicAllocator' will
        // be used to allocate memory that may be released prior to this
        // object's destruction.  The behavior is undefined unless
        // '0 <= index', both 'name' and 'schema' are non-null, and 'name'
        // remains valid and unmodified for the lifetime of this object.  Note
        // that this constructor does *not* modify 'schema' to refer to the
        // newly constructed object, and that clients of 'bdem' should
        // generally *not* attempt to construct a 'bdem_RecordDef' directly,
        // but instead should obtain one from a schema (see 'bdem_schema' for
        // more information).

    ~bdem_RecordDef();
        // Destroy this record definition.

    // MANIPULATORS
    const bdem_FieldDef *appendField(
                               bdem_ElemType::Type  type,
                               const char          *name = 0,
                               int                  id   = BDEM_NULL_FIELD_ID);
        // Append, to the end of this record definition, a field definition
        // having the specified 'type' and, optionally, the specified 'name',
        // or both 'name' and 'id'.  Return the address of the newly-created,
        // non-modifiable field definition on success, and 0 if 'name' or 'id'
        // already exist in this record definition.  If 'name' is 0 (or
        // unspecified), the field definition will be unnamed.  If 'id' is
        // 'BDEM_NULL_FIELD_ID' (or unspecified), the field definition will
        // have no associated explicit id.  Note that using this method to
        // create a field definition of type 'CHOICE', 'CHOICE_ARRAY', 'LIST'
        // or 'TABLE' will result in an unconstrained field.

    const bdem_FieldDef *appendField(
                              bdem_ElemType::Type   type,
                              const bdem_RecordDef *constraint,
                              const char           *name = 0,
                              int                   id   = BDEM_NULL_FIELD_ID);
    const bdem_FieldDef *appendField(
                              bdem_ElemType::Type   type,
                              LowPrecedenceEnumPtr  constraint,
                              const char           *name = 0,
                              int                   id   = BDEM_NULL_FIELD_ID);
        // Append, to the end of this record definition, a field definition
        // having the specified 'type' and 'constraint'.  Optionally specify
        // the 'name', or both 'name' and 'id' for the field.  If 'id' is
        // 'BDEM_NULL_FIELD_ID' (or unspecified), the field definition will
        // have no associated explicit id; if 'name' is 0 (or unspecified), the
        // field definition will be unnamed.   Return the address of the
        // newly-created, non-modifiable field definition on success, and 0 if
        // either (1) 'name' or 'id' already exist in this record definition,
        // (2) 'constraint' is not a member of the schema containing this
        // record definition, or (3) 'constraint' is invalid for 'type' --
        // i.e., if 'constraint' is not 0 and
        // 'isValidConstraint(*constraint, type)' would return 'false'.

    const bdem_FieldDef *appendField(
                    const bdem_FieldDefAttributes&  attributes,
                    const char                     *name = 0,
                    int                             id   = BDEM_NULL_FIELD_ID);
    const bdem_FieldDef *appendField(
                    const bdem_FieldDefAttributes&  attributes,
                    const bdem_RecordDef           *constraint,
                    const char                     *name = 0,
                    int                             id   = BDEM_NULL_FIELD_ID);
    const bdem_FieldDef *appendField(
                    const bdem_FieldDefAttributes&  attributes,
                    LowPrecedenceEnumPtr            constraint,
                    const char                     *name = 0,
                    int                             id   = BDEM_NULL_FIELD_ID);
        // Append, to the end of this record definition, a field definition
        // having the specified 'attributes'.  Optionally specify the
        // 'constraint' of the field.  If 'constraint' is 0 (or unspecified),
        // the field will be unconstrained.  Optionally specify the 'name, or
        // both 'name' and 'id' for this field.  If 'id' is
        // 'BDEM_NULL_FIELD_ID' (or unspecified), the field definition will
        // have no associated explicit id; if 'name' is 0 (or unspecified),
        // the field definition will be unnamed.  Return the address of the
        // newly-created, non-modifiable field definition on success, and 0 if
        // either (1) 'name' or 'id' already exist in this record definition,
        // (2) 'constraint' is not a member of the schema containing this
        // record definition, or (3) 'constraint' is invalid for the field type
        // (i.e., if 'constraint' is not 0 and
        // 'isValidConstraint(*constraint, attributes.elemType())' returns
        // 'false').  If the field is a list recursively constrained by the
        // record to which it is being appended, then the 'isNullable'
        // attribute is set to 'true' (irrespective of its value in
        // 'fieldSpec').

    const bdem_FieldDef *appendField(
                             const bdem_FieldSpec&  fieldSpec,
                             const char            *name = 0,
                             int                    id   = BDEM_NULL_FIELD_ID);
        // Append to the end of this record definition a field definition
        // having the specified 'fieldSpec'.  Optionally specify the 'name' or
        // both 'name' and 'id' for this field.  If 'Id' is
        // 'BDEM_NULL_FIELD_ID' (or unspecified), the field definition has no
        // associated explicitly id; if 'name' is 0 (or unspecified), the
        // field definition is unnamed.  Return the address of the
        // newly-created, non-modifiable field definition on success, and 0 if
        // either (1) 'name' or 'id' already exist in this record definition,
        // (2) 'fieldSpec' has a constraint that is not a member of the schema
        // containing this record definition, or (3) fieldSpec has a record
        // constraint that is invalid for the field type returns 'false').  If
        // the field is a list recursively constrained by the record to which
        // it is being appended, then the 'isNullable' attribute is set to
        // 'true' (irrespective of its value in 'fieldSpec').
        //
        // DEPRECATED: Use
        // 'appendField(const bdem_FieldDefAttributes&, const char *, int)'
        // instead.

    // ACCESSORS
    const bdem_FieldDef& field(int index) const;
        // Return a reference to the non-modifiable field definition at the
        // specified 'index' position in this record definition.  The behavior
        // is undefined unless '0 <= index < numFields()'.

    int fieldId(int index) const;
        // Return the id of the field definition at the specified 'index'
        // position in this record definition, and 'BDEM_NULL_FIELD_ID' if the
        // field definition at 'index' has no (explicit or implicit) id.  The
        // behavior is undefined unless '0 <= index < numFields()'.  Note that
        // if 'hasFieldIds' returns 'false' for this record definition, then
        // the (implicit) id of a field has the value of the field's index in
        // this record definition.

    int fieldIndex(const char *name) const;
        // Return the (non-negative) index of the field definition in this
        // record definition having the specified 'name', and a negative value
        // if 'name' does not correspond to any such field.  The behavior is
        // undefined unless 'name' is non-null.

    int fieldIndex(int id) const;
        // Return the (non-negative) index of the field definition in this
        // record definition having the specified 'id', and a negative value if
        // 'id' does not correspond to any such field.  Note that if
        // 'hasFieldIds' returns 'false', then the (implicit) id of a field
        // has the value of the field's index in this record definition.

    int fieldIndexExtended(const char *fieldName) const;
        // Return the (non-negative) index of a field definition within this
        // record definition that either (1) has the specified 'fieldName' or,
        // if no such field definition exists, (2) is unnamed and constrained
        // by a record definition such that 'fieldIndexExtended' applied
        // recursively to the constraining record-definition returns a
        // valid index; otherwise return a negative value.  If no field
        // definition in this record definition has the name 'fieldName', and
        // more than one unnamed field definition (recursively) matches this
        // criteria, it is unspecified which matching unnamed field
        // definition's index is returned.  The behavior is undefined if
        // 'fieldName' is 0, or if this record definition holds an unnamed
        // field definition whose constraining record definition refers back
        // to itself (either directly or indirectly).  Note that the index
        // returned is always for a field definition within this record
        // definition, and never a field definition within a "nested" record
        // definition.

    const char *fieldName(int index) const;
        // Return the name of the field definition at the specified 'index'
        // position in this record definition, and 0 if the field definition at
        // 'index' is unnamed.  The behavior is undefined unless
        // '0 <= index < numFields()'.

    bool hasFieldIds() const;
        // Return 'true' if this record definition contains one or more field
        // definitions having explicit ids, and 'false' otherwise.

    int length() const;
        // Return the number of field definitions in this record definition.
        //
        // DEPRECATED: Use 'numFields' instead.

    int numAnonymousFields() const;
        // Return the number of anonymous (unnamed) field definitions in this
        // record definition (an anonymous field definition has a 'fieldName'
        // of 0).  Note that a field definition having an empty string
        // ("") for a field name is not anonymous.

    int numFields() const;
        // Return the number of field definitions in this record definition.

    const bdem_FieldDef *lookupField(const char *name) const;
        // Return the address of the (non-modifiable) field definition having
        // the specified 'name' in this record definition, and 0 if no such
        // field definition exists.   The behavior is undefined unless 'name'
        // is non-null.

    const bdem_FieldDef *lookupField(int id) const;
        // Return the address of the (non-modifiable) field definition having
        // the specified 'id' in this record definition, and 0 if no such field
        // definition exists.  Note that if 'hasFieldIds' would return
        // 'false' for this record definition, then the (implicit) id of a
        // field has the value of the field's index in this record definition.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format the properties of this object to the specified output
        // 'stream' at the (absolute value of) the optionally specified
        // indentation 'level', and return a reference to 'stream'.  If 'level'
        // is specified, optionally specify 'spacesPerLevel', the number of
        // spaces per indentation level for this and all of its nested
        // objects.  If 'level' is negative, suppress indentation of the first
        // line.  If 'spacesPerLevel' is negative, format the entire output on
        // one line, suppressing all but the initial indentation (as governed
        // by the product of 'level' and (the absolute value of)
        // 'spacesPerLevel').  If 'stream' is not valid on entry, this
        // operation has no effect.

    int recordIndex() const;
        // Return the (non-negative) index for this record definition
        // (established at construction).  Note that a record index is
        // generally useful in identifying a record definition within a 'bdem'
        // schema (see 'bdem_schema').

    const char *recordName() const;
        // Return the name of this record definition (established at
        // construction), and 0 if this record definition is unnamed.  Note
        // that a record name is generally useful in identifying a record
        // definition within a 'bdem' schema (see 'bdem_schema').

    RecordType recordType() const;
        // Return the type of this record definition (established at
        // construction).

    const bdem_Schema& schema() const;
        // Return a reference to this record definition's non-modifiable schema
        // (established at construction).
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                           // --------------------
                           // class bdem_RecordDef
                           // --------------------

// PRIVATE ACCESSORS
inline
bslma_Allocator *bdem_RecordDef::writeOnceAllocator() const
{
    return d_fieldNames.get_allocator().mechanism();
}

// CLASS METHODS
inline
bool bdem_RecordDef::isValidConstraint(const bdem_EnumerationDef&,
                                       bdem_ElemType::Type        type)
{
    return type == bdem_ElemType::BDEM_INT
        || type == bdem_ElemType::BDEM_STRING
        || type == bdem_ElemType::BDEM_INT_ARRAY
        || type == bdem_ElemType::BDEM_STRING_ARRAY;
}

// MANIPULATORS
inline
const bdem_FieldDef *bdem_RecordDef::appendField(bdem_ElemType::Type  type,
                                                 const char          *name,
                                                 int                  id)
{
    bdem_FieldDefAttributes fielddefattributes(type);
    return appendField(fielddefattributes, 0, name, id);
}

inline
const bdem_FieldDef *bdem_RecordDef::appendField(
                                             bdem_ElemType::Type   type,
                                             const bdem_RecordDef *constraint,
                                             const char           *name,
                                             int                   id)
{
    bdem_FieldDefAttributes fielddefattributes(type);
    return appendField(fielddefattributes, constraint, name, id);
}

inline
const bdem_FieldDef *bdem_RecordDef::appendField(
                                              bdem_ElemType::Type   type,
                                              LowPrecedenceEnumPtr  constraint,
                                              const char           *name,
                                              int                   id)
{
    bdem_FieldDefAttributes fielddefattributes(type);
    return appendField(fielddefattributes, constraint.d_ptr, name, id);
}

inline
const bdem_FieldDef *bdem_RecordDef::appendField(
                                const bdem_FieldDefAttributes&  attributes,
                                const char                     *name,
                                int                             id)
{
    return appendField(attributes, 0, name, id);
}

// ACCESSORS
inline
const bdem_FieldDef& bdem_RecordDef::field(int index) const
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  numFields());

    return *d_fieldDefs[index];
}

inline
int bdem_RecordDef::fieldId(int index) const
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  numFields());

    return d_fieldIds.empty() ? index : d_fieldDefs[index]->fieldId();
}

inline
const bdem_FieldDef *bdem_RecordDef::lookupField(const char *name) const
{
    BSLS_ASSERT_SAFE(name);

    CharptrIntMap::const_iterator iter = d_fieldNames.find(name);
    return iter != d_fieldNames.end() ? d_fieldDefs[iter->second] : 0;
}

inline
const bdem_FieldDef *bdem_RecordDef::lookupField(int id) const
{
    if (d_fieldIds.empty()) {
        return static_cast<unsigned>(id) < d_fieldDefs.size() ? d_fieldDefs[id]
                                                              : 0;    // RETURN
    }

    bsl::map<int, int>::const_iterator iter = d_fieldIds.find(id);

    return iter != d_fieldIds.end() ? d_fieldDefs[iter->second] : 0;
}

inline
int bdem_RecordDef::fieldIndex(const char *name) const
{
    BSLS_ASSERT_SAFE(name);

    CharptrIntMap::const_iterator iter = d_fieldNames.find(name);
    return iter != d_fieldNames.end() ? iter->second : -1;
}

inline
int bdem_RecordDef::fieldIndex(int id) const
{
    if (d_fieldIds.empty()) {
        return static_cast<unsigned>(id) < d_fieldDefs.size() ? id
                                                              : -1;   // RETURN
    }

    bsl::map<int, int>::const_iterator iter = d_fieldIds.find(id);

    return iter != d_fieldIds.end() ? iter->second : -1;
}

inline
const char *bdem_RecordDef::fieldName(int index) const
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  numFields());

    return d_fieldDefs[index]->fieldName();
}

inline
bool bdem_RecordDef::hasFieldIds() const
{
    return !d_fieldIds.empty();
}

inline
int bdem_RecordDef::length() const
{
    return static_cast<int>(d_fieldDefs.size());
}

inline
int bdem_RecordDef::numAnonymousFields() const
{
    return static_cast<int>(d_fieldDefs.size() - d_fieldNames.size());
}

inline
int bdem_RecordDef::numFields() const
{
    return static_cast<int>(d_fieldDefs.size());
}

inline
int bdem_RecordDef::recordIndex() const
{
    return d_recordIndex;
}

inline
const char *bdem_RecordDef::recordName() const
{
    return d_recordName_p;
}

inline
bdem_RecordDef::RecordType bdem_RecordDef::recordType() const
{
    return d_recordType;
}

inline
const bdem_Schema& bdem_RecordDef::schema() const
{
    return *d_schema_p;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
