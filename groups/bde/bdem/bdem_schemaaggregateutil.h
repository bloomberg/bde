// bdem_schemaaggregateutil.h                                         -*-C++-*-
#ifndef INCLUDED_BDEM_SCHEMAAGGREGATEUTIL
#define INCLUDED_BDEM_SCHEMAAGGREGATEUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utility functions on combinations of schemas & aggregates.
//
//@CLASSES:
//  bdem_SchemaAggregateUtil: namespace for functions on schemas & aggregates
//
//@SEE_ALSO: bdem_schema, bdem_aggregate, bdem_recorddef,
//           bdem_schemaenumerationutil
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a suite of utility functions that
// operate simultaneously on a 'bdem_RecordDef' object (henceforth referred to
// as a record definition) and an object of 'bdem' aggregate type (i.e.,
// 'bdem_Choice', 'bdem_ChoiceArray', 'bdem_ChoiceArrayItem', 'bdem_List',
// 'bdem_Row', and 'bdem_Table', which are henceforth referred to as "choice",
// "choice array", "choice item", "list", "row", and "table", respectively).
// In particular, this component provides methods for (1) initializing choices,
// choice arrays, lists, and tables conformant to the meta-data in a record
// definition, and (2) methods for verifying that objects of 'bdem' aggregate
// type conform to a specified record.
//
///Terminology
///-----------
// This section describes the basic terminology used throughout this component.
// We start by specifying an alias, 'Element Types', that we use to refer to
// the element types contained within a 'bdem' aggregate type.  Next we
// discuss what we mean by schema record types.  We then describe the different
// types of conformance: "shallow conformance", "conformance", "deep
// conformance", and "satisfaction".  Finally, we delineate what constitutes a
// valid schema.
//
///Element Types
///- - - - - - -
// In the following, we informally refer to the "element types" of a
// 'bdem' aggregate.  The element types of a row or list are the sequence of
// 'bdem' element types corresponding to its contained elements, and the
// element types of a table are its column types.  The element types of a
// choice, choice array, or choice item are the set of types in its type
// catalog (note that the order of the set members is not salient).
//
///Schema Record Types
///- - - - - - - - - -
// Every schema record definition has a record type, which is either
// *sequence* (meaning
// 'bdem_RecordDef::BDEM_SEQUENCE_RECORD == bdem_RecordDef::recordType()') or
// *choice* (meaning
// 'bdem_RecordDef::BDEM_CHOICE_RECORD == bdem_RecordDef::recordType()').  A
// *sequence* record definition applies to aggregates of types 'BDEM_LIST',
// 'BDEM_ROW', and 'BDEM_TABLE'.  A *choice* record definition applies to
// objects of type 'BDEM_CHOICE', 'BDEM_CHOICE_ARRAY', and
// 'BDEM_CHOICE_ARRAY_ITEM'.
//
///Conformance
///- - - - - -
// Conformance, in its three varieties (*shallow* *conformance*, *conformance*,
// and *deep* *conformance*), is a relationship between an object of 'bdem'
// aggregate type and a record.  In general, conformance considers both the
// element types of a 'bdem' aggregate object, as well as the structure of any
// contained 'BDEM_LIST', 'BDEM_TABLE', 'BDEM_CHOICE', and 'BDEM_CHOICE_ARRAY'
// elements, in relation to the sequence of field types and any
// enumeration-based or record-based constraints imposed by a given record.
//
///Conformance to an Enumeration Constraint
///-  -  -  -  -  -  -  -  -  -  -  -  -  -
// An enumeration constraint can be specified for a 'BDEM_INT', 'BDEM_STRING',
// 'BDEM_INT_ARRAY', or 'BDEM_STRING_ARRAY'.  A 'BDEM_INT' or 'BDEM_STRING'
// satisfies the constraint if it is either null or has one of the values in
// the enumeration constraint.  A 'BDEM_INT_ARRAY' or 'BDEM_STRING_ARRAY'
// are conformant to the constraint if all the elements in the array have
// values that are either null or are in the enumeration constraint.  Note that
// the component 'bdem_schemaenumerationutil' evaluates such conformance.
//
///Shallow Conformance
///-  -  -  -  -  -  -
// An aggregate 'A' is *shallow* *conformant* to a record 'R' if the number and
// types of elements of 'A' (or the catalog types of 'A' in the case of choice
// items, choices, and choice arrays) exactly match the number and types of
// field types in 'R', without regard to either constraints on the individual
// fields or their nullability status.  Note that actual values of elements
// contained in 'A' are completely ignored.
//
///Conformance
/// -  -  -  -
// An aggregate 'A' is *conformant* to a record 'R' if 'A' is shallow
// conformant to 'R', and every contained aggregate or enumerated element
// within 'A' (at any iterative depth of nesting) is either (1) unconstrained,
// (2) null, or (3) conformant to any record definition or enumeration
// constraint constraining it.  Note that conformance always implies shallow
// conformance and satisfaction (below).
//
///Deep Conformance
///-  -  -  -  -  -
// An aggregate or row 'A' is *deep* *conformant* to a record 'R' if 'A' is
// shallow conformant to 'R', and every contained aggregate or enumerated
// element within 'A' (at any iterative depth of nesting) is either (1)
// unconstrained, (2) null with the corresponding field in the constraining
// record marked 'nullable', or (3) deep conformant to the corresponding
// constraint in 'R'.  Note that deep conformance always implies conformance,
// shallow conformance, and satisfaction (below).
//
///Satisfying a Record Definition with an Aggregate
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// An aggregate or row 'A' *can* *satisfy* a record 'R', where 'A' (or its
// catalog) has 'M' types and 'R' has 'N' fields if 'M' >= 'N' and the first
// 'N' fields of 'A' are conformant to 'R'.  Note that for any form of
// "conformance", 'M' must be equal to 'N', while "satisfaction" allows 'A' to
// contain a superset of the data as specified by 'R'.  Note that if 'M' > 'N',
// the fields after the first 'N' fields of 'A' are ignored.  Note that deep
// conformance and conformance, but not shallow conformance, always imply
// satisfaction.
//
///Valid and Invalid Schemas
///- - - - - - - - - - - - -
// A schema is *invalid* if a recursion loop exists through fields of type
// 'BDEM_LIST' that are not marked as 'nullable', which will lead to infinite
// recursion when called with 'initListDeep', or if any constrained enumerated
// field has a default value that is not among the values in the enumeration
// constraint.  Note that a schema is always either valid or invalid.
//
///Usage
///-----
// In mortgage bonds, something like 1000 home loans are packaged together in
// a "deal", which is a family of bonds.  As homeowners pay off their
// mortgages, the money flows into the deal, and then payments are made to
// several different bonds.  Some bonds in the deal are paid before others, the
// bonds paid last take the most risk.  The first bonds to be paid are "senior"
// and have good ratings.  The "junior" bonds are riskier, have lower ratings,
// but pay higher interest.  We will have two bonds from the same deal,
// "ACME-04-D", a senior bond ("ACME-04-D A") and a junior bond
// ("ACME-04-D J").
//
// We will outline a schema to describe a mortgage bond.  Here is the
// a sketch of what the schema will look like:
//..
//  +-------------------------------------------------------------------------+
//  |{                                                                        |
//  |    ENUMERATION "RATING" {                                               |
//  |        "AAA"                                                            |
//  |        "AA"                                                             |
//  |        "A"                                                              |
//  |        "BBB"                                                            |
//  |        "BB"                                                             |
//  |        "B"                                                              |
//  |        "CCC"                                                            |
//  |        "CC"                                                             |
//  |        "C"                                                              |
//  |        "D"                                                              |
//  |        "NR"                                                             |
//  |    }                                                                    |
//  |                                                                         |
//  |    RECORD "LOCATION" {                                                  |
//  |        STRING    "state";                                               |
//  |        DOUBLE    "percent";    // percent of homes in deal in state     |
//  |    }                                                                    |
//  |                                                                         |
//  |    RECORD "DELINQUENCY" {                                               |
//  |        DOUBLE    "percentDelinq60Plus";  // homeowners > 90 days behind |
//  |        DOUBLE    "percentDelinq90Plus";  // homeowners > 90 days behind |
//  |        DOUBLE    "percentForeclosed";    // homeowners foreclosed upon  |
//  |    }                                                                    |
//  |                                                                         |
//  |    RECORD "BOND" {                                                      |
//  |        STRING    "name";                                                |
//  |        STRING    "rating"    CONSTRAINED BY "RATING" DEFAULT "AAA";     |
//  |        TABLE     "locationOfHomes"    CONSTRAINED BY "LOCATION";        |
//  |        LIST      "delinquency"    CONSTRAINED BY "DELINQUENCY";         |
//  |        INT       "avgFICOScore";   // average FICO score of homeowners  |
//  |                                    // in deal                           |
//  |        DOUBLE    "percentInterest";    // annual interest %             |
//  |    }                                                                    |
//  |                                                                         |
//  |    // note all records are of sequence type                             |
//  |                                                                         |
//  +-------------------------------------------------------------------------+
//..
// We now illustrate the code to code used to build it.  We start by creating
// a modifiable schema, 'mS', and a non-modifiable reference to it, 'S':
//..
//  bdem_Schema mS;    const bdem_Schema& S = mS;
//..
// Next, we create the "RATING" enumeration constraint.
//..
//  bdem_EnumerationDef *rating = mS.createEnumeration("RATING");
//  const char *ratings[] = { "AAA", "AA", "A", "BBB", "BB", "B",
//                            "CCC", "CC", "C", "D", "NR" };
//  const int NUM_RATINGS = sizeof ratings / sizeof *ratings;
//  for (int i = 0; i < NUM_RATINGS; ++i) {
//      rating->addEnumerator(ratings[i]);
//  }
//..
// Now create the "LOCATION" sequence record definition.
//..
//  bdem_RecordDef *location = mS.createRecord("LOCATION");
//  location->appendField(bdem_ElemType::BDEM_STRING, "state");
//  location->appendField(bdem_ElemType::BDEM_DOUBLE, "percent");
//
//  const int stateIndex = location->fieldIndex("state");
//  const int percentIndex = location->fieldIndex("percent");
//..
// Next, the "DELINQUENCY" sequence record definition.
//..
//  bdem_RecordDef *delinquency = mS.createRecord("DELINQUENCY");
//  delinquency->appendField(bdem_ElemType::BDEM_DOUBLE,
//                           "percentDelinq60Plus");
//  delinquency->appendField(bdem_ElemType::BDEM_DOUBLE,
//                           "percentDelinq90Plus");
//  delinquency->appendField(bdem_ElemType::BDEM_DOUBLE,
//                           "percentForeclosed");
//
//  const int sixtyIndex = delinquency->fieldIndex("percentDelinq60Plus");
//  const int ninetyIndex = delinquency->fieldIndex("percentDelinq90Plus");
//  const int forecloseIndex =delinquency->fieldIndex("percentForeclosed");
//..
// Then, the "BOND" sequence record definition.
//..
//  bdem_RecordDef *bond = mS.createRecord("BOND");
//  bond->appendField(bdem_ElemType::BDEM_STRING, "name");
//  {
//      bdem_FieldDefAttributes fieldAttr(bdem_ElemType::BDEM_STRING);
//      fieldAttr.defaultValue().theModifiableString() = "AAA";
//      bond->appendField(fieldAttr, rating, "rating");
//  }
//  bond->appendField(bdem_ElemType::BDEM_TABLE,
//                    location,
//                    "locationOfHomes");
//  assert(0 == bond->lookupField("locationOfHomes")->isNullable());
//  bond->appendField(bdem_ElemType::BDEM_LIST,
//                    delinquency,
//                    "delinquency");
//  assert(0 == bond->lookupField("delinquency")->isNullable());
//  bond->appendField(bdem_ElemType::BDEM_INT, "avgFICOScore");
//  bond->appendField(bdem_ElemType::BDEM_DOUBLE, "percentInterest");
//
//  const int nameIndex = bond->fieldIndex("name");
//  const int ratingIndex = bond->fieldIndex("rating");
//  const int locationIndex = bond->fieldIndex("locationOfHomes");
//  const int delinquencyIndex = bond->fieldIndex("delinquency");
//  const int ficoIndex = bond->fieldIndex("avgFICOScore");
//  const int interestIndex = bond->fieldIndex("percentInterest");
//..
// We're done with the definitions, now let's start initializing some data.
//..
//  bdem_List acme04DA;
//  typedef bdem_SchemaAggregateUtil Util;
//..
// 'initListAllNull' will satisfy all forms of conformance and satisfaction
// except for deep conformance:
//..
//  Util::initListAllNull(&acme04DA, *bond);
//  assert( Util::isListShallowConformant(acme04DA, *bond));
//  assert( Util::isListConformant(       acme04DA, *bond));
//  assert(!Util::isListDeepConformant(   acme04DA, *bond));
//  assert( Util::canSatisfyRecord(       acme04DA, *bond));
//..
// Verify that the types of a few of the fields are correct.
//..
//  assert(acme04DA[ratingIndex].type() == bdem_ElemType::BDEM_STRING);
//  assert(acme04DA[ratingIndex].isNull());
//  assert(acme04DA[locationIndex].type() == bdem_ElemType::BDEM_TABLE);
//  assert(acme04DA[locationIndex].isNull());
//  assert(acme04DA[delinquencyIndex].type() == bdem_ElemType::BDEM_LIST);
//  assert(acme04DA[delinquencyIndex].isNull());
//..
// Note that 'initListAllNull' did not initialize contained aggregates.
//..
//  assert(!Util::isTableShallowConformant(
//                         acme04DA[locationIndex].theTable(), *location));
//  assert(!Util::isListShallowConformant(
//                    acme04DA[delinquencyIndex].theList(), *delinquency));
//..
// 'initListShallow' is like 'initListAllNull' except that it initializes
// default values.
//..
//  Util::initListShallow(&acme04DA, *bond);
//  assert( Util::isListShallowConformant(acme04DA, *bond));
//  assert( Util::isListConformant(       acme04DA, *bond));
//  assert(!Util::isListDeepConformant(   acme04DA, *bond));
//  assert( Util::canSatisfyRecord(       acme04DA, *bond));
//..
// The types are as before, except this time the rating has been initialized to
// its default value:
//..
//  assert(acme04DA[ratingIndex].type() == bdem_ElemType::BDEM_STRING);
//  assert(acme04DA[ratingIndex].theString() == "AAA");        // default value
//  assert(acme04DA[locationIndex].type() == bdem_ElemType::BDEM_TABLE);
//  assert(acme04DA[locationIndex].isNull());
//  assert(acme04DA[delinquencyIndex].type() == bdem_ElemType::BDEM_LIST);
//  assert(acme04DA[delinquencyIndex].isNull());
//..
// Note that 'initListShallow' did not initialize sub-aggregates.
//..
//  assert(!Util::isTableShallowConformant(
//                         acme04DA[locationIndex].theTable(), *location));
//  assert(!Util::isListShallowConformant(
//                    acme04DA[delinquencyIndex].theList(), *delinquency));
//..
// 'initListDeep' will deeply initialize sub-aggregates.
//..
//  Util::initListDeep(&acme04DA, *bond);
//  assert( Util::isListShallowConformant(acme04DA, *bond));
//  assert( Util::isListConformant(       acme04DA, *bond));
//  assert( Util::isListDeepConformant(   acme04DA, *bond));
//  assert( Util::canSatisfyRecord(       acme04DA, *bond));
//
//  assert( acme04DA[ratingIndex].type() == bdem_ElemType::BDEM_STRING);
//  assert( acme04DA[ratingIndex].theString() == "AAA");    // default value
//  assert( acme04DA[locationIndex].type() == bdem_ElemType::BDEM_TABLE);
//  assert(!acme04DA[locationIndex].isNull());
//  assert( acme04DA[delinquencyIndex].type() == bdem_ElemType::BDEM_LIST);
//  assert(!acme04DA[delinquencyIndex].isNull());
//
//  bdem_Table *table = &acme04DA[locationIndex].theModifiableTable();
//  bdem_List  *list  = &acme04DA[delinquencyIndex].theModifiableList();
//
//  assert( Util::isTableShallowConformant(*table, *location));
//  assert( Util::isTableConformant(       *table, *location));
//  assert( Util::isTableDeepConformant(   *table, *location));
//  assert( Util::canSatisfyRecord(        *table, *location));
//
//  assert( Util::isListShallowConformant(*list, *delinquency));
//  assert( Util::isListConformant(       *list, *delinquency));
//  assert( Util::isListDeepConformant(   *list, *delinquency));
//  assert( Util::canSatisfyRecord(       *list, *delinquency));
//..
// Now populate the new bond with some actual data:
//..
//  acme04DA[nameIndex].theModifiableString()   = "ACME-04-D A";
//  acme04DA[ratingIndex].theModifiableString() = "AA";
//
//  {
//      bdem_List locationList;
//      Util::initListShallow(&locationList, *location);
//      locationList[stateIndex].theModifiableString() = "NJ";
//      locationList[percentIndex].theModifiableDouble() = 35.7;
//
//      assert( Util::isListDeepConformant(locationList, *location));
//      table->appendRow(locationList);
//
//      locationList[stateIndex].theModifiableString() = "NY";
//      locationList[percentIndex].theModifiableDouble() = 21.3;
//      table->appendRow(locationList);
//
//      locationList[stateIndex].theModifiableString() = "PA";
//      locationList[percentIndex].theModifiableDouble() = 12.6;
//      table->appendRow(locationList);
//
//      assert( Util::isTableDeepConformant(*table, *location));
//  }
//..
// Populate the delinquency list.
//..
//  (*list)[sixtyIndex].theModifiableDouble()     = 5.7;
//  (*list)[ninetyIndex].theModifiableDouble()    = 3.2;
//  (*list)[forecloseIndex].theModifiableDouble() = 2.1;
//
//  acme04DA[ficoIndex].theModifiableInt()        = 703;
//  acme04DA[interestIndex].theModifiableDouble() = 5.5;
//
//  assert( Util::isListShallowConformant(acme04DA, *bond));
//  assert( Util::isListConformant(       acme04DA, *bond));
//  assert( Util::isListDeepConformant(   acme04DA, *bond));
//  assert( Util::canSatisfyRecord(       acme04DA, *bond));
//..
// Print out the bond.
//..
//  cout << "ACME-04-D A:" << acme04DA << endl;
//..
// The resulting single-line of output is:
//..
//  ACME-04-D A:{ STRING ACME-04-D A STRING AA TABLE { Column Types: [ STRING
//  DOUBLE ] Row 0: { NJ 35.7 } Row 1: { NY 21.3 } Row 2: { PA 12.6 } } LIST {
//  DOUBLE 5.7 DOUBLE 3.2 DOUBLE 2.1 } INT 703 DOUBLE 5.5 }
//..
// Create another bond, "ACME-04-D J" - it is from the same "deal" or family of
// bonds, so much of the data will be the same, but it is of a lower tranche,
// so its rating will be lower and it will pay better interest:
//..
//  bdem_List acme04DJ = acme04DA;
//
//  acme04DJ[nameIndex].theModifiableString()     = "ACME-04-D J";
//  acme04DJ[ratingIndex].theModifiableString()   = "C";
//  acme04DJ[interestIndex].theModifiableDouble() = 6.1;
//
//  assert(Util::isListDeepConformant(acme04DJ, *bond));
//
//  cout << "ACME-04-D J:" << acme04DJ << endl;
//..
// The resulting (single-line of) output is:
//..
//  ACME-04-D J:{ STRING ACME-04-D J STRING C TABLE { Column Types: [ STRING
//  DOUBLE ] Row 0: { NJ 35.7 } Row 1: { NY 21.3 } Row 2: { PA 12.6 } } LIST {
//  DOUBLE 5.7 DOUBLE 3.2 DOUBLE 2.1 } INT 703 DOUBLE 6.1 }
//..
// Now we create another bond that we will manipulate to test various types of
// conformance and satisfaction.
//..
//  bdem_List testBond(acme04DJ);
//  assert(Util::isListDeepConformant(   testBond, *bond));
//..
// Note that appending a stray element breaks all forms of conformance, but not
// satisfaction.
//..
//  testBond.appendInt(5);
//  assert(!Util::isListShallowConformant(testBond, *bond));
//  assert(!Util::isListConformant(       testBond, *bond));
//  assert(!Util::isListDeepConformant(   testBond, *bond));
//  assert( Util::canSatisfyRecord(       testBond, *bond));
//  testBond.removeElement(testBond.length() - 1);
//  assert( Util::isListDeepConformant(   testBond, *bond));
//..
// Setting the rating (which is constrained by an enum) to an invalid value is
// caught by everything but shallow conformance.
//..
//  testBond[ratingIndex].theModifiableString() = "Z";
//  assert( Util::isListShallowConformant(testBond, *bond));
//  assert(!Util::isListConformant(       testBond, *bond));
//  assert(!Util::isListDeepConformant(   testBond, *bond));
//  assert(!Util::canSatisfyRecord(       testBond, *bond));
//  testBond[ratingIndex].theModifiableString() = "AA";
//
//  assert( Util::isListDeepConformant(   testBond, *bond));
//..
// Changing the type of the last element breaks all conformance and
// satisfaction.
//..
//  assert(testBond.length() - 1 == interestIndex);
//  assert(testBond[interestIndex].type() == bdem_ElemType::BDEM_DOUBLE);
//  testBond.removeElement(interestIndex);
//  testBond.appendNullElement(bdem_ElemType::BDEM_INT);
//  assert(!Util::isListShallowConformant(testBond, *bond));
//  assert(!Util::isListConformant(       testBond, *bond));
//  assert(!Util::isListDeepConformant(   testBond, *bond));
//  assert(!Util::canSatisfyRecord(       testBond, *bond));
//
//  testBond.removeElement(interestIndex);
//  testBond.appendNullElement(bdem_ElemType::BDEM_DOUBLE);
//  assert( Util::isListDeepConformant(   testBond, *bond));
//..
// Setting the delinquency sublist to null only causes deep conformance to
// return 'false', other types of conformance and satisfaction are unaffected.
//..
//  testBond[delinquencyIndex].makeNull();
//  assert(1 == Util::isListShallowConformant(testBond, *bond));
//  assert(1 == Util::isListConformant(       testBond, *bond));
//  assert(0 == Util::isListDeepConformant(   testBond, *bond));
//  assert(1 == Util::canSatisfyRecord(       testBond, *bond));
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_AGGREGATE
#include <bdem_aggregate.h>
#endif

