// bdem_schemautil.h                                                  -*-C++-*-
#ifndef INCLUDED_BDEM_SCHEMAUTIL
#define INCLUDED_BDEM_SCHEMAUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide comparison operations for schemas and record definitions.
//
//@CLASSES:
//  bdem_SchemaUtil: non-primitive operations on schemas and record definitions
//
//@SEE_ALSO: bdem_schema, bdem_schemaaggregateutil, bdem_schemaenumerationutil
//
//@AUTHOR: John Lakos, Tom Marshall
//
//@DESCRIPTION: This component provides a utility, 'bdem_SchemaUtil', that
// serves as a namespace for a suite of pure procedures that operate on
// objects of types that comprise a data schema: 'bdem_EnumerationDef',
// 'bdem_RecordDef', and 'bdem_Schema'.  In particular, 'bdem_SchemaUtil'
// defines functions for comparing pairs of 'bdem_EnumerationDef' objects
// ("enumeration definitions"), pairs of 'bdem_RecordDef' objects ("record
// definitions"), and pairs of 'bdem_Schema' objects ("schemas").
//
// A data schema is used to facilitate manipulation of various aggregate data
// types.  The aggregate data types are unspecified at the schema level, but
// they could represent such things as sequences and choices, where a sequence
// contains an ordered list of objects, and a choice contains a single object,
// where the type of object contained by a choice can vary.
//
// Objects contained in sequences or choices can be of a variety of types.
// Aggregate types, which are either sequences or choices, can be constrained
// by record definitions.  The types are stored in a record definition as a
// "record type".  In the case of a sequence, the record definition specifies
// the sequence of types of objects contained in the sequence.  In the case of
// a choice, the record definition specifies the types of objects the choice
// can contain.  Enumeration definitions do not constrain aggregate types;
// instead, they constrain only 'BDEM_STRING' and 'BDEM_INT' types, along
// with their corresponding array types, 'BDEM_STRING_ARRAY and
// 'BDEM_INT_ARRAY'.
//
// Note that the specific components within the 'bdem' package describing
// different kinds of aggregate types can be found within the 'bdem'
// package-level documentation (also see the 'bdem_aggregate' component).
//
// In what follows, we start with a mechanical discussion of the classification
// of the comparison methods, and slowly add details and background information
// to provide guidance on the usage of specific methods implemented within this
// component.  More broadly, this documentation lays the foundation for
// effective use of schemas within the 'bdem' messaging usage paradigm.  Also
// see the 'bdem' package-level documentation for a yet broader discussion of
// schemas and effective schema negotiation within 'bdem'.
//
///Classification of Comparison Methods
///------------------------------------
// The comparison methods in this component can be classified according to
// three main aspects: the general *Relationship* between the two objects, the
// specific comparison *Criterion*, and the method *Signature* (i.e., the types
// of the objects being compared).  These aspects are interrelated, and not all
// possible combinations of aspects necessarily produce useful functions.  Each
// aspect is introduced (briefly) in the subsections below.  Subsequent
// sections document in more detail the motivation for specific definitions and
// the anticipated usage in conjunction with aggregate data types.
//
///Comparison Relationships
/// - - - - - - - - - - - -
// This component supports two logical Relationships: *areEquivalent* and
// *isSuperset*.  Although the specific Criteria will influence the subtle
// meanings of these Relationships, they are broadly analogous to the
// fundamental operators '==' and '>', respectively.
//
///Comparison Signatures
///- - - - - - - - - - -
// The comparison Signature specifies the types of the two objects being
// compared.  This component supports four (homogeneous) comparison signatures:
//..
//  (const bdem_Schema&,         const bdem_Schema&)
//  (const bdem_EnumerationDef&, const bdem_EnumerationDef&)
//  (const bdem_RecordDef&,      const bdem_RecordDef&)
//  (const bdem_RecordDef&,      const bdem_RecordDef&, int attributeMask)
//..
// That is, this component supports comparison of two schemas, two enumeration
// definitions, and two record definitions.  Note that enumeration definitions
// are non-recursive.  An optional 'attributeMask' may be supplied to some of
// the methods that compare record definitions to affect recursion-related
// details of the comparisons that are performed.  More information on
// attribute masks is provided below.
//
// Note that record/enumeration-definition-based comparisons are, in general,
// far more useful than schema-based ones because the notion of "conformance".
// Conformance of a sequence or choice is with respect to a specific record
// definition (and its constraints) within a schema, but not with the schema as
// a whole.  A schema is an indexed collection of record definitions that need
// not have any relationship to one another.
//
///Comparison Criteria
///- - - - - - - - - -
// This component provides three separate comparison Criteria: *exact*,
// *structural*, and *symbolic*.
//
// The *exact* Criterion, when applied to a pair of value-semantic objects
// (e.g., schemas), considers all aspects that are part of each object's value.
// In particular, the *exact* version of the *areEquivalent* Relationship
// between two schemas would be functionally identical to 'operator==' as
// defined in the 'bdem_schema' component (and for that reason alone is not
// implemented here).  The definition of *exact* is somewhat less obvious when
// applied to record/enumeration definitions, since 'bdem_RecordDef' and
// 'bdem_EnumerationDef' are *not* value-semantic types.  In particular, the
// record definition name could be considered relevant (as it is when comparing
// schemas), but, on purely mechanical grounds, the record definition names are
// managed by the schema (neither a record nor enumeration definition directly
// knows its own name).  Yet more important reasons for not considering record
// definition names for comparison purposes are discussed below.
//
// The *structural* Criterion is concerned with comparison by index position
// only.  Neither record nor enumeration definition names, nor field names (or
// ids) are considered for this comparison: Only the relative order (i.e.,
// index position) of the entries are significant.  The *structural* Criterion
// is therefore useful for validating by-index access of data elements (see
// below).
//
// The *symbolic* Criterion is concerned with comparison by field name and id
// only.  Neither record/enumeration definition names nor fields that are
// unnamed (and have no id), nor field indexes are considered for this
// comparison.  The *symbolic* Criterion is therefore useful for validating
// by-name and by-id access of data elements (see below).
//
// Note that besides their treatment of record definitions and field names (and
// field ids), the methods that support the *structural* and *symbolic*
// Criteria differ in another important respect from the methods that support
// the *exact* one: The *exact* methods compare the formatting mode, default
// value attributes and the enumeration definitions of corresponding fields,
// whereas, by default, these attributes are ignored by the *structural* and
// *symbolic* methods.  Note that an optional attribute mask may be supplied to
// the *structural* and *symbolic* methods to indicate that some or all of
// these field attributes should also be compared by the methods.
//
///Synopsis
///--------
// Given the above classification, the method names for the utilities in this
// component are formed by taking the base Relationship name and inserting a
// grammatically appropriate form of the Criterion name (except that the
// *exact* Criterion name can be safely omitted without confusion).
//
///Comparison Methods on Schemas
///- - - - - - - - - - - - - - -
//..
//  bool isSuperset(const bdem_Schema&, const bdem_Schema&);
//..
//
///Comparison Methods on Enumeration Definitions
///- - - - - - - - - - - - - - - - - - - - - - -
//..
//  bool isSuperset(const bdem_EnumerationDef& super,
//                  const bdem_EnumerationDef& sub);
//
//  bool areEquivalent(const bdem_EnumerationDef& e1,
//                     const bdem_EnumerationDef& e2);
//..
//
///Comparison Methods on Record Definitions
/// - - - - - - - - - - - - - - - - - - - -
//..
//  bool isSuperset(const bdem_RecordDef&, const bdem_RecordDef&);
//  bool areEquivalent(const bdem_RecordDef&, const bdem_RecordDef&);
//
//  bool isStructuralSuperset(const bdem_RecordDef&, const bdem_RecordDef&);
//  bool areStructurallyEquivalent(const bdem_RecordDef&,
//                                 const bdem_RecordDef&);
//
//  bool isSymbolicSuperset(const bdem_RecordDef&, const bdem_RecordDef&);
//  bool areSymbolicallyEquivalent(const bdem_RecordDef&,
//                                 const bdem_RecordDef&);
//
//  bool isStructuralSuperset(const bdem_RecordDef&,
//                            const bdem_RecordDef&,
//                            int   attributeMask);
//  bool areStructurallyEquivalent(const bdem_RecordDef&,
//                                 const bdem_RecordDef&,
//                                 int   attributeMask);
//
//  bool isSymbolicSuperset(const bdem_RecordDef&,
//                          const bdem_RecordDef&,
//                          int   attributeMask);
//  bool areSymbolicallyEquivalent(const bdem_RecordDef&,
//                                 const bdem_RecordDef&,
//                                 int   attributeMask);
//..
//
///Schema Negotiation, Modes of Data Access, and Comparison Criteria
///-----------------------------------------------------------------
// One of the main usage paradigms of 'bdem' is "schema negotiation".  Two
// entities (for convenience, hereby referred to as "client" and "server") each
// have a schema representing data.  At any one juncture, one record definition
// of the client's schema represents the client's need for data, and the
// server's schema represents the data that the server can provide.  Loosely
// speaking, if the server's schema is a *superset* of the client's schema,
// then the server can satisfy the client's request.  There are, however, some
// problems with the above statement.  First, we cannot speak "loosely" but
// must provide one or more precise definitions of *superset* -- the Criteria
// discussed above.  But worse than that, the two schemas can fail to obey a
// reasonably defined *superset* relationship, but the server still can have
// sufficient data to satisfy the client's needs.  In this section, we will
// explain the two main reasons why this "false negative" can occur, and what
// the user needs to understand about schemas and their usage to obtain the
// most effective schema negotiation and subsequent data access.
//
///Technical Details Regarding 'bdem_Schema'
///- - - - - - - - - - - - - - - - - - - - -
// We assume that the reader is reasonably familiar with the components that,
// collectively, define a schema (i.e., 'bdem_schema', 'bdem_enumerationdef',
// 'bdem_recorddef', and 'bdem_fielddef') but we focus here on a few details,
// since those details are subtle and they govern the definition and
// appropriate use of the various comparison functions.  It should be clear
// that each record/enumeration definition in a schema, and each field in a
// record definition, may optionally be named.  Also, each field in a record
// definition may also optionally have an integral id, any field representing
// a sequence or choice may optionally be constrained by a record definition
// within the schema, and any field representing a string, 'int', string array,
// or 'int' array may optionally be constrained by an enumeration definition
// within the schema.  What might be less clear is that, to the user who is
// interested in data rather than in schemas for their own sake, constraint
// (i.e., record/enumeration definition) names and indices are rather less
// important than field names and ids, or field indices.  Constraint names and
// indices are internal organizational details of a schema, but have no effect
// on the actual location of a data element in, e.g., a sequence that is
// conformant with the record definition containing the constrained field.
//
// This point -- that constraint names and indices are just internal schema
// details -- explains why one schema could fail to be a *superset* of another,
// and yet could adequately describe all of the *data* described by the latter
// schema.  Specifically, a comparison of *schemas* would reasonably compare
// record definition names and/or indices, but, if the *content* of a
// constraint record definition is a *superset* of the *content* of a
// corresponding constraint record definition, the location of conformant
// *data* will not be affected by differences in constraint names or constraint
// indices.  Data access -- whether by name, id, or index -- is governed by the
// *field* definition names and ids, and *field* definition indices.  The above
// discussion also explains why the record-definition-based comparisons, even
// for the "exact" Criterion, ignore constraint names and indices.
//
///Schema Negotiations and User-Defined Conventions
/// - - - - - - - - - - - - - - - - - - - - - - - -
// The second of the two above-mentioned reasons why a given *superset* method
// could fail even when the data corresponds completely is more obvious, but no
// less important: Users must agree on exactly how data is to be organized and
// accessed before that data can be shared effectively.  For example, if the
// client and server have schemas that are identical (according to
// 'operator=='), then the server can clearly (mechanically) satisfy any
// request that the client can make.  If the fields within the schemas are
// unnamed and have no ids, however, and if the client and server know nothing
// more about each other than their schemas, then it is not 100% certain that
// the server's data will be useful to the client, even if all requested fields
// can be populated with data of the right types.  In short, it is possible
// that the client has reached a "wrong number", and the schema agreement is
// coincidental.
//
// Although the above scenario might be far-fetched, and a true "wrong number"
// might better be considered a programming error at a higher level than 'bdem'
// should be addressing, we offer this extreme case to illustrate that, in all
// cases, some assumptions must be made about data representations.
//
// A more practical (and therefore more interesting) scenario occurs when the
// client knows the server and the server's conventions as of a particular
// time, but the server has since augmented its offerings (i.e., the client
// knows a less current "version" of the server's schema).  In this case, the
// client must know something about *how* the server is allowed to augment its
// data representation.  For example, the server can safely add fields to the
// ends of record definitions under any of the *superset* criteria in this
// component.  Similarly, if the server and client have "agreed" to by-name
// negotiation, then the server can insert fields into the middle of existing
// record definitions (e.g., to keep the record definition's logical
// organization tidy); if server and client negotiate using the *symbolic*
// Criterion methods, then the negotiation will be accurate.  If, however, the
// client is interested in the most efficient possible data access (i.e.,
// by-index), and therefore uses the *structural* Criterion, then the server's
// inserted record definition will cause the comparison to fail, even if
// by-name access would still work perfectly.
//
// As the set of examples above suggests, pure by-index "negotiation" is more
// error-prone than is by-name or by-id negotiation, because of the possibility
// of augmenting one of the schemas.  However, by-index is more efficient than
// by-name and by-id access.  For these reasons, both the *symbolic* and
// *structural* comparison Criteria are supported.
//
///Method Definitions
///------------------
// This section provides exact definitions for the comparison Criteria used by
// the functions of this component.  The definitions are equivalent to the
// function-level documentation, but are intentionally more structured and more
// abstract in that they deliberately do no not refer explicitly to "method
// behavior", and they use a suggestive but fictitious set of pseudo-method
// names (e.g., 'constraintIndex') to define specific attributes of a record
// or field definition.  Note that these pseudo-methods evaluate to a unique
// value if the object in question does not have the attribute indicated in the
// pseudo-method name.  E.g.:
//..
//  'name(a)' equals 'name(b)'
//..
// evaluates to logical 'true' if both 'a' and 'b' are unnamed.  Similarly:
//..
//  'id(a)' equals 'id(b)'
//..
// evaluates to logical 'true' if both 'a' and 'b' have no id.
//
///'isSuperset' for Schemas
/// - - - - - - - - - - - -
// A schema 'super' is an *exact* *superset* of a schema 'sub' if for each
// field 'f1' in each record definition 'r1' in 'sub' there exists a field 'f2'
// in a record definition 'r2' in 'super' such that:
//: o 'index(r1)' in 'sub' equals 'index(r2)' in 'super'
//: o 'name(r1)' in 'sub' equals 'name(r2)' in 'super'
//: o 'recordType(r1)' equals 'recordType(r2)'
//: o 'index(f1)' in 'r1' equals 'index(f2)' in 'r2'
//: o 'name(f1)' in 'r1' equals 'name(f2)' in 'r2'
//: o 'id(f1)' in 'r1' equals 'id(f2)' in 'r2'
//: o 'elementType(f1)' equals 'elementType(f2)'
//: o 'isNullable(f1)' equals 'isNullable(f2)'
//: o 'hasDefaultValue(f1)' equals 'hasDefaultValue(f2)'
//: o 'defaultValue(f1)' equals 'defaultValue(f2)' (provided both 'f1' and
//:   'f2' have default values)
//: o 'formattingMode(f1)' equals 'formattingMode(f2)'
//: o 'constraintIndex(f1)' equals 'constraintIndex(f2)'
//
// and for each enumerator 'e1' in each enumeration definition 'E1' in 'sub'
// there exists an enumerator 'e2' in enumeration definition 'E2' in 'super'
// such that:
//: o 'index(e1)' in 'sub' equals 'index(e2)' in 'super'
//: o 'name(e1)' in 'sub' equals 'name(e2)' in 'super'
//: o 'name(e1)' in 'E1' equals 'name(e2)' in 'E2'
//: o 'id(e1)' in 'E1' equals 'id(e2)' in 'E2'
//
// Note that each such record definition 'r2' in 'super' necessarily satisfies
// the 'isSuperset' relationship with the corresponding record definition 'r1'
// and each such enumeration definition 'E2' in 'super' necessarily satisfies
// the 'isSuperset' relationship with the corresponding enumeration definition
// 'E1', as per the 'isSuperset' relationship for record and enumeration
// definitions defined below.  This relationship considers constraint (record
// and enumeration) indices and therefore imposes stronger considerations on
// 'r1' and 'r2' than does the 'isSuperset' relationship for record definitions
// and stronger considerations on 'E1' and 'E2' than does the 'isSuperset'
// relationship for enumeration definitions.  Also note that default values
// apply only to scalar 'bdem' types.
//
///'isSuperset' for Record Definitions
///- - - - - - - - - - - - - - - - - -
// A record definition 'super' is an *exact* *superset* of a record definition
// 'sub' if for each field 'f1' in 'sub' there exists a field 'f2' in 'super'
// such that:
//: o 'recordType(sub)' equals 'recordType(super)'
//: o 'index(f1)' in 'sub' equals 'index(f2)' in 'super'
//: o 'name(f1)' in 'sub' equals 'name(f2)' in 'super'
//: o 'id(f1)' in 'sub' equals 'id(f2)' in 'super'
//: o 'elementType(f1)' equals 'elementType(f2)'
//: o 'isNullable(f1)' equals 'isNullable(f2)'
//: o 'hasDefaultValue(f1)' equals 'hasDefaultValue(f2)'
//: o 'defaultValue(f1)' equals 'defaultValue(f2)' (provided both 'f1' and 'f2'
//:   have default values)
//: o 'formattingMode(f1)' equals 'formattingMode(f2)'
//: o 'constraint(f2)' is an exact *superset* of 'constraint(f1)' (recursively
//:   for record definitions, non-recursively for enumeration definitions)
//
// Note that the last condition applies this relationship recursively to pairs
// of corresponding constraints (that are themselves record definitions), and
// applies the 'isSuperset' relationship (non-recursively) defined below for
// constraints that are enumeration definitions.  Also note that the last
// condition evaluates to 'true' if both 'f1' and 'f2' are unconstrained, and
// to 'false' if exactly one of ('f1', 'f2') is unconstrained.  Also note that
// default values apply only to scalar 'bdem' types.
//
///'isSuperset' for Enumeration Definitions
/// - - - - - - - - - - - - - - - - - - - -
// An enumeration definition 'super' is an *exact* *superset* of an enumeration
// definition 'sub' if for each enumerator 'e1' in enumeration definition 'E1'
// in 'sub' there exists an enumerator 'e2' in enumeration definition 'E2' in
// 'super' such that:
//: o 'name(e1)' in 'E1' equals 'name(e2)' in 'E2'
//: o 'id(e1)' in 'E1' equals 'id(e2)' in 'E2'
//
// Note that enumeration definitions are not recursive.  Only the enumerators
// within the two enumeration definitions need to be checked.
//
///'areEquivalent' for Record Definitions
/// - - - - - - - - - - - - - - - - - - -
// A record definition 'r1' is *exactly* equivalent to a record definition 'r2'
// if for each field 'f1' in 'r1' there exists a field 'f2' in 'r2' such that:
//: o 'recordType(r1)' equals 'recordType(r2)'
//: o 'index(f1)' in 'r1' equals 'index(f2)' in 'r2'
//: o 'name(f1)' in 'r1' equals 'name(f2)' in 'r2'
//: o 'id(f1)' in 'r1' equals 'id(f2)' in 'r2'
//: o 'elementType(f1)' equals 'elementType(f2)'
//: o 'isNullable(f1)' equals 'isNullable(f2)'
//: o 'hasDefaultValue(f1)' equals 'hasDefaultValue(f2)'
//: o 'defaultValue(f1)' equals 'defaultValue(f2)' (provided both 'f1' and 'f2'
//:   have default values)
//: o 'formattingMode(f1)' equals 'formattingMode(f2)'
//: o 'constraint(f2)' is exactly equivalent to 'constraint(f1)' (recursively
//:   for record definitions, non-recursively for enumeration definitions)
//
// Note that the last condition applies this relationship recursively to pairs
// of corresponding constraints (that are themselves record definitions), and
// applies the 'areEquivalent' relationship (non-recursively) defined below for
// constraints that are enumeration definitions.  Also note that the last
// condition evaluates to 'true' if both 'f1' and 'f2' are unconstrained, and
// to 'false' if exactly one of ('f1', 'f2') is unconstrained.  Also note that
// default values apply only to scalar 'bdem' types.
//
///'areEquivalent' for Enumeration Definitions
///- - - - - - - - - - - - - - - - - - - - - -
// An enumeration definition 'E1' is *exactly* equivalent to an enumeration
// definition 'E1' if for each enumerator 'e1' in enumeration definition 'E1'
// there exists an enumerator 'e2' in enumeration definition 'E2' such that:
//: o 'name(e1)' in 'E1' equals 'name(e2)' in 'E2'
//: o 'id(e1)' in 'E1' equals 'id(e2)' in 'E2'
//
///'isStructuralSuperset' for Record Definitions
///- - - - - - - - - - - - - - - - - - - - - - -
// A record definition 'super' is a *structural* *superset* of a record
// definition 'sub' if for each field 'f1' in 'sub' there exists a field 'f2'
// in 'super' such that:
//: o 'recordType(sub)' equals 'recordType(super)'
//: o 'index(f1)' in 'sub' equals 'index(f2)' in 'super'
//: o 'elementType(f1)' equals 'elementType(f2)'
//: o 'isNullable(f1)' equals 'isNullable(f2)'
//: o 'recordConstraint(f2)' is a *structural* *superset* of
//:   'recordConstraint(f1)' (recursively)
//
// Furthermore, an optionally-supplied 'attributeMask' specifies whether some
// or all of the following relationships must also hold in the *structural*
// *superset* comparison:
//: o 'hasDefaultValue(f1)' equals 'hasDefaultValue(f2)'
//: o 'defaultValue(f1)' equals 'defaultValue(f2)' (provided both 'f1' and 'f2'
//:   have default values)
//: o 'formattingMode(f1)' equals 'formattingMode(f2)'
//: o 'enumerationConstraint(f1)' equals 'enumerationConstraint(f2)'
//
// Note that the condition on constraints applies this relationship recursively
// to pairs of corresponding constraints (that are themselves record
// definitions), and non-recursively to pairs of corresponding constraints that
// are enumeration definitions (if the 'attributeMask' is set).  Also note that
// the condition on constraints evaluates to 'true' if both 'f1' and 'f2' are
// unconstrained, and to 'false' if exactly one of ('f1', 'f2') is
// unconstrained.
//
///'areStructurallyEquivalent' for Record Definitions
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// A record definition 'r1' is *structurally* equivalent to a record
// definition 'r2' if for each field 'f1' in 'r1' there exists a field 'f2' in
// 'r2' such that:
//: o 'recordType(r1)' equals 'recordType(r2)'
//: o 'index(f1)' in 'r1' equals 'index(f2)' in 'r2'
//: o 'elementType(f1)' equals 'elementType(f2)'
//: o 'isNullable(f1)' equals 'isNullable(f2)'
//: o 'recordConstraint(f2)' is structurally equivalent to
//:   'recordConstraint(f1)' (recursively)
//
// Furthermore, an optionally-supplied 'attributeMask' specifies whether some
// or all of the following relationships must also hold in the *structural*
// equivalence comparison:
//: o 'hasDefaultValue(f1)' equals 'hasDefaultValue(f2)'
//: o 'defaultValue(f1)' equals 'defaultValue(f2)' (provided both 'f1' and 'f2'
//:   have default values)
//: o 'formattingMode(f1)' equals 'formattingMode(f2)'
//: o 'enumerationConstraint(f1)' equals 'enumerationConstraint(f2)'
//
// Note that the condition on constraints applies this relationship recursively
// to pairs of corresponding constraints (that are themselves record
// definitions), and non-recursively to pairs of corresponding constraints that
// are enumeration definitions (if the 'attributeMask' is set).  Also note that
// the condition on constraints evaluates to 'true' if both 'f1' and 'f2' are
// unconstrained, and to 'false' if exactly one of ('f1', 'f2') is
// unconstrained.
//
///'isSymbolicSuperset' for Record Definitions
///- - - - - - - - - - - - - - - - - - - - - -
// A record definition 'super' is a *symbolic* *superset* of a record
// definition 'sub' if for each *named* field 'f1' in 'sub' there exists a
// field 'f2' in 'super' such that:
//: o 'recordType(sub)' equals 'recordType(super)'
//: o 'name(f1)' in 'sub' equals 'name(f2)' in 'super'
//: o 'id(f1)' in 'sub' equals 'id(f2)' in 'super'
//: o 'elementType(f1)' equals 'elementType(f2)'
//: o 'isNullable(f1)' equals 'isNullable(f2)'
//: o 'recordConstraint(f2)' is a *symbolic* *superset* of
//:   'recordConstraint(f1)' (recursively)
//
// Furthermore, an optionally-supplied 'attributeMask' specifies whether some
// or all of the following relationships must also hold in the *symbolic*
// *superset* comparison:
//: o 'hasDefaultValue(f1)' equals 'hasDefaultValue(f2)'
//: o 'defaultValue(f1)' equals 'defaultValue(f2)' (provided both 'f1' and
//:   'f2' have default values)
//: o 'formattingMode(f1)' equals 'formattingMode(f2)'
//: o 'enumerationConstraint(f1)' equals 'enumerationConstraint(f2)'
//
// Note that fields that have neither a name nor an explicit id are
// completely ignored for this comparison.  Also note that the condition on
// constraints applies this relationship recursively to pairs of corresponding
// constraints (that are themselves record definitions), and non-recursively to
// pairs of corresponding constraints that are enumeration definitions (if the
// 'attributeMask' is set).  Finally, note that the condition on constraints
// evaluates to 'true' if both 'f1' and 'f2' are unconstrained, and to 'false'
// if exactly one of ('f1', 'f2') is unconstrained.
//
///'areSymbolicallyEquivalent' for Record Definitions
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// A record definition 'r1' is *symbolically* equivalent to a record definition
// 'r2' if for each *named* field 'f1' in 'r1' there exists a *named* field
// 'f2' in 'r2' such that:
//: o 'recordType(r1)' equals 'recordType(r2)'
//: o 'name(f1)' in 'r1' equals 'name(f2)' in 'r2'
//: o 'id(f1)' in 'r1' equals 'id(f2)' in 'r2'
//: o 'elementType(f1)' equals 'elementType(f2)'
//: o 'isNullable(f1)' equals 'isNullable(f2)'
//: o 'recordConstraint(f2)' is symbolically equivalent to
//:   'recordConstraint(f1)' (recursively)
//
// Furthermore, an optionally-supplied 'attributeMask' specifies whether some
// or all of the following relationships must also hold in the *symbolic*
// equivalence comparison:
//: o 'hasDefaultValue(f1)' equals 'hasDefaultValue(f2)'
//: o 'defaultValue(f1)' equals 'defaultValue(f2)' (provided both 'f1' and
//:   'f2' have default values)
//: o 'formattingMode(f1)' equals 'formattingMode(f2)'
//: o 'enumerationConstraint(f1)' equals 'enumerationConstraint(f2)'
//
// Note that fields that have neither a name nor an explicit id are completely
// ignored for this comparison.  Also note that the condition on constraints
// applies this relationship recursively to pairs of corresponding constraints
// (that are themselves record definitions), and non-recursively to pairs of
// corresponding constraints that are enumeration definitions (if the
// 'attributeMask' is set).  Finally, note that the condition on constraints
// evaluates to 'true' if both 'f1' and 'f2' are unconstrained, and to 'false'
// if exactly one of ('f1', 'f2') is unconstrained.
//
///Observations on Usage
///---------------------
// In this section we discuss some of the finer points of by-name and by-index
// data access using schemas negotiated with the comparison methods of this
// component and the conformance methods of 'bdem_schemaaggregateutil'.  Once
// again, a subtle detail of the schema meta-data will influence usage of these
// methods for data access.
//
// Consider the following three statements illustrating how the three
// *superset* methods that compare record definitions can be used to validate
// different forms of data access in a top-level sequence.
//
// I. Given:
//: 1 a record definition 'sub' containing constrained fields (constrained by
//:   another record or enumeration definition) nested to an arbitrary depth
//:   (i.e., the constraints that are record definitions may have constrained
//:   fields, etc.)
//: 2 a sequence 'S1' that is deep-conformant with 'sub' (see
//:   'bdem_schemaaggregateutil')
//: 3 a record definition 'super' that is a *symbolic* *superset* of 'sub'
//: 4 a sequence 'S2' that is deep-conformant with 'super'
//
// Then, for any element in 'S1' (at any depth of nesting) that can be uniquely
// identified by a sequence 'S' of field names and field ids from among those
// in 'sub' and its (nested) constraint record definitions, there is a
// corresponding element in 'S2' that can be uniquely identified by the same
// sequence 'S' of field names and field ids.
//
// II. In Condition (3) of Statement I., replace the Criterion *symbolic* with
// the Criterion *structural*, and, in the "Then" clause of that statement,
// replace the term "field names and field ids" with "field indices" in both
// occurrences.
//
// III. In Condition (3) of Statement I., replace the Criterion *symbolic* with
// the Criterion *exact*, and, in the "Then" clause of that statement,
// replace the phrase "sequence 'S' of field names and ids" with "sequence 'S'
// of arbitrarily-mixed field names, field ids, and field indices" in both
// occurrences.
//
// Note that these three statements each refer to data access by a sequence of
// *field* labels (i.e., names, ids, or indices) only.  This mode of element
// referencing does not provide access to every possible element contained in a
// list.  Specifically, any element that is contained in an element of the list
// that is itself a *table* will require a row index along with the field label
// that indicates the column within the table.  The schema, however, has no way
// of expressing either how many rows a table has, nor which row within a
// given table is to be indicated.  Therefore, all of the above statements
// regarding the safe usage of a form of element reference for more than one
// top-level list becomes false if the element reference is allowed to involve
// an element within a table (i.e., to involve a row index) at any level of
// the hierarchical reference.  The user must always verify that a given table
// actually has a specific row before that row index is used in accessing any
// data.
//
///Usage
///-----
// The following example features a server that can supply data described by
// its schema.  Due to expanding needs from its clients, the server schema
// goes through three extensions (i.e., a total of four schema Versions: 1.0,
// 1.1, 1.2, and 1.3).  The client in this example is only aware of the first
// server schema, Version 1.0.  The problem that we will address is how the
// client can use the server's schema to verify that the server's data is in
// fact useful to the client.
//
// To illustrate the essential features of this "schema negotiation", we have
// chosen especially simple data models, and (intentionally) have made version
// changes that represent questionable data-modeling practice.  The user is
// encouraged to focus on the schema and utility usage patterns in the face of
// these changes, and not on the wisdom of the changes themselves.  In
// particular, this example focuses on schema comparison and data access only;
// all details about how data is exchanged and used are omitted for simplicity.
// See the 'bdem_schema' component for details on how to build a schema object,
// and see the 'bdem' package-level documentation for more details on data
// marshaling using the 'bdex' externalization mechanisms.
//
// The server's job in this example is to supply the price of a stock.  The
// server uses 'bdem' messaging -- the server sends data as a sequence and
// meta-data as a 'bdem_Schema'.  In the initial data model, the data is to be
// sent as a sequence whose first element is an object of type 'bsl::string'
// containing the stock's symbol, and whose second element is itself a sequence
// containing the price structure.  This sequence is described by the "Stock"
// record definition within the following schema:
//..
//  {                                           // Server Schema Version 1.0
//      RECORD "Price" {                        // record definition {0}
//          DOUBLE         "value";
//          INT            "priceCode";
//      }
//      RECORD "Stock" {                        // record definition {1}
//          STRING         "symbol";
//          SEQUENCE       "priceVal" constrained by record "Price";
//      }
//  }
//..
// In 'bdem' terms, we say that a sequence containing the stock data is
// constrained by the "Stock" record of the above schema.  The first element
// of that sequence is a 'STRING' named "symbol", and the second element is a
// sequence named "priceVal" that is constrained by the "Price" record.  Here,
// a "Price" is a 'double' value named "value" and an 'int' value named
// "priceCode".
//
// This schema is made "well known" in human-friendly form, so that programmers
// can code to it.  Nevertheless, it is prudent for the user ("client") to
// request the server's schema and validate that schema against a local schema.
// In this example, we will step through several specific "validation" checks,
// and describe exactly what is being validated.  In particular, we will show
// how some "validation" checks can lead to a false negative result (i.e., the
// schema is usable, but the test says that it is not).
//
// Throughout this example, we will assume that the client has a schema object,
// 'clientSchema', having the value given by the above textual representation.
// All code snippets in this example are in the client's source code.  Objects
// labeled "server" are assumed to have been transported to the client and
// "unexternalized" by the client (see the 'bdex' package documentation).
//
// The first, simplest test that the client can perform is for schema equality.
// Assuming that the client has build a 'serverSchema' object, and, at this
// time, 'serverSchema' has the value of Server Schema Version 1.0 above, the
// following test will work just fine:
//..
//      if (serverSchema == clientSchema) {
//          // process data with confidence
//      }
//      else {
//          // conclude that can't use data; clean up and go elsewhere
//      }
//..
// Since the above 'if' test will succeed, the client can process the data with
// confidence.
//
// Consider now the case where the server upgrades its data model.
// Specifically, it is decided for some business reason that the 'priceVal'
// structure must have a timestamp, so that any user will always be able to
// convert the price to a different currency code and get the historically
// correct result.  The server adds to the end of the "Price" record definition
// a field of type 'DATETIME', which in turn implies that the sequence named
// "priceVal" will now have a third element of type 'bdet_DateTime'.  The new
// Server Schema Version 1.1 is as follows:
//..
//  {                                           // Server Schema Version 1.1
//      RECORD "Price" {                        // record definition {0}
//          DOUBLE         "value";
//          INT            "priceCode";
//          DATETIME       "timeStamp";
//      }
//      RECORD "Stock" {                        // record definition {1}
//          STRING         "symbol";
//          SEQUENCE       "priceVal" constrained by record "Price";
//      }
//  }
//..
// Returning to our validations, the above client code will conclude that the
// server's data is unusable, but in fact the server's data will meet the
// client's needs perfectly (since the client knows nothing of the existence
// of, nor the need for, a timestamp).  The client code can therefore be
// improved as follows:
//..
//      if (isSuperset(serverSchema, clientSchema)) {
//          // process data with confidence
//      }
//      else {
//          // conclude that can't use data; clean up and go elsewhere
//      }
//..
// The 'isSuperset' test will succeed whether 'serverSchema' is using Version
// 1.0 or Version 1.1, and the client code will be able to access data
// perfectly in either event.  In general, the 'isSuperset' method is always
// safer (with respect to false negatives) than the schema 'operator=='.  To
// anticipate the next problem, however, note that the 'isSuperset' method
// comparing two schemas (as opposed to the overloaded method comparing two
// record definitions) is inherently more restrictive than users typically need
// in the course of schema negotiations.  To illustrate this point, consider
// the following upgrade to the server's data model.
//
// For business reasons that remain obscure to the poor programmer, it is
// decided that the price structure must also contain information on the recent
// price history just prior to the actual quote.  This is accomplished by
// adding to the end of the "Price" record definition a sequence constrained by
// the "PriceHistory" record definition.  The new schema, Version 1.2, is as
// follows:
//..
//  {                                           // Server Schema Version 1.2
//      RECORD "PriceHistory" {                 // record definition {0}
//          DOUBLE_ARRAY   "prices";
//          DATETIME_ARRAY "timeStamps";
//      }
//      RECORD "Price" {                        // record definition {1}
//          DOUBLE         "value";
//          INT            "priceCode";
//          DATETIME       "timeStamp";
//          SEQUENCE       "history" constrained by record "PriceHistory";
//      }
//      RECORD "Stock" {                        // record definition {2}
//          STRING         "symbol";
//          SEQUENCE       "priceVal" constrained by record "Price";
//      }
//  }
//..
// Remembering that our purpose is to contrast schema validation methods, we
// observe that the client code above using the 'isSuperset' method (on
// schemas) will now conclude that the server's data is unusable, even though
// the server did nothing more than add yet another element to the end of the
// "priceVal" sequence within the parent sequence.  This change to the *data*
// is the same irrelevant extension, and the client should be just as happy as
// before with the server's data.  The problem is that the server added a
// constrained sequence, which means that the new constraint record definition
// must be inserted *before* the "Price" record definition.  But, as we
// described above, the schema comparisons depend on the whole structure of the
// schema, and are therefore too restrictive.
//
// The solution is to use the 'isSuperset' comparison method on the relevant
// *record* *definitions* of the two schemas.  The new test would be as
// follows:
//..
//  const bdem_RecordDef *serverRec = serverSchema.lookupRecord("Stock");
//  const bdem_RecordDef *clientRec = clientSchema.lookupRecord("Stock");
//
//  if (serverRec && isSuperset(*serverRec, *clientRec)) {
//      // process data with confidence
//  }
//  else {
//      // conclude that can't use data; clean up and go elsewhere
//  }
//..
// This test will (correctly) succeed, and the client can process the server's
// data with confidence.  The reason is because the 'isSuperset' method on
// record definitions is not concerned with record definition indices (or
// record definition names, either), but rather allows the schema to find the
// constraint by its internal mechanism (which happens to be the record
// definition's address).  Note that the client is quite confident about its
// own schema, but wisely chooses to verify that 'serverSchema' actually has a
// record definition named "Stock" -- the client checks that 'serverRec' is not
// null before dereferencing it in the 'isSuperset' method call.
//
// For the final data model change, consider that the "Stock" record definition
// must be augmented with an integer exchange code to document exactly where
// the price quote comes from.  This time, however, the decision is made that,
// in order to keep closely related information close together within the
// sequence's elements, the "exchangeCode" field will be inserted at field
// index position 1 rather than just added to the end (i.e., at index 2).  The
// new schema, Version 1.3, is as follows:
//..
//  {                                           // Server Schema Version 1.3
//      RECORD "PriceHistory" {                 // record definition {0}
//          DOUBLE_ARRAY   "prices";
//          DATETIME_ARRAY "timeStamps";
//      }
//      RECORD "Price" {                        // record definition {1}
//          DOUBLE         "value";
//          INT            "priceCode";
//          DATETIME       "timeStamp";
//          SEQUENCE       "history" constrained by record "PriceHistory";
//      }
//      RECORD "Stock" {                        // record definition {2}
//          STRING         "symbol";
//          INT            "exchangeCode";
//          SEQUENCE       "priceVal" constrained by record "Price";
//      }
//  }
//..
// This decision has several consequences, which we will explore.  The first
// consequence, from our perspective, is that the validation test above using
// 'isSuperset' on records, will now fail.  This brings up the question: Is
// this a false negative?  The answer depends upon how the client intends to
// access the data.  If the client intends to access the data by index, then
// there may be a problem.  We will discuss the problem and a possible solution
// below.  However, since in the server's schema, each field is named, it is
// always possible to access the data by name.  In this case, the 'isSuperset'
// method is too restrictive, and the 'isSymbolicSuperset' method is adequate.
// The test would then be as follows:
//..
//  const bdem_RecordDef *clientRec = clientSchema.lookupRecord("Stock");
//
//  if (serverRec && isSymbolicSuperset(*serverRec, *clientRec)) {
//      // process data with confidence
//  }
//  else {
//      // conclude that can't use data; clean up and go elsewhere
//  }
//..
// This test will succeed, and the client will be able safely to access data
// by name.
//
// To return to the question of by-index access, we must consider a few
// details.  For the case where the client needs by-index access because it is
// much faster, and the client will be looking at a continuous stream of like
// data (i.e., sequences) all conformant with the same schema, *but* the client
// can do the work locally, it is always possible to pre-compute the indices
// (once) and use them repeatedly.  As a simple example, let us assume that the
// price code will not change from sequence to sequence, and so the client
// needs only to extract the 'double' value of the price.  Using only
// 'serverSchema' and the known *field* names, the client can cache the
// relevant indices and use them to extract a 'double' value from the server's
// data, 'serverList':
//..
//  Sequence serverList;    // population mechanism not shown
//  int      serverRecIdx;  // population mechanism not shown
//  // ...
//
//  // These five lines are executed once.
//  const bdem_RecordDef&  serverRec = serverSchema.record(serverRecIdx);
//  int                    priceIdx  = serverRec.fieldIndex("priceVal");
//  const bdem_FieldDef   *priceFld  = serverRec.field(priceListIdx);
//  const bdem_RecordDef  *priceRec  = priceFld.constraint();
//  int                    valueIdx  = priceRec.fieldIndex("value");
//
//  // The following line may be executed as often as needed, presumably
//  // changing 'serverList' each time, but reusing the pre-calculated indices.
//
//  double priceValue = serverList.theList(priceIdx).theDouble(valueIdx);
//..
// Note that the code above could be slightly simplified by using constraint
// record definition names, but we choose not to do so to illustrate that
// record definition names are *not* necessary for data access.  The one
// *possible* exception is the record definition with which the requested data
// is conformant.  As a hyper-technicality, the server must make this record
// definition known to the client in some form.  One possibility is the
// pre-agreed name, in which case that record definition name *is* needed.  The
// server can always furnish the record definition index as part of the initial
// response to the client.  In the code above, we also assumed that the server
// has supplied the record definition index into 'serverSchema', which the
// client has already stored in 'serverRecIdx'.  Also note that no further
// validation is needed, since we assume that the server's sequence is
// guaranteed to be deep-conformant with the indicated record definition.
//
// To recap, if the client needs by-index data access, the indices can be
// generated (by-name) from a schema that is a *symbolic* *superset* of the
// client's schema, where the required names are all known.  Knowledge of
// constraint record definition names is never needed, but the client must gain
// access to the server's *conformant* (i.e., top-level) record definition,
// either by a (known) name or else by a server-provided index.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

