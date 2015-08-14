// bdlaggxxx_aggregate.h                                              -*-C++-*-
#ifndef INCLUDED_BDLAGGXXX_AGGREGATE
#define INCLUDED_BDLAGGXXX_AGGREGATE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type to reference self-describing data.
//
//@CLASSES:
//  bdlaggxxx::Aggregate: reference to fully-introspective, dynamically-typed data
//
//@SEE_ALSO: bdlaggxxx_aggregateraw, bdlmxxx_elemtype, bdlmxxx_schema, bdlmxxx_list,
//           bdlmxxx_table, bdlmxxx_choice,
//           bdlmxxx_choicearray, bdlmxxx_elemref, bdlmxxx_convert,
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a fully-introspective
// data structure, 'bdlaggxxx::Aggregate', capable of representing a reference to
// any of the following types of data:
//..
//  - A single scalar value (integer, string, date, enumeration, etc.)
//  - A heterogeneous list (sequence) of values (with optional field names)
//  - A choice value containing a single selection from a list of possible
//    types (where each possible selection may have a field name)
//  - An array of homogeneous scalar values
//  - A table of homogeneous list values (rows)
//  - An array of homogeneous choice values (choice array items)
//..
// See the 'bdem_elemtypes' component for a complete list of types.
//
///Summary of 'bdem' Data Structures
///---------------------------------
// The 'bdlaggxxx::Aggregate' type relies heavily on the data structures in the
// 'bdem' package.  The bdem package contains four self-describing data
// containers: 'bdlmxxx::List', 'bdlmxxx::Choice', 'bdlmxxx::Table', and
// 'bdlmxxx::ChoiceArray' (often referred to simply as "list", "choice", "table",
// and "choice array" respectively, and collectively known as "bdem aggregate
// types").  Lists and tables contain a (run-time constructed) "row
// definition" consisting essentially of a sequence of 'bdlmxxx::ElemType::Type'
// enumerator values.  A 'bdlmxxx::List' contains a sequence of data elements of
// heterogeneous type, where each element's type is indicated by a
// corresponding entry in the list's row definition.  A 'bdlmxxx::Table' is an
// array of rows, where each row is like a 'bdlmxxx::List' except that all rows
// share the same row definition.  A 'bdlmxxx::Choice' contains a (run-time
// constructed) catalog of 'bdlmxxx::ElemType::Type' values, and a current data
// selection that belongs to one of the types in the catalog.  A
// 'bdlmxxx::ChoiceArray' is an array of items where each item is like a
// 'bdlmxxx::Choice' except that all items share the same catalog.
//
// A list may contain a field that is itself a list.  However, because fields
// are inserted into lists *by value*, there is no way to cause a list to
// directly or indirectly contain itself.  Similarly, lists can contain
// choices, choices can contain lists, choices can contain choices, table rows
// can contain lists, etc., allowing an arbitrarily complex, tree-like
// structure to be created, but not a cyclic graph.
//
// A record definition ('bdlmxxx::RecordDef' in the 'bdlmxxx_schema' component)
// provides a higher-level description of the structure of a 'bdem' aggregate
// than is provided by the row definition or catalog.  The record definition
// is a sequence of 'bdlmxxx::FieldDef' objects, each of which has a field name,
// (optional) field ID, field type, and other attributes of a field (within a
// list or table) or selection (within a choice or choice array).  For fields
// that describe aggregate types, the field definition may also contain a
// pointer to a record definition for that type.  For fields that describe
// enumerated values, the field definition will contain a pointer to a
// 'bdlmxxx::EnumerationDef' object, which maps enumerator name strings to integer
// enumerator IDs.  Thus, a record definition can describe the entire nested
// structure of an aggregate type and provide by-name and by-ID access to its
// fields and selections.  The classes in 'bdlmxxx_schema' along with the utility
// functions in 'bdlmxxx_schemaaggregateutil' are used to construct and validate
// 'bdem' aggregates that conform to some internal or external specification.
//
// A record definition may directly or indirectly refer to itself.  This does
// not contradict the earlier statement about an aggregate not being able to
// contain itself.  Just as a node in a tree can point to other, identically
// structured, nodes, a 'bdem' aggregate may contain other aggregates whose
// structure is described by the same record definition.  The structure of the
// aggregate is recursive, not the aggregate itself.  An aggregate with a null
// value (see "Null Values and Default Values," below) becomes a "leaf" of the
// tree-like structure.  A field definition that contains a pointer to its
// parent record definition must have the "nullable" property set, meaning
// that the field it describes may contain a null aggregate; otherwise, no
// aggregate of finite depth could conform to the record definition.
//
///The 'bdlaggxxx::Aggregate' Abstraction
///--------------------------------
// A 'bdlaggxxx::Aggregate' can hold a reference to data belonging to any of the
// 'bdem' types along with schema information describing the data: its record
// or enumeration constraint (if any), its default value, and whether or not it
// is nullable.
//
// A 'bdlaggxxx::Aggregate' exhibits pointer-like semantics with reference-like
// syntax.  The user should think of a 'bdlaggxxx::Aggregate' as pointing to
// anonymous data, rather than holding the data itself.  The copy constructor
// and assignment operator copy only the pointer, not the data.  Unlike
// pointers, however, methods called on a 'bdlaggxxx::Aggregate' object generally
// access and manipulate the referenced data (i.e., like a C++ reference),
// without the use of the pointer-dereference operators '*' and '->'.  For
// example, 'a.setValue(x)' sets the object pointed to by aggregate 'a' to a
// new value, 'x'.
//
// Although the mix of pointer-like semantics and reference-like syntax seems
// to have caused very little confusion when copying and accessing
// 'bdlaggxxx::Aggregate' objects, the same cannot be said for testing
// aggregates for equality.  Pointer-like semantics would imply that
// 'operator==' returns 'true' only if two 'bdlaggxxx::Aggregate' objects point to
// the same object, whereas the reference-like syntax would imply that
// 'operator==' returns 'true' if the referred-to objects have the same value,
// even if they are distinct objects.  For this reason, 'operator==' was
// entirely removed from this component.  Instead, the caller must explicitly
// choose one of two static methods for equality testing: 'areIdentical'
// returns 'true' only if its arguments point to the same object and
// 'areEquivalent' returns 'true' if the referred-to objects have the same
// value.  If 'areIdentical' returns 'true' for a given pair of
// 'bdlaggxxx::Aggregate' arguments, then 'areEquivalent' will also return 'true' for
// the same arguments.
//
// Several accessors permit navigating the aggregate structure.  Most
// navigation functions return a 'bdlaggxxx::Aggregate' representing a part (called
// a "sub-aggregate") of the parent aggregate.  The sub-aggregate points to a
// sub-part of its parent aggregate.  Thus, operations that modify a
// sub-aggregate will modify its parent.  (For the sake of simplicity, a
// 'bdlaggxxx::Aggregate' always points to a modifiable data structure; there is no
// 'bcem::ConstAggregate'.)
//
// The lifetimes of the data and of the schema referred to by a
// 'bdlaggxxx::Aggregate' are managed automatically through the use of reference
// counting -- the user does not need to deallocate the referenced object.
// The user also does not need to *allocate* the referenced object -- there
// are constructors that, given a schema, will automatically allocate and
// construct the referenced data using the default values stored in the
// schema.  It is also possible to construct the referenced data outside of
// the 'bdlaggxxx::Aggregate' and then bind it to a 'bdlaggxxx::Aggregate' via a
// 'bsl::shared_ptr'.
//
///Error Handling
///--------------
// The structure of a 'bdlaggxxx::Aggregate' is determined at runtime, often from
// external data (e.g., in an XML schema file).  Thus, there are many
// opportunities for errors to occur, even in code that is logically correct.
// For this reason, 'bdlaggxxx::Aggregate' is designed to behave in a predictable
// manner when confronted with detectable error conditions (listed below).
// When an error is detected, most methods will return an error in the form of
// a 'bdlaggxxx::Aggregate' in a special error state, called an "error aggregate".
// An error aggregate is a 'bdlaggxxx::Aggregate' object that has type 'VOID' and
// that contains information about the nature of the error.  The caller can
// detect the error by calling 'isError' on the returned aggregate and can
// extract an error code and descriptive string using, respectively, the
// 'errorCode' and 'errorMessage' methods.  Note that the error object
// returned by an erroneous operation is separate from the (unmodified)
// aggregate being accessed or manipulated, except that constructors report
// errors by constructing an error aggregate.
//
// Invoking most methods on an error aggregate will result in a no-op, with
// the error object returned unmodified.  Thus, client code can safely chain
// calls to multiple methods without checking the error-status in-between.
// The resulting aggregate will be in an error state and will preserve the
// error code of the first error that occurred.  This idiom preserves the use
// of syntactic sugar, both facilitating ease of use and reducing
// error-checking clutter.
//
// See 'bdlaggxxx::ErrorCode' for a list of error codes and the conditions that
// cause them.  Unless explicitly stated otherwise in the function-level
// documentation, any method that returns a 'bdlaggxxx::Aggregate' will detect these
// conditions and return the appropriate error aggregate.
//
///Extended Type Conversions
///-------------------------
// Throughout this component, the phrases "is converted" and "is convertible"
// refer to the the "extended type conversion" semantics described in the
// 'bdlmxxx_convert' component-level documentation.  Extended type conversion
// semantics can be summarized as follows:
//..
// - All normal C++ conversions apply except that converting from a 'char*' to
//   'bool' is performed by treating the pointer as a string and parsing it,
//   rather than by checking the pointer against null.
//
// - All types are convertible to 'bsl::string'.
//
// - 'bdem' scalar types are convertible from 'bsl::string' by parsing the
//   string contents.
//
// - Conversions involving a value of type 'bdlmxxx::ElemRef' or
//   'bdlmxxx::ConstElemRef' are performed by applying the previous rules to the
//   referenced value.
//..
// In addition to the above rules, a 'bdlaggxxx::Aggregate' is treated, for
// conversion purposes, as the type of value it holds.  Also, setting or
// retrieving an enumeration value as a numeric type will set or return the
// appropriate enumerator ID within the enumeration definition and setting or
// retrieving an enumeration value as a string type will set or return the
// appropriate enumerator name within the enumeration definition.
//
///Null and Default Values
///-----------------------
// Every type described in 'bdlmxxx::ElemType' has a corresponding null value.
// For scalar types, this null value comes from the 'bdltuxxx::Unset' component.
// For array types, the null value is the empty array.  For 'bdem' aggregate
// types, the null value consists of a list with zero elements, a table with
// zero rows, a choice with no current selection, or a choice array with zero
// items.  Additionally, a null unconstrained table contains no rows and no
// columns, a null unconstrained choice contains no current selection and an
// empty types catalog, and a null unconstrained choice array contains no
// items and an empty types catalog.
//
// When a new value is created (by constructing a new 'bdlaggxxx::Aggregate', by
// inserting into a table or array, by making a selection in a choice, or by
// calling 'makeValue'), it is given a default value.  In the case of scalar
// and array-of-scalar fields, the default value (which may be null) is
// specified in the schema.  In the case of lists or table rows, the default
// value is constructed by assigning each scalar element its corresponding
// default value as specified in the schema and, for each non-nullable nested
// list element, recursively constructing its default value.  At present, the
// default values for choices, tables, and choice array items are always null.
//
///Anonymous Fields
///----------------
// An anonymous field is a field within an aggregate that does not have a name
// in its corresponding schema record definition.  Anonymous fields are
// typically used for anonymous 'CHOICE' objects, especially in schemas that
// were generated from XML schema documents.  The idea of an anonymous field
// is that the currently-selected sub-field within a choice field should be
// accessible as if it were directly nested within the choice's parent
// aggregate.  An anonymous 'LIST' is less common, but the same concept
// applies: the fields within the anonymous list can be accessed as if they
// were directly within the list's parent aggregate.
//
// In most cases, 'bdlaggxxx::Aggregate' will allow transparent access to anonymous
// fields through the parent aggregate.  For example, given an aggregate, 'a',
// with the following structure:
//..
//  LIST a {
//      INT    x;
//      CHOICE <unnamed> {
//          INT    y;
//          STRING z;
//      }
//  }
//..
// The unnamed choice object within 'a' can be set to selector "y" with value
// '30' using the following statement:
//..
//  a.makeSelection("y", 30);
//..
// Note that the 'makeSelection' method behaves as though "y" is a selector
// within 'a', even though "y" is nested one level deeper and even though 'a'
// is not a choice object.  For methods that take a field name (e.g.,
// 'makeSelection', 'field', and 'setField'), 'bdlaggxxx::Aggregate' will find named
// sub-fields within an anonymous field as if they belonged to the parent
// aggregate (recursively, if an anonymous field contains an anonymous field).
// Choice-object operations, such as 'makeSelection' may be applied to
// non-choice aggregates if one or more of the anonymous fields in the chain
// leading to the named sub-field are choice objects.  Note that ambiguities
// are possible in that the same field may be found via more than one anonymous
// path or via an anonymous and non-anonymous path (e.g., if a field, 'y'
// could be directly in list 'a' and also within an anonymous choice within
// 'a').  At present, the behavior when such an ambiguity is present is well
// defined but unspecified and should not be relied on.  For efficiency
// reasons, we do not attempt to detect every ambiguity.
//
// For choice methods that access a field but do not take a field name
// (e.g., 'selector', 'selection', etc.), 'bdlaggxxx::Aggregate' will descend down
// through anonymous fields (if any) until it finds a choice object (if any),
// then it will apply the operation to that choice object.  If a choice object
// cannot be found by traversing anonymous fields, then a
// 'BCEM_NOT_A_CHOICE' error is returned.  Thus, in our example, the
// following expression will return the value "y":
//..
//  const char *s = a.selector();
//..
// Again, note that a choice-object operation is being applied to 'a', even
// though 'a' is a list object.  If there is more than one anonymous field
// directly or indirectly within the top-level aggregate, then such an access
// will be ambiguous and will result in an error.  Methods that do not take a
// field name do not attempt to disambiguate an ambiguous reference, even if
// one of the anonymous fields is not a choice object.  For example, if
// aggregate 'b' has the following structure:
//..
//  LIST b {
//      INT    x;
//      CHOICE <unnamed> {
//          INT    y;
//          STRING z;
//      }
//      LIST   <unnamed> {
//          CHAR   c;
//      }
//  }
//..
// Then the following expression will return a 'BCEM_AMBIGUOUS' error:
//..
//  b.selection();
//..
// But the following statement will still work because 'setField' takes a
// field name argument, which unambiguously identifies the field in question:
//..
//  b.setField("c", 'A');
//..
// Finally, it is possible to retrieve an anonymous field directly by using
// the 'anonymousField' methods.  With no arguments, 'anonymousField' returns
// the only anonymous field within the aggregate, or an error if there is more
// than one (or none at all).  With a numeric argument, 'anonymousField'
// returns the nth anonymous field within the aggregate.  At present, the
// 'anonymousField' methods are the only way to access anonymous scalar and
// array fields.
//
///Thread Safety
///-------------
// A 'bdlaggxxx::Aggregate' maintains a reference counted handle to possibly-shared
// data.  It is not safe to access or modify this shared data concurrently from
// different threads, therefore 'bdlaggxxx::Aggregate' is, strictly speaking,
// *thread* *unsafe* as two aggregates may refer to the same shared data.
// However, it is safe to concurrently access or modify two 'bdlaggxxx::Aggregate'
// objects refering to different data.  In addition, it is safe to destroy a
// 'bdlaggxxx::Aggregate' on one thread, while another thread is accessing or
// modified the same shared data through another 'bdlaggxxx::Aggregate' object.
//
///Usage
///-----
// In this example, we will create a schema for a postal-service address
// record, then create a 'bdlaggxxx::Aggregate' object that conforms to that schema
// and demonstrate accessing and modifying the aggregate.  The address record
// contains a street address, a city, state or province, country, postal code
// and an array of entities at that address.  Each entity can be either a
// company or a person.  A company has a name and postal account number.  A
// person has a name and birth date.  The XML schema description looks like
// this:
//..
//  <?xml version="1.0" encoding="UTF-8"?>
//  <schema xmlns="http://www.w3.org/2001/XMLSchema"
//          xmlns:bb="http://bloomberg.com/schemas/xyz"
//          targetNamespace="http://bloomberg.com/schemas/xyz"
//          elementFormDefault="qualified">
//
//      <complexType name="Company">
//          <sequence>
//              <element name="Name" type="string"/>
//              <element name="AccountNum" type="int"/>
//          </sequence>
//      </complexType>
//
//      <complexType name="Person">
//          <sequence>
//              <element name="LastName" type="string"/>
//              <element name="FirstName" type="string"/>
//              <element name="BirthDate" type="date"/>
//          </sequence>
//      </complexType>
//
//      <complexType name="Entity">
//          <choice>
//              <element name="Corp" type="bb:Company"/>
//              <element name="Human" type="bb:Person"/>
//          </choice>
//      </complexType>
//
//      <complexType name="Address">
//          <sequence>
//              <element name="StreetAddress" type="string"/>
//              <element name="City" type="string"/>
//              <element name="Province" type="string"/>
//              <element name="Country" type="string"/>
//              <element name="PostalCode" type="int"/>
//              <element name="Entities" type="bb:Entity"
//                       maxOccurs="unbounded"/>
//          </sequence>
//      </complexType>
//
//      <element name="Address" type="bb:Address"/>
//
//  </schema>
//..
// We could create the bdlmxxx::Schema object from this XML schema, but we'll do
// it manually for this example:
//..
//  bsl::shared_ptr<bdlmxxx::Schema> schema(new bdlmxxx::Schema);
//
//  bdlmxxx::RecordDef *company = schema->createRecord("Company");
//  company->appendField(bdlmxxx::ElemType::BDEM_STRING, "Name");
//  company->appendField(bdlmxxx::ElemType::BDEM_INT,    "AccountNum");
//
//  bdlmxxx::RecordDef *person = schema->createRecord("Person");
//  person->appendField(bdlmxxx::ElemType::BDEM_STRING, "LastName");
//  person->appendField(bdlmxxx::ElemType::BDEM_STRING, "FirstName");
//  person->appendField(bdlmxxx::ElemType::BDEM_DATE,   "BirthDate");
//
//  // The "entity" recordDef describes a choice
//  bdlmxxx::RecordDef *entity =
//      schema->createRecord("Entity", bdlmxxx::RecordDef::BDEM_CHOICE_RECORD);
//  entity->appendField(bdlmxxx::ElemType::BDEM_LIST, company, "Corp");
//  entity->appendField(bdlmxxx::ElemType::BDEM_LIST, person,  "Human");
//
//  bdlmxxx::RecordDef *address = schema->createRecord("Address");
//  address->appendField(bdlmxxx::ElemType::BDEM_STRING,       "StreetAddress");
//  address->appendField(bdlmxxx::ElemType::BDEM_STRING,       "City");
//  address->appendField(bdlmxxx::ElemType::BDEM_STRING,       "Province");
//  address->appendField(bdlmxxx::ElemType::BDEM_STRING,       "Country");
//  address->appendField(bdlmxxx::ElemType::BDEM_INT,          "PostalCode");
//  address->appendField(bdlmxxx::ElemType::BDEM_CHOICE_ARRAY, entity, "Entities");
//..
// Now we create and populate a simple aggregate object representing a person.
// The fields are accessed by name using the square-bracket ('[]') operator.
// 'setValue' is used to modify an aggregate's value and 'asString' is used to
// retrieve the value as a string (converting to string if necessary).
//..
//  bdlaggxxx::Aggregate michael(schema, "Person");
//  assert(michael.dataType() == bdlmxxx::ElemType::BDEM_LIST);
//  assert(michael.length() == 3);
//  michael["LastName"].setValue("Bloomberg");
//  michael["FirstName"].setValue("Michael");
//  assert("Bloomberg" == michael["LastName"].asString());
//  assert("Michael"   == michael["FirstName"].asString());
//..
// The 'field' method can be used instead of one or more '[]' operations.
// Similarly, the 'setField' method combines the effect of one or more '[]'
// operations and the 'setValue' method.  Both methods are slightly more
// efficient than using 'operator[]', especially when navigating down several
// levels.
//..
//  michael.setField("BirthDate", bdlt::Date(1942, 2, 14));
//  assert(bdlt::Date(1942, 2, 14) == michael.field("BirthDate").asDate());
//..
// Getting more sophisticated, we create and populate a schema representing an
// address.  We begin with the simple scalar fields:
//..
//  bdlaggxxx::Aggregate addr1(schema, "Address");
//  addr1.setField("StreetAddress", "499 Park Ave.");  // Before the move
//  addr1.setField("City", "New York");
//  addr1.setField("Province", "NY");
//  addr1.setField("Country", "USA");
//  addr1.setField("PostalCode", 10022);
//..
// A 'bdlaggxxx::Aggregate' object does not actually hold data.  Rather it holds a
// smart reference to some other anonymous data, similar to the way a pointer
// holds the address of anonymous data, but without the need for dereferencing
// operators ('*' or '->').  A 'bdlaggxxx::Aggregate' can also hold a reference to a
// sub-part of the data referenced by another 'bdlaggxxx::Aggregate'.  To simplify
// access to the "Entities" choice array, retrieve the empty array from
// 'addr1' and keep a reference to this sub-aggregate in another
// 'bdlaggxxx::Aggregate' object.  'bdlaggxxx::Aggregate' uses reference counting so that
// the sub-aggregate will remain valid even if the original aggregate goes out
// of scope:
//..
//  bdlaggxxx::Aggregate entities = addr1.field("Entities");
//  entities.appendItems(2);
//  assert(2 == addr1.field("Entities").length());
//..
// We set the first entity to be a company, and we set its name:
//..
//  entities[0].makeSelection("Corp").setField("Name", "Bloomberg LP");
//  assert(0 == bsl::strcmp("Corp", entities[0].selector()));
//..
// We set the second entity to be a person and set its value from the
// 'michael' aggregate.  Note that the copy of 'michael' stored in the choice
// item is independent of the data referred to by 'michael'; 'setField',
// 'setValue', 'makeSelection', and other methods have value semantics and
// their operation should not be confused with assignment.
//..
//  entities[1].makeSelection("Human", michael);
//  assert(0 == bsl::strcmp("Human", entities[1].selector()));
//..
// Array items or table rows can be accessed by using the '[]' operator or by
// supplying an integer argument to the 'field' function.  The current
// selection in a choice can be accessed using the 'selection()' method, by
// supplying the selector name to the 'field' method or the '[]', or by
// supplying the empty string ("") to the 'field' method or '[]' operator.
// The 'field' method provides convenient access to data up to 10 levels deep
// in nested aggregates.  All data types can be accessed as strings using the
// 'asString' function:
//..
//  bsl::string birthday = addr1.field("Entities")[1].selection().
//                               field("BirthDate").asString();
//  bsl::string bb = addr1.field("Entities", 0, "Corp", "Name").asString();
//  assert("Bloomberg LP" == bb);
//  assert("1942-02-14"   == birthday);
//..
// The 'setValue' and 'setField' methods as well the 'asString', 'asInt',
// 'asDate', and similar methods automatically convert between the source and
// target types.  This is especially useful for converting between string and
// non-string types, although numeric and other conversions are also
// performed.  The behavior is undefined if an impossible conversion is
// attempted.
//..
//  assert(10022 == addr1["PostalCode"].asInt());      // Retrieve int
//  assert("10022" == addr1["PostalCode"].asString()); // Convert to string
//  addr1["PostalCode"].setValue("10023");             // Convert from string
//  assert(10023 == addr1["PostalCode"].asInt());      // Retrieve int
//  addr1["City"].setValue(101);                       // Convert from int
//  assert("101" == addr1["City"].asString());         // Retrieve string
//  assert(101 == addr1["City"].asInt());              // Convert to int
//  addr1["City"].setValue("New York");                // Set string
//..
// Copying an aggregate using assignment or copy construction results in the
// copy referring to the same data as the original.  'areIdentical' returns
// 'true' if and only if one aggregate references the same data as the other.
//..
//  bdlaggxxx::Aggregate addr2 = addr1;
//  assert(bdlaggxxx::Aggregate::areIdentical(addr1, addr2));
//..
// To create a new aggregate with an independent copy of the data instead of a
// second reference to the same data, use the 'clone' function.  Just as two
// pointers that point to different but equal objects will not compare equal,
// the original 'bdlaggxxx::Aggregate' and its clone are equivalent, but will not
// compare equal.  To compare the referenced data, use the 'areEquivalent'
// class methods:
//..
//  addr2 = addr1.clone();
//  assert(!bdlaggxxx::Aggregate::areIdentical(addr1, addr2));  // Not identical...
//  assert(bdlaggxxx::Aggregate::areEquivalent(addr1, addr2));  // but equivalent
//  assert(&addr1.recordDef() != &addr2.recordDef()); // Different schema addr
//..
// The clone can be shown to be independent by changing a field, thus breaking
// the equivalence relationship.
//..
//  addr2.setField("StreetAddress", "731 Lexington Ave.");  // After the move
//  assert(! bdlaggxxx::Aggregate::areEquivalent(addr1, addr2));
//  assert("499 Park Ave."      == addr1.field("StreetAddress").asString());
//  assert("731 Lexington Ave." == addr2.field("StreetAddress").asString());
//..
// Changing the schema after creating an aggregate using that schema produces
// undefined behavior.  The schema can thus be considered constant and it is
// rarely necessary to clone the schema.  It is thus more efficient to clone
// the data only, using the 'cloneData' method:
//..
//  addr2 = addr1.cloneData();
//  assert(addr1 != addr2);                               // Not equal...
//  assert(bdlaggxxx::Aggregate::areEquivalent(addr1, addr2));  // but equivalent
//  assert(&addr1.recordDef() == &addr2.recordDef()); // exactly same schema
//..
// It is important to note that the assignment operator always replaces both
// the schema and data of a 'bdlaggxxx::Aggregate' so that both its structure and
// value may change.  It is thus not possible to modify a sub-aggregate via
// assignment:
//..
//  addr1.setField("Entities", 1, "", "FirstName", "Mike");
//  bdlaggxxx::Aggregate mike = addr1.field("Entities", 1).selection();
//  // 'mike' is a copy of 'addr1["Entities"][1]["Human"].
//  // It's structure is defined by the 'person' record definition.
//  assert("Mike" == mike.field("FirstName").asString());
//
//  // Assign 'mike' to refer to different data:
//  mike = michael["FirstName"];
//  // 'mike' is a copy of 'michael["FirstName"]' and contains a scalar string.
//  assert(mike.asString() == "Michael");
//
//  // "FirstName" field within the (former) parent aggregate is unchanged.
//  assert("Mike" == addr1.field("Entities", 1, "", "FirstName").asString());
//..
// Similarly, you cannot modify a sub-aggregate through an rvalue expression:
//..
//  bdlaggxxx::Aggregate x(bdlmxxx::ElemType::BDEM_STRING, "Michael");
//  addr1["Entities"][1][""]["FirstName"] = x;  // Won't work
//..
// In fact, methods that return sub-aggregates return them as 'const' to
// ensure that the previous example will not compile.  To modify a
// sub-aggregate, use 'setValue' (or one of the other manipulators):
//..
//  mike = addr1.field("Entities", 1).selection();
//  // 'mike' is another reference to 'addr1["Entities"][1]["Human"]
//  assert("Mike" == mike.field("FirstName").asString());
//..
// The following would be an error because 'michael["FirstName"]' does not
// have the same structure (dynamic type) as 'mike':
//..
//  bdlaggxxx::Aggregate result = mike.setValue(michael["FirstName"]);
//  assert(result.isError());
//  assert(result.errorCode() == bdlaggxxx::ErrorCode::BCEM_NON_CONFORMING);
//  bsl::cout << result.errorMessage();
//..
// Modify the data that 'mike' references using 'setValue' with data having
// compatible structure.
//..
//  result = mike.setValue(michael);
//  assert(! result.isError());
//  assert(bdlaggxxx::Aggregate::areEquivalent(mike, michael));
//
//  // "FirstName" field within the parent aggregate has been changed through
//  // the sub-aggregate:
//  assert("Michael"==addr1.field("Entities", 1, "", "FirstName").asString());
//..
// Error handling is designed so that a chain of operations will produce error
// aggregate that encodes the cause of the first failure in the chain.  For
// example, if we attempt to access a table field using an invalid index, then
// go on to try to access a field within the table using an invalid field
// name, the resulting error object reports that an array-index error occurred:
//..
//  result = addr1["Entities"][40]["Human"]["FirstName"];
//  assert(result.isError());
//  assert(result.errorCode() == bdlaggxxx::ErrorCode::BCEM_BAD_ARRAYINDEX);
//  bsl::cout << result.errorMessage();
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLAGGXXX_AGGREGATERAW
#include <bdlaggxxx_aggregateraw.h>
#endif

