// bdem_schema.h                                                      -*-C++-*-
#ifndef INCLUDED_BDEM_SCHEMA
#define INCLUDED_BDEM_SCHEMA

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a collection of record definitions defining a data schema.
//
//@CLASSES:
//  bdem_Schema: collection of record definitions defining a schema for data
//
//@SEE_ALSO: bdem_aggregate, bdem_schemaaggregateutil
//
//@AUTHOR: Tom Marshall, Henry Verschell
//
//@DESCRIPTION: This component defines a fully *value-semantic* type,
// 'bdem_Schema', for representing the structure of hierarchal data values.
// Each 'bdem_Schema' object comprises an indexed sequence of enumeration
// definitions, and an indexed sequence of record definitions.  Each record
// definition, in turn, is defined by an indexed sequence of field definitions.
// Note that this component was designed to be compatible with aggregate types
// such as those defined in 'bdem_choice', 'bdem_choicearray', 'bdem_list',
// and 'bdem_table', but is otherwise logically as well as physically
// independent of those components.
//
///Schema Structure
///----------------
// Each 'bdem_Schema' object contains an indexed sequence of zero or more
// 'bdem_EnumerationDef' enumeration definitions, each comprising a set of
// enumerated (int, string) pair values.  Each 'bdem_Schema' object also
// contains an indexed sequence of zero or more 'bdem_RecordDef' objects
// (record definitions), each comprised of a sequence of zero or more
// 'bdem_FieldDef' objects (field definitions).  Each field definition
// indicates the data type, nullability, default value, and text formatting of
// a field in a corresponding record.  Both enumeration and record definitions
// within a schema may optionally be named and, thereby, can be accessed by
// either name or index.  Note that accessing an enumeration or record
// definition by index is guaranteed to be an efficient, constant-time
// operation.
//
///Field Constraints
///- - - - - - - - -
// Field definitions within a record definition may, optionally, be provided a
// constraint that indicates a constraint on the value of a corresponding
// field.  Field definitions without a constraint describe fields whose values
// are unconstrained.  The allowable constraints for different data types is
// outlined in the following table:
//..
//  .===========================================================.
//  | Data Types   | Constraint                                 |
//  |-----------------------------------------------------------|
//  | INT          | bdem_EnumerationDef                        |
//  | STRING       |                                            |
//  | INT_ARRAY    |                                            |
//  | STRING_ARRAY |                                            |
//  |-----------------------------------------------------------|
//  | LIST         | bdem_RecordDef (type BDEM_SEQUENCE_RECORD) |
//  | TABLE        |                                            |
//  |-----------------------------------------------------------|
//  | CHOICE       | bdem_RecordDef (type BDEM_CHOICE_RECORD)   |
//  | CHOICE_ARRAY |                                            |
//  `==========================================================='
//..
// If a field definition's type is one of 'INT', 'STRING', 'INT_ARRAY', or
// 'STRING_ARRAY', values of that field can be constrained by an "enumeration
// constraint".  The enumeration constraint is specified using a
// 'bdem_EnumerationDef' object defined within the *same* schema.  To satisfy
// the enumeration constraint, a field's value, if scalar, must either be null
// or have one of the 'int' or string values specified in the collection of
// 'int'-string pairs; if the field is an array, every element of the array
// must satisfy the constraint as a scalar.  No other types of fields may be
// constrained by an enumeration constraint.
//
// If a field definition's type is either 'LIST' or 'TABLE', values of that
// field can be constrainted by a "record constraint".  The record constraint
// is specified using a *sequence* 'bdem_RecordDef' object (i.e., a record
// definition of type 'BDEM_SEQUENCE_RECORD') defined within the *same*
// schema.   A (sequence) record constraint (if present) indicates that the
// field (of type 'LIST' or 'TABLE') must have the sequence of types specified
// by the constraining record definition.  Note that a constraint on a table
// uniformly constrains the types of elements in each row of that table.
//
// Finally if a field definition's type is either 'CHOICE' or 'CHOICE_ARRAY',
// values of that field can also be constrained by a "record constraint".
// However, in this case the record constraint is specified using a *choice*
// 'bdem_RecordDef' object (i.e., a record definition of type
// 'BDEM_CHOICE_RECORD') defined within the *same* schema.  A (choice) record
// constraint (if present) indicates that the field (of type 'CHOICE' or
// 'CHOICE_ARRAY') must provide the selection of types specified by the
// constraining record definition.  Note that a constraint on a choice array
// constrains the types of each choice in that array.
//
///Naming for Enumeration, Record, and Field Definitions
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Both record definitions and their field definitions may optionally be
// named, so long as each name is unique among peers in the containing object.
// That is, record definition names must be unique within a schema, and field
// definition names must be unique within a record definition.  In addition,
// field definitions may optionally have integral ids.  As with field names,
// each field id must be unique among peers in the containing record
// definition (except that more than one field in a record definition may have
// the null id).
//
// Enumeration definitions may also, optionally, be named.  Enumeration
// definitions may not share a name with an another enumeration definition, or
// another record definition within the same schema.
//
///Integral field Ids: Explicit, Implicit, and Null
///- - - - - - - - - - - - - - - - - - - - - - - -
// A field definition may have an *explicit* integral field id, an *implicit*
// (non-negative) field id, or the null id.  A field definition is assigned an
// *explicit* field id by specifying a non-null id when the field is created,
// using the appropriate overloaded 'appendField' manipulator of
// 'bdem_RecordDef'.  If one or more field definitions within a record
// definition are assigned explicit field ids, then the remaining field
// definitions (i.e., those not explicitly assigned non-null field ids) are
// considered to have *null* ids.  If _none_ of the field definitions within a
// record definition are assigned explicit field ids, then _all_ of the field
// definitions are considered to have *implicit* ids, in which case, the
// (implicit) field id of a given field definition is equivalent to its
// 0-based index position within its containing record definition.
//
// Note that the expected usage of field ids is such that either all field
// definitions within a record definition would have explicit (unique) ids, or
// all field definitions would have implicit (unique) ids.  However, it is
// certainly possible to assign explicit ids to a proper (non-empty) subset of
// fields in a record, while allowing the remaining fields to have (default)
// null ids.
//
///The *value* of a schema
///- - - - - - - - - - - -
// A 'bdem_Schema' object (like a graph having distinct node and edge objects)
// is composed of multiple, managed sub-objects that together comprise its
// value.  Just as it is not meaningful to compare corresponding nodes or
// edges from two distinct graphs for equality, so it is with respect to the
// field, record, and enumeration definitions of a schema.
//
// The *salient* *attributes* of a schema, i.e., those which comprise its
// value, are maintained by two top-level indexed sequences: (1) *enumeration*
// *definitions*, and (2) *record* *definitions*.  In order for two schemas to
// have the same value, the number of enumeration definitions must be the
// same, and at each enumeration-definition-index position, corresponding
// enumeration definitions must be equivalent (see
// 'bdem_EnumerationDef::areEquivalent'), meaning they (a) have the same name
// (or both be unnamed), and (b) have the same number of enumerators, where
// for each enumerator (consisting of an int, string pair) in one, there is a
// corresponding enumerator pair in the other having the same value (i.e.,
// corresponding 'int' and string values respectively have the same value).
//
// In addition, the number of record definitions must also be the same, and at
// each record-definition-index position, the record definition must (a) have
// the same name (or both must be unnamed), and (b) must have the same number
// of field-definitions, where, at each field-definition-index position,
// corresponding field-definitions have the same (1) data type,
// (2) formatting mode, (3) nullability value, (4) in the case of 'bdem'
// scalar and array types, have the same default value (or both have no
// default value), and (5) are both constrained by corresponding record or
// enumeration definitions within their respective schemas (or else are both
// unconstrained).
//
///Recursive Schemas
///- - - - - - - - -
// A 'bdem_RecordDef' may indicate, either directly or indirectly, a
// recursive schema -- e.g., a record definition, "A", that contains a field
// definition for an aggregate field (i.e., a field definition having a data
// type of 'LIST', 'TABLE', CHOICE', or 'CHOICE_ARRAY') whose type is
// constrained, recursively, by record "A".  A recursive schema is considered
// "well-formed" *unless* it goes through a field of type 'LIST' for which
// 'isNullable' returns 'false': if 'isNullable' returns 'false' for a
// recursively defined list field, then the schema describes a non-terminating
// recursive structure (having infinite size), and is therefore invalid.
//
///Usage
///-----
// The following examples illustrate how to create and use a
// 'bdem_Schema'.  Note that using "ALL_CAPITALS' for record names, and
// "mixedCase" for field names, is a convention, and not required by
// 'bdem_schema'.
//
///Example 1: Creating a Trivial Schema
///- - - - - - - - - - - - - - - - - -
// In this example we create a simple schema having a single record
// definition that describes a record representing a sale.  A sale is a record
// containing an integer product id, a customer name, a floating point price,
// and a date.
//..
//  .---------------------------------------.
//  |{                                      |
//  |    RECORD "SALE" {                    |
//  |        INT           "productId";     |
//  |        STRING        "customerName";  |
//  |        DOUBLE        "price";         |
//  |        DATE          "date";          |
//  |    }                                  |
//  |}                                      |
//  `---------------------------------------'
//..
// We start by creating an empty schema:
//..
//  bslma_Allocator *allocator = bslma_Default::allocator();
//  bdem_Schema schema(allocator);
//..
// We create a new record definition "SALE" of type 'BDEM_SEQUENCE_RECORD',
// which is the default:
//..
//  bdem_RecordDef *saleRecDef = schema.createRecord("SALE");
//..
// A record definition of type 'BDEM_SEQUENCE_RECORD' means that records
// created with that definition will be a *sequence* of all the defined
// fields (similar to a C++ 'struct'), rather than *choice* between them
// (similar to a C++ 'union').   Creating a *choice* record definition
// requires an explicit 'bdem_RecordDef::BDEM_CHOICE_RECORD' argument to the
// 'createRecord' method (see example 2).
//
// Next we append the fields to this record:
//..
//  const bdem_FieldDef *productIdFldDef =
//         saleRecDef->appendField(bdem_ElemType::BDEM_INT,    "productId");
//  const bdem_FieldDef *customerNameFldDef =
//         saleRecDef->appendField(bdem_ElemType::BDEM_STRING, "customerName");
//  const bdem_FieldDef *priceFldDef =
//         saleRecDef->appendField(bdem_ElemType::BDEM_DOUBLE, "price");
//  const bdem_FieldDef *dateFldDef =
//         saleRecDef->appendField(bdem_ElemType::BDEM_DATE,   "date");
//..
// We verify the properties of a single field.  By default, a field will not
// be nullable, it will not have a default value, and its formatting
// mode will be 'bdeat_FormattingMode::BDEAT_DEFAULT':
//..
//  assert(0 == bsl::strcmp("date", dateFldDef->fieldName()));
//  assert(bdem_ElemType::BDEM_DATE == dateFldDef->elemType());
//  assert(false                    == dateFldDef->hasDefaultValue());
//  assert(false                    == dateFldDef->isNullable());
//  assert(bdeat_FormattingMode::BDEAT_DEFAULT ==
//                                     dateFldDef->formattingMode());
//..
// We can write this schema to the console:
//..
//  schema.print(bsl::cout, 1, 3);
//..
// The resulting output will be:
//..
//  {
//     SEQUENCE RECORD "SALE" {
//        INT "productId" { !nullable 0x0 }
//        STRING "customerName" { !nullable 0x0 }
//        DOUBLE "price" { !nullable 0x0 }
//        DATE "date" { !nullable 0x0 }
//     }
//  }
//..
//
///Example 2: Using Choices, Constraints, and Default Values
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we create a more complex schema for describing a sale.  The
// schema, illustrated below, supports sales performed using either a check, a
// credit card, or cash:
//..
//  .---------------------------------------------------------.
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
//  `---------------------------------------------------------'
//..
// The selection among three different payment options (check, credit-card,
// and cash) is described by the choice record "PAYMENT_INFO".  Choice record
// definitions (i.e., record definitions of type 'BDEM_CHOICE') indicate a
// selection from among its contained fields.  A choice record definition can
// be used to constrain fields of type 'CHOICE' and 'CHOICE_ARRAY'.  The choice
// record definition "PAYMENT_INFO" is used to constraint the choice field,
// "paymentInfo", in the "SALE" record definition.
//
// By default, record definitions are of type 'BDEM_SEQUENCE_RECORD'.
// A *sequence* record-definition indicates that the record value is a
// *sequence* of the indicated field values (rather than a *choice* among
// field values).  Sequence record-definitions can be used to constraint fields
// definitions of type 'LIST' and 'TABLE'.  In this example, the *sequence*
// record-definitions "CREDIT_CARD_PAYMENT" and "CHECK_PAYMENT" are used to
// constraint the values in the lists "creditCardPayment" and "cashPayment"
// in the "PAYMENT_INFO' *choice* record-definition.
//
// Finally, note that "SALE" has a "quantity" whose default value is 1.
//
// Here are a couple hypothetical record values that this schema might
// describe:
//..
//  .---------------------------------------.
//  | {                                     |
//  |     productId:    386                 |
//  |     quantity:     4                   |
//  |     customerName: Jeff                |
//  |     price:        29.99               |
//  |     date:         April 22, 2010      |
//  |     paymentInfo:  {                   |
//  |         cashPayment: { true }         |
//  |     }                                 |
//  | }                                     |
//  | {                                     |
//  |     productId:    486                 |
//  |     customerName: Sally               |
//  |     price:        48.50               |
//  |     date:         May 1, 2010         |
//  |     paymentInfo:  {                   |
//  |         creditCardPayment:  {         |
//  |             cardType:   "VISA"        |
//  |             name:       Sally Jenkins |
//  |             cardNumber: 1111222223333 |
//  |         }                             |
//  |     }                                 |
//  | }                                     |
//  `---------------------------------------'
//..
// Notice that the second record does not specify 'quantity', so it is
// interpreted as having the schema's default 'quantity' value of 1.
//
//Creating a Schema:
//- - - - - - - - -
// We now examine how to create a 'bdem_Schema' for the schema described in
// the previous section.  We start by creating a 'bdem_Schema' object, and
// adding an enumeration named "CREDIT_CARD_TYPE":
//..
//  bslma_Allocator *allocator = bslma_Default::allocator();
//  bdem_Schema schema(allocator);
//
//  bdem_EnumerationDef *ccTypeEnumDef =
//                               schema.createEnumeration("CREDIT_CARD_TYPE");
//..
// Next we add the individual of enumerator values, which by default take on
// sequential integer values starting with 0:
//..
//  int id1 = ccTypeEnumDef->addEnumerator("MASTERCARD");
//  int id2 = ccTypeEnumDef->addEnumerator("VISA");
//  int id3 = ccTypeEnumDef->addEnumerator("AMEX");
//  assert(0 == id1);  assert(1 == id2);  assert(2 == id3);
//..
// Then we create the *sequential* record definition "CREDIT_CARD_PAYMENT",
// and append its fields.  When we add the field definition "cardType", we
// supply the enumeration definition 'ccTypeEnumDef' as an enumeration
// constraint for the values of that field:
//..
//  bdem_RecordDef *ccPaymentRecDef = schema.createRecord(
//                                                      "CREDIT_CARD_PAYMENT");
//  ccPaymentRecDef->appendField(bdem_ElemType::BDEM_STRING,
//                               ccTypeEnumDef,
//                               "cardType");
//  ccPaymentRecDef->appendField(bdem_ElemType::BDEM_STRING,
//                               "name");
//  ccPaymentRecDef->appendField(bdem_ElemType::BDEM_INT,
//                               "cardNumber");
//..
// Next we create the *sequential* record definition for "CHECK_PAYMENT", and
// append its fields:
//..
//  bdem_RecordDef *checkPaymentRecDef = schema.createRecord("CHECK_PAYMENT");
//  checkPaymentRecDef->appendField(bdem_ElemType::BDEM_STRING,
//                                  "bankName");
//  checkPaymentRecDef->appendField(bdem_ElemType::BDEM_INT,
//                                  "routingNumber");
//..
// Then we create the *choice* record definition for "PAYMENT_INFO".  We
// indicate that the new record definition should be a choice by supplying
// 'bdem_RecordDef::BDEM_CHOICE_RECORD' to 'createRecord':
//..
//  bdem_RecordDef *paymentInfoRecDef = schema.createRecord(
//                                         "PAYMENT_INFO",
//                                         bdem_RecordDef::BDEM_CHOICE_RECORD);
//..
// Next we append the fields to the "PAYMENT_INFO" record.  When we add the
// field definition "creditCardPayment" we provide the record definition
// 'ccPaymentRecDef', as a record constraint for the values of that field.
// Similarly, we provide the record definition 'checkPaymentRecDef' as a
// constraint for the values of the "checkPayment" field:
//..
//  paymentInfoRecDef->appendField(bdem_ElemType::BDEM_LIST,
//                                 ccPaymentRecDef,
//                                 "creditCardPayment");
//  paymentInfoRecDef->appendField(bdem_ElemType::BDEM_LIST,
//                                 checkPaymentRecDef,
//                                 "checkPayment");
//  paymentInfoRecDef->appendField(bdem_ElemType::BDEM_BOOL,
//                                 "cashPayment");
//..
// Then we create a record for "SALE", and append its first field "productId":
//..
//  bdem_RecordDef *saleRecDef = schema.createRecord("SALE");
//  saleRecDef->appendField(bdem_ElemType::BDEM_INT, "productId");
//..
// For the "quantity" field, we create a 'bdem_FieldDefAttributes' so we can
// provide it a default value of 1.  We then append the "quantity" field, and
// the remaining fields to the "SALE" record definition:
//..
//  bdem_FieldDefAttributes quantityType(bdem_ElemType::BDEM_INT);
//  quantityType.defaultValue().theModifiableInt() = 1;
//
//  saleRecDef->appendField(quantityType,               "quantity");
//  saleRecDef->appendField(bdem_ElemType::BDEM_INT,    "productId");
//  saleRecDef->appendField(bdem_ElemType::BDEM_STRING, "customerName");
//  saleRecDef->appendField(bdem_ElemType::BDEM_DOUBLE, "price");
//  saleRecDef->appendField(bdem_ElemType::BDEM_DATE,   "date");
//  saleRecDef->appendField(bdem_ElemType::BDEM_LIST,
//                          paymentInfoRecDef,
//                          "paymentInfo");
//..
// Finally, we can write a description of the created schema to the console:
//..
//  schema.print(bsl::cout, 1, 3);
//..
// The resulting console output looks like:
//..
//  {
//     SEQUENCE RECORD "CREDIT_CARD_PAYMENT" {
//        STRING ENUM<"CREDIT_CARD_TYPE"> "cardType" { !nullable 0x0 }
//        STRING "name" { !nullable 0x0 }
//        INT "cardNumber" { !nullable 0x0 }
//     }
//     SEQUENCE RECORD "CHECK_PAYMENT" {
//        STRING "bankName" { !nullable 0x0 }
//        INT "routingNumber" { !nullable 0x0 }
//     }
//     CHOICE RECORD "PAYMENT_INFO" {
//        LIST<"CREDIT_CARD_PAYMENT"> "creditCardPayment" { !nullable 0x0 }
//        LIST<"CHECK_PAYMENT"> "checkPayment" { !nullable 0x0 }
//        BOOL "cashPayment" { !nullable 0x0 }
//     }
//     SEQUENCE RECORD "SALE" {
//        INT "productId" { !nullable 0x0 }
//        INT "quantity" { !nullable 0x0 1 }
//        STRING "customerName" { !nullable 0x0 }
//        DOUBLE "price" { !nullable 0x0 }
//        DATE "date" { !nullable 0x0 }
//     }
//     ENUMERATION "CREDIT_CARD_TYPE" {
//        "MASTERCARD" = 0
//        "VISA" = 1
//        "AMEX" = 2
//     }
//  }
//..
//
///Example 3: Creating a Recursive Schema
/// - - - - - - - - - - - - - - - - - - -
// Fields of type 'LIST', 'TABLE', 'CHOICE', and 'CHOICE_ARRAY' are known as
// aggregate fields because they contain (or aggregate) a sequence of
// sub-fields (see 'bdem_ElemType::isAggregateType').  Field definitions of
// aggregate field types may be constrained by another record definition,
// indicating a constraint on the values of the sub-fields of the aggregate
// field.  It is possible for such a constraint to be recursive, i.e., a
// record definition, "A", contains an field definition for an aggregate field
// that is constrained by record "A".  Such a recursive schema allows the
// construction of recursive data structures such as lists and trees.
//
// In this example we illustrates how to construct a schema for a binary tree.
// The finished schema will have the following textual representation of
// its structure.
//..
//  .------------------------------.
//  |{                             |
//  |    RECORD "TREE" {           |
//  |        STRING       "key";   |
//  |        LIST<"TREE"> "left";  |
//  |        LIST<"TREE"> "right"; |
//  |    }                         |
//  |}                             |
//  `------------------------------'
//..
// First we create an empty 'bdem_Schema' object, 'schema':
//..
//  bdem_Schema schema;
//..
// Next we append a record definition named "TREE" to 'schema':
//..
//  bdem_RecordDef *treeRecDef = schema.createRecord("TREE");
//..
// Then we append to "TREE" a field definition of type 'STRING' named "key",
// followed by two field definitions, each of type 'LIST', respectively named
// "left" and "right", and each constrained by the record definition "TREE":
//..
//  treeRecDef->appendField(bdem_ElemType::BDEM_STRING, "key");
//  treeRecDef->appendField(bdem_ElemType::BDEM_LIST, treeRecDef, "left");
//  treeRecDef->appendField(bdem_ElemType::BDEM_LIST, treeRecDef, "right");
//..
// It's also possible to create records with mutually recursive constraints --
// e.g., a schema describing a graph, consisting of mutually referential nodes
// and edges.  Note that both self-referential and mutually self-referential
// constraints are viable only if underlying data structures support null
// fields.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_ENUMERATIONDEF
#include <bdem_enumerationdef.h>
#endif

