// bdlmxxx_table.h                                                       -*-C++-*-
#ifndef INCLUDED_BDLMXXX_TABLE
#define INCLUDED_BDLMXXX_TABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a table of homogeneous rows of heterogeneous column types.
//
//@CLASSES:
//  bdlmxxx::Table: sequence of rows having the same (heterogeneous) column types
//
//@SEE_ALSO: bdlmxxx_row, bdlmxxx_list, bdlmxxx_choice, bdlmxxx_choicearray, bdlmxxx_schema
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component implements a fully value-semantic container
// class, 'bdlmxxx::Table', capable of holding a homogeneous sequence of zero or
// more (logical) rows, each of which may be efficiently rendered as a
// 'bdlmxxx::Row' object.  Each row consists of a heterogeneous sequence of 'bdem'
// elements (see the 'bdlmxxx_row' component for more information regarding the
// various supported element types).  All of the rows in a 'bdlmxxx::Table' object
// have the same sequence of element types, thus producing a regular row-column
// structure, where all of the elements in a column are of the same 'bdem'
// element type, but different columns can represent objects of different
// ('bdem' element) types.
//
// Each 'bdlmxxx::Table' object is typically configured at construction to have its
// own fixed sequence of column types.  Once a table is constructed, the only
// way to change the number or types of the columns is to discard all of the
// rows and reset all of the column types at once to the new sequence (see
// 'reset').  Copy assignment will also reset the structure of the 'lhs' table,
// discarding any rows and columns types, and replacing them with the values of
// those from the 'rhs' table.  Two tables have the same value, and thus
// compare equal, only if they have the same number of rows and sequence of
// column types, and each pair of corresponding 'bdem' element objects at every
// row and column position represent the same value.  Note that the table
// structure is also preserved through serialization operations; hence,
// invoking 'bdexStreamIn' on a 'bdlmxxx::Table' object will also necessarily reset
// its structure.
//
///'bdem' Null States
///------------------
// The concept of null applies to each 'bdem' type.  In addition to the range
// of values in a given 'bdem' type's domain (e.g., '[ INT_MIN .. INT_MAX ]'
// for 'BDEM_INT'), each type has a null value.  When a 'bdem' element is null,
// it has an underlying (unique) designated unset value (or state) as indicated
// in the following table:
//..
//       'bdem' element type                 "unset" value (or state)
//  ------------------------------    --------------------------------------
//  BDEM_CHOICE and
//  BDEM_CHOICE_ARRAY_ITEM            selector() < 0 && 0 == numSelections()
//
//  BDEM_CHOICE_ARRAY                 0 == length()  && 0 == numSelections()
//
//  BDEM_LIST                         0 == length()
//
//  BDEM_ROW                          All the elements in the row are "null"
//
//  BDEM_TABLE                        0 == numRows() && 0 == numColumns()
//
//  scalar (BDEM_INT, etc.)           bdltuxxx::Unset<TYPE>::unsetValue()
//
//  vector (BDEM_INT_ARRAY, etc.)     0 == size()
//..
// To illustrate, consider a 'BDEM_BOOL' element within a 'bdlmxxx::Table'.  The
// element can be in one of three possible states:
//: o null with underlying value 'bdltuxxx::Unset<bool>::unsetValue()'
//: o non-null with underlying value 'false'
//: o non-null with underlying value 'true'
//
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have a 'bdlmxxx::Table', 'myTable', whose first column
// is of type 'BDEM_STRING':
//..
//  myTable.theModifiableRow(0)[0].theModifiableString() = "HELLO";
//  assert(myTable[0][0].isNonNull());
//..
// Making the element null also makes it have the unset value (which, for
// 'BDEM_STRING', is the empty string):
//..
//  myTable.theModifiableRow(0)[0].makeNull();
//  assert(myTable[0][0].isNull());
//  assert(myTable[0][0].theString().empty());
//..
///Table Nullability
///- - - - - - - - -
// Although a table can contain null elements (each element has "nullability
// information" associated with it), the table itself can be null only in
// relation to another object in which it is contained (i.e., its parent).  The
// nullness of an element in a table can be queried via the 'bdlmxxx::ConstElemRef'
// that is returned through 'operator[]' of 'bdlmxxx::Row' (for example,
// 'table[0][0].isNull()'.
//
// The preferred way to assign a value to an element within a table is through
// calling the 'theModifiableTYPE' methods of 'bdlmxxx::Row' (e.g.,
// 'theModifiableBool(columnIndex)') to first obtain a reference to the
// modifiable element in a row.  The 'theModifiableTYPE' methods have the
// side-effect of making a null value non-null, so the 'const' 'theTYPE'
// methods should be used instead if the value is only being accessed (and
// *not* modified).
//
///Usage
///-----
// A 'bdlmxxx::Table', like a 'bdlmxxx::List', is designed to represent and transmit
// arbitrarily-complex self-describing data in and out of a process.  Both
// objects are containers for sub-objects of type 'bdlmxxx::Row'.  A 'bdlmxxx::Table'
// owns and manages a sequence of 0 or more 'bdlmxxx::Row' objects, whereas each
// 'bdlmxxx::List' always manages exactly one 'bdlmxxx::Row'.  Hence, 'bdlmxxx::Row' is the
// common "vocabulary" type that makes the data records contained in list and
// table objects interchangeable.
//
// We use a 'bdlmxxx::List' when we want to represent a single record (or C-style
// 'struct').  For example, we might choose to represent a single "Leg" of a
// securities-trading transaction as a 'bdlmxxx::List' having the following three
// fields:
//..
//  LEG: { STRING /* secId */, INT /* quantity */, TIME /* time */ }
//..
// i.e., a "security ID" of type 'BDEM_STRING', a "quantity" of type
// 'BDEM_INT', and a "time" of type 'BDEM_TIME'.  The 'bdlmxxx::List' would, in
// turn, contain a single 'bdlmxxx::Row' with the same sequence of field types.
// Note that, for simplicity, we have deliberately side-stepped the need for
// defining a "price" field.  Suppose we want to represent a sequence of two
// legs as in the following:
//..
//  ===========================================
//  *                 twoLegs                 *
//  -------------------------------------------
//  Field Mnemonic:  secId    quantity    time
//                   ~~~~~    ~~~~~~~~    ~~~~
//  Field Type:      STRING     INT       TIME
//                   ~~~~~~     ~~~       ~~~~
//  Leg 0:            IBM       1000      12:03
//
//  Leg 1:            TYC       -500      12:05
//  ===========================================
//..
// We could represent legs as a list of lists:
//..
//  {
//      bdlmxxx::List twoLegs;
//
//      twoLegs.appendNullList();
//      twoLegs[0].theModifiableList().appendString("IBM");
//      twoLegs[0].theModifiableList().appendInt(1000);
//      twoLegs[0].theModifiableList().appendTime(bdlt::Time(12, 3));
//
//      twoLegs.appendNullList();
//      twoLegs[1].theModifiableList().appendString("TYC");
//      twoLegs[1].theModifiableList().appendInt(-500);
//      twoLegs[1].theModifiableList().appendTime(bdlt::Time(12, 5));
//
//      twoLegs.print(cout);
//  }
//..
// The above code snippet creates a 'bdlmxxx::List' with two similar sublists and
// produces the following list-based textual representation:
//..
//  {
//      LIST {
//          STRING "IBM"
//          INT 1000
//          TIME 12:03:00.000
//      }
//      LIST {
//          STRING "TYC"
//          INT -500
//          TIME 12:05:00.000
//      }
//  }
//..
// But given that the lengths and respective field types in each sublist are
// necessarily the same, this representation is sub-optimal.  We can instead
// represent a sequence of transactions more simply *and* *efficiently* in a
// 'bdlmxxx::Table':
//..
//  {
//      const bdlmxxx::ElemType::Type LEG[] = {
//          bdlmxxx::ElemType::BDEM_STRING,      // column 0: secId
//          bdlmxxx::ElemType::BDEM_INT,         // column 1: quantity
//          bdlmxxx::ElemType::BDEM_TIME         // column 2: time
//      };
//      const int NUM_LEG_FIELDS = sizeof LEG / sizeof *LEG;
//
//      bdlmxxx::Table twoLegs(LEG, NUM_LEG_FIELDS);
//      twoLegs.appendNullRows(2);
//
//      twoLegs.theModifiableRow(0)[0].theModifiableString() = "IBM";
//      twoLegs.theModifiableRow(0)[1].theModifiableInt()    = 1000;
//      twoLegs.theModifiableRow(0)[2].theModifiableTime()   = bdlt::Time(12,3);
//
//      twoLegs.theModifiableRow(1)[0].theModifiableString() = "TYC";
//      twoLegs.theModifiableRow(1)[1].theModifiableInt()    = -500;
//      twoLegs.theModifiableRow(1)[2].theModifiableTime()   = bdlt::Time(12,5);
//
//      twoLegs.print(cout);
//  }
//..
// This code creates a 'bdlmxxx::Table' object with three columns and two rows that
// represents the same information as the list of lists above.  The more
// compact representation is reflected in the text output below:
//..
//  {
//      [
//          STRING
//          INT
//          TIME
//      ]
//      {
//          "IBM"
//          1000
//          12:03:00.000
//      }
//      {
//          "TYC"
//          -500
//          12:05:00.000
//      }
//  }
//..
// Note that a much richer output format is achieved in conjunction with a
// corresponding 'bdlmxxx::Schema' when using generator methods that can be found
// in 'bdlmxxx_schemaaggregateutil'.
//
// Together, lists and tables support efficient representation of arbitrarily
// complex hierarchical data.  As a more comprehensive example, consider the
// following "MY SCHEMA" informally describing a set of hierarchical business
// types:
//..
//  ======================================================================
//  *                             MY SCHEMA                              *
//  ----------------------------------------------------------------------
//
//   bdlmxxx::List     bdlmxxx::Table     (Common) Row Fields for Lists and Tables
//   ---------     ------------   ----------------------------------------
//   LEG           LEGS           STRING secId, INT quantity, TIME time
//
//   TRANSACTION   TRANSACTIONS   LEGS legs, DATE date
//
//   POSITION      POSITIONS      STRING secId, INT64 quantity
//
//   ACCOUNT       ACCOUNTS       CHAR type, POSITIONS portfolio
//
//   CUSTOMER      CUSTOMERS      STRING name, STRING address,
//                                ACCOUNTS accounts, TRANSACTIONS history
//
//  ======================================================================
//..
// The first step is to convert this informal schema to code.  For the purpose
// of illustration, we will do so here by hand; However, use of 'bdlmxxx_schema'
// will greatly facilitate this task, and is strongly recommended:
//..
//              // *** "LEG" RECORD DEFINITION ***
//
//  const bdlmxxx::ElemType::Type LEG[] = {
//      bdlmxxx::ElemType::BDEM_STRING,          // 0: STRING secId;
//      bdlmxxx::ElemType::BDEM_INT,             // 1: INT    quantity;
//      bdlmxxx::ElemType::BDEM_TIME             // 2: TIME   time;
//  };
//  const int LEG_NUM_FIELDS = sizeof LEG / sizeof *LEG;
//  enum {
//      LEG_SECID_INDEX,                     // 0: STRING secId;
//      LEG_QUANTITY_INDEX,                  // 1: INT    quantity;
//      LEG_TIME_INDEX,                      // 2: TIME   time;
//      LEG_NUM_INDICES
//  };
//  assert(LEG_NUM_INDICES == LEG_NUM_FIELDS);
//
//              // *** "TRANSACTION" RECORD DEFINITION ***
//
//  const bdlmxxx::ElemType::Type TRANSACTION[] = {
//      bdlmxxx::ElemType::BDEM_TABLE,           // 0: TABLE<LEG> legs;
//      bdlmxxx::ElemType::BDEM_DATE,            // 1: DATE       date;
//  };
//  const int TRANSACTION_NUM_FIELDS = sizeof TRANSACTION / sizeof*TRANSACTION;
//  enum {
//      TRANSACTION_LEGS_INDEX,              // 0: TABLE<LEG> legs;
//      TRANSACTION_DATE_INDEX,              // 1: DATE       date;
//      TRANSACTION_NUM_INDICES
//  };
//  assert(TRANSACTION_NUM_INDICES == TRANSACTION_NUM_FIELDS);
//
//              // *** "POSITION" RECORD DEFINITION ***
//
//  const bdlmxxx::ElemType::Type POSITION[] = {
//      bdlmxxx::ElemType::BDEM_STRING,          // 0: STRING secId;
//      bdlmxxx::ElemType::BDEM_INT64            // 1: INT64  quantity;
//  };
//  const int POSITION_NUM_FIELDS = sizeof POSITION / sizeof *POSITION;
//  enum {
//      POSITION_SECID_INDEX,                // 0: STRING secId;
//      POSITION_QUANTITY_INDEX,             // 1: INT64  quantity;
//      POSITION_NUM_INDICES
//  };
//  assert(POSITION_NUM_INDICES == POSITION_NUM_FIELDS);
//
//              // *** "ACCOUNT" RECORD DEFINITION ***
//
//  const bdlmxxx::ElemType::Type ACCOUNT[] = {
//      bdlmxxx::ElemType::BDEM_CHAR,            // 0: CHAR            type;
//      bdlmxxx::ElemType::BDEM_TABLE            // 1: TABLE<POSITION> portfolio;
//  };
//  const int ACCOUNT_NUM_FIELDS = sizeof ACCOUNT / sizeof *ACCOUNT;
//  enum {
//      ACCOUNT_TYPE_INDEX,                  // 0: CHAR            type;
//      ACCOUNT_PORTFOLIO_INDEX,             // 1: TABLE<POSITION> portfolio;
//      ACCOUNT_NUM_INDICES
//  };
//  assert(ACCOUNT_NUM_INDICES == ACCOUNT_NUM_FIELDS);
//
//              // *** "CUSTOMER" RECORD DEFINITION ***
//
//  const bdlmxxx::ElemType::Type CUSTOMER[] = {
//      bdlmxxx::ElemType::BDEM_STRING,          // 0: STRING             name;
//      bdlmxxx::ElemType::BDEM_STRING,          // 1: STRING             address;
//      bdlmxxx::ElemType::BDEM_TABLE,           // 2: TABLE<ACCOUNT>     accounts;
//      bdlmxxx::ElemType::BDEM_TABLE            // 3: TABLE<TRANSACTION> history;
//  };
//  const int CUSTOMER_NUM_FIELDS = sizeof CUSTOMER / sizeof *CUSTOMER;
//  enum {
//      CUSTOMER_NAME_INDEX,                 // 0: STRING             name;
//      CUSTOMER_ADDRESS_INDEX,              // 1: STRING             address;
//      CUSTOMER_ACCOUNTS_INDEX,             // 2: TABLE<ACCOUNT>     accounts;
//      CUSTOMER_HISTORY_INDEX,              // 3: TABLE<TRANSACTION> history;
//      CUSTOMER_NUM_INDICES
//  };
//  assert(CUSTOMER_NUM_INDICES == CUSTOMER_NUM_FIELDS);
//..
// Now suppose we want to build up an in-core representation of a single
// customer record for "John Smith" on "Easy Street" having two accounts: 'B'
// for "Brokerage" and 'R' for "Retirement".  Furthermore, we want to populate
// each account with a number of positions (3 and 2, respectively) and simulate
// a plausible recent trading history that has lead to the current positions:
//..
//  {
//    STRING "John Smith"                           // customer.name
//    STRING "Easy Street"                          // customer.address
//    TABLE { [ CHAR TABLE                    ]     // customer.accounts
//            { 'B'  { [ STRING   INT64   ]         //   'B' for "Brokerage"
//                     { "USD"    4522500 }         //      Position 0
//                     { "IBM"    1000    }         //      Position 1
//                     { "TYC"    -500    } } }     //      Position 2
//            { 'R'  { [ STRING   INT64   ]         //   'R' for "Retirement"
//                     { "USD"    1275000 }         //      Position 0
//                     { "NT"     800     } } }     //      Position 1
//    }                                             // customer.history
//    TABLE { [ TABLE                                   DATE      ]  // History
//            { { [ STRING  INT        TIME         ]                //
//                { "USD"   100000000  10:52:00.000 } } 02JAN2000 }  // H0.Leg0
//            { { [ STRING  INT        TIME         ]
//                { "USD"   -6350000   09:13:00.000 }                // H1.Leg0
//                { "IBM"   1000       09:13:00.000 } } 04JAN2000 }  // H1.Leg1
//            { { [ STRING  INT        TIME         ]
//                { "USD"   -250000    14:10:00.000 }                // H2.Leg0
//                { "NT"    1000       14:10:00.000 } } 28FEB2000 }  // H2.Leg1
//            { { [ STRING  INT        TIME         ]
//                { "USD"   1750000    09:23:00.000 }                // H3.Leg0
//                { "TYC"   500        09:23:00.000 } } 15MAR2000 }  // H3.Leg1
//    }
//  }
//..
// The following block-structured code produces the output shown above (albeit,
// less compactly formatted and, of course, without '//' anotations):
//..
//  bdlmxxx::List aCustomer(CUSTOMER, CUSTOMER_NUM_FIELDS);
//  {
//    assert(4 == aCustomer.length());
//    aCustomer[CUSTOMER_NAME_INDEX].theModifiableString()    = "John Smith";
//    aCustomer[CUSTOMER_ADDRESS_INDEX].theModifiableString() = "Easy Street";
//
//    bdlmxxx::Table& accounts =
//                     aCustomer[CUSTOMER_ACCOUNTS_INDEX].theModifiableTable();
//    {
//      accounts.reset(ACCOUNT, ACCOUNT_NUM_FIELDS);
//      accounts.appendNullRows(2);
//      {
//        bdlmxxx::Row& row = accounts.theModifiableRow(0);
//        row[ACCOUNT_TYPE_INDEX].theModifiableChar() = 'B';
//        bdlmxxx::Table& portfolio =
//                           row[ACCOUNT_PORTFOLIO_INDEX].theModifiableTable();
//        {
//          portfolio.reset(POSITION, POSITION_NUM_FIELDS);
//          portfolio.appendNullRows(3);
//          {
//            bdlmxxx::Row& row = portfolio.theModifiableRow(0);
//            row[POSITION_SECID_INDEX].theModifiableString()   = "USD";
//            row[POSITION_QUANTITY_INDEX].theModifiableInt64() = 4522500;
//          }
//          {
//            bdlmxxx::Row& row = portfolio.theModifiableRow(1);
//            row[POSITION_SECID_INDEX].theModifiableString()   = "IBM";
//            row[POSITION_QUANTITY_INDEX].theModifiableInt64() = 1000;
//          }
//          {
//            bdlmxxx::Row& row = portfolio.theModifiableRow(2);
//
//            row[POSITION_SECID_INDEX].theModifiableString()   = "TYC";
//            row[POSITION_QUANTITY_INDEX].theModifiableInt64() = -500;
//          }
//        }
//      }
//      {
//        bdlmxxx::Row& row = accounts.theModifiableRow(1);
//        row[ACCOUNT_TYPE_INDEX].theModifiableChar() = 'R';
//        bdlmxxx::Table& portfolio =
//                           row[ACCOUNT_PORTFOLIO_INDEX].theModifiableTable();
//        {
//          portfolio.reset(POSITION, POSITION_NUM_FIELDS);
//          portfolio.appendNullRows(2);
//          {
//            bdlmxxx::Row& row = portfolio.theModifiableRow(0);
//            row[POSITION_SECID_INDEX].theModifiableString()   = "USD";
//            row[POSITION_QUANTITY_INDEX].theModifiableInt64() = 1275000;
//          }
//          {
//            bdlmxxx::Row& row = portfolio.theModifiableRow(1);
//            row[POSITION_SECID_INDEX].theModifiableString()   = "NT";
//            row[POSITION_QUANTITY_INDEX].theModifiableInt64() = 800;
//          }
//        }
//      }
//    }
//    bdlmxxx::Table& history =
//                      aCustomer[CUSTOMER_HISTORY_INDEX].theModifiableTable();
//    {
//      history.reset(TRANSACTION, TRANSACTION_NUM_FIELDS);
//      history.appendNullRows(4);
//      {
//        bdlmxxx::Row& row = history.theModifiableRow(0);
//        bdlmxxx::Table& legs = row[TRANSACTION_LEGS_INDEX].theModifiableTable();
//        {
//          legs.reset(LEG, LEG_NUM_FIELDS);
//          legs.appendNullRows(1);
//          {
//            bdlmxxx::Row& row = legs.theModifiableRow(0);
//            row[LEG_SECID_INDEX].theModifiableString() = "USD";
//            row[LEG_QUANTITY_INDEX].theModifiableInt() = 100000000;
//            row[LEG_TIME_INDEX].theModifiableTime()    = bdlt::Time(10, 52);
//          }
//        }
//        row[TRANSACTION_DATE_INDEX].theModifiableDate() =
//                                                       bdlt::Date(2000, 1, 2);
//      }
//      {
//        bdlmxxx::Row& row = history.theModifiableRow(1);
//        bdlmxxx::Table& legs = row[TRANSACTION_LEGS_INDEX].theModifiableTable();
//        {
//          legs.reset(LEG, LEG_NUM_FIELDS);
//          legs.appendNullRows(2);
//          {
//            bdlmxxx::Row& row = legs.theModifiableRow(0);
//            row[LEG_SECID_INDEX].theModifiableString() = "USD";
//            row[LEG_QUANTITY_INDEX].theModifiableInt() = -6350000;
//            row[LEG_TIME_INDEX].theModifiableTime()    = bdlt::Time(9, 13);
//          }
//          {
//            bdlmxxx::Row& row = legs.theModifiableRow(1);
//            row[LEG_SECID_INDEX].theModifiableString() = "IBM";
//            row[LEG_QUANTITY_INDEX].theModifiableInt() = 1000;
//            row[LEG_TIME_INDEX].theModifiableTime()    = bdlt::Time(9, 13);
//          }
//        }
//        row[TRANSACTION_DATE_INDEX].theModifiableDate() =
//                                                       bdlt::Date(2000, 1, 4);
//      }
//      {
//        bdlmxxx::Row& row = history.theModifiableRow(2);
//        bdlmxxx::Table& legs = row[TRANSACTION_LEGS_INDEX].theModifiableTable();
//        {
//          legs.reset(LEG, LEG_NUM_FIELDS);
//          legs.appendNullRows(2);
//          {
//            bdlmxxx::Row& row = legs.theModifiableRow(0);
//            row[LEG_SECID_INDEX].theModifiableString() = "USD";
//            row[LEG_QUANTITY_INDEX].theModifiableInt() = -250000;
//            row[LEG_TIME_INDEX].theModifiableTime()    = bdlt::Time(14, 10);
//          }
//          {
//            bdlmxxx::Row& row = legs.theModifiableRow(1);
//            row[LEG_SECID_INDEX].theModifiableString() = "NT";
//            row[LEG_QUANTITY_INDEX].theModifiableInt() = 1000;
//            row[LEG_TIME_INDEX].theModifiableTime()    = bdlt::Time(14, 10);
//          }
//        }
//        row[TRANSACTION_DATE_INDEX].theModifiableDate() =
//                                                      bdlt::Date(2000, 2, 28);
//      }
//      {
//        bdlmxxx::Row& row = history.theModifiableRow(3);
//        bdlmxxx::Table& legs = row[TRANSACTION_LEGS_INDEX].theModifiableTable();
//        {
//          legs.reset(LEG, LEG_NUM_FIELDS);
//          legs.appendNullRows(2);
//          {
//            bdlmxxx::Row& row = legs.theModifiableRow(0);
//            row[LEG_SECID_INDEX].theModifiableString() = "USD";
//            row[LEG_QUANTITY_INDEX].theModifiableInt() = 1750000;
//            row[LEG_TIME_INDEX].theModifiableTime()    = bdlt::Time(9, 23);
//          }
//          {
//            bdlmxxx::Row& row = legs.theModifiableRow(1);
//            row[LEG_SECID_INDEX].theModifiableString() = "TYC";
//            row[LEG_QUANTITY_INDEX].theModifiableInt() = 500;
//            row[LEG_TIME_INDEX].theModifiableTime()    = bdlt::Time(9, 23);
//          }
//        }
//        row[TRANSACTION_DATE_INDEX].theModifiableDate() =
//                                                      bdlt::Date(2000, 3, 15);
//      }
//    }
//  }
//
//  aCustomer.print(cout);
//..
// The order in which the data structure is created can significantly affect
// runtime efficiency.  Notice in the above code that we started with the
// customer list and worked our way down.  By starting at the top we minimize
// the amount of copying that was needed to create the data structure.   If
// instead we create the lower level constructs (e.g., "POSITION") first, we
// would then have to copy those values into the higher-level constructs
// (e.g., "ACCOUNT"), which, in turn, would then have to be copied into the
// "CUSTOMER" list.
//
// Furthermore, if we know that we are going to build up the list once and not
// subsequently modify it (much), significant additional runtime and spatial
// performance gains can be achieved throughout the object hierarchy simply by
// supplying the 'BDEM_WRITE_ONCE' hint to the constructor of the top-level
// list (or table):
//..
//  bdlmxxx::List fastCustomer(CUSTOMER, CUSTOMER_NUM_FIELDS,
//                         bdlmxxx::AggregateOption::BDEM_WRITE_ONCE);
//..
// Robustness, as well as efficiency, is achieved through the use of 'bdem'
// lists and tables.  As our final illustration, consider that we wish to
// augment MY SCHEMA above to include two new record types:
//..
//  =================================================================
//                       NEW RECORD DEFINITIONS
//
//  REPORT   REPORTS   CUSTOMER subject, DATETIME asOf
//
//  UPDATE   UPDATES   INT custId, INT accountId, TRANSACTIONS events
//
//  =================================================================
//..
// Rendering these new types in code is straightforward:
//..
//              // *** "REPORT" RECORD DEFINITION ***
//
//  const bdlmxxx::ElemType::Type REPORT[] = {
//      bdlmxxx::ElemType::BDEM_LIST,            // 0: LIST<CUSTOMER> subject;
//      bdlmxxx::ElemType::BDEM_DATETIME,        // 1: DATETIME       asOf;
//  };
//  const int REPORT_NUM_FIELDS = sizeof REPORT / sizeof *REPORT;
//  enum {
//      REPORT_SUBJECT_INDEX,                // 0: LIST<CUSTOMER> subject;
//      REPORT_ASOF_INDEX,                   // 1: DATETIME       asOf;
//      REPORT_NUM_INDICES
//  };
//  assert(REPORT_NUM_INDICES == REPORT_NUM_FIELDS);
//
//              // *** "UPDATE" RECORD DEFINITION ***
//
//  const bdlmxxx::ElemType::Type UPDATE[] = {
//      bdlmxxx::ElemType::BDEM_INT,             // 0: INT custId;
//      bdlmxxx::ElemType::BDEM_INT,             // 1: INT accountId;
//      bdlmxxx::ElemType::BDEM_TABLE            // 2: TABLE<TRANSACTION> events;
//  };
//  const int UPDATE_NUM_FIELDS = sizeof UPDATE / sizeof *UPDATE;
//  enum {
//      UPDATE_CUSTID_INDEX,                 // 0: INT custId;
//      UPDATE_ACCOUNTID_INDEX,              // 1: INT accountId;
//      UPDATE_EVENT_INDEX,                  // 2: TABLE<TRANSACTION> events;
//      UPDATE_NUM_INDICES
//  };
//  assert(UPDATE_NUM_INDICES == UPDATE_NUM_FIELDS);
//..
// Notice, however, that the latter of these new types makes reference to a
// customer id and an account id -- fields that now need to be added to the
// respective, previously-defined records.  Our concern here is that we do not
// want to break existing code that currently has no need for these new fields,
// yet we want to make them available where they are needed.  The goal is
// achieved by supplying new definitions for the affected records, with the
// missing fields appended to the ends, respectively:
//..
//  ========================================================================
//                       AUGMENTED RECORD DEFINITIONS
//
//  ACCOUNT    ACCOUNTS      CHAR type, POSITIONS portfolio, INT accountId
//
//  CUSTOMER   CUSTOMERS     STRING name, STRING address, ACCOUNTS accounts,
//                           TRANSACTIONS history, INT custId
//  ========================================================================
//..
// For new applications, we can now use the extended definitions to create,
// read, and write ACCOUNT and CUSTOMER structures; existing code will be
// able to read the relevant (leading) fields without having to recompile:
//..
//  {
//              // *** AUGMENTED "ACCOUNT" RECORD DEFINITION ***
//
//      const bdlmxxx::ElemType::Type ACCOUNT[] = {
//          bdlmxxx::ElemType::BDEM_CHAR,          // 0: CHAR            type;
//          bdlmxxx::ElemType::BDEM_TABLE,         // 1: TABLE<POSITION> portfolio;
//          bdlmxxx::ElemType::BDEM_INT            // 1: INT             accountId;
//      };
//      const int ACCOUNT_NUM_FIELDS = sizeof ACCOUNT / sizeof *ACCOUNT;
//      enum {
//          ACCOUNT_TYPE_INDEX,                // 0: CHAR            type;
//          ACCOUNT_PORTFOLIO_INDEX,           // 1: TABLE<POSITION> portfolio;
//          ACCOUNT_ACCOUNTID_INDEX,           // 2: INT             accountId;
//          ACCOUNT_NUM_INDICES
//      };
//      assert(ACCOUNT_NUM_INDICES == ACCOUNT_NUM_FIELDS);
//
//                  // *** AUGMENTED "CUSTOMER" RECORD DEFINITION ***
//
//      const bdlmxxx::ElemType::Type CUSTOMER[] = {
//          bdlmxxx::ElemType::BDEM_STRING,      // 0: STRING             name;
//          bdlmxxx::ElemType::BDEM_STRING,      // 1: STRING             address;
//          bdlmxxx::ElemType::BDEM_TABLE,       // 2: TABLE<ACCOUNT>     accounts;
//          bdlmxxx::ElemType::BDEM_TABLE,       // 3: TABLE<TRANSACTION> history;
//          bdlmxxx::ElemType::BDEM_INT          // 4: INT                custId;
//      };
//      const int CUSTOMER_NUM_FIELDS = sizeof CUSTOMER / sizeof *CUSTOMER;
//      enum {
//          CUSTOMER_NAME_INDEX,             // 0: STRING             name;
//          CUSTOMER_ADDRESS_INDEX,          // 1: STRING             address;
//          CUSTOMER_ACCOUNTS_INDEX,         // 2: TABLE<ACCOUNT>     accounts;
//          CUSTOMER_HISTORY_INDEX,          // 3: TABLE<TRANSACTION> history;
//          CUSTOMER_CUSTID_INDEX,           // 4: INT                custId;
//          CUSTOMER_NUM_INDICES
//      };
//      assert(CUSTOMER_NUM_INDICES == CUSTOMER_NUM_FIELDS);
//  }
//..
// In the examples above, we have hard-coded all of the indices at
// compile-time.  Using 'bdlmxxx::Schema', however, it is possible to look up and
// install the needed indices at runtime, thereby relaxing the requirement that
// augmented record fields must necessarily be appended to the ends of existing
// records.  For more information, see 'bdlmxxx_schema'.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_AGGREGATEOPTION
#include <bdlmxxx_aggregateoption.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMATTRLOOKUP
#include <bdlmxxx_elemattrlookup.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMREF
#include <bdlmxxx_elemref.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMTYPE
#include <bdlmxxx_elemtype.h>
#endif