namespace BloombergLP {

class bdem_RecordDef;

                        // ===============================
                        // struct bdem_SchemaAggregateUtil
                        // ===============================

struct bdem_SchemaAggregateUtil {
    // This class provides a namespace for a suite of pure procedures that
    // operate simultaneously on 'bdem_RecordDef' objects and objects of
    // 'bdem' aggregate type (i.e., 'bdem_Choice', 'bdem_ChoiceArray',
    // 'bdem_ChoiceArrayItem', 'bdem_List', 'bdem_Row', and 'bdem_Table').

    // CLASS METHODS
    static
    bool canSatisfyRecord(const bdem_Row&       object,
                          const bdem_RecordDef& record);
    static
    bool canSatisfyRecord(const bdem_List&      object,
                          const bdem_RecordDef& record);
    static
    bool canSatisfyRecord(const bdem_Table&     object,
                          const bdem_RecordDef& record);
    static
    bool canSatisfyRecord(const bdem_Choice&    object,
                          const bdem_RecordDef& record);
    static
    bool canSatisfyRecord(const bdem_ChoiceArray& object,
                          const bdem_RecordDef&   record);
    static
    bool canSatisfyRecord(const bdem_ChoiceArrayItem& object,
                          const bdem_RecordDef&       record);
        // Return 'true' if the specified 'object' can satisfy the specified
        // 'record' and 'false' otherwise.  The behavior is undefined if
        // 'object' is a row, list, or table and 'record' represents a choice
        // or if 'object' is a choice, choice array, or choice array item and
        // 'record' represents a sequence', or if 'record' belongs to an
        // invalid schema.

