// bdem_binding.h                                                     -*-C++-*-
#ifndef INCLUDED_BDEM_BINDING
#define INCLUDED_BDEM_BINDING

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide proxies for row/table/column/choice/choicearray access.
//
//@CLASSES:
//            bdem_ConstRowBinding: binding to "const" 'bdem_Row'
//                 bdem_RowBinding: binding to modifiable 'bdem_Row'
//          bdem_ConstTableBinding: binding to "const" 'bdem_Table'
//               bdem_TableBinding: binding to modifiable 'bdem_Table'
//         bdem_ConstColumnBinding: binding to "const" column
//              bdem_ColumnBinding: binding to modifiable column
//         bdem_ConstChoiceBinding: binding to "const" 'bdem_Choice'
//              bdem_ChoiceBinding: binding to modifiable 'bdem_Choice'
//    bdem_ConstChoiceArrayBinding: binding to "const" 'bdem_ChoiceArray'
//         bdem_ChoiceArrayBinding: binding to modifiable 'bdem_ChoiceArray'
//
//@AUTHOR: Rohan Bhindwale (rbhindwale)
//
//@SEE_ALSO: bdem_row, bdem_table, bdem_schema, bdem_schemaaggregateutil,
//           bdem_elemref, bdem_choice, bdem_choicearray
//
//@DESCRIPTION: This component provides several classes ("bindings"), each of
// which acts as a proxy to an underlying 'bdem' aggregate object (i.e., an
// instance of 'bdem_Row' or 'bdem_Table').  A "row binding" acts as a proxy to
// an instance of 'bdem_Row' ("row"), whereas a "table binding" acts as a proxy
// to an instance of 'bdem_Table' ("table").  A "column binding", acts as a
// proxy to a single column within an instance of 'bdem_Table' ("column").  A
// "choice binding" acts as a proxy to an instance of 'bdem_Choice' ("choice"),
// and a "choice array binding" acts as a proxy to an instance of
// 'bdem_ChoiceArray.  Within this component, we collectively refer to the row,
// table, column, choice or choice array proxied by a binding as the
// "referenced structure" or "referenced aggregate".
//
// A binding binds its referenced aggregate ("data") to "meta-data" that
// describes the hierarchical structure of the data and the types of the
// elements contained within the data; in turn, a binding is used to access
// elements of its referenced structure.  A row or table binding binds a
// 'bdem_RecordDef' ("record") to a row or table, respectively; a column
// binding binds a 'bdem_FieldDef' ("field") to a column.  Typically the record
// or field bound by a binding is obtained directly from a schema that
// describes a set of 'bdem' structures that the client code processes.
// (See the component-level documentation of 'bdem_schema' for information on
// 'bdem_RecordDef', 'bdem_EnumerationDef', 'bdem_FieldDef', and schemas.)
//
// A binding is a (logical) reference to the data in a referenced aggregate
// coupled to a meta-data descriptor.  Given this close relationship between
// data and meta-data, it is often convenient in documentation to speak of the
// "element having 'elementName' in this binding" to denote the data element in
// the referenced aggregate corresponding to the field named 'elementName' in
// the record meta-data of the binding.  Similarly, it is also convenient to
// speak of the "field named 'elementName' in this binding" to denote the
// corresponding field in the record meta-data of the binding.
//
///Bindings and By-Name Element Access
///-----------------------------------
// The native 'bdem' interfaces (e.g., 'bdem_row' and 'bdem_table') provide
// efficient, indexed access to their contained data elements, but client code
// must be cognizant of the detailed structure of the data for correct access.
// A schema provides structured information about 'bdem' aggregates, including
// names for fields and sub-structures.  Accordingly, a schema can be used to
// support by-name access to data elements, but indices still must be managed
// ("by hand") by client code to take advantage of this capability.  Bindings
// solve this problem by closely associating a 'bdem' aggregate with its
// corresponding meta-data descriptor, and providing methods that directly
// support by-name access to the data.  In fact, by-name access is *the*
// substantive functionality provided by bindings.
//
// Although by-name access sacrifices some efficiency as compared to indexed
// access, code that exclusively employs by-name access to its 'bdem' data can
// be more reliable and maintainable.  For example, code written using indexed
// access may require modification if the ordering of the elements in the
// referenced structure changes at a later date (or, as is more likely, fields
// are inserted, causing the indices of subsequent elements to change).  Of
// course, by-name access is not fool-proof (e.g., fields may be deleted or
// renamed), but it is less error-prone to correct code to account for these
// kinds of modifications.
//
// The relationship expected of the referenced structure and the meta-data
// bound by a row or table binding is such that the bound aggregate must
// contain sufficient data of the appropriate type and structure to match every
// field in the bound record.  For column bindings, each element in the bound
// column must have the appropriate type and structure to match the bound
// field.  (See the component-level documentation of 'bdem_schemaaggregateutil'
// for information on schema/aggregate conformance.  Also see the overloaded
// 'bdem_SchemaAggregateUtil::canSatisfyRecord' methods in that component for
// information on constraint satisfaction.)  Note that it is the user's
// responsibility to ensure that the bound data indeed satisfies the bound
// meta-data.
//
// Given the overriding importance of by-name access afforded by bindings, the
// 'length' methods of the row binding classes return the number of fields in
// the bound record -- and *not* the number of elements in the bound row.
// Similarly, the 'numColumns' methods of the table binding classes also return
// the number of fields in the bound record.  The perspective is different for
// column bindings (for which by-name access is not meaningful) whereby the
// 'numRows' methods return the number of elements in the bound column
// (analogous to the 'numRows' methods of table bindings).
//
///Binding Classes
///---------------
// The 10 binding classes in this component are characterized by (1) the
// kind of aggregate that is bound (i.e., row, table, or column), (2) whether
// the contained elements may be accessed modifiably ("non-const") or only
// non-modifiably ("const")
//..
// A c c e s s
// - - - - - -
//             +--------------------------------+
//     non-    | 'bdem_ConstRowBinding'         |
//  modifiable | 'bdem_ConstTableBinding'       |
//             | 'bdem_ConstColumnBinding'      |
//             | 'bdem_ConstChoiceBinding'      |
//             | 'bdem_ConstChoiceArrayBinding' |
//             |--------------------------------+
//             | 'bdem_RowBinding'              |
//  modifiable | 'bdem_TableBinding'            |
//             | 'bdem_ColumnBinding'           |
//             | 'bdem_ChoiceBinding'           |
//             | 'bdem_ChoiceArrayBinding'      |
//             +--------------------------------+
//..
///Bindings
/// - - - -
// The following diagram shows, generically, the relationship between the
// "const" and "non-const" variants of the binding classes (where "XXX" is
// "Row", "Table", or "Column"):
//..
//                  ,---------------.          Additional functionality
//                 ( bdem_XXXBinding )         to enable modification
//                  `---------------'          of non-const elements,
//                          |                  but no additional data.
//                          v
//                ,--------------------.
//               ( bdem_ConstXXXBinding )      No virtual functions.
//                `--------------------'
//..
// The (base) classes 'bdem_ConstRowBinding', 'bdem_ConstTableBinding', and
// 'bdem_ConstColumnBinding' provide non-modifiable access to the referenced
// structure and other 'const' functions (e.g., returning the number of
// elements in the referenced structure, the type of an element at a specified
// index, etc.).  The derived classes 'bdem_RowBinding', 'bdem_TableBinding',
// and 'bdem_ColumnBinding' extend the capabilities of their respective base
// classes by providing modifiable access to the referenced structure.  Unless
// the referenced structure is modifiable, a 'bdem_RowBinding',
// 'bdem_TableBinding', or 'bdem_ColumnBinding' cannot be created for it; a
// 'bdem_ConstRowBinding', 'bdem_ConstTableBinding', or
// 'bdem_ConstColumnBinding' must be used instead.
//
///Reference Semantics
///-------------------
// A binding object provides (possibly modifiable) access to a referenced
// structure using "reference semantics".  Reference semantics are analogous to
// the semantics of fundamental reference types and, in particular, differ
// significantly from the more common (within 'bde') value semantics.  A
// binding object is bound to its referenced structure at construction and by
// subsequent calls to the 'rebind' manipulator methods.  Following
// construction (or rebinding), all accessor functions on the binding apply
// directly to the referenced structure or to the held meta-data and not to the
// binding itself, as conventional (value) semantics might suggest.  Note,
// however, that since the conventional assignment operator might reasonably be
// expected to assign the referenced structure and the bound record or field,
// but that such behavior is impractical (or even undesirable), 'operator=' has
// been suppressed from all bindings.
//
///Semantics of 'const'
/// - - - - - - - - - -
// In further analogy to fundamental references, bindings are provided in two
// forms, (logically) 'const' and (logically) non-'const'.  The constant
// (base-class) versions:
//..
//    'bdem_ConstRowBinding'
//    'bdem_ConstTableBinding'
//    'bdem_ConstColumnBinding'
//    'bdem_ConstChoiceBinding'
//    'bdem_ConstChoiceArrayBinding'
//..
// omit all operations that would enable the client to modify the referenced
// structure.  These classes are collectively referred to as "const" bindings.
// The modifiable (derived) versions (collectively referred to as "non-const"
// bindings):
//..
//    'bdem_RowBinding'
//    'bdem_TableBinding'
//    'bdem_ColumnBinding'
//    'bdem_ChoiceBinding'
//    'bdem_ChoiceArrayBinding'
//..
// extend the behavior of their respective base classes to permit modification
// to the referenced structure.
//
// Unless the *referenced structure* is modifiable, a "non-const" binding
// object cannot be created for it; a "const" binding must be used instead.  An
// instance of a binding that is declared 'const' (whether or not that instance
// is a "non-const" binding) cannot be rebound to reference a different
// aggregate after construction since the 'rebind' methods are (non-'const')
// manipulators.
//
///Aggregate Lifetime
/// - - - - - - - - -
// As is the case for a fundamental reference, a binding has no management
// responsibility for its referenced structure, and, in particular, cannot
// determine whether or not the referenced *structure* is valid.  It is the
// user's responsibility to keep track of the valid lifetime of a referenced
// structure.  This is especially important when the referenced structure is
// contained in a 'bdem_List', a 'bdem_Table', or other container, because
// modification of the container may affect the validity of the reference.
// Unless otherwise noted, the documented behavior of all methods provided by
// this component assume pointers to valid objects.
//
///Usage
///-----
// Given the large number of classes provided by this component, we present
// three examples illustrating basic use of several of the binding classes.
// The three examples make use of a common schema and common data, which is
// described in this initial section.  The three examples follow in separate
// sub-sections.
//
// The schema used in these examples is shown in Figure 1 below:
//..
//        +------------------------------------------+
//        |{                                         |
//        |    RECORD "EMPLOYEE" {                   |
//        |        STRING            "empName";      |
//        |        INT               "salary";       |
//        |        DATE              "dateOfHire";   |
//        |    }                                     |
//        |                                          |
//        |    RECORD "TEAM" {                       |
//        |        STRING            "teamName";     |
//        |        DOUBLE            "revenue";      |
//        |        LIST<"EMPLOYEE">  "leader";       |
//        |        TABLE<"EMPLOYEE"> "members";      |
//        |    }                                     |
//        |                                          |
//        |    RECORD "DEPARTMENT" {                 |
//        |        STRING            "deptName";     |
//        |        DOUBLE            "revenue";      |
//        |        TABLE<"TEAM">     "teams";        |
//        |    }                                     |
//        |}                                         |
//        +------------------------------------------+
//
//               Fig 1 -- "DEPARTMENT" Schema
//..
// This schema describes hypothetical departments, teams, and employees.  A
// department has a name ("deptName"), earns "revenue", and consists of one or
// more "teams".  Each team also has a name ("teamName") and earns "revenue".
// Furthermore, a team consists of one or more employees (its "members"), with
// one of these employees designated the team "leader".  Lastly, an employee
// has a name ("empName"), earns a "salary", and was hired on some date
// ("dateOfHire").  The following block of code initializes an instance of
// 'bdem_Schema' corresponding to the schema in Figure 1:
//..
//  bdem_Schema deptSchema;
//  {
//      // EMPLOYEE record
//
//      bdem_RecordDef *employee = deptSchema.createRecord("EMPLOYEE");
//
//      employee->appendField(bdem_ElemType::BDEM_STRING, "empName");
//      employee->appendField(bdem_ElemType::BDEM_INT,    "salary");
//      employee->appendField(bdem_ElemType::BDEM_DATE,   "dateOfHire");
//
//      // TEAM record
//
//      bdem_RecordDef *team = deptSchema.createRecord("TEAM");
//
//      team->appendField(bdem_ElemType::BDEM_STRING, "teamName");
//      team->appendField(bdem_ElemType::BDEM_DOUBLE, "revenue");
//
//      team->appendField(bdem_ElemType::BDEM_LIST,  employee, "leader");
//      team->appendField(bdem_ElemType::BDEM_TABLE, employee, "members");
//
//      // DEPARTMENT record
//
//      bdem_RecordDef *department = deptSchema.createRecord("DEPARTMENT");
//
//      department->appendField(bdem_ElemType::BDEM_STRING, "deptName");
//      department->appendField(bdem_ElemType::BDEM_DOUBLE, "revenue");
//
//      department->appendField(bdem_ElemType::BDEM_TABLE, team, "teams");
//  }
//..
// When the 'deptSchema' instance is printed on 'bsl::cout':
//..
//  bsl::cout << deptSchema << bsl::endl;
//..
// it displays as follows:
//..
//  {
//      RECORD "EMPLOYEE" {
//          STRING "empName";
//          INT "salary";
//          DATE "dateOfHire";
//      }
//
//      RECORD "TEAM" {
//          STRING "teamName";
//          DOUBLE "revenue";
//          LIST<"EMPLOYEE"> "leader";
//          TABLE<"EMPLOYEE"> "members";
//      }
//
//      RECORD "DEPARTMENT" {
//          STRING "deptName";
//          DOUBLE "revenue";
//          TABLE<"TEAM"> "teams";
//      }
//  }
//..
// Next we provide sample data used to populate a department consisting of two
// teams, each having four members.  For convenience, we first populate a
// 'struct'.  A more realistic example might make use of the text-parsing
// methods in 'bdempu' to go directly from "source" text to 'bdem' data:
//..
//  static const struct Employee {
//      bsl::string d_name;
//      int         d_salary;
//      bdet_Date   d_hireDate;
//  } EMPLOYEES[] = {
//      // name                salary (knuts x K)   hireDate
//      // -----------------   ------------------   -----------------------
//      {  "Alice Field",      135,                 bdet_Date(2001,  1, 27)  },
//      {  "Martina Hingis",   107,                 bdet_Date(1999, 11,  9)  },
//      {  "Sean O'Casey",     126,                 bdet_Date(1998,  5,  5)  },
//      {  "Harry Tonto",       95,                 bdet_Date(2003,  4,  9)  },
//      {  "Michael Blix",     125,                 bdet_Date(1998,  5,  5)  },
//      {  "Jen-Pi Huang",     110,                 bdet_Date(2004,  2, 16)  },
//      {  "John Smith",       105,                 bdet_Date(2004,  7, 17)  },
//      {  "Sandra Mueller",    95,                 bdet_Date(2004,  8,  8)  },
//  };
//  const int numEmployees = sizeof(EMPLOYEES) / sizeof(*EMPLOYEES);
//
//  const bdem_ElemType::Type EmpTypes[] = {
//      bdem_ElemType::BDEM_STRING,
//      bdem_ElemType::BDEM_INT,
//      bdem_ElemType::BDEM_DATE
//  };
//  const int numEmpTypes = sizeof(EmpTypes) / sizeof(*EmpTypes);
//
//  static const struct Team {
//      bsl::string d_name;
//      double      d_revenue;
//  } TEAMS[] = {
//      // name  revenue (knuts x M)
//      // ----  -------------------
//      {  "A",  100.50             },
//      {  "B",   73.27             },
//  };
//  const int numTeams = sizeof(TEAMS) / sizeof(*TEAMS);
//
//  const bdem_ElemType::Type TeamTypes[] = {
//      bdem_ElemType::BDEM_STRING,
//      bdem_ElemType::BDEM_DOUBLE,
//      bdem_ElemType::BDEM_LIST,
//      bdem_ElemType::BDEM_TABLE
//  };
//  const int numTeamTypes = sizeof(TeamTypes) / sizeof(*TeamTypes);
//
//  const bdem_ElemType::Type DeptTypes[] = {
//      bdem_ElemType::BDEM_STRING,
//      bdem_ElemType::BDEM_DOUBLE,
//      bdem_ElemType::BDEM_TABLE
//  };
//  const int numDeptTypes = sizeof(DeptTypes) / sizeof(*DeptTypes);
//..
// Teams and team leaders are instances of 'bdem_List'.  The team leaders for
// our two teams are 'leaderA' and 'leaderB':
//..
//  bdem_List leaderA(EmpTypes, numEmpTypes);
//  leaderA[0].theModifiableString() = EMPLOYEES[0].d_name;
//  leaderA[1].theModifiableInt()    = EMPLOYEES[0].d_salary;
//  leaderA[2].theModifiableDate()   = EMPLOYEES[0].d_hireDate;
//
//  bdem_List leaderB(EmpTypes, numEmpTypes);
//  leaderB[0].theModifiableString() = EMPLOYEES[4].d_name;
//  leaderB[1].theModifiableInt()    = EMPLOYEES[4].d_salary;
//  leaderB[2].theModifiableDate()   = EMPLOYEES[4].d_hireDate;
//..
// 'teamA' and 'teamB' are the teams led by 'leaderA' and 'leaderB',
// respectively:
//..
//  bdem_List teamA(TeamTypes, numTeamTypes);
//  teamA[0].theModifiableString() = TEAMS[0].d_name;
//  teamA[1].theModifiableDouble() = TEAMS[0].d_revenue;
//  teamA[2].theModifiableList()   = leaderA;
//  teamA[3].theModifiableTable()  = bdem_Table(EmpTypes, numEmpTypes);
//  int i;
//  for (i = 0; i < 4; ++i) {
//      bdem_List member(EmpTypes, numEmpTypes);
//      member[0].theModifiableString() = EMPLOYEES[i].d_name;
//      member[1].theModifiableInt()    = EMPLOYEES[i].d_salary;
//      member[2].theModifiableDate()   = EMPLOYEES[i].d_hireDate;
//      teamA[3].theModifiableTable().appendRow(member);
//  }
//
//  bdem_List teamB(TeamTypes, numTeamTypes);
//  teamB[0].theModifiableString() = TEAMS[1].d_name;
//  teamB[1].theModifiableDouble() = TEAMS[1].d_revenue;
//  teamB[2].theModifiableList()   = leaderB;
//  teamB[3].theModifiableTable()  = bdem_Table(EmpTypes, numEmpTypes);
//  for (i = 4; i < numEmployees; ++i) {
//      bdem_List member(EmpTypes, numEmpTypes);
//      member[0].theModifiableString() = EMPLOYEES[i].d_name;
//      member[1].theModifiableInt()    = EMPLOYEES[i].d_salary;
//      member[2].theModifiableDate()   = EMPLOYEES[i].d_hireDate;
//      teamB[3].theModifiableTable().appendRow(member);
//  }
//..
// Finally, we create a 'bdem_List' comprised of 'teamA' and 'teamB' to
// represent the "Widgets" department:
//..
//  bdem_List widgetsDept(DeptTypes, numDeptTypes);
//  widgetsDept[0].theModifiableString() = "Widgets";  // department name
//  widgetsDept[1].theModifiableDouble() = 195.7;      // revenue (knuts x M)
//  widgetsDept[2].theModifiableTable()  = bdem_Table(TeamTypes, numTeamTypes);
//  widgetsDept[2].theModifiableTable().appendRow(teamA);
//  widgetsDept[2].theModifiableTable().appendRow(teamB);
//
//..
// When printed on 'bsl::cout':
//..
//  bsl::cout << "Team A" << bsl::endl
//            << "------" << bsl::endl;  teamA.print(bsl::cout, -2, 2);
//  bsl::cout << "Team B" << bsl::endl
//            << "------" << bsl::endl;  teamB.print(bsl::cout, -2, 2);
//..
// the two teams display as follows:
//..
//  Team A
//  ------
//  [
//        STRING A
//        DOUBLE 100.5
//        LIST [
//          STRING Alice Field
//          INT 135
//          DATE 27JAN2001
//        ]
//        TABLE [
//          Row 0: [
//            STRING Alice Field
//            INT 135
//            DATE 27JAN2001
//          ]
//          Row 1: [
//            STRING Martina Hingis
//            INT 107
//            DATE 09NOV1999
//          ]
//          Row 2: [
//            STRING Sean O'Casey
//            INT 126
//            DATE 05MAY1998
//          ]
//          Row 3: [
//            STRING Harry Tonto
//            INT 95
//            DATE 09APR2003
//          ]
//        ]
//      ]
//  Team B
//  ------
//  [
//        STRING B
//        DOUBLE 73.27
//        LIST [
//          STRING Michael Blix
//          INT 125
//          DATE 05MAY1998
//        ]
//        TABLE [
//          Row 0: [
//            STRING Michael Blix
//            INT 125
//            DATE 05MAY1998
//          ]
//          Row 1: [
//            STRING Jen-Pi Huang
//            INT 110
//            DATE 16FEB2004
//          ]
//          Row 2: [
//            STRING John Smith
//            INT 105
//            DATE 17JUL2004
//          ]
//          Row 3: [
//            STRING Sandra Mueller
//            INT 95
//            DATE 08AUG2004
//          ]
//        ]
//      ]
//..
///Usage Example 1
///- - - - - - - -
// This first example illustrates basic use of row bindings.  First we create
// a row binding useful for accessing elements of 'teamA' using the constructor
// that takes a 'const bdem_List *', a 'const bdem_Schema *', and a
// 'const char *' (naming a record in the supplied schema):
//..
//  {
//      const bdem_ConstRowBinding CRB(&teamA, &deptSchema, "TEAM");
//      assert(teamA.length() >= CRB.length());
//..
// Using names corresponding to fields in the bound record, the name and
// revenue of Team A may be accessed as follows:
//..
//      const bsl::string& teamName = CRB.theString("teamName");
//      const double       revenue  = CRB.theDouble("revenue");
//
//      bsl::cout << "Team "                           << teamName
//                << " current revenue (knuts x M) = " << revenue << bsl::endl;
//..
// The same two fields also may be accessed by their index in the record.
// The 'operator[]' method of 'bdem_ConstRowBinding' returns an instance of
// 'bdem_ConstElemRef' (an element reference):
//..
//      assert(teamName == CRB[0].theString());
//      assert(revenue  == CRB[1].theDouble());
//..
// Next we create a second row binding to access elements of the "leader" of
// Team A.  The 'rowBinding' method is used to construct this binding:
//..
//      bdem_ConstRowBinding leader(CRB.rowBinding("leader"));
//      const bsl::string&   leaderName = leader.theString("empName");
//      const bdet_Date      hiredOn    = leader.theDate  ("dateOfHire");
//
//      bsl::cout << "  Led by "   << leaderName
//                << " (hired on " << hiredOn << ")" << bsl::endl;
//..
// The 'CRB' binding cannot be used to modify its bound aggregate.  We create
// a second binding for that purpose.  In particular, we update the revenue of
// Team A using the 'RB' binding:
//..
//      const bdem_RowBinding RB(&teamA, &deptSchema, "TEAM");
//      RB.theModifiableDouble("revenue") += 12.7;
//..
// We access the revenue of Team A through the 'CRB' binding a second time to
// obtain the latest figure:
//..
//      const double revisedRevenue = CRB.theDouble("revenue");
//
//      bsl::cout << "Team "                           << teamName
//                << " revised revenue (knuts x M) = " << revisedRevenue
//                << bsl::endl;
//..
// The 'leader' binding may be rebound to another 'bdem_List' that satisfies
// the 'EMPLOYEE' record.  In the following, we rebind 'leader' to the leader
// of Team B:
//..
//      leader.rebind(&leaderB);
//      const bsl::string& anotherLeaderName = leader.theString("empName");
//
//      bsl::cout << anotherLeaderName << " is another team leader."
//                << bsl::endl;
//..
// Although 'RB' binds a modifiable row, it may *not* be rebound to another row
// since it was declared 'const':
//..
//      // RB.rebind(&teamB);  // compile-time error
//  }
//..
// The above block of code prints the following on 'bsl::cout':
//..
//  Team A current revenue (knuts x M) = 100.5
//    Led by Alice Field (hired on 27JAN2001)
//  Team A revised revenue (knuts x M) = 113.2
//  Michael Blix is another team leader.
//..
///Usage Example 2
///- - - - - - - -
// This second example illustrates row, table, and column bindings used in
// combination to print out the members of the Widgets department.  We create
// a row binding to access the contents of the department:
//..
//  {
//      const bdem_ConstRowBinding dept(&widgetsDept,
//                                      &deptSchema,
//                                      "DEPARTMENT");
//      const bsl::string& deptName = dept.theString("deptName");
//      bsl::cout << deptName << " Department" << bsl::endl;
//..
// Next, we create a table binding to access the teams that belong to the
// Widgets department.  The 'tableBinding' method yields a table binding that
// is used to initialize the 'allTeams' binding:
//..
//      const bdem_ConstTableBinding allTeams(dept.tableBinding("teams"));
//..
// We now iterate over the teams in the Widgets department, printing the names
// of the members of each team:
//..
//      const int numTeams = allTeams.numRows();
//      for (int i = 0; i < numTeams; ++i) {
//..
// The table binding's 'rowBinding' method returns a row binding for the team
// that will be processed in the current iteration:
//..
//          const bdem_ConstRowBinding team(allTeams.rowBinding(i));
//          const bsl::string& teamName = team.theString("teamName");
//..
// We access the name of the leader of the current team through the temporary
// row binding returned by the call to 'rowBinding("leader")':
//..
//          const bsl::string& leaderName =
//                              team.rowBinding("leader").theString("empName");
//          bsl::cout << "  Team "   << teamName
//                    << ", led by " << leaderName << ", includes:"
//                    << bsl::endl;
//..
// Since we are interested in accessing only the names of team members, we
// opt to use a column binding that is bound to the column holding the names.
// The call to 'tableBinding("members")' returns a temporary table binding.
// The call to 'columnBinding("empName")' on that transient binding yields the
// column binding that is desired:
//..
//          const bdem_ConstColumnBinding members(
//                      team.tableBinding("members").columnBinding("empName"));
//..
// Finally, we iterate over all of the members of the current team and print
// their names.  The name of the team leader was already printed above, so it
// is treated as a special case and filtered out in the following loop.
// Similar to row bindings, the column binding's 'operator[]' method returns an
// element reference:
//..
//          const int numMembers = members.numRows();
//          for (int j = 0; j < numMembers; ++j) {
//              const bsl::string& memberName = members[j].theString();
//              if (memberName != leaderName) {
//                  bsl::cout << "    " << memberName << bsl::endl;
//              }
//          }
//      }
//  }
//..
// The previous block of code prints the following on 'bsl::cout':
//..
//  Widgets Department
//    Team A, led by Alice Field, includes:
//      Martina Hingis
//      Sean O'Casey
//      Harry Tonto
//    Team B, led by Michael Blix, includes:
//      Jen-Pi Huang
//      John Smith
//      Sandra Mueller
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_AGGREGATE
#include <bdem_aggregate.h>
#endif

#ifndef INCLUDED_BDEM_ELEMREF
#include <bdem_elemref.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BDEM_SCHEMA
#include <bdem_schema.h>
#endif

#ifndef INCLUDED_BDEM_SCHEMAAGGREGATEUTIL
#include <bdem_schemaaggregateutil.h>
#endif

#ifndef INCLUDED_BDEM_SCHEMAUTIL
#include <bdem_schemautil.h>
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

class bdem_RowBinding;
class bdem_ColumnBinding;
class bdem_ConstChoiceBinding;
class bdem_ChoiceBinding;
class bdem_ConstChoiceArrayBinding;
class bdem_ChoiceArrayBinding;
class bdem_ConstTableBinding;
class bdem_TableBinding;

                        // ==========================
                        // class bdem_ConstRowBinding
                        // ==========================

class bdem_ConstRowBinding {
    // This class binds a non-modifiable 'bdem_Row' ("row") to a non-modifiable
    // 'bdem_RecordDef' ("record"), and provides both by-name and indexed
    // access to the elements of the row.  Indexed access is an efficient
    // constant-time operation; by-name access is somewhat less efficient, but
    // no less so than by-name access using the record 'lookup' methods.  For
    // correct by-name access to elements in the bound row, the row data must
    // satisfy the record meta-data.
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // although copy construction and equality are supported.  Except for the
    // constructors, destructor, and the overloaded 'rebind' methods, all
    // instance methods of this object are 'const' and apply not to this
    // object, but rather to the bound row or the bound meta-data.  Note also
    // that, unlike the derived 'bdem_RowBinding' class (also defined within
    // this component), none of the methods within this base class enable
    // modifiable access to the bound row, thus ensuring 'const' correctness.

  public:
    // TYPES
    typedef bdem_ConstElemRef            ElemRef;
    typedef bdem_ConstRowBinding         RowBinding;
    typedef bdem_ConstTableBinding       TableBinding;
    typedef bdem_ConstChoiceBinding      ChoiceBinding;
    typedef bdem_ConstChoiceArrayBinding ChoiceArrayBinding;
    typedef const bdem_Row               Row;
    typedef const bdem_List              List;

  private:
    // DATA
    const bdem_Row       *d_row_p;         // data (held, not owned)
    const bdem_RecordDef *d_constraint_p;  // meta-data (held, not owned)

    // FRIENDS
    friend bool operator==(const bdem_ConstRowBinding& lhs,
                           const bdem_ConstRowBinding& rhs);

    friend class bdem_RowBinding;
        // Allow the derived class to access the private members of this class.

  private:
    // NOT IMPLEMENTED
    bdem_ConstRowBinding& operator=(const bdem_ConstRowBinding&);

  public:
    // CREATORS
    bdem_ConstRowBinding(const bdem_Row *row, const bdem_RecordDef *record);
        // Create a "const" row binding that binds the specified non-modifiable
        // 'row' to the specified non-modifiable 'record'.  The behavior is
        // undefined unless the 'row' data can satisfy the 'record' meta-data.

    bdem_ConstRowBinding(const bdem_Row    *row,
                         const bdem_Schema *schema,
                         const char        *recordName);
        // Create a "const" row binding that binds the specified non-modifiable
        // 'row' to the non-modifiable record definition having the specified
        // 'recordName' in the specified 'schema'.  The behavior is undefined
        // unless the 'row' data can satisfy the record meta-data named
        // 'recordName' in 'schema'.

    bdem_ConstRowBinding(const bdem_Row    *row,
                         const bdem_Schema *schema,
                         int                recordIndex);
        // Create a "const" row binding that binds the specified non-modifiable
        // 'row' to the non-modifiable record definition at the specified
        // 'recordIndex' in the specified 'schema'.  The behavior is undefined
        // unless '0 <= recordIndex < schema->length()' and the 'row' data can
        // satisfy the record meta-data at 'recordIndex' in 'schema'.

    bdem_ConstRowBinding(const bdem_List      *list,
                         const bdem_RecordDef *record);
        // Create a "const" row binding that binds the non-modifiable row in
        // the specified 'list' to the specified non-modifiable 'record'.  The
        // behavior is undefined unless the row data in 'list' can satisfy the
        // 'record' meta-data.

    bdem_ConstRowBinding(const bdem_List   *list,
                         const bdem_Schema *schema,
                         const char        *recordName);
        // Create a "const" row binding that binds the non-modifiable row in
        // the specified 'list' to the non-modifiable record definition having
        // the specified 'recordName' in the specified 'schema'.  The behavior
        // is undefined unless the row data in 'list' can satisfy the record
        // meta-data named 'recordName' in 'schema'.

    bdem_ConstRowBinding(const bdem_List   *list,
                         const bdem_Schema *schema,
                         int                recordIndex);
        // Create a "const" row binding that binds the non-modifiable row in
        // the specified 'list' to the non-modifiable record definition at the
        // specified 'recordIndex' in the specified 'schema'.  The behavior is
        // undefined unless '0 <= recordIndex < schema->length()' and the row
        // data in 'list' can satisfy the record meta-data at 'recordIndex' in
        // 'schema'.

    bdem_ConstRowBinding(const bdem_Table     *table,
                         int                   rowIndex,
                         const bdem_RecordDef *record);
        // Create a "const" row binding that binds the non-modifiable row at
        // the specified 'rowIndex' in the specified 'table' to the specified
        // non-modifiable 'record'.  The behavior is undefined unless
        // '0 <= rowIndex < table->numRows()' and the row data at 'rowIndex' in
        // 'table' can satisfy the 'record' meta-data.

    bdem_ConstRowBinding(const bdem_Table  *table,
                         int                rowIndex,
                         const bdem_Schema *schema,
                         const char        *recordName);
        // Create a "const" row binding that binds the non-modifiable row at
        // the specified 'rowIndex' in the specified 'table' to the
        // non-modifiable record definition having the specified 'recordName'
        // in the specified 'schema'.  The behavior is undefined unless
        // '0 <= rowIndex < table->numRows()' and the row data at 'rowIndex' in
        // 'table' can satisfy the record meta-data named 'recordName' in
        // 'schema'.

     bdem_ConstRowBinding(const bdem_Table  *table,
                          int                rowIndex,
                          const bdem_Schema *schema,
                          int                recordIndex);
        // Create a "const" row binding that binds the non-modifiable row at
        // the specified 'rowIndex' in the specified 'table' to the
        // non-modifiable record definition at the specified 'recordIndex' in
        // the specified 'schema'.  The behavior is undefined unless
        // '0 <= rowIndex < table->numRows()',
        // '0 <= recordIndex < schema->length()', and the row data at
        // 'rowIndex' in 'table' can satisfy the record meta-data at
        // 'recordIndex' in 'schema'.

    bdem_ConstRowBinding(const bdem_ConstRowBinding& original);
        // Create a "const" row binding that binds a non-modifiable row to a
        // non-modifiable record, both acquired from the specified 'original'
        // row binding.

    // ~bdem_ConstRowBinding();
        // Destroy this 'const' row binding.  Note that this trivial destructor
        // is generated by the compiler.

    // MANIPULATORS
    void rebind(const bdem_ConstRowBinding& binding);
        // Reinitialize this row binding to bind a newly-acquired
        // non-modifiable row to a newly-acquired non-modifiable record, both
        // acquired from the specified row 'binding'.

    void rebind(const bdem_Row *row);
        // Reinitialize this row binding to bind the specified non-modifiable
        // 'row' to the non-modifiable record already held by this binding.
        // The behavior is undefined unless the 'row' data can satisfy the
        // record meta-data.

    void rebind(const bdem_List *list);
        // Reinitialize this row binding to bind the non-modifiable row in the
        // specified 'list' to the non-modifiable record already held by this
        // binding.  The behavior is undefined unless the row data in 'list'
        // can satisfy the record meta-data.

    void rebind(const bdem_Table *table, int rowIndex);
        // Reinitialize this row binding to bind the non-modifiable row at the
        // specified 'rowIndex' in the specified 'table' to the non-modifiable
        // record already held by this binding.  The behavior is undefined
        // unless '0 <= rowIndex < table->numRows()' and the row data at
        // 'rowIndex' in 'table' can satisfy the record meta-data.