#ifndef INCLUDED_BDLMXXX_LISTIMP
#include <bdlmxxx_listimp.h>
#endif

#ifndef INCLUDED_BDLMXXX_TABLEIMP
#include <bdlmxxx_tableimp.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {



// Updated by 'bde-replace-bdet-forward-declares.py -m bdlt': 2015-02-03
// Updated declarations tagged with '// bdet -> bdlt'.

namespace bdlt { class Datetime; }                              // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Datetime Datetime;            // bdet -> bdlt
}  // close package namespace

namespace bdlt { class DatetimeTz; }                            // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DatetimeTz DatetimeTz;        // bdet -> bdlt
}  // close package namespace

namespace bdlt { class Date; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Date Date;                    // bdet -> bdlt
}  // close package namespace

namespace bdlt { class DateTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DateTz DateTz;                // bdet -> bdlt
}  // close package namespace

namespace bdlt { class Time; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Time Time;                    // bdet -> bdlt
}  // close package namespace

namespace bdlt { class TimeTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::TimeTz TimeTz;                // bdet -> bdlt
}  // close package namespace


namespace bdlmxxx {class Choice;
class ChoiceArray;
class Row;
class List;

                            // ================
                            // class Table
                            // ================

class Table {
    // This class implements a value-semantic, homogeneous, indexable sequence
    // container of 'Row' objects (see 'bdlmxxx_row'), each consisting of the
    // same heterogeneous sequence of scalar, array, and aggregate types, thus
    // producing a regular row-column structure, where all of the elements in a
    // column are of the same 'bdem' element type, but different columns can
    // represent objects of different ('bdem' element) types.  Each
    // 'Table' object is typically configured at construction to have its
    // own fixed sequence of column types; however, once constructed, any
    // change to the sequence of column types -- e.g., via 'reset' -- requires
    // discarding all of the rows.  Copy assignment and 'bdexStreamIn' will
    // also necessarily reset the table's structure.  Note that the runtime
    // cost of assigning array or aggregate values may be prohibitive, which
    // argues for populating such values in place.
    //
    // More generally, this class supports a complete set of *value-semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex'-compatible serialization.
    // (A precise operational definition of when two objects have the same
    // value can be found in the description of 'operator==' for the class.)
    // This class is *exception* *neutral* and, unless otherwise stated,
    // provides no guarantee of rollback: If an exception is thrown during the
    // invocation of a method on a pre-existing object, the object is left in a
    // valid state, but its value is unspecified.  In no event is memory
    // leaked.  Finally, *aliasing* (e.g., using all or part of an object as
    // both source and destination) is supported in all cases.