#ifndef INCLUDED_BDLAGGXXX_ERRORATTRIBUTES
#include <bdlaggxxx_errorattributes.h>
#endif

#ifndef INCLUDED_BDLAGGXXX_ERRORCODE
#include <bdlaggxxx_errorcode.h>
#endif

#ifndef INCLUDED_BDLAGGXXX_FIELDSELECTOR
#include <bdlaggxxx_fieldselector.h>
#endif

#ifndef INCLUDED_BDLAT_TYPECATEGORY
#include <bdlat_typecategory.h>
#endif

#ifndef INCLUDED_BDLMXXX_CHOICE
#include <bdlmxxx_choice.h>
#endif

#ifndef INCLUDED_BDLMXXX_CHOICEARRAY
#include <bdlmxxx_choicearray.h>
#endif

#ifndef INCLUDED_BDLMXXX_CONVERT
#include <bdlmxxx_convert.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMATTRLOOKUP
#include <bdlmxxx_elemattrlookup.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMREF
#include <bdlmxxx_elemref.h>
#endif

#ifndef INCLUDED_BDLMXXX_LIST
#include <bdlmxxx_list.h>
#endif

#ifndef INCLUDED_BDLMXXX_ROW
#include <bdlmxxx_row.h>
#endif

#ifndef INCLUDED_BDLMXXX_SCHEMA
#include <bdlmxxx_schema.h>
#endif

#ifndef INCLUDED_BDLMXXX_SCHEMAUTIL
#include <bdlmxxx_schemautil.h>
#endif

#ifndef INCLUDED_BDLMXXX_SELECTBDEMTYPE
#include <bdlmxxx_selectbdemtype.h>
#endif

#ifndef INCLUDED_BDLMXXX_TABLE
#include <bdlmxxx_table.h>
#endif

#ifndef INCLUDED_BDLTUXXX_UNSET
#include <bdltuxxx_unset.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#include <bdlb_nullablevalue.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_SHAREDPTRREP
#include <bslma_sharedptrrep.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ANNOTATION
#include <bsls_annotation.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>       // for 'bsl::swap'
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>   // for 'bsl::pair'
#endif

namespace BloombergLP {

namespace bdlaggxxx {
typedef FieldSelector Aggregate_NameOrIndex;
    // Alias for a field selector.
    //
    // DEPRECATED: Use 'FieldSelector' instead.

class Aggregate {
    // This class provides a reference to a fully-introspective data
    // structure capable of representing any of the following types of bdem
    // data:
    //..
    //  - A single scalar value (integer, string, date, enumeration, etc.)
    //  - A heterogeneous list (sequence) of values (with optional field names)
    //  - A choice value containing a single selection from a list of possible
    //    types (where each possible selection may have a field name).
    //  - An array of homogeneous scalar values
    //  - A table of homogeneous list values (rows)
    //  - An array of homogeneous choice values (choice array items)
    //..

    // DATA
    AggregateRaw   d_aggregateRaw;                  // aggregate
                                                         // representation
                                                         // without reference
                                                         // counting
    bslma::SharedPtrRep *d_schemaRep_p;                  // shared schema
    bslma::SharedPtrRep *d_valueRep_p;                   // pointer to data
    bslma::SharedPtrRep *d_isTopLevelAggregateNullRep_p; // nullness indicator
                                                         // for top-level
                                                         // aggregate in bit 0

    // PRIVATE MANIPULATORS
    const Aggregate fieldImp(
              bool               makeNonNullFlag,
              FieldSelector fieldSelector1,
              FieldSelector fieldSelector2  = FieldSelector(),
              FieldSelector fieldSelector3  = FieldSelector(),
              FieldSelector fieldSelector4  = FieldSelector(),
              FieldSelector fieldSelector5  = FieldSelector(),
              FieldSelector fieldSelector6  = FieldSelector(),
              FieldSelector fieldSelector7  = FieldSelector(),
              FieldSelector fieldSelector8  = FieldSelector(),
              FieldSelector fieldSelector9  = FieldSelector(),
              FieldSelector fieldSelector10 = FieldSelector()) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of two to ten 'fieldSelector' arguments, each of
        // which specifies a field name or array index.  If this aggregate
        // references a nillable array, the specified field is null, and the
        // specified 'makeNonNullFlag' is 'true', assign that field its default
        // value; otherwise leave the field unmodified.  Return a sub-aggregate
        // referring to the modifiable field on success or an error object on
        // failure (as described in the "Error Handling" section of the
        // 'bdlaggxxx_aggregate' component-level documentation).  An empty string
        // can be used for any of the 'fieldSelector' arguments to specify the
        // current selection within a choice object.  An unused argument
        // results in the construction of a null 'FieldSelector', which is
        // treated as the end of the argument list.