    static
    void initRowShallow(bdem_Row *row, const bdem_RecordDef& record);
        // Set each element of scalar type in the specified 'row' to the
        // default value indicated by the corresponding field of the specified
        // 'record', if such a default values exists, and initialize all other
        // elements to null.  The behavior is undefined unless the sequence of
        // element types in 'row' matches the sequence of field types in
        // 'record',  the 'record' represents a sequence, and 'record' belongs
        // to a valid schema.  Note that a row initialized with this function
        // will be conformant with 'record'.

    static
    void initRowDeep(bdem_Row *row, const bdem_RecordDef& record);
        // Set each element of scalar type in the specified 'row' to the
        // default value indicated by the corresponding field of the specified
        // 'record', if such a default values exists, and initialize all other
        // scalar elements to null.  Set each element of aggregate type in
        // 'row' to the null value if the corresponding element in 'record' is
        // nullable, or otherwise recursively populate that element according
        // to its record constraint (in the corresponding element of 'record').
        // The behavior is undefined unless the sequence of element types in
        // 'row' matches the sequence of field types in 'record', the 'record'
        // represents a sequence, and 'record' belongs to a valid schema.  Note
        // that a row initialized with this function will be conformant with
        // 'record'.

    static
    void initListAllNull(bdem_List *list, const bdem_RecordDef& record);
        // Set the element types of the specified 'list' to match the field
        // types in the specified 'record', with each element being set to
        // null.  The behavior is undefined unless 'record' represents a
        // sequence.  Note that any initial elements of 'list' (types and
        // values) are lost.  Also note that a list initialized with this
        // function will be conformant with 'record'.