    // DATA
    TableImp d_tableImp; // table implementation

    // FRIENDS
    friend bool operator==(const Table&, const Table&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Table,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // TYPES
    class InitialMemory {
        // DATA
        int d_i;

      public:
        // CREATORS
        explicit InitialMemory(int i) : d_i(i) {}
        ~InitialMemory() {}

        // ACCESSORS
        operator int() const
        {
            return d_i;
        }
    };

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    explicit
    Table(bslma::Allocator                         *basicAllocator = 0);
    explicit
    Table(AggregateOption::AllocationStrategy  allocationStrategy,
               bslma::Allocator                         *basicAllocator = 0);
    Table(AggregateOption::AllocationStrategy  allocationStrategy,
               const InitialMemory&                      initialMemory,
               bslma::Allocator                         *basicAllocator = 0);
        // Create an empty table (having no rows and no columns).  Optionally
        // specify a memory 'allocationStrategy'.  If 'allocationStrategy' is
        // not specified, then 'BDEM_PASS_THROUGH' is used.  (The meanings of
        // the various 'allocationStrategy' values are described in
        // 'bdlmxxx_aggregateoption'.)  If 'allocationStrategy' is specified,
        // optionally specify an 'initialMemorySize' (in bytes) that will be
        // preallocated in order to satisfy allocation requests without
        // replenishment (i.e., without internal allocation): it has no effect
        // unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= initialMemorySize', if 'initialMemorySize' is
        // specified.

    Table(const ElemType::Type                 columnTypes[],
               int                                       numColumns,
               bslma::Allocator                         *basicAllocator = 0);
    Table(const ElemType::Type                 columnTypes[],
               int                                       numColumns,
               AggregateOption::AllocationStrategy  allocationStrategy,
               bslma::Allocator                         *basicAllocator = 0);
    Table(const ElemType::Type                 columnTypes[],
               int                                       numColumns,
               AggregateOption::AllocationStrategy  allocationStrategy,
               const InitialMemory&                      initialMemory,
               bslma::Allocator                         *basicAllocator = 0);
        // Create an empty table (having no rows) configured with the specified
        // 'numColumns' 'columnTypes'.  Optionally specify a memory
        // 'allocationStrategy'.  If 'allocationStrategy' is not specified,
        // then 'BDEM_PASS_THROUGH' is used.  (The meanings of the various
        // 'allocationStrategy' values are described in
        // 'bdlmxxx_aggregateoption'.)  If 'allocationStrategy' is specified,
        // optionally specify an 'initialMemorySize' that will be preallocated
        // in order to satisfy allocation requests without replenishment (i.e.,
        // without internal allocation): it has no effect unless
        // 'allocationStrategy' is 'BDEM_WRITE_ONCE' or 'BDEM_WRITE_MANY'.  If
        // 'initialMemorySize' is not specified, an implementation-dependent
        // value will be used.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // 'columnTypes' contains at least 'numColumns' and,
        // '0 <= initialMemorySize', if 'initialMemorySize' is specified.

    explicit
    Table(const bsl::vector<ElemType::Type>&   columnTypes,
               bslma::Allocator                         *basicAllocator = 0);
    Table(const bsl::vector<ElemType::Type>&   columnTypes,
               AggregateOption::AllocationStrategy  allocationStrategy,
               bslma::Allocator                         *basicAllocator = 0);
    Table(const bsl::vector<ElemType::Type>&   columnTypes,
               AggregateOption::AllocationStrategy  allocationStrategy,
               const InitialMemory&                      initialMemory,
               bslma::Allocator                         *basicAllocator = 0);
        // Create an empty table (having no rows) configured with the specified
        // 'columnTypes'.  Optionally specify a memory 'allocationStrategy'.
        // If 'allocationStrategy' is not specified, then 'BDEM_PASS_THROUGH'
        // is used.  (The meanings of the various 'allocationStrategy' values
        // are described in 'bdlmxxx_aggregateoption'.)  If 'allocationStrategy'
        // is specified, optionally specify an 'initialMemorySize' (in bytes)
        // that will be preallocated in order to satisfy allocation requests
        // without replenishment (i.e., without internal allocation): it has no
        // effect unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= initialMemorySize', if 'initialMemorySize'
        // is specified.

    Table(const Table&                         original,
               bslma::Allocator                         *basicAllocator = 0);
    Table(const Table&                         original,
               AggregateOption::AllocationStrategy  allocationStrategy,
               bslma::Allocator                         *basicAllocator = 0);
    Table(const Table&                         original,
               AggregateOption::AllocationStrategy  allocationStrategy,
               const InitialMemory&                      initialMemory,
               bslma::Allocator                         *basicAllocator = 0);
        // Create a table having the value of the specified 'original' table.
        // Optionally specify a memory 'allocationStrategy'.  If
        // 'allocationStrategy' is not specified, then 'BDEM_PASS_THROUGH' is
        // used.  (The meanings of the various 'allocationStrategy' values are
        // described in 'bdlmxxx_aggregateoption'.)  If 'allocationStrategy' is
        // specified, optionally specify an 'initialMemorySize' (in bytes) that
        // will be preallocated in order to satisfy allocation requests without
        // replenishment (i.e., without internal allocation): it has no effect
        // unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= initialMemorySize', if 'initialMemorySize'
        // is specified.

    ~Table();
        // Destroy this table object.  If the table was constructed with any
        // memory-management mode other than 'BDEM_PASS_THROUGH', then
        // destructors on individually contained elements are not invoked.  The
        // memory used by those elements will be released efficiently (all at
        // once) when the internal (managed) allocator is destroyed.

    // MANIPULATORS
    Table& operator=(const Table& rhs);
        // Assign to this table the value of the specified 'rhs' object, and
        // return a reference to this modifiable table.  Note that the initial
        // rows and column structure of this table are discarded.

    void appendRow(const Row& srcRow);
        // Append (a copy of) the specified 'srcRow' after the last row of
        // this table.  The behavior is undefined unless the number and
        // respective element types in 'srcRow' exactly match those of the
        // columns in this table.  Note that this function is logically
        // equivalent to:
        //..
        //  insertRow(numRows(), srcRow);
        //..
        //
        // DEPRECATED: use 'appendRow(const Table& table, int index)'
        // instead.

    void appendRow(const List& srcList);
        // Append to this table the value of the specified 'srcList'.  The
        // behavior is undefined unless the sequence of element types in
        // 'srcList' is the same as that of the columns types in this table.
        // Note that the behavior of this method is the same as:
        //..
        //  insertRow(numRows(), srcList);
        //..

    void appendRow(const Table& srcTable, int srcRowIndex);
        // Append to this table the value of the row at the specified
        // 'srcRowIndex' in the specified 'srcTable'.  The behavior is
        // undefined unless '0 <= srcRowIndex < srcTable.numRows()', and the
        // sequence of column types in 'srcTable' is the same as that of this
        // table.   Note that this function is the same as:
        //..
        //  insertRow(numRows(), srcTable, srcRowIndex);
        //..

    void appendNullRow();
        // Append to this table the value of a null row.  Note that, if
        // accessed, the value at each column position of the appended row will
        // be the unset value corresponding to its element type (see
        // 'bdltuxxx_unset').  Also note that the behavior of this method is the
        // same as:
        //..
        //  insertNullRow(numRows());
        //..

    void appendRows(const Table& srcTable);
        // Append to this table the value of each row in the specified
        // 'srcTable'.  The behavior is undefined unless the sequence of column
        // types in 'srcTable' is the same as that of this table.  Note that
        // the behavior of this method is the same as:
        //..
        //  insertRows(numRows(), srcTable);
        //..

    void appendRows(const Table& srcTable, int srcRowIndex, int numRows);
        // Append to this table the value of each of the specified 'numRows'
        // starting at the specified 'srcRowIndex' in the specified 'srcTable'.
        // The behavior is undefined unless '0 <= srcRowIndex', '0 <= numRows',
        // 'srcRowIndex + numRows <= srcTable.numRows()', and the sequence of
        // column types in 'srcTable' is the same as that of this table.  Note
        // that the behavior of this method is the same as:
        //..
        //  insertRows(numRows(), srcTable, srcRowIndex, numRows);
        //..

    void appendNullRows(int numRows);
        // Append to this table the specified 'numRows' null row values.  Note
        // that, if accessed, each value at each column position will be the
        // unset value corresponding to its element type (see 'bdltuxxx_unset').
        // Also note that the behavior of this method is the same as:
        //..
        //  insertNullRows(numRows(), numRows);
        //..

    void insertRow(int dstRowIndex, const List& srcList);
        // Insert into this table at the specified 'dstRowIndex' the value of
        // the specified 'srcList'.  The behavior is undefined unless
        // '0 <= dstRowIndex <= numRows()', and the sequence of element types
        // in 'srcList' is the same as that of the columns in this table.

    void insertRow(int               dstRowIndex,
                   const Table& srcTable,
                   int               srcRowIndex);
        // Insert into this table at the specified 'dstRowIndex' the value of
        // the row at the specified 'srcRowIndex' in the specified 'srcTable'.
        // The behavior is undefined unless '0 <= dstRowIndex <= numRows()',
        // '0 <= srcRowIndex < srcTable.numRows()', and the sequence of column
        // types in 'srcTable' is the same as that of this table.

    void insertNullRow(int dstRowIndex);
        // Insert into this table at the specified 'dstRowIndex' the value of a
        // null row.  The behavior is undefined unless
        // '0 <= rowIndex <= numRows()'.  Note that, if accessed, the value at
        // each column position of the inserted row will be the unset value
        // corresponding to its element type (see 'bdltuxxx_unset').

    void insertRows(int dstRowIndex, const Table& srcTable);
        // Insert into this table at the specified 'dstRowIndex' the value of
        // each row in the specified 'srcTable'.  The behavior is undefined
        // unless '0 <= dstRowIndex <= numRows()', and the sequence of column
        // types in 'srcTable' is the same as that of this table.

    void insertRows(int               dstRowIndex,
                    const Table& srcTable,
                    int               srcRowIndex,
                    int               numRows);
        // Insert into this table at the specified 'dstRowIndex' the value of
        // each of the specified 'numRows' starting at the specified
        // 'srcRowIndex' in the specified 'srcTable'.  The behavior is
        // undefined unless '0 <= dstRowIndex <= numRows()',
        // '0 <= srcRowIndex', '0 <= numRows',
        // 'srcRowIndex + numRows <= srcTable.numRows()', and the sequence of
        // column types in 'srcTable' is the same as that of this table.

    void insertNullRows(int dstRowIndex, int numRows);
        // Insert into this table at the specified 'dstRowIndex' the specified
        // 'numRows' null row values.  The behavior is undefined unless
        // '0 <= dstRowIndex <= numRows()' and '0 <= numRows'.  Note that, if
        // accessed, the value at each column position in each of the appended
        // rows will be the unset value corresponding to its element type (see
        // 'bdltuxxx_unset').

    void makeAllNull();
        // Set the value of each element in this table to null.  Note that, if
        // accessed, the value at each column position in each row will be the
        // unset value corresponding to its element type (see 'bdltuxxx_unset').

    void makeColumnNull(int columnIndex);
        // Set the value of the element at the specified 'columnIndex' in
        // each row of this table to null.  The behavior is undefined unless
        // '0 <= columnIndex < numColumns()'.  Note that, if accessed, the
        // value at 'columnIndex in each row will be the unset value
        // corresponding to its element type (see 'bdltuxxx_unset').

    void makeRowsNull(int startRowIndex, int numRows);
        // Set the value of each element in the specified 'numRows' beginning
        // at the specified 'startRowIndex' of this table to null.  The
        // behavior is undefined unless '0 <= startRowIndex', '0 <= numRows',
        // and 'startRowIndex + numRows <= numRows()'.  Note that, if accessed,
        // the value at each affected row/column position will be the unset
        // value corresponding to its element type (see 'bdltuxxx_unset').

    void reserveMemory(int numBytes);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numBytes' without replenishment (i.e., without
        // internal allocation).  The behavior is undefined unless
        // '0 <= numBytes'.  Note that this method has no effect unless the
        //  internal allocation mode is 'BDEM_WRITE_ONCE' or 'BDEM_WRITE_MANY'.

    void reserveRaw(bsl::size_t numRows);
        // Reserve sufficient memory to hold at least the footprints for the
        // specified 'numRows'.  Other memory needed to initialize a row upon
        // insertion *may* or may *not* be reserved, depending on the
        // allocation mode.  In the future, this method may strengthen its
        // guarantee such that no additional allocation will occur upon row
        // insertion (regardless of allocation mode) unless a row data element
        // itself allocates memory.

    void reset(const ElemType::Type columnTypes[], int numColumns);
        // Remove all of the rows from this table and set the sequence of
        // column types in this table to be the same as the specified leading
        // 'numColumns' in the specified 'columnTypes' array.  The behavior
        // is undefined unless 'columnTypes' contains at least 'numColumns'
        // types.

    void reset(const bsl::vector<ElemType::Type>& columnTypes);
        // Remove all of the rows from this table and set the sequence of
        // column types in the table to be the same as the specified
        // 'columnTypes'.

    void removeRow(int rowIndex);
        // Remove the row at the specified 'rowIndex' from this table.  Each
        // row having an index greater than 'rowIndex' before the removal is
        // shifted down by one index position.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()'.

    void removeRows(int startRowIndex, int numRows);
        // Remove the specified 'numRows' from this table beginning at the
        // specified 'startRowIndex'.  Each row having an index greater than
        // 'startRowIndex + numRows' before the removal is shifted down by
        // 'numRows' index positions.  The behavior is undefined unless
        // '0 <= startRowIndex', '0 <= numRows', and
        // 'startRowIndex + numRows <= numRows()'.

    void removeAllRows();
        // Remove all of the rows from this table, leaving the table empty, but
        // its column types intact.

    void removeAll();
        // Remove all of the rows and column types from this table (leaving it
        // in its default constructed value).

    ElemRef rowElemRef(int rowIndex);
        // Return a 'bdem' element reference to the modifiable row at the
        // specified 'rowIndex' in this table.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()'.

    void setColumnBool(int columnIndex, bool value);
    void setColumnChar(int columnIndex, char value);
    void setColumnShort(int columnIndex, short value);
    void setColumnInt(int columnIndex, int value);
    void setColumnInt64(int columnIndex, bsls::Types::Int64 value);
    void setColumnFloat(int columnIndex, float value);
    void setColumnDouble(int columnIndex, double value);
    void setColumnString(int columnIndex, const char *value);
    void setColumnString(int columnIndex, const bsl::string& value);
    void setColumnDatetime(int columnIndex, const bdlt::Datetime& value);
    void setColumnDatetimeTz(int columnIndex, const bdlt::DatetimeTz& value);
    void setColumnDate(int columnIndex, const bdlt::Date& value);
    void setColumnDateTz(int columnIndex, const bdlt::DateTz& value);
    void setColumnTime(int columnIndex, const bdlt::Time& value);
    void setColumnTimeTz(int columnIndex, const bdlt::TimeTz& value);
    void setColumnBoolArray(int columnIndex, const bsl::vector<bool>& value);
    void setColumnCharArray(int columnIndex, const bsl::vector<char>& value);
    void setColumnShortArray(int columnIndex, const bsl::vector<short>& value);
    void setColumnIntArray(int columnIndex, const bsl::vector<int>& value);
    void setColumnInt64Array(
                            int                                    columnIndex,
                            const bsl::vector<bsls::Types::Int64>& value);
    void setColumnFloatArray(int columnIndex, const bsl::vector<float>& value);
    void setColumnDoubleArray(int                        columnIndex,
                              const bsl::vector<double>& value);
    void setColumnStringArray(int                             columnIndex,
                              const bsl::vector<bsl::string>& value);
    void setColumnDatetimeArray(int                               columnIndex,
                                const bsl::vector<bdlt::Datetime>& value);
    void setColumnDatetimeTzArray(
                               int                                 columnIndex,
                               const bsl::vector<bdlt::DatetimeTz>& value);
    void setColumnDateArray(int                           columnIndex,
                            const bsl::vector<bdlt::Date>& value);
    void setColumnDateTzArray(int                             columnIndex,
                              const bsl::vector<bdlt::DateTz>& value);
    void setColumnTimeArray(int                           columnIndex,
                            const bsl::vector<bdlt::Time>& value);
    void setColumnTimeTzArray(int                             columnIndex,
                              const bsl::vector<bdlt::TimeTz>& value);
    void setColumnChoice(int columnIndex, const Choice& value);
    void setColumnChoiceArray(int columnIndex, const ChoiceArray& value);
    void setColumnList(int columnIndex, const Row& value);
    void setColumnList(int columnIndex, const List& value);
    void setColumnTable(int columnIndex, const Table& value);
        // Set the value of the element at the specified 'columnIndex' in
        // each row of this table to the specified 'value'.  The behavior is
        // undefined unless '0 <= columnIndex < numColumns()' and the type of
        // the indexed column is of the type indicated by the name of the
        // method used.

    void setColumnValue(int columnIndex, const ConstElemRef& value);
        // Set the value of the element at the specified 'columnIndex' in
        // each row of this table to the specified 'value'.  The behavior is
        // undefined unless '0 <= columnIndex < numColumns()' and
        // 'columnType(columnIndex) == value.type()'.

    void swapRows(int rowIndex1, int rowIndex2);
        // Exchange the value of each element in the row indicated by the
        // specified 'rowIndex1' with that of the corresponding element in the
        // row indicated by the specified 'rowIndex2' in constant time without
        // the possibility of an exception being thrown (i.e., this method
        // provides the no-throw guarantee).  The behavior is undefined unless
        // '0 <= rowIndex1 <= numRows()' and '0 <= rowIndex2 <= numRows()'.

    Row& theModifiableRow(int rowIndex);
        // Return a reference to the modifiable row header corresponding to
        // the row at the specified 'rowIndex' in this table.  If the
        // referenced row is null, it is made non-null before returning, but
        // its value is not otherwise modified.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()'.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is not specified.
        // If 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    // ACCESSORS
    const Row& operator[](int rowIndex) const;
        // Return a reference to the heterogeneous row of non-modifiable
        // elements at the specified 'rowIndex' in this table.  The nullness
        // of the indexed row is not affected.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()'.

    ElemType::Type columnType(int columnIndex) const;
        // Return the element type of the column at the specified 'columnIndex'
        // in this table.  The behavior is undefined unless
        // '0 <= columnIndex < numColumns()'.

    void columnTypes(bsl::vector<ElemType::Type> *result) const;
        // Load into the specified 'result', the sequence of column types in
        // this table.

    bsl::size_t capacityRaw() const;
        // Return the total number of row footprints for which sufficient
        // memory is currently allocated.  Inserting rows that do not exceed
        // this capacity *may* or may *not* result in additional allocations
        // depending on the allocation mode, and whether any row data element
        // itself allocates memory (see the 'reserveRaw' method).  Note that
        // 'numRows() <= capacityRaw()' is an invariant of this class.

    bool isAnyInColumnNull(int columnIndex) const;
        // Return 'true' if the value of an element at the specified
        // 'columnIndex' in any row of this table is null, and 'false'
        // otherwise.  The behavior is undefined unless
        // '0 <= columnIndex < numColumns()'.

    bool isAnyInColumnNonNull(int columnIndex) const;
        // Return 'true' if the value of an element at the specified
        // 'columnIndex' in any row of this table is non-null, and 'false'
        // otherwise.  The behavior is undefined unless
        // '0 <= columnIndex < numColumns()'.

    bool isAnyNull() const;
        // Return 'true' if the value of an element at any row/column position
        // in this table is null, and 'false' otherwise.

    bool isAnyNonNull() const;
        // Return 'true' if the value of an element at any row/column position
        // in this table is non-null, and 'false' otherwise.

    bool isRowNull(int rowIndex) const;
        // Return 'true' if the value of the row (as a whole) at the specified
        // 'rowIndex' in this table is null, and 'false' otherwise.  The
        // behavior is undefined unless '0 <= rowIndex < numRows()'.  Note that
        // having every element of a row being null, is not the same as the row
        // itself being null: The latter implies the former, but not vice
        // versa.

    bool isAnyRowNonNull(int rowIndex, int numRows) const;
        // Return 'true' if the value of any of the specified 'numRows'
        // starting at the specified 'rowIndex' of this table is (as a whole)
        // non-null, and 'false' otherwise.  The behavior is undefined unless
        // '0 <= rowIndex', '0 <= numRows', and
        // 'rowIndex + numRows <= numRows()'.  Note that having every element
        // of a row being null, is not the same as the row itself being null:
        // The latter implies the former, but not vice versa.

    bool isAnyRowNull(int rowIndex, int numRows) const;
        // Return 'true' if the value of any of the specified 'numRows'
        // starting at the specified 'rowIndex' of this table is
        // (as a whole) non-null, and 'false' otherwise.  The behavior is
        // undefined unless '0 <= rowIndex', '0 <= numRows', and
        // 'rowIndex + numRows <= numRows()'.  Note that having every element
        // of a row being null, is not the same as the row itself being null:
        // The latter implies the former, but not vice versa.

    int numColumns() const;
        // Return the number of columns in this table.

    int numRows() const;
        // Return the number of rows in this table.

    ConstElemRef rowElemRef(int rowIndex) const;
        // Return a 'bdem' element reference to the non-modifiable row at the
        // specified 'rowIndex' in this table.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()'.

    const Row& theRow(int rowIndex) const;
        // Return a reference to the non-modifiable row header corresponding
        // to the row at the specified 'rowIndex' in this table.  The behavior
        // is undefined unless '0 <= rowIndex < numRows()'.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // absolute value of the optionally specified indentation 'level' and
        // return a reference to 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // CLASS METHOD
    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: Use 'maxSupportedBdexVersion' instead.

    // MANIPULATOR
    void insertRow(int dstIndex, const Row& srcRow);
        // Insert the specified 'srcRow' into this table at the specified
        // 'dstIndex'.  The new row element at each column position is set
        // to the corresponding value in 'srcRow'.  The behavior is undefined
        // unless '0 <= dstIndex <= numRows()', and the number and
        // respective element types in 'srcRow' exactly match those of the
        // columns in this table.
        //
        // DEPRECATED: use
        // 'insertRow(int dstIndex, const Table& table, int index)'
        // instead.

#endif // BDE_OMIT_INTERNAL_DEPRECATED
};

// FREE OPERATORS
bool operator==(const Table& lhs, const Table& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' tables have the same
    // value, and 'false' otherwise.  Two tables have the same value if they
    // have the same number of rows, the same number of columns, each pair of
    // corresponding columns (at the same column index) are of the same 'bdem'
    // element type, each pair of corresponding rows (at the same row index)
    // has (as a whole) the same nullability (i.e., both rows are null or both
    // are not), and each pair of corresponding elements (at the column
    // index and non-null row index) have the same value.

bool operator!=(const Table& lhs, const Table& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' tables do not have the
    // same value, and 'false' otherwise.  Two tables do not have the same
    // value if they do not have the same number of rows, do not have the same
    // number of columns, corresponding columns at one or more column indices
    // are not of the same 'bdem' element type, corresponding rows at one or
    // more row indices do not have the same nullness (as a whole), or
    // corresponding elements at one or more column and non-null row indices do
    // not have the same value.

bsl::ostream& operator<<(bsl::ostream& stream, const Table& table);
    // Format the value of the specified 'table', unindented on a single line,
    // to the specified output 'stream', and return a reference to 'stream'.
    // If stream is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is the same as what would result from
    // invoking:
    //..
    //  table.print(stream, 0, -1);
    //..
    // which is not fully specified, and subject to change without notice.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                            // ----------------
                            // class Table
                            // ----------------

                            // -----------------
                            // Level-0 Functions
                            // -----------------

// ACCESSORS
inline
int Table::numColumns() const
{
    return d_tableImp.numColumns();
}

inline
int Table::numRows() const
{
    return d_tableImp.numRows();
}

                            // -------------------
                            // All Other Functions
                            // -------------------

// CLASS METHODS
inline
int Table::maxSupportedBdexVersion()
{
    return 3;
}

// CREATORS
inline
Table::Table(bslma::Allocator *basicAllocator)
: d_tableImp(AggregateOption::BDEM_PASS_THROUGH, basicAllocator)
{
}

inline
Table::Table(
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_tableImp(allocationStrategy, basicAllocator)
{
}

inline
Table::Table(
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemory,
                  bslma::Allocator                         *basicAllocator)
: d_tableImp(allocationStrategy, initialMemory, basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= initialMemory);
}

inline
Table::Table(const ElemType::Type  columnTypes[],
                       int                        numColumns,
                       bslma::Allocator          *basicAllocator)
: d_tableImp(columnTypes,
             numColumns,
             ElemAttrLookup::lookupTable(),
             AggregateOption::BDEM_PASS_THROUGH,
             basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= numColumns);
}

inline
Table::Table(
                 const ElemType::Type                 columnTypes[],
                 int                                       numColumns,
                 AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma::Allocator                         *basicAllocator)
: d_tableImp(columnTypes,
             numColumns,
             ElemAttrLookup::lookupTable(),
             allocationStrategy,
             basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= numColumns);
}

inline
Table::Table(
                  const ElemType::Type                 columnTypes[],
                  int                                       numColumns,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemory,
                  bslma::Allocator                         *basicAllocator)
: d_tableImp(columnTypes,
             numColumns,
             ElemAttrLookup::lookupTable(),
             allocationStrategy,
             initialMemory,
             basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= numColumns);
    BSLS_ASSERT_SAFE(0 <= initialMemory);
}

inline
Table::Table(const Table&  original,
                       bslma::Allocator  *basicAllocator)
: d_tableImp(original.d_tableImp,
             AggregateOption::BDEM_PASS_THROUGH,
             basicAllocator)
{
}

inline
Table::Table(
                  const Table&                         original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_tableImp(original.d_tableImp, allocationStrategy, basicAllocator)
{
}

inline
Table::Table(
                  const Table&                         original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemory,
                  bslma::Allocator                         *basicAllocator)
: d_tableImp(original.d_tableImp,
             allocationStrategy,
             initialMemory,
             basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= initialMemory);
}

inline
Table::~Table()
{
}

// MANIPULATORS
inline
Table& Table::operator=(const Table& rhs)
{
    d_tableImp = rhs.d_tableImp;
    return *this;
}

inline
void Table::appendNullRow()
{
    insertNullRow(numRows());
}

inline
void Table::appendNullRows(int numRows)
{
    BSLS_ASSERT_SAFE(0 <= numRows);

    insertNullRows(this->numRows(), numRows);
}
}  // close package namespace

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