class bdem_EnumerationDef;
class bdem_RecordDef;
class bdem_Schema;

                        //=======================
                        // struct bdem_SchemaUtil
                        //=======================

struct bdem_SchemaUtil {
    // This untility class defines a set of pure procedures that operate on
    // objects of schema-related types, independently of the data that they
    // model.  Specifically, the boolean-valued methods in this utility class
    // implement a variety of comparison operations between pairs of
    // (1) 'bdem_EnumerationDef' objects, (2) 'bdem_RecordDef' objects,
    // and (3) 'bdem_Schema' objects.  These comparisons operations include
    // both the *structural* and *symbolic* forms of equivalence and
    // containment.  Field attributes can be used to configure various aspects
    // of record definition comparison that arise from their potentially
    // recursive nature.

    // TYPES
    enum FieldAttribute {
        // The 'FieldAttribute' enumeration defines values used to construct
        // attribute masks that are optionally supplied to the methods that
        // apply the *structural* Criterion or *symbolic* Criterion for
        // comparing records.

        BDEM_DEFAULT_VALUE     = 0x1,  // compare field def'n default values
        BDEM_FORMATTING_MODE   = 0x2,  // compare field def'n formatting modes
        BDEM_CHECK_ENUMERATION = 0x4   // compare enum definitions
    };

