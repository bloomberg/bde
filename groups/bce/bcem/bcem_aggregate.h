
// bcem_aggregate.h                  -*-C++-*-
#ifndef INCLUDED_BCEM_AGGREGATE
#define INCLUDED_BCEM_AGGREGATE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a type to reference self-describing data.
//
//@CLASSES:
//  bcem_Aggregate: reference to fully-introspective, dynamically-typed data
//
//@SEE_ALSO: bcem_aggregateraw, bdem_elemtype, bdem_schema, bdem_list, 
//           bdem_table, bdem_choice,
//           bdem_choicearray, bdem_elemref, bdem_convert, 
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a fully-introspective
// data structure, 'bcem_Aggregate', capable of representing a reference to
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
// The 'bcem_Aggregate' type relies heavily on the data structures in the
// 'bdem' package.  The bdem package contains four self-describing data
// containers: 'bdem_List', 'bdem_Choice', 'bdem_Table', and
// 'bdem_ChoiceArray' (often referred to simply as "list", "choice", "table",
// and "choice array" respectively, and collectively known as "bdem aggregate
// types").  Lists and tables contain a (run-time constructed) "row
// definition" consisting essentially of a sequence of 'bdem_ElemType::Type'
// enumerator values.  A 'bdem_List' contains a sequence of data elements of
// heterogeneous type, where each element's type is indicated by a
// corresponding entry in the list's row definition.  A 'bdem_Table' is an
// array of rows, where each row is like a 'bdem_List' except that all rows
// share the same row definition.  A 'bdem_Choice' contains a (run-time
// constructed) catalog of 'bdem_ElemType::Type' values, and a current data
// selection that belongs to one of the types in the catalog.  A
// 'bdem_ChoiceArray' is an array of items where each item is like a
// 'bdem_Choice' except that all items share the same catalog.
//
// A list may contain a field that is itself a list.  However, because fields
// are inserted into lists *by value*, there is no way to cause a list to
// directly or indirectly contain itself.  Similarly, lists can contain
// choices, choices can contain lists, choices can contain choices, table rows
// can contain lists, etc., allowing an arbitrarily complex, tree-like
// structure to be created, but not a cyclic graph.
//
// A record definition ('bdem_RecordDef' in the 'bdem_schema' component)
// provides a higher-level description of the structure of a 'bdem' aggregate
// than is provided by the row definition or catalog.  The record definition
// is a sequence of 'bdem_FieldDef' objects, each of which has a field name,
// (optional) field ID, field type, and other attributes of a field (within a
// list or table) or selection (within a choice or choice array).  For fields
// that describe aggregate types, the field definition may also contain a
// pointer to a record definition for that type.  For fields that describe
// enumerated values, the field definition will contain a pointer to a
// 'bdem_EnumerationDef' object, which maps enumerator name strings to integer
// enumerator IDs.  Thus, a record definition can describe the entire nested
// structure of an aggregate type and provide by-name and by-ID access to its
// fields and selections.  The classes in 'bdem_schema' along with the utility
// functions in 'bdem_schemaaggregateutil' are used to construct and validate
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
///The 'bcem_Aggregate' Abstraction
///--------------------------------
// A 'bcem_Aggregate' can hold a reference to data belonging to any of the
// 'bdem' types along with schema information describing the data: its record
// or enumeration constraint (if any), its default value, and whether or not it
// is nullable.
//
// A 'bcem_Aggregate' exhibits pointer-like semantics with reference-like
// syntax.  The user should think of a 'bcem_Aggregate' as pointing to
// anonymous data, rather than holding the data itself.  The copy constructor
// and assignment operator copy only the pointer, not the data.  Unlike
// pointers, however, methods called on a 'bcem_Aggregate' object generally
// access and manipulate the referenced data (i.e., like a C++ reference),
// without the use of the pointer-dereference operators '*' and '->'.  For
// example, 'a.setValue(x)' sets the object pointed to by aggregate 'a' to a
// new value, 'x'.
//
// Although the mix of pointer-like semantics and reference-like syntax seems
// to have caused very little confusion when copying and accessing
// 'bcem_Aggregate' objects, the same cannot be said for testing
// aggregates for equality.  Pointer-like semantics would imply that
// 'operator==' returns 'true' only if two 'bcem_Aggregate' objects point to
// the same object, whereas the reference-like syntax would imply that
// 'operator==' returns 'true' if the referred-to objects have the same value,
// even if they are distinct objects.  For this reason, 'operator==' was
// entirely removed from this component.  Instead, the caller must explicitly
// choose one of two static methods for equality testing: 'areIdentical'
// returns 'true' only if its arguments point to the same object and
// 'areEquivalent' returns 'true' if the referred-to objects have the same
// value.  If 'areIdentical' returns 'true' for a given pair of
// 'bcem_Aggregate' arguments, then 'areEquivalent' will also return 'true' for
// the same arguments.
//
// Several accessors permit navigating the aggregate structure.  Most
// navigation functions return a 'bcem_Aggregate' representing a part (called
// a "sub-aggregate") of the parent aggregate.  The sub-aggregate points to a
// sub-part of its parent aggregate.  Thus, operations that modify a
// sub-aggregate will modify its parent.  (For the sake of simplicity, a
// 'bcem_Aggregate' always points to a modifiable data structure; there is no
// 'bcem_ConstAggregate'.)
//
// The lifetimes of the data and of the schema referred to by a
// 'bcem_Aggregate' are managed automatically through the use of reference
// counting -- the user does not need to deallocate the referenced object.
// The user also does not need to *allocate* the referenced object -- there
// are constructors that, given a schema, will automatically allocate and
// construct the referenced data using the default values stored in the
// schema.  It is also possible to construct the referenced data outside of
// the 'bcem_Aggregate' and then bind it to a 'bcem_Aggregate' via a
// 'bcema_SharedPtr'.
//
///Error Handling
///--------------
// The structure of a 'bcem_Aggregate' is determined at runtime, often from
// external data (e.g., in an XML schema file).  Thus, there are many
// opportunities for errors to occur, even in code that is logically correct.
// For this reason, 'bcem_Aggregate' is designed to behave in a predictable
// manner when confronted with detectable error conditions (listed below).
// When an error is detected, most methods will return an error in the form of
// a 'bcem_Aggregate' in a special error state, called an "error aggregate".
// An error aggregate is a 'bcem_Aggregate' object that has type 'VOID' and
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
// See 'bcem_AggregateRaw' for a list of error codes and the conditions that
// cause them.  Unless explicitly stated otherwise in the function-level
// documentation, any method that returns a 'bcem_Aggregate' will detect these
// conditions and return the appropriate error aggregate.
//
///Extended Type Conversions
///-------------------------
// Throughout this component, the phrases "is converted" and "is convertible"
// refer to the the "extended type conversion" semantics described in the
// 'bdem_convert' component-level documentation.  Extended type conversion
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
// - Conversions involving a value of type 'bdem_ElemRef' or
//   'bdem_ConstElemRef' are performed by applying the previous rules to the
//   referenced value.
//..
// In addition to the above rules, a 'bcem_Aggregate' is treated, for
// conversion purposes, as the type of value it holds.  Also, setting or
// retrieving an enumeration value as a numeric type will set or return the
// appropriate enumerator ID within the enumeration definition and setting or
// retrieving an enumeration value as a string type will set or return the
// appropriate enumerator name within the enumeration definition.
//
///Null and Default Values
///------------------------------
// Every type described in 'bdem_ElemType' has a corresponding null value.
// For scalar types, this null value comes from the 'bdetu_Unset' component.
// For array types, the null value is the empty array.  For 'bdem' aggregate
// types, the null value consists of a list with zero elements, a table with
// zero rows, a choice with no current selection, or a choice array with zero
// items.  Additionally, a null unconstrained table contains no rows and no
// columns, a null unconstrained choice contains no current selection and an
// empty types catalog, and a null unconstrained choice array contains no
// items and an empty types catalog.
//
// When a new value is created (by constructing a new 'bcem_Aggregate', by
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
// In most cases, 'bcem_Aggregate' will allow transparent access to anonymous
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
// 'makeSelection', 'field', and 'setField'), 'bcem_Aggregate' will find named
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
// (e.g., 'selector', 'selection', etc.), 'bcem_Aggregate' will descend down
// through anonymous fields (if any) until it finds a choice object (if any),
// then it will apply the operation to that choice object.  If a choice object
// cannot be found by traversing anonymous fields, then a
// 'BCEM_ERR_NOT_A_CHOICE' error is returned.  Thus, in our example, the
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
// Then the following expression will return a 'BCEM_ERR_AMBIGUOUS' error:
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
//  A 'bcem_Aggregate' maintains a reference counted handle to possibly-shared
// data. It is not safe to access or modify this shared data concurrently from
// different threads, therefore 'bcem_Aggregate' is, strictly speaking, *thread*
// *unsafe* as two aggregates may refer to the same shared data.  However, it 
// is safe to concurrently access or modify two 'bcem_Aggregate' objects 
// refering to different data. In addition, it is safe to destroy a 
// 'bcem_Aggregate' on one thread, while another thread is accessing or 
// modified the same shared data through another 'bcem_Aggregate' object.
// 
///Usage
///-----
// In this example, we will create a schema for a postal-service address
// record, then create a 'bcem_Aggregate' object that conforms to that schema
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
// We could create the bdem_Schema object from this XML schema, but we'll do
// it manually for this example:
//..
//  bcema_SharedPtr<bdem_Schema> schema(new bdem_Schema);
//
//  bdem_RecordDef *company = schema->createRecord("Company");
//  company->appendField(bdem_ElemType::BDEM_STRING, "Name");
//  company->appendField(bdem_ElemType::BDEM_INT,    "AccountNum");
//
//  bdem_RecordDef *person = schema->createRecord("Person");
//  person->appendField(bdem_ElemType::BDEM_STRING, "LastName");
//  person->appendField(bdem_ElemType::BDEM_STRING, "FirstName");
//  person->appendField(bdem_ElemType::BDEM_DATE,   "BirthDate");
//
//  // The "entity" recordDef describes a choice
//  bdem_RecordDef *entity =
//      schema->createRecord("Entity", bdem_RecordDef::BDEM_CHOICE_RECORD);
//  entity->appendField(bdem_ElemType::BDEM_LIST, company, "Corp");
//  entity->appendField(bdem_ElemType::BDEM_LIST, person,  "Human");
//
//  bdem_RecordDef *address = schema->createRecord("Address");
//  address->appendField(bdem_ElemType::BDEM_STRING,       "StreetAddress");
//  address->appendField(bdem_ElemType::BDEM_STRING,       "City");
//  address->appendField(bdem_ElemType::BDEM_STRING,       "Province");
//  address->appendField(bdem_ElemType::BDEM_STRING,       "Country");
//  address->appendField(bdem_ElemType::BDEM_INT,          "PostalCode");
//  address->appendField(bdem_ElemType::BDEM_CHOICE_ARRAY, entity, "Entities");
//..
// Now we create and populate a simple aggregate object representing a person.
// The fields are accessed by name using the square-bracket ('[]') operator.
// 'setValue' is used to modify an aggregate's value and 'asString' is used to
// retrieve the value as a string (converting to string if necessary).
//..
//  bcem_Aggregate michael(schema, "Person");
//  assert(michael.dataType() == bdem_ElemType::BDEM_LIST);
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
//  michael.setField("BirthDate", bdet_Date(1942, 2, 14));
//  assert(bdet_Date(1942, 2, 14) == michael.field("BirthDate").asDate());
//..
// Getting more sophisticated, we create and populate a schema representing an
// address.  We begin with the simple scalar fields:
//..
//  bcem_Aggregate addr1(schema, "Address");
//  addr1.setField("StreetAddress", "499 Park Ave.");  // Before the move
//  addr1.setField("City", "New York");
//  addr1.setField("Province", "NY");
//  addr1.setField("Country", "USA");
//  addr1.setField("PostalCode", 10022);
//..
// A 'bcem_Aggregate' object does not actually hold data.  Rather it holds a
// smart reference to some other anonymous data, similar to the way a pointer
// holds the address of anonymous data, but without the need for dereferencing
// operators ('*' or '->').  A 'bcem_Aggregate' can also hold a reference to a
// sub-part of the data referenced by another 'bcem_Aggregate'.  To simplify
// access to the "Entities" choice array, retrieve the empty array from
// 'addr1' and keep a reference to this sub-aggregate in another
// 'bcem_Aggregate' object.  'bcem_Aggregate' uses reference counting so that
// the sub-aggregate will remain valid even if the original aggregate goes out
// of scope:
//..
//  bcem_Aggregate entities = addr1.field("Entities");
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
//  bcem_Aggregate addr2 = addr1;
//  assert(bcem_Aggregate::areIdentical(addr1, addr2));
//..
// To create a new aggregate with an independent copy of the data instead of a
// second reference to the same data, use the 'clone' function.  Just as two
// pointers that point to different but equal objects will not compare equal,
// the original 'bcem_Aggregate' and its clone are equivalent, but will not
// compare equal.  To compare the referenced data, use the 'areEquivalent'
// class methods:
//..
//  addr2 = addr1.clone();
//  assert(addr1 != addr2);                               // Not equal...
//  assert(bcem_Aggregate::areEquivalent(addr1, addr2));  // but equivalent
//  assert(&addr1.recordDef() != &addr2.recordDef()); // Different schema addr
//..
// The clone can be shown to be independent by changing a field, thus breaking
// the equivalence relationship.
//..
//  addr2.setField("StreetAddress", "731 Lexington Ave.");  // After the move
//  assert(! bcem_Aggregate::areEquivalent(addr1, addr2));
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
//  assert(bcem_Aggregate::areEquivalent(addr1, addr2));  // but equivalent
//  assert(&addr1.recordDef() == &addr2.recordDef()); // exactly same schema
//..
// It is important to note that the assignment operator always replaces both
// the schema and data of a 'bcem_Aggregate' so that both its structure and
// value may change.  It is thus not possible to modify a sub-aggregate via
// assignment:
//..
//  addr1.setField("Entities", 1, "", "FirstName", "Mike");
//  bcem_Aggregate mike = addr1.field("Entities", 1).selection();
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
//  bcem_Aggregate x(bdem_ElemType::BDEM_STRING, "Michael");
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
//  bcem_Aggregate result = mike.setValue(michael["FirstName"]);
//  assert(result.isError());
//  assert(result.errorCode() == bcem_AggregateError::BCEM_ERR_NON_CONFORMING);
//  bsl::cout << result.errorMessage();
//..
// Modify the data that 'mike' references using 'setValue' with data having
// compatible structure.
//..
//  result = mike.setValue(michael);
//  assert(! result.isError());
//  assert(bcem_Aggregate::areEquivalent(mike, michael));
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
//  assert(result.errorCode() == bcem_AggregateError::BCEM_ERR_BAD_ARRAYINDEX);
//  bsl::cout << result.errorMessage();
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEM_AGGREGATERAW
#include <bcem_aggregateraw.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEM_CHOICE
#include <bdem_choice.h>
#endif