#ifndef INCLUDED_BDEM_FIELDDEF
#include <bdem_fielddef.h>
#endif

#ifndef INCLUDED_BDEM_FIELDDEFATTRIBUTES
#include <bdem_fielddefattributes.h>
#endif

#ifndef INCLUDED_BDEM_RECORDDEF
#include <bdem_recorddef.h>
#endif

#ifndef INCLUDED_BDEM_ELEMREF
#include <bdem_elemref.h>
#endif

#ifndef INCLUDED_BDEM_ELEMATTRLOOKUP
#include <bdem_elemattrlookup.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BDEM_DESCRIPTOR
#include <bdem_descriptor.h>
#endif

#ifndef INCLUDED_BDEAT_FORMATTINGMODE
#include <bdeat_formattingmode.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEMA_SEQUENTIALALLOCATOR
#include <bdema_sequentialallocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>          // for 'bsl::strlen'
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

                        // =============================
                        // class bdem_Schema_NameCompare
                        // =============================

struct bdem_Schema_NameCompare
                     : bsl::binary_function<const char *, const char *, bool> {
    // Component-private struct: DO NOT USE OUTSIDE OF THIS COMPONENT.
    // This binary function object class provides a less-than operation that
    // compares 'const char' pointers as strings.  It is used for sorting
    // 'const char *' values.

    bool operator() (const char *lhs, const char *rhs) const
        // Return 'true' if the specified 'lhs' is lexicographically less
        // than the specified 'rhs', and return 'false' otherwise.  The
        // behavior is undefined unless 'lhs' and 'rhs' are non-zero.
    {
        return bsl::strcmp(lhs, rhs) < 0;
    }
};

                               // =================
                               // class bdem_Schema
                               // =================