    void init(const bsl::shared_ptr<const bdlmxxx::Schema>&     schemaPtr,
              const bdlmxxx::RecordDef                         *recordDefPtr,
              bdlmxxx::ElemType::Type                           elemType,
              bslma::Allocator                             *basicAllocator);
    void init(const bsl::shared_ptr<const bdlmxxx::RecordDef>&  recordDefPtr,
              bdlmxxx::ElemType::Type                           elemType,
              bslma::Allocator                             *basicAllocator);
    void init(const bsl::shared_ptr<const bdlmxxx::Schema>&     schemaPtr,
              const char                                   *recordName,
              bdlmxxx::ElemType::Type                           elemType,
              bslma::Allocator                             *basicAllocator);
        // Initialize this object from the specified record definition and
        // element type.  By default, the constructed object is not null.
        // The behavior is undefined if this method is invoked on an
        // already-initialized Aggregate.

    // PRIVATE ACCESSORS
    const Aggregate makeError(
                           const ErrorAttributes& errorDescription) const;
    const Aggregate makeError(ErrorCode::Code  errorCode,
                                   const char           *msg, ...) const
        // Declare this function as printf-like in gcc.
        // The 'msg' arg is the 3rd arg, including the implicit 'this'.
        BSLS_ANNOTATION_PRINTF(3,4)
        ;
        // Generate an error object with the specified 'errorCode' and the
        // specified 'printf'-style 'msg'.  If this object is an error
        // aggregate ('isError()' returns 'true'), do nothing but return this
        // object.  The behavior when this object is an error object prevents
        // an error message from being overridden by a subsequent error in the
        // same expression.  For example, given a typical method-call chain,
        // 'agg.func1().func2()', If 'func1' returns an error and 'func2' uses
        // 'makeError' to generate another error, then the effect will be the
        // same as if the call chain stopped after 'func1'.  Note that this is
        // a 'const' function -- it returns a new error aggregate and does not
        // put this object into an error state.  Note also that the generated
        // error aggregate allocates its error record using the default
        // allocator.

    // PRIVATE CLASS METHODS
    static bsl::shared_ptr<void> makeValuePtr(
                                    bdlmxxx::ElemType::Type  type,
                                    bslma::Allocator    *basicAllocator = 0);
        // Return a shared pointer to a newly-created unset value of the
        // specified 'type'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    template <class TYPE>
    static const TYPE&  valueRef(const TYPE& value);
    static bdlmxxx::ElemRef valueRef(const Aggregate& value);
        // Return the specified 'value' if 'value' is not a 'Aggregate'
        // and 'value.asElemRef()' otherwise.  Note that this method
        // facilitates passing values to the 'AggregateRaw' template
        // methods for assignment.

    // PRIVATE CREATORS
    Aggregate(const AggregateRaw&  aggregateRaw,
                   bslma::SharedPtrRep      *schemaRep,
                   bslma::SharedPtrRep      *valueRep,
                   bslma::SharedPtrRep      *isTopLevelAggregateNullRep);
       // Create a Aggregate holding a counted reference to the same
       // schema and data as the specified 'aggregateRaw' object.  Increment
       // and maintain the refererence count using the specified 'schemaRep'
       // for the shared schema reference, the specified 'valueRep' for the
       // shared data reference, and the specified 'isTopLevelAggregateNullRep'
       // for the top-level nullness bit.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(Aggregate,
                                  bslalg::TypeTraitBitwiseMoveable,
                                  bdlb::TypeTraitHasPrintMethod);
        // Note that this class does not have the
        // 'bslalg::TypeTraitUsesBslmaAllocator' trait.  Although some
        // constructors do take an allocator, the pointer-like semantics do not
        // fully allow the use of the allocator idioms.

    // TYPES
    enum {
        // Navigation status codes when descending into a field or array item.
        // Values are large negative integers not equal to 'INT_MIN' so as not
        // to be confused with valid return values such as -1 or
        // 'bdet::Null<int>::unsetValue()'.

        // DEPRECATED: Use 'ErrorCode' instead.

        BCEM_ERR_UNKNOWN_ERROR  = ErrorCode::BCEM_UNKNOWN_ERROR,
        BCEM_ERR_NOT_A_RECORD   = ErrorCode::BCEM_NOT_A_RECORD,
        BCEM_ERR_NOT_A_SEQUENCE = ErrorCode::BCEM_NOT_A_SEQUENCE,
        BCEM_ERR_NOT_A_CHOICE   = ErrorCode::BCEM_NOT_A_CHOICE,
        BCEM_ERR_NOT_AN_ARRAY   = ErrorCode::BCEM_NOT_AN_ARRAY,
        BCEM_ERR_BAD_FIELDNAME  = ErrorCode::BCEM_BAD_FIELDNAME,
        BCEM_ERR_BAD_FIELDID    = ErrorCode::BCEM_BAD_FIELDID,
        BCEM_ERR_BAD_FIELDINDEX = ErrorCode::BCEM_BAD_FIELDINDEX,
        BCEM_ERR_BAD_ARRAYINDEX = ErrorCode::BCEM_BAD_ARRAYINDEX,
        BCEM_ERR_NOT_SELECTED   = ErrorCode::BCEM_NOT_SELECTED,
        BCEM_ERR_BAD_CONVERSION = ErrorCode::BCEM_BAD_CONVERSION,
        BCEM_ERR_BAD_ENUMVALUE  = ErrorCode::BCEM_BAD_ENUMVALUE,
        BCEM_ERR_NON_CONFORMANT = ErrorCode::BCEM_NON_CONFORMANT,
        BCEM_ERR_AMBIGUOUS_ANON = ErrorCode::BCEM_AMBIGUOUS_ANON
    };

    // CLASS METHODS
    static
    bool areIdentical(const Aggregate& lhs, const Aggregate& rhs);
        // Return 'true' if the specified 'lhs' aggregate refers to the same
        // object as the specified 'rhs' aggregate and 'false' otherwise.
        // Specifically, 'lhs' and 'rhs' are considered identical if 'dataType'
        // and 'dataPtr' return equal values for the respective arguments.
        // When two 'Aggregate' objects compare identical, modifications
        // to one of their referred-to object will be visible through the
        // other.  This predicate is roughly equivalent to comparing two
        // pointers for equality.  Note that if
        // 'bdlmxxx::ElemType::BDEM_VOID == lhs.dataType()' or
        // 'bdlmxxx::ElemType::BDEM_VOID == rhs.dataType()', then 'false' is
        // returned.

    static
    bool areEquivalent(const Aggregate& lhs, const Aggregate& rhs);
        // Return 'true' if the value of the object referenced by the specified
        // 'lhs' aggregate equals the value of the object referenced by the
        // specified 'rhs' aggregate.  The aggregates need not be identical (as
        // per 'areIdentical'), but must have equivalent record definitions and
        // equal data and nullness values.  This predicate is roughly
        // equivalent to dereferencing two pointers and comparing their
        // pointed-to values for equality.  'areIdentical(lhs, rhs)' implies
        // 'areEquivalent(lhs, rhs)', but not vice versa.

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported
        // by this class.  See the BDE package-group-level documentation for
        // more information on 'bdex' streaming of container types.

    // CREATORS
    Aggregate();
        // Create an empty aggregate with no schema or data.  Note that after
        // construction 'isNul2()' returns 'true'.

    template <class VALTYPE>
    Aggregate(const bdlmxxx::ElemType::Type  dataType,
                   const VALTYPE&             value,
                   bslma::Allocator          *basicAllocator = 0);
        // Create an aggregate representing a value of the type specified in
        // 'dataType' and initialize it to the specified 'value'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  If 'dataType' is a 'bdem' aggregate type, then the
        // constructed object will be unconstrained, i.e., it will have no
        // record definition and therefore no by-name or by-ID field access.
        // The constructed aggregate will be in an error state if 'value' is
        // not convertible to the type specified by 'dataType'.  (See "Extended
        // Type Conversions" and "Error Handling" in the 'Aggregate'
        // component-level documentation for a detailed definition of "is
        // convertible".)  Note that after construction, 'isNul2()' returns
        // 'false'.

    // SHARED-POINTER CONSTRUCTORS
    // The following constructors initialize the aggregate's structure and
    // data by sharing references to the arguments provided by the client.  If
    // a schema pointer or record definition pointer is provided without value
    // data, then only the schema is shared with the client; the value data is
    // allocated and given a default non-null value.  (See "Null Values and
    // Default Values" in the 'bdlaggxxx_aggregate' component-level documentation
    // for a more complete description of default values.)

    explicit Aggregate(
        const bsl::shared_ptr<const bdlmxxx::RecordDef>&  recordDefPtr,
        bslma::Allocator                             *basicAllocator = 0);
    explicit Aggregate(
        const bsl::shared_ptr<bdlmxxx::RecordDef>&        recordDefPtr,
        bslma::Allocator                             *basicAllocator = 0);
        // Create an aggregate containing a list or choice object structured
        // according to the record definition pointed to by the specified
        // 'recordDefPtr'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  If
        // 'BDEM_SEQUENCE_RECORD == recordDefPtr->recordType()', create a
        // sequence (list) of fields, each initialized with its default value
        // (recursively for constrained list elements).  If
        // 'BDEM_CHOICE_RECORD == recordDefPtr->recordType()', create a choice
        // aggregate with no current selection.  (See "Null Values and Default
        // Values" in the 'Aggregate' component-level documentation for a
        // more complete description of default values.)  The new aggregate
        // retains a reference to the shared record definition.  The behavior
        // is undefined if any part of the record definition's parent schema is
        // modified during the lifetime of this aggregate.  Note that after
        // construction, 'isNul2()' returns 'false'.

    Aggregate(
        const bsl::shared_ptr<const bdlmxxx::RecordDef>&  recordDefPtr,
        bdlmxxx::ElemType::Type                           elemType,
        bslma::Allocator                             *basicAllocator = 0);
    Aggregate(
        const bsl::shared_ptr<bdlmxxx::RecordDef>&        recordDefPtr,
        bdlmxxx::ElemType::Type                           elemType,
        bslma::Allocator                             *basicAllocator = 0);
        // Create an aggregate containing a list, table, choice, or
        // choice-array object structured according to the record definition
        // specified by 'recordDefPtr'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  The new aggregate retains a
        // reference to the shared record definition.  If 'elemType' is 'LIST',
        // initialize each element of the list according to its default value
        // (recursively for constrained lists and choices).  If 'elemType' is
        // 'CHOICE', initialize the choice object to have no current selection.
        // If 'elemType' is 'TABLE' or 'CHOICE_ARRAY', initialize the object to
        // have no rows or items.  (See "Null Values and Default Values" in the
        // 'Aggregate' component-level documentation for a more complete
        // description of default values.)  The constructed aggregate will be
        // in an error state (see "Error Handling" in the 'Aggregate'
        // component-level documentation) unless 'elemType' is 'LIST', 'TABLE',
        // 'CHOICE', or 'CHOICE_ARRAY' and 'recordDefPtr->recordType()' is
        // 'BDEM_SEQUENCE_RECORD' for an 'elemType' of 'LIST' or 'TABLE' and
        // 'recordDefPtr->recordType()' is 'BDEM_CHOICE_RECORD' for an
        // 'elemType' of 'CHOICE' or 'CHOICE_ARRAY'.  The behavior is undefined
        // if any part of the record definition's parent schema is modified
        // during the lifetime of this aggregate.  Note that after
        // construction, 'isNul2()' returns 'false'.

    Aggregate(
 const bsl::shared_ptr<const bdlmxxx::Schema>& schemaPtr,
 const bsl::string&                        recordName,
 bdlmxxx::ElemType::Type                       elemType = bdlmxxx::ElemType::BDEM_VOID,
 bslma::Allocator                         *basicAllocator = 0);
    Aggregate(
   const bsl::shared_ptr<bdlmxxx::Schema>&     schemaPtr,
   const bsl::string&                      recordName,
   bdlmxxx::ElemType::Type                     elemType = bdlmxxx::ElemType::BDEM_VOID,
   bslma::Allocator                       *basicAllocator = 0);
        // Create an aggregate containing a list, table, choice, or
        // choice-array object structured according to the record definition
        // identified by the specified 'recordName' within the schema pointed
        // to by specified 'schemaPtr'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  The new aggregate retains a
        // reference to the shared record definition.  If the specified
        // 'elemType' is 'VOID', then the element type is set to 'LIST' or
        // 'CHOICE', depending on whether the referenced record definition is a
        // 'BDEM_SEQUENCE_RECORD' or 'BDEM_CHOICE_RECORD', respectively.  If
        // the element type is 'LIST', each element of the list is initialized
        // according to its default value (recursively for constrained lists
        // and choices).  If the element type is 'CHOICE', initialize the
        // choice object to have no current selection.  If the element type is
        // 'TABLE' or 'CHOICE_ARRAY', initialize the object to have no rows or
        // items.  (See "Null Values and Default Values" in the
        // 'bdlaggxxx_aggregate' component-level documentation for a more complete
        // description of default values.)  The constructed aggregate will be
        // in an error state (see "Error Handling" in the 'Aggregate'
        // component-level documentation) unless 'elemType' is 'VOID', 'LIST',
        // 'TABLE', 'CHOICE', or 'CHOICE_ARRAY' and
        // 'schemaPtr->lookupRecord(recordName)->recordType()' is
        // 'BDEM_SEQUENCE_RECORD' for an element type of 'LIST' or 'TABLE' and
        // 'schemaPtr->lookupRecord(recordName)->recordType()' is
        // 'BDEM_CHOICE_RECORD' for an element type of 'CHOICE' or
        // 'CHOICE_ARRAY'.  The behavior is undefined if the schema is modified
        // during the lifetime of this aggregate.  Note that after
        // construction, 'isNul2()' returns 'false'.

    Aggregate(const Aggregate& original);
        // Create a new aggregate that refers to the same data and schema
        // as the specified 'original' aggregate.  This creates a new reference
        // to existing data -- no data is copied.  After the construction,
        // 'areIdentical(*this, original)' will return 'true'.  Note that,
        // unlike most BDE types, the allocator for this aggregate will change
        // to be the same as the allocator for 'other'.  Note that
        // copy-constructing an error aggregate will yield an identical error
        // aggregate.

    ~Aggregate();
        // Destroy this object.  If this is the last aggregate referring to a
        // schema, then the schema is destroyed and deallocated.  If this is
        // the last aggregate referring to a data object, then the data object
        // is also destroyed and deallocated.

    // MANIPULATORS
    Aggregate& operator=(const Aggregate& rhs);
        // Make this aggregate refer to the same data and schema as the
        // specified 'rhs' aggregate and return a reference providing
        // modifiable access to this aggregate.  This creates a new reference
        // to existing data -- no data is copied.  The reference counts on the
        // previously-referenced data and schema are decremented, possibly
        // causing either or both to be destroyed and deallocated.  After the
        // assignment, 'areIdentical(*this, rhs)' will return 'true'.  Also
        // note that, unlike most BDE types, the allocator for this aggregate
        // will change to be the same as the allocator for 'rhs'.  Note that if
        // 'rhs' is an error aggregate, then this aggregate will be assigned
        // the same error state.

    AggregateRaw& aggregateRaw();
        // Return a reference providing modifiable access to the
        // non-reference-counted portion of this aggregate.

    const Aggregate reserveRaw(bsl::size_t numItems);
        // Reserve sufficient memory for at least the specified 'numItems' if
        // this aggregate references a scalar or choice array, or reserve
        // sufficient memory for at least the footprint of 'numItems' rows, if
        // this aggregate references a table.  In the latter case, additional
        // memory needed to initialize a new row upon insertion, *may* or may
        // *not* be reserved depending on the allocation mode.  In the future,
        // this method may strengthen its guarantee such that no additional
        // allocation will occur upon row insertion (regardless of allocation
        // mode) unless a data element itself allocates memory.  Return the
        // value of this aggregate on success or an error aggregate otherwise.

    const Aggregate& reset();
        // Reset this object to the void aggregate ('BDEM_VOID' data type, no
        // schema) and release references to schema and data held.  Return a
        // reference providing non-modifiable access to the changed object.
        // Note that this aggregate will never be in an error state after a
        // call to 'reset'.  Also note that after the function call, 'isNul2()'
        // returns 'true'.