#ifndef INCLUDED_BDEM_CHOICEARRAY
#include <bdem_choicearray.h>
#endif

#ifndef INCLUDED_BDEM_CONVERT
#include <bdem_convert.h>
#endif

#ifndef INCLUDED_BDEM_ELEMATTRLOOKUP
#include <bdem_elemattrlookup.h>
#endif

#ifndef INCLUDED_BDEM_ELEMREF
#include <bdem_elemref.h>
#endif

#ifndef INCLUDED_BDEM_LIST
#include <bdem_list.h>
#endif

#ifndef INCLUDED_BDEM_ROW
#include <bdem_row.h>
#endif

#ifndef INCLUDED_BDEM_SCHEMAUTIL
#include <bdem_schemautil.h>
#endif

#ifndef INCLUDED_BDEM_SELECTBDEMTYPE
#include <bdem_selectbdemtype.h>
#endif

#ifndef INCLUDED_BDEM_TABLE
#include <bdem_table.h>
#endif

#ifndef INCLUDED_BDETU_UNSET
#include <bdetu_unset.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
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

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>   // for 'bsl::pair'
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

class  bdem_ChoiceArrayItem;

class bcem_Aggregate {
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

  private:
    // PRIVATE TYPES
    typedef bcem_AggregateRawNameOrIndex NameOrIndex;  // short hand

    // Data invariants:
    // - If 'd_dataType' is 'bdem_ElemType::BDEM_VOID', then 'd_value' will be
    //   null or point to an error record.  The remaining invariants need not
    //   hold.
    // - If 'd_schema' is null, then both 'd_recordDef' and 'd_fieldDef' are
    //   null.
    // - If 'd_schema' is non-null, then 'd_recordDef' and/or 'd_fieldDef'
    //   are non-null.
    // - 'd_recordDef' is either null or points to a record within 'd_schema.'
    //   Its memory is not managed separately from the schema's.
    // - 'd_fieldDef' is either null or points to a field definition within
    //   'd_schema'.  Its memory is not managed separately from the schema's.
    // - If 'd_fieldDef' is not null, then 'd_fieldDef->elemType()' is equal
    //   to either 'd_dataType' or to 'bdem_ElemType::toArrayType(d_dataType)'.
    //   The code in this class always uses 'd_dataType', not
    //   'd_fieldDef->elemType()'.
    // - If this is the root object, then 'd_fieldDef' will be null, but
    //   'd_recordDef' may still have a value.  Otherwise,
    //   'd_fieldDef->recordConstraint()' will always be equal to
    //   'd_recordDef'.  The code in this class always uses 'd_recordDef', not
    //   'd_fieldDef->recordConstraint()'.

    // DATA
    bcem_AggregateRaw                   d_rawData;
    bcema_SharedPtrRep                 *d_schemaRep_p;    // shared schema
    bcema_SharedPtrRep                 *d_valueRep_p;     // pointer to data
    bcema_SharedPtrRep                 *d_isTopLevelAggregateNullRep_p;
                                                          // nullness
                                                          // indicator for
                                                          // top-level
                                                          // aggregate in bit
                                                          // 0

    // PRIVATE MANIPULATORS
    const bcem_Aggregate fieldImp(
                               bool        makeNonNullFlag,
                               NameOrIndex fieldOrIdx1,
                               NameOrIndex fieldOrIdx2 = NameOrIndex(),
                               NameOrIndex fieldOrIdx3 = NameOrIndex(),
                               NameOrIndex fieldOrIdx4 = NameOrIndex(),
                               NameOrIndex fieldOrIdx5 = NameOrIndex(),
                               NameOrIndex fieldOrIdx6 = NameOrIndex(),
                               NameOrIndex fieldOrIdx7 = NameOrIndex(),
                               NameOrIndex fieldOrIdx8 = NameOrIndex(),
                               NameOrIndex fieldOrIdx9 = NameOrIndex(),
                               NameOrIndex fieldOrIdx10 = NameOrIndex()) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of two to ten 'fieldOrIdx' arguments, each of which
        // specifies a field name or array index.  If this aggregate references
        // a nillable array, the specified field is null, and the specified 
        // 'makeNonNullFlag' is 'true', assign that field its default value; 
        // otherwise leave the field unmodified.  Return a sub-aggregate 
        // referring to the modifiable field on success or an error object on 
        // failure (as described in the "Error Handling" section of the 
        // 'bcem_aggregate' component-level documentation).  An empty string 
        // can be used for any of the 'fieldOrIdx' arguments to specify the
        // current selection within a choice object.  An unused argument 
        // results in the construction of a null 'NameOrIndex', which is 
        // treated as the end of the argument list.

    void init(const bcema_SharedPtr<const bdem_Schema>&     schemaPtr,
              const bdem_RecordDef                         *recordDefPtr,
              bdem_ElemType::Type                           elemType,
              bslma_Allocator                              *basicAllocator);
    void init(const bcema_SharedPtr<const bdem_RecordDef>&  recordDefPtr,
              bdem_ElemType::Type                           elemType,
              bslma_Allocator                              *basicAllocator);
    void init(const bcema_SharedPtr<const bdem_Schema>&     schemaPtr,
              const char                                   *recordName,
              bdem_ElemType::Type                           elemType,
              bslma_Allocator                              *basicAllocator);
        // Initialize this object from the specified record definition and
        // element type.  By default, the constructed object is not null.
        // The behavior is undefined if this method is invoked on an
        // already-initialized Aggregate.

    // PRIVATE ACCESSORS
    const bcem_Aggregate makeError(
                            const bcem_AggregateError& errorDescription) const;
    const bcem_Aggregate makeError(bcem_AggregateError::Code errorCode, 
                                   const char *msg, ...) const
#ifdef BSLS_PLATFORM__CMP_GNU
        // Declare this function as printf-like in gcc.
        // The 'msg' arg is the 3rd arg, including the implicit 'this'.
        __attribute__ ((__format__ (__printf__, 3, 4)))
#endif
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
    static bcema_SharedPtr<void> makeValuePtr(
                                    bdem_ElemType::Type  type,
                                    bslma_Allocator     *basicAllocator = 0);
        // Return a shared pointer to a newly-created unset value of the
        // specified 'type'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    template <typename TYPE>
    static const TYPE&         valueRef(const TYPE& value);
    static bdem_ElemRef        valueRef(const bcem_Aggregate& value);
        // Return the specified 'value', unless 'value' is bcem_Aggregate, 
        // in which case return 'value.asElemRef()'.  This method facilitates
        // passing values to AggregateRaw template methods for assignment.