    void rebind(const bdem_Row *row, const bdem_RecordDef *record);
        // Reinitialize this row binding to bind the specified non-modifiable
        // 'row' to the specified non-modifiable 'record'.  The behavior is
        // undefined unless the 'row' data can satisfy the 'record' meta-data.

    void rebind(const bdem_Row    *row,
                const bdem_Schema *schema,
                const char        *recordName);
        // Reinitialize this row binding to bind the specified non-modifiable
        // 'row' to the non-modifiable record definition having the specified
        // 'recordName' in the specified 'schema'.  The behavior is undefined
        // unless the 'row' data can satisfy the record meta-data named
        // 'recordName' in 'schema'.

    void rebind(const bdem_Row    *row,
                const bdem_Schema *schema,
                int                recordIndex);
        // Reinitialize this row binding to bind the specified non-modifiable
        // 'row' to the non-modifiable record definition at the specified
        // 'recordIndex' in the specified 'schema'.  The behavior is undefined
        // unless '0 <= recordIndex < schema->length()' and the 'row' data can
        // satisfy the record meta-data at 'recordIndex' in 'schema'.

    void rebind(const bdem_List *list, const bdem_RecordDef *record);
        // Reinitialize this row binding to bind the non-modifiable row in the
        // specified 'list' to the specified non-modifiable 'record'.  The
        // behavior is undefined unless the row data in 'list' can satisfy the
        // 'record' meta-data.

    void rebind(const bdem_List   *list,
                const bdem_Schema *schema,
                const char        *recordName);
        // Reinitialize this row binding to bind the non-modifiable row in the
        // specified 'list' to the non-modifiable record definition having the
        // specified 'recordName' in the specified 'schema'.  The behavior is
        // undefined unless the row data in 'list' can satisfy the record
        // meta-data named 'recordName' in 'schema'.

    void rebind(const bdem_List   *list,
                const bdem_Schema *schema,
                int                recordIndex);
        // Reinitialize this row binding to bind the non-modifiable row in the
        // specified 'list' to the non-modifiable record definition at the
        // specified 'recordIndex' in the specified 'schema'.  The behavior is
        // undefined unless '0 <= recordIndex < schema->length()' and the row
        // data in 'list' can satisfy the record meta-data at 'recordIndex' in
        // 'schema'.

    void rebind(const bdem_Table     *table,
                int                   rowIndex,
                const bdem_RecordDef *record);
        // Reinitialize this row binding to bind the non-modifiable row at the
        // specified 'rowIndex' in the specified 'table' to the specified
        // non-modifiable 'record'.  The behavior is undefined unless
        // '0 <= rowIndex < table->numRows()' and the row data at 'rowIndex' in
        // 'table' can satisfy the 'record' meta-data.

    void rebind(const bdem_Table  *table,
                int                rowIndex,
                const bdem_Schema *schema,
                const char        *recordName);
        // Reinitialize this row binding to bind the non-modifiable row at the
        // specified 'rowIndex' in the specified 'table' to the non-modifiable
        // record definition having the specified 'recordName' in the specified
        // 'schema'.  The behavior is undefined unless
        // '0 <= rowIndex < table->numRows()' and the row data at 'rowIndex' in
        // 'table' can satisfy the record meta-data named 'recordName' in
        // 'schema'.

    void rebind(const bdem_Table  *table,
                int                rowIndex,
                const bdem_Schema *schema,
                int                recordIndex);
        // Reinitialize this row binding to bind the non-modifiable row at the
        // specified 'rowIndex' in the specified 'table' to the non-modifiable
        // record definition at the specified 'recordIndex' in the specified
        // 'schema'.  The behavior is undefined unless
        // '0 <= rowIndex < table->numRows()',
        // '0 <= recordIndex < schema->length()', and the row data at
        // 'rowIndex' in 'table' can satisfy the record meta-data at
        // 'recordIndex' in 'schema'.

    // ACCESSORS
    bdem_ConstElemRef operator[](int index) const;
        // Return a "const" element reference that refers to the non-modifiable
        // element at the specified 'index' in this binding.  The behavior is
        // undefined unless '0 <= index < row().length()'.

    bdem_ConstElemRef element(const char *elementName) const;
        // Return a "const" element reference that refers to the non-modifiable
        // element having the specified 'elementName' in this binding.  The
        // behavior is undefined unless this binding has a field named
        // 'elementName'.

    bdem_ConstElemRef element(int index) const;
        // Return a "const" element reference that refers to the non-modifiable
        // element at the specified 'index' in this binding.  The behavior is
        // undefined unless '0 <= index < row().length()'.

    bdem_ElemType::Type elemType(const char *elementName) const;
        // Return the 'bdem_ElemType::Type' value of the element having the
        // specified 'elementName' in this binding.  The behavior is undefined
        // unless this binding has a field named 'elementName'.

    bdem_ElemType::Type elemType(int index) const;
        // Return the 'bdem_ElemType::Type' value of the element at the
        // specified 'index' in this binding.  The behavior is undefined unless
        // '0 <= index < row().length()'.

    int length() const;
        // Return the number of fields in the record held by this binding.

    const bdem_RecordDef& record() const;
        // Return a reference to the non-modifiable record held by this
        // binding.

    const bdem_Row& row() const;
        // Return a reference to the non-modifiable row held by this binding.

    const bool& theBool(const char *elementName) const;
    const char& theChar(const char *elementName) const;
    const short& theShort(const char *elementName) const;
    const int& theInt(const char *elementName) const;
    const bsls_Types::Int64& theInt64(const char *elementName) const;
    const float& theFloat(const char *elementName) const;
    const double& theDouble(const char *elementName) const;
    const bsl::string& theString(const char *elementName) const;
    const bdet_Datetime& theDatetime(const char *elementName) const;
    const bdet_DatetimeTz& theDatetimeTz(const char *elementName) const;
    const bdet_Date& theDate(const char *elementName) const;
    const bdet_DateTz& theDateTz(const char *elementName) const;
    const bdet_Time& theTime(const char *elementName) const;
    const bdet_TimeTz& theTimeTz(const char *elementName) const;
    const bsl::vector<bool>& theBoolArray(const char *elementName) const;
    const bsl::vector<char>& theCharArray(const char *elementName) const;
    const bsl::vector<short>& theShortArray(const char *elementName) const;
    const bsl::vector<int>& theIntArray(const char *elementName) const;
    const bsl::vector<bsls_Types::Int64>& theInt64Array(
                                                const char *elementName) const;
    const bsl::vector<float>& theFloatArray(const char *elementName) const;
    const bsl::vector<double>& theDoubleArray(const char *elementName) const;
    const bsl::vector<bsl::string>& theStringArray(
                                                const char *elementName) const;
    const bsl::vector<bdet_Datetime>& theDatetimeArray(
                                                const char *elementName) const;
    const bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray(
                                                const char *elementName) const;
    const bsl::vector<bdet_Date>& theDateArray(const char *elementName) const;
    const bsl::vector<bdet_DateTz>& theDateTzArray(
                                                const char *elementName) const;
    const bsl::vector<bdet_Time>& theTimeArray(const char *elementName) const;
    const bsl::vector<bdet_TimeTz>& theTimeTzArray(
                                                const char *elementName) const;
    const bdem_Choice& theChoice(const char *elementName) const;
    const bdem_ChoiceArray& theChoiceArray(const char *elementName) const;
    const bdem_List& theList(const char *elementName) const;
    const bdem_Table& theTable(const char *elementName) const;
        // Return a reference to the non-modifiable element having the
        // specified 'elementName' in this binding.  The nullness of the
        // element is not affected.  The behavior is undefined unless this
        // binding has a field named 'elementName' and the type of the
        // referenced element matches the return type of the method used.

    bdem_ConstChoiceBinding choiceBinding(const char *elementName) const;
        // Return a "const" choice binding that binds the non-modifiable
        // choice in the list having the specified 'elementName' in this
        // binding to the non-modifiable constraint record in the field named
        // 'elementName' in this binding.  The behavior is undefined unless
        // this binding has a field named 'elementName' and the field
        // constrains a 'bdem_Choice'.

    bdem_ConstChoiceBinding choiceBinding(int index) const;
        // Return a "const" choice binding that binds the non-modifiable
        // choice in the list at the specified 'index' in this binding to the
        // non-modifiable constraint record in the field at 'index' in this
        // binding.  The behavior is undefined unless 0 <= index < length()
        // and the indexed field constrains a 'bdem_Choice'.

    bdem_ConstChoiceArrayBinding choiceArrayBinding(
                                               const char *elementName) const;
        // Return a "const" choice array binding that binds the non-modifiable
        // choice array in the list having the specified 'elementName' in this
        // binding to the non-modifiable constraint record in the field named
        // 'elementName' in this binding.  The behavior is undefined unless
        // this binding has a field named 'elementName' and the field
        // constrains a 'bdem_ChoiceArray'.

    bdem_ConstChoiceArrayBinding choiceArrayBinding(int index) const;
        // Return a "const" choice array binding that binds the non-modifiable
        // choice array in the list at the specified 'index' in this binding
        // to the non-modifiable constraint record in the field at 'index' in
        // this binding.  The behavior is undefined unless
        // '0 <= index < length()' and the indexed field constrains a
        // 'bdem_ChoiceArray'.

    bdem_ConstRowBinding rowBinding(const char *elementName) const;
        // Return a "const" row binding that binds the non-modifiable row in
        // the list having the specified 'elementName' in this binding to the
        // non-modifiable constraint record in the field named 'elementName' in
        // this binding.  The behavior is undefined unless this binding has a
        // field named 'elementName' and the field constrains a 'bdem_List'.

    bdem_ConstRowBinding rowBinding(int index) const;
        // Return a "const" row binding that binds the non-modifiable row in
        // the list at the specified 'index' in this binding to the
        // non-modifiable constraint record in the field at 'index' in this
        // binding.  The behavior is undefined unless '0 <= index < length()'
        // and the indexed field constrains a 'bdem_List'.

    bdem_ConstTableBinding tableBinding(const char *elementName) const;
        // Return a "const" table binding that binds the non-modifiable table
        // having the specified 'elementName' in this binding to the
        // non-modifiable constraint record in the field named 'elementName' in
        // this binding.  The behavior is undefined unless this binding has a
        // field named 'elementName' and the field constrains a 'bdem_Table'.

    bdem_ConstTableBinding tableBinding(int index) const;
        // Return a "const" table binding that binds the non-modifiable table
        // at the specified 'index' in this binding to the non-modifiable
        // constraint record in the field at 'index' in this binding.  The
        // behavior is undefined unless '0 <= index < length()' and the indexed
        // field constrains a 'bdem_Table'.

    const char *enumerationAsString(const char *elementName) const;
        // Return the string representation of the enumeration element having
        // the specified 'elementName' in this binding.  Return 0 if the
        // element has a null or unset value.  The returned pointer is valid
        // until the bound schema is destroyed.  The behavior is undefined
        // unless this binding has a field named 'elementName', the type of the
        // corresponding element is 'INT' or 'STRING', and the element is
        // constrained by an enumeration definition.

    const char *enumerationAsString(int index) const;
        // Return the string representation of the enumeration element at the
        // specified 'index' in this binding.  Return 0 if the element
        // has a null or unset value.  The returned pointer is valid until the
        // bound schema is destroyed.  The behavior is undefined unless
        // '0 <= index < length()', the type of the indexed element is
        // INT' or 'STRING', and the element is constrained by an enumeration
        // definition.

    int enumerationAsInt(const char *elementName) const;
        // Return the integer ID of the enumeration element having the
        // specified 'elementName' in this binding.  Return
        // 'bdetu_Unset<int>::unsetValue()' if the element has an unset
        // value.  The behavior is undefined unless this binding has a field
        // named 'elementName', the type of the corresponding element is 'INT'
        // or 'STRING', and the element is constrained by an enumeration
        // definition.

    int enumerationAsInt(int index) const;
        // Return the integer ID of the enumeration element at the specified
        // 'index' in this binding.  Return 'bdetu_Unset<int>::unsetValue()'
        // if the element has an unset value.  The behavior is undefined unless
        // '0 <= index < length()', the type of the indexed element is 'INT'
        // or 'STRING', and the element is constrained by an enumeration
        // definition.
};