    // CLASS METHODS
    static bool areEquivalent(const bdem_EnumerationDef& e1,
                              const bdem_EnumerationDef& e2);
        // Return 'true' if the specified 'e1' and 'e2' enumeration definition
        // constraints are equivalent, and 'false' otherwise.  Two enumeration
        // definitions are equivalent if they have the same number of
        // enumerators and each enumerator has the same string (name) and
        // integer (ID) representation.

    static bool areEquivalent(const bdem_RecordDef& r1,
                              const bdem_RecordDef& r2);
        // Return 'true' if the specified 'r1' and 'r2' record definitions are
        // equivalent, and 'false' otherwise.  Two record definitions are
        // equivalent if they have the same record type and the same number of
        // fields, and fields at corresponding index positions have the same
        // element type, constraint, name (or both fields are unnamed), id (or
        // both fields have no explicit id), default value, formatting mode,
        // and nullability.  Two constraints are considered "the same" if they
        // indicate records in their respective schemas that are (recursively)
        // equivalent with respect to their sequence of fields.  Note that
        // neither the names nor the record indices of constraints are relevant
        // for this comparison.  Also note that default values apply only to
        // scalar and array 'bdem' types.

    static bool areStructurallyEquivalent(const bdem_RecordDef& r1,
                                          const bdem_RecordDef& r2);
    static bool areStructurallyEquivalent(const bdem_RecordDef& r1,
                                          const bdem_RecordDef& r2,
                                          int                   attributeMask);
        // Return 'true' if the specified 'r1' and 'r2' record definitions are
        // structurally equivalent, and 'false' otherwise.  Two record
        // definitions are structurally equivalent if they have the same record
        // type and the same number of fields, and fields at the corresponding
        // index positions have the same element type and nullability.
        // Furthermore, for each field that has a constraint, the corresponding
        // field must have a constraint that is (recursively) structurally
        // equivalent to the corresponding constraint.  Optionally specify an
        // 'attributeMask' to indicate that corresponding fields in 'r1' and
        // 'r2' must also have equivalent default values (if
        // 'attributeMask & BDEM_DEFAULT_VALUE' is non-zero) and equivalent
        // formatting modes (if 'attributeMask & BDEM_FORMATTING_MODE' is
        // non-zero) for 'r1' and 'r2' to be considered structurally
        // equivalent.  Note that this comparison is solely based on field
        // indices; record and field names, field ids, and the names and
        // indices of constraints are not relevant for this comparison.  Also
        // note that the following are equivalent:
        //..
        //  bdem_SchemaUtil::areStructurallyEquivalent(r1, r2);
        //  bdem_SchemaUtil::areStructurallyEquivalent(r1, r2, 0);
        //..