    // PRIVATE CREATORS
    bcem_Aggregate(const bcem_AggregateRaw&     rawData, 
                   bcema_SharedPtrRep          *schemaRep,
                   bcema_SharedPtrRep          *valueRep,
                   bcema_SharedPtrRep          *isTopLevelAggregateNullRep);
       // Create a bcem_Aggregate holding a counted reference to the same 
       // schema and data as the specified 'rawData' object.  Increment 
       // and maintain the refererence count using the specified 'schemaRep'
       // for the shared schema reference, the specified 'valueRep' for 
       // the shared data reference, and the specified
       // 'isTopLevelAggregateNullRep' for the top-level nullness bit.  
    
  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bcem_Aggregate,
                                  bslalg_TypeTraitBitwiseMoveable,
                                  bdeu_TypeTraitHasPrintMethod);
        // Note that this class does not have the
        // 'bslalg_TypeTraitUsesBslmaAllocator' trait.  Although some
        // constructors do take an allocator, the pointer-like semantics do not
        // fully allow the use of the allocator idioms.

    // PUBLIC TYPES
    enum {
        // Navigation status codes when descending into a field or array item.
        // Values are large negative integers not equal to 'INT_MIN' so as not
        // to be confused with valid return values such as -1 or
        // 'bdet_Null<int>::unsetValue()'.
        BCEM_ERR_UNKNOWN_ERROR = INT_MIN + 1,
        BCEM_ERR_NOT_A_RECORD,    // Aggregate is not a sequence or choice.
        BCEM_ERR_NOT_A_SEQUENCE,  // Aggregate is not a sequence.
        BCEM_ERR_NOT_A_CHOICE,    // Aggregate is not a choice.
        BCEM_ERR_NOT_AN_ARRAY,    // Aggregate is not an array.
        BCEM_ERR_BAD_FIELDNAME,   // Field name does not exist in record.
        BCEM_ERR_BAD_FIELDID,     // Field ID does not exist in record.
        BCEM_ERR_BAD_FIELDINDEX,  // Field index does not exist in record.
        BCEM_ERR_BAD_ARRAYINDEX,  // Array index is out of bounds.
        BCEM_ERR_NOT_SELECTED,    // Field does not match current selection.
        BCEM_ERR_BAD_CONVERSION,  // Cannot convert value.
        BCEM_ERR_BAD_ENUMVALUE,   // Invalid enumerator value
        BCEM_ERR_NON_CONFORMANT,  // Value does not conform to the schema.
        BCEM_ERR_AMBIGUOUS_ANON   // Anonymous field reference is ambiguous.
    };

    // CLASS METHODS
    static
    bool areIdentical(const bcem_Aggregate& lhs, const bcem_Aggregate& rhs);
        // Return 'true' if the specified 'lhs' aggregate refers to the same
        // object as the specified 'rhs' aggregate and 'false' otherwise.
        // Specifically, 'lhs' and 'rhs' are considered identical if 'dataType'
        // and 'dataPtr' return equal values for the respective arguments.
        // When two 'bcem_Aggregate' objects compare identical, modifications
        // to one of their referred-to object will be visible through the
        // other.  This predicate is roughly equivalent to comparing two
        // pointers for equality.  Note that if
        // 'bdem_ElemType::BDEM_VOID == lhs.dataType()' or
        // 'bdem_ElemType::BDEM_VOID == rhs.dataType()', then 'false' is
        // returned.

    static
    bool areEquivalent(const bcem_Aggregate& lhs, const bcem_Aggregate& rhs);
        // Return 'true' if the value of the object referenced by the specified
        // 'lhs' aggregate equals the value of the object referenced by the
        // specified 'rhs' aggregate.  The aggregates need not be identical (as
        // per 'areIdentical'), but must have equivalent record definitions and
        // equal data and nullness values.  This predicate is roughly
        // equivalent to dereferencing two pointers and comparing their
        // pointed-to values for equality.  'areIdentical(lhs, rhs)' implies
        // 'areEquivalent(lhs, rhs)', but not vice versa.
        // TBD: Update for nillable scalar arrays

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported
        // by this class.  See the BDE package-group-level documentation for
        // more information on 'bdex' streaming of container types.

    // CREATORS
    bcem_Aggregate();
        // Create an empty aggregate with no schema or data.  Note that after
        // construction 'isNul2()' returns 'true'.

    template <typename VALTYPE>
    bcem_Aggregate(const bdem_ElemType::Type  dataType,
                   const VALTYPE&             value,
                   bslma_Allocator           *basicAllocator = 0);
        // Create an aggregate representing a value of the type specified
        // in 'dataType' and initialize it to the specified 'value'.  If
        // 'dataType' is a 'bdem' aggregate type, then the constructed object
        // will be unconstrained, i.e., it will have no record definition and
        // therefore no by-name or by-ID field access.  The constructed
        // aggregate will be in an error state if 'value' is not convertible
        // to the type specified by 'dataType'.  (See "Extended Type
        // Conversions" and "Error Handling" in the 'bcem_Aggregate'
        // component-level documentation for a detailed definition of "is
        // convertible".)  Note that after construction, 'isNul2()' returns
        // 'false'.

    // SHARED-POINTER CONSTRUCTORS
    // The following constructors initialize the aggregate's structure and
    // data by sharing references to the arguments provided by the client.  If
    // a schema pointer or record definition pointer is provided without value
    // data, then only the schema is shared with the client; the value data is
    // allocated and given a default non-null value.  (See "Null Values and
    // Default Values" in the 'bcem_aggregate' component-level documentation
    // for a more complete description of default values.)

    explicit bcem_Aggregate(
        const bcema_SharedPtr<const bdem_RecordDef>&  recordDefPtr,
        bslma_Allocator                              *basicAllocator = 0);
    explicit bcem_Aggregate(
        const bcema_SharedPtr<bdem_RecordDef>&        recordDefPtr,
        bslma_Allocator                              *basicAllocator = 0);
        // Create an aggregate containing a list or choice object
        // structured according to the record definition pointed to by the
        // specified 'recordDefPtr'.  If
        // 'BDEM_SEQUENCE_RECORD == recordDefPtr->recordType()', create a
        // sequence (list) of fields, each initialized with its default value
        // (recursively for constrained list elements).  If
        // 'BDEM_CHOICE_RECORD == recordDefPtr->recordType()', create a choice
        // aggregate with no current selection.  (See "Null Values and Default
        // Values" in the 'bcem_Aggregate' component-level documentation for a
        // more complete description of default values.)  The new aggregate
        // retains a reference to the shared record definition.  The behavior
        // is undefined if any part of the record definition's parent schema
        // is modified during the lifetime of this aggregate.  Note that after
        // construction, 'isNul2()' returns 'false'.

    bcem_Aggregate(
        const bcema_SharedPtr<const bdem_RecordDef>&  recordDefPtr,
        bdem_ElemType::Type                           elemType,
        bslma_Allocator                              *basicAllocator = 0);
    bcem_Aggregate(
        const bcema_SharedPtr<bdem_RecordDef>&        recordDefPtr,
        bdem_ElemType::Type                           elemType,
        bslma_Allocator                              *basicAllocator = 0);
        // Create an aggregate containing a list, table, choice, or
        // choice-array object structured according to the record definition
        // specified by 'recordDefPtr'.  The new aggregate retains a
        // reference to the shared record definition.  If 'elemType' is
        // 'LIST', initialize each element of the list according to its
        // default value (recursively for constrained lists and choices).  If
        // 'elemType' is 'CHOICE', initialize the choice object to have no
        // current selection.  If 'elemType' is 'TABLE' or 'CHOICE_ARRAY',
        // initialize the object to have no rows or items.  (See "Null Values
        // and Default Values" in the 'bcem_Aggregate' component-level
        // documentation for a more complete description of default values.)
        // The constructed aggregate will be in an error state (see "Error
        // Handling" in the 'bcem_Aggregate' component-level documentation)
        // unless 'elemType' is 'LIST', 'TABLE', 'CHOICE', or 'CHOICE_ARRAY'
        // and 'recordDefPtr->recordType()' is 'BDEM_SEQUENCE_RECORD' for an
        // 'elemType' of 'LIST' or 'TABLE' and 'recordDefPtr->recordType()' is
        // 'BDEM_CHOICE_RECORD' for an 'elemType' of 'CHOICE' or
        // 'CHOICE_ARRAY'.  The behavior is undefined if any part of the
        // record definition's parent schema is modified during the lifetime
        // of this aggregate.  Note that after construction, 'isNul2()' returns
        // 'false'.

    bcem_Aggregate(
 const bcema_SharedPtr<const bdem_Schema>& schemaPtr,
 const bsl::string&                        recordName,
 bdem_ElemType::Type                       elemType = bdem_ElemType::BDEM_VOID,
 bslma_Allocator                          *basicAllocator = 0);
    bcem_Aggregate(
   const bcema_SharedPtr<bdem_Schema>&     schemaPtr,
   const bsl::string&                      recordName,
   bdem_ElemType::Type                     elemType = bdem_ElemType::BDEM_VOID,
   bslma_Allocator                        *basicAllocator = 0);
        // Create an aggregate containing a list, table, choice, or
        // choice-array object structured according to the record definition
        // identified by the specified 'recordName' within the schema pointed
        // to by specified 'schemaPtr'.  The new aggregate retains a reference
        // to the shared record definition.  If the specified 'elemType' is
        // 'VOID', then the element type is set to 'LIST' or 'CHOICE',
        // depending on whether the referenced record definition is a
        // 'BDEM_SEQUENCE_RECORD' or 'BDEM_CHOICE_RECORD', respectively.  If
        // the element type is 'LIST', each element of the list is initialized
        // according to its default value (recursively for constrained lists
        // and choices).  If the element type is 'CHOICE', initialize the
        // choice object to have no current selection.  If the element type is
        // 'TABLE' or 'CHOICE_ARRAY', initialize the object to have no rows or
        // items.  (See "Null Values and Default Values" in the
        // 'bcem_aggregate' component-level documentation for a more complete
        // description of default values.)  The constructed aggregate will be
        // in an error state (see "Error Handling" in the 'bcem_Aggregate'
        // component-level documentation) unless 'elemType' is 'VOID', 'LIST',
        // 'TABLE', 'CHOICE', or 'CHOICE_ARRAY' and
        // 'schemaPtr->lookupRecord(recordName)->recordType()' is
        // 'BDEM_SEQUENCE_RECORD' for an element type of 'LIST' or 'TABLE' and
        // 'schemaPtr->lookupRecord(recordName)->recordType()' is
        // 'BDEM_CHOICE_RECORD' for an element type of 'CHOICE' or
        // 'CHOICE_ARRAY'.  The behavior is undefined if the schema is
        // modified during the lifetime of this aggregate.  Note that after
        // construction, 'isNul2()' returns 'false'.

    bcem_Aggregate(const bcem_Aggregate& original);
        // Create a new aggregate that refers to the same data and schema
        // as the specified 'original' aggregate.  This creates a new reference
        // to existing data -- no data is copied.  After the construction,
        // 'areIdentical(*this, original)' will return 'true'.  Note that,
        // unlike most BDE types, the allocator for this aggregate will change
        // to be the same as the allocator for 'other'.  Note that
        // copy-constructing an error aggregate will yield an identical error
        // aggregate.

    ~bcem_Aggregate();
        // Destroy this object.  If this is the last aggregate referring to a
        // schema, then the schema is destroyed and deallocated.  If this is
        // the last aggregate referring to a data object, then the data object
        // is also destroyed and deallocated.

    // MANIPULATORS
    bcem_Aggregate& operator=(const bcem_Aggregate& rhs);
        // Make this aggregate refer to the same data and schema as the
        // specified 'rhs' aggregate and return a modifiable reference to this
        // aggregate.  This creates a new reference to existing data -- no
        // data is copied.  The reference counts on the previously-referenced
        // data and schema are decremented, possibly causing either or both to
        // be destroyed and deallocated.  After the assignment,
        // 'areIdentical(*this, rhs)' will return 'true'.  Note that, unlike
        // most BDE types, the allocator for this aggregate will change to be
        // the same as the allocator for 'rhs'.  Note that if 'rhs' is an
        // error aggregate, then this aggregate will be assigned the same
        // error state.

    const bcem_Aggregate reserveRaw(bsl::size_t numItems);
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

    const bcem_Aggregate& reset();
        // Reset this object to the void aggregate ('BDEM_VOID' data type, no
        // schema).  Decrement the reference counts of the previous schema and
        // data (possibly causing them to be destroyed).  This aggregate will
        // never be in an error state after a call to 'reset'.  After the
        // function call, 'isNul2()' returns 'true'.

    // REFERENCED-VALUE MANIPULATORS

    // The following set of methods are technically accessors in that they
    // do not modify the 'bcem_Aggregate' on which they are called.  However,
    // they do manipulate the referenced value.

    bcema_SharedPtr<void> dataPtr() const;
        // Return a shared pointer to the value referenced by this aggregate.
        // The behavior is undefined if the returned pointer is used to modify
        // the data in such a way that it no longer conforms to the schema or
        // if the aggregate is currently null.  The result of calling
        // 'dataPtr' on an error aggregate is unspecified.

    const bcem_Aggregate& makeNull() const;
        // Set the object referenced by this aggregate to null and return a
        // reference to this aggregate.  If the object is null before the
        // call, then do nothing.  The object's type and constraints are
        // unchanged.

    template <typename VALTYPE>
    const bcem_Aggregate setValue(const VALTYPE& value) const;
        // Set the value referenced by this aggregate to the specified
        // 'value', resetting its nullness flag if 'isNul2()' is 'true',
        // converting 'value' as necessary (see "Extended Type Conversions" in
        // the 'bcem_Aggregate' component-level documentation for a detailed
        // definition of "is convertible") and return this aggregate.  If
        // value is null then make this aggregate null.  Leave this
        // aggregate unchanged and return an error if 'value' is not
        // convertible to the type stored in this aggregate.  The schema
        // (dynamic type) of this aggregate is not changed.  Return this
        // aggregate unmodified if this aggregate was in an error state before
        // calling 'setValue'.

    template <typename VALTYPE>
    const bcem_Aggregate setField(NameOrIndex    fieldOrIdx1,
                                  const VALTYPE& value) const;
    template <typename VALTYPE>
    const bcem_Aggregate setField(NameOrIndex    fieldOrIdx1,
                                  NameOrIndex    fieldOrIdx2,
                                  const VALTYPE& value) const;
    template <typename VALTYPE>
    const bcem_Aggregate setField(NameOrIndex    fieldOrIdx1,
                                  NameOrIndex    fieldOrIdx2,
                                  NameOrIndex    fieldOrIdx3,
                                  const VALTYPE& value) const;
    template <typename VALTYPE>
    const bcem_Aggregate setField(NameOrIndex    fieldOrIdx1,
                                  NameOrIndex    fieldOrIdx2,
                                  NameOrIndex    fieldOrIdx3,
                                  NameOrIndex    fieldOrIdx4,
                                  const VALTYPE& value) const;
    template <typename VALTYPE>
    const bcem_Aggregate setField(NameOrIndex    fieldOrIdx1,
                                  NameOrIndex    fieldOrIdx2,
                                  NameOrIndex    fieldOrIdx3,
                                  NameOrIndex    fieldOrIdx4,
                                  NameOrIndex    fieldOrIdx5,
                                  const VALTYPE& value) const;
    template <typename VALTYPE>
    const bcem_Aggregate setField(NameOrIndex    fieldOrIdx1,
                                  NameOrIndex    fieldOrIdx2,
                                  NameOrIndex    fieldOrIdx3,
                                  NameOrIndex    fieldOrIdx4,
                                  NameOrIndex    fieldOrIdx5,
                                  NameOrIndex    fieldOrIdx6,
                                  const VALTYPE& value) const;
    template <typename VALTYPE>
    const bcem_Aggregate setField(NameOrIndex    fieldOrIdx1,
                                  NameOrIndex    fieldOrIdx2,
                                  NameOrIndex    fieldOrIdx3,
                                  NameOrIndex    fieldOrIdx4,
                                  NameOrIndex    fieldOrIdx5,
                                  NameOrIndex    fieldOrIdx6,
                                  NameOrIndex    fieldOrIdx7,
                                  const VALTYPE& value) const;
    template <typename VALTYPE>
    const bcem_Aggregate setField(NameOrIndex    fieldOrIdx1,
                                  NameOrIndex    fieldOrIdx2,
                                  NameOrIndex    fieldOrIdx3,
                                  NameOrIndex    fieldOrIdx4,
                                  NameOrIndex    fieldOrIdx5,
                                  NameOrIndex    fieldOrIdx6,
                                  NameOrIndex    fieldOrIdx7,
                                  NameOrIndex    fieldOrIdx8,
                                  const VALTYPE& value) const;
    template <typename VALTYPE>
    const bcem_Aggregate setField(NameOrIndex    fieldOrIdx1,
                                  NameOrIndex    fieldOrIdx2,
                                  NameOrIndex    fieldOrIdx3,
                                  NameOrIndex    fieldOrIdx4,
                                  NameOrIndex    fieldOrIdx5,
                                  NameOrIndex    fieldOrIdx6,
                                  NameOrIndex    fieldOrIdx7,
                                  NameOrIndex    fieldOrIdx8,
                                  NameOrIndex    fieldOrIdx9,
                                  const VALTYPE& value) const;
    template <typename VALTYPE>
    const bcem_Aggregate setField(NameOrIndex    fieldOrIdx1,
                                  NameOrIndex    fieldOrIdx2,
                                  NameOrIndex    fieldOrIdx3,
                                  NameOrIndex    fieldOrIdx4,
                                  NameOrIndex    fieldOrIdx5,
                                  NameOrIndex    fieldOrIdx6,
                                  NameOrIndex    fieldOrIdx7,
                                  NameOrIndex    fieldOrIdx8,
                                  NameOrIndex    fieldOrIdx9,
                                  NameOrIndex    fieldOrIdx10,
                                  const VALTYPE& value) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of one to ten 'fieldOrIdx' arguments, each of which
        // specifies a field name or array index, then set that field to the
        // specified 'value', resetting its nullness flag if 'field.isNul2()'
        // is 'true', after appropriate conversions (see "Extended Type
        // Conversions" in the 'bcem_Aggregate' component-level
        // documentation).  Return a sub-aggregate referring to the modified
        // field on success or an error object on failure (as described in the
        // "Error Handling" section of the 'bcem_aggregate' component-level
        // documentation).  An empty string can be used for any of the
        // 'fieldOrIdx' arguments to specify the current selection within a
        // choice object.  If value is null then make the field null.  Note
        // that if any field in the chain of fields is null then an error is
        // returned.  This aggregate is not modified if an error is detected.

    const bcem_Aggregate setFieldNull(NameOrIndex fieldOrIdx1) const;
    const bcem_Aggregate setFieldNull(NameOrIndex fieldOrIdx1,
                                      NameOrIndex fieldOrIdx2) const;
    const bcem_Aggregate setFieldNull(NameOrIndex fieldOrIdx1,
                                      NameOrIndex fieldOrIdx2,
                                      NameOrIndex fieldOrIdx3) const;
    const bcem_Aggregate setFieldNull(NameOrIndex fieldOrIdx1,
                                      NameOrIndex fieldOrIdx2,
                                      NameOrIndex fieldOrIdx3,
                                      NameOrIndex fieldOrIdx4) const;
    const bcem_Aggregate setFieldNull(NameOrIndex fieldOrIdx1,
                                      NameOrIndex fieldOrIdx2,
                                      NameOrIndex fieldOrIdx3,
                                      NameOrIndex fieldOrIdx4,
                                      NameOrIndex fieldOrIdx5) const;
    const bcem_Aggregate setFieldNull(NameOrIndex fieldOrIdx1,
                                      NameOrIndex fieldOrIdx2,
                                      NameOrIndex fieldOrIdx3,
                                      NameOrIndex fieldOrIdx4,
                                      NameOrIndex fieldOrIdx5,
                                      NameOrIndex fieldOrIdx6) const;
    const bcem_Aggregate setFieldNull(NameOrIndex fieldOrIdx1,
                                      NameOrIndex fieldOrIdx2,
                                      NameOrIndex fieldOrIdx3,
                                      NameOrIndex fieldOrIdx4,
                                      NameOrIndex fieldOrIdx5,
                                      NameOrIndex fieldOrIdx6,
                                      NameOrIndex fieldOrIdx7) const;
    const bcem_Aggregate setFieldNull(NameOrIndex fieldOrIdx1,
                                      NameOrIndex fieldOrIdx2,
                                      NameOrIndex fieldOrIdx3,
                                      NameOrIndex fieldOrIdx4,
                                      NameOrIndex fieldOrIdx5,
                                      NameOrIndex fieldOrIdx6,
                                      NameOrIndex fieldOrIdx7,
                                      NameOrIndex fieldOrIdx8) const;
    const bcem_Aggregate setFieldNull(NameOrIndex fieldOrIdx1,
                                      NameOrIndex fieldOrIdx2,
                                      NameOrIndex fieldOrIdx3,
                                      NameOrIndex fieldOrIdx4,
                                      NameOrIndex fieldOrIdx5,
                                      NameOrIndex fieldOrIdx6,
                                      NameOrIndex fieldOrIdx7,
                                      NameOrIndex fieldOrIdx8,
                                      NameOrIndex fieldOrIdx9) const;
    const bcem_Aggregate setFieldNull(NameOrIndex fieldOrIdx1,
                                      NameOrIndex fieldOrIdx2,
                                      NameOrIndex fieldOrIdx3,
                                      NameOrIndex fieldOrIdx4,
                                      NameOrIndex fieldOrIdx5,
                                      NameOrIndex fieldOrIdx6,
                                      NameOrIndex fieldOrIdx7,
                                      NameOrIndex fieldOrIdx8,
                                      NameOrIndex fieldOrIdx9,
                                      NameOrIndex fieldOrIdx10) const;
        // Set the field within this aggregate reached through the specified
        // chain of one to ten 'fieldOrIdx' arguments, each of which specifies
        // a field name or array index, to null.  Return a sub-aggregate
        // referring to the field on success or an error object on failure (as
        // described in the "Error Handling" section of the 'bcem_aggregate'
        // component-level documentation).  An empty string can be used for
        // any of the 'fieldOrIdx' arguments to specify the current selection
        // within a choice object.  Note that if any field in the chain of
        // fields is null then an error is returned.  This aggregate is not
        // modified if an error is detected.

    template <typename VALTYPE>
    const bcem_Aggregate setFieldById(int fieldId, const VALTYPE& value) const;
        // Set the value of the field in this aggregate selected by the
        // specified 'fieldId' to the specified 'value', resetting its
        // nullness flag if 'field.isNul2()' returns 'true', after appropriate
        // conversions (see "Extended Type Conversions" in the
        // 'bcem_Aggregate' component-level documentation).  Return a
        // sub-aggregate referring to the modified field on success or an error
        // object on failure (as described in the "Error Handling" section of
        // the 'bcem_aggregate' component-level documentation).  If
        // value is null then make the field null.  Note that if
        // 'true == isNul2()' then an error is returned.  This aggregate is
        // not modified if an error is detected.

    const bcem_Aggregate setFieldNullById(int fieldId) const;
        // Set the field in this aggregate selected by the specified 'fieldId'
        // to null.  Return a sub-aggregate referring to the modified field on
        // success or an error object on failure (as described in the "Error
        // Handling" section of the 'bcem_aggregate' component-level
        // documentation).  Note that if 'true == isNul2()' then an error is
        // returned.  This aggregate is not modified if an error is detected.

    template <typename VALTYPE>
    const bcem_Aggregate setFieldByIndex(int            fieldIndex,
                                         const VALTYPE& value) const;
        // Set the value of the field in this aggregate selected by the
        // specified 'fieldIndex' to the specified 'value', resetting its
        // nullness flag if 'field.isNul2()' is 'true', after appropriate
        // conversions (see "Extended Type Conversions" in the
        // 'bcem_Aggregate' component-level documentation).  Return a
        // sub-aggregate referring to the modified field on success or an error
        // object on failure (as described in the "Error Handling" section of
        // the 'bcem_aggregate' component-level documentation).  If
        // value is null then make the field null.  Note that if
        // 'true == isNul2()' then an error is returned.  This aggregate is not
        // modified if an error is detected.

    const bcem_Aggregate setFieldNullByIndex(int fieldIndex) const;
        // Set the value of the field in this aggregate selected by the
        // specified 'fieldIndex' to null.  Return a sub-aggregate referring
        // to the modified field on success or an error object on failure (as
        // described in the "Error Handling" section of the 'bcem_aggregate'
        // component-level documentation).  Note that if 'true == isNul2()'
        // then an error is returned.  This aggregate is not modified if an
        // error is detected.

    template <typename VALTYPE>
    const bcem_Aggregate setItem(int index, const VALTYPE& value) const;
        // Set the value in this aggregate of the scalar array item, table
        // row, or choice array item selected by the specified 'index' to the
        // specified 'value', resetting its nullness flag if 'item.isNul2()'
        // is 'true', after appropriate conversions (see "Extended Type
        // Conversions" in the 'bcem_Aggregate' component-level
        // documentation).  Return a sub-aggregate referring to the modified
        // item on success or an error object on failure (as described in the
        // "Error Handling" section of the 'bcem_aggregate' component-level
        // documentation).  If value is null then make the item null.  Note
        // that if 'true == isNul2()' then an error is returned.  This
        // aggregate is not modified if an error is detected.

    const bcem_Aggregate setItemNull(int index) const;
        // Set the value in this aggregate of the scalar array item, table
        // row, or choice array item selected by the specified 'index' to
        // null.  Return a sub-aggregate referring to the modified item on
        // success or an error object on failure (as described in the "Error
        // Handling" section of the 'bcem_aggregate' component-level
        // documentation).  Note that if 'true == isNul2()' then an error is
        // returned.  This aggregate is not modified if an error is detected.

    template <typename VALTYPE>
    const bcem_Aggregate append(const VALTYPE& newItem) const;
        // Append a copy of 'newItem' to the end of the scalar array, table or
        // choice array referenced by this aggregate resetting the nullness
        // flag if 'isNul2()' is 'true'.  Return a sub-aggregate referring to
        // the modifiable newly-inserted item on success or an error object on
        // failure (as described in the "Error Handling" section of the
        // 'bcem_aggregate' component-level documentation).  If
        // value is null then append a null item.  This aggregate is not
        // modified if an error is detected.
        //
        // Note that this function is logically equivalent to:
        //..
        //  this->insert(length(), newItem);
        //..

    const bcem_Aggregate appendNull() const;
        // Append a null item to the end of the scalar array, table or choice
        // array referenced by this aggregate resetting the nullness flag if
        // 'isNul2()' is 'true'.  Return a sub-aggregate referring to the
        // modifiable newly-inserted item on success or an error object on
        // failure (as described in the "Error Handling" section of the
        // 'bcem_aggregate' component-level documentation).
        // This aggregate is not modified if an error is detected.
        //
        // Note that this function is logically equivalent to:
        //..
        //  this->insertNull(length());
        //..

    template <typename VALTYPE>
    const bcem_Aggregate insert(int pos, const VALTYPE& newItem) const;
        // Insert a copy of the specified 'newItem' at the specified 'pos'
        // index in the scalar array, table, or choice array referenced by
        // this aggregate.  Return a sub-aggregate referring to the modifiable
        // newly-inserted item on success or an error object on failure (as
        // described in the "Error Handling" section of the 'bcem_aggregate'
        // component-level documentation).  If 'true == isNul2()' reset the
        // nullness flag if '0 == pos' and return an error otherwise.  If
        // newItem is null then insert a null item.  This aggregate is not
        // modified if an error is detected.

    const bcem_Aggregate insertNull(int pos) const;
        // Insert a null item at the specified 'pos' index in the scalar array,
        // table, or choice array referenced by this aggregate.  Return a
        // sub-aggregate referring to the modifiable newly-inserted item on
        // success or an error object on failure (as described in the "Error
        // Handling" section of the 'bcem_aggregate' component-level
        // documentation).  If 'true == isNul2()' reset the nullness flag if
        // '0 == pos' and return an error otherwise.  This aggregate is not
        // modified if an error is detected.

    const bcem_Aggregate resize(int newSize) const;
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

    const bcem_Aggregate appendItems(int numItems) const;
        // Append 'numItems' new elements to the end of the scalar array,
        // table, or choice array referenced by this aggregate resetting the
        // nullness flag if 'isNul2()' returns 'true'.  The new items are set
        // to their default values, as specified in the schema (recursively
        // for the fields within inserted table rows).  Return this aggregate
        // on success or an error object on failure (as described in the
        // "Error Handling" section of the 'bcem_aggregate' component-level
        // documentation).  This aggregate is not modified if an error is
        // detected.
        //
        // This function is logically equivalent to:
        //..
        //  this->insertItems(length(), numItems);
        //..

    const bcem_Aggregate appendNullItems(int numItems) const;
        // Append 'numItems' null elements to the end of the scalar array,
        // table, or choice array referenced by this aggregate resetting the
        // nullness flag if 'isNul2()' returns 'true'.  The new items are set
        // to their default values, as specified in the schema (recursively
        // for the fields within inserted table rows).  Return this aggregate
        // on success or an error object on failure (as described in the
        // "Error Handling" section of the 'bcem_aggregate' component-level
        // documentation).  This aggregate is not modified if an error is
        // detected.
        //
        // This function is logically equivalent to:
        //..
        //  this->insertNullItems(length(), numItems);
        //..

    const bcem_Aggregate insertItems(int pos, int numItems) const;
        // Insert 'numItems' new elements before the specified 'pos' index in
        // the scalar array, table, or choice array referenced by this
        // aggregate.  The new items are set to their default values, as
        // specified in the schema (recursively for the fields within inserted
        // table rows).  Return this aggregate on success or an error object
        // on failure (as described in the "Error Handling" section of the
        // 'bcem_aggregate' component-level documentation).  If
        // 'true == isNul2()' reset the nullness flag if '0 == pos' and return
        // an error otherwise.  This aggregate is not modified if an error is
        // detected.

    const bcem_Aggregate insertNullItems(int pos, int numItems) const;
        // Insert 'numItems' null elements before the specified 'pos' index in
        // the scalar array, table, or choice array referenced by this
        // aggregate.  Return this aggregate on success or an error object on
        // failure (as described in the "Error Handling" section of the
        // 'bcem_aggregate' component-level documentation).  If
        // 'true == isNul2()' reset the nullness flag if '0 == pos' and return
        // an error otherwise.  This aggregate is not modified if an error is
        // detected.

    const bcem_Aggregate remove(int pos) const;
        // Remove the item at the specified 'pos' index in the scalar array,
        // choice array, or table referenced by this aggregate.  Return this
        // aggregate on success or an error object on failure (as described in
        // the "Error Handling" section of the 'bcem_aggregate'
        // component-level documentation).  If 'true == isNul2()' return an
        // error.  This aggregate is not modified if an error is detected.

    const bcem_Aggregate removeItems(int pos, int numItems) const;
        // Remove the specified 'numItems' items starting at the specified
        // 'pos' index in the scalar array, choice array, or table referenced
        // by this aggregate.  Return this aggregate on success or an error
        // object on failure (as described in the "Error Handling" section of
        // the 'bcem_aggregate' component-level documentation).  If
        // 'true == isNul2()' return an error.  This aggregate is not modified
        // if an error is detected.

    const bcem_Aggregate removeAllItems() const;
        // Remove all items from the scalar array, choice array, or table
        // referenced by this aggregate.  Return this aggregate on success or
        // an error object on failure (as described in the "Error Handling"
        // section of the 'bcem_aggregate' component-level documentation).  If
        // 'true == isNul2()' return an error.  This aggregate is not modified
        // if an error is detected.
        //
        // This function is logically equivalent to:
        //..
        //  this->removeItems(0, length());
        //..

    const bcem_Aggregate makeSelection(const char         *newSelector) const;
    const bcem_Aggregate makeSelection(const bsl::string&  newSelector) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelector' resetting the
        // nullness flag if 'isNul2()' returns 'true'.  If 'newSelector'
        // is null or the empty string then the selection is reset.  The newly
        // selected sub-object is initialized to its default value.  (See "Null
        // Values and Default Values" in the 'bcem_Aggregate' component-level
        // documentation for a more complete description of default values.)
        // Return a sub-aggregate referring to the modifiable selection on
        // success or an error object on failure (as described in the "Error
        // Handling" section of the 'bcem_aggregate' component-level
        // documentation).  This aggregate is not modified if an error is
        // detected.

    template <typename VALTYPE>
    const bcem_Aggregate makeSelection(const char        *newSelector,
                                       const VALTYPE&     value) const;
    template <typename VALTYPE>
    const bcem_Aggregate makeSelection(const bsl::string& newSelector,
                                       const VALTYPE&     value) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelector' and set the newly
        // selected sub-object to 'value', resetting the nullness flag if
        // 'isNul2()' returns 'true', after appropriate conversions (see
        // "Extended Type Conversions" in the 'bcem_Aggregate' component-level
        // documentation).  If 'newSelector' is null or the empty string then
        // the selection is reset.  Return a sub-aggregate referring to the
        // modifiable selection on success or an error object on failure (as
        // described in the "Error Handling" section of the 'bcem_aggregate'
        // component-level documentation).  If value is null then the
        // selected aggregate is made null.  If 'newSelector' is valid but
        // 'value' is not convertible to the newly-selected sub-object type,
        // then the sub-object is initialized to its default value, otherwise
        // this aggregate is not modified when an error is detected.

    const bcem_Aggregate makeSelectionById(int newSelectorId) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelectorId' resetting the
        // nullness flag if 'isNul2()' returns 'true'.  If 'newSelectorId' is
        // 'bdem_RecordDef::BDEM_NULL_FIELD_ID' then the selection is reset.
        // The newly selected sub-object is initialized to its default
        // value. (See "Null Values and Default Values" in the 'bcem_Aggregate'
        // component-level documentation for a more complete description of
        // default values.)  Return a sub-aggregate referring to the
        // modifiable selection on success or an error object on failure (as
        // described in the "Error Handling" section of the 'bcem_aggregate'
        // component-level documentation).  This aggregate is not modified if
        // an error is detected.

    template <typename VALTYPE>
    const bcem_Aggregate makeSelectionById(int            newSelectorId,
                                           const VALTYPE& value) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelectorId' and set the newly
        // selected sub-object to 'value', resetting the nullness flag if
        // 'isNul2()' returns 'true', after appropriate conversions (see
        // "Extended Type Conversions" in the 'bcem_Aggregate' component-level
        // documentation).  If 'newSelectorId' is
        // 'bdem_RecordDef::BDEM_NULL_FIELD_ID' then the selection is reset.
        // Return a sub-aggregate referring to the new selection on success or
        // an error object on failure (as described in the "Error Handling"
        // section of the 'bcem_aggregate' component-level documentation).  If
        // value is null then the selected aggregate is made null.  If
        // 'newSelectorId' is valid but 'value' is not convertible to the
        // newly-selected sub-object type, then the sub-object is initialized
        // to its default value, otherwise this aggregate is not modified when
        // an error is detected.

    const bcem_Aggregate makeSelectionByIndex(int newSelectorIndex) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelectorIndex' resetting the
        // nullness flag if 'isNul2()' returns 'true'.  If 'newSelectorIndex'
        // is negative then the selection is reset.  The newly selected
        // sub-object is initialized to its default value.  (See "Null Values
        // and Default Values" in the 'bcem_Aggregate' component-level
        // documentation for a more complete description of default values.)
        // Return a sub-aggregate referring to the modifiable selection on
        // success or an error object on failure (as described in the "Error
        // Handling" section of the 'bcem_aggregate' component-level
        // documentation).  This aggregate is not modified if an error is
        // detected.

    template <typename VALTYPE>
    const bcem_Aggregate makeSelectionByIndex(int            newSelectorIndex,
                                              const VALTYPE& value) const;
        // Change the selector in the choice object referenced by this
        // aggregate to the one specified by 'newSelectorIndex' and set the
        // newly selected sub-object to 'value' resetting the nullness flag if
        // 'isNul2()' returns 'true', after appropriate conversions (see
        // "Extended Type Conversions" in the 'bcem_Aggregate' component-level
        // documentation).  If 'newSelectorIndex' is negative then the
        // selection is reset.  Return a sub-aggregate referring to the
        // modifiable selection on success or an error object on failure (as
        // described in the "Error Handling" section of the 'bcem_aggregate'
        // component-level documentation).  If 'newSelectorIndex' is valid but
        // 'value' is not convertible to the newly-selected sub-object type,
        // then the sub-object is initialized to its default value, otherwise
        // this aggregate is not modified when an error is detected.

    const bcem_Aggregate makeValue() const;
        // Construct a value for the nullable object referenced by this
        // aggregate by resetting its nullness flag and assigning it the
        // default value specified in the schema, or (for 'LIST' aggregates)
        // by giving each scalar field its default value and recursively
        // constructing the default value for each non-nullable sublist (see
        // "Null Values and Default Values" in the 'bcem_aggregate'
        // component-level documentation).  Return this aggregate on success
        // or an error aggregate on failure.  If the aggregate is not null
        // when this function is called, do nothing (this is not an error).
        // Furthermore, if this aggregate refers to a list with an empty
        // record definition, then the entire list will still be null after
        // this function is called.

    void swap(bcem_Aggregate& rhs);
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
        // 'dataType()' is 'LIST', call 'bdem_List::bdexStreamIn' on the
        // referenced object).  See the 'bdex' package-level documentation for
        // more information on 'bdex' streaming of value-semantic types and
        // containers.

    // ACCESSORS
    const bcem_Aggregate capacityRaw(bsl::size_t *capacity) const;
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
        // 'dataType()' will return 'bdem_ElemType::BDEM_VOID', 'errorCode()'
        // will return a non-zero value, and 'errorMessage()' will return a
        // non-empty string.

    bool isVoid() const;
        // Return 'true' if 'dataType() == bdem_ElemType::BDEM_VOID'.  This
        // function will always return 'true' if 'isError()' returns 'true'.

    bool isNul2() const;
        // Return 'true' if the data referenced by this aggregate has a null
        // value, and 'false' otherwise.  This function returns 'true' if
        // 'isVoid()' returns 'true'.

    bool isNullable() const;
        // Return 'true' if the data referenced by this aggregate can be made
        // null, and 'false' otherwise.

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
    bool isUnset() const;
        // Return 'true' if the data referenced by this aggregate has the
        // "unset" value for its type (i.e., is an "unset" scalar as defined in
        // 'bdetu_Unset', or is an empty array or aggregate), and 'false'
        // otherwise.  This function returns 'true' if 'isVoid()' returns
        // 'true'.  Note that this function should rarely be needed; generally
        // 'isNul2()' should be used instead; 'isUnset()' will be removed in a
        // future release.