class bdem_Schema {
    // This class implements a fully value-semantic, recursive data schema,
    // appropriate for data types that properly map onto the enumerators
    // defined in 'bdem_elemtype'.  Each object of this class comprises two
    // indexed sequences of optionally-named definitions: (1) enumeration
    // definitions, and (2) record definitions (which in turn comprise an
    // indexed sequence of field definitions).  Both enumeration and records
    // definitions can be used to constrain fields defined by the record
    // definitions, and both kinds of definitions be accessed either via index
    // or, if defined, by name.  Accessing either an enumeration or record
    // definition via index is guaranteed to be an efficient, constant-time
    // operation.  Enumeration and record definition names are attributes of
    // the containing schema and, as such, are accessed directly via the schema
    // interface.  Both enumeration and record definition names share the same
    // namespace, and all such names within a schema object must be unique.
    // Note that, with respect to naming, the empty string ("") is considered
    // a valid name, whereas null string (0) is used to imply that the
    // enumeration or record definition is unnamed (and therefore need not
    // be unique within the shared namespace).
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including default and copy construction, copy assignment,
    // equality comparison, 'ostream' printing, and 'bdex' serialization.  (A
    // precise operational definition of when two objects have the same value
    // can be found in the description of 'operator==' for the class.)  This
    // class is *exception* *neutral* with no guarantee of rollback: If an
    // exception is thrown during the invocation of a method on a pre-existing
    // object, the object is left in a valid state, but (unless otherwise
    // stated) its value is unspecified.  In no event is memory leaked.
    // Finally, *aliasing* (e.g., using all or part of an object as both
    // source and destination) is supported in all cases.