    static
    void initListShallow(bdem_List *list, const bdem_RecordDef& record);
        // Set each element of scalar type in the specified 'list' to the
        // default value indicated by the corresponding field of the specified
        // 'record', if such a default values exists, and initialize all other
        // elements to null.  The behavior is undefined unless the sequence of
        // element types in 'list' matches the sequence of field types in
        // 'record',  the 'record' represents a sequence, and 'record' belongs
        // to a valid schema.  Note that a list initialized with this function
        // will be conformant with 'record'.    Also note that the only
        // difference between this function and 'initListAllNull' is the
        // setting of scalar elements to default values.

    static
    void initListDeep(bdem_List *list, const bdem_RecordDef& record);
        // Set each element of scalar type in the specified 'list' to the
        // default value indicated by the corresponding field of the specified
        // 'record', if such a default values exists, and initialize all other
        // scalar elements to null.  Set each element of aggregate type in
        // 'list' to the null value if the corresponding element in 'record' is
        // nullable, or otherwise recursively populate that element according
        // to its record constraint (in the corresponding element of 'record').
        // The behavior is undefined unless 'record' represents a sequence, and
        // 'record' belongs to a valid schema.  Note that when sub-aggregates
        // are initialized to non-null, lists will be initialized with
        // 'initListDeep', tables will be initialized with 'initTable', choices
        // with 'initChoice', and choice arrays with 'initChoiceArray'.  Also
        // note that, unlike 'initListDeep', this function erases all initial
        // state of 'list', including the types of elements.  Also note that a
        // list initialized with this function will be deep conformant with
        // 'record'.