    // REFERENCED-VALUE MANIPULATORS

    // The following set of methods are technically accessors in that they
    // do not modify the 'Aggregate' on which they are called.  However,
    // they do manipulate the referenced value.

    bsl::shared_ptr<void> dataPtr() const;
        // Return a shared pointer to the value referenced by this aggregate.
        // The behavior is undefined if the returned pointer is used to modify
        // the data in such a way that it no longer conforms to the schema or
        // if the aggregate is currently null.  The result of calling
        // 'dataPtr' on an error aggregate is unspecified.

    const Aggregate& makeNull() const;
        // Set the object referenced by this aggregate to null and return a
        // reference to this aggregate.  If the object is null before the
        // call, then do nothing.  The object's type and constraints are
        // unchanged.

    template <class VALTYPE>
    const Aggregate setValue(const VALTYPE& value) const;
        // Set the value referenced by this aggregate to the specified
        // 'value', resetting its nullness flag if 'isNul2()' is 'true',
        // converting 'value' as necessary (see "Extended Type Conversions" in
        // the 'Aggregate' component-level documentation for a detailed
        // definition of "is convertible") and return this aggregate.  If
        // value is null then make this aggregate null.  Leave this
        // aggregate unchanged and return an error if 'value' is not
        // convertible to the type stored in this aggregate.  The schema
        // (dynamic type) of this aggregate is not changed.  Return this
        // aggregate unmodified if this aggregate was in an error state before
        // calling 'setValue'.

    template <class VALTYPE>
    const Aggregate setField(FieldSelector fieldSelector1,
                                  const VALTYPE&     value) const;
    template <class VALTYPE>
    const Aggregate setField(FieldSelector fieldSelector1,
                                  FieldSelector fieldSelector2,
                                  const VALTYPE&     value) const;
    template <class VALTYPE>
    const Aggregate setField(FieldSelector fieldSelector1,
                                  FieldSelector fieldSelector2,
                                  FieldSelector fieldSelector3,
                                  const VALTYPE&     value) const;
    template <class VALTYPE>
    const Aggregate setField(FieldSelector fieldSelector1,
                                  FieldSelector fieldSelector2,
                                  FieldSelector fieldSelector3,
                                  FieldSelector fieldSelector4,
                                  const VALTYPE&     value) const;
    template <class VALTYPE>
    const Aggregate setField(FieldSelector fieldSelector1,
                                  FieldSelector fieldSelector2,
                                  FieldSelector fieldSelector3,
                                  FieldSelector fieldSelector4,
                                  FieldSelector fieldSelector5,
                                  const VALTYPE&     value) const;
    template <class VALTYPE>
    const Aggregate setField(FieldSelector fieldSelector1,
                                  FieldSelector fieldSelector2,
                                  FieldSelector fieldSelector3,
                                  FieldSelector fieldSelector4,
                                  FieldSelector fieldSelector5,
                                  FieldSelector fieldSelector6,
                                  const VALTYPE&     value) const;
    template <class VALTYPE>
    const Aggregate setField(FieldSelector fieldSelector1,
                                  FieldSelector fieldSelector2,
                                  FieldSelector fieldSelector3,
                                  FieldSelector fieldSelector4,
                                  FieldSelector fieldSelector5,
                                  FieldSelector fieldSelector6,
                                  FieldSelector fieldSelector7,
                                  const VALTYPE&     value) const;
    template <class VALTYPE>
    const Aggregate setField(FieldSelector fieldSelector1,
                                  FieldSelector fieldSelector2,
                                  FieldSelector fieldSelector3,
                                  FieldSelector fieldSelector4,
                                  FieldSelector fieldSelector5,
                                  FieldSelector fieldSelector6,
                                  FieldSelector fieldSelector7,
                                  FieldSelector fieldSelector8,
                                  const VALTYPE&     value) const;
    template <class VALTYPE>
    const Aggregate setField(FieldSelector fieldSelector1,
                                  FieldSelector fieldSelector2,
                                  FieldSelector fieldSelector3,
                                  FieldSelector fieldSelector4,
                                  FieldSelector fieldSelector5,
                                  FieldSelector fieldSelector6,
                                  FieldSelector fieldSelector7,
                                  FieldSelector fieldSelector8,
                                  FieldSelector fieldSelector9,
                                  const VALTYPE&     value) const;
    template <class VALTYPE>
    const Aggregate setField(FieldSelector fieldSelector1,
                                  FieldSelector fieldSelector2,
                                  FieldSelector fieldSelector3,
                                  FieldSelector fieldSelector4,
                                  FieldSelector fieldSelector5,
                                  FieldSelector fieldSelector6,
                                  FieldSelector fieldSelector7,
                                  FieldSelector fieldSelector8,
                                  FieldSelector fieldSelector9,
                                  FieldSelector fieldSelector10,
                                  const VALTYPE&     value) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of one to ten 'fieldSelector' arguments, each of
        // which specifies a field name or array index, then set that field to
        // the specified 'value', resetting its nullness flag if
        // 'field.isNul2()' is 'true', after appropriate conversions (see
        // "Extended Type Conversions" in the 'Aggregate' component-level
        // documentation).  Return a sub-aggregate referring to the modified
        // field on success or an error object on failure (as described in the
        // "Error Handling" section of the 'bdlaggxxx_aggregate' component-level
        // documentation).  An empty string can be used for any of the
        // 'fieldSelector' arguments to specify the current selection within a
        // choice object.  If value is null then make the field null.  Note
        // that if any field in the chain of fields is null then an error is
        // returned.  This aggregate is not modified if an error is detected.

    const Aggregate setFieldNull(FieldSelector fieldSelector1) const;
    const Aggregate setFieldNull(FieldSelector fieldSelector1,
                                      FieldSelector fieldSelector2) const;
    const Aggregate setFieldNull(FieldSelector fieldSelector1,
                                      FieldSelector fieldSelector2,
                                      FieldSelector fieldSelector3) const;
    const Aggregate setFieldNull(FieldSelector fieldSelector1,
                                      FieldSelector fieldSelector2,
                                      FieldSelector fieldSelector3,
                                      FieldSelector fieldSelector4) const;
    const Aggregate setFieldNull(FieldSelector fieldSelector1,
                                      FieldSelector fieldSelector2,
                                      FieldSelector fieldSelector3,
                                      FieldSelector fieldSelector4,
                                      FieldSelector fieldSelector5) const;
    const Aggregate setFieldNull(FieldSelector fieldSelector1,
                                      FieldSelector fieldSelector2,
                                      FieldSelector fieldSelector3,
                                      FieldSelector fieldSelector4,
                                      FieldSelector fieldSelector5,
                                      FieldSelector fieldSelector6) const;
    const Aggregate setFieldNull(FieldSelector fieldSelector1,
                                      FieldSelector fieldSelector2,
                                      FieldSelector fieldSelector3,
                                      FieldSelector fieldSelector4,
                                      FieldSelector fieldSelector5,
                                      FieldSelector fieldSelector6,
                                      FieldSelector fieldSelector7) const;
    const Aggregate setFieldNull(FieldSelector fieldSelector1,
                                      FieldSelector fieldSelector2,
                                      FieldSelector fieldSelector3,
                                      FieldSelector fieldSelector4,
                                      FieldSelector fieldSelector5,
                                      FieldSelector fieldSelector6,
                                      FieldSelector fieldSelector7,
                                      FieldSelector fieldSelector8) const;
    const Aggregate setFieldNull(FieldSelector fieldSelector1,
                                      FieldSelector fieldSelector2,
                                      FieldSelector fieldSelector3,
                                      FieldSelector fieldSelector4,
                                      FieldSelector fieldSelector5,
                                      FieldSelector fieldSelector6,
                                      FieldSelector fieldSelector7,
                                      FieldSelector fieldSelector8,
                                      FieldSelector fieldSelector9) const;
    const Aggregate setFieldNull(
                                     FieldSelector fieldSelector1,
                                     FieldSelector fieldSelector2,
                                     FieldSelector fieldSelector3,
                                     FieldSelector fieldSelector4,
                                     FieldSelector fieldSelector5,
                                     FieldSelector fieldSelector6,
                                     FieldSelector fieldSelector7,
                                     FieldSelector fieldSelector8,
                                     FieldSelector fieldSelector9,
                                     FieldSelector fieldSelector10) const;
        // Set the field within this aggregate reached through the specified
        // chain of one to ten 'fieldSelector' arguments, each of which
        // specifies a field name or array index, to null.  Return a
        // sub-aggregate referring to the field on success or an error object
        // on failure (as described in the "Error Handling" section of the
        // 'bdlaggxxx_aggregate' component-level documentation).  An empty string
        // can be used for any of the 'fieldSelector' arguments to specify the
        // current selection within a choice object.  Note that if any field in
        // the chain of fields is null then an error is returned.  This
        // aggregate is not modified if an error is detected.

    template <class VALTYPE>
    const Aggregate setFieldById(int fieldId, const VALTYPE& value) const;
        // Set the value of the field in this aggregate selected by the
        // specified 'fieldId' to the specified 'value', resetting its
        // nullness flag if 'field.isNul2()' returns 'true', after appropriate
        // conversions (see "Extended Type Conversions" in the
        // 'Aggregate' component-level documentation).  Return a
        // sub-aggregate referring to the modified field on success or an error
        // object on failure (as described in the "Error Handling" section of
        // the 'bdlaggxxx_aggregate' component-level documentation).  If
        // value is null then make the field null.  Note that if
        // 'true == isNul2()' then an error is returned.  This aggregate is
        // not modified if an error is detected.

    const Aggregate setFieldNullById(int fieldId) const;
        // Set the field in this aggregate selected by the specified 'fieldId'
        // to null.  Return a sub-aggregate referring to the modified field on
        // success or an error object on failure (as described in the "Error
        // Handling" section of the 'bdlaggxxx_aggregate' component-level
        // documentation).  Note that if 'true == isNul2()' then an error is
        // returned.  This aggregate is not modified if an error is detected.

    template <class VALTYPE>
    const Aggregate setFieldByIndex(int            fieldIndex,
                                         const VALTYPE& value) const;
        // Set the value of the field in this aggregate selected by the
        // specified 'fieldIndex' to the specified 'value', resetting its
        // nullness flag if 'field.isNul2()' is 'true', after appropriate
        // conversions (see "Extended Type Conversions" in the
        // 'Aggregate' component-level documentation).  Return a
        // sub-aggregate referring to the modified field on success or an error
        // object on failure (as described in the "Error Handling" section of
        // the 'bdlaggxxx_aggregate' component-level documentation).  If
        // value is null then make the field null.  Note that if
        // 'true == isNul2()' then an error is returned.  This aggregate is not
        // modified if an error is detected.

    const Aggregate setFieldNullByIndex(int fieldIndex) const;
        // Set the value of the field in this aggregate selected by the
        // specified 'fieldIndex' to null.  Return a sub-aggregate referring
        // to the modified field on success or an error object on failure (as
        // described in the "Error Handling" section of the 'bdlaggxxx_aggregate'
        // component-level documentation).  Note that if 'true == isNul2()'
        // then an error is returned.  This aggregate is not modified if an
        // error is detected.

    template <class VALTYPE>
    const Aggregate setItem(int index, const VALTYPE& value) const;
        // Set the value in this aggregate of the scalar array item, table
        // row, or choice array item selected by the specified 'index' to the
        // specified 'value', resetting its nullness flag if 'item.isNul2()'
        // is 'true', after appropriate conversions (see "Extended Type
        // Conversions" in the 'Aggregate' component-level
        // documentation).  Return a sub-aggregate referring to the modified
        // item on success or an error object on failure (as described in the
        // "Error Handling" section of the 'bdlaggxxx_aggregate' component-level
        // documentation).  If value is null then make the item null.  Note
        // that if 'true == isNul2()' then an error is returned.  This
        // aggregate is not modified if an error is detected.

    const Aggregate setItemNull(int index) const;
        // Set the value in this aggregate of the scalar array item, table
        // row, or choice array item selected by the specified 'index' to
        // null.  Return a sub-aggregate referring to the modified item on
        // success or an error object on failure (as described in the "Error
        // Handling" section of the 'bdlaggxxx_aggregate' component-level
        // documentation).  Note that if 'true == isNul2()' then an error is
        // returned.  This aggregate is not modified if an error is detected.

    template <class VALTYPE>
    const Aggregate append(const VALTYPE& newItem) const;
        // Append a copy of 'newItem' to the end of the scalar array, table or
        // choice array referenced by this aggregate resetting the nullness
        // flag if 'isNul2()' is 'true'.  Return a sub-aggregate referring to
        // the modifiable newly-inserted item on success or an error object on
        // failure (as described in the "Error Handling" section of the
        // 'bdlaggxxx_aggregate' component-level documentation).  If
        // value is null then append a null item.  This aggregate is not
        // modified if an error is detected.
        //
        // Note that this function is logically equivalent to:
        //..
        //  this->insert(length(), newItem);
        //..

    const Aggregate appendNull() const;
        // Append a null item to the end of the scalar array, table or choice
        // array referenced by this aggregate resetting the nullness flag if
        // 'isNul2()' is 'true'.  Return a sub-aggregate referring to the
        // modifiable newly-inserted item on success or an error object on
        // failure (as described in the "Error Handling" section of the
        // 'bdlaggxxx_aggregate' component-level documentation).
        // This aggregate is not modified if an error is detected.
        //
        // Note that this function is logically equivalent to:
        //..
        //  this->insertNull(length());
        //..

    template <class VALTYPE>
    const Aggregate insert(int pos, const VALTYPE& newItem) const;
        // Insert a copy of the specified 'newItem' at the specified 'pos'
        // index in the scalar array, table, or choice array referenced by
        // this aggregate.  Return a sub-aggregate referring to the modifiable
        // newly-inserted item on success or an error object on failure (as
        // described in the "Error Handling" section of the 'bdlaggxxx_aggregate'
        // component-level documentation).  If 'true == isNul2()' reset the
        // nullness flag if '0 == pos' and return an error otherwise.  If
        // newItem is null then insert a null item.  This aggregate is not
        // modified if an error is detected.

    const Aggregate insertNull(int pos) const;
        // Insert a null item at the specified 'pos' index in the scalar array,
        // table, or choice array referenced by this aggregate.  Return a
        // sub-aggregate referring to the modifiable newly-inserted item on
        // success or an error object on failure (as described in the "Error
        // Handling" section of the 'bdlaggxxx_aggregate' component-level
        // documentation).  If 'true == isNul2()' reset the nullness flag if
        // '0 == pos' and return an error otherwise.  This aggregate is not
        // modified if an error is detected.

    const Aggregate resize(int newSize) const;
        // Grow or shrink the scalar array, table, or choice array referenced
        // by this aggregate to the specified 'newSize' number of items
        // resetting the nullness flag if 'isNul2()' returns 'true'.  If
        // 'newSize > length()', grow the array or table by inserting new
        // items at the end and set them to their default value, as specified
        // in the schema (recursively for the fields within inserted table
        // rows).  If 'newSize < length()' shrink the array or table by
        // discarding excess items at the end.  Return this aggregate on
        // success or an error object, with no effect on this aggregate,
        // if this is not an array.  Note that 'newSize' is interpreted as
        // an unsigned value.

    const Aggregate appendItems(int numItems) const;
        // Append 'numItems' new elements to the end of the scalar array,
        // table, or choice array referenced by this aggregate resetting the
        // nullness flag if 'isNul2()' returns 'true'.  The new items are set
        // to their default values, as specified in the schema (recursively
        // for the fields within inserted table rows).  Return this aggregate
        // on success or an error object on failure (as described in the
        // "Error Handling" section of the 'bdlaggxxx_aggregate' component-level
        // documentation).  This aggregate is not modified if an error is
        // detected.
        //
        // This function is logically equivalent to:
        //..
        //  this->insertItems(length(), numItems);
        //..

    const Aggregate appendNullItems(int numItems) const;
        // Append 'numItems' null elements to the end of the scalar array,
        // table, or choice array referenced by this aggregate resetting the
        // nullness flag if 'isNul2()' returns 'true'.  The new items are set
        // to their default values, as specified in the schema (recursively
        // for the fields within inserted table rows).  Return this aggregate
        // on success or an error object on failure (as described in the
        // "Error Handling" section of the 'bdlaggxxx_aggregate' component-level
        // documentation).  This aggregate is not modified if an error is
        // detected.
        //
        // This function is logically equivalent to:
        //..
        //  this->insertNullItems(length(), numItems);
        //..