#endif

    int errorCode() const;
        // Return a negative error code describing the the status of this
        // object if 'isError()' is 'true', or zero if 'isError()' is 'false'.
        // A set of error code constants with names beginning with 'BCEM_ERR_'
        // are described in the 'bcem_aggregateraw' component-level
        // documentation.

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
        //  bcem_Aggregate agg1(bdem_ElemType::BDEM_SHORT, 123);
        //  bsl::string s = agg1.asString();              // string conversion
        //  assert(s == "123");
        //  bcem_Aggregate agg2(bdem_ElemType::BDEM_SHORT, s); // reverse
        //                                                     // conversion
        //  assert(bcem_Aggregate::areEquivalent(agg1, agg2));
        //..

    void loadAsString(bsl::string *result) const;
        // Load into the specified 'result' string a text representation of
        // the value referenced by this aggregate, as returned by
        // 'asString()'.

    bool asBool() const;
    char asChar() const;
    short asShort() const;
    int asInt() const;
    bsls_Types::Int64 asInt64() const;
    float asFloat() const;
    double asDouble() const;
    bdet_Datetime asDatetime() const;
    bdet_DatetimeTz asDatetimeTz() const;
    bdet_Date asDate() const;
    bdet_DateTz asDateTz() const;
    bdet_Time asTime() const;
    bdet_TimeTz asTimeTz() const;
        // Convert the value referenced by this aggregate to the return type
        // of specified conversion function using "Extended Type Conversions"
        // as described in the 'bcem_Aggregate' component-level documentation
        // (returning the enumerator ID when converting enumeration objects to
        // numeric values).  Return the appropriate "null" value if conversion
        // fails.

    const bdem_ElemRef asElemRef() const;
        // Return a reference to the modifiable element value held by this
        // aggregate.

    bool hasField(const char *fieldName) const;
        // Return 'true' if this aggregate contains a field having the
        // specified 'fieldName' and 'false' otherwise.

    bool hasFieldById(int fieldId) const;
        // Return 'true' if this aggregate contains a field having the
        // specified 'fieldId' and 'false' otherwise.

    bool hasFieldByIndex(int fieldIndex) const;
        // Return 'true' if this aggregate contains a field having the
        // specified 'fieldIndex' and 'false' otherwise.

    const bcem_Aggregate field(NameOrIndex fieldOrIdx) const;
        // Get the field within this aggregate specified by the 'fieldOrIdx'
        // argument, which specifies a field name or array index.  Return a
        // modifiable sub-aggregate referring to the field on success or an
        // error object on failure (as described in the "Error Handling"
        // section of the 'bcem_aggregate' component-level documentation).
        // Note that if 'true == isNul2()' then an error is returned.

    const bcem_Aggregate field(NameOrIndex fieldOrIdx1,
                               NameOrIndex fieldOrIdx2,
                               NameOrIndex fieldOrIdx3 = NameOrIndex(),
                               NameOrIndex fieldOrIdx4 = NameOrIndex(),
                               NameOrIndex fieldOrIdx5 = NameOrIndex(),
                               NameOrIndex fieldOrIdx6 = NameOrIndex(),
                               NameOrIndex fieldOrIdx7 = NameOrIndex(),
                               NameOrIndex fieldOrIdx8 = NameOrIndex(),
                               NameOrIndex fieldOrIdx9 = NameOrIndex(),
                               NameOrIndex fieldOrIdx10 = NameOrIndex()) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of two to ten 'fieldOrIdx' arguments, each of which
        // specifies a field name or array index.  Return a sub-aggregate
        // referring to the modifiable field on success or an error object on
        // failure (as described in the "Error Handling" section of the
        // 'bcem_aggregate' component-level documentation).  An empty string
        // can be used for any of the 'fieldOrIdx' arguments to specify the
        // current selection within a choice object.  An unused argument
        // results in the construction of a null 'NameOrIndex', which is
        // treated as the end of the argument list.  Note that if
        // 'true == isNul2()' for any field in the chain of fields then an
        // error is returned.
        //
        // This function is equivalent to iteratively calling the
        // single-argument version of 'field', where each 'fieldOrIdx'
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

    const bcem_Aggregate fieldById(int fieldId) const;
        // Get the field within this aggregate with the specified 'fieldId'.
        // Return a modifiable sub-aggregate referring to the field on success
        // or an error object on failure (as described in the "Error Handling"
        // section of the 'bcem_aggregate' component-level documentation).
        // Note that 'fieldId' refers to a field's numeric identifier
        // (assigned in its record definition within the schema), not to its
        // positional index within the record definition (see
        // 'fieldByIndex').  Note that if 'true == isNul2()' then an error is
        // returned.

    const bcem_Aggregate fieldByIndex(int index) const;
        // Get the field within this aggregate with the specified (zero-based)
        // 'fieldIndex'.  Return a modifiable sub-aggregate referring to the
        // field on success or an error object on failure (as described in the
        // "Error Handling" section of the 'bcem_aggregate' component-level
        // documentation).  Note that if 'true == isNul2()' then an error is
        // returned.

    const bcem_Aggregate anonymousField(int n) const;
        // Return a modifiable sub-aggregate referring to the specified 'n'th
        // anonymous field within this [list or choice] aggregate (where 'n'
        // is zero-based) or an error object on failure (as described in the
        // "Error Handling" section of the 'bcem_aggregate' component-level
        // documentation).  An anonymous field is a field with with a null
        // name and is typically, but not always, an aggregate with the
        // 'IS_UNTAGGED' bit set in its formatting mode.  Note that this
        // function differs from 'fieldByIndex' in that only the anonymous
        // fields are counted.  Also note that it is rarely necessary to call
        // this function, since 'field', 'setField', 'makeSelection', and
        // 'selection' will automatically descend into anonymous fields in
        // most cases.  For more information see the "Anonymous Fields"
        // section of the 'bcem_aggregate' component-level documentation.

    const bcem_Aggregate anonymousField() const;
        // If this aggregate contains exactly one anonymous field, then return
        // 'anonymousField(0)', otherwise return an error object.

    bdem_ElemType::Type
    fieldType(NameOrIndex fieldOrIdx1,
              NameOrIndex fieldOrIdx2  = NameOrIndex(),
              NameOrIndex fieldOrIdx3  = NameOrIndex(),
              NameOrIndex fieldOrIdx4  = NameOrIndex(),
              NameOrIndex fieldOrIdx5  = NameOrIndex(),
              NameOrIndex fieldOrIdx6  = NameOrIndex(),
              NameOrIndex fieldOrIdx7  = NameOrIndex(),
              NameOrIndex fieldOrIdx8  = NameOrIndex(),
              NameOrIndex fieldOrIdx9  = NameOrIndex(),
              NameOrIndex fieldOrIdx10 = NameOrIndex()) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of one to ten 'fieldOrIdx' arguments, each of which
        // specifies a field name or array index.  Return the type of the
        // field on success, or 'bdem_ElemType::BDEM_VOID' on a navigation
        // error.  An empty string can be used for any of the 'fieldOrIdx'
        // arguments to specify the current selection within a choice object.
        // An unused argument results in the construction of a null
        // 'NameOrIndex', which is treated as the end of the argument list.

    bdem_ElemType::Type fieldTypeById(int fieldId) const;
        // Return the type of field referenced by the specified 'fieldId' or
        // 'bdem_ElemType::BDEM_VOID' if 'fieldId' is not in this aggregate's
        // record definition.

    bdem_ElemType::Type fieldTypeByIndex(int index) const;
        // Return the type of field at the position specified by the
        // (zero-based) 'index' or 'bdem_ElemType::BDEM_VOID' if 'index' < 0 or
        // 'recordDef().length() <= index'.

    bdem_ElemRef fieldRef(NameOrIndex fieldOrIdx1,
                          NameOrIndex fieldOrIdx2  = NameOrIndex(),
                          NameOrIndex fieldOrIdx3  = NameOrIndex(),
                          NameOrIndex fieldOrIdx4  = NameOrIndex(),
                          NameOrIndex fieldOrIdx5  = NameOrIndex(),
                          NameOrIndex fieldOrIdx6  = NameOrIndex(),
                          NameOrIndex fieldOrIdx7  = NameOrIndex(),
                          NameOrIndex fieldOrIdx8  = NameOrIndex(),
                          NameOrIndex fieldOrIdx9  = NameOrIndex(),
                          NameOrIndex fieldOrIdx10 = NameOrIndex()) const;
        // Navigate to the field within this aggregate reached through the
        // specified chain of one to ten 'fieldOrIdx' arguments, each of which
        // specifies a field name or array index.  Return an element reference
        // to the specified modifiable field on success or a 'VOID' element
        // reference on error.  An empty string can be used for any of the
        // arguments to specify the current selection within a choice object.
        // An unused argument results in the construction of a null
        // 'NameOrIndex', which is treated as the end of the argument list.
        // Note that if 'true == isNul2()' for any field in the chain of
        // fields then an error is returned.  Modifying the element through
        // the returned reference will modify the original aggregate.  The
        // behavior is undefined if the resulting element ref is used to
        // modify the data such that it no longer conforms to the schema.

    bdem_ElemRef fieldRefById(int fieldId) const;
        // Return an element reference to the modifiable field specified by
        // 'fieldId' within this aggregate or a 'VOID' element reference on
        // error.  If this aggregate refers to a choice value, then a
        // 'fieldId' of 'bdem_RecordDef::BDEM_NULL_FIELD_ID' can be used to
        // specify the current selection.  Modifying the element through the
        // returned reference will modify the original aggregate.  Note that
        // 'fieldId' refers to a field's numeric identifier assigned in its
        // record definition within the schema, not to its positional index
        // within the record definition.  Note that if 'true == isNul2()' then
        // an error is returned.  The behavior is undefined if the resulting
        // element ref is used to modify the data such that it no longer
        // conforms to the schema.

    bdem_ElemRef fieldRefByIndex(int index) const;
        // Return an element reference to the modifiable field within this
        // aggregate specified by the (zero-based) 'fieldIndex' or a 'VOID'
        // element reference on error.  If this aggregate refers to a choice
        // value, then a 'fieldIndex' of -1 can be used to specify the current
        // selection.  Modifying the element through the returned reference
        // will modify the original aggregate.  Note that if
        // 'true == isNul2()' then an error is returned.  The behavior is
        // undefined if the resulting element ref is used to modify the data
        // such that it no longer conforms to the schema.

    const bcem_Aggregate operator[](int index) const;
        // Set the value in this aggregate of the scalar array item, table
        // row, or choice array item selected by the specified 'index'.
        // Return a sub-aggregate referring to the modifiable item on success
        // or an error object on failure (as described in the "Error Handling"
        // section of the 'bcem_aggregate' component-level documentation).
        // Note that if 'true == isNul2()' then an error is returned.

    const bcem_Aggregate operator[](const bsl::string& fieldName) const;
    const bcem_Aggregate operator[](const char *fieldName) const;
        // Get the field within this aggregate with the specified 'fieldId' or
        // 'fieldName'.  Return a modifiable sub-aggregate referring to the
        // field on success or an error object on failure (as described in the
        // "Error Handling" section of the 'bcem_aggregate' component-level
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
        // 'BCEM_ERR_NOT_AN_ARRAY' for other data types.  Note that a null
        // array will return 0.

    int size() const;
        // Equivalent to 'length()' (STL-style).

    const bcem_Aggregate selection() const;
        // If this aggregate refers to choice or choice array item, return the
        // sub-aggregate that refers to the modifiable current selection, or a
        // void aggregate if there is no current selection.  If this aggregate
        // refers to a list or row, look for an anonymous field within the
        // list or row and recursively look for a choice in the anonymous
        // field (if any), as per the "Anonymous Fields" section of the
        // 'bcem_aggregate' component-level documentation.  Otherwise, (if
        // this aggregate does not directly or indirectly refer to a choice or
        // choice array item) or 'true == isNul2()' return an error object.

    int numSelections() const;
        // If this aggregate refers to choice or choice array item, return the
        // number of available selectors or 0 if 'true == isNul2()'.  If this
        // aggregate refers to a list or row, look for an anonymous field
        // within the list or row and recursively look for a choice in the
        // anonymous field (if any), as per the "Anonymous Fields" section of
        // the 'bcem_aggregate' component-level documentation.  Otherwise,
        // return an error code.

    const char *selector() const;
        // Return the name for selector of the choice or choice array item
        // referenced by this aggregate or an empty string either if there is
        // no current selection, if the current selector does not have a text
        // name, if this aggregate does not refer to a choice or choice array
        // item, or 'true == isNul2()'.  If this aggregate refers to a list or
        // row, look for an  anonymous field within the list or row and
        // recursively look for a choice in the anonymous field (if any), as
        // per the "Anonymous Fields" section of the 'bcem_aggregate'
        // component-level documentation.  The returned pointer is valid until
        // the the choice object is modified or destroyed.  Note that an error
        // condition is indistinguishable from one of the valid reasons for
        // returning an empty string.  The 'selection' or 'selectorId' methods
        // can be used to distinguish an error condition from a valid
        // selector.

    int selectorId() const;
        // Return the ID for selector of the choice or choice array item
        // referenced by this aggregate, 'bdem_RecordDef::BDEM_NULL_FIELD_ID'
        // if there is no current selection, the current selection does not
        // have a numeric ID, this aggregate does not refer to a choice or
        // choice array item, or 'true == isNul2()'.  If this aggregate refers
        // to a list or row, look for an anonymous field within the list or
        // row and recursively look for a choice in the anonymous field (if
        // any), as per the "Anonymous Fields" section of the 'bcem_aggregate'
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
        // 'bcem_aggregate' component-level documentation.

    const bcem_Aggregate clone(bslma_Allocator *basicAllocator = 0) const;
        // Make an independent copy, using the specified 'basicAllocator', of
        // this aggregate's data and schema and return a new aggregate object
        // holding the copy.  Changes made to the clone's data will have no
        // effect on this aggregate's data.  The returned clone will compare
        // equivalent to this aggregate (using 'areEquivalent') but will not
        // compare identical to this aggregate (using 'areIdentical').  If
        // this aggregate is an error object, then the clone will be an
        // equivalent, but not identical, error object.  Note that the
        // nullness information is also cloned.

    const bcem_Aggregate cloneData(bslma_Allocator *basicAllocator = 0) const;
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

    bdem_ElemType::Type dataType() const;
        // Return the type of data referenced by this aggregate.  Return
        // 'bdem_ElemType::BDEM_VOID' for a void or error aggregate.

    bcema_SharedPtr<const bdem_Schema> schemaPtr() const;
        // Return a shared pointer to the non-modifiable schema associated
        // with this aggregate.  Return an empty pointer if this aggregate has
        // no associated schema (i.e., is unconstrained).

    const bcem_AggregateRaw& rawData() const;
        // Return a reference to the non-reference-counted portion of 
        // this aggregate. 

    const bdem_RecordDef& recordDef() const;
        // Return a reference to the non-modifiable record definition that
        // describes the structure of the object referenced by this aggregate.
        // The behavior is undefined unless this aggregate references a
        // constrained list, constrained table, or constrained choice object.
        // (Use 'recordConstraint' if there is any doubt as to whether this
        // aggregate has a record definition.)

    bcema_SharedPtr<const bdem_RecordDef> recordDefPtr() const;
        // Return a shared pointer to the non-modifiable record definition
        // that describes the structure of object referenced by this
        // aggregate.  Return an empty pointer if this aggregate references a
        // scalar, array of scalars, or unconstrained aggregate.

    const bdem_RecordDef *recordConstraint() const;
        // Return a pointer to the non-modifiable record definition that
        // describes the structure of the object referenced by this aggregate,
        // or a null pointer if this aggregate references a scalar, array of
        // scalars, or unconstrained 'bdem' aggregate.

    const bdem_EnumerationDef *enumerationConstraint() const;
        // Return a pointer to the non-modifiable enumeration definition that
        // constrains the object referenced by this aggregate, or a null
        // pointer if this aggregate does not reference an enumeration object.

    const bdem_FieldDef *fieldDef() const;
        // Return a pointer to the non-modifiable field definition for the
        // object referenced by this aggregate, or null if this object does
        // not have a field definition.  An aggregate constructed directly
        // using a record definition will not have a field definition,
        // whereas a sub-aggregate returned by a field-access function (e.g.,
        // 'operator[]' or 'field') will.  Note that, if this aggregate is an
        // item within an array, table, or choice array, then
        // 'fieldDef()->elemType()' will return the *array* type, not the
        // *item* type (i.e., 'fieldDef()->elemType()' will not match
        // 'dataType()').

    const bdem_FieldDef *fieldSpec() const;
        // Return a pointer to the non-modifiable field definition for the
        // object referenced by this aggregate, or null if this object does
        // not have a field definition.  An aggregate constructed directly
        // using a record definition will not have a field definition,
        // whereas a sub-aggregate returned by a field-access function (e.g.,
        // 'operator[]' or 'field') will.  Note that, if this aggregate is an
        // item within an array, table, or choice array, then
        // 'fieldSpec()->elemType()' will return the *array* type, not the
        // *item* type (i.e., 'fieldSpec()->elemType()' will not match
        // 'dataType()').
        //
        // DEPRECATED: use 'fieldDef' instead.

    const void *data() const;
        // Return the address of the non-modifiable data referenced by this
        // aggregate.  The behavior is undefined unless
        // 'bdem_ElemType::BDEM_VOID != dataType()'.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of the object referenced by this aggregate to the
        // specified output 'stream' using the specified 'version' format and
        // return a reference to the modifiable 'stream'.  If 'version' is not
        // supported, 'stream' is unmodified.  Note that 'version' is not
        // written to 'stream'.  This operation has the same effect as calling
        // 'bdexStreamOut' on the referenced object (e.g., if 'dataType()' is
        // 'LIST', call 'bdem_List::bdexStreamOut' on the referenced object).
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

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, const bcem_Aggregate& rhs);
    // Format 'rhs' in human-readable form (same format as
    // 'rhs.print(stream, 0, -1)') and return a modifiable reference to
    // 'stream'.