    static
    void initTable(bdem_Table *table, const bdem_RecordDef& record);
        // Discard any rows from the specified table and set the column types
        // to match those of the respective field definitions in the specified
        // 'record'.  The behavior is undefined unless 'record' represents a
        // sequence.

    static
    void initChoice(bdem_Choice *choice, const bdem_RecordDef& record);
        // Set the types catalog of the specified 'choice' to match the types
        // of the respective field definitions in the specified 'record', and
        // set the selection to '-1'.  The behavior is undefined unless
        // 'record' represents a choice.

    static
    void initChoiceArray(bdem_ChoiceArray      *choiceArray,
                         const bdem_RecordDef&  record);
        // Discard any elements from the specified 'choiceArray' and set its
        // types catalog to match the types of the respective field definitions
        // in the specified 'record'.  The behavior is undefined unless
        // 'record' represents a choice.

    static
    bool isRowShallowConformant(const bdem_Row&       row,
                                const bdem_RecordDef& record);
        // Return 'true' if the specified 'row' is shallow conformant to the
        // specified 'record', and 'false' otherwise.  'row' is shallow
        // conformant to 'record' if each element in 'row' is of the same type
        // as the corresponding field definition in 'record', without regard to
        // either constraint or nullability status.  The behavior is undefined
        // unless 'record' represents a sequence.