// FREE OPERATORS
inline
bool operator==(const bdem_ConstRowBinding& lhs,
                const bdem_ConstRowBinding& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' bindings reference rows
    // having the same value and hold records that are equivalent as deemed by
    // 'bdem_SchemaUtil::areEquivalent', and 'false' otherwise.

inline
bool operator!=(const bdem_ConstRowBinding& lhs,
                const bdem_ConstRowBinding& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' bindings reference rows
    // having different values or hold records that are not equivalent as
    // deemed by 'bdem_SchemaUtil::areEquivalent', and 'false' otherwise.

                        // =====================
                        // class bdem_RowBinding
                        // =====================

class bdem_RowBinding : public bdem_ConstRowBinding {
    // This class binds a modifiable 'bdem_Row' ("row") to a non-modifiable
    // 'bdem_RecordDef' ("record"), and provides both by-name and indexed
    // access to the elements of the row.  Indexed access is an efficient
    // constant-time operation; by-name access is somewhat less efficient, but
    // no less so than by-name access using the record 'lookup' methods.  For
    // correct by-name access to elements in the bound row, the row data must
    // satisfy the record meta-data.
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // although copy construction and equality are supported.  Except for the
    // constructors, destructor, and overloaded 'rebind' methods, all instance
    // methods of this object are 'const' and apply not to this object, but
    // rather to the bound row or the bound meta-data.  This class adds to the
    // functionality of its base class by enabling modification of the bound
    // row, but adds *no* additional data.

  public:
    // TYPES
    typedef bdem_ElemRef            ElemRef;
    typedef bdem_RowBinding         RowBinding;
    typedef bdem_TableBinding       TableBinding;
    typedef bdem_ChoiceBinding      ChoiceBinding;
    typedef bdem_ChoiceArrayBinding ChoiceArrayBinding;
    typedef bdem_Row                Row;
    typedef bdem_List               List;

  private:
    // Note: all data resides in the base class.

    // NOT IMPLEMENTED
    bdem_RowBinding& operator=(const bdem_RowBinding&);

  public:
    // CREATORS
    bdem_RowBinding(bdem_Row *row, const bdem_RecordDef *record);
        // Create a row binding that binds the specified modifiable 'row' to
        // the specified non-modifiable 'record'.  The behavior is undefined
        // unless the 'row' data can satisfy the 'record' meta-data.

    bdem_RowBinding(bdem_Row          *row,
                    const bdem_Schema *schema,
                    const char        *recordName);
        // Create a row binding that binds the specified modifiable 'row' to
        // the non-modifiable record definition having the specified
        // 'recordName' in the specified 'schema'.  The behavior is undefined
        // unless the 'row' data can satisfy the record meta-data named
        // 'recordName' in 'schema'.

    bdem_RowBinding(bdem_Row          *row,
                    const bdem_Schema *schema,
                    int                recordIndex);
        // Create a row binding that binds the specified modifiable 'row' to
        // the non-modifiable record definition at the specified 'recordIndex'
        // in the specified 'schema'.  The behavior is undefined unless
        // '0 <= recordIndex < schema->length()' and the 'row' data can satisfy
        // the record meta-data at 'recordIndex' in 'schema'.

    bdem_RowBinding(bdem_List *list, const bdem_RecordDef *record);
        // Create a row binding that binds the modifiable row in the specified
        // 'list' to the specified non-modifiable 'record'.  The behavior is
        // undefined unless the row data in 'list' can satisfy the 'record'
        // meta-data.

    bdem_RowBinding(bdem_List         *list,
                    const bdem_Schema *schema,
                    const char        *recordName);
        // Create a row binding that binds the modifiable row in the specified
        // 'list' to the non-modifiable record definition having the specified
        // 'recordName' in the specified 'schema'.  The behavior is undefined
        // unless the row data in 'list' can satisfy the record meta-data named
        // 'recordName' in 'schema'.

    bdem_RowBinding(bdem_List         *list,
                    const bdem_Schema *schema,
                    int                recordIndex);
        // Create a row binding that binds the modifiable row in the specified
        // 'list' to the non-modifiable record definition at the specified
        // 'recordIndex' in the specified 'schema'.  The behavior is undefined
        // unless '0 <= recordIndex < schema->length()' and the row data in
        // 'list' can satisfy the record meta-data at 'recordIndex' in
        // 'schema'.

    bdem_RowBinding(bdem_Table           *table,
                    int                   rowIndex,
                    const bdem_RecordDef *record);
        // Create a row binding that binds the modifiable row at the specified
        // 'rowIndex' in the specified 'table' to the specified non-modifiable
        // 'record'.  The behavior is undefined unless
        // '0 <= rowIndex < table->numRows()' and the row data at 'rowIndex' in
        // 'table' can satisfy the 'record' meta-data.

    bdem_RowBinding(bdem_Table        *table,
                    int                rowIndex,
                    const bdem_Schema *schema,
                    const char        *recordName);
        // Create a row binding that binds the modifiable row at the specified
        // 'rowIndex' in the specified 'table' to the non-modifiable record
        // definition having the specified 'recordName' in the specified
        // 'schema'.  The behavior is undefined unless
        // '0 <= rowIndex < table->numRows()' and the row data at 'rowIndex' in
        // 'table' can satisfy the record meta-data named 'recordName' in
        // 'schema'.

     bdem_RowBinding(bdem_Table        *table,
                     int                rowIndex,
                     const bdem_Schema *schema,
                     int                recordIndex);
        // Create a row binding that binds the modifiable row at the specified
        // 'rowIndex' in the specified 'table' to the non-modifiable record
        // definition at the specified 'recordIndex' in the specified 'schema'.
        // The behavior is undefined unless '0 <= rowIndex < table->numRows()',
        // '0 <= recordIndex < schema->length()', and the row data at
        // 'rowIndex' in 'table' can satisfy the record meta-data at
        // 'recordIndex' in 'schema'.

    bdem_RowBinding(const bdem_RowBinding& original);
        // Create a row binding that binds a modifiable row to a non-modifiable
        // record, both acquired from the specified 'original' row binding.

    // ~bdem_RowBinding();
        // Destroy this row binding.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    void rebind(const bdem_RowBinding& binding);
        // Reinitialize this row binding to bind a newly-acquired modifiable
        // row to a newly-acquired non-modifiable record, both acquired from
        // the specified row 'binding'.

    void rebind(bdem_Row *row);
        // Reinitialize this row binding to bind the specified modifiable 'row'
        // to the non-modifiable record already held by this binding.  The
        // behavior is undefined unless the 'row' data can satisfy the record
        // meta-data.

    void rebind(bdem_List *list);
        // Reinitialize this row binding to bind the modifiable row in the
        // specified 'list' to the non-modifiable record already held by this
        // binding.  The behavior is undefined unless the row data in 'list'
        // can satisfy the record meta-data.

    void rebind(bdem_Table *table, int rowIndex);
        // Reinitialize this row binding to bind the modifiable row at the
        // specified 'rowIndex' in the specified 'table' to the non-modifiable
        // record already held by this binding.  The behavior is undefined
        // unless '0 <= rowIndex < table->numRows()' and the row data at
        // 'rowIndex' in 'table' can satisfy the record meta-data.

    void rebind(bdem_Row *row, const bdem_RecordDef *record);
        // Reinitialize this row binding to bind the specified modifiable 'row'
        // to the specified non-modifiable 'record'.  The behavior is undefined
        // unless the 'row' data can satisfy the 'record' meta-data.

    void rebind(bdem_Row          *row,
                const bdem_Schema *schema,
                const char        *recordName);
        // Reinitialize this row binding to bind the specified modifiable 'row'
        // to the non-modifiable record definition having the specified
        // 'recordName' in the specified 'schema'.  The behavior is undefined
        // unless the 'row' data can satisfy the record meta-data named
        // 'recordName' in 'schema'.

    void rebind(bdem_Row          *row,
                const bdem_Schema *schema,
                int                recordIndex);
        // Reinitialize this row binding to bind the specified modifiable 'row'
        // to the non-modifiable record definition at the specified
        // 'recordIndex' in the specified 'schema'.  The behavior is undefined
        // unless '0 <= recordIndex < schema->length()' and the 'row' data can
        // satisfy the record meta-data at 'recordIndex' in 'schema'.

    void rebind(bdem_List            *list,
                const bdem_RecordDef *record);
        // Reinitialize this row binding to bind the modifiable row in the
        // specified 'list' to the specified non-modifiable 'record'.  The
        // behavior is undefined unless the row data in 'list' can satisfy
        // the 'record' meta-data.

    void rebind(bdem_List         *list,
                const bdem_Schema *schema,
                const char        *recordName);
        // Reinitialize this row binding to bind the modifiable row in the
        // specified 'list' to the non-modifiable record definition having the
        // specified 'recordName' in the specified 'schema'.  The behavior is
        // undefined unless the row data in 'list' can satisfy the record
        // meta-data named 'recordName' in 'schema'.

    void rebind(bdem_List         *list,
                const bdem_Schema *schema,
                int                recordIndex);
        // Reinitialize this row binding to bind the modifiable row in the
        // specified 'list' to the non-modifiable record definition at the
        // specified 'recordIndex' in the specified 'schema'.  The behavior is
        // undefined unless '0 <= recordIndex < schema->length()' and the row
        // data in 'list' can satisfy the record meta-data at 'recordIndex' in
        // 'schema'.

    void rebind(bdem_Table           *table,
                int                   rowIndex,
                const bdem_RecordDef *record);
        // Reinitialize this row binding to bind the modifiable row at the
        // specified 'rowIndex' in the specified 'table' to the specified
        // non-modifiable 'record'.  The behavior is undefined unless
        // '0 <= rowIndex < table->numRows()' and the row data at 'rowIndex' in
        // 'table' can satisfy the 'record' meta-data.

    void rebind(bdem_Table        *table,
                int                rowIndex,
                const bdem_Schema *schema,
                const char        *recordName);
        // Reinitialize this row binding to bind the modifiable row at the
        // specified 'rowIndex' in the specified 'table' to the non-modifiable
        // record definition having the specified 'recordName' in the specified
        // 'schema'.  The behavior is undefined unless
        // '0 <= rowIndex < table->numRows()' and the row data at 'rowIndex' in
        // 'table' can satisfy the record meta-data named 'recordName' in
        // 'schema'.

    void rebind(bdem_Table        *table,
                int                rowIndex,
                const bdem_Schema *schema,
                int                recordIndex);
        // Reinitialize this row binding to bind the modifiable row at the
        // specified 'rowIndex' in the specified 'table' to the non-modifiable
        // record definition at the specified 'recordIndex' in the specified
        // 'schema'.  The behavior is undefined unless
        // '0 <= rowIndex < table->numRows()',
        // '0 <= recordIndex < schema->length()', and the row data at
        // 'rowIndex' in 'table' can satisfy the record meta-data at
        // 'recordIndex' in 'schema'.

    int setEnumeration(const char         *elementName,
                       const bsl::string&  enumeratorName) const;
        // Set the value of the enumeration element having the specified
        // 'elementName' in this binding.  If the element is of type
        // 'bdem_ElemType::BDEM_STRING', store the specified 'enumeratorName'
        // in the element.  If the element is of type
        // 'bdem_ElemType::BDEM_INT', look up 'enumeratorName' in the element's
        // enumeration definition and store the corresponding integer
        // enumerator ID in the element.  Return 0 on success, and a non-zero
        // value otherwise.  The behavior is undefined unless this binding has
        // a field named 'elementName', and the type of the referenced element
        // is 'INT' or 'STRING'.

    int setEnumeration(int                 columnIndex,
                       const bsl::string&  enumeratorName) const;
        // Set the value of the enumeration element at the specified
        // 'columnIndex' in this binding.  If the element is of type
        // 'bdem_ElemType::BDEM_STRING', store the specified 'enumeratorName'
        // in the element.  If the element is of type
        // 'bdem_ElemType::BDEM_INT', look up 'enumeratorName' in the element's
        // enumeration definition and store the corresponding integer
        // enumerator ID in the element.  Return 0 on success, and a non-zero
        // value otherwise.  The behavior is undefined unless
        // '0 <= columnIndex < length()', and the type of the referenced
        // element is 'INT' or 'STRING'.

    int setEnumeration(const char *elementName, int enumeratorId) const;
        // Set the value of the enumeration element having the specified
        // 'elementName' in this binding.  If the element is of type
        // 'bdem_ElemType::BDEM_INT', store the specified 'enumeratorId' in the
        // element.  If the element is of type 'bdem_ElemType::BDEM_STRING,
        // look up 'enumeratorId' in the element's enumeration definition and
        // store the corresponding enumerator name in the element.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless this binding has a field named 'elementName', and the type of
        // the referenced element is 'INT' or 'STRING'.

    int setEnumeration(int columnIndex, int enumeratorId) const;
        // Set the value of the enumeration element at the specified
        // 'columnIndex' in this binding.  If the element is of type
        // 'bdem_ElemType::BDEM_INT', store the specified 'enumeratorId' in the
        // element.  If the element is of type 'bdem_ElemType::BDEM_STRING,
        // look up 'enumeratorId' in the element's enumeration definition and
        // store the corresponding enumerator name in the element.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless '0 <= columnIndex < length()', and the type of the referenced
        // element is 'INT' or 'STRING'.

    // ACCESSORS
    bdem_ElemRef operator[](int index) const;
        // Return an element reference that refers to the modifiable element at
        // the specified 'index' in this binding.  The behavior is undefined
        // unless '0 <= index < row().length()'.

    bdem_ElemRef element(const char *elementName) const;
        // Return an element reference that refers to the modifiable element
        // having the specified 'elementName' in this binding.  The behavior is
        // undefined unless this binding has a field named 'elementName'.

    bdem_ElemRef element(int index) const;
        // Return an element reference that refers to the modifiable element at
        // the specified 'index' in this binding.  The behavior is undefined
        // unless '0 <= index < row().length()'.

    bdem_Row& row() const;
        // Return a reference to the modifiable row held by this binding.

    bool& theModifiableBool(const char *elementName) const;
    char& theModifiableChar(const char *elementName) const;
    short& theModifiableShort(const char *elementName) const;
    int& theModifiableInt(const char *elementName) const;
    bsls_Types::Int64& theModifiableInt64(const char *elementName) const;
    float& theModifiableFloat(const char *elementName) const;
    double& theModifiableDouble(const char *elementName) const;
    bsl::string& theModifiableString(const char *elementName) const;
    bdet_Datetime& theModifiableDatetime(const char *elementName) const;
    bdet_DatetimeTz& theModifiableDatetimeTz(const char *elementName) const;
    bdet_Date& theModifiableDate(const char *elementName) const;
    bdet_DateTz& theModifiableDateTz(const char *elementName) const;
    bdet_Time& theModifiableTime(const char *elementName) const;
    bdet_TimeTz& theModifiableTimeTz(const char *elementName) const;
    bsl::vector<bool>& theModifiableBoolArray(const char *elementName) const;
    bsl::vector<char>& theModifiableCharArray(const char *elementName) const;
    bsl::vector<short>& theModifiableShortArray(const char *elementName) const;
    bsl::vector<int>& theModifiableIntArray(const char *elementName) const;
    bsl::vector<bsls_Types::Int64>& theModifiableInt64Array(
                                                const char *elementName) const;
    bsl::vector<float>& theModifiableFloatArray(const char *elementName) const;
    bsl::vector<double>& theModifiableDoubleArray(
                                                const char *elementName) const;
    bsl::vector<bsl::string>& theModifiableStringArray(
                                                const char *elementName) const;
    bsl::vector<bdet_Datetime>& theModifiableDatetimeArray(
                                                const char *elementName) const;
    bsl::vector<bdet_DatetimeTz>& theModifiableDatetimeTzArray(
                                                const char *elementName) const;
    bsl::vector<bdet_Date>& theModifiableDateArray(
                                                const char *elementName) const;
    bsl::vector<bdet_DateTz>& theModifiableDateTzArray(
                                                const char *elementName) const;
    bsl::vector<bdet_Time>& theModifiableTimeArray(
                                                const char *elementName) const;
    bsl::vector<bdet_TimeTz>& theModifiableTimeTzArray(
                                                const char *elementName) const;
    bdem_Choice& theModifiableChoice(const char *elementName) const;
    bdem_ChoiceArray& theModifiableChoiceArray(const char *elementName) const;
    bdem_List& theModifiableList(const char *elementName) const;
    bdem_Table& theModifiableTable(const char *elementName) const;
        // Return a reference to the modifiable element having the specified
        // 'elementName' in this binding.  If the element is null, it is made
        // non-null before returning, but its value is not otherwise modified.
        // The behavior is undefined unless this binding has a field named
        // 'elementName' and the type of the referenced element matches the
        // return type of the method used.  Note that unless there is an
        // intention of modifying the element, the corresponding 'theTYPE'
        // method should be used instead.

    bdem_ChoiceBinding choiceBinding(const char *elementName) const;
        // Return a choice binding that binds the modifiable choice in the
        // list having the specified 'elementName' in this binding to the
        // non-modifiable constraint record in the field named 'elementName'
        // in this binding.  The behavior is undefined unless this binding has
        // a field named 'elementName' and the field constrains a
        // 'bdem_Choice'.

    bdem_ChoiceBinding choiceBinding(int index) const;
        // Return a choice binding that binds the modifiable choice in the
        // list at the specified 'index' in this binding to the non-modifiable
        // constraint record in the field at 'index' in this binding.  The
        // behavior is undefined unless '0 <= index < length()' and the indexed
        // field constrains a 'bdem_Choice'.

    bdem_ChoiceArrayBinding choiceArrayBinding(const char *elementName) const;
        // Return a choice array binding that binds the modifiable choice
        // array in the list having the specified 'elementName' in this
        // binding to the non-modifiable constraint record in the field named
        // 'elementName' in this binding.  The behavior is undefined unless
        // this binding has a field named 'elementName' and the field
        // constrains a 'bdem_ChoiceArray'.

    bdem_ChoiceArrayBinding choiceArrayBinding(int index) const;
        // Return a choice array binding that binds the modifiable choice
        // array in the list at the specified 'index' in this binding to the
        // non-modifiable constraint record in the field at 'index' in this
        // binding.  The behavior is undefined unless '0 <= index < length()'
        // and the indexed field constrains a 'bdem_ChoiceArray'.

    bdem_RowBinding rowBinding(const char *elementName) const;
        // Return a row binding that binds the modifiable row in the list
        // having the specified 'elementName' in this binding to the
        // non-modifiable constraint record in the field named 'elementName' in
        // this binding.  The behavior is undefined unless this binding has a
        // field named 'elementName' and the field constrains a 'bdem_List'.

    bdem_RowBinding rowBinding(int index) const;
        // Return a row binding that binds the modifiable row in the list at
        // the specified 'index' in this binding to the non-modifiable
        // constraint record in the field at 'index' in this binding.  The
        // behavior is undefined unless '0 <= index < length()' and the indexed
        // field constrains a 'bdem_List'.

    bdem_TableBinding tableBinding(const char *elementName) const;
        // Return a table binding that binds the modifiable table having the
        // specified 'elementName' in this binding to the non-modifiable
        // constraint record in the field named 'elementName' in this binding.
        // The behavior is undefined unless this binding has a field named
        // 'elementName' and the field constrains a 'bdem_Table'.

    bdem_TableBinding tableBinding(int index) const;
        // Return a table binding that binds the modifiable table at the
        // specified 'index' in this binding to the non-modifiable constraint
        // record in the field at 'index' in this binding.  The behavior is
        // undefined unless '0 <= index < length()' and the indexed field
        // constrains a 'bdem_Table'.
};

class bdem_ConstColumnBinding;
class bdem_ColumnBinding;

                        // ============================
                        // class bdem_ConstTableBinding
                        // ============================

class bdem_ConstTableBinding {
    // This class binds a non-modifiable 'bdem_Table' ("table") to a
    // non-modifiable 'bdem_RecordDef' ("record"), and provides both by-name
    // and indexed access to the elements of the table.  Indexed access is an
    // efficient constant-time operation; by-name access is somewhat less
    // efficient, but no less so than by-name access using the record 'lookup'
    // methods.  For correct by-name access to elements in the bound table, the
    // table data must satisfy the record meta-data.
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // although copy construction and equality are supported.  Except for the
    // constructors, destructor, and the overloaded 'rebind' methods, all
    // instance methods of this object are 'const' and apply not to this
    // object, but rather to the bound table or the bound meta-data.  Note also
    // that, unlike the derived 'bdem_TableBinding' class (also defined within
    // this component), none of the methods within this base class enable
    // modifiable access to the bound table, thus ensuring 'const' correctness.

  public:
    // TYPES
    typedef bdem_ConstElemRef       ElemRef;
    typedef bdem_ConstRowBinding    RowBinding;
    typedef bdem_ConstColumnBinding ColumnBinding;
    typedef const bdem_Table        Table;
    typedef const bdem_Row          Row;

  private:
    // DATA
    const bdem_Table     *d_table_p;       // data (held, not owned)
    const bdem_RecordDef *d_constraint_p;  // meta-data (held, not owned)

    // FRIENDS
    friend bool operator==(const bdem_ConstTableBinding& lhs,
                           const bdem_ConstTableBinding& rhs);

    friend class bdem_TableBinding;
        // Allow the derived class to access the private members of this class.

  private:
    // NOT IMPLEMENTED
    bdem_ConstTableBinding& operator=(const bdem_ConstTableBinding&);

  public:
    // CREATORS
    bdem_ConstTableBinding(const bdem_Table     *table,
                           const bdem_RecordDef *record);
        // Create a "const" table binding that binds the specified
        // non-modifiable 'table' to the specified non-modifiable 'record'.
        // The behavior is undefined unless the 'table' data can satisfy the
        // 'record' meta-data.

    bdem_ConstTableBinding(const bdem_Table  *table,
                           const bdem_Schema *schema,
                           const char        *recordName);
        // Create a "const" table binding that binds the specified
        // non-modifiable 'table' to the non-modifiable record definition
        // having the specified 'recordName' in the specified 'schema'.  The
        // behavior is undefined unless the 'table' data can satisfy the record
        // meta-data named 'recordName' in 'schema'.

    bdem_ConstTableBinding(const bdem_Table  *table,
                           const bdem_Schema *schema,
                           int                recordIndex);
        // Create a "const" table binding that binds the specified
        // non-modifiable 'table' to the non-modifiable record definition at
        // the specified 'recordIndex' in the specified 'schema'.  The behavior
        // is undefined unless '0 <= recordIndex < schema->length()' and the
        // 'table' data can satisfy the record meta-data at 'recordIndex' in
        // 'schema'.

    bdem_ConstTableBinding(const bdem_ConstTableBinding& original);
        // Create a "const" table binding that binds a non-modifiable table to
        // a non-modifiable record, both acquired from the specified 'original'
        // table binding.

    // ~bdem_ConstTableBinding();
        // Destroy this 'const' table binding.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    void rebind(const bdem_ConstTableBinding& binding);
        // Reinitialize this table binding to bind a newly-acquired
        // non-modifiable table to a newly-acquired non-modifiable record, both
        // acquired from the specified table 'binding'.

    void rebind(const bdem_Table *table);
        // Reinitialize this table binding to bind the specified non-modifiable
        // 'table' to the non-modifiable record already held by this binding.
        // The behavior is undefined unless the 'table' data can satisfy the
        // record meta-data.

    void rebind(const bdem_Table *table, const bdem_RecordDef *record);
        // Reinitialize this table binding to bind the specified non-modifiable
        // 'table' to the specified non-modifiable 'record'.  The behavior is
        // undefined unless the 'table' data can satisfy the 'record'
        // meta-data.

    void rebind(const bdem_Table  *table,
                const bdem_Schema *schema,
                const char        *recordName);
        // Reinitialize this table binding to bind the specified non-modifiable
        // 'table' to the non-modifiable record definition having the specified
        // 'recordName' in the specified 'schema'.  The behavior is undefined
        // unless the 'table' data can satisfy the record meta-data named
        // 'recordName' in 'schema'.

    void rebind(const bdem_Table  *table,
                const bdem_Schema *schema,
                int                recordIndex);
        // Reinitialize this table binding to bind the specified non-modifiable
        // 'table' to the non-modifiable record definition at the specified
        // 'recordIndex' in the specified 'schema'.  The behavior is undefined
        // unless '0 <= recordIndex < schema->length()' and the 'table' data
        // can satisfy the record meta-data at 'recordIndex' in 'schema'.

    // ACCESSORS
    const bdem_Row& operator[](int rowIndex) const;
        // Return a reference to the non-modifiable row at the specified
        // 'rowIndex' in this binding.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()'.

    bdem_ConstElemRef element(int rowIndex, const char *columnName) const;
        // Return a "const" element reference that refers to the non-modifiable
        // element at the specified 'rowIndex' in the column having the
        // specified 'columnName' in this binding.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()', and this binding has a field
        // named 'columnName'.

    bdem_ConstElemRef element(int rowIndex, int columnIndex) const;
        // Return a "const" element reference that refers to the non-modifiable
        // element at the specified 'rowIndex' and 'columnIndex' in this
        // binding.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()', and
        // '0 <= columnIndex < table().numColumns()'.

    bdem_ElemType::Type elemType(const char *columnName) const;
        // Return the 'bdem_ElemType::Type' value of the elements in the column
        // having the specified 'columnName' in this binding.  The behavior is
        // undefined unless this binding has a field named 'columnName'.

    bdem_ElemType::Type elemType(int columnIndex) const;
        // Return the 'bdem_ElemType::Type' value of the elements at the
        // specified 'columnIndex' in this binding.  The behavior is undefined
        // unless '0 <= columnIndex < table().numColumns()'.

    int numColumns() const;
        // Return the number of fields in the record held by this binding.

    int numRows() const;
        // Return the number of rows in the table held by this binding.

    const bdem_RecordDef& record() const;
        // Return a reference to the non-modifiable record held by this
        // binding.

    const bdem_Table& table() const;
        // Return a reference to the non-modifiable table held by this binding.

    const bool& theBool(int rowIndex, const char *columnName) const;
    const char& theChar(int rowIndex, const char *columnName) const;
    const short& theShort(int rowIndex, const char *columnName) const;
    const int& theInt(int rowIndex, const char *columnName) const;
    const bsls_Types::Int64& theInt64(int         rowIndex,
                                      const char *columnName) const;
    const float& theFloat(int rowIndex, const char *columnName) const;
    const double& theDouble(int rowIndex, const char *columnName) const;
    const bsl::string& theString(int rowIndex, const char *columnName) const;
    const bdet_Datetime& theDatetime(int         rowIndex,
                                     const char *columnName) const;
    const bdet_DatetimeTz& theDatetimeTz(int         rowIndex,
                                         const char *columnName) const;
    const bdet_Date& theDate(int rowIndex, const char *columnName) const;
    const bdet_DateTz& theDateTz(int rowIndex, const char *columnName) const;
    const bdet_Time& theTime(int rowIndex, const char *columnName) const;
    const bdet_TimeTz& theTimeTz(int rowIndex, const char *columnName) const;
    const bsl::vector<bool>& theBoolArray(int         rowIndex,
                                          const char *columnName) const;
    const bsl::vector<char>& theCharArray(int         rowIndex,
                                          const char *columnName) const;
    const bsl::vector<short>& theShortArray(int         rowIndex,
                                            const char *columnName) const;
    const bsl::vector<int>& theIntArray(int         rowIndex,
                                        const char *columnName) const;
    const bsl::vector<bsls_Types::Int64>& theInt64Array(
                                                int         rowIndex,
                                                const char *columnName) const;
    const bsl::vector<float>& theFloatArray(int         rowIndex,
                                            const char *columnName) const;
    const bsl::vector<double>& theDoubleArray(int         rowIndex,
                                              const char *columnName) const;
    const bsl::vector<bsl::string>& theStringArray(
                                                int         rowIndex,
                                                const char *columnName) const;
    const bsl::vector<bdet_Datetime>& theDatetimeArray(
                                                int         rowIndex,
                                                const char *columnName) const;
    const bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray(
                                                int         rowIndex,
                                                const char *columnName) const;
    const bsl::vector<bdet_Date>& theDateArray(int         rowIndex,
                                               const char *columnName) const;
    const bsl::vector<bdet_DateTz>& theDateTzArray(int         rowIndex,
                                               const char *columnName) const;
    const bsl::vector<bdet_Time>& theTimeArray(int         rowIndex,
                                               const char *columnName) const;
    const bsl::vector<bdet_TimeTz>& theTimeTzArray(
                                               int         rowIndex,
                                               const char *columnName) const;
    const bdem_Choice& theChoice(int rowIndex, const char *columnName) const;
    const bdem_ChoiceArray& theChoiceArray(int         rowIndex,
                                           const char *columnName) const;
    const bdem_List& theList(int rowIndex, const char *columnName) const;
    const bdem_Table& theTable(int rowIndex, const char *columnName) const;
        // Return a reference to the non-modifiable element at the specified
        // 'rowIndex' in the column having the specified 'columnName' in this
        // binding.  The nullness of the element is not affected.  The behavior
        // is undefined unless '0 <= rowIndex < numRows()', this binding has a
        // field named 'columnName', and the type of the referenced element
        // matches the return type of the method used.

    bdem_ConstChoiceBinding choiceBinding(int         rowIndex,
                                          const char *columnName) const;
        // Return a "const" choice binding that binds the non-modifiable
        // choice in the list at the specified 'rowIndex' in the column having
        // the specified 'columnName' in this binding to the non-modifiable
        // constraint record in the field named 'columnName' in this binding.
        // The behavior is undefined unless '0 <= rowIndex < numRows()', this
        // binding has a field named 'columnName', and the field constrains a
        // 'bdem_Choice'.

    bdem_ConstChoiceBinding choiceBinding(int rowIndex, int columnIndex) const;
        // Return a "const" choice binding that binds the non-modifiable
        // choice in the list at the specified 'rowIndex' and 'columnIndex' in
        // this binding to the non-modifiable constraint record at
        // 'columnIndex' in this binding.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()', '0 <= columnIndex < numColumns()', and
        // the indexed field constrains a 'bdem_Choice'.

    bdem_ConstChoiceArrayBinding choiceArrayBinding(
                                                 int         rowIndex,
                                                 const char *columnName) const;
        // Return a "const" choice array binding that binds the non-modifiable
        // choice array in the list at the specified 'rowIndex' in the column
        // having the specified 'columnName' in this binding to the
        // non-modifiable constraint record in the field named 'columnName' in
        // this binding.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()', this binding has a field named
        // 'columnName', and the field constrains a 'bdem_ChoiceArray'.

    bdem_ConstChoiceArrayBinding choiceArrayBinding(int rowIndex,
                                                    int columnIndex) const;
        // Return a "const" choice array binding that binds the non-modifiable
        // choice array in the list at the specified 'rowIndex' and
        // 'columnIndex' in this binding to the non-modifiable constraint
        // record at 'columnIndex' in this binding.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()',
        // '0 <= columnIndex < numColumns()', and the indexed field constrains
        // a 'bdem_ChoiceArray'.

    bdem_ConstRowBinding rowBinding(int rowIndex) const;
        // Return a "const" row binding that binds the non-modifiable row at
        // the specified 'rowIndex' in this binding to the non-modifiable
        // record bound by this binding.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()'.

    bdem_ConstRowBinding rowBinding(int         rowIndex,
                                    const char *columnName) const;
        // Return a "const" row binding that binds the non-modifiable row in
        // the list at the specified 'rowIndex' in the column having the
        // specified 'columnName' in this binding to the non-modifiable
        // constraint record in the field named 'columnName' in this binding.
        // The behavior is undefined unless '0 <= rowIndex < numRows()', this
        // binding has a field named 'columnName', and the field constrains a
        // 'bdem_List'.

    bdem_ConstRowBinding rowBinding(int rowIndex, int columnIndex) const;
        // Return a "const" row binding that binds the non-modifiable row in
        // the list at the specified 'rowIndex' and 'columnIndex' in this
        // binding to the non-modifiable constraint record at 'columnIndex' in
        // this binding.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()', '0 <= columnIndex < numColumns()', and
        // the indexed field constrains a 'bdem_List'.

    bdem_ConstTableBinding tableBinding(int         rowIndex,
                                        const char *columnName) const;
        // Return a "const" table binding that binds the non-modifiable table
        // at the specified 'rowIndex' in the column having the specified
        // 'columnName' in this binding to the non-modifiable constraint record
        // in the field named 'columnName' in this binding.  The behavior is
        // undefined unless '0 <= rowIndex < numRows()', this binding has a
        // field named 'columnName', and the field constrains a 'bdem_Table'.

    bdem_ConstTableBinding tableBinding(int rowIndex, int columnIndex) const;
        // Return a "const" table binding that binds the non-modifiable table
        // at the specified 'rowIndex' and 'columnIndex' in this binding to the
        // non-modifiable constraint record at 'columnIndex' in this binding.
        // The behavior is undefined unless '0 <= rowIndex < numRows()',
        // '0 <= columnIndex < numColumns()', and the indexed field constrains
        // a 'bdem_Table'.

    bdem_ConstColumnBinding columnBinding(const char *columnName) const;
        // Return a "const" column binding that binds the non-modifiable column
        // having the specified 'columnName' in this binding to the
        // non-modifiable field named 'columnName' in this binding.  The
        // behavior is undefined unless this binding has a field named
        // 'columnName'.

    bdem_ConstColumnBinding columnBinding(int columnIndex) const;
        // Return a "const" column binding that binds the non-modifiable column
        // at the specified 'columnIndex' in this binding to the non-modifiable
        // field at 'columnIndex' in this binding.  The behavior is undefined
        // unless '0 <= columnIndex < numColumns()'.

    const char *enumerationAsString(int         rowIndex,
                                    const char *elementName) const;
        // Return the string representation of the enumeration element having
        // the specified 'rowIndex' and 'elementName' in this binding.  Return
        // 0 if the element has a null or unset value.  The returned pointer is
        // valid until the bound schema is destroyed.  The behavior is
        // undefined unless '0 <= rowIndex < numRows()', this binding has a
        // field named 'elementName' in the row positioned at 'rowIndex', the
        // type of the corresponding element is 'INT' or 'STRING', and the
        // element is constrained by an enumeration definition.

    const char *enumerationAsString(int rowIndex, int columnIndex) const;
        // Return the string representation of the enumeration element at the
        // specified 'rowIndex' and 'columnIndex' in this binding.  Return 0
        // if the element has a null or unset value.  The returned pointer is
        // valid until the bound schema is destroyed.  The behavior is
        // undefined unless '0 <= rowIndex < numRows()',
        // '0 <= columnIndex < numColumns()', the type of the indexed element
        // is 'INT' or 'STRING', and the element is constrained by an
        // enumeration definition.

    int enumerationAsInt(int rowIndex, const char *elementName) const;
        // Return the integer ID of the enumeration element having the
        // specified 'rowIndex' and 'elementName' in this binding.  Return
        // 'bdetu_Unset<int>::unsetValue()' if the element has an unset value.
        // The behavior is undefined unless '0 <= rowIndex < numRows()', this
        // binding has a field named 'elementName' in the row positioned at
        // 'rowIndex', the type of the corresponding element is 'INT' or
        // 'STRING', and the element is constrained by an enumeration
        // definition.

    int enumerationAsInt(int rowIndex, int columnIndex) const;
        // Return the integer ID of the enumeration element at the specified
        // 'rowIndex' and 'columnIndex' in this binding.  Return
        // 'bdetu_Unset<int>::unsetValue()' if the element has an unset value.
        // The behavior is undefined unless '0 <= rowIndex < numRows()',
        // '0 <= columnIndex < numColumns()', the type of the indexed element
        // is 'INT' or 'STRING', and the element is constrained by an
        // enumeration definition.
};

// FREE OPERATORS
inline
bool operator==(const bdem_ConstTableBinding& lhs,
                const bdem_ConstTableBinding& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' bindings reference tables
    // having the same value and hold records that are equivalent as deemed by
    // 'bdem_SchemaUtil::areEquivalent', and 'false' otherwise.

inline
bool operator!=(const bdem_ConstTableBinding& lhs,
                const bdem_ConstTableBinding& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' bindings reference tables
    // having different values or hold records that are not equivalent as
    // deemed by 'bdem_SchemaUtil::areEquivalent', and 'false' otherwise.

                        // =======================
                        // class bdem_TableBinding
                        // =======================

class bdem_TableBinding: public bdem_ConstTableBinding {
    // This class binds a modifiable 'bdem_Table' ("table") to a non-modifiable
    // 'bdem_RecordDef' ("record"), and provides both by-name and indexed
    // access to the elements of the table.  Indexed access is an efficient
    // constant-time operation; by-name access is somewhat less efficient, but
    // no less so than by-name access using the record 'lookup' methods.  For
    // correct by-name access to elements in the bound table, the table data
    // must satisfy the record meta-data.
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // although copy construction and equality are supported.  Except for the
    // constructors, destructor, and overloaded 'rebind' methods, all instance
    // methods of this object are 'const' and apply not to this object, but
    // rather to the bound table or the bound meta-data.  This class adds to
    // the functionality of its base class by enabling modification of the
    // bound table, but adds *no* additional data.

  public:
    // TYPES
    typedef bdem_ElemRef       ElemRef;
    typedef bdem_RowBinding    RowBinding;
    typedef bdem_ColumnBinding ColumnBinding;
    typedef bdem_Table         Table;
    typedef bdem_Row           Row;

  private:
    // Note: all data resides in the base class.

    // NOT IMPLEMENTED
    bdem_TableBinding& operator=(const bdem_TableBinding&);

  public:
    // CREATORS
    bdem_TableBinding(bdem_Table *table, const bdem_RecordDef *record);
        // Create a table binding that binds the specified modifiable 'table'
        // to the specified non-modifiable 'record'.  The behavior is undefined
        // unless the 'table' data can satisfy the 'record' meta-data.

    bdem_TableBinding(bdem_Table        *table,
                      const bdem_Schema *schema,
                      const char        *recordName);
        // Create a table binding that binds the specified modifiable 'table'
        // to the non-modifiable record definition having the specified
        // 'recordName' in the specified 'schema'.  The behavior is undefined
        // unless the 'table' data can satisfy the record meta-data named
        // 'recordName' in 'schema'.

    bdem_TableBinding(bdem_Table        *table,
                      const bdem_Schema *schema,
                      int                recordIndex);
        // Create a table binding that binds the specified modifiable 'table'
        // to the non-modifiable record definition at the specified
        // 'recordIndex' in the specified 'schema'.  The behavior is undefined
        // unless '0 <= recordIndex < schema->length()' and the 'table' data
        // can satisfy the record meta-data at 'recordIndex' in 'schema'.

    bdem_TableBinding(const bdem_TableBinding& original);
        // Create a table binding that binds a modifiable table to a
        // non-modifiable record, both acquired from the specified 'original'
        // table binding.

    // ~bdem_TableBinding();
        // Destroy this table binding.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    void rebind(const bdem_TableBinding& binding);
        // Reinitialize this table binding to bind a newly-acquired modifiable
        // table to a newly-acquired non-modifiable record, both acquired from
        // the specified table 'binding'.

    void rebind(bdem_Table *table);
        // Reinitialize this table binding to bind the specified modifiable
        // 'table' to the non-modifiable record already held by this binding.
        // The behavior is undefined unless the 'table' data can satisfy the
        // record meta-data.

    void rebind(bdem_Table *table, const bdem_RecordDef *record);
        // Reinitialize this table binding to bind the specified modifiable
        // 'table' to the specified non-modifiable 'record'.  The behavior is
        // undefined unless the 'table' data can satisfy the 'record'
        /// meta-data.

    void rebind(bdem_Table        *table,
                const bdem_Schema *schema,
                const char        *recordName);
        // Reinitialize this table binding to bind the specified modifiable
        // 'table' to the non-modifiable record definition having the specified
        // 'recordName' in the specified 'schema'.  The behavior is undefined
        // unless the 'table' data can satisfy the record meta-data named
        // 'recordName' in 'schema'.

    void rebind(bdem_Table        *table,
                const bdem_Schema *schema,
                int                recordIndex);
        // Reinitialize this table binding to bind the specified modifiable
        // 'table' to the non-modifiable record definition at the specified
        // 'recordIndex' in the specified 'schema'.  The behavior is undefined
        // unless '0 <= recordIndex < schema->length()' and the 'table' data
        // can satisfy the record meta-data at 'recordIndex' in 'schema'.

    int setEnumeration(int                 rowIndex,
                       const char         *elementName,
                       const bsl::string&  enumeratorName) const;
        // Set the value of the enumeration element having the specified
        // 'rowIndex' and 'elementName' in this binding.  If the element is of
        // type 'bdem_ElemType::BDEM_STRING', store the specified
        // 'enumeratorName' in the element.  If the element is of type
        // 'bdem_ElemType::BDEM_INT', look up 'enumeratorName' in the element's
        // enumeration definition and store the corresponding integer
        // enumerator ID in the element.  Return 0 on success, and a non-zero
        // value otherwise.  The behavior is undefined unless this binding has
        // a field named 'elementName' in the row positioned at 'rowIndex', and
        // the type of the referenced element is 'INT' or 'STRING'.

    int setEnumeration(int                 rowIndex,
                       int                 columnIndex,
                       const bsl::string&  enumeratorName) const;
        // Set the value of the enumeration element at the specified 'rowIndex'
        // and 'columnIndex' in this binding.  If the element is of type
        // 'bdem_ElemType::BDEM_STRING', store the specified 'enumeratorName'
        // in the element.  If the element is of type
        // 'bdem_ElemType::BDEM_INT', look up 'enumeratorName' in the element's
        // enumeration definition and store the corresponding integer
        // enumerator ID in the element.  Return 0 on success, and a non-zero
        // value otherwise.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()', '0 <= columnIndex < numColumns()', and
        // the type of the referenced element is 'INT' or 'STRING'.

    int setEnumeration(int         rowIndex,
                       const char *elementName,
                       int         enumeratorId) const;
        // Set the value of the enumeration element having the specified
        // 'rowIndex' and 'elementName' in this binding.  If the element is of
        // type 'bdem_ElemType::BDEM_INT', store the specified 'enumeratorId'
        // in the element.  If the element is of type
        // 'bdem_ElemType::BDEM_STRING, look up 'enumeratorId' in the element's
        // enumeration definition and store the corresponding enumerator name
        // in the element.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined unless this binding has a
        // field named 'elementName' in the row at 'rowIndex', and the type of
        // the referenced element is 'INT' or 'STRING'.

    int setEnumeration(int rowIndex, int columnIndex, int enumeratorId) const;
        // Set the value of the enumeration element at the specified 'rowIndex'
        // and 'columnIndex' in this binding.  If the element is of type
        // 'bdem_ElemType::BDEM_INT', store the specified 'enumeratorId' in the
        // element.  If the element is of type 'bdem_ElemType::BDEM_STRING,
        // look up 'enumeratorId' in the element's enumeration definition and
        // store the corresponding enumerator name in the element.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()',
        // '0 <= columnIndex < numColumns()', and the type of the referenced
        // element is 'INT' or 'STRING'.

    // ACCESSORS
    bdem_Row& operator[](int rowIndex) const;
        // Return a reference to the modifiable row at the specified 'rowIndex'
        // in this binding.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()'.
        //
        // DEPRECATED: use 'theModifiableRow(int rowIndex)' instead.

    bdem_ElemRef element(int rowIndex, const char *columnName) const;
        // Return an element reference that refers to the modifiable element at
        // the specified 'rowIndex' in the column having the specified
        // 'columnName' in this binding.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()', and this binding has a field named
        // 'columnName'.

    bdem_ElemRef element(int rowIndex, int columnIndex) const;
        // Return an element reference that refers to the modifiable element at
        // the specified 'rowIndex' and 'columnIndex' in this binding.  The
        // behavior is undefined unless '0 <= rowIndex < numRows()', and
        // '0 <= columnIndex < table().numColumns()'.

    bdem_Table& table() const;
        // Return a reference to the modifiable table held by this binding.

    bdem_Row& theModifiableRow(int rowIndex) const;
        // Return a reference to the modifiable row at the specified 'rowIndex'
        // in this binding.  If the referenced row is null, it is made non-null
        // before returning, but its value is not otherwise modified.  The
        // behavior is undefined unless '0 <= rowIndex < numRows()'.

    bool& theModifiableBool(int rowIndex, const char *columnName) const;
    char& theModifiableChar(int rowIndex, const char *columnName) const;
    short& theModifiableShort(int rowIndex, const char *columnName) const;
    int& theModifiableInt(int rowIndex, const char *columnName) const;
    bsls_Types::Int64& theModifiableInt64(int         rowIndex,
                                          const char *columnName) const;
    float& theModifiableFloat(int rowIndex, const char *columnName) const;
    double& theModifiableDouble(int rowIndex, const char *columnName) const;
    bsl::string& theModifiableString(int         rowIndex,
                                     const char *columnName) const;
    bdet_Datetime& theModifiableDatetime(int         rowIndex,
                                         const char *columnName) const;
    bdet_DatetimeTz& theModifiableDatetimeTz(int         rowIndex,
                                             const char *columnName) const;
    bdet_Date& theModifiableDate(int rowIndex, const char *columnName) const;
    bdet_DateTz& theModifiableDateTz(int         rowIndex,
                                     const char *columnName) const;
    bdet_Time& theModifiableTime(int rowIndex, const char *columnName) const;
    bdet_TimeTz& theModifiableTimeTz(int         rowIndex,
                                     const char *columnName) const;
    bsl::vector<bool>& theModifiableBoolArray(int         rowIndex,
                                              const char *columnName) const;
    bsl::vector<char>& theModifiableCharArray(int         rowIndex,
                                              const char *columnName) const;
    bsl::vector<short>& theModifiableShortArray(int         rowIndex,
                                                const char *columnName) const;
    bsl::vector<int>& theModifiableIntArray(int         rowIndex,
                                            const char *columnName) const;
    bsl::vector<bsls_Types::Int64>& theModifiableInt64Array(
                                                int         rowIndex,
                                                const char *columnName) const;
    bsl::vector<float>& theModifiableFloatArray(int         rowIndex,
                                                const char *columnName) const;
    bsl::vector<double>& theModifiableDoubleArray(
                                                 int         rowIndex,
                                                 const char *columnName) const;
    bsl::vector<bsl::string>& theModifiableStringArray(
                                                 int         rowIndex,
                                                 const char *columnName) const;
    bsl::vector<bdet_Datetime>& theModifiableDatetimeArray(
                                                int         rowIndex,
                                                const char *columnName) const;
    bsl::vector<bdet_DatetimeTz>& theModifiableDatetimeTzArray(
                                                int         rowIndex,
                                                const char *columnName) const;
    bsl::vector<bdet_Date>& theModifiableDateArray(
                                                 int         rowIndex,
                                                 const char *columnName) const;
    bsl::vector<bdet_DateTz>& theModifiableDateTzArray(
                                                 int         rowIndex,
                                                 const char *columnName) const;
    bsl::vector<bdet_Time>& theModifiableTimeArray(
                                                 int         rowIndex,
                                                 const char *columnName) const;
    bsl::vector<bdet_TimeTz>& theModifiableTimeTzArray(
                                                 int         rowIndex,
                                                 const char *columnName) const;
    bdem_Choice& theModifiableChoice(int         rowIndex,
                                     const char *columnName) const;
    bdem_ChoiceArray& theModifiableChoiceArray(int         rowIndex,
                                               const char *columnName) const;
    bdem_List& theModifiableList(int rowIndex, const char *columnName) const;
    bdem_Table& theModifiableTable(int rowIndex, const char *columnName) const;
        // Return a reference to the modifiable element at the specified
        // 'rowIndex' in the column having the specified 'columnName' in this
        // binding.  If the element is null, it is made non-null before
        // returning, but its value is not otherwise modified.  The behavior is
        // undefined unless '0 <= rowIndex < numRows()', this binding has a
        // field named 'columnName', and the type of the referenced element
        // matches the return type of the method used.  Note that unless there
        // is an intention of modifying the element, the corresponding
        // 'theTYPE' method should be used instead.

    bdem_ChoiceBinding choiceBinding(int         rowIndex,
                                     const char *columnName) const;
        // Return a choice binding that binds the modifiable choice in the
        // list at the specified 'rowIndex' in the column having the specified
        // 'columnName' in this binding to the non-modifiable constraint
        // record in the field named 'columnName' in this binding.  The
        // behavior is undefined unless '0 <= rowIndex < numRows()', this
        // binding has a field named 'columnName', and the field constrains a
        // 'bdem_Choice'.

    bdem_ChoiceBinding choiceBinding(int rowIndex,
                                     int columnIndex) const;
        // Return a choice binding that binds the modifiable choice in the
        // list at the specified 'rowIndex' and 'columnIndex' in this binding
        // to the non-modifiable constraint record at 'columnIndex' in this
        // binding.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()',
        // '0 <= columnIndex < numColumns()', and the indexed field constrains
        // a 'bdem_Choice'.

    bdem_ChoiceArrayBinding choiceArrayBinding(int         rowIndex,
                                               const char *columnName) const;
        // Return a choice array binding that binds the modifiable
        // choice array in the list at the specified 'rowIndex' in the column
        // having the specified 'columnName' in this binding to the
        // non-modifiable constraint record in the field named 'columnName' in
        // this binding.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()', this binding has a field named
        // 'columnName', and the field constrains a 'bdem_ChoiceArray'.

    bdem_ChoiceArrayBinding choiceArrayBinding(int rowIndex,
                                               int columnIndex) const;
        // Return a choice array binding that binds the modifiable
        // choice array in the list at the specified 'rowIndex' and
        // 'columnIndex' in this binding to the non-modifiable constraint
        // record at 'columnIndex' in this binding.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()',
        // '0 <= columnIndex < numColumns()', and the indexed field constrains
        // a 'bdem_ChoiceArray'.

    bdem_RowBinding rowBinding(int rowIndex) const;
        // Return a row binding that binds the modifiable row at the specified
        // 'rowIndex' in this binding to the non-modifiable record bound by
        // this binding.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()'.

    bdem_RowBinding rowBinding(int rowIndex, const char *columnName) const;
        // Return a row binding that binds the modifiable row in the list at
        // the specified 'rowIndex' in the column having the specified
        // 'columnName' in this binding to the non-modifiable constraint record
        // in the field named 'columnName' in this binding.  The behavior is
        // undefined unless '0 <= rowIndex < numRows()', this binding has a
        // field named 'columnName', and the field constrains a 'bdem_List'.

    bdem_RowBinding rowBinding(int rowIndex, int columnIndex) const;
        // Return a row binding that binds the modifiable row in the list at
        // the specified 'rowIndex' and 'columnIndex' in this binding to the
        // non-modifiable constraint record at 'columnIndex' in this binding.
        // The behavior is undefined unless '0 <= rowIndex < numRows()',
        // '0 <= columnIndex < numColumns()', and the indexed field constrains
        // a 'bdem_List'.

    bdem_TableBinding tableBinding(int rowIndex, const char *columnName) const;
        // Return a table binding that binds the modifiable table at the
        // specified 'rowIndex' in the column having the specified 'columnName'
        // in this binding to the non-modifiable constraint record in the field
        // named 'columnName' in this binding.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()', this binding has a field named
        // 'columnName', and the field constrains a 'bdem_Table'.

    bdem_TableBinding tableBinding(int rowIndex, int columnIndex) const;
        // Return a table binding that binds the modifiable table at the
        // specified 'rowIndex' and 'columnIndex' in this binding to the
        // non-modifiable constraint record at 'columnIndex' in this binding.
        // The behavior is undefined unless '0 <= rowIndex < numRows()',
        // '0 <= columnIndex < numColumns()', and the indexed field constrains
        // a 'bdem_Table'.

    bdem_ColumnBinding columnBinding(const char *columnName) const;
        // Return a column binding that binds the modifiable column having the
        // specified 'columnName' in this binding to the non-modifiable field
        // named 'columnName' in this binding.  The behavior is undefined
        // unless this binding has a field named 'columnName'.

    bdem_ColumnBinding columnBinding(int columnIndex) const;
        // Return a column binding that binds the modifiable column at the
        // specified 'columnIndex' in this binding to the non-modifiable field
        // at 'columnIndex' in this binding.  The behavior is undefined unless
        // '0 <= columnIndex < numColumns()'.
};

                        // =============================
                        // class bdem_ConstColumnBinding
                        // =============================

class bdem_ConstColumnBinding {
    // This class binds a non-modifiable column in a 'bdem_Table' ("column") to
    // a non-modifiable 'bdem_FieldDef' ("field"), and provides indexed access
    // to the elements of the column (by-name access is not provided by column
    // bindings).  Indexed access is an efficient constant-time operation.
    // The column data must satisfy the field meta-data to ensure that this
    // binding can create valid row and table bindings ('rowBinding' and
    // 'tableBinding' methods, respectively).
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // although copy construction and equality are supported.  Except for the
    // constructors, destructor, and the overloaded 'rebind' methods, all
    // instance methods of this object are 'const' and apply not to this
    // object, but rather to the bound column or the bound meta-data.  Note
    // also that, unlike the derived 'bdem_ColumnBinding' class (also defined
    // within this component), none of the methods within this base class
    // enable modifiable access to the bound column, thus ensuring 'const'
    // correctness.

  public:
    // TYPES
    typedef bdem_ConstElemRef      ElemRef;
    typedef bdem_ConstRowBinding   RowBinding;
    typedef bdem_ConstTableBinding TableBinding;

  private:
    // DATA
    const bdem_Table    *d_table_p;       // data (held, not owned)
    int                  d_columnIndex;   // column index in table
    const bdem_FieldDef *d_constraint_p;  // meta-data (held, not owned)

    // FRIENDS
    friend bool operator==(const bdem_ConstColumnBinding& lhs,
                           const bdem_ConstColumnBinding& rhs);

    friend class bdem_ColumnBinding;
        // Allow the derived class to access the private members of this class.

  private:
    // NOT IMPLEMENTED
    bdem_ConstColumnBinding& operator=(const bdem_ConstColumnBinding&);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    // PRIVATE ACCESSORS
    bool canSatisfyColumn(const bdem_Table    *table,
                          int                  columnIndex,
                          const bdem_FieldDef *field);
        // Return 'true' if the data in the column at the specified
        // 'columnIndex' of the specified 'table' can satisfy the constraint of
        // the specified 'field', and 'false' otherwise.
#endif

  public:
    // CREATORS
    bdem_ConstColumnBinding(const bdem_Table    *table,
                            int                  columnIndex,
                            const bdem_FieldDef *field);
        // Create a "const" column binding that binds the non-modifiable column
        // at the specified 'columnIndex' in the specified 'table' to the
        // specified non-modifiable 'field'.  The behavior is undefined unless
        // '0 <= columnIndex < table->numColumns()' and every data element at
        // 'columnIndex' in 'table' can satisfy the 'field' meta-data.

    bdem_ConstColumnBinding(const bdem_ConstColumnBinding& original);
        // Create a "const" column binding that binds a non-modifiable column
        // to a non-modifiable field, both acquired from the specified
        // 'original' column binding.

    // ~bdem_ConstColumnBinding();
        // Destroy this 'const' column binding.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    void rebind(const bdem_ConstColumnBinding& binding);
        // Reinitialize this column binding to bind a newly-acquired
        // non-modifiable column to a newly-acquired non-modifiable field, both
        // acquired from the specified column 'binding'.

    void rebind(const bdem_Table    *table,
                int                  columnIndex,
                const bdem_FieldDef *field);
        // Reinitialize this column binding to bind the non-modifiable column
        // at the specified 'columnIndex' in the specified 'table' to the
        // specified non-modifiable 'field'.  The behavior is undefined unless
        // '0 <= columnIndex < table->numColumns()' and every data element at
        // 'columnIndex' in 'table' can satisfy the 'field' meta-data.

    // ACCESSORS
    bdem_ConstElemRef operator[](int rowIndex) const;
        // Return a "const" element reference that refers to the non-modifiable
        // element at the specified 'rowIndex' in this binding.  The behavior
        // is undefined unless '0 <= rowIndex < numRows()'.

    bdem_ConstElemRef element(int rowIndex) const;
        // Return a "const" element reference that refers to the non-modifiable
        // element at the specified 'rowIndex' in this binding.  The behavior
        // is undefined unless '0 <= rowIndex < numRows()'.

    bdem_ElemType::Type elemType() const;
        // Return the 'bdem_ElemType::Type' value of the elements in this
        // binding.

    int numRows() const;
        // Return the number of elements (rows) in the column held by this
        // binding.

    int columnIndex() const;
        // Return the column index of the column held by this binding.

    const bdem_FieldDef& field() const;
        // Return a reference to the non-modifiable field held by this binding.

    const bdem_Table& table() const;
        // Return a reference to the non-modifiable table holding the column
        // held by this binding.

    bdem_ConstChoiceBinding choiceBinding(int rowIndex) const;
        // Return a "const" choice binding that binds the non-modifiable
        // choice at the specified 'rowIndex' in this binding to the
        // non-modifiable constraint record held by the field bound by this
        // binding.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()', and the field constrains a
        // 'bdem_Choice'.

    bdem_ConstChoiceArrayBinding choiceArrayBinding(int rowIndex) const;
        // Return a "const" choice array binding that binds the non-modifiable
        // choice array in at the specified 'rowIndex' in this binding to the
        // non-modifiable constraint record held by the field bound by this
        // binding.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()', and the field constrains a
        // 'bdem_ChoiceArray'.

    bdem_ConstRowBinding rowBinding(int rowIndex) const;
        // Return a "const" row binding that binds the non-modifiable row in
        // the list at the specified 'rowIndex' in this binding to the
        // non-modifiable constraint record held by the field bound by this
        // binding.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()', and the field constrains a
        // 'bdem_List'.

    bdem_ConstTableBinding tableBinding(int rowIndex) const;
        // Return a "const" table binding that binds the non-modifiable table
        // at the specified 'rowIndex' in this binding to the non-modifiable
        // constraint record held by the field bound by this binding.  The
        // behavior is undefined unless '0 <= rowIndex < numRows()', and
        // the field constrains a 'bdem_Table'.

    const char *enumerationAsString(int rowIndex) const;
        // Return the string representation of the enumeration element at the
        // specified 'rowIndex' in this binding.  Return 0 if the element has a
        // null or unset value.  The returned pointer is valid until the bound
        // schema is destroyed.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()', the type of the indexed element is
        // 'INT' or 'STRING', and the element is constrained by an enumeration
        // definition.

    int enumerationAsInt(int rowIndex) const;
        // Return the integer ID of the enumeration element at the specified
        // 'rowIndex' in this binding.  Return 'bdetu_Unset<int>::unsetValue()'
        // if the element has an unset value.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()', the type of the indexed element is
        // 'INT' or 'STRING', and the element is constrained by an enumeration
        // definition.
};

// FREE OPERATORS
bool operator==(const bdem_ConstColumnBinding& lhs,
                const bdem_ConstColumnBinding& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' bindings hold fields
    // having the same type and either null constraints or else equivalent
    // constraints as deemed by 'bdem_SchemaUtil::areEquivalent', and reference
    // columns having the same number of elements with respective elements at
    // each index position having the same value; return 'false' otherwise.

bool operator!=(const bdem_ConstColumnBinding& lhs,
                const bdem_ConstColumnBinding& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' bindings hold fields
    // having differing types or exactly one null constraint or non-equivalent
    // (non-null) constraints as deemed by 'bdem_SchemaUtil::areEquivalent', or
    // reference columns having a differing number of elements or with
    // respective elements having differing values in at least one index
    // position; return 'false' otherwise.

                        // ========================
                        // class bdem_ColumnBinding
                        // ========================

class bdem_ColumnBinding: public bdem_ConstColumnBinding {
    // This class binds a modifiable column in a 'bdem_Table' ("column") to a
    // non-modifiable 'bdem_FieldDef' ("field"), and provides indexed access
    // to the elements of the column (by-name access is not provided by column
    // bindings).  Indexed access is an efficient constant-time operation.
    // The column data must satisfy the field meta-data to ensure that this
    // binding can create valid row and table bindings ('rowBinding' and
    // 'tableBinding' methods, respectively).
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // although copy construction and equality are supported.  Except for the
    // constructors, destructor, and overloaded 'rebind' methods, all instance
    // methods of this object are 'const' and apply not to this object, but
    // rather to the bound column or the bound meta-data.  This class adds to
    // the functionality of its base class by enabling modification of the
    // bound column, but adds *no* additional data.

  public:
    // TYPES
    typedef bdem_ElemRef      ElemRef;
    typedef bdem_RowBinding   RowBinding;
    typedef bdem_TableBinding TableBinding;

  private:
    // Note: all data resides in the base class.

    // NOT IMPLEMENTED
    bdem_ColumnBinding& operator=(const bdem_ColumnBinding&);

  public:
    // CREATORS
    bdem_ColumnBinding(bdem_Table          *table,
                       int                  columnIndex,
                       const bdem_FieldDef *field);
        // Create a column binding that binds the modifiable column at the
        // specified 'columnIndex' in the specified 'table' to the specified
        // non-modifiable 'field'.  The behavior is undefined unless
        // '0 <= columnIndex < table->numColumns()' and every data element at
        // 'columnIndex' in 'table' can satisfy the 'field' meta-data.

    bdem_ColumnBinding(const bdem_ColumnBinding& original);
        // Create a column binding that binds a modifiable column to a
        // non-modifiable field, both acquired from the specified 'original'
        // column binding.

    // ~bdem_ColumnBinding();
        // Destroy this column binding.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    void rebind(const bdem_ColumnBinding& binding);
        // Reinitialize this column binding to bind a newly-acquired modifiable
        // column to a newly-acquired non-modifiable field, both acquired from
        // the specified column 'binding'.

    void rebind(bdem_Table          *table,
                int                  columnIndex,
                const bdem_FieldDef *field);
        // Reinitialize this column binding to bind the modifiable column at
        // the specified 'columnIndex' in the specified 'table' to the
        // specified non-modifiable 'field'.  The behavior is undefined unless
        // '0 <= columnIndex < table->numColumns()' and every data element at
        // 'columnIndex' in 'table' can satisfy the 'field' meta-data.

    int setEnumeration(int rowIndex, const bsl::string& enumeratorName) const;
        // Set the value of the enumeration element at the specified 'rowIndex'
        // in this binding.  If the element is of type
        // 'bdem_ElemType::BDEM_STRING', store the specified 'enumeratorName'
        // in the element.  If the element is of type
        // 'bdem_ElemType::BDEM_INT', look up 'enumeratorName' in the element's
        // enumeration definition and store the corresponding integer
        // enumerator ID in the element.  Return 0 on success, and a non-zero
        // value otherwise.  The behavior is undefined unless
        // '0 <= index < numRows()', and the type of the referenced element is
        // 'INT' or 'STRING'.

    int setEnumeration(int rowIndex, int enumeratorId) const;
        // Set the value of the enumeration element at the specified 'rowIndex'
        // in this binding.  If the element is of type
        // 'bdem_ElemType::BDEM_INT', store the specified 'enumeratorId' in the
        // element.  If the element is of type 'bdem_ElemType::BDEM_STRING,
        // look up 'enumeratorId' in the element's enumeration definition and
        // store the corresponding enumerator name in the element.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()', and the type of the referenced
        // element is 'INT' or 'STRING'.

    // ACCESSORS
    bdem_ElemRef operator[](int rowIndex) const;
        // Return an element reference that refers to the modifiable element at
        // the specified 'rowIndex' in this binding.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()'.

    bdem_Table& table() const;
        // Return a reference to the modifiable table holding the column held
        // by this binding.

    bdem_ElemRef element(int rowIndex) const;
        // Return an element reference that refers to the modifiable element at
        // the specified 'rowIndex' in this binding.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()'.

    bdem_ChoiceBinding choiceBinding(int rowIndex) const;
        // Return a choice binding that binds the modifiable choice at the
        // specified 'rowIndex' in this binding to the non-modifiable
        // constraint record held by the field bound by this binding.  The
        // behavior is undefined unless '0 <= rowIndex < numRows()', and
        // the field constrains a 'bdem_Choice'.

    bdem_ChoiceArrayBinding choiceArrayBinding(int rowIndex) const;
        // Return a choice array binding that binds the modifiable choice
        // array in at the specified 'rowIndex' in this binding to the
        // non-modifiable constraint record held by the field bound by this
        // binding.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()', and the field constrains a
        // 'bdem_ChoiceArray'.

    bdem_RowBinding rowBinding(int rowIndex) const;
        // Return a row binding that binds the modifiable row in the list at
        // the specified 'rowIndex' in this binding to the non-modifiable
        // constraint record held by the field bound by this binding.  The
        // behavior is undefined unless '0 <= rowIndex < numRows()', and
        // the field constrains a 'bdem_List'.

    bdem_TableBinding tableBinding(int rowIndex) const;
        // Return a table binding that binds the modifiable table at the
        // specified 'rowIndex' in this binding to the non-modifiable
        // constraint record held by the field bound by this binding.  The
        // behavior is undefined unless '0 <= rowIndex < numRows()', and
        // the field constrains a 'bdem_Table'.
};

class bdem_ConstChoiceArrayBinding;
class bdem_ChoiceArrayBinding;
class bdem_ConstChoiceArrayItem;
class bdem_ChoiceArrayItem;

                        // =============================
                        // class bdem_ConstChoiceBinding
                        // =============================

class bdem_ConstChoiceBinding {
    // This class binds a non-modifiable 'bdem_ChoiceArrayItem' ("item") to a
    // non-modifiable 'bdem_RecordDef' ("record"), and provides access to the
    // elements of the item.  This class also provides by-name and by-index
    // access to the various selections of the underlying item.  For correct
    // by-name access to selections in the bound item, the item data must
    // satisfy the record meta-data.
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // although copy construction and equality are supported.  Except for the
    // constructors, destructor, and the overloaded 'rebind' methods, all
    // instance methods of this object are 'const' and apply not to this
    // object, but rather to the bound item or the bound meta-data.  Note also
    // that, unlike the derived 'bdem_ChoiceBinding' class (also defined within
    // this component), none of the methods within this base class enable
    // modifiable access to the bound item, thus ensuring 'const' correctness.

  public:
    // TYPES
    typedef bdem_ConstElemRef            ElemRef;
    typedef bdem_ConstRowBinding         RowBinding;
    typedef bdem_ConstTableBinding       TableBinding;
    typedef bdem_ConstChoiceBinding      ChoiceBinding;
    typedef bdem_ConstChoiceArrayBinding ChoiceArrayBinding;
    typedef const bdem_Choice            Choice;

  private:
    // DATA
    const bdem_ChoiceArrayItem *d_item_p;        // data (held, not owned)
    const bdem_RecordDef       *d_constraint_p;  // meta-data (held, not owned)

    // FRIENDS
    friend bool operator==(const bdem_ConstChoiceBinding& lhs,
                           const bdem_ConstChoiceBinding& rhs);

    friend class bdem_ChoiceBinding;
        // Allow the derived class to access the private members of this
        // class.

  private:
    // NOT IMPLEMENTED
    bdem_ConstChoiceBinding& operator=(const bdem_ConstChoiceBinding&);

  public:
    // CREATORS
    bdem_ConstChoiceBinding(const bdem_ChoiceArrayItem *item,
                            const bdem_RecordDef       *record);
        // Create a "const" choice binding that binds the specified
        // non-modifiable 'item' to the specified non-modifiable 'record'.
        // The behavior is undefined unless the 'item' data can satisfy the
        // 'record' meta-data.

    bdem_ConstChoiceBinding(const bdem_ChoiceArrayItem *item,
                            const bdem_Schema          *schema,
                            const char                 *recordName);
        // Create a "const" choice binding that binds the specified
        // non-modifiable 'item' to the non-modifiable record definition
        // having the specified 'recordName' in the specified 'schema'.  The
        // behavior is undefined unless the 'item' data can satisfy the
        // record meta-data named 'recordName' in 'schema'.

    bdem_ConstChoiceBinding(const bdem_ChoiceArrayItem *item,
                            const bdem_Schema          *schema,
                            int                         recordIndex);
        // Create a "const" choice binding that binds the specified
        // non-modifiable 'item' to the non-modifiable record definition at
        // the specified 'recordIndex' in the specified 'schema'.  The behavior
        // is undefined unless '0 <= recordIndex < schema->length()' and the
        // 'item' data can satisfy the record meta-data at 'recordIndex' in
        // 'schema'.

    bdem_ConstChoiceBinding(const bdem_Choice    *choice,
                            const bdem_RecordDef *record);
        // Create a "const" choice binding that binds the specified
        // non-modifiable 'choice' to the specified non-modifiable 'record'.
        // The behavior is undefined unless the 'choice' data can satisfy the
        // 'record' meta-data.

    bdem_ConstChoiceBinding(const bdem_Choice *choice,
                            const bdem_Schema *schema,
                            const char        *recordName);
        // Create a "const" choice binding that binds the specified
        // non-modifiable 'choice' to the non-modifiable record definition
        // having the specified 'recordName' in the specified 'schema'.  The
        // behavior is undefined unless the 'choice' data can satisfy the
        // record meta-data named 'recordName' in 'schema'.

    bdem_ConstChoiceBinding(const bdem_Choice *choice,
                            const bdem_Schema *schema,
                            int                recordIndex);
        // Create a "const" choice binding that binds the specified
        // non-modifiable 'choice' to the non-modifiable record definition at
        // the specified 'recordIndex' in the specified 'schema'.  The behavior
        // is undefined unless '0 <= recordIndex < schema->length()' and the
        // 'choice' data can satisfy the record meta-data at 'recordIndex' in
        // 'schema'.

    bdem_ConstChoiceBinding(const bdem_ChoiceArray *choiceArray,
                            int                     choiceIndex,
                            const bdem_RecordDef   *record);
        // Create a "const" choice binding that binds the non-modifiable
        // choice array item at the specified 'choiceIndex' in the specified
        // 'choiceArray' to the specified non-modifiable 'record'.  The
        // behavior is undefined
        // unless '0 <= choiceIndex < choiceArray->length()' and the item data
        // at 'choiceIndex' in 'choiceArray' can satisfy the 'record'
        // meta-data.

    bdem_ConstChoiceBinding(const bdem_ChoiceArray *choiceArray,
                            int                     choiceIndex,
                            const bdem_Schema      *schema,
                            const char             *recordName);
        // Create a "const" choice binding that binds the non-modifiable
        // choice array item at the specified 'choiceIndex' in the specified
        // 'choiceArray' to the non-modifiable record definition having the
        // specified 'recordName' in the specified 'schema'.  The behavior is
        // undefined unless '0 <= choiceIndex < choiceArray->length()' and the
        // item data at 'choiceIndex' in 'choiceArray' can satisfy the record
        // meta-data named 'recordName' in 'schema'.

     bdem_ConstChoiceBinding(const bdem_ChoiceArray *choiceArray,
                             int                     choiceIndex,
                             const bdem_Schema      *schema,
                             int                     recordIndex);
        // Create a "const" choice binding that binds the non-modifiable
        // choice array item at the specified 'choiceIndex' in the specified
        // 'choiceArray' to the non-modifiable record definition at the
        // specified 'recordIndex' in the specified 'schema'.  The behavior is
        // undefined unless '0 <= choiceIndex < choiceArray->length()',
        // '0 <= recordIndex < schema->length()', and the item data at
        // 'choiceIndex' in 'choiceArray' can satisfy the record meta-data at
        // 'recordIndex' in 'schema'.

    bdem_ConstChoiceBinding(const bdem_ConstChoiceBinding& original);
        // Create a "const" choice binding that binds a non-modifiable choice
        // array item to a non-modifiable record, both acquired from the
        // specified 'original' choice binding.

    // ~bdem_ConstChoiceBinding();
        // Destroy this 'const' choice binding.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    void rebind(const bdem_ConstChoiceBinding& binding);
        // Reinitialize this choice binding to bind a newly-acquired
        // non-modifiable choice array item to a newly-acquired non-modifiable
        // record, both acquired from the specified choice 'binding'.

    void rebind(const bdem_ChoiceArrayItem *item);
        // Reinitialize this choice binding to bind the specified
        // non-modifiable 'item' to the non-modifiable record already held
        // by this binding.  The behavior is undefined unless the 'item'
        // data can satisfy the record meta-data.

    void rebind(const bdem_Choice *choice);
        // Reinitialize this choice binding to bind the specified
        // non-modifiable 'choice' to the non-modifiable record already held
        // by this binding.  The behavior is undefined unless the 'choice'
        // data can satisfy the record meta-data.

    void rebind(const bdem_ChoiceArray *choiceArray, int choiceIndex);
        // Reinitialize this choice binding to bind the non-modifiable item at
        // the specified 'choiceIndex' in the specified 'choiceArray' to the
        // non-modifiable record already held by this binding.  The behavior
        // is undefined unless '0 <= choiceIndex < choiceArray->length()'
        // and the choice data at 'choiceIndex' in 'choiceArray' can satisfy
        // the record meta-data.

    void rebind(const bdem_ChoiceArrayItem *item,
                const bdem_RecordDef       *record);
        // Reinitialize this choice binding to bind the specified
        // non-modifiable 'item' to the specified non-modifiable 'record'.
        // The behavior is undefined unless the 'item' data can satisfy the
        // 'record' meta-data.

    void rebind(const bdem_ChoiceArrayItem *item,
                const bdem_Schema          *schema,
                const char                 *recordName);
        // Reinitialize this choice binding to bind the specified
        // non-modifiable 'item' to the non-modifiable record definition
        // having the specified 'recordName' in the specified 'schema'.  The
        // behavior is undefined unless the 'item' data can satisfy the
        // record meta-data named 'recordName' in 'schema'.

    void rebind(const bdem_ChoiceArrayItem *item,
                const bdem_Schema          *schema,
                int                         recordIndex);
        // Reinitialize this choice binding to bind the specified
        // non-modifiable 'item' to the non-modifiable record definition at
        // the specified 'recordIndex' in the specified 'schema'.  The
        // behavior is undefined unless '0 <= recordIndex < schema->length()'
        // and the 'item' data can satisfy the record meta-data at
        // 'recordIndex' in 'schema'.

    void rebind(const bdem_Choice *choice, const bdem_RecordDef *record);
        // Reinitialize this choice binding to bind the specified
        // non-modifiable 'choice' to the specified non-modifiable 'record'.
        // The behavior is undefined unless the 'choice' data can satisfy the
        // 'record' meta-data.

    void rebind(const bdem_Choice *choice,
                const bdem_Schema *schema,
                const char        *recordName);
        // Reinitialize this choice binding to bind the specified
        // non-modifiable 'choice' to the non-modifiable record definition
        // having the specified 'recordName' in the specified 'schema'.  The
        // behavior is undefined unless the 'choice' data can satisfy the
        // record meta-data named 'recordName' in 'schema'.

    void rebind(const bdem_Choice *choice,
                const bdem_Schema *schema,
                int                recordIndex);
        // Reinitialize this choice binding to bind the specified
        // non-modifiable 'choice' to the non-modifiable record definition at
        // the specified 'recordIndex' in the specified 'schema'.  The
        // behavior is undefined unless '0 <= recordIndex < schema->length()'
        // and the 'choice' data can satisfy the record meta-data at
        // 'recordIndex' in 'schema'.

    void rebind(const bdem_ChoiceArray *choiceArray,
                int                     choiceIndex,
                const bdem_RecordDef   *record);
        // Reinitialize this choice binding to bind the non-modifiable choice
        // array item at the specified 'choiceIndex' in the specified
        // 'choiceArray' to the specified non-modifiable 'record'.  The
        // behavior is undefined
        // unless '0 <= choiceIndex < choiceArray->length()' and the item data
        // at 'choiceIndex' in 'choiceArray' can satisfy the 'record'
        // meta-data.

    void rebind(const bdem_ChoiceArray *choiceArray,
                int                     choiceIndex,
                const bdem_Schema      *schema,
                const char             *recordName);
        // Reinitialize this choice binding to bind the non-modifiable choice
        // at the specified 'choiceIndex' in the specified 'choiceArray' to the
        // specified non-modifiable 'record'.  The behavior is undefined unless
        // '0 <= choiceIndex < choiceArray->numChoices()' and the choice data
        // at 'choiceIndex' in 'choiceArray' can satisfy the 'record'
        // meta-data.

    void rebind(const bdem_ChoiceArray *choiceArray,
                int                     choiceIndex,
                const bdem_Schema      *schema,
                int                     recordIndex);
        // Reinitialize this choice binding to bind the non-modifiable choice
        // array item at the specified 'choiceIndex' in the specified
        // 'choiceArray' to the non-modifiable record definition at the
        // specified 'recordIndex' in the specified 'schema'.  The behavior is
        // undefined unless '0 <= choiceIndex < choiceArray->length()',
        // '0 <= recordIndex < schema->length()', and the item data at
        // 'choiceIndex' in 'choiceArray' can satisfy the record meta-data at
        // 'recordIndex' in 'schema'.

    // ACCESSORS
    int numSelections() const;
        // Return the number of fields in the 'record' held by this binding.

    const bdem_ChoiceArrayItem& item() const;
        // Return a reference to the non-modifiable choice array item held by
        // this binding.

    const bdem_RecordDef& record() const;
        // Return a reference to the non-modifiable record held by this
        // binding.

    bdem_ConstElemRef selection() const;
        // Return a "const" element reference that refers to the non-modifiable
        // selection stored in this binding.  The behavior is undefined unless
        // the enclosed 'item' has an element selected.

    bdem_ElemType::Type selectionType(int index) const;
        // Return the 'bdem_ElemType::Type' value of the element at the
        // specified 'index' of the 'record' stored in this binding.  The
        // behavior is undefined unless '0 <= index < numSelections()'.

    bdem_ElemType::Type selectionType(const char *elementName) const;
        // Return the 'bdem_ElemType::Type' value of the element having the
        // specified 'elementName' in this binding.  The behavior is undefined
        // unless this binding stores a field named 'elementName' in its
        // 'record'.

    bdem_ElemType::Type selectionType() const;
        // Return the 'bdem_ElemType::Type' value of the selection stored in
        // this binding or 'bdem_ElemType::BDEM_VOID' if no element is
        // currently selected in the held 'choice'.

    int selector() const;
        // Return the current selector (i.e., the index of the currently
        // selected type within the types catalog) of the stored 'item' or -1
        // if no element is currently selected.

    int selectorId() const;
        // Return the id associated with the current selector of the stored
        // item (i.e., the field id for they selected type within the 'record'
        // associated with this binding).  Return
        // 'bdem_RecordDef::BDEM_NULL_FIELD_ID' if no element is currently
        // selected.

    const char *selectorName() const;
        // Return the name of the current selection of the stored 'item' as
        // specified in the 'record' stored in this binding or 0 if no element
        // is currently selected.  The behavior is undefined unless the stored
        // 'item' can satisfy the stored 'record'.

    const bool& theBool() const;
    const char& theChar() const;
    const short& theShort() const;
    const int& theInt() const;
    const bsls_Types::Int64& theInt64() const;
    const float& theFloat() const;
    const double& theDouble() const;
    const bsl::string& theString() const;
    const bdet_Datetime& theDatetime() const;
    const bdet_DatetimeTz& theDatetimeTz() const;
    const bdet_Date& theDate() const;
    const bdet_DateTz& theDateTz() const;
    const bdet_Time& theTime() const;
    const bdet_TimeTz& theTimeTz() const;
    const bsl::vector<bool>& theBoolArray() const;
    const bsl::vector<char>& theCharArray() const;
    const bsl::vector<short>& theShortArray() const;
    const bsl::vector<int>& theIntArray() const;
    const bsl::vector<bsls_Types::Int64>& theInt64Array() const;
    const bsl::vector<float>& theFloatArray() const;
    const bsl::vector<double>& theDoubleArray() const;
    const bsl::vector<bsl::string>& theStringArray() const;
    const bsl::vector<bdet_Datetime>& theDatetimeArray() const;
    const bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray() const;
    const bsl::vector<bdet_Date>& theDateArray() const;
    const bsl::vector<bdet_DateTz>& theDateTzArray() const;
    const bsl::vector<bdet_Time>& theTimeArray() const;
    const bsl::vector<bdet_TimeTz>& theTimeTzArray() const;
    const bdem_Choice& theChoice() const;
    const bdem_ChoiceArray& theChoiceArray() const;
    const bdem_List& theList() const;
    const bdem_Table& theTable() const;
        // Return a reference to the non-modifiable selection of this binding.
        // The nullness of the element is not affected.  The behavior is
        // undefined unless the type of the selection matches the return type
        // of the method used.

    bdem_ConstChoiceBinding choiceBinding() const;
        // Return a "const" choice binding that binds the non-modifiable
        // choice array item selected by the 'item' stored in this binding to
        // the non-modifiable constraint record stored in the 'record' held by
        // this binding at the index specified by the selector value of 'item'.
        // The behavior is undefined unless
        // '0 <= selector() < numSelections()', the indexed record constrains
        // a 'bdem_Choice' and the choice array item satisfies the indexed
        // record.

    bdem_ConstChoiceArrayBinding choiceArrayBinding() const;
        // Return a "const" choice array binding that binds the non-modifiable
        // choice array selected by the 'item' stored in this binding to the
        // non-modifiable constraint record stored in the 'record' held by
        // this binding at the index specified by the selector value of 'item'.
        // The behavior is undefined unless
        // '0 <= selector() < numSelections()', the indexed record constrains
        // a 'bdem_ChoiceArray' and the choice array satisfies the indexed
        // record.

    bdem_ConstRowBinding rowBinding() const;
        // Return a "const" row binding that binds the non-modifiable row
        // selected by the 'item' stored in this binding to the
        // non-modifiable constraint record stored in the 'record' held by
        // this binding at the index specified by the selector value of 'item'.
        // The behavior is undefined unless
        // '0 <= selector() < numSelections()', the indexed record constrains
        // a 'bdem_List' and the row satisfies the indexed record.

    bdem_ConstTableBinding tableBinding() const;
        // Return a "const" table binding that binds the non-modifiable table
        // selected by the 'item' stored in this binding to the
        // non-modifiable constraint record stored in the 'record' held by
        // this binding at the index specified by the selector value of 'item'.
        // The behavior is undefined unless
        // '0 <= selector() < numSelections()', the indexed record constrains
        // a 'bdem_Table' and the table satisfies the indexed record.

    const char *enumerationAsString() const;
        // Return the string representation of the enumeration element for the
        // currently selected type in this binding.  Return 0 if the element
        // has a null or unset value.  The returned pointer is valid until the
        // bound schema is destroyed.  The behavior is undefined unless
        // '0 <= selector() < numSelections()', the type of the selected
        // element is 'INT' or 'STRING', and the element is constrained by an
        // enumeration definition.

    int enumerationAsInt() const;
        // Return the integer ID of the enumeration element for the currently
        // selected type in this binding.  Return
        // 'bdetu_Unset<int>::unsetValue()' if the element has an unset value.
        // The behavior is undefined unless
        // '0 <= selector() < numSelections()', the type of the selected
        // element is 'INT' or 'STRING', and the element is constrained by an
        // enumeration definition.
};

                        // ========================
                        // class bdem_ChoiceBinding
                        // ========================

class bdem_ChoiceBinding : public bdem_ConstChoiceBinding {
    // This class binds a modifiable 'bdem_ChoiceArrayItem' ("item") to a
    // non-modifiable 'bdem_RecordDef' ("record"), and provides access to the
    // elements of the item.  This class also provides by-name and by-index
    // modifiable access to the various selections of the underlying item.
    // For correct by-name access to selections in the bound item, the
    // item data must satisfy the record meta-data.
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // although copy construction and equality are supported.  Except for the
    // constructors, destructor, and overloaded 'rebind' methods, all instance
    // methods of this object are 'const' and apply not to this object, but
    // rather to the bound item or the bound meta-data.  This class adds to the
    // functionality of its base class by enabling modification of the bound
    // item, but adds *no* additional data.  Additionally, it provides users
    // the ability to modify the current selection of the bound item.

  public:
    // TYPES
    typedef bdem_ElemRef            ElemRef;
    typedef bdem_RowBinding         RowBinding;
    typedef bdem_TableBinding       TableBinding;
    typedef bdem_ChoiceBinding      ChoiceBinding;
    typedef bdem_ChoiceArrayBinding ChoiceArrayBinding;
    typedef bdem_Choice             Choice;

  private:
    // Note: all data resides in the base class.

    // NOT IMPLEMENTED
    bdem_ChoiceBinding& operator=(const bdem_ChoiceBinding&);

  public:
    // CREATORS
    bdem_ChoiceBinding(bdem_ChoiceArrayItem *item,
                       const bdem_RecordDef *record);
        // Create a choice binding that binds the specified modifiable
        // 'item' to the specified non-modifiable 'record'.  The
        // behavior is undefined unless the 'item' data can satisfy
        // the 'record' meta-data.

    bdem_ChoiceBinding(bdem_ChoiceArrayItem *item,
                       const bdem_Schema    *schema,
                       const char           *recordName);
        // Create a choice binding that binds the specified modifiable
        // 'item' to the non-modifiable record definition having the
        // specified 'recordName' in the specified 'schema'.  The behavior is
        // undefined unless the 'item' data can satisfy the record meta-data
        // named 'recordName' in 'schema'.

    bdem_ChoiceBinding(bdem_ChoiceArrayItem *item,
                       const bdem_Schema    *schema,
                       int                   recordIndex);
        // Create a choice binding that binds the specified modifiable
        // 'item' to the non-modifiable record definition at the specified
        // 'recordIndex' in the specified 'schema'.  The behavior is undefined
        // unless '0 <= recordIndex < schema->length()' and the 'item' data can
        // satisfy the record meta-data at 'recordIndex' in 'schema'.

    bdem_ChoiceBinding(bdem_Choice          *choice,
                       const bdem_RecordDef *record);
        // Create a choice binding that binds the specified modifiable
        // 'choice' to the specified non-modifiable 'record'.  The behavior is
        // undefined unless the 'choice' data can satisfy the 'record'
        // meta-data.

    bdem_ChoiceBinding(bdem_Choice       *choice,
                       const bdem_Schema *schema,
                       const char        *recordName);
        // Create a choice binding that binds the specified modifiable
        // 'choice' to the non-modifiable record definition having the
        // specified 'recordName' in the specified 'schema'.  The behavior is
        // undefined unless the 'choice' data can satisfy the record meta-data
        // named 'recordName' in 'schema'.

    bdem_ChoiceBinding(bdem_Choice       *choice,
                       const bdem_Schema *schema,
                       int                recordIndex);
        // Create a choice binding that binds the specified modifiable
        // 'choice' to the non-modifiable record definition at the specified
        // 'recordIndex' in the specified 'schema'.  The behavior is undefined
        // unless '0 <= recordIndex < schema->length()' and the 'choice' data
        // can satisfy the record meta-data at 'recordIndex' in 'schema'.

    bdem_ChoiceBinding(bdem_ChoiceArray     *choiceArray,
                       int                   choiceIndex,
                       const bdem_RecordDef *record);
        // Create a choice binding that binds the modifiable choice array item
        // at the specified 'choiceIndex' in the specified 'choiceArray' to
        // the specified non-modifiable 'record'.  The behavior is undefined
        // unless '0 <= choiceIndex < choiceArray->length()' and the item data
        // at 'choiceIndex' in 'choiceArray' can satisfy the 'record'
        // meta-data.

    bdem_ChoiceBinding(bdem_ChoiceArray  *choiceArray,
                       int                choiceIndex,
                       const bdem_Schema *schema,
                       const char        *recordName);
        // Create a choice binding that binds the modifiable choice array item
        // at the specified 'choiceIndex' in the specified 'choiceArray' to
        // the non-modifiable record definition having the specified
        // 'recordName' in the specified 'schema'.  The behavior is undefined
        // unless '0 <= choiceIndex < choiceArray->length()' and the item data
        // at 'choiceIndex' in 'choiceArray' can satisfy the record meta-data
        // named 'recordName' in 'schema'.

     bdem_ChoiceBinding(bdem_ChoiceArray  *choiceArray,
                        int                choiceIndex,
                        const bdem_Schema *schema,
                        int                recordIndex);
        // Create a choice binding that binds the modifiable choice array item
        // at the specified 'choiceIndex' in the specified 'choiceArray' to
        // the non-modifiable record definition at the specified 'recordIndex'
        // in the specified 'schema'.  The behavior is undefined
        // unless '0 <= choiceIndex < choiceArray->length()',
        // '0 <= recordIndex < schema->length()', and the item data at
        // 'choiceIndex' in 'choiceArray' can satisfy the record meta-data at
        // 'recordIndex' in 'schema'.

    bdem_ChoiceBinding(const bdem_ChoiceBinding& original);
        // Create a choice binding that binds a modifiable item to a
        // non-modifiable record, both acquired from the specified 'original'
        // choice binding.

    // ~bdem_ChoiceBinding();
        // Destroy this choice binding.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    void rebind(const bdem_ChoiceBinding& binding);
        // Reinitialize this choice binding to bind a newly-acquired modifiable
        // choiceArrayItem to a newly-acquired non-modifiable record, both
        // acquired from the specified choice 'binding'.

    void rebind(bdem_ChoiceArrayItem *item);
        // Reinitialize this choice binding to bind the specified
        // modifiable 'item' to the non-modifiable record already held
        // by this binding.  The behavior is undefined unless the 'item'
        // data can satisfy the record meta-data.

    void rebind(bdem_Choice *choice);
        // Reinitialize this choice binding to bind the specified modifiable
        // 'choice' to the non-modifiable record already held by this binding.
        // The behavior is undefined unless the 'choice' data can satisfy the
        // record meta-data.

    void rebind(bdem_ChoiceArray *choiceArray, int choiceIndex);
        // Reinitialize this choice binding to bind the modifiable item at
        // the specified 'choiceIndex' in the specified 'choiceArray' to the
        // non-modifiable record already held by this binding.  The behavior
        // is undefined unless '0 <= choiceIndex < choiceArray->length()'
        // and the choice data at 'choiceIndex' in 'choiceArray' can satisfy
        // the record meta-data.

    void rebind(bdem_ChoiceArrayItem *item, const bdem_RecordDef *record);
        // Reinitialize this choice binding to bind the specified
        // modifiable 'item' to the specified non-modifiable 'record'.
        // The behavior is undefined unless the 'item' data can satisfy the
        // 'record' meta-data.

    void rebind(bdem_ChoiceArrayItem *item,
                const bdem_Schema    *schema,
                const char           *recordName);
        // Reinitialize this choice binding to bind the specified
        // modifiable 'item' to the non-modifiable record definition
        // having the specified 'recordName' in the specified 'schema'.  The
        // behavior is undefined unless the 'item' data can satisfy the
        // record meta-data named 'recordName' in 'schema'.

    void rebind(bdem_ChoiceArrayItem *item,
                const bdem_Schema    *schema,
                int                   recordIndex);
        // Reinitialize this choice binding to bind the specified
        // modifiable 'item' to the non-modifiable record definition at
        // the specified 'recordIndex' in the specified 'schema'.  The
        // behavior is undefined unless '0 <= recordIndex < schema->length()'
        // and the 'item' data can satisfy the record meta-data at
        // 'recordIndex' in 'schema'.

    void rebind(bdem_Choice *choice, const bdem_RecordDef *record);
        // Reinitialize this choice binding to bind the specified modifiable
        // 'choice' to the specified non-modifiable 'record'.  The behavior is
        // undefined unless the 'choice' data can satisfy the 'record'
        // meta-data.

    void rebind(bdem_Choice       *choice,
                const bdem_Schema *schema,
                const char        *recordName);
        // Reinitialize this choice binding to bind the specified modifiable
        // 'choice' to the non-modifiable record definition having the
        // specified 'recordName' in the specified 'schema'.  The behavior is
        // undefined unless the 'choice' data can satisfy the record meta-data
        // named 'recordName' in 'schema'.

    void rebind(bdem_Choice       *choice,
                const bdem_Schema *schema,
                int                recordIndex);
        // Reinitialize this choice binding to bind the specified modifiable
        // 'choice' to the non-modifiable record definition at the specified
        // 'recordIndex' in the specified 'schema'.  The behavior is undefined
        // unless '0 <= recordIndex < schema->length()' and the 'choice' data
        // can satisfy the record meta-data at 'recordIndex' in 'schema'.

    void rebind(bdem_ChoiceArray     *choiceArray,
                int                   choiceIndex,
                const bdem_RecordDef *record);
        // Reinitialize this choice binding to bind the modifiable choice
        // array item at the specified 'choiceIndex' in the specified
        // 'choiceArray' to the specified non-modifiable 'record'.  The
        // behavior is undefined
        // unless '0 <= choiceIndex < choiceArray->length()' and the item data
        // at 'choiceIndex' in 'choiceArray' can satisfy the 'record'
        // meta-data.

    void rebind(bdem_ChoiceArray  *choiceArray,
                int                choiceIndex,
                const bdem_Schema *schema,
                const char        *recordName);
        // Reinitialize this choice binding to bind the modifiable choice
        // at the specified 'choiceIndex' in the specified 'choiceArray' to the
        // specified non-modifiable 'record'.  The behavior is undefined unless
        // '0 <= choiceIndex < choiceArray->numChoices()' and the choice data
        // at 'choiceIndex' in 'choiceArray' can satisfy the 'record'
        // meta-data.

    void rebind(bdem_ChoiceArray  *choiceArray,
                int                choiceIndex,
                const bdem_Schema *schema,
                int                recordIndex);
        // Reinitialize this choice binding to bind the modifiable choice array
        // item at the specified 'choiceIndex' in the specified 'choiceArray'
        // to the non-modifiable record definition at the specified
        // 'recordIndex' in the specified 'schema'.  The behavior is undefined
        // unless '0 <= choiceIndex < choiceArray->length()',
        // '0 <= recordIndex < schema->length()', and the item data at
        // 'choiceIndex' in 'choiceArray' can satisfy the record meta-data at
        // 'recordIndex' in 'schema'.

    bdem_ElemRef makeSelection(int newSelector);
        // If the specified 'newSelector' does not match 'selector()', destroy
        // the current selection value (if any), construct a value of type
        // indicated by looking up 'newSelector' in the types catalog,
        // initialize the selection value to the corresponding unset value, and
        // return an element reference to the newly-constructed selection.  If
        // 'newSelector == selector()', return an element reference to the
        // current selection.  If 'newSelector == -1', destroy the current
        // selection value, set the choice to its unset state, and return an
        // element reference to void.  The behavior is undefined unless
        // '-1 <= newSelector < numSelections()'.

    // TBD makeSelection using Id
    bdem_ElemRef makeSelection(const char *selectionName);
        // If the specified 'selectionName' does not match 'selectorName()',
        // destroy the current selection value (if any), construct a value of
        // type indicated by looking up 'selectionName' in the types catalog,
        // initialize the selection value to the corresponding unset value, and
        // return an element reference to the newly-constructed selection.  If
        // 'selectionName == selectorName()', return an element reference to
        // the current selection.  The behavior is undefined unless
        // 'selectionName' corresponds to a type in the types catalog of the
        // bound choice.

    int setEnumeration(const bsl::string& enumeratorName) const;
        // Set the value of the enumeration element for the currently selected
        // type in this binding.  If the element is of type
        // 'bdem_ElemType::BDEM_STRING', store the specified 'enumeratorName'
        // in the element.  If the element is of type
        // 'bdem_ElemType::BDEM_INT', look up 'enumeratorName' in the element's
        // enumeration definition and store the corresponding integer
        // enumerator ID in the element.  Return 0 on success, and a non-zero
        // value otherwise.  The behavior is undefined unless
        // '0 <= selector() < numSelections()', the type of the referenced
        // element is 'INT' or 'STRING', and the referenced element is
        // constrained by an enumeration definition.

    int setEnumeration(int enumeratorId) const;
        // Set the value of the enumeration element for the currently selected
        // type in this binding.  If the element is of type
        // 'bdem_ElemType::BDEM_INT', store the specified 'enumeratorId' in the
        // element.  If the element is of type 'bdem_ElemType::BDEM_STRING,
        // look up 'enumeratorId' in the element's enumeration definition and
        // store the corresponding enumerator name in the element.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless unless '0 <= selector() < numSelections()', the type of the
        // referenced element is 'INT' or 'STRING', the referenced element is
        // constrained by an enumeration definition.

    // ACCESSORS
    bdem_ChoiceArrayItem& item() const;
        // Return a reference to the modifiable choice array item held by this
        // binding.

    bdem_ElemRef selection() const;
        // Return an element reference that refers to the modifiable
        // selection stored in this binding.  The behavior is undefined unless
        // the enclosed 'choice' has an element selected.

    bool& theModifiableBool() const;
    char& theModifiableChar() const;
    short& theModifiableShort() const;
    int& theModifiableInt() const;
    bsls_Types::Int64& theModifiableInt64() const;
    float& theModifiableFloat() const;
    double& theModifiableDouble() const;
    bsl::string& theModifiableString() const;
    bdet_Datetime& theModifiableDatetime() const;
    bdet_DatetimeTz& theModifiableDatetimeTz() const;
    bdet_Date& theModifiableDate() const;
    bdet_DateTz& theModifiableDateTz() const;
    bdet_Time& theModifiableTime() const;
    bdet_TimeTz& theModifiableTimeTz() const;
    bsl::vector<bool>& theModifiableBoolArray() const;
    bsl::vector<char>& theModifiableCharArray() const;
    bsl::vector<short>& theModifiableShortArray() const;
    bsl::vector<int>& theModifiableIntArray() const;
    bsl::vector<bsls_Types::Int64>& theModifiableInt64Array() const;
    bsl::vector<float>& theModifiableFloatArray() const;
    bsl::vector<double>& theModifiableDoubleArray() const;
    bsl::vector<bsl::string>& theModifiableStringArray() const;
    bsl::vector<bdet_Datetime>& theModifiableDatetimeArray() const;
    bsl::vector<bdet_DatetimeTz>& theModifiableDatetimeTzArray() const;
    bsl::vector<bdet_Date>& theModifiableDateArray() const;
    bsl::vector<bdet_DateTz>& theModifiableDateTzArray() const;
    bsl::vector<bdet_Time>& theModifiableTimeArray() const;
    bsl::vector<bdet_TimeTz>& theModifiableTimeTzArray() const;
    bdem_Choice& theModifiableChoice() const;
    bdem_ChoiceArray& theModifiableChoiceArray() const;
    bdem_List& theModifiableList() const;
    bdem_Table& theModifiableTable() const;
        // Return a reference to the modifiable selection of this binding.  If
        // the element is null, it is made non-null before returning, but its
        // value is not otherwise modified.  The behavior is undefined unless
        // the type of the selection matches the return type of the method
        // used.  Note that unless there is an intention of modifying the
        // element, the corresponding 'theTYPE' method should be used instead.

    bdem_ChoiceBinding choiceBinding() const;
        // Return a choice binding that binds the modifiable choice array item
        // selected by the 'item' stored in this binding to the non-modifiable
        // constraint record stored in the 'record' held by this binding at
        // the index specified by the selector value of 'item'.  The behavior
        // is undefined unless '0 <= selector() < numSelections()', the indexed
        // record constrains a 'bdem_Choice' and the choice array item
        // satisfies the indexed record.

    bdem_ChoiceArrayBinding choiceArrayBinding() const;
        // Return a choice array binding that binds the modifiable
        // choice array selected by the 'item' stored in this binding to the
        // non-modifiable constraint record stored in the 'record' held by
        // this binding at the index specified by the selector value of 'item'.
        // The behavior is undefined unless
        // '0 <= selector() < numSelections()', the indexed record constrains
        // a 'bdem_ChoiceArray' and the choice array satisfies the indexed
        // record.

    bdem_RowBinding rowBinding() const;
        // Return a row binding that binds the modifiable row selected by the
        // 'item' stored in this binding to the non-modifiable constraint
        // record stored in the 'record' held by this binding at the index
        // specified by the selector value of 'item'.  The behavior is
        // undefined unless '0 <= selector() < numSelections()', the indexed
        // record constrains a 'bdem_List' and the row satisfies the indexed
        // record.

    bdem_TableBinding tableBinding() const;
        // Return a table binding that binds the modifiable table
        // selected by the 'item' stored in this binding to the
        // non-modifiable constraint record stored in the 'record' held by
        // this binding at the index specified by the selector value of 'item'.
        // The behavior is undefined unless
        // '0 <= selector() < numSelections()', the indexed record constrains a
        // 'bdem_Table' and the table satisfies the indexed record.
};

                        // ==================================
                        // class bdem_ConstChoiceArrayBinding
                        // ==================================

class bdem_ConstChoiceArrayBinding {
    // This class binds a non-modifiable 'bdem_ChoiceArray' ("choice array")
    // to a non-modifiable 'bdem_RecordDef' ("record"), and provides access to
    // the 'choice array item's in the choice array.  This class also provides
    // by-name and by-index access to the selections of the stored 'choice
    // array item's of the underlying choice array.  For correct by-name
    // access to selections in the stored choice array item, each choice array
    // item data must satisfy the record meta-data.
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // although copy construction and equality are supported.  Except for the
    // constructors, destructor, and the overloaded 'rebind' methods, all
    // instance methods of this object are 'const' and apply not to this
    // object, but rather to the bound choice array or the bound meta-data.
    // Note also that, unlike the derived 'bdem_ChoiceArrayBinding' class
    // (also defined within this component), none of the methods within
    // this base class enable modifiable access to the bound choice array,
    // thus ensuring 'const' correctness.

  public:
    // TYPES
    typedef bdem_ConstElemRef             ElemRef;
    typedef bdem_ConstChoiceBinding       ChoiceBinding;
    typedef bdem_ConstChoiceArrayBinding  ChoiceArrayBinding;
    typedef bdem_ConstRowBinding          RowBinding;
    typedef bdem_ConstTableBinding        TableBinding;
    typedef const bdem_ChoiceArrayItem    Item;
    typedef const bdem_Choice             Choice;
    typedef const bdem_ChoiceArray        ChoiceArray;
    typedef const bdem_Row                Row;
    typedef const bdem_List               List;
    typedef const bdem_Table              Table;

  private:
    // DATA
    const bdem_ChoiceArray *d_choiceArray_p;  // data (held, not owned)
    const bdem_RecordDef   *d_constraint_p;   // meta-data (held, not owned)

    // FRIENDS
    friend bool operator==(const bdem_ConstChoiceArrayBinding& lhs,
                           const bdem_ConstChoiceArrayBinding& rhs);

    friend class bdem_ChoiceArrayBinding;
        // Allow the derived class to access the private members of this
        // class.

  private:
    // NOT IMPLEMENTED
    bdem_ConstChoiceArrayBinding& operator=(
                                          const bdem_ConstChoiceArrayBinding&);

  public:
    // CREATORS
    bdem_ConstChoiceArrayBinding(const bdem_ChoiceArray *choiceArray,
                                 const bdem_RecordDef   *record);
        // Create a "const" choice array binding that binds the specified
        // non-modifiable 'choice array' to the specified non-modifiable
        // 'record'.  The behavior is undefined unless the 'choice array' data
        // can satisfy the 'record' meta-data.

    bdem_ConstChoiceArrayBinding(const bdem_ChoiceArray *choiceArray,
                                 const bdem_Schema      *schema,
                                 const char             *recordName);
        // Create a "const" choice array binding that binds the specified
        // non-modifiable 'choice array' to the non-modifiable record
        // definition having the specified 'recordName' in the specified
        // 'schema'.  The behavior is undefined unless the 'choice array' data
        // can satisfy the record meta-data named 'recordName' in 'schema'.

    bdem_ConstChoiceArrayBinding(const bdem_ChoiceArray *choiceArray,
                                 const bdem_Schema      *schema,
                                 int                     recordIndex);
        // Create a "const" choice array binding that binds the specified
        // non-modifiable 'choice array' to the non-modifiable record
        // definition at the specified 'recordIndex' in the specified
        // 'schema'.  The behavior is undefined
        // unless '0 <= recordIndex < schema->length()' and the 'choice array'
        // data can satisfy the record meta-data at 'recordIndex' in 'schema'.

    bdem_ConstChoiceArrayBinding(
                                const bdem_ConstChoiceArrayBinding& original);
        // Create a "const" choice array binding that binds a non-modifiable
        // choice array to a non-modifiable record, both acquired from the
        // specified 'original' choice array binding.

    // ~bdem_ConstChoiceArrayBinding();
        // Destroy this 'const' choice array binding.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    void rebind(const bdem_ConstChoiceArrayBinding& binding);
        // Reinitialize this choice array binding to bind a newly-acquired
        // non-modifiable choice array to a newly-acquired non-modifiable
        // record, both acquired from the specified choice array 'binding'.

    void rebind(const bdem_ChoiceArray *choiceArray);
        // Reinitialize this choice array binding to bind the specified
        // non-modifiable 'choice array' to the non-modifiable record already
        // held by this binding.  The behavior is undefined unless the 'choice
        // array' data can satisfy the record meta-data.

    void rebind(const bdem_ChoiceArray *choiceArray,
                const bdem_RecordDef   *record);
        // Reinitialize this choice array binding to bind the specified
        // non-modifiable 'choice array' to the specified non-modifiable
        // 'record'.  The behavior is undefined unless the 'choice array' data
        // can satisfy the 'record' meta-data.

    void rebind(const bdem_ChoiceArray *choiceArray,
                const bdem_Schema      *schema,
                const char             *recordName);
        // Reinitialize this choice array binding to bind the specified
        // non-modifiable 'choice array' to the non-modifiable record
        // definition having the specified 'recordName' in the specified
        // 'schema'.  The behavior is undefined unless the 'choice array' data
        // can satisfy the record meta-data named 'recordName' in 'schema'.

    void rebind(const bdem_ChoiceArray *choiceArray,
                const bdem_Schema      *schema,
                int                     recordIndex);
        // Reinitialize this choice array binding to bind the specified
        // non-modifiable 'choice array' to the non-modifiable record
        // definition at the specified 'recordIndex' in the specified
        // 'schema'.  The behavior is undefined unless
        // '0 <= recordIndex < schema->length()' and the 'choice array' data
        // can satisfy the record meta-data at 'recordIndex' in 'schema'.

    // ACCESSORS
    const bdem_ChoiceArrayItem& operator[](int itemIndex) const;
        // Return a reference to the non-modifiable choice array item at the
        // specified 'itemIndex' in this binding.  The behavior is undefined
        // unless '0 <= itemIndex < length()'.

    const bdem_ChoiceArray& choiceArray() const;
        // Return a reference to the non-modifiable choice array held by this
        // binding.

    const bdem_RecordDef& record() const;
        // Return a reference to the non-modifiable record held by this
        // binding.

    int length() const;
        // Return the number of items in this binding.

    int numSelections() const;
        // Return the number of fields in the record held by this binding.

    bdem_ElemType::Type selectionType(int selectionIndex) const;
        // Return the type enumeration at position 'selectionIndex' of the
        // 'record' stored in this binding.  The behavior is undefined
        // unless '0 <= selectionIndex < numSelections()'.

    bdem_ElemType::Type selectionType(const char *elementName) const;
        // Return the 'bdem_ElemType::Type' value of the element having the
        // specified 'elementName' in this binding.  The behavior is undefined
        // unless this binding stores a field named 'elementName' in its
        // 'record'.

    bdem_ElemType::Type selectorType(int itemIndex) const;
        // Return the type enumeration of the item selected at position
        // 'itemIndex' of this binding.  The behavior is undefined
        // unless '0 <= itemIndex < length()'.

    bdem_ConstElemRef selection(int itemIndex) const;
        // Return a "const" element reference that refers to the non-modifiable
        // selection at the specified 'itemIndex' in this binding.  The
        // behavior is undefined unless '0 <= itemIndex < length()'.

    int selector(int itemIndex) const;
        // Return the current selector (i.e., the index of the currently
        // selected type within the types catalog) of the choice array item
        // stored at the specified 'itemIndex'.

    int selectorId(int itemIndex) const;
        // Return the id associated with the current selector of the stored
        // item at 'itemIndex' (i.e., the field id for they selected type
        // within the 'record' associated with this binding).  Return
        // 'bdem_RecordDef::BDEM_NULL_FIELD_ID' if no element is currently
        // selected.

    const char *selectorName(int itemIndex) const;
        // Return the name of the current selection of the choice array item
        // stored at the specified 'itemIndex' as specified in the 'record'
        // stored in this binding or 0 if not element is selected.  The
        // behavior is undefined unless '0 <= itemIndex < length()'.

    const bool& theBool(int itemIndex) const;
    const char& theChar(int itemIndex) const;
    const short& theShort(int itemIndex) const;
    const int& theInt(int itemIndex) const;
    const bsls_Types::Int64& theInt64(int itemIndex) const;
    const float& theFloat(int itemIndex) const;
    const double& theDouble(int itemIndex) const;
    const bsl::string& theString(int itemIndex) const;
    const bdet_Datetime& theDatetime(int itemIndex) const;
    const bdet_DatetimeTz& theDatetimeTz(int itemIndex) const;
    const bdet_Date& theDate(int itemIndex) const;
    const bdet_DateTz& theDateTz(int itemIndex) const;
    const bdet_Time& theTime(int itemIndex) const;
    const bdet_TimeTz& theTimeTz(int itemIndex) const;
    const bsl::vector<bool>& theBoolArray(int itemIndex) const;
    const bsl::vector<char>& theCharArray(int itemIndex) const;
    const bsl::vector<short>& theShortArray(int itemIndex) const;
    const bsl::vector<int>& theIntArray(int itemIndex) const;
    const bsl::vector<bsls_Types::Int64>& theInt64Array(int itemIndex) const;
    const bsl::vector<float>& theFloatArray(int itemIndex) const;
    const bsl::vector<double>& theDoubleArray(int itemIndex) const;
    const bsl::vector<bsl::string>& theStringArray(int itemIndex) const;
    const bsl::vector<bdet_Datetime>& theDatetimeArray(int itemIndex) const;
    const bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray(
                                                          int itemIndex) const;
    const bsl::vector<bdet_Date>& theDateArray(int itemIndex) const;
    const bsl::vector<bdet_DateTz>& theDateTzArray(int itemIndex) const;
    const bsl::vector<bdet_Time>& theTimeArray(int itemIndex) const;
    const bsl::vector<bdet_TimeTz>& theTimeTzArray(int itemIndex) const;
    const bdem_Choice& theChoice(int itemIndex) const;
    const bdem_ChoiceArray& theChoiceArray(int itemIndex) const;
    const bdem_List& theList(int itemIndex) const;
    const bdem_Table& theTable(int itemIndex) const;
        // Return a reference to the non-modifiable selection at the specified
        // 'itemIndex' in this binding.  The nullness of the element is not
        // affected.  The behavior is undefined unless
        // '0 <= itemIndex < length()' and the type of the indexed selection
        // matches the return type of the method used.

    bdem_ConstChoiceBinding choiceBinding(int itemIndex) const;
        // Return a "const" choice binding that binds the non-modifiable
        // choice array item stored at the specified 'index' in this binding
        // to the non-modifiable constraint record of this binding.  The
        // behavior is undefined unless '0 <= index < length()'.

    bdem_ConstChoiceArrayBinding choiceArrayBinding(int itemIndex) const;
        // Return a "const" choice array binding that binds the non-modifiable
        // choice array selected by the choice array item stored in this
        // binding at the specified 'itemIndex' to the non-modifiable
        // constraint record at the index specified by the selector value of
        // that choice array item.  The behavior is undefined
        // unless '0 <= selector(itemIndex) < numSelections()' and the indexed
        // record constrains a 'bdem_Choice'.

    bdem_ConstRowBinding rowBinding(int itemIndex) const;
        // Return a "const" row binding that binds the non-modifiable
        // row selected by the choice array item stored in this binding at
        // the specified 'itemIndex' to the non-modifiable constraint record
        // at the index specified by the selector value of that choice array
        // item.  The behavior is undefined
        // unless '0 <= selector(itemIndex) < numSelections()' and the indexed
        // record constrains a 'bdem_List'.

    bdem_ConstTableBinding tableBinding(int itemIndex) const;
        // Return a "const" table binding that binds the non-modifiable
        // table selected by the choice array item stored in this binding at
        // the specified 'itemIndex' to the non-modifiable constraint record
        // at the index specified by the selector value of that choice array
        // item.  The behavior is undefined unless
        // '0 <= selector(itemIndex) < numSelections()' and the indexed
        // record constrains a 'bdem_Table'.

    const char *enumerationAsString(int itemIndex) const;
        // Return the string representation of the enumeration element at the
        // specified 'itemIndex' in this binding.  Return 0 if the element has
        // a null or unset value.  The returned pointer is valid until the
        // bound schema is destroyed.  The behavior is undefined unless
        // '0 <= itemIndex < length()', the type of the indexed element is
        // 'INT' or 'STRING', and the element is constrained by an enumeration
        // definition.

    int enumerationAsInt(int itemIndex) const;
        // Return the integer ID of the enumeration element at the specified
        // 'itemIndex' in this binding.  Return
        // 'bdetu_Unset<int>::unsetValue()' if the element has an unset value.
        // The behavior is undefined unless '0 <= itemIndex < length()', the
        // type of the indexed element is 'INT' or 'STRING', and the element is
        // constrained by an enumeration definition.
};

                        // =============================
                        // class bdem_ChoiceArrayBinding
                        // =============================

class bdem_ChoiceArrayBinding : public bdem_ConstChoiceArrayBinding {
    // This class binds a modifiable 'bdem_ChoiceArray' ("choice array")
    // to a non-modifiable 'bdem_RecordDef' ("record"), and provides access to
    // the 'choice array item's in the choice array.  This class also provides
    // by-name and by-index access to the selections of the stored 'choice
    // array item's of the underlying choice array.  For correct by-name
    // access to selections in the stored choice array item, each choice array
    // item data must satisfy the record meta-data.
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // although copy construction and equality are supported.  Except for the
    // constructors, destructor, and the overloaded 'rebind' methods, all
    // instance methods of this object are 'const' and apply not to this
    // object, but rather to the bound choice array or the bound meta-data.

  public:
    // TYPES
    typedef bdem_ElemRef             ElemRef;
    typedef bdem_ChoiceBinding       ChoiceBinding;
    typedef bdem_ChoiceArrayBinding  ChoiceArrayBinding;
    typedef bdem_RowBinding          RowBinding;
    typedef bdem_TableBinding        TableBinding;
    typedef bdem_ChoiceArrayItem     Item;
    typedef bdem_Choice              Choice;
    typedef bdem_ChoiceArray         ChoiceArray;
    typedef bdem_Row                 Row;
    typedef bdem_List                List;
    typedef bdem_Table               Table;

  private:
    // Note: all data resides in the base class.

    // NOT IMPLEMENTED
    bdem_ChoiceArrayBinding& operator=(const bdem_ChoiceArrayBinding&);

  public:
    // CREATORS
    bdem_ChoiceArrayBinding(bdem_ChoiceArray     *choiceArray,
                            const bdem_RecordDef *record);
        // Create a choice array binding that binds the specified modifiable
        // 'choice array' to the specified non-modifiable 'record'.  The
        // behavior is undefined unless the 'choice array' data can satisfy
        // the 'record' meta-data.

    bdem_ChoiceArrayBinding(bdem_ChoiceArray  *choiceArray,
                            const bdem_Schema *schema,
                            const char        *recordName);
        // Create a choice array binding that binds the specified modifiable
        // 'choice array' to the non-modifiable record definition having the
        // specified 'recordName' in the specified 'schema'.  The behavior is
        // undefined unless the 'choice array' data can satisfy the record
        // meta-data named 'recordName' in 'schema'.

    bdem_ChoiceArrayBinding(bdem_ChoiceArray  *choiceArray,
                            const bdem_Schema *schema,
                            int                recordIndex);
        // Create a choice array binding that binds the specified modifiable
        // 'choice array' to the non-modifiable record definition at the
        // specified 'recordIndex' in the specified 'schema'.  The behavior is
        // undefined unless 0 '<= recordIndex < schema->length()' and the
        // 'choice array' data can satisfy the record meta-data at
        // 'recordIndex' in 'schema'.

    bdem_ChoiceArrayBinding(const bdem_ChoiceArrayBinding& original);
        // Create a choice array binding that binds a modifiable choice array
        // to a non-modifiable record, both acquired from the specified
        // 'original' choice array binding.

    // ~bdem_ChoiceArrayBinding();
        // Destroy this choice array binding.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    void rebind(const bdem_ChoiceArrayBinding& binding);
        // Reinitialize this choice array binding to bind a newly-acquired
        // modifiable choice array to a newly-acquired non-modifiable record,
        // both acquired from the specified choice array 'binding'.

    void rebind(bdem_ChoiceArray *choiceArray);
        // Reinitialize this choice array binding to bind the specified
        // modifiable 'choice array' to the non-modifiable record already held
        // by this binding.  The behavior is undefined unless the 'choice
        // array' data can satisfy the record meta-data.

    void rebind(bdem_ChoiceArray *choiceArray, const bdem_RecordDef *record);
        // Reinitialize this choice array binding to bind the specified
        // modifiable 'choice array' to the specified non-modifiable 'record'.
        // The behavior is undefined unless the 'choice array' data can
        // satisfy the 'record' meta-data.

    void rebind(bdem_ChoiceArray  *choiceArray,
                const bdem_Schema *schema,
                const char        *recordName);
        // Reinitialize this choice array binding to bind the specified
        // modifiable 'choice array' to the non-modifiable record definition
        // having the specified 'recordName' in the specified 'schema'.  The
        // behavior is undefined unless the 'choice array' data can satisfy
        // the record meta-data named 'recordName' in 'schema'.

    void rebind(bdem_ChoiceArray  *choiceArray,
                const bdem_Schema *schema,
                int                recordIndex);
        // Reinitialize this choice array binding to bind the specified
        // modifiable 'choice array' to the non-modifiable record definition
        // at the specified 'recordIndex' in the specified 'schema'.  The
        // behavior is undefined unless '0 <= recordIndex < schema->length()'
        // and the 'choice array' data can satisfy the record meta-data at
        // 'recordIndex' in 'schema'.

    bdem_ElemRef makeSelection(int itemIndex, int newSelector);
        // For the choice array item stored at the specified 'itemIndex', if
        // the specified 'newSelector' does not match 'selector()', destroy the
        // current selection value (if any), construct a value of type
        // indicated by looking up 'newSelector' in the types catalog,
        // initialize the selection value to the corresponding unset value, and
        // return an element reference to the newly-constructed selection.  If
        // 'newSelector == selector()', return an element reference to the
        // current selection.  If 'newSelector == -1', destroy the current
        // selection value, set the choice to its unset state, and return an
        // element reference to void.  The behavior is undefined unless
        // '-1 <= newSelector < numSelections()' and
        // '0 <= itemIndex < length()'.

    // TBD makeSelection using Id
    bdem_ElemRef makeSelection(int itemIndex, const char *selectionName);
        // For the choice array item stored at the specified 'itemIndex', if
        // the specified 'selectionName' does not match 'selectorName()',
        // destroy the current selection value (if any), construct a value of
        // type indicated by looking up 'selectionName' in the types catalog,
        // initialize the selection value to the corresponding unset value, and
        // return an element reference to the newly-constructed selection.  If
        // 'selectionName == selectorName()', return an element reference to
        // the current selection.  If 'selectionName == -1', destroy the
        // current selection value, set this choice to its unset state, and
        // return an element reference to void.  The behavior is undefined
        // unless '0 <= itemIndex < length()' and 'selectionName' corresponds
        // to a type in the types catalog of the bound choice array.

    int setEnumeration(int itemIndex, const bsl::string& enumeratorName) const;
        // Set the value of the enumeration element at the specified
        // 'itemIndex' in this binding.  If the element is of type
        // 'bdem_ElemType::BDEM_STRING', store the specified 'enumeratorName'
        // in the element.  If the element is of type
        // 'bdem_ElemType::BDEM_INT', look up 'enumeratorName' in the element's
        // enumeration definition and store the corresponding integer
        // enumerator ID in the element.  Return 0 on success, and a non-zero
        // value otherwise.  The behavior is undefined unless
        // '0 <= itemIndex < length()', the type of the referenced element is
        // 'INT' or 'STRING', and the referenced element is constrained by an
        // enumeration definition.

    int setEnumeration(int itemIndex, int enumeratorId) const;
        // Set the value of the enumeration element at the specified
        // 'itemIndex' in this binding.  If the element is of type
        // 'bdem_ElemType::BDEM_INT', store the specified 'enumeratorId' in the
        // element.  If the element is of type 'bdem_ElemType::BDEM_STRING,
        // look up 'enumeratorId' in the element's enumeration definition and
        // store the corresponding enumerator name in the element.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless '0 <= itemIndex < length()', the type of the referenced
        // element is 'INT' or 'STRING', and the referenced element is
        // constrained by an enumeration definition.

    // ACCESSORS
    bdem_ChoiceArrayItem& operator[](int itemIndex) const;
        // Return a reference to the modifiable choice array item at the
        // specified 'itemIndex' in this binding.  The behavior is undefined
        // unless '0 <= itemIndex < length()'.
        //
        // DEPRECATED: use 'theModifiableItem(int itemIndex)' instead.

    bdem_ChoiceArray& choiceArray() const;
        // Return a reference to the modifiable choice array held by this
        // binding.

    bdem_ElemRef selection(int itemIndex) const;
        // Return an element reference that refers to the modifiable selection
        // at the specified 'itemIndex' in this binding.  The behavior is
        // undefined unless '0 <= itemIndex < length()'.

    bdem_ChoiceArrayItem& theModifiableItem(int itemIndex) const;
        // Return a reference to the modifiable choice array item at the
        // specified 'itemIndex' in this binding.  If the referenced item is
        // null, it is made non-null before returning, but its value is not
        // otherwise modified.  The behavior is undefined unless
        // '0 <= itemIndex < length()'.

    bool& theModifiableBool(int itemIndex) const;
    char& theModifiableChar(int itemIndex) const;
    short& theModifiableShort(int itemIndex) const;
    int& theModifiableInt(int itemIndex) const;
    bsls_Types::Int64& theModifiableInt64(int itemIndex) const;
    float& theModifiableFloat(int itemIndex) const;
    double& theModifiableDouble(int itemIndex) const;
    bsl::string& theModifiableString(int itemIndex) const;
    bdet_Datetime& theModifiableDatetime(int itemIndex) const;
    bdet_DatetimeTz& theModifiableDatetimeTz(int itemIndex) const;
    bdet_Date& theModifiableDate(int itemIndex) const;
    bdet_DateTz& theModifiableDateTz(int itemIndex) const;
    bdet_Time& theModifiableTime(int itemIndex) const;
    bdet_TimeTz& theModifiableTimeTz(int itemIndex) const;
    bsl::vector<bool>& theModifiableBoolArray(int itemIndex) const;
    bsl::vector<char>& theModifiableCharArray(int itemIndex) const;
    bsl::vector<short>& theModifiableShortArray(int itemIndex) const;
    bsl::vector<int>& theModifiableIntArray(int itemIndex) const;
    bsl::vector<bsls_Types::Int64>& theModifiableInt64Array(
                                                          int itemIndex) const;
    bsl::vector<float>& theModifiableFloatArray(int itemIndex) const;
    bsl::vector<double>& theModifiableDoubleArray(int itemIndex) const;
    bsl::vector<bsl::string>& theModifiableStringArray(int itemIndex) const;
    bsl::vector<bdet_Datetime>& theModifiableDatetimeArray(
                                                          int itemIndex) const;
    bsl::vector<bdet_DatetimeTz>& theModifiableDatetimeTzArray(
                                                          int itemIndex) const;
    bsl::vector<bdet_Date>& theModifiableDateArray(int itemIndex) const;
    bsl::vector<bdet_DateTz>& theModifiableDateTzArray(int itemIndex) const;
    bsl::vector<bdet_Time>& theModifiableTimeArray(int itemIndex) const;
    bsl::vector<bdet_TimeTz>& theModifiableTimeTzArray(int itemIndex) const;
    bdem_Choice& theModifiableChoice(int itemIndex) const;
    bdem_ChoiceArray& theModifiableChoiceArray(int itemIndex) const;
    bdem_List& theModifiableList(int itemIndex) const;
    bdem_Table& theModifiableTable(int itemIndex) const;
        // Return a reference to the modifiable selection at the specified
        // 'itemIndex' in this binding.  If the element is null, it is made
        // non-null before returning, but its value is not otherwise modified.
        // The behavior is undefined unless '0 <= itemIndex < length()' and the
        // type of the indexed selection matches the return type of the method
        // used.  Note that unless there is an intention of modifying the
        // element, the corresponding 'theTYPE' method should be used instead.

    bdem_ChoiceBinding choiceBinding(int index) const;
        // Return a choice binding that binds the modifiable choice array item
        // stored at the specified 'index' in this binding to the
        // non-modifiable constraint record of this binding.  The behavior is
        // undefined unless '0 <= index < length()'.

    bdem_ChoiceArrayBinding choiceArrayBinding(int itemIndex) const;
        // Return a choice array binding that binds the modifiable choice
        // array selected by the choice array item stored in this binding at
        // the specified 'itemIndex' to the modifiable constraint record at
        // the index specified by the selector value of that choice array
        // item.  The behavior is undefined
        // unless '0 <= selector(itemIndex) < numSelections()' and the indexed
        // record constrains a 'bdem_Choice'.

    bdem_RowBinding rowBinding(int itemIndex) const;
        // Return a row binding that binds the modifiable row selected by the
        // choice array item stored in this binding at the specified
        // 'itemIndex' to the modifiable constraint record at the index
        // specified by the selector value of that choice array item.  The
        // behavior is undefined
        // unless '0 <= selector(itemIndex) < numSelections()' and the indexed
        // record constrains a 'bdem_List'.

    bdem_TableBinding tableBinding(int itemIndex) const;
        // Return a table binding that binds the modifiable table selected by
        // the choice array item stored in this binding at the specified
        // 'itemIndex' to the modifiable constraint record at the index
        // specified by the selector value of that choice array item.  The
        // behavior is undefined
        // unless '0 <= selector(itemIndex) < numSelections()' and the indexed
        // record constrains a 'bdem_Table'.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------------------
                        // class bdem_ConstRowBinding
                        // --------------------------

// CREATORS
inline
bdem_ConstRowBinding::bdem_ConstRowBinding(const bdem_Row       *row,
                                           const bdem_RecordDef *record)
: d_row_p(row)
, d_constraint_p(record)
{
    BSLS_ASSERT_SAFE(row);
    BSLS_ASSERT_SAFE(record);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(*row,
                                                                *record));
}

inline
bdem_ConstRowBinding::bdem_ConstRowBinding(const bdem_Row    *row,
                                           const bdem_Schema *schema,
                                           const char        *recordName)
: d_row_p(row)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(row);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *row,
                                                             *d_constraint_p));
}

inline
bdem_ConstRowBinding::bdem_ConstRowBinding(const bdem_Row    *row,
                                           const bdem_Schema *schema,
                                           int                recordIndex)
: d_row_p(row)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(row);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->length());

    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                 *row,
                                                 schema->record(recordIndex)));
}