  private:
    // PRIVATE TYPES
    typedef bsl::map<const char *, int, bdem_Schema_NameCompare> CharptrIntMap;
        // Short name for charptr-to-int map

    // PRIVATE CONSTANTS
    enum { BDEX_MINOR_VERSION_MASK = 1 << 30 };
        // The 'BDEX_MINOR_VERSION_MASK' enumerator is used to encode 'bdex'
        // streaming minor version numbers that are non-zero.  Note that the
        // 'bdex' 'putLength' method uses the high-order bit to encode the
        // value streamed.

    // DATA
    bdema_SequentialAllocator          d_writeOnceAlloc;  // arena for
                                                          // allocating
                                                          // sub-parts

    bsl::vector<bdem_RecordDef *>      d_recordDefs;      // sequence of record
                                                          // defs

    CharptrIntMap                      d_recordNames;     // name-to-index map
                                                          // of records that
                                                          // have names

    bsl::vector<bdem_EnumerationDef *> d_enumDefs;        // sequence of
                                                          // enumeration defs

    CharptrIntMap                      d_enumNames;       // name-to-index map
                                                          // of enums that have
                                                          // names
  private:
    // PRIVATE MANIPULATORS
    template <class STREAM>
    STREAM& bdexStreamInEnumerationDef(STREAM& stream, int detailedVersion);
        // Append to this schema an enumeration definition read from the
        // specified 'stream' using the specified 'version' format, and return
        // a reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, this object is valid, but its value is
        // undefined.  If 'version' is not supported, 'stream' is marked
        // invalid and this object is unaltered.

    template <class STREAM>
    STREAM& bdexStreamInRecordDef(
                                 bsl::vector<int>           *constraintIndices,
                                 bdem_RecordDef::RecordType  recordType,
                                 STREAM&                     stream,
                                 int                         detailedVersion);
        // Append to this schema a record definition read from the
        // specified 'stream' using the specified 'version' format, and load
        // into 'constraintIndices' a constraint index for each field in the
        // new record definition, where an index value of -1 indicates the
        // corresponding field definition has no constraint.  Return a
        // reference to the modifiable 'stream'.  On success
        // 'constraintIndices->size()' will equal the number of field
        // definitions in the new record definition, and each index is the
        // (potentially invalid) index into this schema's records (or -1) that
        // was read from 'stream'.  This method does *not* initialize the
        // constraints of the fields in the new record definition -- that can
        // be done only after all the record and enumeration definitions have
        // been created (see 'setRecordConstraints').  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, this object is valid, but
        // its value is undefined.  If 'version' is not supported, 'stream' is
        // marked invalid and this object is unaltered.  Note that this method
        // is intended to be used in conjunction with 'setRecordConstraints'.

    int setRecordConstraints(bdem_RecordDef    *recordDef,
                             bsl::vector<int>&  constraintIndices);
        // Set the constraint for each field definition in 'recordDef' to the
        // record or enumeration definition indicated by the
        // corresponding element of 'constraintIndices'.  Return 0 on success,
        // and a non-zero value if a value in 'constraintIndices' is not either
        // -1 or the index for a valid constraint for the corresponding field.
        // The behavior is undefined unless
        // 'recordDef->length() == constraintIndices.size()'.  Note that this
        // method is used in conjunction with 'bdexStreamInRecordDef' by
        // 'bdexStreamIn'; this method must be called, but only after all the
        // record and enumeration definitions have been created so that the
        // indexes indicated by 'constraintIndices' refer to valid objects.

    // PRIVATE ACCESSORS
    int bdexMinorVersion() const;
        // Return the smallest 'bdex' streaming format minor version number
        // that is capable of representing the data in this schema.  For
        // compatibility with programs that were compiled with older versions
        // of bdem, 'maxSupportedBdexVersion' always returns 1.  However, some
        // features of 'bdem_Schema' are cannot be represented in the original
        // (version 1) streaming format.  To accommodate these features, new
        // formats are created with the same major version number (1) but with
        // different minor version numbers.  When a new features is not used,
        // the original, version 1.0 streaming format is used for
        // compatibility with older programs.  A schema can thus be streamed
        // to an older program provided the features being used are compatible
        // with that program.
        //
        // This function determines the minor version number needed to stream
        // this schema.  Specifically, if this schema contains any enumeration
        // definitions, return minor version 2 (stream format 1.2);
        // Otherwise, if this schema contains any choice records, uses any
        // element type larger than 'TABLE', uses nullable fields, or uses
        // default values, return minor version 1; oOtherwise, return minor
        // version 0.