namespace bdlmxxx {inline
void Table::appendRow(const Row& srcRow)
{
    insertRow(numRows(), srcRow);
}
}  // close package namespace
#endif // BDE_OMIT_INTERNAL_DEPRECATED

namespace bdlmxxx {
inline
void Table::appendRow(const List& srcList)
{
    insertRow(numRows(), srcList);
}

inline
void Table::appendRow(const Table& srcTable, int srcRowIndex)
{
    BSLS_ASSERT_SAFE(0 <= srcRowIndex);
    BSLS_ASSERT_SAFE(     srcRowIndex < srcTable.numRows());

    insertRow(numRows(), srcTable, srcRowIndex);
}

inline
void Table::appendRows(const Table& srcTable)
{
    insertRows(numRows(), srcTable);
}

inline
void Table::appendRows(const Table& srcTable,
                            int               srcRowIndex,
                            int               numRows)
{
    BSLS_ASSERT_SAFE(0 <= srcRowIndex);
    BSLS_ASSERT_SAFE(0 <= numRows);
    BSLS_ASSERT_SAFE(srcRowIndex + numRows <= srcTable.numRows());

    insertRows(this->numRows(), srcTable, srcRowIndex, numRows);
}

inline
void Table::insertNullRow(int dstRowIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstRowIndex);
    BSLS_ASSERT_SAFE(     dstRowIndex <= numRows());