    const Aggregate insertItems(int pos, int numItems) const;
        // Insert 'numItems' new elements before the specified 'pos' index in
        // the scalar array, table, or choice array referenced by this
        // aggregate.  The new items are set to their default values, as
        // specified in the schema (recursively for the fields within inserted
        // table rows).  Return this aggregate on success or an error object
        // on failure (as described in the "Error Handling" section of the
        // 'bdlaggxxx_aggregate' component-level documentation).  If
        // 'true == isNul2()' reset the nullness flag if '0 == pos' and return
        // an error otherwise.  This aggregate is not modified if an error is
        // detected.

    const Aggregate insertNullItems(int pos, int numItems) const;
        // Insert 'numItems' null elements before the specified 'pos' index in
        // the scalar array, table, or choice array referenced by this
        // aggregate.  Return this aggregate on success or an error object on
        // failure (as described in the "Error Handling" section of the
        // 'bdlaggxxx_aggregate' component-level documentation).  If
        // 'true == isNul2()' reset the nullness flag if '0 == pos' and return
        // an error otherwise.  This aggregate is not modified if an error is
        // detected.

    const Aggregate remove(int pos) const;
        // Remove the item at the specified 'pos' index in the scalar array,
        // choice array, or table referenced by this aggregate.  Return this
        // aggregate on success or an error object on failure (as described in
        // the "Error Handling" section of the 'bdlaggxxx_aggregate'
        // component-level documentation).  If 'true == isNul2()' return an
        // error.  This aggregate is not modified if an error is detected.

    const Aggregate removeItems(int pos, int numItems) const;
        // Remove the specified 'numItems' items starting at the specified
        // 'pos' index in the scalar array, choice array, or table referenced
        // by this aggregate.  Return this aggregate on success or an error
        // object on failure (as described in the "Error Handling" section of
        // the 'bdlaggxxx_aggregate' component-level documentation).  If
        // 'true == isNul2()' return an error.  This aggregate is not modified
        // if an error is detected.

    const Aggregate removeAllItems() const;
        // Remove all items from the scalar array, choice array, or table
        // referenced by this aggregate.  Return this aggregate on success or
        // an error object on failure (as described in the "Error Handling"
        // section of the 'bdlaggxxx_aggregate' component-level documentation).  If
        // 'true == isNul2()' return an error.  This aggregate is not modified
        // if an error is detected.
        //
        // This function is logically equivalent to:
        //..
        //  this->removeItems(0, length());
        //..

    const Aggregate makeSelection(const char         *newSelector) const;
    const Aggregate makeSelection(const bsl::string&  newSelector) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelector' resetting the
        // nullness flag if 'isNul2()' returns 'true'.  If 'newSelector'
        // is null or the empty string then the selection is reset.  The newly
        // selected sub-object is initialized to its default value.  (See "Null
        // Values and Default Values" in the 'Aggregate' component-level
        // documentation for a more complete description of default values.)
        // Return a sub-aggregate referring to the modifiable selection on
        // success or an error object on failure (as described in the "Error
        // Handling" section of the 'bdlaggxxx_aggregate' component-level
        // documentation).  This aggregate is not modified if an error is
        // detected.

    template <class VALTYPE>
    const Aggregate makeSelection(const char        *newSelector,
                                       const VALTYPE&     value) const;
    template <class VALTYPE>
    const Aggregate makeSelection(const bsl::string& newSelector,
                                       const VALTYPE&     value) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelector' and set the newly
        // selected sub-object to 'value', resetting the nullness flag if
        // 'isNul2()' returns 'true', after appropriate conversions (see
        // "Extended Type Conversions" in the 'Aggregate' component-level
        // documentation).  If 'newSelector' is null or the empty string then
        // the selection is reset.  Return a sub-aggregate referring to the
        // modifiable selection on success or an error object on failure (as
        // described in the "Error Handling" section of the 'bdlaggxxx_aggregate'
        // component-level documentation).  If value is null then the
        // selected aggregate is made null.  If 'newSelector' is valid but
        // 'value' is not convertible to the newly-selected sub-object type,
        // then the sub-object is initialized to its default value, otherwise
        // this aggregate is not modified when an error is detected.

    const Aggregate makeSelectionById(int newSelectorId) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelectorId' resetting the
        // nullness flag if 'isNul2()' returns 'true'.  If 'newSelectorId' is
        // 'bdlmxxx::RecordDef::BDEM_NULL_FIELD_ID' then the selection is reset.
        // The newly selected sub-object is initialized to its default
        // value. (See "Null Values and Default Values" in the 'Aggregate'
        // component-level documentation for a more complete description of
        // default values.)  Return a sub-aggregate referring to the
        // modifiable selection on success or an error object on failure (as
        // described in the "Error Handling" section of the 'bdlaggxxx_aggregate'
        // component-level documentation).  This aggregate is not modified if
        // an error is detected.

    template <class VALTYPE>
    const Aggregate makeSelectionById(int            newSelectorId,
                                           const VALTYPE& value) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelectorId' and set the newly
        // selected sub-object to 'value', resetting the nullness flag if
        // 'isNul2()' returns 'true', after appropriate conversions (see
        // "Extended Type Conversions" in the 'Aggregate' component-level
        // documentation).  If 'newSelectorId' is
        // 'bdlmxxx::RecordDef::BDEM_NULL_FIELD_ID' then the selection is reset.
        // Return a sub-aggregate referring to the new selection on success or
        // an error object on failure (as described in the "Error Handling"
        // section of the 'bdlaggxxx_aggregate' component-level documentation).  If
        // value is null then the selected aggregate is made null.  If
        // 'newSelectorId' is valid but 'value' is not convertible to the
        // newly-selected sub-object type, then the sub-object is initialized
        // to its default value, otherwise this aggregate is not modified when
        // an error is detected.

    const Aggregate makeSelectionByIndex(int newSelectorIndex) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelectorIndex' resetting the
        // nullness flag if 'isNul2()' returns 'true'.  If 'newSelectorIndex'
        // is negative then the selection is reset.  The newly selected
        // sub-object is initialized to its default value.  (See "Null Values
        // and Default Values" in the 'Aggregate' component-level
        // documentation for a more complete description of default values.)
        // Return a sub-aggregate referring to the modifiable selection on
        // success or an error object on failure (as described in the "Error
        // Handling" section of the 'bdlaggxxx_aggregate' component-level
        // documentation).  This aggregate is not modified if an error is
        // detected.

    template <class VALTYPE>
    const Aggregate makeSelectionByIndex(int            newSelectorIndex,
                                              const VALTYPE& value) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelectorIndex' and set the
        // newly selected sub-object to 'value' resetting the nullness flag if
        // 'isNul2()' returns 'true', after appropriate conversions (see
        // "Extended Type Conversions" in the 'Aggregate' component-level
        // documentation).  If 'newSelectorIndex' is negative then the
        // selection is reset.  Return a sub-aggregate referring to the
        // modifiable selection on success or an error object on failure (as
        // described in the "Error Handling" section of the 'bdlaggxxx_aggregate'
        // component-level documentation).  If 'newSelectorIndex' is valid but
        // 'value' is not convertible to the newly-selected sub-object type,
        // then the sub-object is initialized to its default value, otherwise
        // this aggregate is not modified when an error is detected.

    const Aggregate makeValue() const;
        // Construct a value for the nullable object referenced by this
        // aggregate by resetting its nullness flag and assigning it the
        // default value specified in the schema, or (for 'LIST' aggregates)
        // by giving each scalar field its default value and recursively
        // constructing the default value for each non-nullable sublist (see
        // "Null Values and Default Values" in the 'bdlaggxxx_aggregate'
        // component-level documentation).  Return this aggregate on success
        // or an error aggregate on failure.  If the aggregate is not null
        // when this function is called, do nothing (this is not an error).
        // Furthermore, if this aggregate refers to a list with an empty
        // record definition, then the entire list will still be null after
        // this function is called.

    void swap(Aggregate& rhs);
        // Efficiently exchange the states of this aggregate object and the
        // specified 'src' aggregate object such that value and nullness
        // information held by each will be what was formerly held by the
        // other.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version) const;
        // Assign to the object referenced by this aggregate the value read
        // from the specified input 'stream' using the specified 'version'
        // format and return a reference to the modifiable 'stream'.  This
        // aggregate must be initialized with the type and record definition
        // of the expected input before calling this method.  If 'stream' is
        // initially invalid, this operation has no effect.  If 'stream'
        // becomes invalid during this operation, this object is valid, but
        // its value is undefined.  If 'version' is not supported, 'stream' is
        // marked invalid and the object is unaltered.  Note that no version
        // is read from 'stream'.  This operation has the same effect as
        // calling 'bdexStreamIn' on the referenced object (e.g., if
        // 'dataType()' is 'LIST', call 'bdlmxxx::List::bdexStreamIn' on the
        // referenced object).  See the 'bdex' package-level documentation for
        // more information on 'bdex' streaming of value-semantic types and
        // containers.

    // ACCESSORS
    const Aggregate capacityRaw(bsl::size_t *capacity) const;
        // Load, in the specified 'capacity', the total number of items for
        // which sufficient memory is currently allocated if this aggregate
        // references a scalar or choice array, or load the total number of row
        // footprints for which sufficient memory is currently allocated, if
        // this aggregate references a table.  In the latter case, inserting
        // rows that do not exceed this capacity *may* or may *not* result in
        // additional allocations depending on the allocation mode, and whether
        // any row data element itself allocates memory (see the 'reserveRaw'
        // method).  Return the value of this aggregate on success or an error
        // aggregate if this aggregate does not reference an array type.

    bool isError() const;
        // Return 'true' if this object was returned from a function that
        // detected an error.  If this function returns 'true', then
        // 'dataType()' will return 'bdlmxxx::ElemType::BDEM_VOID', 'errorCode()'
        // will return a non-zero value, and 'errorMessage()' will return a
        // non-empty string.

    bool isVoid() const;
        // Return 'true' if 'dataType() == bdlmxxx::ElemType::BDEM_VOID'.  This
        // function will always return 'true' if 'isError()' returns 'true'.

    bool isNul2() const;
        // Return 'true' if the data referenced by this aggregate has a null
        // value, and 'false' otherwise.  This function returns 'true' if
        // 'isVoid()' returns 'true'.

    bool isNullable() const;
        // Return 'true' if the data referenced by this aggregate can be made
        // null, and 'false' otherwise.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    bool isUnset() const;
        // Return 'true' if the data referenced by this aggregate has the
        // "unset" value for its type (i.e., is an "unset" scalar as defined in
        // 'bdltuxxx::Unset', or is an empty array or aggregate), and 'false'
        // otherwise.  This function returns 'true' if 'isVoid()' returns
        // 'true'.  Note that this function should rarely be needed; generally
        // 'isNul2()' should be used instead; 'isUnset()' will be removed in a
        // future release.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    int errorCode() const;
        // Return a negative error code describing the the status of this
        // object if 'isError()' is 'true', or zero if 'isError()' is 'false'.
        // A set of error code constants with names beginning with 'BCEM_' are
        // described in the 'bdlaggxxx_errorcode' component-level documentation.

    bsl::string errorMessage() const;
        // Return a string describing the error state of this object of
        // 'isError()' is 'true', or an empty string if 'isError()' is 'false'.
        // The contents of the string are intended to be human readable and
        // descriptive.  The exact format of the string may change at any time
        // and should not be relied on in a program (use 'errorCode()',
        // instead).

    bsl::string asString() const;
        // Return a text representation of the value referenced by this
        // aggregate.  For enumeration values, the resulting string is the
        // enumerator name corresponding to the referenced value.  For date
        // and time values, the resulting string will have iso8601 format.
        // For other types, the resulting string will have the same format as
        // 'ostream' printing of the underlying data value.  In the case of
        // scalar values, this conversion is "reversible" via the scalar
        // constructor and 'setValue' function; i.e., the string returned by
        // this function can be assigned to a field of the same type to yield
        // the original value (except for round-off error on float and double
        // values):
        //..
        //  Aggregate agg1(bdlmxxx::ElemType::BDEM_SHORT, 123);
        //  bsl::string s = agg1.asString();              // string conversion
        //  assert(s == "123");
        //  Aggregate agg2(bdlmxxx::ElemType::BDEM_SHORT, s); // reverse
        //                                                     // conversion
        //  assert(Aggregate::areEquivalent(agg1, agg2));
        //..

    void loadAsString(bsl::string *result) const;
        // Load into the specified 'result' string a text representation of
        // the value referenced by this aggregate, as returned by
        // 'asString()'.

    bool asBool() const;
    char asChar() const;
    short asShort() const;
    int asInt() const;
    bsls::Types::Int64 asInt64() const;
    float asFloat() const;
    double asDouble() const;
    bdlt::Datetime asDatetime() const;
    bdlt::DatetimeTz asDatetimeTz() const;
    bdlt::Date asDate() const;
    bdlt::DateTz asDateTz() const;
    bdlt::Time asTime() const;
    bdlt::TimeTz asTimeTz() const;
        // Convert the value referenced by this aggregate to the return type
        // of specified conversion function using "Extended Type Conversions"
        // as described in the 'Aggregate' component-level documentation
        // (returning the enumerator ID when converting enumeration objects to
        // numeric values).  Return the appropriate "null" value if conversion
        // fails.

    const bdlmxxx::ElemRef asElemRef() const;
        // Return an element reference providing modifiable access to the
        // element value held by this aggregate.

    bool hasField(const char *fieldName) const;
        // Return 'true' if this aggregate contains a field having the
        // specified 'fieldName' and 'false' otherwise.

    bool hasFieldById(int fieldId) const;
        // Return 'true' if this aggregate contains a field having the
        // specified 'fieldId' and 'false' otherwise.

    bool hasFieldByIndex(int fieldIndex) const;
        // Return 'true' if this aggregate contains a field having the
        // specified 'fieldIndex' and 'false' otherwise.

    const Aggregate field(FieldSelector fieldSelector) const;
        // Get the field within this aggregate specified by the 'fieldSelector'
        // argument, which specifies a field name or array index.  Return a
        // modifiable sub-aggregate referring to the field on success or an
        // error object on failure (as described in the "Error Handling"
        // section of the 'bdlaggxxx_aggregate' component-level documentation).
        // Note that if 'true == isNul2()' then an error is returned.

    const Aggregate field(
              FieldSelector fieldSelector1,
              FieldSelector fieldSelector2,
              FieldSelector fieldSelector3 = FieldSelector(),
              FieldSelector fieldSelector4 = FieldSelector(),
              FieldSelector fieldSelector5 = FieldSelector(),
              FieldSelector fieldSelector6 = FieldSelector(),
              FieldSelector fieldSelector7 = FieldSelector(),
              FieldSelector fieldSelector8 = FieldSelector(),
              FieldSelector fieldSelector9 = FieldSelector(),
              FieldSelector fieldSelector10 = FieldSelector()) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of two to ten 'fieldSelector' arguments, each of
        // which specifies a field name or array index.  Return a sub-aggregate
        // referring to the modifiable field on success or an error object on
        // failure (as described in the "Error Handling" section of the
        // 'bdlaggxxx_aggregate' component-level documentation).  An empty string
        // can be used for any of the 'fieldSelector' arguments to specify the
        // current selection within a choice object.  An unused argument
        // results in the construction of a null 'FieldSelector', which is
        // treated as the end of the argument list.  Note that if
        // 'true == isNul2()' for any field in the chain of fields then an
        // error is returned.
        //
        // This function is equivalent to iteratively calling the
        // single-argument version of 'field', where each 'fieldSelector'
        // argument indicates the name of successively deeper sub-aggregates.
        // For example:
        //..
        //  agg.field("USA","New York","Lexington")
        //..
        // Is equivalent to:
        //..
        //  agg.field("USA").field("New York").field("Lexington")
        //..
        // In addition to being more concise, the first version is more
        // efficient than the second.

    const Aggregate fieldById(int fieldId) const;
        // Get the field within this aggregate with the specified 'fieldId'.
        // Return a modifiable sub-aggregate referring to the field on success
        // or an error object on failure (as described in the "Error Handling"
        // section of the 'bdlaggxxx_aggregate' component-level documentation).
        // Note that 'fieldId' refers to a field's numeric identifier
        // (assigned in its record definition within the schema), not to its
        // positional index within the record definition (see
        // 'fieldByIndex').  Note that if 'true == isNul2()' then an error is
        // returned.

    const Aggregate fieldByIndex(int fieldIndex) const;
        // Get the field within this aggregate with the specified (zero-based)
        // 'fieldIndex'.  Return a sub-aggregate referring to the field on
        // success or an error object on failure (as described in the "Error
        // Handling" section of the 'bdlaggxxx_aggregate' component-level
        // documentation).  Note that if 'true == isNul2()' then an error is
        // returned.