    template <class STREAM>
    STREAM& bdexStreamOutEnumerationDef(
                             STREAM&                    stream,
                             const bdem_EnumerationDef& enumerationDef,
                             int                        detailedVersion) const;
        // Write the specified 'enumerationDef' value to the specified output
        // 'stream' using the specified 'detailedVersion' (i.e.,
        // '1000 * majorVersion + minorVersion') format, and return a reference
        // to the modifiable 'stream'.  If 'version' is not supported, 'stream'
        // is unmodified.  Note that 'detailedVersion' is not written to
        // 'stream'.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    template <class STREAM>
    STREAM& bdexStreamOutRecordDef(
                                  STREAM&               stream,
                                  const bdem_RecordDef& recordDef,
                                  int                   detailedVersion) const;
        // Write the specified 'recordDef' value to the specified output
        // 'stream' using the specified 'detailedVersion' (i.e.,
        // '1000 * majorVersion + minorVersion' format, and return a reference
        // to the modifiable 'stream'.  If 'version' is not supported, 'stream'
        // is unmodified.  Note that 'detailedVersion' is not written to
        // 'stream'.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdem_Schema,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    bdem_Schema(bslma_Allocator *basicAllocator = 0);
        // Create an empty schema object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    bdem_Schema(const bdem_Schema&  original,
                bslma_Allocator    *basicAllocator = 0);
        // Create a schema having the value of the specified 'original' schema.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~bdem_Schema();
        // Destroy this schema object.

    // MANIPULATORS
    bdem_Schema& operator=(const bdem_Schema& rhs);
        // Assign to this object the value of the specified 'rhs' schema.

    bdem_RecordDef *createRecord(bdem_RecordDef::RecordType  type);
    bdem_RecordDef *createRecord(const char                 *name = 0,
                                 bdem_RecordDef::RecordType  type =
                                         bdem_RecordDef::BDEM_SEQUENCE_RECORD);
        // Create an empty 'bdem' record definition, and append it to the
        // sequence of records managed by this schema.  Optionally specify
        // the 'type' of the new record definition.  If 'type' is not
        // provided, the new record definition will be a
        // 'BDEM_SEQUENCE_RECORD'.  Optionally specify the 'name' of the
        // record definition.  If 'name' is 0 (or unspecified), the record
        // definition is unnamed.   Return the address of the newly-created
        // modifiable record definition, or 0 if a non-zero 'name' is
        // provided, and 'name' is the same as that of an existing record
        // or enumeration definition managed by this schema.  Note that an
        // address is returned so that the caller may conveniently append
        // field definitions, thus completing the new record definition.

    bdem_EnumerationDef *createEnumeration(const char *name = 0);
        // Create an empty 'bdem' enumeration definition, and append it to the
        // sequence of enumeration definitions managed by this schema.
        // Optionally specify the 'name' of the enumeration definition.  If
        // 'name' is 0 (or unspecified), the enumeration definition is
        // unnamed.  Return the address of the newly-created modifiable
        // enumeration definition, or 0 if a non-zero 'name' is provided, and
        // 'name' is the same as that of an existing record or enumeration
        // definition managed by this schema.  Note that the address of the new
        // enumeration definition is returned so that the caller may
        // conveniently add enumerators and thus complete the enumeration
        // definition.

    void removeAll();
        // Remove all enumeration and record definitions from this schema
        // object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, this object is valid, but its value is
        // undefined.  If 'version' is not supported, 'stream' is marked
        // invalid and this object is unaltered.  Note that no version is read
        // from 'stream'.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    // ACCESSORS
    bslma_Allocator *allocator() const;
        // Return the address of the modifiable allocator supplied at
        // construction or, if no allocator was explicitly supplied, the
        // currently installed default allocator at the time the time of
        // construction.

    bool isEmpty() const;
        // Return 'true' if this schema contains no enumeration or record
        // definitions, and 'false' otherwise'.  Note that the value returned
        // by calling this method is logically equivalent to the expression
        // '0 == numEnumerations() + numRecords()'.

    int length() const;
        // Return the number of record definitions contained in this schema.
        //
        // DEPRECATED: Use 'numRecords' instead.

    int numEnumerations() const;
        // Return the number of enumeration definitions in this schema.

    int numRecords() const;
        // Return the number of record definitions in this schema.

    const bdem_RecordDef *lookupRecord(const char *name) const;
        // Return the address of the non-modifiable record definition having
        // the specified 'name' in this schema, or 0 if no such record
        // definition exists.  The behavior is undefined if 'name' is null.

    const bdem_RecordDef& record(int index) const;
        // Return a reference to the non-modifiable record definition at the
        // specified (record definition) 'index' position in this schema.  The
        // behavior is undefined unless '0 <= index < numRecords()'.

    int recordIndex(const char *name) const;
        // Return the (non-negative) index of the record definition having the
        // specified 'name' in this schema, or a negative value if no such
        // record exists.  The behavior is undefined if 'name' is null.

    const char *recordName(int index) const;
        // Return the name of the record definition at the specified (record
        // definition) 'index' position in this schema, or 0 if the record
        // definition at the specified 'index' is unnamed.  The behavior is
        // undefined unless '0 <= index < numRecords()'.

    const bdem_EnumerationDef *lookupEnumeration(const char *name) const;
        // Return the address of the non-modifiable enumeration definition
        // having the specified 'name' in this schema, or 0 if no such
        // enumeration definition exists.  The behavior is undefined if 'name'
        // is null.

    const bdem_EnumerationDef& enumeration(int index) const;
        // Return a reference to the non-modifiable enumeration definition at
        // the specified (enumeration definition) 'index' position in this
        // schema.  The behavior is undefined unless
        // '0 <= index < numEnumerations()'.

    int enumerationIndex(const char *name) const;
        // Return the (non-negative) index of the enumeration definition
        // having the specified 'name' in this schema, or a negative integer
        // if no such enumeration definition exists.  The behavior is
        // undefined if 'name' is null.

    const char *enumerationName(int index) const;
        // Return the non-modifiable name of the enumeration definition at the
        // specified (enumeration definition) 'index' position in this schema,
        // or 0 if the enumeration definition at the specified 'index' is
        // unnamed.  The behavior is undefined unless
        // '0 <= index < numEnumerations()'.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format the value of this object to the specified output 'stream'
        // at the (absolute value of) the optionally specified indentation
        // 'level', and return a reference to 'stream'.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', the number of
        // spaces per indentation level for this and all of its nested
        // objects.  If 'level' is negative, suppress indentation of the first
        // line.  If 'spacesPerLevel' is negative, format the entire output on
        // one line, suppressing all but the initial indentation (as governed
        // by 'level').  If 'stream' is not valid on entry, this operation has
        // no effect.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is invalidated.
        // Note that 'version' is not written to 'stream'.  See the 'bdex'
        // package-level documentation for more information on 'bdex' streaming
        // of value-semantic types and containers.
};