    d_tableImp.insertNullRows(dstRowIndex, 1);
}

inline
void Table::insertNullRows(int dstRowIndex, int numRows)
{
    BSLS_ASSERT_SAFE(0 <= dstRowIndex);
    BSLS_ASSERT_SAFE(     dstRowIndex <= this->numRows());
    BSLS_ASSERT_SAFE(0 <= numRows);

    d_tableImp.insertNullRows(dstRowIndex, numRows);
}

inline
void Table::insertRow(int dstRowIndex, const List& srcList)
{
    BSLS_ASSERT_SAFE(0 <= dstRowIndex);
    BSLS_ASSERT_SAFE(     dstRowIndex <= numRows());

    d_tableImp.insertRow(
                     dstRowIndex,
                     reinterpret_cast<const ListImp&>(srcList).rowData());
}

inline
void Table::insertRow(int               dstRowIndex,
                           const Table& srcTable,
                           int               srcRowIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstRowIndex);
    BSLS_ASSERT_SAFE(     dstRowIndex <= numRows());
    BSLS_ASSERT_SAFE(0 <= srcRowIndex);
    BSLS_ASSERT_SAFE(     srcRowIndex < srcTable.numRows());

    d_tableImp.insertRows(dstRowIndex, srcTable.d_tableImp, srcRowIndex, 1);
}