    static bool areSymbolicallyEquivalent(const bdem_RecordDef& r1,
                                          const bdem_RecordDef& r2);
    static bool areSymbolicallyEquivalent(const bdem_RecordDef& r1,
                                          const bdem_RecordDef& r2,
                                          int                   attributeMask);
        // Return 'true' if the specified 'r1' and 'r2' record definitions are
        // symbolically equivalent, and 'false' otherwise.  Two record
        // definitions are symbolically equivalent if they have the same record
        // definition type, their fields having a name or id are in a
        // one-to-one correspondence, and fields having the same name and id
        // have the same element type and nullability.  Furthermore, for each
        // field that is named or has an id that is constrained, the
        // corresponding field must have a constraint that is (recursively)
        // symbolically equivalent to the corresponding constraint.  Optionally
        // specify an 'attributeMask' to indicate that corresponding fields in
        // 'r1' and 'r2' must also have equivalent default values (if
        // 'attributeMask & BDEM_DEFAULT_VALUE' is non-zero) and equivalent
        // formatting modes (if 'attributeMask & BDEM_FORMATTING_MODE' is
        // non-zero) for 'r1' and 'r2' to be considered symbolically
        // equivalent.  Note that record definition names are not relevant for
        // this comparison, nor are fields that have neither a name nor an
        // explicit id.  Also note that the following are equivalent:
        //..
        //  bdem_SchemaUtil::areSymbolicallyEquivalent(r1, r2);
        //  bdem_SchemaUtil::areSymbolicallyEquivalent(r1, r2, 0);
        //..