// FREE OPERATORS
bool operator==(const bdem_Schema& lhs, const bdem_Schema& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' schemas have the same
    // value, and 'false' otherwise.  Two schemas have the same value if
    // (1) the number of enumeration definitions in each schema is the same,
    // and at each enumeration-definition-index position, corresponding
    // enumeration definitions are equivalent (see
    // 'bdem_EnumerationDef::areEquivalent'), meaning they (a) have the same
    // name (or both be unnamed), and (b) have the same number of enumerators
    // (each consisting of an int-string pair) where, for each enumerator in
    // one, there is a corresponding enumerator in the other having the same
    // value, and (2) the number of record definitions in each schema is also
    // the same, and at each record-definition-index position, the
    // corresponding record definitions (a) have the same name (or both are
    // unnamed), and (b) have the same number of field-definitions, where, at
    // each field-definition-index position, corresponding field-definitions
    // (i) have the same data type, formatting mode, and nullability value,
    // (ii) in the case of 'bdem' scalar and array types, have the same
    // default value (or both have no default value), and (iii) are both
    // constrained by corresponding record or enumeration definitions within
    // their respective schemas (or else are both unconstrained).

bool operator!=(const bdem_Schema& lhs, const bdem_Schema& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' schemas do not have the
    // same value, and 'false' otherwise.  Two schemas do not have the same
    // value if (1) the number of enumeration definitions in each schema is
    // not the same, or if any two enumeration definitions at the same index
    // position, are not equivalent (see 'bdem_EnumerationDef::areEquivalent'),
    // meaning they (a) do not have the same name or (b) do not have the same
    // number of enumerators or (c) there is an enumerator (consisting of an
    // int-string pair) in one enumeration, with no corresponding enumerator
    // having the same value in the other, or (2) the number of record
    // definitions in each schema is not the same, or if any two record
    // definitions at the same index position (a) do not have the same name, or
    // (b) do not have the same number of field-definitions, or (c) if any two
    // field definitions at the same index position do not (i) have the same
    // data type, formatting mode, nullability value, (ii) in the case of
    // 'bdem' scalar and array types, do not have the same default value, or
    // (iii) are not constrained by corresponding record or enumeration
    // definitions within their respective schemas.

bsl::ostream& operator<<(bsl::ostream& stream, const bdem_Schema& schema);
    // Write the value of the specified 'schema' to the specified output
    // 'stream', and return a reference to the modifiable 'stream'.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -----------------
                        // class bdem_Schema
                        // -----------------

// PRIVATE MANIPULATORS
template <class STREAM>
STREAM& bdem_Schema::bdexStreamInEnumerationDef(STREAM& stream,
                                                int     detailedVersion)
{
    if (!stream || detailedVersion < 1002) {
        // Only streaming-format version 1.2 or greater supports enumeration.
        stream.invalidate();
        return stream;                                                // RETURN
    }

    // Use sequential allocator for temporary strings.  The sequential
    // allocator provides exception safety because the strings are
    // automatically deallocated.

    bdema_SequentialAllocator tempAllocator;
    tempAllocator.reserveCapacity(200);

    int nameLen = -1;  // length of enumeration def name; 0 -> unset (not "")
    stream.getLength(nameLen);
    if (!stream) {
        return stream;                                                // RETURN
    }

    char *enumName = 0;
    if (nameLen) {
        // Length read in includes null terminator, but contents of string
        // does not.  This allows a distinction between an empty name and
        // a non-existent name.

        enumName = (char *) tempAllocator.allocate(nameLen);
        stream.getArrayInt8(enumName, nameLen - 1);
        if (!stream) {
            return stream;                                            // RETURN
        }

        enumName[nameLen - 1] = '\0';
    }

    // Note that memory for this enumeration definition has already been
    // reserved in 'bdexStreamIn'.

    bdem_EnumerationDef *enumerationDef = createEnumeration(enumName);
    if (0 == enumerationDef) {
        stream.invalidate();
        return stream;                                                // RETURN
    }

    int numEnumerators = 0;
    stream.getLength(numEnumerators);
    for (int i = 0; i < numEnumerators; ++i) {
        stream.getLength(nameLen);
        if (!stream || nameLen <= 0) {
            stream.invalidate();
            return stream;                                            // RETURN
        }

        char *enumeratorName = (char *) tempAllocator.allocate(nameLen);
        stream.getArrayInt8(enumeratorName, nameLen - 1);
        enumeratorName[nameLen - 1] = '\0';

        int enumeratorId;
        stream.getInt32(enumeratorId);
        if (!stream) {
            return stream;                                            // RETURN
        }

        if (enumerationDef->addEnumerator(enumeratorName, enumeratorId) ==
                                              bdetu_Unset<int>::unsetValue()) {
            stream.invalidate();
            return stream;                                            // RETURN
        }
    }

    return stream;
}

template <class STREAM>
STREAM& bdem_Schema::bdexStreamInRecordDef(
                                 bsl::vector<int>           *constraintIndices,
                                 bdem_RecordDef::RecordType  recordType,
                                 STREAM&                     stream,
                                 int                         detailedVersion)
{
    BSLS_ASSERT(constraintIndices);

    // Use sequential allocator for temporary strings.  This provides
    // exception safety because the strings are automatically deallocated.
    bdema_SequentialAllocator tempAllocator;
    tempAllocator.reserveCapacity(200);

    int version = detailedVersion / 1000;
    switch (version) {  // Switch on the schema version (starting with 1).
      case 1: {
        int nameLen = -1;  // length of record name; 0 -> unset (not "")
        stream.getLength(nameLen);
        if (!stream) {
            return stream;                                            // RETURN
        }

        char *recordName = 0;
        if (nameLen) {
            // Length read in includes null terminator, but contents of string
            // does not.  This allows a distinction between an empty name and
            // a non-existent name.

            recordName = (char *) tempAllocator.allocate(nameLen);
            stream.getArrayInt8(recordName, nameLen - 1);
            if (!stream) {
                return stream;                                        // RETURN
            }

            recordName[nameLen - 1] = '\0';
        }

        // Note that memory for this record definition has already been
        // reserved in 'bdexStreamIn'.

        bdem_RecordDef *recordDef = createRecord(recordName, recordType);
        if (0 == recordDef) {
            stream.invalidate();
            return stream;                                            // RETURN
        }

        int numFields = 0;        // number of element fields in the record
        stream.getLength(numFields);

        if (!stream) {
            return stream;                                            // RETURN
        }

        constraintIndices->reserve(numFields);
        for (int i = 0; i < numFields; ++i) {
            int nameLen = -1;  // length of ith field name
            stream.getLength(nameLen);
            if (!stream) {
                return stream;                                        // RETURN
            }

            char *fieldName = 0;
            if (nameLen) {
                fieldName = (char *) tempAllocator.allocate(nameLen);
                stream.getArrayInt8(fieldName, nameLen - 1);
                if (!stream) {
                    return stream;                                    // RETURN
                }

                fieldName[nameLen - 1] = '\0';
            }

            int fieldId = bdem_RecordDef::BDEM_NULL_FIELD_ID;
            if (detailedVersion >= 1001) {
                stream.getInt32(fieldId);
                if (!stream) {
                    return stream;                                    // RETURN
                }
            }

            bdem_ElemType::Type elemType = bdem_ElemType::BDEM_VOID;
            bdem_ElemType::bdexStreamIn(stream, elemType, 1);
                                                      // version deliberately 1
            if (!stream) {
                return stream;                                        // RETURN
            }

            int constraintIndex = 0;
            stream.getInt32(constraintIndex);
            if (!stream) {
                return stream;                                        // RETURN
            }
            constraintIndices->push_back(constraintIndex);

            // Set default formatting mode and nullability.  Note that a list
            // is not nullable by default, whereas all other field types are.
            // The exception to that rule (where a list is nullable by default)
            // is when the record definition for a list also serves as a
            // constraint on the list (i.e., a recursively defined record
            // definition).

            int  formattingMode = bdeat_FormattingMode::BDEAT_DEFAULT;
            bool isNullable     = bdem_ElemType::BDEM_LIST != elemType ||
                                  recordDef->recordIndex() == constraintIndex;
            if (detailedVersion >= 1001) {
                // Read formatting mode and nullability.

                stream.getInt32(formattingMode);
                if (!stream) {
                    return stream;                                    // RETURN
                }

                char temp;
                stream.getInt8(temp);
                if (!stream) {
                    return stream;                                    // RETURN
                }
                isNullable = static_cast<bool>(temp);
            }

            bdem_FieldDefAttributes attributes(elemType,
                                               isNullable,
                                               formattingMode,
                                               &tempAllocator);

            if (detailedVersion >= 1001
             && !bdem_ElemType::isAggregateType(elemType)) {
                // Read the default value for the field.
                // Default value may have a different type than the field.
                // Specifically, if field is an array, then its default value
                // will have the corresponding scalar type.

                const bdem_ElemRef& defaultValue = attributes.defaultValue();
                bdem_ElemType::Type defaultValueType = defaultValue.type();

                // Clear the default value, which may be set later (below).
                // Note that 'defaultValue' already has the "unset"
                // value.

                attributes.defaultValue().makeNull();

                char hasDefault = 0;  // 'bool', really
                if (detailedVersion >= 1003) {
                    stream.getInt8(hasDefault);
                }

                if (detailedVersion < 1003 || hasDefault) {
                    const bdem_DescriptorStreamIn<STREAM> *inLookup =
                            bdem_ElemStreamInAttrLookup<STREAM>::lookupTable();
                    inLookup[(int) defaultValueType].streamIn(
                                           defaultValue.data(),
                                           stream,
                                           version,
                                           inLookup,
                                           bdem_ElemAttrLookup::lookupTable());
                }

                if (!hasDefault) {
                    if (defaultValue.descriptor()->isUnset(
                                                        defaultValue.data())) {
                        // Set the 'DEFAULT_VALUE_FLAG' bit.

                        defaultValue.makeNull();
                    }
                }

                if (!stream) {
                    return stream;                                    // RETURN
                }
            }

            // Note that the constraint for this field will be set later
            // (after all the enumerations have been added) by calling
            // 'setRecordConstraint' on 'constraintIndices'.

            const bdem_FieldDef *newField = recordDef->appendField(attributes,
                                                                   0,
                                                                   fieldName,
                                                                   fieldId);
            // We have all the pieces needed to add the field.
            if (0 == newField) {
                stream.invalidate();
                return stream;                                        // RETURN
            }
        }
      } break;
      default: {
        stream.invalidate();
      } break;
    }

    return stream;
}

// PRIVATE ACCESSORS
template <class STREAM>
STREAM& bdem_Schema::bdexStreamOutEnumerationDef(
                      STREAM&                          stream,
                      const bdem_EnumerationDef& enumerationDef,
                      int                              detailedVersion) const
{
    if (!stream || detailedVersion < 1002) {
        // Only streaming-format version 1.2 or greater supports enumeration.
        stream.invalidate();
        return stream;                                                // RETURN
    }

    if (enumerationDef.enumerationName()) {
        // 'len' includes the null terminator, but the character array does
        // not.  This allows a length of 1 (empty string), to be distinguished
        // from a length of 0 (null string).

        const int len = static_cast<int>(
                            bsl::strlen(enumerationDef.enumerationName())) + 1;
        stream.putLength(len);
        stream.putArrayInt8(enumerationDef.enumerationName(), len - 1);
    }
    else {
        stream.putLength(0);
    }

    const int enumeratorCount = enumerationDef.numEnumerators();
    stream.putLength(enumeratorCount);

    for (bdem_EnumerationDefIterator i = enumerationDef.begin();
                                           i != enumerationDef.end();
         ++i) {
        const int nameLen = static_cast<int>(bsl::strlen(i.name())) + 1;
        stream.putLength(nameLen);
        stream.putArrayInt8(i.name(), nameLen - 1);
        stream.putInt32(i.id());
    }

    return stream;
}

template <class STREAM>
STREAM& bdem_Schema::bdexStreamOutRecordDef(
                                   STREAM&               stream,
                                   const bdem_RecordDef& recordDef,
                                   int                   detailedVersion) const
{
    // The logic for streaming record and field names is:
    //..
    //  if (name) {
    //      output(length, name);  // 'length' includes the null terminator.
    //  }
    //  else {
    //      output(0);             // '0' denotes the null string.
    //  }
    //..
    // Note that "" is valid as both a record name and a field name.

    int version = detailedVersion / 1000;
    switch (version) {  // Switch on the schema version (starting with 1).
      case 1: {
        if (recordDef.recordName()) {
            // 'len' includes the null terminator, but the character array does
            // not.   This allows a length of 1 (empty string), to be
            // distinguished from a length of 0 (null string).

            const int len = static_cast<int>(
                                      bsl::strlen(recordDef.recordName())) + 1;
            stream.putLength(len);
            stream.putArrayInt8(recordDef.recordName(), len - 1);
        }
        else {
            stream.putLength(0);
        }

        int numFields = recordDef.numFields();
        stream.putLength(numFields);

        for (int i = 0; i < numFields; ++i) {
            // ith field in record definition

            const bdem_FieldDef& fieldDef = recordDef.field(i);

            const char *name = fieldDef.fieldName();
            if (name) {
                const int len = static_cast<int>(bsl::strlen(name)) + 1;
                stream.putLength(len);
                stream.putArrayInt8(name, len - 1);
            }
            else {
                stream.putLength(0);
            }

            if (detailedVersion >= 1001) {
                stream.putInt32(recordDef.field(i).fieldId());
            }

            const bdem_ElemType::Type fieldType = fieldDef.elemType();
            bdem_ElemType::bdexStreamOut(stream, fieldType, 1);
                                                     // version deliberately 1

            // Stream out the constraint index, or -1 if field is
            // unconstrained.

            int constraintIndex = -1;

            if (bdem_EnumerationDef::canHaveEnumConstraint(fieldType)) {

                // Enumeration constraint

                const bdem_EnumerationDef *constraint =
                                            fieldDef.enumerationConstraint();
                BSLS_ASSERT(detailedVersion >= 1002 || 0 == constraint);

                constraintIndex = constraint ? constraint->enumerationIndex()
                                             : -1;
            }
            else if (bdem_ElemType::isAggregateType(fieldType)) {

                // Record constraint

                const bdem_RecordDef *constraint = fieldDef.recordConstraint();
                constraintIndex = constraint ? constraint->recordIndex() : -1;
            }
            stream.putInt32(constraintIndex);

            if (detailedVersion >= 1001) {
                stream.putInt32(fieldDef.formattingMode());
                stream.putInt8(fieldDef.isNullable());

                if (!bdem_ElemType::isAggregateType(fieldDef.elemType())) {

                    const bool hasDefault = fieldDef.hasDefaultValue();
                    if (detailedVersion >= 1003) {
                        stream.putInt8(hasDefault);
                    }

                    // Default value may have a different type than the field.
                    // Specifically, if field is an array, then its default
                    // value will have the corresponding scalar type.

                    if (detailedVersion < 1003 || hasDefault) {
                        const bdem_ConstElemRef& defaultValue =
                                                       fieldDef.defaultValue();
                        bdem_ElemType::Type defaultValueType =
                                                      defaultValue.type();

                        const bdem_DescriptorStreamOut<STREAM> *outLookup =
                           bdem_ElemStreamOutAttrLookup<STREAM>::lookupTable();
                        outLookup[(int) defaultValueType].streamOut(
                                                           defaultValue.data(),
                                                           stream,
                                                           version,
                                                           outLookup);
                    }
                }
            }
        }
      } break;
    }

    return stream;
}

// CLASS METHODS
inline
int bdem_Schema::maxSupportedBdexVersion()
{
    return 1;  // Required by BDE policy; version starts at 1.
}

// MANIPULATORS
inline
bdem_RecordDef *bdem_Schema::createRecord(bdem_RecordDef::RecordType type)
{
    return createRecord(0, type);
}

template <class STREAM>
STREAM& bdem_Schema::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        removeAll();  // If stream is initially valid, remove the old contents.
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            int numRecords = -1;
            int numEnums = 0;
            stream.getLength(numRecords);
            if (!stream) {
                return stream;                                        // RETURN
            }

            // If 'version > 1.0', then 'numRecords' will actually hold the
            // minor version rather than the number of records.

            const int minorVersion = numRecords & BDEX_MINOR_VERSION_MASK
                                   ? numRecords ^ BDEX_MINOR_VERSION_MASK
                                   : 0;

            switch (minorVersion) {
              case 0:
                break;
              case 1:
                // Read the *real* number of records.

                stream.getLength(numRecords);
                break;
              case 2:                                           // FALL THROUGH
              case 3:
                // Read the *real* number of records and the number of enums.

                stream.getLength(numRecords);
                stream.getLength(numEnums);
                break;
              default:
                stream.invalidate();
                return stream;
            }

            if (!stream) {
                return stream;
            }

            const int detailedVersion = 1000 * version + minorVersion;

            // Create 'numRecords' record type values, reading reach record's
            // type (SEQUENCE or CHOICE) from the stream.  For format version
            // 1.0, all records are of type SEQUENCE and the stream is not
            // read in this step.

            bsl::vector<bdem_RecordDef::RecordType> recordTypes;
            recordTypes.reserve(numRecords);
            for (int r = 0; r < numRecords; ++r) {
                // In 'bdex' version 1.0, all records are of type
                // 'BDEM_SEQUENCE_RECORD'.
                bdem_RecordDef::RecordType recordType =
                                         bdem_RecordDef::BDEM_SEQUENCE_RECORD;
                if (detailedVersion >= 1001) {
                    // In 'bdex' version 1.1 and above, records may be of type
                    // 'BDEM_SEQUENCE_RECORD' or 'BDEM_CHOICE_RECORD'.

                    unsigned char readValue;
                    stream.getUint8(readValue);
                    if (!stream) {
                        return stream;                                // RETURN
                    }
                    if (readValue > bdem_RecordDef::BDEM_CHOICE_RECORD) {
                        stream.invalidate();
                        return stream;                                // RETURN
                    }
                    recordType =
                            static_cast<bdem_RecordDef::RecordType>(readValue);
                }
                recordTypes.push_back(recordType);
            }

            // Reserve space for the records and enumerations
            d_recordDefs.reserve(numRecords);
            d_enumDefs.reserve(numEnums);

            // Streaming in record definitions is a two step process:
            // 1) Create the record definitions and enumeration definitions
            // described by 'stream', and load into 'constrainIndices' the
            // constraint indices in 'stream'
            // 2) Verify and set field constraints.
            //
            // Note that the constraints can only be set once all the record
            // definitions and enumeration definitions that they reference
            // have been created.

            bsl::vector<bsl::vector<int> > constraintIndices;
            constraintIndices.resize(numRecords);

            // Read the record definitions
            for (int r = 0; r < numRecords && stream; ++r) {
                bdexStreamInRecordDef(&constraintIndices[r],
                                      recordTypes[r],
                                      stream,
                                      detailedVersion);
            }

            // Read the enumeration definitions.

            for (int e = 0; e < numEnums && stream; ++e) {
                bdexStreamInEnumerationDef(stream, detailedVersion);
            }

            // Verify and set the constrains of the record definitions.

            for (int r = 0; r < numRecords && stream; ++r) {
                if (0 != setRecordConstraints(d_recordDefs[r],
                                              constraintIndices[r])) {
                    stream.invalidate();
                    return stream;                                    // RETURN
                }
            }
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          } break;
        }
    }
    return stream;
}