inline
bdem_ConstRowBinding::bdem_ConstRowBinding(const bdem_List      *list,
                                           const bdem_RecordDef *record)
: d_row_p(0)
, d_constraint_p(record)
{
    BSLS_ASSERT_SAFE(list);
    BSLS_ASSERT_SAFE(record);

    d_row_p = &list->row();

    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(*d_row_p,
                                                                *record));
}

inline
bdem_ConstRowBinding::bdem_ConstRowBinding(const bdem_List   *list,
                                           const bdem_Schema *schema,
                                           const char        *recordName)
: d_row_p(0)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(list);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_row_p        = &list->row();
    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_row_p,
                                                             *d_constraint_p));
}

inline
bdem_ConstRowBinding::bdem_ConstRowBinding(const bdem_List   *list,
                                           const bdem_Schema *schema,
                                           int                recordIndex)
: d_row_p(0)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(list);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->length());

    d_row_p        = &list->row();
    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_row_p,
                                                             *d_constraint_p));
}

inline
bdem_ConstRowBinding::bdem_ConstRowBinding(const bdem_Table     *table,
                                           int                   rowIndex,
                                           const bdem_RecordDef *record)
: d_row_p(0)
, d_constraint_p(record)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < table->numRows());
    BSLS_ASSERT_SAFE(record);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(*table,
                                                                *record));

    d_row_p = &(*table)[rowIndex];
}