// ---  Anything below this line is implementation specific.  Do not use.  ----

                    // =====================================
                    // local class bcem_Aggregate_RepProctor
                    // =====================================

class bcem_Aggregate_RepProctor {
    // This "component-private" class is a proctor for managing
    // shared pointers with RAII.

    bcema_SharedPtrRep *d_rep_p; 
    
public:
    bcem_Aggregate_RepProctor(bcema_SharedPtrRep *rep)
    : d_rep_p(rep)
    {}

    ~bcem_Aggregate_RepProctor()
    {
        if (d_rep_p) {
            d_rep_p->releaseRef();
        }
    }

    void release() {
        d_rep_p = 0;
    }
};

// ===========================================================================
//                      INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ===========================================================================

                        //---------------------
                        // class bcem_Aggregate
                        //---------------------

inline
const bcem_Aggregate& bcem_Aggregate::makeNull()  const
{
    d_rawData.makeNull();
    return *this;
}

inline
const bcem_Aggregate bcem_Aggregate::makeValue() const
{
    d_rawData.makeValue();
    return *this;
}

// CLASS METHODS
template <typename TYPE>
inline 
const TYPE& bcem_Aggregate::valueRef(const TYPE& value) {
    return value;
}

inline 
bdem_ElemRef bcem_Aggregate::valueRef(const bcem_Aggregate& value) {
    return value.asElemRef();
}