inline
void Table::insertRows(int dstRowIndex, const Table& srcTable)
{
    BSLS_ASSERT_SAFE(0 <= dstRowIndex);
    BSLS_ASSERT_SAFE(     dstRowIndex <= numRows());

    insertRows(dstRowIndex, srcTable, 0, srcTable.numRows());
}

inline
void Table::insertRows(int               dstRowIndex,
                            const Table& srcTable,
                            int               srcRowIndex,
                            int               numRows)
{
    BSLS_ASSERT_SAFE(0 <= dstRowIndex);
    BSLS_ASSERT_SAFE(     dstRowIndex <= this->numRows());
    BSLS_ASSERT_SAFE(0 <= srcRowIndex);
    BSLS_ASSERT_SAFE(0 <= numRows);
    BSLS_ASSERT_SAFE(srcRowIndex + numRows <= srcTable.numRows());

    d_tableImp.insertRows(dstRowIndex,
                          srcTable.d_tableImp,
                          srcRowIndex,
                          numRows);
}

inline
void Table::makeAllNull()
{
    d_tableImp.makeAllNull();
}

inline
void Table::makeColumnNull(int columnIndex)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.makeColumnNull(columnIndex);
}

inline
void Table::makeRowsNull(int startRowIndex, int numRows)
{
    BSLS_ASSERT_SAFE(0 <= startRowIndex);
    BSLS_ASSERT_SAFE(0 <= numRows);
    BSLS_ASSERT_SAFE(startRowIndex + numRows <= this->numRows());

    d_tableImp.makeRowsNull(startRowIndex, numRows);
}