    static bool isSuperset(const bdem_EnumerationDef& super,
                           const bdem_EnumerationDef& sub);
        // Return 'true' if the specified 'super' is a *superset* of the
        // specified 'sub', and 'false' otherwise.  'super' is a *superset* of
        // 'sub' if for each enumerator in 'sub' there exists an enumerator in
        // 'super' with the same string (name) and integer (ID) representation.

    static bool isSuperset(const bdem_RecordDef& super,
                           const bdem_RecordDef& sub);
        // Return 'true' if the specified 'super' is a *superset* of the
        // specified 'sub', and 'false' otherwise.  'super' is a *superset* of
        // 'sub' if they have the same record type and each field in 'sub' has
        // the same element type, name (or both fields are unnamed), id (or
        // both fields have no explicit id), default value, formatting mode,
        // and nullability as the field at the corresponding index position in
        // 'super'.  Furthermore, for each field in 'sub' that is constrained,
        // the corresponding field in 'super' must have a constraint that is
        // (recursively) a *superset* of the constraint in 'sub'.  Note that
        // neither the names nor the record definition indices of constraints
        // are relevant for this comparison; this behavior (i.e., ignoring
        // constraint names and indices) is distinctly different from the
        // behavior of the 'isSuperset' method taking two schemas as its
        // arguments.  Also note that default values apply only to scalar and
        // array 'bdem' types.