    static
    bool isRowConformant(const bdem_Row& row, const bdem_RecordDef& record);
        // Return 'true' if the specified 'row' is conformant to the specified
        // 'record', and 'false' otherwise.  'row' is conformant to 'record'
        // if each element in 'row' is of the same type as the corresponding
        // field definition in 'record' (i.e., it is shallow conformant) and,
        // for each row element containing an aggregate or enumerated value,
        // that value is either (1) unconstrained in that the corresponding
        // field definition does not define a constraint, (2) null, or (3)
        // conformant to the record or enumeration constraint of the
        // corresponding field definition.  The behavior is undefined unless
        // 'record' represents a sequence and belongs to a valid schema.

    static
    bool isRowDeepConformant(const bdem_Row&       row,
                             const bdem_RecordDef& record);
        // Return 'true' if the specified 'row' is deep conformant to the
        // specified record, and 'false' otherwise.  'row' is deep conformant
        // to 'record' if each element in 'row' is of the same type as the
        // corresponding field definition in 'record' and, for each row
        // element containing an aggregate or enumerated value, that value is
        // either (1) unconstrained in that the corresponding field definition
        // does not define a constraint, (2) null and its corresponding field
        // definition is marked 'nullable', or (3) conformant to the record or
        // enumeration constraint of the corresponding field definition.  The
        // behavior is undefined unless 'record' represents a sequence and
        // belongs to a valid schema.  Note that deep conformance always
        // implies both conformance and shallow conformance.

    static
    bool isListShallowConformant(const bdem_List&      list,
                                 const bdem_RecordDef& record);
        // Return 'true' if the specified 'list' is shallow conformant to the
        // specified 'record', and 'false' otherwise.  'list' is shallow
        // conformant to 'record' if each element in 'list' is of the same
        // type as the corresponding field definition in 'record', without
        // regard either to constraint or nullability status.  The behavior is
        // undefined unless 'record' represents a sequence.  Note that this
        // function is identical to the following:
        //..
        //  isRowShallowConformant(list.theRow(), record);
        //..

    static
    bool isListConformant(const bdem_List& list, const bdem_RecordDef& record);
        // Return 'true' if the specified 'list' is conformant to the
        // specified 'record', and 'false' otherwise.  'list' is conformant to
        // 'record' if each element in 'list' is of the same type as the
        // corresponding field definition in 'record' (i.e., it is shallow
        // conformant) and, for each list element containing an aggregate or
        // enumerated value, that value is either (1) unconstrained in that
        // the corresponding field definition does not define a constraint,
        // (2) null, or (3) conformant to the record or enumeration constraint
        // of the corresponding field definition.  The behavior is undefined
        // unless 'record' represents a sequence and belongs to a valid schema.
        // Note that this function is identical to the following:
        //..
        //  isRowConformant(list.theRow(), record);
        //..

    static
    bool isListDeepConformant(const bdem_List&      list,
                              const bdem_RecordDef& record);
        // Return 'true' if the specified 'list' is deep conformant to the
        // specified record, and 'false' otherwise.  'list' is deep conformant
        // to 'record' if each element in 'list' is of the same type as the
        // corresponding field definition in 'record' and, for each list
        // element containing an aggregate or enumerated value, that value is
        // either (1) unconstrained in that the corresponding field definition
        // does not define a constraint, (2) null and its corresponding field
        // definition is marked 'nullable', or (3) conformant to the record or
        // enumeration constraint of the corresponding field definition.  The
        // behavior is undefined unless the record represents a sequence and
        // belongs to a valid schema.  Note that deep conformance always
        // implies both conformance and shallow conformance.  Also note that
        // this function is identical to the following:
        //..
        //  isRowDeepConformant(list.theRow(), record);
        //..

    static
    bool isTableShallowConformant(const bdem_Table&     table,
                                  const bdem_RecordDef& record);
        // Return 'true' if the specified 'table' is shallow conformant to the
        // specified 'record', and 'false' otherwise.  'table' is shallow
        // conformant to 'record' if each row in 'table' is shallow conformant
        // to 'record', meaning that for every row in 'table', each row element
        // is of the same type as the corresponding field definition in
        // 'record', without regard to either constraint or nullability status.
        // The behavior is undefined unless record represents a sequence.

    static
    bool isTableConformant(const bdem_Table&     table,
                           const bdem_RecordDef& record);
        // Return 'true' if the specified 'table' is conformant to the
        // specified 'record', and 'false' otherwise.  'table' is conformant
        // to 'record' if each row in 'table' is conformant to 'record',
        // meaning that for every row in 'table', each row element is of the
        // same type as the corresponding field definition in 'record' (i.e.,
        // it is shallow conformant) and, for each row element containing an
        // aggregate or enumerated value, that value is either (1)
        // unconstrained in that the corresponding field definition does not
        // define a constraint, (2) null, or (3) conformant to the record or
        // enumeration constraint of the corresponding field definition.  The
        // behavior is undefined unless 'record' represents a sequence and
        // belongs to a valid schema.