inline
void Table::reserveMemory(int numBytes)
{
    BSLS_ASSERT_SAFE(0 <= numBytes);

    d_tableImp.reserveMemory(numBytes);
}

inline
void Table::reserveRaw(bsl::size_t numRows)
{
    d_tableImp.reserveRaw(numRows);
}

inline
void Table::reset(const ElemType::Type columnTypes[],
                       int                       numColumns)
{
    BSLS_ASSERT_SAFE(0 <= numColumns);

    d_tableImp.reset(columnTypes,
                     numColumns,
                     ElemAttrLookup::lookupTable());
}

inline
void Table::removeRow(int rowIndex)
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < numRows());

    d_tableImp.removeRow(rowIndex);
}

inline
void Table::removeRows(int startRowIndex, int numRows)
{
    BSLS_ASSERT_SAFE(0 <= startRowIndex);
    BSLS_ASSERT_SAFE(0 <= numRows);
    BSLS_ASSERT_SAFE(startRowIndex + numRows <= this->numRows());

    d_tableImp.removeRows(startRowIndex, numRows);
}

inline
void Table::removeAllRows()
{
    d_tableImp.removeRows(0, numRows());
}

inline
void Table::removeAll()
{
    d_tableImp.removeAll();
}

inline
ElemRef Table::rowElemRef(int rowIndex)
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < numRows());

    return d_tableImp.rowElemRef(rowIndex);
}