    static bool isSuperset(const bdem_Schema& super, const bdem_Schema& sub);
        // Return 'true' if the specified 'super' is a *superset* of the
        // specified 'sub', and 'false' otherwise.  'super' is a *superset* of
        // 'sub' if each record definition in 'sub' has the same record type
        // and the same name as the record definition at the corresponding
        // index position in 'super' (or both record definitions at that index
        // position are unnamed), and each field in 'sub' has the same element
        // type, name (or both fields are unnamed), id (or both fields have no
        // explicit id), default value, formatting mode, and nullability as the
        // field in the corresponding record definition at the corresponding
        // field-definition index position in 'super'.  Furthermore, for each
        // field in 'sub' that is constrained, the corresponding field in
        // 'super' must have a constraint having the same record-definition
        // index as the constraint in 'sub'.  Note that if the constraint
        // record definition indices match, then the constraint in 'super' is
        // necessarily a subset of the constraint in 'sub', as defined by the
        // 'isSuperset' method taking two record definitions as arguments, and
        // their names (or lack thereof) will necessarily match.  Also note
        // that default values apply only to scalar and array 'bdem' types.

    static bool isStructuralSuperset(const bdem_RecordDef& super,
                                     const bdem_RecordDef& sub);
    static bool isStructuralSuperset(const bdem_RecordDef& super,
                                     const bdem_RecordDef& sub,
                                     int                   attributeMask);
        // Return 'true' if the specified 'super' is a *structural* *superset*
        // of the specified 'sub', and 'false' otherwise.  'super' is a
        // *structural* *superset* of 'sub' if they have the same record type
        // and each field in 'sub' has the same element type and nullability as
        // the field at the corresponding index position in 'super'.
        // Furthermore, for each field in 'sub' that is constrained, the
        // corresponding field in 'super' must have a constraint that is
        // (recursively) a structural *superset* of the constraint in 'sub'.
        // Optionally specify an 'attributeMask' to indicate that corresponding
        // fields in 'super' and 'sub' must also have equivalent default values
        // (if 'attributeMask & BDEM_DEFAULT_VALUE' is non-zero) and equivalent
        // formatting modes (if 'attributeMask & BDEM_FORMATTING_MODE' is
        // non-zero) for 'super' to be considered a structural *superset* of
        // 'sub'.  Note that this comparison is solely based on field indices;
        // record definition and field names, field ids, and the names and
        // indices of constraints are not relevant for this comparison.  Also
        // note that the following are equivalent:
        //..
        //  bdem_SchemaUtil::isStructuralSuperset(r1, r2);
        //  bdem_SchemaUtil::isStructuralSuperset(r1, r2, 0);
        //..