    static
    bool isTableDeepConformant(const bdem_Table&     table,
                               const bdem_RecordDef& record);
        // Return 'true' if the specified 'table' is deep conformant to the
        // specified 'record', and 'false' otherwise.  'table' is deep
        // conformant to 'record' if each row in 'table' is deep conformant to
        // 'record', meaning that for every row in 'table', each row element
        // is of the same type as the corresponding field definition in
        // 'record' and, for each row element containing an aggregate or
        // enumerated value, that value is either (1) unconstrained in that
        // the corresponding field definition does not define a constraint,
        // (2) null and its corresponding field definition is marked
        // 'nullable', or (3) conformant to the record or enumeration
        // constraint of the corresponding field definition.  The behavior is
        // undefined unless 'record' represents a sequence and belongs to a
        // valid schema.  Note that deep conformance always implies both
        // conformance and shallow conformance.

    static
    bool isChoiceShallowConformant(const bdem_Choice&    choice,
                                   const bdem_RecordDef& record);
        // Return 'true' if the specified 'choice' is shallow conformant to the
        // specified 'record', and 'false' otherwise.  'choice' is shallow
        // conformant to 'record' if each possible selection in 'choice' is
        // of the same type as the corresponding field definition in 'record',
        // without regard either to constraint or nullability status.  Note
        // that the selection held by 'choice' is not verified.  The behavior
        // is undefined unless 'record' represents a choice.

    static
    bool isChoiceConformant(const bdem_Choice&    choice,
                            const bdem_RecordDef& record);
        // Return 'true' if the specified 'choice' is conformant to the
        // specified 'record', and 'false' otherwise.  'choice' is conformant
        // to 'record' if each possible selection in 'choice' is of the same
        // type as the corresponding field definition in 'record' (i.e., it is
        // shallow conformant) and, if the current selection is an aggregate
        // or enumerated value, that value is either (1) unconstrained, in that
        // the corresponding field definition does not define a constraint,
        // (2) null, or (3) conformant to the record or enumeration constraint
        // of the corresponding field definition.  The behavior is undefined
        // unless 'record' represents a choice and belongs to a valid schema.

    static
    bool isChoiceDeepConformant(const bdem_Choice&    choice,
                                const bdem_RecordDef& record);
        // Return 'true' if the specified 'choice' is deep conformant to the
        // specified 'record', and 'false' otherwise.  'choice' is deep
        // conformant to 'record' if each possible selection in 'choice' is of
        // the same type as the corresponding field definition in 'record'
        // and, if the current selection is an aggregate or enumerated value,
        // that value is either (1) unconstrained in that the corresponding
        // field definition does not define a constraint, (2) null and its
        // corresponding field definition is marked 'nullable', or (3)
        // conformant to the record or enumeration constraint of the
        // corresponding field definition.  The behavior is undefined unless
        // 'record' represents a choice and belongs to a valid schema.  Note
        // that deep conformance always implies both conformance and shallow
        // conformance.

    static
    bool isChoiceArrayShallowConformant(const bdem_ChoiceArray& choiceArray,
                                        const bdem_RecordDef&   record);
        // Return 'true' if the specified 'choiceArray' is shallow conformant
        // to the specified 'record', and 'false' otherwise.  'choiceArray' is
        // shallow conformant to 'record' if each choice item in 'choiceArray'
        // is shallow conformant to 'record', meaning that for every choice
        // item in 'choiceArray', each possible selection is of the same type
        // as the corresponding field definition in 'record', without regard
        // either to constraint or nullability status.  The behavior is
        // undefined unless 'record' represents a choice.  Note that the
        // selection values of the choice items in 'choiceArray' are not
        // verified.

    static
    bool isChoiceArrayConformant(const bdem_ChoiceArray& choiceArray,
                                 const bdem_RecordDef&   record);
        // Return 'true' if the specified 'choiceArray' is conformant to the
        // specified 'record', and 'false' otherwise.  'choiceArray' is
        // conformant to 'record' if each choice item in 'choiceArray' is
        // conformant to 'record', meaning that for every choice item in
        // 'choiceArray', each possible selection is of the same type as the
        // corresponding field definition in 'record' (i.e., it is shallow
        // conformant) and, if the current selection is an aggregate or
        // enumerated value, that value is either (1) unconstrained in that
        // the corresponding field definition does not define a constraint,
        // (2) null, or (3) conformant to the record or enumeration constraint
        // of the corresponding field definition.  The behavior is undefined
        // unless 'record' represents a choice and belongs to a valid schema.

    static
    bool isChoiceArrayDeepConformant(const bdem_ChoiceArray& choiceArray,
                                     const bdem_RecordDef&   record);
        // Return 'true' if the specified 'choiceArray' is deep conformant to
        // the specified 'record', and 'false' otherwise.  'choiceArray' is
        // deep conformant to 'record' if each choice item in 'choiceArray' is
        // deep conformant to 'record', meaning that for every choice item in
        // 'choiceArray', each possible selection is of the same type as the
        // corresponding field definition in 'record' and, if the current
        // selection is an aggregate or enumerated value, that value is either
        // (1) unconstrained in that the corresponding field definition does
        // not define a constraint, (2) null and its corresponding field
        // definition is marked 'nullable', or (3) conformant to the record or
        // enumeration constraint of the corresponding field definition.  The
        // behavior is undefined unless 'record' represents a choice and
        // belongs to a valid schema.

    static
    bool isChoiceArrayItemShallowConformant(
                                   const bdem_ChoiceArrayItem& choiceArrayItem,
                                   const bdem_RecordDef&       record);
        // Return 'true' if the specified 'choiceArrayItem' is shallow
        // conformant to the specified 'record', and 'false' otherwise.
        // 'choiceArrayItem' is shallow conformant to 'record' if each possible
        // selection in 'choiceArrayItem' is of the same type as the
        // corresponding field definition in 'record', without regard either to
        // constraint or nullability status.  Note that the selection held by
        // 'choiceArrayItem' is not verified.  The behavior is undefined unless
        // 'record' represents a choice.