// ACCESSORS
inline
bslma_Allocator *bdem_Schema::allocator() const
{
    // Get the allocator from the 'd_recordDefs' vector.
    return d_recordDefs.get_allocator().mechanism();
}

inline
bool bdem_Schema::isEmpty() const
{
    return numRecords() + numEnumerations() == 0;
}

inline
int bdem_Schema::length() const
{
    return numRecords();
}

inline
int bdem_Schema::numRecords() const
{
    return static_cast<int>(d_recordDefs.size());
}

inline
int bdem_Schema::numEnumerations() const
{
    return static_cast<int>(d_enumDefs.size());
}

inline
const bdem_RecordDef *bdem_Schema::lookupRecord(const char *name) const
{
    CharptrIntMap::const_iterator iter = d_recordNames.find(name);
    return iter != d_recordNames.end() ? d_recordDefs[iter->second] : 0;
}

inline
const bdem_RecordDef& bdem_Schema::record(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < numRecords());

    return *d_recordDefs[index];
}

inline
int bdem_Schema::recordIndex(const char *name) const
{
    CharptrIntMap::const_iterator iter = d_recordNames.find(name);
    return iter != d_recordNames.end() ? iter->second : -1;
}

inline
const char *bdem_Schema::recordName(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < numRecords());

    return d_recordDefs[index]->recordName();
}