    static bool isSymbolicSuperset(const bdem_RecordDef& super,
                                   const bdem_RecordDef& sub);
    static bool isSymbolicSuperset(const bdem_RecordDef& super,
                                   const bdem_RecordDef& sub,
                                   int                   attributeMask);
        // Return 'true' if the specified 'super' is a *symbolic* *superset* of
        // the specified 'sub', and 'false' otherwise.  'super' is a *symbolic*
        // *superset* of 'sub' if they have the same record type, and for every
        // field in 'sub' that has a name or id there exists a field in 'super'
        // that has the same name (or both are unnamed), id (or both have no
        // explicit id), element type, and nullability.  Furthermore, for each
        // field in 'sub' that is constrained, the corresponding field in
        // 'super' must have a constraint that is (recursively) a *symbolic*
        // *superset* of the constraint in 'sub'.  Optionally specify an
        // 'attributeMask' to indicate that corresponding fields in 'super' and
        // 'sub' must also have equivalent default values (if
        // 'attributeMask & BDEM_DEFAULT_VALUE' is non-zero) and equivalent
        // formatting modes (if 'attributeMask & BDEM_FORMATTING_MODE' is
        // non-zero) for 'super' to be considered a *symbolic* *superset* of
        // 'sub'.  Note that record definition names are not relevant for this
        // comparison, nor are fields that are neither named nor have an
        // explicit id.  Also note that the following are equivalent:
        //..
        //  bdem_SchemaUtil::isSymbolicSuperset(r1, r2);
        //  bdem_SchemaUtil::isSymbolicSuperset(r1, r2, 0);
        //..
};

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