    const Aggregate anonymousField(int n) const;
        // Return a sub-aggregate referring to the specified 'n'th anonymous
        // field within this [list or choice] aggregate (where 'n' is
        // zero-based) or an error object on failure (as described in the
        // "Error Handling" section of the 'bdlaggxxx_aggregate' component-level
        // documentation).  An anonymous field is a field with a null name and
        // is typically, but not always, an aggregate with the 'IS_UNTAGGED'
        // bit set in its formatting mode.  Note that this function differs
        // from 'fieldByIndex' in that only the anonymous fields are counted.
        // Also note that it is rarely necessary to call this function, since
        // 'field', 'setField', 'makeSelection', and 'selection' will
        // automatically descend into anonymous fields in most cases.  For more
        // information see the "Anonymous Fields" section of the
        // 'bdlaggxxx_aggregate' component-level documentation.

    const Aggregate anonymousField() const;
        // If this aggregate contains exactly one anonymous field, then return
        // 'anonymousField(0)', otherwise return an error object.

    bdlmxxx::ElemType::Type
    fieldType(FieldSelector fieldSelector1,
              FieldSelector fieldSelector2  = FieldSelector(),
              FieldSelector fieldSelector3  = FieldSelector(),
              FieldSelector fieldSelector4  = FieldSelector(),
              FieldSelector fieldSelector5  = FieldSelector(),
              FieldSelector fieldSelector6  = FieldSelector(),
              FieldSelector fieldSelector7  = FieldSelector(),
              FieldSelector fieldSelector8  = FieldSelector(),
              FieldSelector fieldSelector9  = FieldSelector(),
              FieldSelector fieldSelector10 = FieldSelector()) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of one to ten 'fieldSelector' arguments, each of
        // which specifies a field name or array index.  Return the type of the
        // field on success, or 'bdlmxxx::ElemType::BDEM_VOID' on a navigation
        // error.  An empty string can be used for any of the 'fieldSelector'
        // arguments to specify the current selection within a choice object.
        // An unused argument results in the construction of a null
        // 'FieldSelector', which is treated as the end of the argument
        // list.

    bdlmxxx::ElemType::Type fieldTypeById(int fieldId) const;
        // Return the type of field referenced by the specified 'fieldId' or
        // 'bdlmxxx::ElemType::BDEM_VOID' if 'fieldId' is not in this aggregate's
        // record definition.

    bdlmxxx::ElemType::Type fieldTypeByIndex(int index) const;
        // Return the type of field at the position specified by the
        // (zero-based) 'index' or 'bdlmxxx::ElemType::BDEM_VOID' if 'index' < 0 or
        // 'recordDef().length() <= index'.

    bdlmxxx::ElemRef fieldRef(
              FieldSelector fieldSelector1,
              FieldSelector fieldSelector2  = FieldSelector(),
              FieldSelector fieldSelector3  = FieldSelector(),
              FieldSelector fieldSelector4  = FieldSelector(),
              FieldSelector fieldSelector5  = FieldSelector(),
              FieldSelector fieldSelector6  = FieldSelector(),
              FieldSelector fieldSelector7  = FieldSelector(),
              FieldSelector fieldSelector8  = FieldSelector(),
              FieldSelector fieldSelector9  = FieldSelector(),
              FieldSelector fieldSelector10 = FieldSelector()) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of one to ten 'fieldSelector' arguments, each of
        // which specifies a field name or array index.  Return an element
        // reference to the specified modifiable field on success or a 'VOID'
        // element reference on error.  An empty string can be used for any of
        // the arguments to specify the current selection within a choice
        // object.  An unused argument results in the construction of a null
        // 'FieldSelector', which is treated as the end of the argument
        // list.  Note that if 'true == isNul2()' for any field in the chain of
        // fields then an error is returned.  Modifying the element through the
        // returned reference will modify the original aggregate.  The behavior
        // is undefined if the resulting element ref is used to modify the data
        // such that it no longer conforms to the schema.

    bdlmxxx::ElemRef fieldRefById(int fieldId) const;
        // Return an element reference to the modifiable field specified by
        // 'fieldId' within this aggregate or a 'VOID' element reference on
        // error.  If this aggregate refers to a choice value, then a
        // 'fieldId' of 'bdlmxxx::RecordDef::BDEM_NULL_FIELD_ID' can be used to
        // specify the current selection.  Modifying the element through the
        // returned reference will modify the original aggregate.  Note that
        // 'fieldId' refers to a field's numeric identifier assigned in its
        // record definition within the schema, not to its positional index
        // within the record definition.  Note that if 'true == isNul2()' then
        // an error is returned.  The behavior is undefined if the resulting
        // element ref is used to modify the data such that it no longer
        // conforms to the schema.

    bdlmxxx::ElemRef fieldRefByIndex(int index) const;
        // Return an element reference to the modifiable field within this
        // aggregate specified by the (zero-based) 'fieldIndex' or a 'VOID'
        // element reference on error.  If this aggregate refers to a choice
        // value, then a 'fieldIndex' of -1 can be used to specify the current
        // selection.  Modifying the element through the returned reference
        // will modify the original aggregate.  Note that if
        // 'true == isNul2()' then an error is returned.  The behavior is
        // undefined if the resulting element ref is used to modify the data
        // such that it no longer conforms to the schema.

    const Aggregate operator[](int index) const;
        // Set the value in this aggregate of the scalar array item, table
        // row, or choice array item selected by the specified 'index'.
        // Return a sub-aggregate referring to the modifiable item on success
        // or an error object on failure (as described in the "Error Handling"
        // section of the 'bdlaggxxx_aggregate' component-level documentation).
        // Note that if 'true == isNul2()' then an error is returned.

    const Aggregate operator[](const bsl::string& fieldName) const;
    const Aggregate operator[](const char *fieldName) const;
        // Get the field within this aggregate with the specified 'fieldId' or
        // 'fieldName'.  Return a modifiable sub-aggregate referring to the
        // field on success or an error object on failure (as described in the
        // "Error Handling" section of the 'bdlaggxxx_aggregate' component-level
        // documentation).  Note that if 'true == isNul2()' then an error is
        // returned.  This operator provides syntactic sugar and can be
        // chained, as follows:
        //..
        //  agg["USA"]["New York"]["Lexington"];
        //..
        // The preceding expression is equivalent to (but slightly less
        // efficient than) the following:
        //..
        //  agg.field("USA","New York","Lexington");
        //..

    int length() const;
        // Return the fields or items in the scalar array, list, table, or
        // choice array referenced by this aggregate or
        // 'BCEM_NOT_AN_ARRAY' for other data types.  Note that a null
        // array will return 0.

    int size() const;
        // Equivalent to 'length()' (STL-style).

    const Aggregate selection() const;
        // If this aggregate refers to choice or choice array item, return the
        // sub-aggregate that refers to the modifiable current selection, or a
        // void aggregate if there is no current selection.  If this aggregate
        // refers to a list or row, look for an anonymous field within the
        // list or row and recursively look for a choice in the anonymous
        // field (if any), as per the "Anonymous Fields" section of the
        // 'bdlaggxxx_aggregate' component-level documentation.  Otherwise, (if
        // this aggregate does not directly or indirectly refer to a choice or
        // choice array item) or 'true == isNul2()' return an error object.

    int numSelections() const;
        // If this aggregate refers to choice or choice array item, return the
        // number of available selectors or 0 if 'true == isNul2()'.  If this
        // aggregate refers to a list or row, look for an anonymous field
        // within the list or row and recursively look for a choice in the
        // anonymous field (if any), as per the "Anonymous Fields" section of
        // the 'bdlaggxxx_aggregate' component-level documentation.  Otherwise,
        // return an error code.

    const char *selector() const;
        // Return the name for selector of the choice or choice array item
        // referenced by this aggregate or an empty string either if there is
        // no current selection, if the current selector does not have a text
        // name, if this aggregate does not refer to a choice or choice array
        // item, or 'true == isNul2()'.  If this aggregate refers to a list or
        // row, look for an anonymous field within the list or row and
        // recursively look for a choice in the anonymous field (if any), as
        // per the "Anonymous Fields" section of the 'bdlaggxxx_aggregate'
        // component-level documentation.  The returned pointer is valid until
        // the the choice object is modified or destroyed.  Note that an error
        // condition is indistinguishable from one of the valid reasons for
        // returning an empty string.  The 'selection' or 'selectorId' methods
        // can be used to distinguish an error condition from a valid selector.

    int selectorId() const;
        // Return the ID for selector of the choice or choice array item
        // referenced by this aggregate, 'bdlmxxx::RecordDef::BDEM_NULL_FIELD_ID'
        // if there is no current selection, the current selection does not
        // have a numeric ID, this aggregate does not refer to a choice or
        // choice array item, or 'true == isNul2()'.  If this aggregate refers
        // to a list or row, look for an anonymous field within the list or
        // row and recursively look for a choice in the anonymous field (if
        // any), as per the "Anonymous Fields" section of the 'bdlaggxxx_aggregate'
        // component-level documentation.  Note that the returned ID is the
        // selector's numeric identifier (assigned in its record definition
        // within the schema), not its positional index within the record
        // definition (see 'selectorIndex').

    int selectorIndex() const;
        // Return the index for selector of the choice or choice array item
        // referenced by this aggregate, -1 if there is no current selection,
        // or if 'true == isNul2()' or an error code if this aggregate does
        // not refer to a choice or choice array item.  If this aggregate
        // refers to a list or row, look for an anonymous field within the
        // list or row and recursively look for a choice in the anonymous
        // field (if any), as per the "Anonymous Fields" section of the
        // 'bdlaggxxx_aggregate' component-level documentation.

    const Aggregate clone(bslma::Allocator *basicAllocator = 0) const;
        // Make an independent copy, using the specified 'basicAllocator', of
        // this aggregate's data and schema and return a new aggregate object
        // holding the copy.  Changes made to the clone's data will have no
        // effect on this aggregate's data.  The returned clone will compare
        // equivalent to this aggregate (using 'areEquivalent') but will not
        // compare identical to this aggregate (using 'areIdentical').  If
        // this aggregate is an error object, then the clone will be an
        // equivalent, but not identical, error object.  Note that the
        // nullness information is also cloned.

    const Aggregate cloneData(bslma::Allocator *basicAllocator = 0) const;
        // Make an independent copy, using the specified 'basicAllocator', of
        // the this aggregate's data, but not its schema, and return a new a
        // new aggregate object holding the copy.  The new aggregate will
        // share this aggregate's schema (which is read-only), but changes
        // made to the clone's data will have no effect on this aggregate's
        // data.  The returned clone will compare equivalent to this aggregate
        // (using 'areEquivalent') but will not compare identical to this
        // aggregate (using 'areIdentical').  If this aggregate is an error
        // object, then the clone will be an equivalent, but not identical,
        // error object.  Note that the nullness information is also cloned.

    bdlmxxx::ElemType::Type dataType() const;
        // Return the type of data referenced by this aggregate.  Return
        // 'bdlmxxx::ElemType::BDEM_VOID' for a void or error aggregate.

    bsl::shared_ptr<const bdlmxxx::Schema> schemaPtr() const;
        // Return a shared pointer to the non-modifiable schema associated
        // with this aggregate.  Return an empty pointer if this aggregate has
        // no associated schema (i.e., is unconstrained).

    const AggregateRaw& aggregateRaw() const;
        // Return a reference providing non-modifiable access to the
        // non-reference-counted portion of this aggregate.

    const bdlmxxx::RecordDef *recordConstraint() const;
        // Return a pointer providing non-modifiable access to the record
        // definition that describes the structure of the object referenced by
        // this aggregate, or a null pointer if this aggregate references a
        // scalar, array of scalars, or unconstrained 'bdem' aggregate.

    const bdlmxxx::RecordDef& recordDef() const;
        // Return a reference providing non-modifiable access to the record
        // definition that describes the structure of the object referenced by
        // this aggregate.  The behavior is undefined unless this aggregate
        // references a constrained list, constrained table, or constrained
        // choice object.  (Use 'recordConstraint' if there is any doubt as to
        // whether this aggregate has a record definition.)

    bsl::shared_ptr<const bdlmxxx::RecordDef> recordDefPtr() const;
        // Return a shared pointer to the non-modifiable record definition
        // that describes the structure of object referenced by this
        // aggregate.  Return an empty pointer if this aggregate references a
        // scalar, array of scalars, or unconstrained aggregate.

    const bdlmxxx::EnumerationDef *enumerationConstraint() const;
        // Return a pointer providing non-modifiable access to the enumeration
        // definition that constrains the object referenced by this aggregate,
        // or a null pointer if this aggregate does not reference an
        // enumeration object.

    const bdlmxxx::FieldDef *fieldDef() const;
        // Return a pointer providing non-modifiable access to the field
        // definition for the object referenced by this aggregate, or null if
        // this object does not have a field definition.  An aggregate
        // constructed directly using a record definition will not have a field
        // definition, whereas a sub-aggregate returned by a field-access
        // function (e.g., 'operator[]' or 'field') will.  Note that if this
        // aggregate is an item within an array, table, or choice array, then
        // 'fieldDef()->elemType()' will return the *array* type, not the
        // *item* type (i.e., 'fieldDef()->elemType()' will not match
        // 'dataType()').

    const bdlmxxx::FieldDef *fieldSpec() const;
        // Return a pointer providing non-modifiable access to the object
        // referenced by this aggregate, or null if this object does not have a
        // field definition.  An aggregate constructed directly using a record
        // definition will not have a field definition, whereas a sub-aggregate
        // returned by a field-access function (e.g., 'operator[]' or 'field')
        // will.  Note that if this aggregate is an item within an array,
        // table, or choice array, then 'fieldSpec()->elemType()' will return
        // the *array* type, not the *item* type (i.e.,
        // 'fieldSpec()->elemType()' will not match 'dataType()').
        //
        // DEPRECATED: use 'fieldDef' instead.

    const void *data() const;
        // Return a pointer providing non-modifiable access to the data
        // referenced by this aggregate.  The behavior is undefined unless
        // 'bdlmxxx::ElemType::BDEM_VOID != dataType()'.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of the object referenced by this aggregate to the
        // specified output 'stream' using the specified 'version' format and
        // return a reference to the modifiable 'stream'.  If 'version' is not
        // supported, 'stream' is unmodified.  Note that 'version' is not
        // written to 'stream'.  This operation has the same effect as calling
        // 'bdexStreamOut' on the referenced object (e.g., if 'dataType()' is
        // 'LIST', call 'bdlmxxx::List::bdexStreamOut' on the referenced object).
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format the referenced object to the specified output 'stream' at
        // the (absolute value of) the optionally specified indentation
        // 'level' and return a reference to 'stream'.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', the number of
        // spaces per indentation level for this and all of its nested
        // objects.  If 'level' is negative, suppress indentation of the first
        // line.  If 'spacesPerLevel' is negative, format the entire output on
        // one line.  If 'stream' is not valid on entry, this operation has no
        // effect.  For scalars and arrays of scalars, this 'print' function
        // delegates to the appropriate printing mechanism for referenced
        // object -- the aggregate adds no additional text to the
        // output.  (E.g., the result of printing an aggregate that references
        // a string is indistinguishable from the result of printing the
        // string directly.)  For list, row, choice, choice array item, table,
        // and choice array, this print function prepend's each field with the
        // name of the field.
};
}  // close package namespace

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const bdlaggxxx::Aggregate& rhs);

namespace bdlaggxxx {    // Format 'rhs' in human-readable form (same format as
    // 'rhs.print(stream, 0, -1)') and return a modifiable reference to
    // 'stream'.

// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                        //---------------------
                        // class Aggregate
                        //---------------------

// PRIVATE CLASS METHODS
template <class TYPE>
inline
const TYPE& Aggregate::valueRef(const TYPE& value)
{
    return value;
}

inline
bdlmxxx::ElemRef Aggregate::valueRef(const Aggregate& value)
{
    return value.asElemRef();
}

// CLASS METHODS
inline
bool Aggregate::areIdentical(const Aggregate& lhs,
                                  const Aggregate& rhs)
{
    return AggregateRaw::areIdentical(lhs.aggregateRaw(),
                                           rhs.aggregateRaw());
}

inline
bool Aggregate::areEquivalent(const Aggregate& lhs,
                                   const Aggregate& rhs)
{
    return AggregateRaw::areEquivalent(lhs.aggregateRaw(),
                                            rhs.aggregateRaw());
}

inline
int Aggregate::maxSupportedBdexVersion()
{
    return AggregateRaw::maxSupportedBdexVersion();
}

// CREATORS
template <class VALTYPE>
Aggregate::Aggregate(const bdlmxxx::ElemType::Type  dataType,
                               const VALTYPE&             value,
                               bslma::Allocator          *basicAllocator)
: d_aggregateRaw()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
    bsl::shared_ptr<void> value_sp = makeValuePtr(dataType, basicAllocator);