inline
bool bcem_Aggregate::areIdentical(const bcem_Aggregate& lhs,
                                  const bcem_Aggregate& rhs)
{
    // If identical in these respects, then there is no need to check their
    // respective nullness attributes.  We need to special case for aggregates
    // of type 'bdem_ElemType::BDEM_VOID' as those have empty values.

    return bdem_ElemType::BDEM_VOID != lhs.dataType()
        && lhs.dataType()           == rhs.dataType()
        && lhs.data()               == rhs.data();
}

inline
int bcem_Aggregate::maxSupportedBdexVersion()
{
    return bcem_AggregateRaw::maxSupportedBdexVersion();
}

// CREATORS
template <typename VALTYPE>
bcem_Aggregate::bcem_Aggregate(const bdem_ElemType::Type  dataType,
                               const VALTYPE&             value,
                               bslma_Allocator           *basicAllocator)
: d_rawData()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
    d_rawData.setDataType(dataType);

    bcema_SharedPtr<void> value_sp = 
        makeValuePtr(dataType, basicAllocator);

    d_rawData.setDataPointer(value_sp.ptr());
    d_valueRep_p = value_sp.rep();
    d_valueRep_p->acquireRef();
    
    bcem_Aggregate_RepProctor valueRepProctor(d_valueRep_p);

    int status = bdem_Convert::toBdemType(value_sp.ptr(), dataType, 
                                          valueRef(value));
    if (status) {
        *this = makeError(bcem_AggregateError::BCEM_ERR_BAD_CONVERSION,
                          "Invalid conversion to %s from %s",
                          bdem_ElemType::toAscii(dataType),
                          bdem_ElemType::toAscii(
                              bcem_AggregateRaw::getBdemType(valueRef(value))));
    }
    else {
        bcema_SharedPtr<int> null_sp;
        null_sp.createInplace(basicAllocator, 0);
        d_rawData.setTopLevelAggregateNullnessPointer(null_sp.ptr());
        d_isTopLevelAggregateNullRep_p = null_sp.rep();
        d_isTopLevelAggregateNullRep_p->acquireRef();
    }
    valueRepProctor.release();
}

inline
bcem_Aggregate::bcem_Aggregate(
        const bcema_SharedPtr<const bdem_RecordDef>&  recordDefPtr,
        bslma_Allocator                              *basicAllocator)
: d_rawData()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
    init(recordDefPtr, bdem_ElemType::BDEM_VOID, basicAllocator);
}

inline
bcem_Aggregate::bcem_Aggregate(
        const bcema_SharedPtr<bdem_RecordDef>&  recordDefPtr,
        bslma_Allocator                        *basicAllocator)
: d_rawData()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
    init(recordDefPtr, bdem_ElemType::BDEM_VOID, basicAllocator);
}

inline
bcem_Aggregate::bcem_Aggregate(
        const bcema_SharedPtr<const bdem_RecordDef>&  recordDefPtr,
        bdem_ElemType::Type                           elemType,
        bslma_Allocator                              *basicAllocator)
: d_rawData()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
    init(recordDefPtr, elemType, basicAllocator);
}

inline
bcem_Aggregate::bcem_Aggregate(
        const bcema_SharedPtr<bdem_RecordDef>&  recordDefPtr,
        bdem_ElemType::Type                     elemType,
        bslma_Allocator                        *basicAllocator)
: d_rawData()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
    init(recordDefPtr, elemType, basicAllocator);
}

inline
bcem_Aggregate::bcem_Aggregate(
        const bcema_SharedPtr<const bdem_Schema>&  schemaPtr,
        const bsl::string&                         recordName,
        bdem_ElemType::Type                        elemType,
        bslma_Allocator                           *basicAllocator)
: d_rawData()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
    init(schemaPtr, recordName.c_str(), elemType, basicAllocator);
}

inline
bcem_Aggregate::bcem_Aggregate(
        const bcema_SharedPtr<bdem_Schema>&  schemaPtr,
        const bsl::string&                   recordName,
        bdem_ElemType::Type                  elemType,
        bslma_Allocator                     *basicAllocator)
: d_rawData()
, d_schemaRep_p(0)
, d_valueRep_p(0)
, d_isTopLevelAggregateNullRep_p(0)
{
    init(schemaPtr, recordName.c_str(), elemType, basicAllocator);
}


// ACCESSORS THAT MANIPULATE DATA
template <typename VALTYPE>
const bcem_Aggregate bcem_Aggregate::setValue(const VALTYPE& value) const
{
    if (isError()) {
        return *this;                                                 
    }

    bcem_AggregateError errorDescription;
    if (0 != d_rawData.setValue(&errorDescription, valueRef(value))) {
        return makeError(errorDescription);
    }
    return *this;
}

template <typename VALTYPE>
inline
const bcem_Aggregate bcem_Aggregate::setField(NameOrIndex    fieldOrIdx1,
                                              const VALTYPE& value) const
{
    return fieldImp(true, fieldOrIdx1).setValue(value);
}

template <typename VALTYPE>
inline
const bcem_Aggregate bcem_Aggregate::setField(NameOrIndex    fieldOrIdx1,
                                              NameOrIndex    fieldOrIdx2,
                                              const VALTYPE& value) const
{
    return fieldImp(true, fieldOrIdx1, fieldOrIdx2).setValue(value);
}

template <typename VALTYPE>
inline
const bcem_Aggregate bcem_Aggregate::setField(NameOrIndex    fieldOrIdx1,
                                              NameOrIndex    fieldOrIdx2,
                                              NameOrIndex    fieldOrIdx3,
                                              const VALTYPE& value) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3).setValue(value);
}

template <typename VALTYPE>
inline
const bcem_Aggregate bcem_Aggregate::setField(NameOrIndex    fieldOrIdx1,
                                              NameOrIndex    fieldOrIdx2,
                                              NameOrIndex    fieldOrIdx3,
                                              NameOrIndex    fieldOrIdx4,
                                              const VALTYPE& value) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4).setValue(value);
}

template <typename VALTYPE>
inline
const bcem_Aggregate bcem_Aggregate::setField(NameOrIndex    fieldOrIdx1,
                                              NameOrIndex    fieldOrIdx2,
                                              NameOrIndex    fieldOrIdx3,
                                              NameOrIndex    fieldOrIdx4,
                                              NameOrIndex    fieldOrIdx5,
                                              const VALTYPE& value) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5).setValue(value);
}

template <typename VALTYPE>
inline
const bcem_Aggregate bcem_Aggregate::setField(NameOrIndex    fieldOrIdx1,
                                              NameOrIndex    fieldOrIdx2,
                                              NameOrIndex    fieldOrIdx3,
                                              NameOrIndex    fieldOrIdx4,
                                              NameOrIndex    fieldOrIdx5,
                                              NameOrIndex    fieldOrIdx6,
                                              const VALTYPE& value) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5,
                    fieldOrIdx6).setValue(value);
}

template <typename VALTYPE>
inline
const bcem_Aggregate bcem_Aggregate::setField(NameOrIndex    fieldOrIdx1,
                                              NameOrIndex    fieldOrIdx2,
                                              NameOrIndex    fieldOrIdx3,
                                              NameOrIndex    fieldOrIdx4,
                                              NameOrIndex    fieldOrIdx5,
                                              NameOrIndex    fieldOrIdx6,
                                              NameOrIndex    fieldOrIdx7,
                                              const VALTYPE& value) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5,
                    fieldOrIdx6,
                    fieldOrIdx7).setValue(value);
}