inline
bdem_ConstRowBinding::bdem_ConstRowBinding(const bdem_Table  *table,
                                           int                rowIndex,
                                           const bdem_Schema *schema,
                                           const char        *recordName)
: d_row_p(0)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < table->numRows());
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_row_p        = &(*table)[rowIndex];
    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *table,
                                                             *d_constraint_p));
}

inline
bdem_ConstRowBinding::bdem_ConstRowBinding(const bdem_Table  *table,
                                           int                rowIndex,
                                           const bdem_Schema *schema,
                                           int                recordIndex)
: d_row_p(0)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex    < table->numRows());
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->length());

    d_row_p        = &(*table)[rowIndex];
    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *table,
                                                             *d_constraint_p));
}

inline
bdem_ConstRowBinding::bdem_ConstRowBinding(
                                          const bdem_ConstRowBinding& original)
: d_row_p(original.d_row_p)
, d_constraint_p(original.d_constraint_p)
{
}

// MANIPULATORS
inline
void bdem_ConstRowBinding::rebind(const bdem_ConstRowBinding& binding)
{
    d_row_p        = binding.d_row_p;
    d_constraint_p = binding.d_constraint_p;
}

inline
void bdem_ConstRowBinding::rebind(const bdem_Row *row)
{
    BSLS_ASSERT_SAFE(row);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *row,
                                                             *d_constraint_p));

    d_row_p = row;
}