inline void Table::setColumnBool(int columnIndex, bool value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValueRaw(columnIndex, &value);
}

inline
void Table::setColumnChar(int columnIndex, char value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValueRaw(columnIndex, &value);
}

inline
void Table::setColumnShort(int columnIndex, short value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValueRaw(columnIndex, &value);
}

inline
void Table::setColumnInt(int columnIndex, int value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValueRaw(columnIndex, &value);
}

inline
void Table::setColumnInt64(int columnIndex, bsls::Types::Int64 value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValueRaw(columnIndex, &value);
}

inline
void Table::setColumnFloat(int columnIndex, float value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValueRaw(columnIndex, &value);
}

inline
void Table::setColumnDouble(int columnIndex, double value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValueRaw(columnIndex, &value);
}

inline
void Table::setColumnString(int columnIndex, const char *value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    bsl::string s(value);
    d_tableImp.setColumnValue(columnIndex, &s);
}

inline
void Table::setColumnString(int                columnIndex,
                                 const bsl::string& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnDatetime(int columnIndex, const bdlt::Datetime& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValueRaw(columnIndex, &value);
}

inline
void Table::setColumnDatetimeTz(int                    columnIndex,
                                     const bdlt::DatetimeTz& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValueRaw(columnIndex, &value);
}

inline
void Table::setColumnDate(int columnIndex, const bdlt::Date& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValueRaw(columnIndex, &value);
}

inline
void Table::setColumnDateTz(int                columnIndex,
                                 const bdlt::DateTz& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValueRaw(columnIndex, &value);
}

inline
void Table::setColumnTime(int columnIndex, const bdlt::Time& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValueRaw(columnIndex, &value);
}

inline
void Table::setColumnTimeTz(int                columnIndex,
                                 const bdlt::TimeTz& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValueRaw(columnIndex, &value);
}

inline
void Table::setColumnBoolArray(int                      columnIndex,
                                    const bsl::vector<bool>& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnCharArray(int                      columnIndex,
                                    const bsl::vector<char>& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnShortArray(int                       columnIndex,
                                     const bsl::vector<short>& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnIntArray(int                     columnIndex,
                                   const bsl::vector<int>& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnInt64Array(
                            int                                    columnIndex,
                            const bsl::vector<bsls::Types::Int64>& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnFloatArray(int                       columnIndex,
                                     const bsl::vector<float>& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnDoubleArray(int                        columnIndex,
                                      const bsl::vector<double>& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnStringArray(
                                   int                             columnIndex,
                                   const bsl::vector<bsl::string>& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnDatetimeArray(
                                 int                               columnIndex,
                                 const bsl::vector<bdlt::Datetime>& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnDatetimeTzArray(
                               int                                 columnIndex,
                               const bsl::vector<bdlt::DatetimeTz>& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnDateArray(int                           columnIndex,
                                    const bsl::vector<bdlt::Date>& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnDateTzArray(
                                   int                             columnIndex,
                                   const bsl::vector<bdlt::DateTz>& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnTimeArray(int                          columnIndex,
                                    const bsl::vector<bdlt::Time>& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnTimeTzArray(
                                   int                             columnIndex,
                                   const bsl::vector<bdlt::TimeTz>& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnChoice(int columnIndex, const Choice& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnChoiceArray(int                     columnIndex,
                                      const ChoiceArray& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnList(int columnIndex, const List& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::setColumnList(int columnIndex, const Row& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    ListImp list(reinterpret_cast<const RowData&>(value),
                      AggregateOption::BDEM_WRITE_ONCE);
    d_tableImp.setColumnValue(columnIndex, &list);
}

inline
void Table::setColumnTable(int columnIndex, const Table& value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    d_tableImp.setColumnValue(columnIndex, &value);
}

inline
void Table::swapRows(int rowIndex1, int rowIndex2)
{
    BSLS_ASSERT_SAFE(0 <= rowIndex1);
    BSLS_ASSERT_SAFE(     rowIndex1 < numRows());
    BSLS_ASSERT_SAFE(0 <= rowIndex2);
    BSLS_ASSERT_SAFE(     rowIndex2 < numRows());

    d_tableImp.swapRows(rowIndex1, rowIndex2);
}

inline
Row& Table::theModifiableRow(int rowIndex)
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < numRows());

    return reinterpret_cast<Row&>(d_tableImp.theModifiableRow(rowIndex));
}

template <class STREAM>
inline
STREAM& Table::bdexStreamIn(STREAM& stream, int version)
{
    d_tableImp.bdexStreamInImp(
                            stream,
                            version,
                            ElemStreamInAttrLookup<STREAM>::lookupTable(),
                            ElemAttrLookup::lookupTable());
    return stream;
}

// ACCESSORS
inline
const Row& Table::operator[](int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < numRows());

    return reinterpret_cast<const Row&>(d_tableImp.theRow(rowIndex));
}

inline
ElemType::Type Table::columnType(int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    return d_tableImp.columnType(columnIndex);
}

inline
bsl::size_t Table::capacityRaw() const
{
    return d_tableImp.capacityRaw();
}

inline
bool Table::isAnyInColumnNull(int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    return d_tableImp.isAnyInColumnNull(columnIndex);
}

inline
bool Table::isAnyInColumnNonNull(int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    return d_tableImp.isAnyInColumnNonNull(columnIndex);
}

inline
bool Table::isAnyNull() const
{
    return d_tableImp.isAnyNull();
}

inline
bool Table::isAnyNonNull() const
{
    return d_tableImp.isAnyNonNull();
}

inline
bool Table::isRowNull(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < numRows());

    return d_tableImp.isAnyRowNull(rowIndex, 1);
}

inline
bool Table::isAnyRowNull(int rowIndex, int numRows) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(0 <= numRows);
    BSLS_ASSERT_SAFE(rowIndex + numRows <= this->numRows());

    return d_tableImp.isAnyRowNull(rowIndex, numRows);
}

inline
bool Table::isAnyRowNonNull(int rowIndex, int numRows) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(0 <= numRows);
    BSLS_ASSERT_SAFE(rowIndex + numRows <= this->numRows());

    return d_tableImp.isAnyRowNonNull(rowIndex, numRows);
}

inline
ConstElemRef Table::rowElemRef(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < numRows());

    return d_tableImp.rowElemRef(rowIndex);
}

inline
const Row& Table::theRow(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < numRows());

    return reinterpret_cast<const Row&>(d_tableImp.theRow(rowIndex));
}

template <class STREAM>
inline
STREAM& Table::bdexStreamOut(STREAM& stream, int version) const
{
    d_tableImp.bdexStreamOutImp(
                          stream,
                          version,
                          ElemStreamOutAttrLookup<STREAM>::lookupTable());
    return stream;
}

inline
bsl::ostream& Table::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    return d_tableImp.print(stream, level, spacesPerLevel);
}
}  // close package namespace

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

namespace bdlmxxx {
inline
int Table::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

inline
void Table::insertRow(int dstIndex, const Row& srcRow)
{
    d_tableImp.insertRow(dstIndex,
                         reinterpret_cast<const RowData&>(srcRow));
}
}  // close package namespace

#endif // BDE_OMIT_INTERNAL_DEPRECATED

// FREE OPERATORS
inline
bool bdlmxxx::operator==(const Table& lhs, const Table& rhs)
{
    return lhs.d_tableImp == rhs.d_tableImp;
}

inline
bool bdlmxxx::operator!=(const Table& lhs, const Table& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& bdlmxxx::operator<<(bsl::ostream& stream, const Table& table)
{
    table.print(stream, 0, -1);
    return stream;
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