template <typename VALTYPE>
inline
const bcem_Aggregate bcem_Aggregate::setField(NameOrIndex    fieldOrIdx1,
                                              NameOrIndex    fieldOrIdx2,
                                              NameOrIndex    fieldOrIdx3,
                                              NameOrIndex    fieldOrIdx4,
                                              NameOrIndex    fieldOrIdx5,
                                              NameOrIndex    fieldOrIdx6,
                                              NameOrIndex    fieldOrIdx7,
                                              NameOrIndex    fieldOrIdx8,
                                              const VALTYPE& value) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5,
                    fieldOrIdx6,
                    fieldOrIdx7,
                    fieldOrIdx8).setValue(value);
}

template <typename VALTYPE>
inline
const bcem_Aggregate bcem_Aggregate::setField(NameOrIndex    fieldOrIdx1,
                                              NameOrIndex    fieldOrIdx2,
                                              NameOrIndex    fieldOrIdx3,
                                              NameOrIndex    fieldOrIdx4,
                                              NameOrIndex    fieldOrIdx5,
                                              NameOrIndex    fieldOrIdx6,
                                              NameOrIndex    fieldOrIdx7,
                                              NameOrIndex    fieldOrIdx8,
                                              NameOrIndex    fieldOrIdx9,
                                              const VALTYPE& value) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5,
                    fieldOrIdx6,
                    fieldOrIdx7,
                    fieldOrIdx8,
                    fieldOrIdx9).setValue(value);
}

template <typename VALTYPE>
inline
const bcem_Aggregate bcem_Aggregate::setField(NameOrIndex    fieldOrIdx1,
                                              NameOrIndex    fieldOrIdx2,
                                              NameOrIndex    fieldOrIdx3,
                                              NameOrIndex    fieldOrIdx4,
                                              NameOrIndex    fieldOrIdx5,
                                              NameOrIndex    fieldOrIdx6,
                                              NameOrIndex    fieldOrIdx7,
                                              NameOrIndex    fieldOrIdx8,
                                              NameOrIndex    fieldOrIdx9,
                                              NameOrIndex    fieldOrIdx10,
                                              const VALTYPE& value) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5,
                    fieldOrIdx6,
                    fieldOrIdx7,
                    fieldOrIdx8,
                    fieldOrIdx9,
                    fieldOrIdx10).setValue(value);
}

inline
const bcem_Aggregate
bcem_Aggregate::setFieldNull(NameOrIndex fieldOrIdx1) const
{
    return fieldImp(true, fieldOrIdx1).makeNull();
}

inline
const bcem_Aggregate
bcem_Aggregate::setFieldNull(NameOrIndex fieldOrIdx1,
                             NameOrIndex fieldOrIdx2) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2).makeNull();
}

inline
const bcem_Aggregate
bcem_Aggregate::setFieldNull(NameOrIndex fieldOrIdx1,
                             NameOrIndex fieldOrIdx2,
                             NameOrIndex fieldOrIdx3) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3).makeNull();
}

inline
const bcem_Aggregate
bcem_Aggregate::setFieldNull(NameOrIndex fieldOrIdx1,
                             NameOrIndex fieldOrIdx2,
                             NameOrIndex fieldOrIdx3,
                             NameOrIndex fieldOrIdx4) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4).makeNull();
}

inline
const bcem_Aggregate
bcem_Aggregate::setFieldNull(NameOrIndex fieldOrIdx1,
                             NameOrIndex fieldOrIdx2,
                             NameOrIndex fieldOrIdx3,
                             NameOrIndex fieldOrIdx4,
                             NameOrIndex fieldOrIdx5) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5).makeNull();
}

inline
const bcem_Aggregate
bcem_Aggregate::setFieldNull(NameOrIndex fieldOrIdx1,
                             NameOrIndex fieldOrIdx2,
                             NameOrIndex fieldOrIdx3,
                             NameOrIndex fieldOrIdx4,
                             NameOrIndex fieldOrIdx5,
                             NameOrIndex fieldOrIdx6) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5,
                    fieldOrIdx6).makeNull();
}

inline
const bcem_Aggregate
bcem_Aggregate::setFieldNull(NameOrIndex fieldOrIdx1,
                             NameOrIndex fieldOrIdx2,
                             NameOrIndex fieldOrIdx3,
                             NameOrIndex fieldOrIdx4,
                             NameOrIndex fieldOrIdx5,
                             NameOrIndex fieldOrIdx6,
                             NameOrIndex fieldOrIdx7) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5,
                    fieldOrIdx6,
                    fieldOrIdx7).makeNull();
}

inline
const bcem_Aggregate
bcem_Aggregate::setFieldNull(NameOrIndex fieldOrIdx1,
                             NameOrIndex fieldOrIdx2,
                             NameOrIndex fieldOrIdx3,
                             NameOrIndex fieldOrIdx4,
                             NameOrIndex fieldOrIdx5,
                             NameOrIndex fieldOrIdx6,
                             NameOrIndex fieldOrIdx7,
                             NameOrIndex fieldOrIdx8) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5,
                    fieldOrIdx6,
                    fieldOrIdx7,
                    fieldOrIdx8).makeNull();
}

inline
const bcem_Aggregate
bcem_Aggregate::setFieldNull(NameOrIndex fieldOrIdx1,
                             NameOrIndex fieldOrIdx2,
                             NameOrIndex fieldOrIdx3,
                             NameOrIndex fieldOrIdx4,
                             NameOrIndex fieldOrIdx5,
                             NameOrIndex fieldOrIdx6,
                             NameOrIndex fieldOrIdx7,
                             NameOrIndex fieldOrIdx8,
                             NameOrIndex fieldOrIdx9) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5,
                    fieldOrIdx6,
                    fieldOrIdx7,
                    fieldOrIdx8,
                    fieldOrIdx9).makeNull();
}

inline
const bcem_Aggregate
bcem_Aggregate::setFieldNull(NameOrIndex fieldOrIdx1,
                             NameOrIndex fieldOrIdx2,
                             NameOrIndex fieldOrIdx3,
                             NameOrIndex fieldOrIdx4,
                             NameOrIndex fieldOrIdx5,
                             NameOrIndex fieldOrIdx6,
                             NameOrIndex fieldOrIdx7,
                             NameOrIndex fieldOrIdx8,
                             NameOrIndex fieldOrIdx9,
                             NameOrIndex fieldOrIdx10) const
{
    return fieldImp(true,
                    fieldOrIdx1,
                    fieldOrIdx2,
                    fieldOrIdx3,
                    fieldOrIdx4,
                    fieldOrIdx5,
                    fieldOrIdx6,
                    fieldOrIdx7,
                    fieldOrIdx8,
                    fieldOrIdx9,
                    fieldOrIdx10).makeNull();
}

template <typename VALTYPE>
inline
const bcem_Aggregate
bcem_Aggregate::setFieldById(int fieldId, const VALTYPE& value) const
{
    return fieldById(fieldId).setValue(value);
}

inline
const bcem_Aggregate bcem_Aggregate::setFieldNullById(int fieldId) const
{
    return fieldById(fieldId).makeNull();
}

template <typename VALTYPE>
inline
const bcem_Aggregate
bcem_Aggregate::setFieldByIndex(int fieldIndex, const VALTYPE& value) const
{
    return fieldByIndex(fieldIndex).setValue(value);
}

inline
const bcem_Aggregate bcem_Aggregate::setFieldNullByIndex(int fieldIndex) const
{
    return fieldByIndex(fieldIndex).makeNull();
}

template <typename VALTYPE>
inline
const bcem_Aggregate
bcem_Aggregate::setItem(int index, const VALTYPE& value) const
{
    return (*this)[index].setValue(value);
}

inline
const bcem_Aggregate bcem_Aggregate::setItemNull(int index) const
{
    return (*this)[index].makeNull();
}

template <typename VALTYPE>
inline
const bcem_Aggregate bcem_Aggregate::append(const VALTYPE& newItem) const
{
    return insert(length(), newItem);
}

inline
const bcem_Aggregate bcem_Aggregate::appendNull() const
{
    return insertNull(length());
}

template <typename VALTYPE>
inline
const bcem_Aggregate bcem_Aggregate::insert(int            pos,
                                            const VALTYPE& newItem) const
{
    bcem_AggregateRaw field;
    bcem_AggregateError errorDescription;
    if (0 != d_rawData.insertItem(&field, &errorDescription, 
                                  pos, valueRef(newItem))) {
        return makeError(errorDescription);
    }
    return bcem_Aggregate(field, d_schemaRep_p, d_valueRep_p, 
                          d_isTopLevelAggregateNullRep_p);
}

inline
const bcem_Aggregate bcem_Aggregate::insertNull(int pos) const
{
    const bcem_Aggregate& retAgg = insertNullItems(pos, 1);
    return retAgg.isError() ? retAgg : retAgg.field(pos);
}

inline
const bcem_Aggregate bcem_Aggregate::appendItems(int numItems) const
{
    return insertItems(length(), numItems);
}

inline
const bcem_Aggregate bcem_Aggregate::appendNullItems(int numItems) const
{
    return insertNullItems(length(), numItems);
}

inline
bsl::string bcem_Aggregate::asString() const 
{
    return d_rawData.asString();
}

inline
void bcem_Aggregate::loadAsString(bsl::string *result) const 
{
    d_rawData.loadAsString(result);
}

inline
int bcem_Aggregate::selectorId() const
{
    return d_rawData.selectorId();
}

inline
int bcem_Aggregate::length() const
{
    return d_rawData.length();
}

inline
int bcem_Aggregate::numSelections() const
{
    return d_rawData.numSelections();
}

inline 
const char *bcem_Aggregate::selector() const 
{
    return d_rawData.selector();
}

inline
bool bcem_Aggregate::asBool() const
{
    return d_rawData.asBool();
}

inline
char bcem_Aggregate::asChar() const
{
    return d_rawData.asChar();
}

inline
short bcem_Aggregate::asShort() const
{
    return d_rawData.asShort();
}

inline
int bcem_Aggregate::asInt() const
{
    return d_rawData.asInt();
}

inline
bsls_Types::Int64 bcem_Aggregate::asInt64() const
{
    return d_rawData.asInt64();
}

inline
float bcem_Aggregate::asFloat() const
{
    return d_rawData.asFloat();
}

inline
double bcem_Aggregate::asDouble() const
{
    return d_rawData.asDouble();
}

inline
bdet_Datetime bcem_Aggregate::asDatetime() const
{
    return d_rawData.asDatetime();
}

inline
bdet_DatetimeTz bcem_Aggregate::asDatetimeTz() const
{
    return d_rawData.asDatetimeTz();
}

inline
bdet_Date bcem_Aggregate::asDate() const
{
    return d_rawData.asDate();
}

inline
bdet_DateTz bcem_Aggregate::asDateTz() const
{
    return d_rawData.asDateTz();
}

inline
bdet_Time bcem_Aggregate::asTime() const
{
    return d_rawData.asTime();
}

inline
bdet_TimeTz bcem_Aggregate::asTimeTz() const
{
    return d_rawData.asTimeTz();
}

inline
int bcem_Aggregate::size() const
{
    return length();
}

inline
const bcem_Aggregate bcem_Aggregate::remove(int pos) const
{
    return removeItems(pos, 1);
}

inline
const bcem_Aggregate bcem_Aggregate::removeAllItems() const
{
    return removeItems(0, length());
}

inline
const bcem_Aggregate
bcem_Aggregate::makeSelection(const bsl::string& newSelector) const
{
    return makeSelection(newSelector.c_str());
}

template <typename VALTYPE>
const bcem_Aggregate
bcem_Aggregate::makeSelectionByIndex(int            newSelectorIndex,
                                     const VALTYPE& value) const
{
    bool isAggNull = isNul2();
    if (isAggNull) {
        makeValue();
    }

    bcem_AggregateError errorDescription;
    bcem_AggregateRaw   result;
    if (0 != d_rawData.makeSelectionByIndexRaw(&result, 
                                               &errorDescription, 
                                               newSelectorIndex) ||
        0 != result.setValue(&errorDescription, valueRef(value))) {
        if (isAggNull) {
            makeNull();
        }
        return makeError(errorDescription);
    }

    return bcem_Aggregate(result, d_schemaRep_p, d_valueRep_p, 
                          d_isTopLevelAggregateNullRep_p);
}

template <typename VALTYPE>
const bcem_Aggregate
bcem_Aggregate::makeSelection(const char     *newSelector,
                              const VALTYPE&  value) const
{
    int newSelectorIndex = -1;
    bcem_AggregateError errorDescription;
    if (0 != d_rawData.getFieldIndex(&newSelectorIndex,
                                     &errorDescription,
                                     newSelector,
                                     "makeSelection")) {
        return makeError(errorDescription);
    }

    return makeSelectionByIndex(newSelectorIndex, value);
}

template <typename VALTYPE>
inline
const bcem_Aggregate
bcem_Aggregate::makeSelection(const bsl::string& newSelector,
                              const VALTYPE&     value) const
{
    return makeSelection(newSelector.c_str(), value);
}

inline
const bcem_Aggregate
bcem_Aggregate::reserveRaw(bsl::size_t numItems) 
{
    bcem_AggregateError errorDescription;
    if (0 == d_rawData.reserveRaw(&errorDescription, numItems)) {
        return *this;
    }
    return makeError(errorDescription);
}

template <typename VALTYPE>
inline
const bcem_Aggregate
bcem_Aggregate::makeSelectionById(int newSelector, const VALTYPE& value) const
{
    int newSelectorIndex = -1;
    bcem_AggregateError errorDescription;
    if (0 != d_rawData.getFieldIndex(&newSelectorIndex,
                                     &errorDescription,
                                     newSelector,
                                     "makeSelection")) {
        return makeError(errorDescription);
    }
    
    return makeSelectionByIndex(newSelectorIndex, value);
}

template <class STREAM>
inline
STREAM& bcem_Aggregate::bdexStreamIn(STREAM& stream, int version) const
{
    return d_rawData.bdexStreamIn(stream, version);
}

// ACCESSORS
inline 
int bcem_Aggregate::selectorIndex() const
{
    return d_rawData.selectorIndex();
}

inline 
const bdem_ElemRef bcem_Aggregate::asElemRef() const
{
    return d_rawData.asElemRef();
}

inline
bool bcem_Aggregate::isError() const
{
    return d_rawData.isError();
}

inline
bool bcem_Aggregate::isNul2() const
{
    return d_rawData.isNull();
}

inline
bool bcem_Aggregate::isNullable() const
{
    return d_rawData.isNullable();
}

inline
int bcem_Aggregate::errorCode() const
{
    return d_rawData.errorCode();
}