    int status = bdlmxxx::Convert::toBdemType(value_sp.get(),
                                          dataType,
                                          valueRef(value));
    if (status) {
        *this = makeError(ErrorCode::BCEM_BAD_CONVERSION,
                          "Invalid conversion to %s from %s",
                          bdlmxxx::ElemType::toAscii(dataType),
                          bdlmxxx::ElemType::toAscii(
                             AggregateRaw::getBdemType(valueRef(value))));
    }
    else {
        bsl::shared_ptr<int> null_sp;
        null_sp.createInplace(basicAllocator, 0);
        d_isTopLevelAggregateNullRep_p = null_sp.rep();
        d_isTopLevelAggregateNullRep_p->acquireRef();

        d_valueRep_p = value_sp.rep();
        d_valueRep_p->acquireRef();

        d_aggregateRaw.setTopLevelAggregateNullness(null_sp.get());
        d_aggregateRaw.setDataType(dataType);
        d_aggregateRaw.setData(value_sp.get());
    }
}

inline
Aggregate::Aggregate(
                  const bsl::shared_ptr<const bdlmxxx::RecordDef>&  recordDefPtr,
                  bslma::Allocator                             *basicAllocator)
: d_aggregateRaw()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
    init(recordDefPtr, bdlmxxx::ElemType::BDEM_VOID, basicAllocator);
}

inline
Aggregate::Aggregate(
                        const bsl::shared_ptr<bdlmxxx::RecordDef>&  recordDefPtr,
                        bslma::Allocator                       *basicAllocator)
: d_aggregateRaw()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
    init(recordDefPtr, bdlmxxx::ElemType::BDEM_VOID, basicAllocator);
}

inline
Aggregate::Aggregate(
                  const bsl::shared_ptr<const bdlmxxx::RecordDef>&  recordDefPtr,
                  bdlmxxx::ElemType::Type                           elemType,
                  bslma::Allocator                             *basicAllocator)
: d_aggregateRaw()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
    init(recordDefPtr, elemType, basicAllocator);
}

inline
Aggregate::Aggregate(
                        const bsl::shared_ptr<bdlmxxx::RecordDef>&  recordDefPtr,
                        bdlmxxx::ElemType::Type                     elemType,
                        bslma::Allocator                       *basicAllocator)
: d_aggregateRaw()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
    init(recordDefPtr, elemType, basicAllocator);
}

inline
Aggregate::Aggregate(
                     const bsl::shared_ptr<const bdlmxxx::Schema>&  schemaPtr,
                     const bsl::string&                         recordName,
                     bdlmxxx::ElemType::Type                        elemType,
                     bslma::Allocator                          *basicAllocator)
: d_aggregateRaw()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
    init(schemaPtr, recordName.c_str(), elemType, basicAllocator);
}

inline
Aggregate::Aggregate(
                           const bsl::shared_ptr<bdlmxxx::Schema>&  schemaPtr,
                           const bsl::string&                   recordName,
                           bdlmxxx::ElemType::Type                  elemType,
                           bslma::Allocator                    *basicAllocator)
: d_aggregateRaw()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
    init(schemaPtr, recordName.c_str(), elemType, basicAllocator);
}

// MANIPULATORS
inline
AggregateRaw& Aggregate::aggregateRaw()
{
    return d_aggregateRaw;
}

// REFERENCED-VALUE MANIPULATORS
inline
const Aggregate& Aggregate::makeNull()  const
{
    d_aggregateRaw.makeNull();
    return *this;
}

inline
const Aggregate Aggregate::makeValue() const
{
    d_aggregateRaw.makeValue();
    return *this;
}

template <class VALTYPE>
const Aggregate Aggregate::setValue(const VALTYPE& value) const
{
    if (isError()) {
        return *this;                                                 // RETURN
    }

    ErrorAttributes errorDescription;
    if (0 != d_aggregateRaw.setValue(&errorDescription, valueRef(value))) {
        return makeError(errorDescription);                           // RETURN
    }
    return *this;
}

template <class VALTYPE>
inline
const Aggregate Aggregate::setField(
                                             FieldSelector fieldSelector1,
                                             const VALTYPE&     value) const
{
    return fieldImp(true, fieldSelector1).setValue(value);
}

template <class VALTYPE>
inline
const Aggregate Aggregate::setField(
                                             FieldSelector fieldSelector1,
                                             FieldSelector fieldSelector2,
                                             const VALTYPE&     value) const
{
    return fieldImp(true, fieldSelector1, fieldSelector2).setValue(value);
}

template <class VALTYPE>
inline
const Aggregate Aggregate::setField(
                                             FieldSelector fieldSelector1,
                                             FieldSelector fieldSelector2,
                                             FieldSelector fieldSelector3,
                                             const VALTYPE&     value) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3).setValue(value);
}

template <class VALTYPE>
inline
const Aggregate Aggregate::setField(
                                             FieldSelector fieldSelector1,
                                             FieldSelector fieldSelector2,
                                             FieldSelector fieldSelector3,
                                             FieldSelector fieldSelector4,
                                             const VALTYPE&     value) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4).setValue(value);
}

template <class VALTYPE>
inline
const Aggregate Aggregate::setField(
                                             FieldSelector fieldSelector1,
                                             FieldSelector fieldSelector2,
                                             FieldSelector fieldSelector3,
                                             FieldSelector fieldSelector4,
                                             FieldSelector fieldSelector5,
                                             const VALTYPE&     value) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5).setValue(value);
}

template <class VALTYPE>
inline
const Aggregate Aggregate::setField(
                                             FieldSelector fieldSelector1,
                                             FieldSelector fieldSelector2,
                                             FieldSelector fieldSelector3,
                                             FieldSelector fieldSelector4,
                                             FieldSelector fieldSelector5,
                                             FieldSelector fieldSelector6,
                                             const VALTYPE&     value) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5,
                    fieldSelector6).setValue(value);
}

template <class VALTYPE>
inline
const Aggregate Aggregate::setField(
                                             FieldSelector fieldSelector1,
                                             FieldSelector fieldSelector2,
                                             FieldSelector fieldSelector3,
                                             FieldSelector fieldSelector4,
                                             FieldSelector fieldSelector5,
                                             FieldSelector fieldSelector6,
                                             FieldSelector fieldSelector7,
                                             const VALTYPE&     value) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5,
                    fieldSelector6,
                    fieldSelector7).setValue(value);
}

template <class VALTYPE>
inline
const Aggregate Aggregate::setField(
                                             FieldSelector fieldSelector1,
                                             FieldSelector fieldSelector2,
                                             FieldSelector fieldSelector3,
                                             FieldSelector fieldSelector4,
                                             FieldSelector fieldSelector5,
                                             FieldSelector fieldSelector6,
                                             FieldSelector fieldSelector7,
                                             FieldSelector fieldSelector8,
                                             const VALTYPE&     value) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5,
                    fieldSelector6,
                    fieldSelector7,
                    fieldSelector8).setValue(value);
}

template <class VALTYPE>
inline
const Aggregate Aggregate::setField(
                                             FieldSelector fieldSelector1,
                                             FieldSelector fieldSelector2,
                                             FieldSelector fieldSelector3,
                                             FieldSelector fieldSelector4,
                                             FieldSelector fieldSelector5,
                                             FieldSelector fieldSelector6,
                                             FieldSelector fieldSelector7,
                                             FieldSelector fieldSelector8,
                                             FieldSelector fieldSelector9,
                                             const VALTYPE&     value) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5,
                    fieldSelector6,
                    fieldSelector7,
                    fieldSelector8,
                    fieldSelector9).setValue(value);
}

template <class VALTYPE>
inline
const Aggregate Aggregate::setField(
                                            FieldSelector fieldSelector1,
                                            FieldSelector fieldSelector2,
                                            FieldSelector fieldSelector3,
                                            FieldSelector fieldSelector4,
                                            FieldSelector fieldSelector5,
                                            FieldSelector fieldSelector6,
                                            FieldSelector fieldSelector7,
                                            FieldSelector fieldSelector8,
                                            FieldSelector fieldSelector9,
                                            FieldSelector fieldSelector10,
                                            const VALTYPE&     value) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5,
                    fieldSelector6,
                    fieldSelector7,
                    fieldSelector8,
                    fieldSelector9,
                    fieldSelector10).setValue(value);
}

inline
const Aggregate
Aggregate::setFieldNull(FieldSelector fieldSelector1) const
{
    return fieldImp(true, fieldSelector1).makeNull();
}

inline
const Aggregate
Aggregate::setFieldNull(FieldSelector fieldSelector1,
                             FieldSelector fieldSelector2) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2).makeNull();
}

inline
const Aggregate
Aggregate::setFieldNull(FieldSelector fieldSelector1,
                             FieldSelector fieldSelector2,
                             FieldSelector fieldSelector3) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3).makeNull();
}

inline
const Aggregate
Aggregate::setFieldNull(FieldSelector fieldSelector1,
                             FieldSelector fieldSelector2,
                             FieldSelector fieldSelector3,
                             FieldSelector fieldSelector4) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4).makeNull();
}

inline
const Aggregate
Aggregate::setFieldNull(FieldSelector fieldSelector1,
                             FieldSelector fieldSelector2,
                             FieldSelector fieldSelector3,
                             FieldSelector fieldSelector4,
                             FieldSelector fieldSelector5) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5).makeNull();
}

inline
const Aggregate
Aggregate::setFieldNull(FieldSelector fieldSelector1,
                             FieldSelector fieldSelector2,
                             FieldSelector fieldSelector3,
                             FieldSelector fieldSelector4,
                             FieldSelector fieldSelector5,
                             FieldSelector fieldSelector6) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5,
                    fieldSelector6).makeNull();
}

inline
const Aggregate
Aggregate::setFieldNull(FieldSelector fieldSelector1,
                             FieldSelector fieldSelector2,
                             FieldSelector fieldSelector3,
                             FieldSelector fieldSelector4,
                             FieldSelector fieldSelector5,
                             FieldSelector fieldSelector6,
                             FieldSelector fieldSelector7) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5,
                    fieldSelector6,
                    fieldSelector7).makeNull();
}

inline
const Aggregate
Aggregate::setFieldNull(FieldSelector fieldSelector1,
                             FieldSelector fieldSelector2,
                             FieldSelector fieldSelector3,
                             FieldSelector fieldSelector4,
                             FieldSelector fieldSelector5,
                             FieldSelector fieldSelector6,
                             FieldSelector fieldSelector7,
                             FieldSelector fieldSelector8) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5,
                    fieldSelector6,
                    fieldSelector7,
                    fieldSelector8).makeNull();
}

inline
const Aggregate
Aggregate::setFieldNull(FieldSelector fieldSelector1,
                             FieldSelector fieldSelector2,
                             FieldSelector fieldSelector3,
                             FieldSelector fieldSelector4,
                             FieldSelector fieldSelector5,
                             FieldSelector fieldSelector6,
                             FieldSelector fieldSelector7,
                             FieldSelector fieldSelector8,
                             FieldSelector fieldSelector9) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5,
                    fieldSelector6,
                    fieldSelector7,
                    fieldSelector8,
                    fieldSelector9).makeNull();
}

inline
const Aggregate
Aggregate::setFieldNull(FieldSelector fieldSelector1,
                             FieldSelector fieldSelector2,
                             FieldSelector fieldSelector3,
                             FieldSelector fieldSelector4,
                             FieldSelector fieldSelector5,
                             FieldSelector fieldSelector6,
                             FieldSelector fieldSelector7,
                             FieldSelector fieldSelector8,
                             FieldSelector fieldSelector9,
                             FieldSelector fieldSelector10) const
{
    return fieldImp(true,
                    fieldSelector1,
                    fieldSelector2,
                    fieldSelector3,
                    fieldSelector4,
                    fieldSelector5,
                    fieldSelector6,
                    fieldSelector7,
                    fieldSelector8,
                    fieldSelector9,
                    fieldSelector10).makeNull();
}

template <class VALTYPE>
inline
const Aggregate
Aggregate::setFieldById(int fieldId, const VALTYPE& value) const
{
    return fieldById(fieldId).setValue(value);
}

inline
const Aggregate Aggregate::setFieldNullById(int fieldId) const
{
    return fieldById(fieldId).makeNull();
}

template <class VALTYPE>
inline
const Aggregate
Aggregate::setFieldByIndex(int fieldIndex, const VALTYPE& value) const
{
    return fieldByIndex(fieldIndex).setValue(value);
}

inline
const Aggregate Aggregate::setFieldNullByIndex(int fieldIndex) const
{
    return fieldByIndex(fieldIndex).makeNull();
}

template <class VALTYPE>
inline
const Aggregate
Aggregate::setItem(int index, const VALTYPE& value) const
{
    return (*this)[index].setValue(value);
}

inline
const Aggregate Aggregate::setItemNull(int index) const
{
    return (*this)[index].makeNull();
}

template <class VALTYPE>
inline
const Aggregate Aggregate::append(const VALTYPE& newItem) const
{
    return insert(length(), newItem);
}

inline
const Aggregate Aggregate::appendNull() const
{
    return insertNull(length());
}

template <class VALTYPE>
const Aggregate Aggregate::insert(int            pos,
                                            const VALTYPE& newItem) const
{
    AggregateRaw    field;
    ErrorAttributes errorDescription;
    if (0 != d_aggregateRaw.insertItem(&field,
                                  &errorDescription,
                                  pos,
                                  valueRef(newItem))) {
        return makeError(errorDescription);                           // RETURN
    }
    return Aggregate(field,
                          d_schemaRep_p,
                          d_valueRep_p,
                          d_isTopLevelAggregateNullRep_p);
}

inline
const Aggregate Aggregate::insertNull(int pos) const
{
    const Aggregate& retAgg = insertNullItems(pos, 1);
    return retAgg.isError() ? retAgg : retAgg.field(pos);
}

inline
const Aggregate Aggregate::appendItems(int numItems) const
{
    return insertItems(length(), numItems);
}

inline
const Aggregate Aggregate::appendNullItems(int numItems) const
{
    return insertNullItems(length(), numItems);
}

inline
bsl::string Aggregate::asString() const
{
    return d_aggregateRaw.asString();
}

inline
void Aggregate::loadAsString(bsl::string *result) const
{
    d_aggregateRaw.loadAsString(result);
}

inline
int Aggregate::selectorId() const
{
    return d_aggregateRaw.selectorId();
}

inline
int Aggregate::length() const
{
    return d_aggregateRaw.length();
}

inline
int Aggregate::numSelections() const
{
    return d_aggregateRaw.numSelections();
}

inline
const char *Aggregate::selector() const
{
    return d_aggregateRaw.selector();
}

inline
bool Aggregate::asBool() const
{
    return d_aggregateRaw.asBool();
}

inline
char Aggregate::asChar() const
{
    return d_aggregateRaw.asChar();
}

inline
short Aggregate::asShort() const
{
    return d_aggregateRaw.asShort();
}

inline
int Aggregate::asInt() const
{
    return d_aggregateRaw.asInt();
}

inline
bsls::Types::Int64 Aggregate::asInt64() const
{
    return d_aggregateRaw.asInt64();
}

inline
float Aggregate::asFloat() const
{
    return d_aggregateRaw.asFloat();
}

inline
double Aggregate::asDouble() const
{
    return d_aggregateRaw.asDouble();
}

inline
bdlt::Datetime Aggregate::asDatetime() const
{
    return d_aggregateRaw.asDatetime();
}

inline
bdlt::DatetimeTz Aggregate::asDatetimeTz() const
{
    return d_aggregateRaw.asDatetimeTz();
}

inline
bdlt::Date Aggregate::asDate() const
{
    return d_aggregateRaw.asDate();
}

inline
bdlt::DateTz Aggregate::asDateTz() const
{
    return d_aggregateRaw.asDateTz();
}

inline
bdlt::Time Aggregate::asTime() const
{
    return d_aggregateRaw.asTime();
}

inline
bdlt::TimeTz Aggregate::asTimeTz() const
{
    return d_aggregateRaw.asTimeTz();
}

inline
int Aggregate::size() const
{
    return length();
}

inline
const Aggregate Aggregate::remove(int pos) const
{
    return removeItems(pos, 1);
}