inline
const bdem_EnumerationDef *
bdem_Schema::lookupEnumeration(const char *name) const
{
    CharptrIntMap::const_iterator iter = d_enumNames.find(name);
    return iter != d_enumNames.end() ? d_enumDefs[iter->second] : 0;
}

inline
const bdem_EnumerationDef& bdem_Schema::enumeration(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < numEnumerations());

    return *d_enumDefs[index];
}

inline
int bdem_Schema::enumerationIndex(const char *name) const
{
    CharptrIntMap::const_iterator iter = d_enumNames.find(name);
    return iter != d_enumNames.end() ? iter->second : -1;
}

inline
const char *bdem_Schema::enumerationName(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < numEnumerations());

    return d_enumDefs[index]->enumerationName();
}

template <class STREAM>
STREAM& bdem_Schema::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {  // Switch on the schema version (starting with 1).
      case 1: {
        const int minorVersion = bdexMinorVersion();
        const int detailedVersion = 1000 * version + minorVersion;
        if (0 != minorVersion) {
            stream.putLength(minorVersion | BDEX_MINOR_VERSION_MASK);
        }

        int numRecs = numRecords();
        stream.putLength(numRecs);

        int numEnums = numEnumerations();
        if (detailedVersion >= 1002) {
            stream.putLength(numEnums);
        }

        if (detailedVersion >= 1001) {
            for (int r = 0; r < numRecs; ++r) {
                stream.putUint8(record(r).recordType());
            }
        }

        for (int r = 0; r < numRecs; ++r) {
            bdexStreamOutRecordDef(stream, record(r), detailedVersion);
        }

        for (int e = 0; e < numEnums; ++e) {
            bdexStreamOutEnumerationDef(stream,
                                        enumeration(e),
                                        detailedVersion);
        }
      } break;
      default: {
        stream.invalidate();  // unrecognized version number
      } break;
    }

    return stream;
}

// FREE OPERATORS
inline
bool operator!=(const bdem_Schema& lhs, const bdem_Schema& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdem_Schema& schema)
{
    return schema.print(stream, 0, -1);
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