inline
void bdem_ConstRowBinding::rebind(const bdem_List *list)
{
    BSLS_ASSERT_SAFE(list);

    d_row_p = &list->row();

    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_row_p,
                                                             *d_constraint_p));
}

inline
void bdem_ConstRowBinding::rebind(const bdem_Table *table, int rowIndex)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < table->numRows());
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *table,
                                                             *d_constraint_p));

    d_row_p = &(*table)[rowIndex];
}

inline
void bdem_ConstRowBinding::rebind(const bdem_Row       *row,
                                  const bdem_RecordDef *record)
{
    BSLS_ASSERT_SAFE(row);
    BSLS_ASSERT_SAFE(record);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(*row,
                                                                *record));

    d_row_p        = row;
    d_constraint_p = record;
}

inline
void bdem_ConstRowBinding::rebind(const bdem_Row    *row,
                                  const bdem_Schema *schema,
                                  const char        *recordName)
{
    BSLS_ASSERT_SAFE(row);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_row_p        = row;
    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_row_p,
                                                             *d_constraint_p));
}

inline
void bdem_ConstRowBinding::rebind(const bdem_Row    *row,
                                  const bdem_Schema *schema,
                                  int                recordIndex)
{
    BSLS_ASSERT_SAFE(row);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->length());

    d_row_p        = row;
    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_row_p,
                                                             *d_constraint_p));
}

inline
void bdem_ConstRowBinding::rebind(const bdem_List      *list,
                                  const bdem_RecordDef *record)
{
    BSLS_ASSERT_SAFE(list);
    BSLS_ASSERT_SAFE(record);

    d_row_p        = &list->row();
    d_constraint_p = record;

    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(*d_row_p,
                                                                *record));
}

inline
void bdem_ConstRowBinding::rebind(const bdem_List   *list,
                                  const bdem_Schema *schema,
                                  const char        *recordName)
{
    BSLS_ASSERT_SAFE(list);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_row_p        = &list->row();
    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_row_p,
                                                             *d_constraint_p));
}

inline
void bdem_ConstRowBinding::rebind(const bdem_List   *list,
                                  const bdem_Schema *schema,
                                  int                recordIndex)
{
    BSLS_ASSERT_SAFE(list);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->length());

    d_row_p        = &list->row();
    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_row_p,
                                                             *d_constraint_p));
}

inline
void bdem_ConstRowBinding::rebind(const bdem_Table     *table,
                                  int                   rowIndex,
                                  const bdem_RecordDef *record)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(record);
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < table->numRows());

    d_row_p        = &(*table)[rowIndex];
    d_constraint_p = record;

    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(*table,
                                                                *record));
}

inline
void bdem_ConstRowBinding::rebind(const bdem_Table  *table,
                                  int                rowIndex,
                                  const bdem_Schema *schema,
                                  const char        *recordName)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < table->numRows());
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_row_p        = &(*table)[rowIndex];
    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *table,
                                                             *d_constraint_p));
}

inline
void bdem_ConstRowBinding::rebind(const bdem_Table  *table,
                                  int                rowIndex,
                                  const bdem_Schema *schema,
                                  int                recordIndex)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex    < table->numRows());
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->length());

    d_row_p        = &(*table)[rowIndex];
    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *table,
                                                             *d_constraint_p));
}

// ACCESSORS
inline
bdem_ConstElemRef bdem_ConstRowBinding::operator[](int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_row_p->length());

    return (*d_row_p)[index];
}

inline
bdem_ConstElemRef bdem_ConstRowBinding::element(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return (*d_row_p)[fieldIndex];
}

inline
bdem_ConstElemRef bdem_ConstRowBinding::element(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_row_p->length());

    return (*d_row_p)[index];
}

inline
bdem_ElemType::Type bdem_ConstRowBinding::elemType(
                                                 const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return d_row_p->elemType(fieldIndex);
}

inline
bdem_ElemType::Type bdem_ConstRowBinding::elemType(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_row_p->length());

    return d_row_p->elemType(index);
}

inline
int bdem_ConstRowBinding::length() const
{
    return d_constraint_p->numFields();
}

inline
const bdem_RecordDef& bdem_ConstRowBinding::record() const
{
    return *d_constraint_p;
}

inline
const bdem_Row& bdem_ConstRowBinding::row() const
{
    return *d_row_p;
}

inline
const bool& bdem_ConstRowBinding::theBool(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL == elemType(fieldIndex));

    return d_row_p->theBool(fieldIndex);
}

inline
const char& bdem_ConstRowBinding::theChar(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR == elemType(fieldIndex));

    return d_row_p->theChar(fieldIndex);
}

inline
const short& bdem_ConstRowBinding::theShort(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT == elemType(fieldIndex));

    return d_row_p->theShort(fieldIndex);
}

inline
const int& bdem_ConstRowBinding::theInt(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT == elemType(fieldIndex));

    return d_row_p->theInt(fieldIndex);
}

inline
const bsls_Types::Int64&
bdem_ConstRowBinding::theInt64(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64 == elemType(fieldIndex));

    return d_row_p->theInt64(fieldIndex);
}

inline
const float& bdem_ConstRowBinding::theFloat(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT == elemType(fieldIndex));

    return d_row_p->theFloat(fieldIndex);
}

inline
const double& bdem_ConstRowBinding::theDouble(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE == elemType(fieldIndex));

    return d_row_p->theDouble(fieldIndex);
}

inline
const bsl::string&
bdem_ConstRowBinding::theString(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING == elemType(fieldIndex));

    return d_row_p->theString(fieldIndex);
}

inline
const bdet_Datetime&
bdem_ConstRowBinding::theDatetime(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIME == elemType(fieldIndex));

    return d_row_p->theDatetime(fieldIndex);
}

inline
const bdet_DatetimeTz&
bdem_ConstRowBinding::theDatetimeTz(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIMETZ == elemType(fieldIndex));

    return d_row_p->theDatetimeTz(fieldIndex);
}

inline
const bdet_Date& bdem_ConstRowBinding::theDate(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE == elemType(fieldIndex));

    return d_row_p->theDate(fieldIndex);
}

inline
const bdet_DateTz&
bdem_ConstRowBinding::theDateTz(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ == elemType(fieldIndex));

    return d_row_p->theDateTz(fieldIndex);
}

inline
const bdet_Time& bdem_ConstRowBinding::theTime(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME == elemType(fieldIndex));

    return d_row_p->theTime(fieldIndex);
}

inline
const bdet_TimeTz&
bdem_ConstRowBinding::theTimeTz(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ == elemType(fieldIndex));

    return d_row_p->theTimeTz(fieldIndex);
}

inline
const bsl::vector<bool>&
bdem_ConstRowBinding::theBoolArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL_ARRAY == elemType(fieldIndex));

    return d_row_p->theBoolArray(fieldIndex);
}

inline
const bsl::vector<char>&
bdem_ConstRowBinding::theCharArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR_ARRAY == elemType(fieldIndex));

    return d_row_p->theCharArray(fieldIndex);
}

inline
const bsl::vector<short>&
bdem_ConstRowBinding::theShortArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT_ARRAY == elemType(fieldIndex));

    return d_row_p->theShortArray(fieldIndex);
}

inline
const bsl::vector<int>&
bdem_ConstRowBinding::theIntArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT_ARRAY == elemType(fieldIndex));

    return d_row_p->theIntArray(fieldIndex);
}

inline
const bsl::vector<bsls_Types::Int64>&
bdem_ConstRowBinding::theInt64Array(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64_ARRAY == elemType(fieldIndex));

    return d_row_p->theInt64Array(fieldIndex);
}

inline
const bsl::vector<float>&
bdem_ConstRowBinding::theFloatArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT_ARRAY == elemType(fieldIndex));

    return d_row_p->theFloatArray(fieldIndex);
}

inline
const bsl::vector<double>&
bdem_ConstRowBinding::theDoubleArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE_ARRAY == elemType(fieldIndex));

    return d_row_p->theDoubleArray(fieldIndex);
}

inline
const bsl::vector<bsl::string>&
bdem_ConstRowBinding::theStringArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING_ARRAY == elemType(fieldIndex));

    return d_row_p->theStringArray(fieldIndex);
}

inline
const bsl::vector<bdet_Datetime>&
bdem_ConstRowBinding::theDatetimeArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(
                   bdem_ElemType::BDEM_DATETIME_ARRAY == elemType(fieldIndex));

    return d_row_p->theDatetimeArray(fieldIndex);
}

inline
const bsl::vector<bdet_DatetimeTz>&
bdem_ConstRowBinding::theDatetimeTzArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_DATETIMETZ_ARRAY == elemType(fieldIndex));

    return d_row_p->theDatetimeTzArray(fieldIndex);
}

inline
const bsl::vector<bdet_Date>&
bdem_ConstRowBinding::theDateArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE_ARRAY == elemType(fieldIndex));

    return d_row_p->theDateArray(fieldIndex);
}

inline
const bsl::vector<bdet_DateTz>&
bdem_ConstRowBinding::theDateTzArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ_ARRAY == elemType(fieldIndex));

    return d_row_p->theDateTzArray(fieldIndex);
}

inline
const bsl::vector<bdet_Time>&
bdem_ConstRowBinding::theTimeArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME_ARRAY == elemType(fieldIndex));

    return d_row_p->theTimeArray(fieldIndex);
}

inline
const bsl::vector<bdet_TimeTz>&
bdem_ConstRowBinding::theTimeTzArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ_ARRAY == elemType(fieldIndex));

    return d_row_p->theTimeTzArray(fieldIndex);
}

inline
const bdem_Choice&
bdem_ConstRowBinding::theChoice(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == elemType(fieldIndex));

    return d_row_p->theChoice(fieldIndex);
}

inline
const bdem_ChoiceArray&
bdem_ConstRowBinding::theChoiceArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE_ARRAY == elemType(fieldIndex));

    return d_row_p->theChoiceArray(fieldIndex);
}

inline
const bdem_List& bdem_ConstRowBinding::theList(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == elemType(fieldIndex));

    return d_row_p->theList(fieldIndex);
}

inline
const bdem_Table&
bdem_ConstRowBinding::theTable(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == elemType(fieldIndex));

    return d_row_p->theTable(fieldIndex);
}

inline
bdem_ConstChoiceBinding bdem_ConstRowBinding::choiceBinding(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_row_p->length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == elemType(index));

    return bdem_ConstChoiceBinding(
                              &(*d_row_p)[index].theChoice(),
                              d_constraint_p->field(index).recordConstraint());
}

inline
bdem_ConstChoiceBinding
bdem_ConstRowBinding::choiceBinding(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return choiceBinding(fieldIndex);
}

inline
bdem_ConstChoiceArrayBinding
bdem_ConstRowBinding::choiceArrayBinding(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_row_p->length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE_ARRAY == elemType(index));

    return bdem_ConstChoiceArrayBinding(
                              &(*d_row_p)[index].theChoiceArray(),
                              d_constraint_p->field(index).recordConstraint());
}

inline
bdem_ConstChoiceArrayBinding
bdem_ConstRowBinding::choiceArrayBinding(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return choiceArrayBinding(fieldIndex);
}

inline
bdem_ConstRowBinding bdem_ConstRowBinding::rowBinding(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_row_p->length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == elemType(index));

    return bdem_ConstRowBinding(
                              &(*d_row_p)[index].theList().row(),
                              d_constraint_p->field(index).recordConstraint());
}

inline
bdem_ConstRowBinding bdem_ConstRowBinding::rowBinding(
                                                 const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return rowBinding(fieldIndex);
}

inline
bdem_ConstTableBinding bdem_ConstRowBinding::tableBinding(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_row_p->length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == elemType(index));

    return bdem_ConstTableBinding(
                           &(*d_row_p)[index].theTable(),
                           d_constraint_p->field(index).recordConstraint());
}

inline
bdem_ConstTableBinding bdem_ConstRowBinding::tableBinding(
                                                 const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return tableBinding(fieldIndex);
}

// FREE OPERATORS
inline
bool operator==(const bdem_ConstRowBinding& lhs,
                const bdem_ConstRowBinding& rhs)
{
    return bdem_SchemaUtil::areEquivalent(*lhs.d_constraint_p,
                                          *rhs.d_constraint_p)
        && *lhs.d_row_p == *rhs.d_row_p;
}

inline
bool operator!=(const bdem_ConstRowBinding& lhs,
                const bdem_ConstRowBinding& rhs)
{
    return !(lhs == rhs);
}

                        // ---------------------
                        // class bdem_RowBinding
                        // ---------------------

                        // -----------------
                        // Level-0 Functions
                        // -----------------

// ACCESSORS
inline
bdem_Row& bdem_RowBinding::row() const
{
    return *const_cast<bdem_Row *>(d_row_p);
}

// CREATORS
inline
bdem_RowBinding::bdem_RowBinding(bdem_Row *row, const bdem_RecordDef *record)
: bdem_ConstRowBinding(row, record)
{
}

inline
bdem_RowBinding::bdem_RowBinding(bdem_Row          *row,
                                 const bdem_Schema *schema,
                                 const char        *recordName)
: bdem_ConstRowBinding(row, schema, recordName)
{
}

inline
bdem_RowBinding::bdem_RowBinding(bdem_Row          *row,
                                 const bdem_Schema *schema,
                                 int                recordIndex)
: bdem_ConstRowBinding(row, schema, recordIndex)
{
}

inline
bdem_RowBinding::bdem_RowBinding(bdem_List *list, const bdem_RecordDef *record)
: bdem_ConstRowBinding(list, record)
{
}

inline
bdem_RowBinding::bdem_RowBinding(bdem_List         *list,
                                 const bdem_Schema *schema,
                                 const char        *recordName)
: bdem_ConstRowBinding(list, schema, recordName)
{
}

inline
bdem_RowBinding::bdem_RowBinding(bdem_List         *list,
                                 const bdem_Schema *schema,
                                 int                recordIndex)
: bdem_ConstRowBinding(list, schema, recordIndex)
{
}

inline
bdem_RowBinding::bdem_RowBinding(bdem_Table           *table,
                                 int                   rowIndex,
                                 const bdem_RecordDef *record)
: bdem_ConstRowBinding(table, rowIndex, record)
{
}

inline
bdem_RowBinding::bdem_RowBinding(bdem_Table        *table,
                                 int                rowIndex,
                                 const bdem_Schema *schema,
                                 const char        *recordName)
: bdem_ConstRowBinding(table, rowIndex, schema, recordName)
{
}

inline
bdem_RowBinding::bdem_RowBinding(bdem_Table        *table,
                                 int                rowIndex,
                                 const bdem_Schema *schema,
                                 int                recordIndex)
: bdem_ConstRowBinding(table, rowIndex, schema, recordIndex)
{
}

inline
bdem_RowBinding::bdem_RowBinding(const bdem_RowBinding& original)
: bdem_ConstRowBinding(original)
{
}

// MANIPULATORS
inline
void bdem_RowBinding::rebind(const bdem_RowBinding& binding)
{
    bdem_ConstRowBinding::rebind(binding);
}

inline
void bdem_RowBinding::rebind(bdem_Row *row)
{
    bdem_ConstRowBinding::rebind(row);
}

inline
void bdem_RowBinding::rebind(bdem_List *list)
{
    bdem_ConstRowBinding::rebind(list);
}

inline
void bdem_RowBinding::rebind(bdem_Table *table, int rowIndex)
{
    bdem_ConstRowBinding::rebind(table, rowIndex);
}

inline
void bdem_RowBinding::rebind(bdem_Row *row, const bdem_RecordDef *record)
{
    bdem_ConstRowBinding::rebind(row, record);
}

inline
void bdem_RowBinding::rebind(bdem_Row          *row,
                             const bdem_Schema *schema,
                             const char        *recordName)
{
    bdem_ConstRowBinding::rebind(row, schema, recordName);
}

inline
void bdem_RowBinding::rebind(bdem_Row          *row,
                             const bdem_Schema *schema,
                             int                recordIndex)
{
    bdem_ConstRowBinding::rebind(row, schema, recordIndex);
}

inline
void bdem_RowBinding::rebind(bdem_List *list, const bdem_RecordDef *record)
{
    bdem_ConstRowBinding::rebind(list, record);
}

inline
void bdem_RowBinding::rebind(bdem_List         *list,
                             const bdem_Schema *schema,
                             const char        *recordName)
{
    bdem_ConstRowBinding::rebind(list, schema, recordName);
}

inline
void bdem_RowBinding::rebind(bdem_List         *list,
                             const bdem_Schema *schema,
                             int                recordIndex)
{
    bdem_ConstRowBinding::rebind(list, schema, recordIndex);
}

inline
void bdem_RowBinding::rebind(bdem_Table           *table,
                             int                   rowIndex,
                             const bdem_RecordDef *record)
{
    bdem_ConstRowBinding::rebind(table, rowIndex, record);
}

inline
void bdem_RowBinding::rebind(bdem_Table        *table,
                             int                rowIndex,
                             const bdem_Schema *schema,
                             const char        *recordName)
{
    bdem_ConstRowBinding::rebind(table, rowIndex, schema, recordName);
}

inline
void bdem_RowBinding::rebind(bdem_Table        *table,
                             int                rowIndex,
                             const bdem_Schema *schema,
                             int                recordIndex)
{
    bdem_ConstRowBinding::rebind(table, rowIndex, schema, recordIndex);
}

// ACCESSORS
inline
bdem_ElemRef bdem_RowBinding::operator[](int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_row_p->length());

    return row()[index];
}

inline
bdem_ElemRef bdem_RowBinding::element(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_row_p->length());

    return row()[index];
}

inline
bdem_ElemRef bdem_RowBinding::element(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return element(fieldIndex);
}

inline
bool& bdem_RowBinding::theModifiableBool(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL == elemType(fieldIndex));

    return row().theModifiableBool(fieldIndex);
}

inline
char& bdem_RowBinding::theModifiableChar(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR == elemType(fieldIndex));

    return row().theModifiableChar(fieldIndex);
}

inline
short& bdem_RowBinding::theModifiableShort(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT == elemType(fieldIndex));

    return row().theModifiableShort(fieldIndex);
}

inline
int& bdem_RowBinding::theModifiableInt(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT == elemType(fieldIndex));

    return row().theModifiableInt(fieldIndex);
}

inline
bsls_Types::Int64&
bdem_RowBinding::theModifiableInt64(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64 == elemType(fieldIndex));

    return row().theModifiableInt64(fieldIndex);
}

inline
float& bdem_RowBinding::theModifiableFloat(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT == elemType(fieldIndex));

    return row().theModifiableFloat(fieldIndex);
}

inline
double& bdem_RowBinding::theModifiableDouble(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE == elemType(fieldIndex));

    return row().theModifiableDouble(fieldIndex);
}

inline
bsl::string&
bdem_RowBinding::theModifiableString(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING == elemType(fieldIndex));

    return row().theModifiableString(fieldIndex);
}

inline
bdet_Datetime&
bdem_RowBinding::theModifiableDatetime(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIME == elemType(fieldIndex));

    return row().theModifiableDatetime(fieldIndex);
}

inline
bdet_DatetimeTz&
bdem_RowBinding::theModifiableDatetimeTz(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIMETZ == elemType(fieldIndex));

    return row().theModifiableDatetimeTz(fieldIndex);
}

inline
bdet_Date& bdem_RowBinding::theModifiableDate(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE == elemType(fieldIndex));

    return row().theModifiableDate(fieldIndex);
}

inline
bdet_DateTz& bdem_RowBinding::theModifiableDateTz(
                                                 const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ == elemType(fieldIndex));

    return row().theModifiableDateTz(fieldIndex);
}

inline
bdet_Time& bdem_RowBinding::theModifiableTime(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME == elemType(fieldIndex));

    return row().theModifiableTime(fieldIndex);
}

inline
bdet_TimeTz& bdem_RowBinding::theModifiableTimeTz(
                                                 const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ == elemType(fieldIndex));

    return row().theModifiableTimeTz(fieldIndex);
}

inline
bsl::vector<bool>&
bdem_RowBinding::theModifiableBoolArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL_ARRAY == elemType(fieldIndex));

    return row().theModifiableBoolArray(fieldIndex);
}

inline
bsl::vector<char>&
bdem_RowBinding::theModifiableCharArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR_ARRAY == elemType(fieldIndex));

    return row().theModifiableCharArray(fieldIndex);
}

inline
bsl::vector<short>&
bdem_RowBinding::theModifiableShortArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT_ARRAY == elemType(fieldIndex));

    return row().theModifiableShortArray(fieldIndex);
}

inline
bsl::vector<int>&
bdem_RowBinding::theModifiableIntArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT_ARRAY == elemType(fieldIndex));

    return row().theModifiableIntArray(fieldIndex);
}

inline
bsl::vector<bsls_Types::Int64>&
bdem_RowBinding::theModifiableInt64Array(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64_ARRAY == elemType(fieldIndex));

    return row().theModifiableInt64Array(fieldIndex);
}

inline
bsl::vector<float>&
bdem_RowBinding::theModifiableFloatArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT_ARRAY == elemType(fieldIndex));

    return row().theModifiableFloatArray(fieldIndex);
}

inline
bsl::vector<double>&
bdem_RowBinding::theModifiableDoubleArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE_ARRAY == elemType(fieldIndex));

    return row().theModifiableDoubleArray(fieldIndex);
}

inline
bsl::vector<bsl::string>&
bdem_RowBinding::theModifiableStringArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING_ARRAY == elemType(fieldIndex));

    return row().theModifiableStringArray(fieldIndex);
}

inline
bsl::vector<bdet_Datetime>&
bdem_RowBinding::theModifiableDatetimeArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(
                   bdem_ElemType::BDEM_DATETIME_ARRAY == elemType(fieldIndex));

    return row().theModifiableDatetimeArray(fieldIndex);
}

inline
bsl::vector<bdet_DatetimeTz>&
bdem_RowBinding::theModifiableDatetimeTzArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_DATETIMETZ_ARRAY == elemType(fieldIndex));

    return row().theModifiableDatetimeTzArray(fieldIndex);
}

inline
bsl::vector<bdet_Date>&
bdem_RowBinding::theModifiableDateArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE_ARRAY == elemType(fieldIndex));

    return row().theModifiableDateArray(fieldIndex);
}

inline
bsl::vector<bdet_DateTz>&
bdem_RowBinding::theModifiableDateTzArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ_ARRAY == elemType(fieldIndex));

    return row().theModifiableDateTzArray(fieldIndex);
}

inline
bsl::vector<bdet_Time>&
bdem_RowBinding::theModifiableTimeArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME_ARRAY == elemType(fieldIndex));

    return row().theModifiableTimeArray(fieldIndex);
}

inline
bsl::vector<bdet_TimeTz>&
bdem_RowBinding::theModifiableTimeTzArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ_ARRAY == elemType(fieldIndex));

    return row().theModifiableTimeTzArray(fieldIndex);
}

inline
bdem_Choice& bdem_RowBinding::theModifiableChoice(
                                                 const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == elemType(fieldIndex));

    return row().theModifiableChoice(fieldIndex);
}

inline
bdem_ChoiceArray&
bdem_RowBinding::theModifiableChoiceArray(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE_ARRAY == elemType(fieldIndex));

    return row().theModifiableChoiceArray(fieldIndex);
}

inline
bdem_List& bdem_RowBinding::theModifiableList(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == elemType(fieldIndex));

    return row().theModifiableList(fieldIndex);
}

inline
bdem_Table&
bdem_RowBinding::theModifiableTable(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == elemType(fieldIndex));

    return row().theModifiableTable(fieldIndex);
}

inline
bdem_ChoiceBinding bdem_RowBinding::choiceBinding(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_row_p->length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == elemType(index));

    return bdem_ChoiceBinding(&row()[index].theModifiableChoice(),
                              d_constraint_p->field(index).recordConstraint());
}

inline
bdem_ChoiceBinding
bdem_RowBinding::choiceBinding(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return choiceBinding(fieldIndex);
}

inline
bdem_ChoiceArrayBinding
bdem_RowBinding::choiceArrayBinding(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_row_p->length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE_ARRAY == elemType(index));

    return bdem_ChoiceArrayBinding(
                              &row()[index].theModifiableChoiceArray(),
                              d_constraint_p->field(index).recordConstraint());
}

inline
bdem_ChoiceArrayBinding
bdem_RowBinding::choiceArrayBinding(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return choiceArrayBinding(fieldIndex);
}

inline
bdem_RowBinding bdem_RowBinding::rowBinding(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_row_p->length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == elemType(index));

    return bdem_RowBinding(&row()[index].theModifiableList().row(),
                           d_constraint_p->field(index).recordConstraint());
}

inline
bdem_RowBinding bdem_RowBinding::rowBinding(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return rowBinding(fieldIndex);
}

inline
bdem_TableBinding bdem_RowBinding::tableBinding(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_row_p->length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == elemType(index));

    return bdem_TableBinding(&row()[index].theModifiableTable(),
                             d_constraint_p->field(index).recordConstraint());
}

inline
bdem_TableBinding bdem_RowBinding::tableBinding(const char *elementName) const
{
    BSLS_ASSERT_SAFE(elementName);

    const int fieldIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return tableBinding(fieldIndex);
}

                        // ----------------------------
                        // class bdem_ConstTableBinding
                        // ----------------------------

// CREATORS
inline
bdem_ConstTableBinding::bdem_ConstTableBinding(const bdem_Table     *table,
                                               const bdem_RecordDef *record)
: d_table_p(table)
, d_constraint_p(record)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(record);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(*table,
                                                                *record));
}

inline
bdem_ConstTableBinding::bdem_ConstTableBinding(const bdem_Table  *table,
                                               const bdem_Schema *schema,
                                               const char        *recordName)
: d_table_p(table)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *table,
                                                             *d_constraint_p));
}

inline
bdem_ConstTableBinding::bdem_ConstTableBinding(const bdem_Table  *table,
                                               const bdem_Schema *schema,
                                               int                recordIndex)
: d_table_p(table)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->length());

    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *table,
                                                             *d_constraint_p));
}

inline
bdem_ConstTableBinding::bdem_ConstTableBinding(
                                        const bdem_ConstTableBinding& original)
: d_table_p(original.d_table_p)
, d_constraint_p(original.d_constraint_p)
{
}

// MANIPULATORS
inline
void bdem_ConstTableBinding::rebind(const bdem_ConstTableBinding& binding)
{
    d_table_p      = binding.d_table_p;
    d_constraint_p = binding.d_constraint_p;
}

inline
void bdem_ConstTableBinding::rebind(const bdem_Table *table)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *table,
                                                             *d_constraint_p));

    d_table_p = table;
}

inline
void bdem_ConstTableBinding::rebind(const bdem_Table     *table,
                                    const bdem_RecordDef *record)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(record);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(*table,
                                                                *record));

    d_table_p      = table;
    d_constraint_p = record;
}

inline
void bdem_ConstTableBinding::rebind(const bdem_Table  *table,
                                    const bdem_Schema *schema,
                                    const char        *recordName)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_table_p      = table;
    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *table,
                                                             *d_constraint_p));
}

inline
void bdem_ConstTableBinding::rebind(const bdem_Table  *table,
                                    const bdem_Schema *schema,
                                    int                recordIndex)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->length());

    d_table_p      = table;
    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *table,
                                                             *d_constraint_p));
}

// ACCESSORS
inline
const bdem_Row& bdem_ConstTableBinding::operator[](int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());

    return (*d_table_p)[rowIndex];
}

inline
bdem_ConstElemRef
bdem_ConstTableBinding::element(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return (*d_table_p)[rowIndex][fieldIndex];
}

inline
bdem_ConstElemRef bdem_ConstTableBinding::element(int rowIndex,
                                                  int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex    < d_table_p->numRows());
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());

    return (*d_table_p)[rowIndex][columnIndex];
}

inline
bdem_ElemType::Type
bdem_ConstTableBinding::elemType(const char *columnName) const
{
    BSLS_ASSERT_SAFE(columnName);

    const bdem_FieldDef *fieldDef = d_constraint_p->lookupField(columnName);

    BSLS_ASSERT_SAFE(fieldDef);

    return fieldDef->elemType();
}

inline
bdem_ElemType::Type bdem_ConstTableBinding::elemType(int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());

    return d_table_p->columnType(columnIndex);
}

inline
int bdem_ConstTableBinding::numColumns() const
{
    return d_constraint_p->numFields();
}

inline
int bdem_ConstTableBinding::numRows() const
{
    return d_table_p->numRows();
}

inline
const bdem_RecordDef& bdem_ConstTableBinding::record() const
{
    return *d_constraint_p;
}

inline
const bdem_Table& bdem_ConstTableBinding::table() const
{
    return *d_table_p;
}

inline
const bool&
bdem_ConstTableBinding::theBool(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theBool();
}

inline
const char&
bdem_ConstTableBinding::theChar(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theChar();
}

inline
const short&
bdem_ConstTableBinding::theShort(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theShort();
}

inline
const int&
bdem_ConstTableBinding::theInt(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theInt();
}

inline
const bsls_Types::Int64&
bdem_ConstTableBinding::theInt64(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64 == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theInt64();
}

inline
const float&
bdem_ConstTableBinding::theFloat(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theFloat();
}

inline
const double&
bdem_ConstTableBinding::theDouble(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theDouble();
}

inline
const bsl::string&
bdem_ConstTableBinding::theString(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theString();
}

inline
const bdet_Datetime&
bdem_ConstTableBinding::theDatetime(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIME == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theDatetime();
}

inline
const bdet_DatetimeTz&
bdem_ConstTableBinding::theDatetimeTz(int         rowIndex,
                                      const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIMETZ == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theDatetimeTz();
}

inline
const bdet_Date&
bdem_ConstTableBinding::theDate(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theDate();
}

inline
const bdet_DateTz&
bdem_ConstTableBinding::theDateTz(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theDateTz();
}

inline
const bdet_Time&
bdem_ConstTableBinding::theTime(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theTime();
}

inline
const bdet_TimeTz&
bdem_ConstTableBinding::theTimeTz(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theTimeTz();
}

inline
const bsl::vector<bool>&
bdem_ConstTableBinding::theBoolArray(int         rowIndex,
                                     const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theBoolArray();
}

inline
const bsl::vector<char>&
bdem_ConstTableBinding::theCharArray(int         rowIndex,
                                     const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theCharArray();
}

inline
const bsl::vector<short>&
bdem_ConstTableBinding::theShortArray(int         rowIndex,
                                      const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theShortArray();
}

inline
const bsl::vector<int>&
bdem_ConstTableBinding::theIntArray(int         rowIndex,
                                    const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theIntArray();
}

inline
const bsl::vector<bsls_Types::Int64>&
bdem_ConstTableBinding::theInt64Array(int         rowIndex,
                                      const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theInt64Array();
}

inline
const bsl::vector<float>&
bdem_ConstTableBinding::theFloatArray(int         rowIndex,
                                      const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theFloatArray();
}

inline
const bsl::vector<double>&
bdem_ConstTableBinding::theDoubleArray(int         rowIndex,
                                       const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theDoubleArray();
}

inline
const bsl::vector<bsl::string>&
bdem_ConstTableBinding::theStringArray(int         rowIndex,
                                       const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theStringArray();
}

inline
const bsl::vector<bdet_Datetime>&
bdem_ConstTableBinding::theDatetimeArray(int         rowIndex,
                                         const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(
                   bdem_ElemType::BDEM_DATETIME_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theDatetimeArray();
}

inline
const bsl::vector<bdet_DatetimeTz>&
bdem_ConstTableBinding::theDatetimeTzArray(int         rowIndex,
                                           const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_DATETIMETZ_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theDatetimeTzArray();
}

inline
const bsl::vector<bdet_Date>&
bdem_ConstTableBinding::theDateArray(int         rowIndex,
                                     const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theDateArray();
}

inline
const bsl::vector<bdet_DateTz>&
bdem_ConstTableBinding::theDateTzArray(int         rowIndex,
                                       const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theDateTzArray();
}

inline
const bsl::vector<bdet_Time>&
bdem_ConstTableBinding::theTimeArray(int         rowIndex,
                                     const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theTimeArray();
}

inline
const bsl::vector<bdet_TimeTz>&
bdem_ConstTableBinding::theTimeTzArray(int         rowIndex,
                                       const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theTimeTzArray();
}

inline
const bdem_Choice&
bdem_ConstTableBinding::theChoice(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theChoice();
}

inline
const bdem_ChoiceArray&
bdem_ConstTableBinding::theChoiceArray(int         rowIndex,
                                       const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theChoiceArray();
}

inline
const bdem_List&
bdem_ConstTableBinding::theList(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theList();
}

inline
const bdem_Table&
bdem_ConstTableBinding::theTable(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theTable();
}

inline
bdem_ConstChoiceBinding
bdem_ConstTableBinding::choiceBinding(int rowIndex, int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex    < d_table_p->numRows());
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == elemType(columnIndex));

    return bdem_ConstChoiceBinding(
                        &(*d_table_p)[rowIndex][columnIndex].theChoice(),
                        d_constraint_p->field(columnIndex).recordConstraint());
}

inline
bdem_ConstChoiceBinding bdem_ConstTableBinding::choiceBinding(
                                                  int         rowIndex,
                                                  const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == elemType(fieldIndex));

    return choiceBinding(rowIndex, fieldIndex);
}

inline
bdem_ConstChoiceArrayBinding
bdem_ConstTableBinding::choiceArrayBinding(int rowIndex, int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex    < d_table_p->numRows());
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());
    BSLS_ASSERT_SAFE(
                    bdem_ElemType::BDEM_CHOICE_ARRAY == elemType(columnIndex));

    return bdem_ConstChoiceArrayBinding(
                        &(*d_table_p)[rowIndex][columnIndex].theChoiceArray(),
                        d_constraint_p->field(columnIndex).recordConstraint());
}

inline
bdem_ConstChoiceArrayBinding
bdem_ConstTableBinding::choiceArrayBinding(int         rowIndex,
                                           const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE_ARRAY == elemType(fieldIndex));

    return choiceArrayBinding(rowIndex, fieldIndex);
}

inline
bdem_ConstRowBinding bdem_ConstTableBinding::rowBinding(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());

    return bdem_ConstRowBinding(&(*d_table_p)[rowIndex], d_constraint_p);
}

inline
bdem_ConstRowBinding bdem_ConstTableBinding::rowBinding(int rowIndex,
                                                        int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex    < d_table_p->numRows());
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == elemType(columnIndex));

    return bdem_ConstRowBinding(
                        &(*d_table_p)[rowIndex][columnIndex].theList().row(),
                        d_constraint_p->field(columnIndex).recordConstraint());
}

inline
bdem_ConstRowBinding bdem_ConstTableBinding::rowBinding(
                                                  int         rowIndex,
                                                  const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return rowBinding(rowIndex, fieldIndex);
}