inline
const Aggregate Aggregate::removeAllItems() const
{
    return removeItems(0, length());
}

inline
const Aggregate
Aggregate::makeSelection(const bsl::string& newSelector) const
{
    return makeSelection(newSelector.c_str());
}

template <class VALTYPE>
const Aggregate
Aggregate::makeSelection(const char     *newSelector,
                              const VALTYPE&  value) const
{
    AggregateRaw    field;
    ErrorAttributes errorDescription;

    if (0 == d_aggregateRaw.makeSelection(&field,
                                     &errorDescription,
                                     newSelector,
                                     valueRef(value))) {
        return Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

template <class VALTYPE>
inline
const Aggregate
Aggregate::makeSelection(const bsl::string& newSelector,
                              const VALTYPE&     value) const
{
    return makeSelection(newSelector.c_str(), valueRef(value));
}

template <class VALTYPE>
const Aggregate
Aggregate::makeSelectionById(int id, const VALTYPE& value) const
{
    AggregateRaw    field;
    ErrorAttributes errorDescription;

    if (0 == d_aggregateRaw.makeSelectionById(&field,
                                         &errorDescription,
                                         id,
                                         valueRef(value))) {
        return Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

template <class VALTYPE>
const Aggregate
Aggregate::makeSelectionByIndex(int index, const VALTYPE& value) const
{
    AggregateRaw    field;
    ErrorAttributes errorDescription;

    if (0 == d_aggregateRaw.makeSelectionByIndex(&field,
                                            &errorDescription,
                                            index,
                                            valueRef(value))) {
        return Aggregate(field,
                              d_schemaRep_p,
                              d_valueRep_p,
                              d_isTopLevelAggregateNullRep_p);        // RETURN
    }
    else {
        return makeError(errorDescription);                           // RETURN
    }
}

inline
const Aggregate
Aggregate::reserveRaw(bsl::size_t numItems)
{
    ErrorAttributes errorDescription;
    if (0 == d_aggregateRaw.reserveRaw(&errorDescription, numItems)) {
        return *this;                                                 // RETURN
    }
    return makeError(errorDescription);
}

template <class STREAM>
inline
STREAM& Aggregate::bdexStreamIn(STREAM& stream, int version) const
{
    return d_aggregateRaw.bdexStreamIn(stream, version);
}

// ACCESSORS
inline
const Aggregate Aggregate::capacityRaw(bsl::size_t *capacity) const
{
    BSLS_ASSERT_SAFE(capacity);

    ErrorAttributes errorDescription;
    if (0 == d_aggregateRaw.capacityRaw(&errorDescription, capacity)) {
        return *this;                                                 // RETURN
    }
    return makeError(errorDescription);
}

inline
int Aggregate::selectorIndex() const
{
    return d_aggregateRaw.selectorIndex();
}

inline
const bdlmxxx::ElemRef Aggregate::asElemRef() const
{
    return d_aggregateRaw.asElemRef();
}

inline
bool Aggregate::isError() const
{
    return d_aggregateRaw.isError();
}

inline
bool Aggregate::isNul2() const
{
    return d_aggregateRaw.isNull();
}

inline
bool Aggregate::isNullable() const
{
    return d_aggregateRaw.isNullable();
}

inline
int Aggregate::errorCode() const
{
    return d_aggregateRaw.errorCode();
}

inline
bsl::string Aggregate::errorMessage() const
{
    return d_aggregateRaw.errorMessage();
}

inline
bool Aggregate::isVoid() const
{
    return d_aggregateRaw.isVoid();
}

inline
bool Aggregate::hasField(const char *fieldName) const
{
    return d_aggregateRaw.hasField(fieldName);
}

inline
bool Aggregate::hasFieldById(int fieldId) const
{
    return d_aggregateRaw.hasFieldById(fieldId);
}

inline
bool Aggregate::hasFieldByIndex(int fieldIndex) const
{
    return d_aggregateRaw.hasFieldByIndex(fieldIndex);
}

inline
const Aggregate
Aggregate::operator[](const bsl::string& fieldName) const
{
    return fieldImp(true, fieldName.c_str());
}

inline
const Aggregate Aggregate::operator[](const char *fieldName) const
{
    return fieldImp(true, fieldName);
}

inline
bdlmxxx::ElemType::Type Aggregate::dataType() const
{
    return d_aggregateRaw.dataType();
}

inline
const bdlmxxx::RecordDef *Aggregate::recordConstraint() const
{
    return d_aggregateRaw.recordConstraint();
}

inline
const bdlmxxx::RecordDef& Aggregate::recordDef() const
{
    return *recordConstraint();
}

inline
const bdlmxxx::EnumerationDef *Aggregate::enumerationConstraint() const
{
    return d_aggregateRaw.enumerationConstraint();
}

inline
const bdlmxxx::FieldDef *Aggregate::fieldDef() const
{
    return d_aggregateRaw.fieldDef();
}

inline
const bdlmxxx::FieldDef *Aggregate::fieldSpec() const
{
    return fieldDef();
}

inline
const void *Aggregate::data() const
{
    return d_aggregateRaw.data();
}

template <class STREAM>
inline
STREAM& Aggregate::bdexStreamOut(STREAM& stream, int version) const
{
    return d_aggregateRaw.bdexStreamOut(stream, version);
}

inline
bsl::ostream& Aggregate::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    return d_aggregateRaw.print(stream, level, spacesPerLevel);
}

inline
const AggregateRaw& Aggregate::aggregateRaw() const
{
    return d_aggregateRaw;
}
}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdlaggxxx::Aggregate& rhs)
{
    return rhs.print(stream, 0, -1);
}

namespace bdlaggxxx {
// ============================================================================
// Note: All of the remaining code pertains to 'bdeat'.
// ============================================================================

                // ===========================================
                // local struct Aggregate_NullableAdapter
                // ===========================================

struct Aggregate_NullableAdapter {
    Aggregate *d_element_p;
};
}  // close package namespace

// ============================================================================
//           'bdlat_arrayfunctions' overloads and specializations
// ============================================================================

namespace bdeat_ArrayFunctions {

    // META-FUNCTIONS
    template <>
    struct IsArray<bdlaggxxx::Aggregate> {
        enum { VALUE = 1 };
    };

    template <>
    struct ElementType<bdlaggxxx::Aggregate> {
        typedef bdlaggxxx::Aggregate Type;
    };

}  // close namespace bdlat_ArrayFunctions

namespace bdlaggxxx {
template <class MANIPULATOR>
inline
int bdeat_arrayManipulateElement(Aggregate *array,
                                 MANIPULATOR&    manipulator,
                                 int             index)
{
    return bdeat_arrayManipulateElement(&array->aggregateRaw(),
                                        manipulator,
                                        index);
}

inline
void bdeat_arrayResize(Aggregate *array, int newSize)
{
    array->resize(newSize);
}

template <class ACCESSOR>
inline
int bdeat_arrayAccessElement(const Aggregate& array,
                             ACCESSOR&             accessor,
                             int                   index)
{
    return bdeat_arrayAccessElement(array.aggregateRaw(), accessor, index);
}

inline
bsl::size_t bdeat_arraySize(const Aggregate& array)
{
    return array.size();
}
}  // close package namespace

// ============================================================================
//           'bdlat_choicefunctions' overloads and specializations
// ============================================================================

namespace bdeat_ChoiceFunctions {

    // META-FUNCTIONS
    template <>
    struct IsChoice<bdlaggxxx::Aggregate> {
        enum { VALUE = 1 };
    };

}  // close namespace bdlat_ChoiceFunctions

namespace bdlaggxxx {
inline
bool bdeat_choiceHasSelection(const Aggregate&  object,
                              const char            *selectionName,
                              int                    selectionNameLength)
{
    return bdeat_choiceHasSelection(object.aggregateRaw(),
                                    selectionName,
                                    selectionNameLength);
}

inline
int bdeat_choiceMakeSelection(Aggregate *object, int selectionId)
{
    return bdeat_choiceMakeSelection(&object->aggregateRaw(), selectionId);
}

inline
int bdeat_choiceMakeSelection(Aggregate *object,
                              const char     *selectionName,
                              int             selectionNameLength)
{
    return bdeat_choiceMakeSelection(&object->aggregateRaw(),
                                     selectionName,
                                     selectionNameLength);
}

template <class MANIPULATOR>
inline
int bdeat_choiceManipulateSelection(Aggregate *object,
                                    MANIPULATOR&    manipulator)
{
    return bdeat_choiceManipulateSelection(&object->aggregateRaw(),
                                           manipulator);
}

template <class ACCESSOR>
inline
int bdeat_choiceAccessSelection(const Aggregate& object,
                                ACCESSOR&             accessor)
{
    return bdeat_choiceAccessSelection(object.aggregateRaw(), accessor);
}

inline
bool bdeat_choiceHasSelection(const Aggregate& object, int selectionId)
{
    return bdeat_choiceHasSelection(object.aggregateRaw(), selectionId);
}

inline
int bdeat_choiceSelectionId(const Aggregate& object)
{
    return bdeat_choiceSelectionId(object.aggregateRaw());
}
}  // close package namespace

// ============================================================================
//           'bdlat_enumfunctions' overloads and specializations
// ============================================================================

namespace bdeat_EnumFunctions {

    // META-FUNCTIONS
    template <>
    struct IsEnumeration<bdlaggxxx::Aggregate> {
        enum { VALUE = 1 };
    };

}  // close namespace bdlat_EnumFunctions

namespace bdlaggxxx {
inline
int bdeat_enumFromInt(Aggregate *result, int enumId)
{
    return bdeat_enumFromInt(&result->aggregateRaw(), enumId);
}

inline
int bdeat_enumFromString(Aggregate *result,
                         const char     *string,
                         int             stringLength)
{
    return bdeat_enumFromString(&result->aggregateRaw(),
                                string,
                                stringLength);
}

inline
void bdeat_enumToInt(int *result, const Aggregate& value)
{
    bdeat_enumToInt(result, value.aggregateRaw());
}

inline
void bdeat_enumToString(bsl::string *result, const Aggregate& value)
{
    bdeat_enumToString(result, value.aggregateRaw());
}
}  // close package namespace

// ============================================================================
//           'bdlat_nullablevaluefunctions' overloads and specializations
// ============================================================================

namespace bdeat_NullableValueFunctions {

    // META-FUNCTIONS
    template <>
    struct IsNullableValue<bdlaggxxx::Aggregate_NullableAdapter> {
        enum { VALUE = 1 };
    };

    template <>
    struct ValueType<bdlaggxxx::Aggregate_NullableAdapter> {
        typedef bdlaggxxx::Aggregate Type;
    };

}  // close namespace bdlat_NullableValueFunctions

namespace bdlaggxxx {
inline
bool bdeat_nullableValueIsNull(const Aggregate_NullableAdapter& object)
{
    return object.d_element_p->isNul2();
}

inline
void bdeat_nullableValueMakeValue(Aggregate_NullableAdapter *object)
{
    object->d_element_p->makeValue();
}

template <class MANIPULATOR>
inline
int bdeat_nullableValueManipulateValue(
                                   Aggregate_NullableAdapter *object,
                                   MANIPULATOR&                    manipulator)
{
    return manipulator(object->d_element_p);
}

template <class ACCESSOR>
inline
int bdeat_nullableValueAccessValue(
                                const Aggregate_NullableAdapter& object,
                                ACCESSOR&                             accessor)
{
    return accessor(*object.d_element_p);
}

// ============================================================================
//                       'bdeat_valuetype' overloads
// ============================================================================

inline
void bdeat_valueTypeReset(Aggregate_NullableAdapter *object)
{
    BSLS_ASSERT_SAFE(object);
    BSLS_ASSERT_SAFE(object->d_element_p);

    object->d_element_p->makeNull();
}
}  // close package namespace

// ============================================================================
//           'bdlat_sequencefunctions' overloads and specializations
// ============================================================================

namespace bdeat_SequenceFunctions {

    // META-FUNCTIONS
    bslmf::MetaInt<1> isSequenceMetaFunction(const bdlaggxxx::Aggregate&);

    template <>
    struct IsSequence<bdlaggxxx::Aggregate> {
        enum { VALUE = 1 };
    };

}  // close namespace bdlat_SequenceFunctions

namespace bdlaggxxx {
template <class MANIPULATOR>
inline
int bdeat_sequenceManipulateAttribute(Aggregate *object,
                                      MANIPULATOR&    manipulator,
                                      const char     *attributeName,
                                      int             attributeNameLength)
{
    return bdeat_sequenceManipulateAttribute(&object->aggregateRaw(),
                                             manipulator,
                                             attributeName,
                                             attributeNameLength);
}

template <class MANIPULATOR>
inline
int bdeat_sequenceManipulateAttribute(Aggregate *object,
                                      MANIPULATOR&    manipulator,
                                      int             attributeId)
{
    return bdeat_sequenceManipulateAttribute(&object->aggregateRaw(),
                                             manipulator,
                                             attributeId);
}

template <class MANIPULATOR>
inline
int bdeat_sequenceManipulateAttributes(Aggregate *object,
                                       MANIPULATOR&    manipulator)
{
    return bdeat_sequenceManipulateAttributes(&object->aggregateRaw(),
                                              manipulator);
}

template <class ACCESSOR>
inline
int bdeat_sequenceAccessAttribute(const Aggregate&  object,
                                  ACCESSOR&              accessor,
                                  const char            *attributeName,
                                  int                    attributeNameLength)
{
    return bdeat_sequenceAccessAttribute(object.aggregateRaw(),
                                         accessor,
                                         attributeName,
                                         attributeNameLength);
}

template <class ACCESSOR>
inline
int bdeat_sequenceAccessAttribute(const Aggregate& object,
                                  ACCESSOR&             accessor,
                                  int                   attributeId)
{
    return bdeat_sequenceAccessAttribute(object.aggregateRaw(),
                                         accessor,
                                         attributeId);
}

template <class ACCESSOR>
inline
int bdeat_sequenceAccessAttributes(const Aggregate& object,
                                   ACCESSOR&             accessor)
{
    return bdeat_sequenceAccessAttributes(object.aggregateRaw(), accessor);
}

inline
bool bdeat_sequenceHasAttribute(const Aggregate&  object,
                                const char            *attributeName,
                                int                    attributeNameLength)
{
    return bdeat_sequenceHasAttribute(object.aggregateRaw(),
                                      attributeName,
                                      attributeNameLength);
}

inline
bool bdeat_sequenceHasAttribute(const Aggregate& object, int attributeId)
{
    return bdeat_sequenceHasAttribute(object.aggregateRaw(), attributeId);
}

// ============================================================================
//                     'bdlat_typecategory' overloads
// ============================================================================

bdeat_TypeCategory::Value
inline
bdeat_typeCategorySelect(const Aggregate& object)
{
    return bdeat_typeCategorySelect(object.aggregateRaw());
}

template <class MANIPULATOR>
inline
int bdeat_typeCategoryManipulateArray(Aggregate *object,
                                      MANIPULATOR&    manipulator)
{
    return bdeat_typeCategoryManipulateArray(&object->aggregateRaw(),
                                             manipulator);
}

template <class MANIPULATOR>
inline
int bdeat_typeCategoryManipulateSimple(Aggregate *object,
                                       MANIPULATOR&    manipulator)
{
    return bdeat_typeCategoryManipulateSimple(&object->aggregateRaw(),
                                              manipulator);
}

template <class ACCESSOR>
inline
int bdeat_typeCategoryAccessArray(const Aggregate& object,
                                  ACCESSOR&             accessor)
{
    return bdeat_typeCategoryAccessArray(object.aggregateRaw(), accessor);
}

template <class ACCESSOR>
inline
int bdeat_typeCategoryAccessSimple(const Aggregate& object,
                                   ACCESSOR&             accessor)
{
    return bdeat_typeCategoryAccessSimple(object.aggregateRaw(), accessor);
}

// ============================================================================
//                       'bdlat_typename' overloads
// ============================================================================

inline
const char *bdeat_TypeName_className(const Aggregate& object)
     // Return the name of the record or enumeration definition for the
     // specified 'object' aggregate or a null pointer of 'object' does not
     // have a named record or enumeration definition.
{
    return bdeat_TypeName_className(object.aggregateRaw());
}

// ============================================================================
//                       'bdeat_valuetype' overloads
// ============================================================================

inline
int bdeat_valueTypeAssign(Aggregate *lhs, const Aggregate& rhs)
{
    BSLS_ASSERT_SAFE(lhs);

    lhs->setValue(rhs);
    return 0;
}

inline
void bdeat_valueTypeReset(Aggregate *object)
{
    bdeat_valueTypeReset(&object->aggregateRaw());
}
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