inline
bool bcem_Aggregate::isVoid() const
{
    return d_rawData.isVoid();
}

inline
bool bcem_Aggregate::hasField(const char *fieldName) const
{
    return d_rawData.hasField(fieldName);
}

inline
bool bcem_Aggregate::hasFieldById(int fieldId) const
{
    return d_rawData.hasFieldById(fieldId);
}

inline
bool bcem_Aggregate::hasFieldByIndex(int fieldIndex) const
{
    return d_rawData.hasFieldByIndex(fieldIndex);
}

inline
const bcem_Aggregate
bcem_Aggregate::operator[](const bsl::string& fieldName) const
{
    return fieldImp(true, fieldName.c_str());
}

inline
const bcem_Aggregate bcem_Aggregate::operator[](const char *fieldName) const
{
    return fieldImp(true, fieldName);
}

inline
bdem_ElemType::Type bcem_Aggregate::dataType() const
{
    return d_rawData.dataType();
}

inline
const bdem_RecordDef& bcem_Aggregate::recordDef() const
{
    return d_rawData.recordDef();
}

inline
const bdem_RecordDef *bcem_Aggregate::recordConstraint() const
{
    return d_rawData.recordConstraint();
}

inline
const bdem_EnumerationDef *bcem_Aggregate::enumerationConstraint() const
{
    return d_rawData.enumerationConstraint();
}

inline
const bdem_FieldDef *bcem_Aggregate::fieldDef() const
{
    return d_rawData.fieldDef();
}

inline
const bdem_FieldDef *bcem_Aggregate::fieldSpec() const
{
    return fieldDef();
}

inline
const void *bcem_Aggregate::data() const
{
    return d_rawData.data();
}

template <class STREAM>
inline
STREAM& bcem_Aggregate::bdexStreamOut(STREAM& stream, int version) const
{
    return d_rawData.bdexStreamOut(stream, version);
}

inline
bsl::ostream& bcem_Aggregate::print(bsl::ostream& stream, 
                                    int level, 
                                    int spacesPerLevel) const
{
    return d_rawData.print(stream, level, spacesPerLevel);
}

inline
const bcem_AggregateRaw& bcem_Aggregate::rawData() const 
{
    return d_rawData;
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, const bcem_Aggregate& rhs)
{
    return rhs.print(stream, 0, -1);
}

// ============================================================================
// Note: All of the remaining code pertains to 'bdeat'.
// ============================================================================

                // ===========================================
                // local struct bcem_Aggregate_NullableAdapter
                // ===========================================

struct bcem_Aggregate_NullableAdapter {
    bcem_Aggregate *d_element_p;
};

// ============================================================================
//           'bdeat_arrayfunctions' overloads and specializations
// ============================================================================

namespace bdeat_ArrayFunctions {

    // META-FUNCTIONS
    bslmf_MetaInt<1> isArrayMetaFunction(const bcem_Aggregate&);
        // This function can be overloaded to support partial specialization
        // (Sun5.2 compiler is unable to partially specialize the 'struct'
        // below).  Note that this function is has no definition and should not
        // be called at runtime.

    template <>
    struct IsArray<bcem_Aggregate> {
        enum { VALUE = 1 };
    };

    template <>
    struct ElementType<bcem_Aggregate> {
        typedef bcem_Aggregate Type;
    };

}  // close namespace bdeat_ArrayFunctions

template <typename MANIPULATOR>
inline
int bdeat_arrayManipulateElement(bcem_Aggregate *array,
                                 MANIPULATOR&    manipulator,
                                 int             index)
{
    return bdeat_arrayManipulateElement(
                                     (bcem_AggregateRaw*)&array->rawData(),
                                     manipulator, index);
}

inline
void bdeat_arrayResize(bcem_Aggregate *array, int newSize)
{
    array->resize(newSize);
}

template <typename ACCESSOR>
inline
int bdeat_arrayAccessElement(const bcem_Aggregate& array,
                             ACCESSOR&             accessor,
                             int                   index)
{
    return bdeat_arrayAccessElement(array.rawData(), accessor, index);
}

inline
bsl::size_t bdeat_arraySize(const bcem_Aggregate& array)
{
    return array.size();
}

// ============================================================================
//           'bdeat_choicefunctions' overloads and specializations
// ============================================================================

namespace bdeat_ChoiceFunctions {

    // META-FUNCTIONS
    bslmf_MetaInt<1> isChoiceMetaFunction(const bcem_Aggregate&);
        // This function can be overloaded to support partial specialization
        // (Sun5.2 compiler is unable to partially specialize the 'struct'
        // below).  Note that this function is has no definition and should not
        // be called at runtime.

    template <>
    struct IsChoice<bcem_Aggregate> {
        enum { VALUE = 1 };
    };

}  // close namespace bdeat_ChoiceFunctions

inline
bool bdeat_choiceHasSelection(const bcem_Aggregate&  object,
                              const char            *selectionName,
                              int                    selectionNameLength)
{
    return bdeat_choiceHasSelection(object.rawData(), 
                                    selectionName, 
                                    selectionNameLength);
}

inline
int bdeat_choiceMakeSelection(bcem_Aggregate *object, int selectionId)
{
    return bdeat_choiceMakeSelection(
                            const_cast<bcem_AggregateRaw*>(&object->rawData()),
                            selectionId);
}

inline
int bdeat_choiceMakeSelection(bcem_Aggregate *object,
                              const char     *selectionName,
                              int             selectionNameLength)
{ 
    return bdeat_choiceMakeSelection(
                            const_cast<bcem_AggregateRaw*>(&object->rawData()),
                            selectionName, selectionNameLength);
}

template <typename MANIPULATOR>
inline
int bdeat_choiceManipulateSelection(bcem_Aggregate *object,
                                    MANIPULATOR&    manipulator)
{
    return bdeat_choiceManipulateSelection(
                           const_cast<bcem_AggregateRaw*>(&object->rawData()), 
                           manipulator);
}

template <typename ACCESSOR>
inline
int bdeat_choiceAccessSelection(const bcem_Aggregate& object,
                                ACCESSOR&             accessor)
{
    return bdeat_choiceAccessSelection(object.rawData(), accessor);
}

inline
bool bdeat_choiceHasSelection(const bcem_Aggregate& object, int selectionId)
{
    return bdeat_choiceHasSelection(object.rawData(), selectionId);
}

inline
int bdeat_choiceSelectionId(const bcem_Aggregate& object)
{
    return bdeat_choiceSelectionId(object.rawData());
}

// ============================================================================
//           'bdeat_enumfunctions' overloads and specializations
// ============================================================================

namespace bdeat_EnumFunctions {

    // META-FUNCTIONS
    bslmf_MetaInt<1> isEnumerationMetaFunction(const bcem_Aggregate&);
        // This function can be overloaded to support partial specialization
        // (Sun5.2 compiler is unable to partially specialize the 'struct'
        // below).  Note that this function is has no definition and should not
        // be called at runtime.

    template <>
    struct IsEnumeration<bcem_Aggregate> {
        enum { VALUE = 1 };
    };

} // close namespace bdeat_EnumFunctions

inline
int bdeat_enumFromInt(bcem_Aggregate *result, int enumId)
{ 
    return bdeat_enumFromInt((bcem_AggregateRaw*)&result->rawData(),
                             enumId);
}
    
inline
int bdeat_enumFromString(bcem_Aggregate *result,
                         const char     *string,
                         int             stringLength)
{
    return bdeat_enumFromString((bcem_AggregateRaw*)&result->rawData(),
                                string, stringLength);
}

inline
void bdeat_enumToInt(int *result, const bcem_Aggregate& value) 
{
    bdeat_enumToInt(result, value.rawData());
}

inline
void bdeat_enumToString(bsl::string *result, const bcem_Aggregate& value)
{
    bdeat_enumToString(result, value.rawData());
}
 
// ============================================================================
//           'bdeat_nullablevaluefunctions' overloads and specializations
// ============================================================================

namespace bdeat_NullableValueFunctions {

    // META-FUNCTIONS
    template <>
    struct IsNullableValue<bcem_Aggregate_NullableAdapter> {
        enum { VALUE = 1 };
    };

    template <>
    struct ValueType<bcem_Aggregate_NullableAdapter> {
        typedef bcem_Aggregate Type;
    };

}  // close namespace bdeat_NullableValueFunctions

inline
bool bdeat_nullableValueIsNull(
                      const bcem_Aggregate_NullableAdapter& object)
{
    return object.d_element_p->isNul2();
}

inline
void bdeat_nullableValueMakeValue(
                             bcem_Aggregate_NullableAdapter *object)
{
    object->d_element_p->makeValue();
}

template <typename MANIPULATOR>
inline
int bdeat_nullableValueManipulateValue(
                        bcem_Aggregate_NullableAdapter  *object,
                        MANIPULATOR&                     manipulator)
{
    return manipulator(object->d_element_p);
}

template <typename ACCESSOR>
inline
int bdeat_nullableValueAccessValue(
                     const bcem_Aggregate_NullableAdapter& object,
                     ACCESSOR&                             accessor)
{
    return accessor(*object.d_element_p);
}

// ============================================================================
//                       'bdeat_valuetype' overloads
// ============================================================================

inline
void bdeat_valueTypeReset(bcem_Aggregate_NullableAdapter *object)
{
    BSLS_ASSERT_SAFE(object);
    BSLS_ASSERT_SAFE(object->d_element_p);

    object->d_element_p->makeNull();
}

// ============================================================================
//           'bdeat_sequencefunctions' overloads and specializations
// ============================================================================

namespace bdeat_SequenceFunctions {

    // META-FUNCTIONS
    bslmf_MetaInt<1> isSequenceMetaFunction(const bcem_Aggregate&);

    template <>
    struct IsSequence<bcem_Aggregate> {
        enum { VALUE = 1 };
    };

}  // close namespace bdeat_SequenceFunctions

template <typename MANIPULATOR>
inline
int bdeat_sequenceManipulateAttribute(bcem_Aggregate *object,
                                      MANIPULATOR&    manipulator,
                                      const char     *attributeName,
                                      int             attributeNameLength)
{
    return bdeat_sequenceManipulateAttribute(
                           const_cast<bcem_AggregateRaw*>(&object->rawData()),
                           manipulator, attributeName, attributeNameLength);
}

template <typename MANIPULATOR>
inline
int bdeat_sequenceManipulateAttribute(bcem_Aggregate *object,
                                      MANIPULATOR&    manipulator,
                                      int             attributeId)
{
    return bdeat_sequenceManipulateAttribute(
                           const_cast<bcem_AggregateRaw*>(&object->rawData()),
                           manipulator, attributeId);
}

template <typename MANIPULATOR>
inline
int bdeat_sequenceManipulateAttributes(bcem_Aggregate *object,
                                       MANIPULATOR&    manipulator)
{
    return bdeat_sequenceManipulateAttributes(
                            const_cast<bcem_AggregateRaw*>(&object->rawData()),
                            manipulator);
}

template <typename ACCESSOR>
inline
int bdeat_sequenceAccessAttribute(const bcem_Aggregate&  object,
                                  ACCESSOR&              accessor,
                                  const char            *attributeName,
                                  int                    attributeNameLength)
{
    return bdeat_sequenceAccessAttribute(object.rawData(), 
                                         accessor, 
                                         attributeName, 
                                         attributeNameLength);
}

template <typename ACCESSOR>
inline
int bdeat_sequenceAccessAttribute(const bcem_Aggregate& object,
                                  ACCESSOR&             accessor,
                                  int                   attributeId)
{
    return bdeat_sequenceAccessAttribute(object.rawData(), accessor, 
                                         attributeId);
}

template <typename ACCESSOR>
inline
int bdeat_sequenceAccessAttributes(const bcem_Aggregate& object,
                                   ACCESSOR&             accessor)
{
    return bdeat_sequenceAccessAttributes(object.rawData(), accessor);
}


inline
bool bdeat_sequenceHasAttribute(const bcem_Aggregate&  object,
                                const char            *attributeName,
                                int                    attributeNameLength)
{
    return bdeat_sequenceHasAttribute(object.rawData(), attributeName, 
                                      attributeNameLength);
}

inline
bool bdeat_sequenceHasAttribute(const bcem_Aggregate& object, int attributeId)
{
    return bdeat_sequenceHasAttribute(object.rawData(), attributeId);
}

// ============================================================================
//                     'bdeat_typecategory' overloads
// ============================================================================

bdeat_TypeCategory::Value
inline
bdeat_typeCategorySelect(const bcem_Aggregate& object)
{
    return bdeat_typeCategorySelect(object.rawData());
}
                                   
template <typename MANIPULATOR>
inline
int bdeat_typeCategoryManipulateArray(bcem_Aggregate *object,
                                      MANIPULATOR&    manipulator)
{
    return bdeat_typeCategoryManipulateArray(
                           const_cast<bcem_AggregateRaw*>(&object->rawData()),
                           manipulator);
}

template <typename MANIPULATOR>
inline
int bdeat_typeCategoryManipulateSimple(bcem_Aggregate *object,
                                       MANIPULATOR&    manipulator)
{
    return bdeat_typeCategoryManipulateSimple
        ((bcem_AggregateRaw*)&object->rawData(), manipulator);
}

template <typename ACCESSOR>
inline
int bdeat_typeCategoryAccessArray(const bcem_Aggregate& object,
                                  ACCESSOR&             accessor)
{
    return bdeat_typeCategoryAccessArray(object.rawData(), accessor);
}

template <typename ACCESSOR>
inline
int bdeat_typeCategoryAccessSimple(const bcem_Aggregate& object,
                                   ACCESSOR&             accessor)
{
    return bdeat_typeCategoryAccessSimple(object.rawData(), accessor);
}


// ============================================================================
//                       'bdeat_typename' overloads
// ============================================================================

const char *bdeat_TypeName_className(const bcem_AggregateRaw& object);
     // Return the name of the record or enumeration definition for the
     // specified 'object' aggregate or a null pointer of 'object' does not
     // have a named record or enumeration definition.

inline
const char *bdeat_TypeName_className(const bcem_Aggregate& object)
{
    return bdeat_TypeName_className(object.rawData());
}

// ============================================================================
//                       'bdeat_valuetype' overloads
// ============================================================================

inline
int bdeat_valueTypeAssign(bcem_Aggregate *lhs, const bcem_Aggregate& rhs)
{
    BSLS_ASSERT_SAFE(lhs);

    lhs->setValue(rhs);
    return 0;
}

inline
void bdeat_valueTypeReset(bcem_Aggregate *object)
{
    bdeat_valueTypeReset((bcem_AggregateRaw*)&object->rawData());
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