    static
    bool isChoiceArrayItemConformant(
                                   const bdem_ChoiceArrayItem& choiceArrayItem,
                                   const bdem_RecordDef&       record);
        // Return 'true' if the specified 'choiceArrayItem' is conformant to
        // the specified 'record', and 'false' otherwise.  'choiceArrayItem'
        // is conformant to 'record' if each possible selection in
        // 'choiceArrayItem' is of the same type as the corresponding field
        // definition in 'record' (i.e., it is shallow conformant) and, if the
        // current selection is an aggregate or enumerated value, that value
        // is either (1) unconstrained in that the corresponding field
        // definition does not define a constraint, (2) null, or (3) conformant
        // to the record or enumeration constraint of the corresponding field
        // definition.  The behavior is undefined unless 'record' represents a
        // choice and belongs to a valid schema.

    static
    bool isChoiceArrayItemDeepConformant(
                                   const bdem_ChoiceArrayItem& choiceArrayItem,
                                   const bdem_RecordDef&       record);
        // Return 'true' if the specified 'choiceArrayItem' is deep conformant
        // to the specified 'record', and 'false' otherwise.  'choiceArrayItem'
        // is deep conformant to 'record' if each possible selection in
        // 'choiceArrayItem' is of the same type as the corresponding field
        // definition in 'record' and, if the current selection is an aggregate
        // or enumerated value, that value is either (1) unconstrained in that
        // the corresponding field definition does not define a constraint, (2)
        // null and its corresponding field definition is marked 'nullable', or
        // (3) conformant to the record or enumeration constraint of the
        // corresponding field definition.  The behavior is undefined unless
        // 'record' represents a choice and belongs to a valid schema.  Note
        // that deep conformance always implies both conformance and shallow
        // conformance.

    static bsl::ostream& print(bsl::ostream&             stream,
                               const bdem_ConstElemRef&  element,
                               const bdem_RecordDef     *constraint     = 0,
                               int                       level          = 0,
                               int                       spacesPerLevel = 4);
        // Format the specified 'element' (type, name, and value), constrained
        // by the optionally specified 'constraint', to the specified output
        // 'stream' at the (absolute value of) the optionally specified
        // indentation 'level' and return a reference to 'stream'.  If
        // 'constraint' is 0, the 'item' will be printed using its regular
        // 'print' method.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    static bsl::ostream& print(bsl::ostream&         stream,
                               const bdem_Row&       row,
                               const bdem_RecordDef& record,
                               int                   level          = 0,
                               int                   spacesPerLevel = 4);
    static bsl::ostream& print(bsl::ostream&         stream,
                               const bdem_List&      list,
                               const bdem_RecordDef& record,
                               int                   level          = 0,
                               int                   spacesPerLevel = 4);
    static bsl::ostream& print(bsl::ostream&               stream,
                               const bdem_ChoiceArrayItem& item,
                               const bdem_RecordDef&       record,
                               int                         level          = 0,
                               int                         spacesPerLevel = 4);
    static bsl::ostream& print(bsl::ostream&         stream,
                               const bdem_Choice&    choice,
                               const bdem_RecordDef& record,
                               int                   level          = 0,
                               int                   spacesPerLevel = 4);
    static bsl::ostream& print(bsl::ostream&         stream,
                               const bdem_Table&     table,
                               const bdem_RecordDef& record,
                               int                   level          = 0,
                               int                   spacesPerLevel = 4);
    static bsl::ostream& print(bsl::ostream&           stream,
                               const bdem_ChoiceArray& array,
                               const bdem_RecordDef&   record,
                               int                     level          = 0,
                               int                     spacesPerLevel = 4);
        // Format the specified 'row', 'list', 'choice array item', 'choice',
        // 'table', or 'choice array', constrained by the specified 'record',
        // to the specified output 'stream' at the (absolute value of) the
        // optionally specified indentation 'level' and return a reference to
        // 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -------------------------------
                        // struct bdem_SchemaAggregateUtil
                        // -------------------------------

// CLASS METHODS
inline
bool bdem_SchemaAggregateUtil::canSatisfyRecord(const bdem_List&      list,
                                                const bdem_RecordDef& record)
{
    return canSatisfyRecord(list.row(), record);
}

inline
bool bdem_SchemaAggregateUtil::canSatisfyRecord(const bdem_Choice&    choice,
                                                const bdem_RecordDef& record)
{
    return canSatisfyRecord(choice.item(), record);
}

inline
bool bdem_SchemaAggregateUtil::isChoiceShallowConformant(
                                                  const bdem_Choice&    choice,
                                                  const bdem_RecordDef& record)
{
    return isChoiceArrayItemShallowConformant(choice.item(), record);
}

inline
bool bdem_SchemaAggregateUtil::isChoiceConformant(const bdem_Choice&    choice,
                                                  const bdem_RecordDef& record)
{
    return isChoiceArrayItemConformant(choice.item(), record);
}

inline
bool bdem_SchemaAggregateUtil::isChoiceDeepConformant(
                                                  const bdem_Choice&    choice,
                                                  const bdem_RecordDef& record)
{
    return isChoiceArrayItemDeepConformant(choice.item(), record);
}

inline
bool bdem_SchemaAggregateUtil::isListConformant(const bdem_List&      list,
                                                const bdem_RecordDef& record)
{
    return isRowConformant(list.row(), record);
}

inline
bool bdem_SchemaAggregateUtil::isListShallowConformant(
                                                  const bdem_List&      list,
                                                  const bdem_RecordDef& record)
{
    return isRowShallowConformant(list.row(), record);
}

inline
bool bdem_SchemaAggregateUtil::isListDeepConformant(
                                                  const bdem_List&      list,
                                                  const bdem_RecordDef& record)
{
    return isRowDeepConformant(list.row(), record);
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