inline
bdem_ConstTableBinding
bdem_ConstTableBinding::tableBinding(int rowIndex, int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex    < d_table_p->numRows());
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == elemType(columnIndex));

    return bdem_ConstTableBinding(
                        &(*d_table_p)[rowIndex][columnIndex].theTable(),
                        d_constraint_p->field(columnIndex).recordConstraint());
}

inline
bdem_ConstTableBinding
bdem_ConstTableBinding::tableBinding(int         rowIndex,
                                     const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return tableBinding(rowIndex, fieldIndex);
}

inline
bdem_ConstColumnBinding
bdem_ConstTableBinding::columnBinding(int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());

    return bdem_ConstColumnBinding(d_table_p,
                                   columnIndex,
                                   &d_constraint_p->field(columnIndex));
}

inline
bdem_ConstColumnBinding bdem_ConstTableBinding::columnBinding(
                                                  const char *columnName) const
{
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return columnBinding(fieldIndex);
}

// FREE OPERATORS
inline
bool operator==(const bdem_ConstTableBinding& lhs,
                const bdem_ConstTableBinding& rhs)
{
    return bdem_SchemaUtil::areEquivalent(*lhs.d_constraint_p,
                                          *rhs.d_constraint_p)
        && *lhs.d_table_p == *rhs.d_table_p;
}

inline
bool operator!=(const bdem_ConstTableBinding& lhs,
                const bdem_ConstTableBinding& rhs)
{
    return !(lhs == rhs);
}

                        // -----------------------
                        // class bdem_TableBinding
                        // -----------------------

                        // -----------------
                        // Level-0 Functions
                        // -----------------

// ACCESSORS
inline
bdem_Table& bdem_TableBinding::table() const
{
    return *const_cast<bdem_Table *>(d_table_p);
}

// CREATORS
inline
bdem_TableBinding::bdem_TableBinding(bdem_Table           *table,
                                     const bdem_RecordDef *record)
: bdem_ConstTableBinding(table, record)
{
}

inline
bdem_TableBinding::bdem_TableBinding(bdem_Table        *table,
                                     const bdem_Schema *schema,
                                     const char        *recordName)
: bdem_ConstTableBinding(table, schema, recordName)
{
}

inline
bdem_TableBinding::bdem_TableBinding(bdem_Table        *table,
                                     const bdem_Schema *schema,
                                     int                recordIndex)
: bdem_ConstTableBinding(table, schema, recordIndex)
{
}

inline
bdem_TableBinding::bdem_TableBinding(const bdem_TableBinding& original)
: bdem_ConstTableBinding(original)
{
}

// MANIPULATORS
inline
void bdem_TableBinding::rebind(const bdem_TableBinding& binding)
{
    bdem_ConstTableBinding::rebind(binding);
}

inline
void bdem_TableBinding::rebind(bdem_Table *table)
{
    bdem_ConstTableBinding::rebind(table);
}

inline
void bdem_TableBinding::rebind(bdem_Table *table, const bdem_RecordDef *record)
{
    bdem_ConstTableBinding::rebind(table, record);
}

inline
void bdem_TableBinding::rebind(bdem_Table        *table,
                               const bdem_Schema *schema,
                               const char        *recordName)
{
    bdem_ConstTableBinding::rebind(table, schema, recordName);
}

inline
void bdem_TableBinding::rebind(bdem_Table        *table,
                               const bdem_Schema *schema,
                               int                recordIndex)
{
    bdem_ConstTableBinding::rebind(table, schema, recordIndex);
}

// ACCESSORS
inline
bdem_Row& bdem_TableBinding::operator[](int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());

    return table().theModifiableRow(rowIndex);
}

inline
bdem_ElemRef
bdem_TableBinding::element(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return table().theModifiableRow(rowIndex)[fieldIndex];
}

inline
bdem_ElemRef bdem_TableBinding::element(int rowIndex, int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex    < d_table_p->numRows());
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());

    return table().theModifiableRow(rowIndex)[columnIndex];
}

inline
bdem_Row& bdem_TableBinding::theModifiableRow(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());

    return table().theModifiableRow(rowIndex);
}

inline
bool& bdem_TableBinding::theModifiableBool(int         rowIndex,
                                           const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());

    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableBool();
}

inline
char& bdem_TableBinding::theModifiableChar(int         rowIndex,
                                           const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableChar();
}

inline
short& bdem_TableBinding::theModifiableShort(int         rowIndex,
                                             const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableShort();
}

inline
int& bdem_TableBinding::theModifiableInt(int         rowIndex,
                                         const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableInt();
}

inline
bsls_Types::Int64&
bdem_TableBinding::theModifiableInt64(int         rowIndex,
                                      const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64 == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableInt64();
}

inline
float& bdem_TableBinding::theModifiableFloat(int         rowIndex,
                                             const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableFloat();
}

inline
double& bdem_TableBinding::theModifiableDouble(int         rowIndex,
                                               const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableDouble();
}

inline
bsl::string&
bdem_TableBinding::theModifiableString(int         rowIndex,
                                       const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableString();
}

inline
bdet_Datetime&
bdem_TableBinding::theModifiableDatetime(int         rowIndex,
                                         const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIME == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableDatetime();
}

inline
bdet_DatetimeTz&
bdem_TableBinding::theModifiableDatetimeTz(int         rowIndex,
                                           const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIMETZ == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableDatetimeTz();
}

inline
bdet_Date&
bdem_TableBinding::theModifiableDate(int         rowIndex,
                                     const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableDate();
}

inline
bdet_DateTz&
bdem_TableBinding::theModifiableDateTz(int         rowIndex,
                                       const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableDateTz();
}

inline
bdet_Time&
bdem_TableBinding::theModifiableTime(int         rowIndex,
                                     const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableTime();
}

inline
bdet_TimeTz&
bdem_TableBinding::theModifiableTimeTz(int         rowIndex,
                                       const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableTimeTz();
}

inline
bsl::vector<bool>&
bdem_TableBinding::theModifiableBoolArray(int         rowIndex,
                                          const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableBoolArray();
}

inline
bsl::vector<char>&
bdem_TableBinding::theModifiableCharArray(int         rowIndex,
                                          const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableCharArray();
}

inline
bsl::vector<short>&
bdem_TableBinding::theModifiableShortArray(int         rowIndex,
                                           const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableShortArray();
}

inline
bsl::vector<int>&
bdem_TableBinding::theModifiableIntArray(int         rowIndex,
                                         const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableIntArray();
}

inline
bsl::vector<bsls_Types::Int64>&
bdem_TableBinding::theModifiableInt64Array(int         rowIndex,
                                           const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableInt64Array();
}

inline
bsl::vector<float>&
bdem_TableBinding::theModifiableFloatArray(int         rowIndex,
                                           const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableFloatArray();
}

inline
bsl::vector<double>&
bdem_TableBinding::theModifiableDoubleArray(int         rowIndex,
                                            const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableDoubleArray();
}

inline
bsl::vector<bsl::string>&
bdem_TableBinding::theModifiableStringArray(int         rowIndex,
                                            const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableStringArray();
}

inline
bsl::vector<bdet_Datetime>&
bdem_TableBinding::theModifiableDatetimeArray(int         rowIndex,
                                              const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(
                   bdem_ElemType::BDEM_DATETIME_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableDatetimeArray();
}

inline
bsl::vector<bdet_DatetimeTz>&
bdem_TableBinding::theModifiableDatetimeTzArray(int         rowIndex,
                                                const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_DATETIMETZ_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableDatetimeTzArray();
}

inline
bsl::vector<bdet_Date>&
bdem_TableBinding::theModifiableDateArray(int         rowIndex,
                                          const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableDateArray();
}

inline
bsl::vector<bdet_DateTz>&
bdem_TableBinding::theModifiableDateTzArray(int         rowIndex,
                                            const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableDateTzArray();
}

inline
bsl::vector<bdet_Time>&
bdem_TableBinding::theModifiableTimeArray(int         rowIndex,
                                          const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableTimeArray();
}

inline
bsl::vector<bdet_TimeTz>&
bdem_TableBinding::theModifiableTimeTzArray(int         rowIndex,
                                            const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableTimeTzArray();
}

inline
bdem_Choice&
bdem_TableBinding::theModifiableChoice(int         rowIndex,
                                       const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableChoice();
}

inline
bdem_ChoiceArray&
bdem_TableBinding::theModifiableChoiceArray(int         rowIndex,
                                            const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE_ARRAY == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableChoiceArray();
}

inline
bdem_List& bdem_TableBinding::theModifiableList(int         rowIndex,
                                                const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableList();
}

inline
bdem_Table& bdem_TableBinding::theModifiableTable(int         rowIndex,
                                                  const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == elemType(fieldIndex));

    return element(rowIndex, fieldIndex).theModifiableTable();
}

inline
bdem_ChoiceBinding
bdem_TableBinding::choiceBinding(int rowIndex, int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex    < d_table_p->numRows());
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == elemType(columnIndex));

    return bdem_ChoiceBinding(
                        &element(rowIndex, columnIndex).theModifiableChoice(),
                        d_constraint_p->field(columnIndex).recordConstraint());
}

inline
bdem_ChoiceBinding
bdem_TableBinding::choiceBinding(int         rowIndex,
                                 const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return choiceBinding(rowIndex, fieldIndex);
}

inline
bdem_ChoiceArrayBinding
bdem_TableBinding::choiceArrayBinding(int rowIndex, int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex    < d_table_p->numRows());
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());
    BSLS_ASSERT_SAFE(
                    bdem_ElemType::BDEM_CHOICE_ARRAY == elemType(columnIndex));

    return bdem_ChoiceArrayBinding(
                    &element(rowIndex, columnIndex).theModifiableChoiceArray(),
                    d_constraint_p->field(columnIndex).recordConstraint());
}

inline
bdem_ChoiceArrayBinding
bdem_TableBinding::choiceArrayBinding(int         rowIndex,
                                      const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return choiceArrayBinding(rowIndex, fieldIndex);
}

inline
bdem_RowBinding bdem_TableBinding::rowBinding(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());

    return bdem_RowBinding(&table().theModifiableRow(rowIndex),
                           d_constraint_p);
}

inline
bdem_RowBinding
bdem_TableBinding::rowBinding(int rowIndex, int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex    < d_table_p->numRows());
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == elemType(columnIndex));

    return bdem_RowBinding(
                     &element(rowIndex, columnIndex).theModifiableList().row(),
                     d_constraint_p->field(columnIndex).recordConstraint());
}

inline
bdem_RowBinding
bdem_TableBinding::rowBinding(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return rowBinding(rowIndex, fieldIndex);
}

inline
bdem_TableBinding
bdem_TableBinding::tableBinding(int rowIndex, int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex    < d_table_p->numRows());
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == elemType(columnIndex));

    return bdem_TableBinding(
                        &element(rowIndex, columnIndex).theModifiableTable(),
                        d_constraint_p->field(columnIndex).recordConstraint());
}

inline
bdem_TableBinding
bdem_TableBinding::tableBinding(int rowIndex, const char *columnName) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return tableBinding(rowIndex, fieldIndex);
}

inline
bdem_ColumnBinding bdem_TableBinding::columnBinding(int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());

    return bdem_ColumnBinding(&table(),
                              columnIndex,
                              &d_constraint_p->field(columnIndex));
}

inline
bdem_ColumnBinding
bdem_TableBinding::columnBinding(const char *columnName) const
{
    BSLS_ASSERT_SAFE(columnName);

    const int fieldIndex = d_constraint_p->fieldIndex(columnName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return columnBinding(fieldIndex);
}

                        // -----------------------------
                        // class bdem_ConstColumnBinding
                        // -----------------------------

// CREATORS
inline
bdem_ConstColumnBinding::bdem_ConstColumnBinding(
                                              const bdem_Table    *table,
                                              int                  columnIndex,
                                              const bdem_FieldDef *field)
: d_table_p(table)
, d_columnIndex(columnIndex)
, d_constraint_p(field)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());
    BSLS_ASSERT_SAFE(field);
    BSLS_ASSERT_SAFE(canSatisfyColumn(table, columnIndex, field));
}

inline
bdem_ConstColumnBinding::bdem_ConstColumnBinding(
                                       const bdem_ConstColumnBinding& original)
: d_table_p(original.d_table_p)
, d_columnIndex(original.d_columnIndex)
, d_constraint_p(original.d_constraint_p)
{
}

// MANIPULATORS
inline
void bdem_ConstColumnBinding::rebind(const bdem_ConstColumnBinding& binding)
{
    d_table_p      = binding.d_table_p;
    d_columnIndex  = binding.d_columnIndex;
    d_constraint_p = binding.d_constraint_p;
}

inline
void bdem_ConstColumnBinding::rebind(const bdem_Table    *table,
                                     int                  columnIndex,
                                     const bdem_FieldDef *field)
{
    BSLS_ASSERT_SAFE(table);
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < d_table_p->numColumns());
    BSLS_ASSERT_SAFE(field);
    BSLS_ASSERT_SAFE(canSatisfyColumn(table, columnIndex, field));

    d_table_p      = table;
    d_columnIndex  = columnIndex;
    d_constraint_p = field;
}

// ACCESSORS
inline
bdem_ConstElemRef bdem_ConstColumnBinding::operator[](int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());

    return (*d_table_p)[rowIndex][d_columnIndex];
}

inline
bdem_ConstElemRef bdem_ConstColumnBinding::element(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());

    return (*d_table_p)[rowIndex][d_columnIndex];
}

inline
bdem_ElemType::Type bdem_ConstColumnBinding::elemType() const
{
    return d_constraint_p->elemType();
}

inline
int bdem_ConstColumnBinding::numRows() const
{
    return d_table_p->numRows();
}

inline
int bdem_ConstColumnBinding::columnIndex() const
{
    return d_columnIndex;
}

inline
const bdem_FieldDef& bdem_ConstColumnBinding::field() const
{
    return *d_constraint_p;
}

inline
const bdem_Table& bdem_ConstColumnBinding::table() const
{
    return *d_table_p;
}

inline
bdem_ConstChoiceBinding
bdem_ConstColumnBinding::choiceBinding(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == elemType());

    return bdem_ConstChoiceBinding(&element(rowIndex).theChoice(),
                                   d_constraint_p->recordConstraint());
}

inline
bdem_ConstChoiceArrayBinding
bdem_ConstColumnBinding::choiceArrayBinding(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE_ARRAY == elemType());

    return bdem_ConstChoiceArrayBinding(&element(rowIndex).theChoiceArray(),
                                        d_constraint_p->recordConstraint());
}

inline
bdem_ConstRowBinding bdem_ConstColumnBinding::rowBinding(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == elemType());

    return bdem_ConstRowBinding(&element(rowIndex).theList().row(),
                                d_constraint_p->recordConstraint());
}

inline
bdem_ConstTableBinding
bdem_ConstColumnBinding::tableBinding(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == elemType());

    return bdem_ConstTableBinding(&element(rowIndex).theTable(),
                                  d_constraint_p->recordConstraint());
}

                        // ------------------------
                        // class bdem_ColumnBinding
                        // ------------------------

                        // -----------------
                        // Level-0 Functions
                        // -----------------

// ACCESSORS
inline
bdem_Table& bdem_ColumnBinding::table() const
{
    return *const_cast<bdem_Table *>(d_table_p);
}

// CREATORS
inline
bdem_ColumnBinding::bdem_ColumnBinding(bdem_Table          *table,
                                       int                  columnIndex,
                                       const bdem_FieldDef *field)
: bdem_ConstColumnBinding(table, columnIndex, field)
{
}

inline
bdem_ColumnBinding::bdem_ColumnBinding(const bdem_ColumnBinding& original)
: bdem_ConstColumnBinding(original)
{
}

// MANIPULATORS
inline
void bdem_ColumnBinding::rebind(const bdem_ColumnBinding& binding)
{
    bdem_ConstColumnBinding::rebind(binding);
}

inline
void bdem_ColumnBinding::rebind(bdem_Table          *table,
                                int                  columnIndex,
                                const bdem_FieldDef *field)
{
    bdem_ConstColumnBinding::rebind(table, columnIndex, field);
}

// ACCESSORS
inline
bdem_ElemRef bdem_ColumnBinding::operator[](int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());

    return table().theModifiableRow(rowIndex)[d_columnIndex];
}

inline
bdem_ElemRef bdem_ColumnBinding::element(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());

    return table().theModifiableRow(rowIndex)[d_columnIndex];
}

inline
bdem_ChoiceBinding bdem_ColumnBinding::choiceBinding(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == elemType());

    return bdem_ChoiceBinding(&element(rowIndex).theModifiableChoice(),
                              d_constraint_p->recordConstraint());
}

inline
bdem_ChoiceArrayBinding
bdem_ColumnBinding::choiceArrayBinding(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE_ARRAY == elemType());

    return bdem_ChoiceArrayBinding(
                                &element(rowIndex).theModifiableChoiceArray(),
                                d_constraint_p->recordConstraint());
}

inline
bdem_RowBinding bdem_ColumnBinding::rowBinding(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == elemType());

    return bdem_RowBinding(&element(rowIndex).theModifiableList().row(),
                           d_constraint_p->recordConstraint());
}

inline
bdem_TableBinding bdem_ColumnBinding::tableBinding(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < d_table_p->numRows());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == elemType());

    return bdem_TableBinding(&element(rowIndex).theModifiableTable(),
                             d_constraint_p->recordConstraint());
}

                        // -----------------------------
                        // class bdem_ConstChoiceBinding
                        // -----------------------------

// CREATORS
inline
bdem_ConstChoiceBinding::bdem_ConstChoiceBinding(
                                           const bdem_ChoiceArrayItem *item,
                                           const bdem_RecordDef       *record)
: d_item_p(item)
, d_constraint_p(record)
{
    BSLS_ASSERT_SAFE(item);
    BSLS_ASSERT_SAFE(record);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(*item,
                                                                *record));
}

inline
bdem_ConstChoiceBinding::bdem_ConstChoiceBinding(
                                        const bdem_ChoiceArrayItem *item,
                                        const bdem_Schema          *schema,
                                        const char                 *recordName)
: d_item_p(item)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(item);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *item,
                                                             *d_constraint_p));
}

inline
bdem_ConstChoiceBinding::bdem_ConstChoiceBinding(
                                       const bdem_ChoiceArrayItem *item,
                                       const bdem_Schema          *schema,
                                       int                         recordIndex)
: d_item_p(item)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(item);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->numRecords());

    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *item,
                                                             *d_constraint_p));
}

inline
bdem_ConstChoiceBinding::bdem_ConstChoiceBinding(const bdem_Choice    *choice,
                                                 const bdem_RecordDef *record)
: d_item_p(0)
, d_constraint_p(record)
{
    BSLS_ASSERT_SAFE(choice);
    BSLS_ASSERT_SAFE(record);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             choice->item(),
                                                             *d_constraint_p));

    d_item_p = &choice->item();
}

inline
bdem_ConstChoiceBinding::bdem_ConstChoiceBinding(const bdem_Choice *choice,
                                                 const bdem_Schema *schema,
                                                 const char        *recordName)
: d_item_p(0)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(choice);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_item_p       = &choice->item();
    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_item_p,
                                                             *d_constraint_p));
}

inline
bdem_ConstChoiceBinding::bdem_ConstChoiceBinding(
                                                const bdem_Choice *choice,
                                                const bdem_Schema *schema,
                                                int                recordIndex)
: d_item_p(0)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(choice);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->numRecords());

    d_item_p       = &choice->item();
    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_item_p,
                                                             *d_constraint_p));
}

inline
bdem_ConstChoiceBinding::bdem_ConstChoiceBinding(
                                           const bdem_ChoiceArray *choiceArray,
                                           int                     choiceIndex,
                                           const bdem_RecordDef   *record)
: d_item_p(0)
, d_constraint_p(record)
{
    BSLS_ASSERT_SAFE(choiceArray);
    BSLS_ASSERT_SAFE(0 <= choiceIndex);
    BSLS_ASSERT_SAFE(     choiceIndex < choiceArray->length());
    BSLS_ASSERT_SAFE(record);

    d_item_p = &(*choiceArray)[choiceIndex];

    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_item_p,
                                                             *d_constraint_p));
}

inline
bdem_ConstChoiceBinding::bdem_ConstChoiceBinding(
                                           const bdem_ChoiceArray *choiceArray,
                                           int                     choiceIndex,
                                           const bdem_Schema      *schema,
                                           const char             *recordName)
: d_item_p(0)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(choiceArray);
    BSLS_ASSERT_SAFE(0 <= choiceIndex);
    BSLS_ASSERT_SAFE(     choiceIndex < choiceArray->length());
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_item_p       = &(*choiceArray)[choiceIndex];
    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_item_p,
                                                             *d_constraint_p));
}

inline
bdem_ConstChoiceBinding::bdem_ConstChoiceBinding(
                                          const bdem_ChoiceArray  *choiceArray,
                                          int                      choiceIndex,
                                          const bdem_Schema       *schema,
                                          int                      recordIndex)
: d_item_p(0)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(choiceArray);
    BSLS_ASSERT_SAFE(0 <= choiceIndex);
    BSLS_ASSERT_SAFE(     choiceIndex < choiceArray->length());
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->length());

    d_item_p       = &(*choiceArray)[choiceIndex];
    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_item_p,
                                                             *d_constraint_p));
}

inline
bdem_ConstChoiceBinding::bdem_ConstChoiceBinding(
                                       const bdem_ConstChoiceBinding& original)
: d_item_p(original.d_item_p)
, d_constraint_p(original.d_constraint_p)
{
}

// MANIPULATORS
inline
void bdem_ConstChoiceBinding::rebind(const bdem_ConstChoiceBinding& binding)
{
    d_item_p       = binding.d_item_p;
    d_constraint_p = binding.d_constraint_p;
}

inline
void bdem_ConstChoiceBinding::rebind(const bdem_ChoiceArrayItem *item)
{
    BSLS_ASSERT_SAFE(item);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *item,
                                                             *d_constraint_p));

    d_item_p = item;
}

inline
void bdem_ConstChoiceBinding::rebind(const bdem_Choice *choice)
{
    BSLS_ASSERT_SAFE(choice);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             choice->item(),
                                                             *d_constraint_p));

    d_item_p = &choice->item();
}

inline
void bdem_ConstChoiceBinding::rebind(const bdem_ChoiceArray *choiceArray,
                                     int                     choiceIndex)
{
    BSLS_ASSERT_SAFE(choiceArray);
    BSLS_ASSERT_SAFE(0 <= choiceIndex);
    BSLS_ASSERT_SAFE(     choiceIndex < choiceArray->length());

    d_item_p = &(*choiceArray)[choiceIndex];

    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_item_p,
                                                             *d_constraint_p));
}

inline
void bdem_ConstChoiceBinding::rebind(const bdem_ChoiceArrayItem *item,
                                     const bdem_RecordDef       *record)
{
    BSLS_ASSERT_SAFE(item);
    BSLS_ASSERT_SAFE(record);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(*item,
                                                                *record));

    d_item_p       = item;
    d_constraint_p = record;
}

inline
void bdem_ConstChoiceBinding::rebind(const bdem_ChoiceArrayItem *item,
                                     const bdem_Schema          *schema,
                                     const char                 *recordName)
{
    BSLS_ASSERT_SAFE(item);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_item_p       = item;
    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_item_p,
                                                             *d_constraint_p));
}

inline
void bdem_ConstChoiceBinding::rebind(const bdem_ChoiceArrayItem *item,
                                     const bdem_Schema          *schema,
                                     int                         recordIndex)
{
    BSLS_ASSERT_SAFE(item);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->length());

    d_item_p       = item;
    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_item_p,
                                                             *d_constraint_p));
}

inline
void bdem_ConstChoiceBinding::rebind(const bdem_Choice    *choice,
                                     const bdem_RecordDef *record)
{
    BSLS_ASSERT_SAFE(choice);
    BSLS_ASSERT_SAFE(record);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(choice->item(),
                                                                *record));

    d_item_p       = &choice->item();
    d_constraint_p = record;
}

inline
void bdem_ConstChoiceBinding::rebind(const bdem_Choice *choice,
                                     const bdem_Schema *schema,
                                     const char        *recordName)
{
    BSLS_ASSERT_SAFE(choice);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_item_p       = &choice->item();
    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_item_p,
                                                             *d_constraint_p));
}

inline
void bdem_ConstChoiceBinding::rebind(const bdem_Choice *choice,
                                     const bdem_Schema *schema,
                                     int                recordIndex)
{
    BSLS_ASSERT_SAFE(choice);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->length());

    d_item_p       = &choice->item();
    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_item_p,
                                                             *d_constraint_p));
}

inline
void bdem_ConstChoiceBinding::rebind(const bdem_ChoiceArray *choiceArray,
                                     int                     choiceIndex,
                                     const bdem_RecordDef   *record)
{
    BSLS_ASSERT_SAFE(choiceArray);
    BSLS_ASSERT_SAFE(0 <= choiceIndex);
    BSLS_ASSERT_SAFE(     choiceIndex < choiceArray->length());
    BSLS_ASSERT_SAFE(record);

    d_item_p       = &(*choiceArray)[choiceIndex];
    d_constraint_p = record;

    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_item_p,
                                                             *d_constraint_p));
}

inline
void bdem_ConstChoiceBinding::rebind(const bdem_ChoiceArray *choiceArray,
                                     int                     choiceIndex,
                                     const bdem_Schema      *schema,
                                     const char             *recordName)
{
    BSLS_ASSERT_SAFE(choiceArray);
    BSLS_ASSERT_SAFE(0 <= choiceIndex);
    BSLS_ASSERT_SAFE(     choiceIndex < choiceArray->length());
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_item_p       = &(*choiceArray)[choiceIndex];
    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_item_p,
                                                             *d_constraint_p));
}

inline
void bdem_ConstChoiceBinding::rebind(const bdem_ChoiceArray *choiceArray,
                                     int                     choiceIndex,
                                     const bdem_Schema      *schema,
                                     int                     recordIndex)
{
    BSLS_ASSERT_SAFE(choiceArray);
    BSLS_ASSERT_SAFE(0 <= choiceIndex);
    BSLS_ASSERT_SAFE(     choiceIndex < choiceArray->length());
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->length());

    d_item_p       = &(*choiceArray)[choiceIndex];
    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_item_p,
                                                             *d_constraint_p));
}

// ACCESSORS
inline
int bdem_ConstChoiceBinding::numSelections() const
{
    return d_constraint_p->numFields();
}

inline
const bdem_ChoiceArrayItem& bdem_ConstChoiceBinding::item() const
{
    return *d_item_p;
}

inline
const bdem_RecordDef& bdem_ConstChoiceBinding::record() const
{
    return *d_constraint_p;
}

inline
bdem_ConstElemRef bdem_ConstChoiceBinding::selection() const
{
    return d_item_p->selection();
}

inline
bdem_ElemType::Type bdem_ConstChoiceBinding::selectionType(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < numSelections());

    return d_constraint_p->field(index).elemType();
}

inline
bdem_ElemType::Type
bdem_ConstChoiceBinding::selectionType(const char *selectionName) const
{
    BSLS_ASSERT_SAFE(selectionName);

    const bdem_FieldDef *fieldDef = d_constraint_p->lookupField(selectionName);

    BSLS_ASSERT_SAFE(fieldDef);

    return fieldDef->elemType();
}

inline
bdem_ElemType::Type bdem_ConstChoiceBinding::selectionType() const
{
    return d_item_p->selectionType();
}

inline
int bdem_ConstChoiceBinding::selector() const
{
    return d_item_p->selector();
}

inline
int bdem_ConstChoiceBinding::selectorId() const
{
    const int selectionIndex = selector();
    return -1 == selectionIndex
           ? bdem_RecordDef::BDEM_NULL_FIELD_ID
           : d_constraint_p->fieldId(selectionIndex);
}

inline
const char *bdem_ConstChoiceBinding::selectorName() const
{
    const int selectionIndex = selector();
    return -1 == selectionIndex
           ? 0
           : d_constraint_p->fieldName(selectionIndex);
}

inline
const bool& bdem_ConstChoiceBinding::theBool() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL == selectionType());

    return d_item_p->theBool();
}

inline
const char& bdem_ConstChoiceBinding::theChar() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR == selectionType());

    return d_item_p->theChar();
}

inline
const short& bdem_ConstChoiceBinding::theShort() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT == selectionType());

    return d_item_p->theShort();
}

inline
const int& bdem_ConstChoiceBinding::theInt() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT == selectionType());

    return d_item_p->theInt();
}

inline
const bsls_Types::Int64& bdem_ConstChoiceBinding::theInt64() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64 == selectionType());

    return d_item_p->theInt64();
}

inline
const float& bdem_ConstChoiceBinding::theFloat() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT == selectionType());

    return d_item_p->theFloat();
}

inline
const double& bdem_ConstChoiceBinding::theDouble() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE == selectionType());

    return d_item_p->theDouble();
}

inline
const bsl::string& bdem_ConstChoiceBinding::theString() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING == selectionType());

    return d_item_p->theString();
}

inline
const bdet_Datetime& bdem_ConstChoiceBinding::theDatetime() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIME == selectionType());

    return d_item_p->theDatetime();
}

inline
const bdet_DatetimeTz& bdem_ConstChoiceBinding::theDatetimeTz() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIMETZ == selectionType());

    return d_item_p->theDatetimeTz();
}

inline
const bdet_Date& bdem_ConstChoiceBinding::theDate() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE == selectionType());

    return d_item_p->theDate();
}

inline
const bdet_DateTz& bdem_ConstChoiceBinding::theDateTz() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ == selectionType());

    return d_item_p->theDateTz();
}

inline
const bdet_Time& bdem_ConstChoiceBinding::theTime() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME == selectionType());

    return d_item_p->theTime();
}

inline
const bdet_TimeTz& bdem_ConstChoiceBinding::theTimeTz() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ == selectionType());

    return d_item_p->theTimeTz();
}

inline
const bsl::vector<bool>& bdem_ConstChoiceBinding::theBoolArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL_ARRAY == selectionType());

    return d_item_p->theBoolArray();
}

inline
const bsl::vector<char>& bdem_ConstChoiceBinding::theCharArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR_ARRAY == selectionType());

    return d_item_p->theCharArray();
}

inline
const bsl::vector<short>& bdem_ConstChoiceBinding::theShortArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT_ARRAY == selectionType());

    return d_item_p->theShortArray();
}

inline
const bsl::vector<int>& bdem_ConstChoiceBinding::theIntArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT_ARRAY == selectionType());

    return d_item_p->theIntArray();
}

inline
const bsl::vector<bsls_Types::Int64>&
bdem_ConstChoiceBinding::theInt64Array() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64_ARRAY == selectionType());

    return d_item_p->theInt64Array();
}

inline
const bsl::vector<float>& bdem_ConstChoiceBinding::theFloatArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT_ARRAY == selectionType());

    return d_item_p->theFloatArray();
}

inline
const bsl::vector<double>& bdem_ConstChoiceBinding::theDoubleArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE_ARRAY == selectionType());

    return d_item_p->theDoubleArray();
}

inline
const bsl::vector<bsl::string>& bdem_ConstChoiceBinding::theStringArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING_ARRAY == selectionType());

    return d_item_p->theStringArray();
}

inline
const bsl::vector<bdet_Datetime>&
bdem_ConstChoiceBinding::theDatetimeArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIME_ARRAY == selectionType());

    return d_item_p->theDatetimeArray();
}

inline
const bsl::vector<bdet_DatetimeTz>&
bdem_ConstChoiceBinding::theDatetimeTzArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIMETZ_ARRAY == selectionType());

    return d_item_p->theDatetimeTzArray();
}

inline
const bsl::vector<bdet_Date>& bdem_ConstChoiceBinding::theDateArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE_ARRAY == selectionType());

    return d_item_p->theDateArray();
}

inline
const bsl::vector<bdet_DateTz>& bdem_ConstChoiceBinding::theDateTzArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ_ARRAY == selectionType());

    return d_item_p->theDateTzArray();
}

inline
const bsl::vector<bdet_Time>& bdem_ConstChoiceBinding::theTimeArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME_ARRAY == selectionType());

    return d_item_p->theTimeArray();
}

inline
const bsl::vector<bdet_TimeTz>& bdem_ConstChoiceBinding::theTimeTzArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ_ARRAY == selectionType());

    return d_item_p->theTimeTzArray();
}

inline
const bdem_Choice& bdem_ConstChoiceBinding::theChoice() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == selectionType());

    return d_item_p->theChoice();
}

inline
const bdem_ChoiceArray& bdem_ConstChoiceBinding::theChoiceArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE_ARRAY == selectionType());

    return d_item_p->theChoiceArray();
}

inline
const bdem_List& bdem_ConstChoiceBinding::theList() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == selectionType());

    return d_item_p->theList();
}

inline
const bdem_Table& bdem_ConstChoiceBinding::theTable() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == selectionType());

    return d_item_p->theTable();
}

inline
bdem_ConstChoiceBinding bdem_ConstChoiceBinding::choiceBinding() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == selectionType());

    return bdem_ConstChoiceBinding(
                         &d_item_p->theChoice().item(),
                         d_constraint_p->field(selector()).recordConstraint());
}

inline
bdem_ConstChoiceArrayBinding
bdem_ConstChoiceBinding::choiceArrayBinding() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE_ARRAY == selectionType());

    return bdem_ConstChoiceArrayBinding(
                         &d_item_p->theChoiceArray(),
                         d_constraint_p->field(selector()).recordConstraint());
}

inline
bdem_ConstRowBinding bdem_ConstChoiceBinding::rowBinding() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == selectionType());

    return bdem_ConstRowBinding(
                         &d_item_p->theList().row(),
                         d_constraint_p->field(selector()).recordConstraint());
}

inline
bdem_ConstTableBinding bdem_ConstChoiceBinding::tableBinding() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == selectionType());

    return bdem_ConstTableBinding(
                         &d_item_p->theTable(),
                         d_constraint_p->field(selector()).recordConstraint());
}

// FREE OPERATORS
inline
bool operator==(const bdem_ConstChoiceBinding& lhs,
                const bdem_ConstChoiceBinding& rhs)
{
    return bdem_SchemaUtil::areEquivalent(*lhs.d_constraint_p,
                                          *rhs.d_constraint_p)
        && *lhs.d_item_p == *rhs.d_item_p;
}

inline
bool operator!=(const bdem_ConstChoiceBinding& lhs,
                const bdem_ConstChoiceBinding& rhs)
{
    return !(lhs == rhs);
}

                        // ------------------------
                        // class bdem_ChoiceBinding
                        // ------------------------

                        // -----------------
                        // Level-0 Functions
                        // -----------------

// ACCESSORS
inline
bdem_ChoiceArrayItem& bdem_ChoiceBinding::item() const
{
    return *const_cast<bdem_ChoiceArrayItem *>(d_item_p);
}

// CREATORS
inline
bdem_ChoiceBinding::bdem_ChoiceBinding(bdem_ChoiceArrayItem *item,
                                       const bdem_RecordDef *record)
: bdem_ConstChoiceBinding(item, record)
{
}

inline
bdem_ChoiceBinding::bdem_ChoiceBinding(bdem_ChoiceArrayItem *item,
                                       const bdem_Schema    *schema,
                                       const char           *recordName)
: bdem_ConstChoiceBinding(item, schema, recordName)
{
}

inline
bdem_ChoiceBinding::bdem_ChoiceBinding(bdem_ChoiceArrayItem *item,
                                       const bdem_Schema    *schema,
                                       int                   recordIndex)
: bdem_ConstChoiceBinding(item, schema, recordIndex)
{
}

inline
bdem_ChoiceBinding::bdem_ChoiceBinding(bdem_Choice          *choice,
                                       const bdem_RecordDef *record)
: bdem_ConstChoiceBinding(&choice->item(), record)
{
}

inline
bdem_ChoiceBinding::bdem_ChoiceBinding(bdem_Choice       *choice,
                                       const bdem_Schema *schema,
                                       const char        *recordName)
: bdem_ConstChoiceBinding(&choice->item(), schema, recordName)
{
}

inline
bdem_ChoiceBinding::bdem_ChoiceBinding(bdem_Choice       *choice,
                                       const bdem_Schema *schema,
                                       int                recordIndex)
: bdem_ConstChoiceBinding(&choice->item(), schema, recordIndex)
{
}

inline
bdem_ChoiceBinding::bdem_ChoiceBinding(bdem_ChoiceArray     *choiceArray,
                                       int                   choiceIndex,
                                       const bdem_RecordDef *record)
: bdem_ConstChoiceBinding(choiceArray, choiceIndex, record)
{
}

inline
bdem_ChoiceBinding::bdem_ChoiceBinding(bdem_ChoiceArray  *choiceArray,
                                       int                choiceIndex,
                                       const bdem_Schema *schema,
                                       const char        *recordName)
: bdem_ConstChoiceBinding(choiceArray, choiceIndex, schema, recordName)
{
}

inline
bdem_ChoiceBinding::bdem_ChoiceBinding(bdem_ChoiceArray  *choiceArray,
                                       int                choiceIndex,
                                       const bdem_Schema *schema,
                                       int                recordIndex)
: bdem_ConstChoiceBinding(choiceArray, choiceIndex, schema, recordIndex)
{
}

inline
bdem_ChoiceBinding::bdem_ChoiceBinding(const bdem_ChoiceBinding& original)
: bdem_ConstChoiceBinding(original)
{
}

// MANIPULATORS
inline
void bdem_ChoiceBinding::rebind(const bdem_ChoiceBinding& binding)
{
    bdem_ConstChoiceBinding::rebind(binding);
}

inline
void bdem_ChoiceBinding::rebind(bdem_ChoiceArrayItem *item)
{
    bdem_ConstChoiceBinding::rebind(item);
}

inline
void bdem_ChoiceBinding::rebind(bdem_Choice *choice)
{
    bdem_ConstChoiceBinding::rebind(choice);
}

inline
void bdem_ChoiceBinding::rebind(bdem_ChoiceArray *choiceArray,
                                int               choiceIndex)
{
    bdem_ConstChoiceBinding::rebind(choiceArray, choiceIndex);
}

inline
void bdem_ChoiceBinding::rebind(bdem_ChoiceArrayItem *item,
                                const bdem_RecordDef *record)
{
    bdem_ConstChoiceBinding::rebind(item, record);
}

inline
void bdem_ChoiceBinding::rebind(bdem_ChoiceArrayItem *item,
                                const bdem_Schema    *schema,
                                const char           *recordName)
{
    bdem_ConstChoiceBinding::rebind(item, schema, recordName);
}

inline
void bdem_ChoiceBinding::rebind(bdem_ChoiceArrayItem *item,
                                const bdem_Schema    *schema,
                                int                   recordIndex)
{
    bdem_ConstChoiceBinding::rebind(item, schema, recordIndex);
}

inline
void bdem_ChoiceBinding::rebind(bdem_Choice          *choice,
                                const bdem_RecordDef *record)
{
    bdem_ConstChoiceBinding::rebind(choice, record);
}

inline
void bdem_ChoiceBinding::rebind(bdem_Choice       *choice,
                                const bdem_Schema *schema,
                                const char        *recordName)
{
    bdem_ConstChoiceBinding::rebind(choice, schema, recordName);
}

inline
void bdem_ChoiceBinding::rebind(bdem_Choice       *choice,
                                const bdem_Schema *schema,
                                int                recordIndex)
{
    bdem_ConstChoiceBinding::rebind(choice, schema, recordIndex);
}

inline
void bdem_ChoiceBinding::rebind(bdem_ChoiceArray     *choiceArray,
                                int                   choiceIndex,
                                const bdem_RecordDef *record)
{
    bdem_ConstChoiceBinding::rebind(choiceArray, choiceIndex, record);
}

inline
void bdem_ChoiceBinding::rebind(bdem_ChoiceArray  *choiceArray,
                                int                choiceIndex,
                                const bdem_Schema *schema,
                                const char        *recordName)
{
    bdem_ConstChoiceBinding::rebind(choiceArray,
                                    choiceIndex,
                                    schema,
                                    recordName);
}

inline
void bdem_ChoiceBinding::rebind(bdem_ChoiceArray  *choiceArray,
                                int                choiceIndex,
                                const bdem_Schema *schema,
                                int                recordIndex)
{
    bdem_ConstChoiceBinding::rebind(choiceArray,
                                    choiceIndex,
                                    schema,
                                    recordIndex);
}

inline
bdem_ElemRef bdem_ChoiceBinding::makeSelection(int newSelector)
{
    BSLS_ASSERT_SAFE(-1 <= newSelector);
    BSLS_ASSERT_SAFE(      newSelector < numSelections());

    return item().makeSelection(newSelector);
}

inline
bdem_ElemRef bdem_ChoiceBinding::makeSelection(const char *selectionName)
{
    BSLS_ASSERT_SAFE(selectionName);

    const int fieldIndex = d_constraint_p->fieldIndex(selectionName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return item().makeSelection(fieldIndex);
}

// ACCESSORS
inline
bdem_ElemRef bdem_ChoiceBinding::selection() const
{
    return item().selection();
}

inline
bool& bdem_ChoiceBinding::theModifiableBool() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL == selectionType());

    return item().theModifiableBool();
}

inline
char& bdem_ChoiceBinding::theModifiableChar() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR == selectionType());

    return item().theModifiableChar();
}

inline
short& bdem_ChoiceBinding::theModifiableShort() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT == selectionType());

    return item().theModifiableShort();
}

inline
int& bdem_ChoiceBinding::theModifiableInt() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT == selectionType());

    return item().theModifiableInt();
}

inline
bsls_Types::Int64& bdem_ChoiceBinding::theModifiableInt64() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64 == selectionType());

    return item().theModifiableInt64();
}

inline
float& bdem_ChoiceBinding::theModifiableFloat() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT == selectionType());

    return item().theModifiableFloat();
}

inline
double& bdem_ChoiceBinding::theModifiableDouble() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE == selectionType());

    return item().theModifiableDouble();
}

inline
bsl::string& bdem_ChoiceBinding::theModifiableString() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING == selectionType());

    return item().theModifiableString();
}

inline
bdet_Datetime& bdem_ChoiceBinding::theModifiableDatetime() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIME == selectionType());

    return item().theModifiableDatetime();
}

inline
bdet_DatetimeTz& bdem_ChoiceBinding::theModifiableDatetimeTz() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIMETZ == selectionType());

    return item().theModifiableDatetimeTz();
}

inline
bdet_Date& bdem_ChoiceBinding::theModifiableDate() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE == selectionType());

    return item().theModifiableDate();
}

inline
bdet_DateTz& bdem_ChoiceBinding::theModifiableDateTz() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ == selectionType());

    return item().theModifiableDateTz();
}

inline
bdet_Time& bdem_ChoiceBinding::theModifiableTime() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME == selectionType());

    return item().theModifiableTime();
}

inline
bdet_TimeTz& bdem_ChoiceBinding::theModifiableTimeTz() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ == selectionType());

    return item().theModifiableTimeTz();
}

inline
bsl::vector<bool>& bdem_ChoiceBinding::theModifiableBoolArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL_ARRAY == selectionType());

    return item().theModifiableBoolArray();
}

inline
bsl::vector<char>& bdem_ChoiceBinding::theModifiableCharArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR_ARRAY == selectionType());

    return item().theModifiableCharArray();
}

inline
bsl::vector<short>& bdem_ChoiceBinding::theModifiableShortArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT_ARRAY == selectionType());

    return item().theModifiableShortArray();
}

inline
bsl::vector<int>& bdem_ChoiceBinding::theModifiableIntArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT_ARRAY == selectionType());

    return item().theModifiableIntArray();
}

inline
bsl::vector<bsls_Types::Int64>&
bdem_ChoiceBinding::theModifiableInt64Array() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64_ARRAY == selectionType());

    return item().theModifiableInt64Array();
}

inline
bsl::vector<float>& bdem_ChoiceBinding::theModifiableFloatArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT_ARRAY == selectionType());

    return item().theModifiableFloatArray();
}

inline
bsl::vector<double>& bdem_ChoiceBinding::theModifiableDoubleArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE_ARRAY == selectionType());

    return item().theModifiableDoubleArray();
}

inline
bsl::vector<bsl::string>& bdem_ChoiceBinding::theModifiableStringArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING_ARRAY == selectionType());

    return item().theModifiableStringArray();
}

inline
bsl::vector<bdet_Datetime>&
bdem_ChoiceBinding::theModifiableDatetimeArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIME_ARRAY == selectionType());

    return item().theModifiableDatetimeArray();
}

inline
bsl::vector<bdet_DatetimeTz>&
bdem_ChoiceBinding::theModifiableDatetimeTzArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIMETZ_ARRAY == selectionType());

    return item().theModifiableDatetimeTzArray();
}

inline
bsl::vector<bdet_Date>& bdem_ChoiceBinding::theModifiableDateArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE_ARRAY == selectionType());

    return item().theModifiableDateArray();
}

inline
bsl::vector<bdet_DateTz>& bdem_ChoiceBinding::theModifiableDateTzArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ_ARRAY == selectionType());

    return item().theModifiableDateTzArray();
}

inline
bsl::vector<bdet_Time>& bdem_ChoiceBinding::theModifiableTimeArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME_ARRAY == selectionType());

    return item().theModifiableTimeArray();
}

inline
bsl::vector<bdet_TimeTz>& bdem_ChoiceBinding::theModifiableTimeTzArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ_ARRAY == selectionType());

    return item().theModifiableTimeTzArray();
}

inline
bdem_Choice& bdem_ChoiceBinding::theModifiableChoice() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == selectionType());

    return item().theModifiableChoice();
}

inline
bdem_ChoiceArray& bdem_ChoiceBinding::theModifiableChoiceArray() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE_ARRAY == selectionType());

    return item().theModifiableChoiceArray();
}

inline
bdem_List& bdem_ChoiceBinding::theModifiableList() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == selectionType());

    return item().theModifiableList();
}

inline
bdem_Table& bdem_ChoiceBinding::theModifiableTable() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == selectionType());

    return item().theModifiableTable();
}

inline
bdem_ChoiceBinding bdem_ChoiceBinding::choiceBinding() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == selectionType());

    return bdem_ChoiceBinding(
                         &theModifiableChoice().item(),
                         d_constraint_p->field(selector()).recordConstraint());
}

inline
bdem_ChoiceArrayBinding bdem_ChoiceBinding::choiceArrayBinding() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE_ARRAY == selectionType());

    return bdem_ChoiceArrayBinding(
                         &theModifiableChoiceArray(),
                         d_constraint_p->field(selector()).recordConstraint());
}

inline
bdem_RowBinding bdem_ChoiceBinding::rowBinding() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == selectionType());

    return bdem_RowBinding(
                         &theModifiableList().row(),
                         d_constraint_p->field(selector()).recordConstraint());
}

inline
bdem_TableBinding bdem_ChoiceBinding::tableBinding() const
{
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == selectionType());

    return bdem_TableBinding(
                         &theModifiableTable(),
                         d_constraint_p->field(selector()).recordConstraint());
}

                        // ----------------------------------
                        // class bdem_ConstChoiceArrayBinding
                        // ----------------------------------

// CREATORS
inline
bdem_ConstChoiceArrayBinding::bdem_ConstChoiceArrayBinding(
                                           const bdem_ChoiceArray *choiceArray,
                                           const bdem_RecordDef   *record)
: d_choiceArray_p(choiceArray)
, d_constraint_p(record)
{
    BSLS_ASSERT_SAFE(choiceArray);
    BSLS_ASSERT_SAFE(record);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_choiceArray_p,
                                                             *d_constraint_p));
}

inline
bdem_ConstChoiceArrayBinding::bdem_ConstChoiceArrayBinding(
                                          const bdem_ChoiceArray *choiceArray,
                                          const bdem_Schema      *schema,
                                          const char             *recordName)
: d_choiceArray_p(choiceArray)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(choiceArray);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_constraint_p = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_choiceArray_p,
                                                             *d_constraint_p));
}

inline
bdem_ConstChoiceArrayBinding::bdem_ConstChoiceArrayBinding(
                                           const bdem_ChoiceArray *choiceArray,
                                           const bdem_Schema      *schema,
                                           int                     recordIndex)
: d_choiceArray_p(choiceArray)
, d_constraint_p(0)
{
    BSLS_ASSERT_SAFE(choiceArray);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->length());

    d_constraint_p = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_choiceArray_p,
                                                             *d_constraint_p));
}

inline
bdem_ConstChoiceArrayBinding::bdem_ConstChoiceArrayBinding(
                                  const bdem_ConstChoiceArrayBinding& original)
: d_choiceArray_p(original.d_choiceArray_p)
, d_constraint_p(original.d_constraint_p)
{
}

// MANIPULATORS
inline
void bdem_ConstChoiceArrayBinding::rebind(
                                   const bdem_ConstChoiceArrayBinding& binding)
{
    d_choiceArray_p = binding.d_choiceArray_p;
    d_constraint_p  = binding.d_constraint_p;
}

inline
void bdem_ConstChoiceArrayBinding::rebind(const bdem_ChoiceArray *choiceArray)
{
    BSLS_ASSERT_SAFE(choiceArray);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *choiceArray,
                                                             *d_constraint_p));

    d_choiceArray_p = choiceArray;
}

inline
void bdem_ConstChoiceArrayBinding::rebind(const bdem_ChoiceArray *choiceArray,
                                          const bdem_RecordDef   *record)
{
    BSLS_ASSERT_SAFE(choiceArray);
    BSLS_ASSERT_SAFE(record);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(*choiceArray,
                                                                *record));

    d_choiceArray_p = choiceArray;
    d_constraint_p  = record;
}

inline
void bdem_ConstChoiceArrayBinding::rebind(const bdem_ChoiceArray *choiceArray,
                                          const bdem_Schema      *schema,
                                          const char             *recordName)
{
    BSLS_ASSERT_SAFE(choiceArray);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(recordName);

    d_choiceArray_p = choiceArray;
    d_constraint_p  = schema->lookupRecord(recordName);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_choiceArray_p,
                                                             *d_constraint_p));
}

inline
void bdem_ConstChoiceArrayBinding::rebind(const bdem_ChoiceArray *choiceArray,
                                          const bdem_Schema      *schema,
                                          int                     recordIndex)
{
    BSLS_ASSERT_SAFE(choiceArray);
    BSLS_ASSERT_SAFE(schema);
    BSLS_ASSERT_SAFE(0 <= recordIndex);
    BSLS_ASSERT_SAFE(     recordIndex < schema->length());

    d_choiceArray_p = choiceArray;
    d_constraint_p  = &schema->record(recordIndex);

    BSLS_ASSERT_SAFE(d_constraint_p);
    BSLS_ASSERT_SAFE(bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                             *d_choiceArray_p,
                                                             *d_constraint_p));
}

// ACCESSORS
inline
const bdem_ChoiceArrayItem&
bdem_ConstChoiceArrayBinding::operator[](int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return (*d_choiceArray_p)[itemIndex];
}

inline
const bdem_ChoiceArray& bdem_ConstChoiceArrayBinding::choiceArray() const
{
    return *d_choiceArray_p;
}

inline
const bdem_RecordDef& bdem_ConstChoiceArrayBinding::record() const
{
    return *d_constraint_p;
}

inline
int bdem_ConstChoiceArrayBinding::length() const
{
    return d_choiceArray_p->length();
}

inline
int bdem_ConstChoiceArrayBinding::numSelections() const
{
    return d_constraint_p->numFields();
}

inline
bdem_ElemType::Type
bdem_ConstChoiceArrayBinding::selectionType(int selectionIndex) const
{
    BSLS_ASSERT_SAFE(0 <= selectionIndex);
    BSLS_ASSERT_SAFE(     selectionIndex < numSelections());

    return d_constraint_p->field(selectionIndex).elemType();
}

inline
bdem_ElemType::Type
bdem_ConstChoiceArrayBinding::selectionType(const char *selectionName) const
{
    BSLS_ASSERT_SAFE(selectionName);

    const bdem_FieldDef *fieldDef = d_constraint_p->lookupField(selectionName);

    BSLS_ASSERT_SAFE(fieldDef);

    return fieldDef->elemType();
}

inline
bdem_ElemType::Type
bdem_ConstChoiceArrayBinding::selectorType(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return (*d_choiceArray_p)[itemIndex].selectionType();
}

inline
bdem_ConstElemRef bdem_ConstChoiceArrayBinding::selection(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return (*d_choiceArray_p)[itemIndex].selection();
}

inline
int bdem_ConstChoiceArrayBinding::selector(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return (*d_choiceArray_p)[itemIndex].selector();
}

inline
int bdem_ConstChoiceArrayBinding::selectorId(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    const int selectionIndex = selector(itemIndex);

    return -1 == selectionIndex
           ? bdem_RecordDef::BDEM_NULL_FIELD_ID
           : d_constraint_p->fieldId(selectionIndex);
}

inline
const char *bdem_ConstChoiceArrayBinding::selectorName(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    const int selectionIndex = selector(itemIndex);

    return -1 == selectionIndex
           ? 0
           : d_constraint_p->fieldName(selectionIndex);
}

inline
const bool& bdem_ConstChoiceArrayBinding::theBool(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theBool();
}

inline
const char& bdem_ConstChoiceArrayBinding::theChar(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theChar();
}

inline
const short& bdem_ConstChoiceArrayBinding::theShort(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theShort();
}

inline
const int& bdem_ConstChoiceArrayBinding::theInt(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theInt();
}

inline
const bsls_Types::Int64&
bdem_ConstChoiceArrayBinding::theInt64(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64 == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theInt64();
}

inline
const float& bdem_ConstChoiceArrayBinding::theFloat(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theFloat();
}

inline
const double& bdem_ConstChoiceArrayBinding::theDouble(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theDouble();
}

inline
const bsl::string& bdem_ConstChoiceArrayBinding::theString(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theString();
}

inline
const bdet_Datetime&
bdem_ConstChoiceArrayBinding::theDatetime(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIME == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theDatetime();
}

inline
const bdet_DatetimeTz&
bdem_ConstChoiceArrayBinding::theDatetimeTz(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                    bdem_ElemType::BDEM_DATETIMETZ == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theDatetimeTz();
}

inline
const bdet_Date& bdem_ConstChoiceArrayBinding::theDate(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theDate();
}

inline
const bdet_DateTz& bdem_ConstChoiceArrayBinding::theDateTz(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theDateTz();
}

inline
const bdet_Time& bdem_ConstChoiceArrayBinding::theTime(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theTime();
}

inline
const bdet_TimeTz& bdem_ConstChoiceArrayBinding::theTimeTz(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theTimeTz();
}

inline
const bsl::vector<bool>&
bdem_ConstChoiceArrayBinding::theBoolArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                    bdem_ElemType::BDEM_BOOL_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theBoolArray();
}

inline
const bsl::vector<char>&
bdem_ConstChoiceArrayBinding::theCharArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                    bdem_ElemType::BDEM_CHAR_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theCharArray();
}

inline
const bsl::vector<short>&
bdem_ConstChoiceArrayBinding::theShortArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                   bdem_ElemType::BDEM_SHORT_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theShortArray();
}

inline
const bsl::vector<int>&
bdem_ConstChoiceArrayBinding::theIntArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theIntArray();
}

inline
const bsl::vector<bsls_Types::Int64>&
bdem_ConstChoiceArrayBinding::theInt64Array(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                   bdem_ElemType::BDEM_INT64_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theInt64Array();
}

inline
const bsl::vector<float>&
bdem_ConstChoiceArrayBinding::theFloatArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                   bdem_ElemType::BDEM_FLOAT_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theFloatArray();
}

inline
const bsl::vector<double>&
bdem_ConstChoiceArrayBinding::theDoubleArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                  bdem_ElemType::BDEM_DOUBLE_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theDoubleArray();
}

inline
const bsl::vector<bsl::string>&
bdem_ConstChoiceArrayBinding::theStringArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                  bdem_ElemType::BDEM_STRING_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theStringArray();
}

inline
const bsl::vector<bdet_Datetime>&
bdem_ConstChoiceArrayBinding::theDatetimeArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                bdem_ElemType::BDEM_DATETIME_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theDatetimeArray();
}

inline
const bsl::vector<bdet_DatetimeTz>&
bdem_ConstChoiceArrayBinding::theDatetimeTzArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
              bdem_ElemType::BDEM_DATETIMETZ_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theDatetimeTzArray();
}

inline
const bsl::vector<bdet_Date>&
bdem_ConstChoiceArrayBinding::theDateArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                    bdem_ElemType::BDEM_DATE_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theDateArray();
}

inline
const bsl::vector<bdet_DateTz>&
bdem_ConstChoiceArrayBinding::theDateTzArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                  bdem_ElemType::BDEM_DATETZ_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theDateTzArray();
}

inline
const bsl::vector<bdet_Time>&
bdem_ConstChoiceArrayBinding::theTimeArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                    bdem_ElemType::BDEM_TIME_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theTimeArray();
}

inline
const bsl::vector<bdet_TimeTz>&
bdem_ConstChoiceArrayBinding::theTimeTzArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                  bdem_ElemType::BDEM_TIMETZ_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theTimeTzArray();
}

inline
const bdem_Choice&
bdem_ConstChoiceArrayBinding::theChoice(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theChoice();
}

inline
const bdem_ChoiceArray&
bdem_ConstChoiceArrayBinding::theChoiceArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                  bdem_ElemType::BDEM_CHOICE_ARRAY == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theChoiceArray();
}

inline
const bdem_List&
bdem_ConstChoiceArrayBinding::theList(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theList();
}

inline
const bdem_Table&
bdem_ConstChoiceArrayBinding::theTable(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == selectorType(itemIndex));

    return (*d_choiceArray_p)[itemIndex].theTable();
}

inline
bdem_ConstChoiceBinding
bdem_ConstChoiceArrayBinding::choiceBinding(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == selectorType(itemIndex));

    return bdem_ConstChoiceBinding(&(*d_choiceArray_p)[itemIndex],
                                   d_constraint_p);
}

inline
bdem_ConstChoiceArrayBinding
bdem_ConstChoiceArrayBinding::choiceArrayBinding(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                  bdem_ElemType::BDEM_CHOICE_ARRAY == selectorType(itemIndex));

    return bdem_ConstChoiceArrayBinding(
                &(*d_choiceArray_p)[itemIndex].theChoiceArray(),
                d_constraint_p->field(selector(itemIndex)).recordConstraint());
}

inline
bdem_ConstRowBinding
bdem_ConstChoiceArrayBinding::rowBinding(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == selectorType(itemIndex));

    return bdem_ConstRowBinding(
                &(*d_choiceArray_p)[itemIndex].theList().row(),
                d_constraint_p->field(selector(itemIndex)).recordConstraint());
}

inline
bdem_ConstTableBinding
bdem_ConstChoiceArrayBinding::tableBinding(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == selectorType(itemIndex));

    return bdem_ConstTableBinding(
                &(*d_choiceArray_p)[itemIndex].theTable(),
                d_constraint_p->field(selector(itemIndex)).recordConstraint());
}

// FREE OPERATORS
inline
bool operator==(const bdem_ConstChoiceArrayBinding& lhs,
                const bdem_ConstChoiceArrayBinding& rhs)
{
    return bdem_SchemaUtil::areEquivalent(*lhs.d_constraint_p,
                                          *rhs.d_constraint_p)
        && *lhs.d_choiceArray_p == *rhs.d_choiceArray_p;
}

inline
bool operator!=(const bdem_ConstChoiceArrayBinding& lhs,
                const bdem_ConstChoiceArrayBinding& rhs)
{
    return !(lhs == rhs);
}

                        // -----------------------------
                        // class bdem_ChoiceArrayBinding
                        // -----------------------------

                        // -----------------
                        // Level-0 Functions
                        // -----------------

// ACCESSORS
inline
bdem_ChoiceArray& bdem_ChoiceArrayBinding::choiceArray() const
{
    return *const_cast<bdem_ChoiceArray *>(d_choiceArray_p);
}

// CREATORS
inline
bdem_ChoiceArrayBinding::bdem_ChoiceArrayBinding(
                                            bdem_ChoiceArray     *choiceArray,
                                            const bdem_RecordDef *record)
: bdem_ConstChoiceArrayBinding(choiceArray, record)
{
}

inline
bdem_ChoiceArrayBinding::bdem_ChoiceArrayBinding(
                                               bdem_ChoiceArray  *choiceArray,
                                               const bdem_Schema *schema,
                                               const char        *recordName)
: bdem_ConstChoiceArrayBinding(choiceArray, schema, recordName)
{
}

inline
bdem_ChoiceArrayBinding::bdem_ChoiceArrayBinding(
                                               bdem_ChoiceArray  *choiceArray,
                                               const bdem_Schema *schema,
                                               int                recordIndex)
: bdem_ConstChoiceArrayBinding(choiceArray, schema, recordIndex)
{
}

inline
bdem_ChoiceArrayBinding::bdem_ChoiceArrayBinding(
                                      const bdem_ChoiceArrayBinding& original)
: bdem_ConstChoiceArrayBinding(original)
{
}

// MANIPULATORS
inline
void bdem_ChoiceArrayBinding::rebind(const bdem_ChoiceArrayBinding& binding)
{
    bdem_ConstChoiceArrayBinding::rebind(binding);
}

inline
void bdem_ChoiceArrayBinding::rebind(bdem_ChoiceArray *choiceArray)
{
    bdem_ConstChoiceArrayBinding::rebind(choiceArray);
}

inline
void bdem_ChoiceArrayBinding::rebind(bdem_ChoiceArray     *choiceArray,
                                     const bdem_RecordDef *record)
{
    bdem_ConstChoiceArrayBinding::rebind(choiceArray, record);
}

inline
void bdem_ChoiceArrayBinding::rebind(bdem_ChoiceArray  *choiceArray,
                                     const bdem_Schema *schema,
                                     const char        *recordName)
{
    bdem_ConstChoiceArrayBinding::rebind(choiceArray, schema, recordName);
}

inline
void bdem_ChoiceArrayBinding::rebind(bdem_ChoiceArray  *choiceArray,
                                     const bdem_Schema *schema,
                                     int                recordIndex)
{
    bdem_ConstChoiceArrayBinding::rebind(choiceArray, schema, recordIndex);
}

inline
bdem_ElemRef
bdem_ChoiceArrayBinding::makeSelection(int itemIndex, int newSelector)
{
    BSLS_ASSERT_SAFE( 0 <= itemIndex);
    BSLS_ASSERT_SAFE(      itemIndex   < length());
    BSLS_ASSERT_SAFE(-1 <= newSelector);
    BSLS_ASSERT_SAFE(      newSelector < numSelections());

    return choiceArray().theModifiableItem(itemIndex).makeSelection(
                                                                  newSelector);
}

inline
bdem_ElemRef bdem_ChoiceArrayBinding::makeSelection(int         itemIndex,
                                                    const char *selectionName)
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    BSLS_ASSERT_SAFE(selectionName);

    const int fieldIndex = d_constraint_p->fieldIndex(selectionName);

    BSLS_ASSERT_SAFE(0 <= fieldIndex);

    return choiceArray().theModifiableItem(itemIndex).makeSelection(
                                                                   fieldIndex);
}

// ACCESSORS
inline
bdem_ChoiceArrayItem&
bdem_ChoiceArrayBinding::operator[](int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return choiceArray().theModifiableItem(itemIndex);
}

inline
bdem_ElemRef bdem_ChoiceArrayBinding::selection(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return choiceArray().theModifiableItem(itemIndex).selection();
}

inline
bdem_ChoiceArrayItem&
bdem_ChoiceArrayBinding::theModifiableItem(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return choiceArray().theModifiableItem(itemIndex);
}

inline
bool& bdem_ChoiceArrayBinding::theModifiableBool(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL == item.selectionType());

    return item.theModifiableBool();
}

inline
char& bdem_ChoiceArrayBinding::theModifiableChar(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR == item.selectionType());

    return item.theModifiableChar();
}

inline
short& bdem_ChoiceArrayBinding::theModifiableShort(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT == item.selectionType());

    return item.theModifiableShort();
}

inline
int& bdem_ChoiceArrayBinding::theModifiableInt(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT == item.selectionType());

    return item.theModifiableInt();
}

inline
bsls_Types::Int64&
bdem_ChoiceArrayBinding::theModifiableInt64(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64 == item.selectionType());

    return item.theModifiableInt64();
}

inline
float& bdem_ChoiceArrayBinding::theModifiableFloat(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT == item.selectionType());

    return item.theModifiableFloat();
}

inline
double& bdem_ChoiceArrayBinding::theModifiableDouble(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE == item.selectionType());

    return item.theModifiableDouble();
}

inline
bsl::string& bdem_ChoiceArrayBinding::theModifiableString(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING == item.selectionType());

    return item.theModifiableString();
}

inline
bdet_Datetime&
bdem_ChoiceArrayBinding::theModifiableDatetime(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIME == item.selectionType());

    return item.theModifiableDatetime();
}

inline
bdet_DatetimeTz&
bdem_ChoiceArrayBinding::theModifiableDatetimeTz(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETIMETZ == item.selectionType());

    return item.theModifiableDatetimeTz();
}

inline
bdet_Date& bdem_ChoiceArrayBinding::theModifiableDate(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE == item.selectionType());

    return item.theModifiableDate();
}

inline
bdet_DateTz& bdem_ChoiceArrayBinding::theModifiableDateTz(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ == item.selectionType());

    return item.theModifiableDateTz();
}

inline
bdet_Time& bdem_ChoiceArrayBinding::theModifiableTime(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME == item.selectionType());

    return item.theModifiableTime();
}

inline
bdet_TimeTz& bdem_ChoiceArrayBinding::theModifiableTimeTz(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ == item.selectionType());

    return item.theModifiableTimeTz();
}

inline
bsl::vector<bool>&
bdem_ChoiceArrayBinding::theModifiableBoolArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL_ARRAY == item.selectionType());

    return item.theModifiableBoolArray();
}

inline
bsl::vector<char>&
bdem_ChoiceArrayBinding::theModifiableCharArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR_ARRAY == item.selectionType());

    return item.theModifiableCharArray();
}

inline
bsl::vector<short>&
bdem_ChoiceArrayBinding::theModifiableShortArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT_ARRAY == item.selectionType());

    return item.theModifiableShortArray();
}

inline
bsl::vector<int>&
bdem_ChoiceArrayBinding::theModifiableIntArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT_ARRAY == item.selectionType());

    return item.theModifiableIntArray();
}

inline
bsl::vector<bsls_Types::Int64>&
bdem_ChoiceArrayBinding::theModifiableInt64Array(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64_ARRAY == item.selectionType());

    return item.theModifiableInt64Array();
}

inline
bsl::vector<float>&
bdem_ChoiceArrayBinding::theModifiableFloatArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT_ARRAY == item.selectionType());

    return item.theModifiableFloatArray();
}

inline
bsl::vector<double>&
bdem_ChoiceArrayBinding::theModifiableDoubleArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE_ARRAY == item.selectionType());

    return item.theModifiableDoubleArray();
}

inline
bsl::vector<bsl::string>&
bdem_ChoiceArrayBinding::theModifiableStringArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING_ARRAY == item.selectionType());

    return item.theModifiableStringArray();
}

inline
bsl::vector<bdet_Datetime>&
bdem_ChoiceArrayBinding::theModifiableDatetimeArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(
                   bdem_ElemType::BDEM_DATETIME_ARRAY == item.selectionType());

    return item.theModifiableDatetimeArray();
}

inline
bsl::vector<bdet_DatetimeTz>&
bdem_ChoiceArrayBinding::theModifiableDatetimeTzArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_DATETIMETZ_ARRAY == item.selectionType());

    return item.theModifiableDatetimeTzArray();
}

inline
bsl::vector<bdet_Date>&
bdem_ChoiceArrayBinding::theModifiableDateArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE_ARRAY == item.selectionType());

    return item.theModifiableDateArray();
}

inline
bsl::vector<bdet_DateTz>&
bdem_ChoiceArrayBinding::theModifiableDateTzArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ_ARRAY == item.selectionType());

    return item.theModifiableDateTzArray();
}

inline
bsl::vector<bdet_Time>&
bdem_ChoiceArrayBinding::theModifiableTimeArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME_ARRAY == item.selectionType());

    return item.theModifiableTimeArray();
}

inline
bsl::vector<bdet_TimeTz>&
bdem_ChoiceArrayBinding::theModifiableTimeTzArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ_ARRAY == item.selectionType());

    return item.theModifiableTimeTzArray();
}

inline
bdem_Choice& bdem_ChoiceArrayBinding::theModifiableChoice(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == item.selectionType());

    return item.theModifiableChoice();
}

inline
bdem_ChoiceArray&
bdem_ChoiceArrayBinding::theModifiableChoiceArray(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE_ARRAY == item.selectionType());

    return item.theModifiableChoiceArray();
}

inline
bdem_List& bdem_ChoiceArrayBinding::theModifiableList(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == item.selectionType());

    return item.theModifiableList();
}

inline
bdem_Table& bdem_ChoiceArrayBinding::theModifiableTable(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    bdem_ChoiceArrayItem& item = theModifiableItem(itemIndex);

    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == item.selectionType());

    return item.theModifiableTable();
}

inline
bdem_ChoiceBinding bdem_ChoiceArrayBinding::choiceBinding(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());

    return bdem_ChoiceBinding(&theModifiableItem(itemIndex), d_constraint_p);
}

inline
bdem_ChoiceArrayBinding
bdem_ChoiceArrayBinding::choiceArrayBinding(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(
                  bdem_ElemType::BDEM_CHOICE_ARRAY == selectorType(itemIndex));

    return bdem_ChoiceArrayBinding(
                &theModifiableItem(itemIndex).theModifiableChoiceArray(),
                d_constraint_p->field(selector(itemIndex)).recordConstraint());
}

inline
bdem_RowBinding bdem_ChoiceArrayBinding::rowBinding(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == selectorType(itemIndex));

    return bdem_RowBinding(
                &theModifiableItem(itemIndex).theModifiableList().row(),
                d_constraint_p->field(selector(itemIndex)).recordConstraint());
}

inline
bdem_TableBinding bdem_ChoiceArrayBinding::tableBinding(int itemIndex) const
{
    BSLS_ASSERT_SAFE(0 <= itemIndex);
    BSLS_ASSERT_SAFE(     itemIndex < length());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == selectorType(itemIndex));

    return bdem_TableBinding(
                &theModifiableItem(itemIndex).theModifiableTable(),
                d_constraint_p->field(selector(itemIndex)).recordConstraint());
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
