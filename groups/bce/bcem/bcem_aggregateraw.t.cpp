// bcem_aggregateraw.t.cpp                                            -*-C++-*-

#include <bcem_aggregateraw.h>

#include <bcema_sharedptr.h>

#include <bdem_choice.h>
#include <bdem_choicearray.h>
#include <bdem_elemattrlookup.h>
#include <bdem_elemref.h>
#include <bdem_elemtype.h>
#include <bdem_list.h>
#include <bdem_properties.h>
#include <bdem_row.h>
#include <bdem_schema.h>
#include <bdem_schemaaggregateutil.h>
#include <bdem_schemautil.h>
#include <bdem_table.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bsl_iostream.h>


using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
#define T_() cout << '\t' << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bcem_AggregateRaw   Obj;
typedef bcem_AggregateError Error;
typedef bdem_ElemType       ET;

static bool streq(const char *a, const char *b)
    // Return true if null-terminated strings 'a' and 'b' are equal.  Returns
    // true if both 'a' and 'b' are null and false if either 'a' or 'b', but
    // not both, are null.
{
    if (a == b) {
        return true;
    }
    else if (0 == a || 0 == b) {
        return false;
    }
    else {
        return 0 == bsl::strcmp(a, b);
    }
}


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Test that the usage example compiles
        // --------------------------------------------------------------------

          if (verbose) bsl::cout << bsl::endl << "Usage Example"
                                 << bsl::endl << "============="
                                 << bsl::endl;

          Obj object;

          for (int i = 0; i < object.length(); ++i) {
              bcem_AggregateRaw field;
              bcem_AggregateError error;
              if (0 == object.fieldByIndex(&field, &error, i)) {
                  field.print(bsl::cout, 0, -1);
              }
          }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Confirm that all the functions are called at least once
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        if (verbose) cout << "Testing scalar constructors" << endl;
        {
            bslma_TestAllocator ta(veryVeryVerbose);

            Obj agg1;
            ASSERT(agg1.dataType() == ET::BDEM_VOID);
            ASSERT(agg1.isNull());
            if (verbose) P(agg1);

            int intData = 55;
            Obj agg2;
            agg2.setDataType(ET::BDEM_INT);
            agg2.setDataPointer(&intData);
            ASSERT(agg2.dataType() == ET::BDEM_INT);
            ASSERT(!agg2.isNull());
            ASSERT(agg2.asInt() == intData);
            ASSERT(agg2.asString() == "55");
            if (verbose) P(agg2);

            string stringData = "";
            Obj agg3;
            agg3.setDataType(ET::BDEM_STRING);
            agg3.setDataPointer(&stringData);
            ASSERT(agg3.dataType() == ET::BDEM_STRING);
            ASSERT(!agg3.isNull());
            ASSERT(agg3.asString() == stringData);
            if (verbose) P(agg3);
        }

        bslma_TestAllocator sa;
        bcema_SharedPtr<bdem_Schema> schema(new (sa) bdem_Schema(&sa), &sa);

        bdem_RecordDef *level2 = schema->createRecord("Level2");
        level2->appendField(ET::BDEM_STRING_ARRAY, "StringArray2.1");
        bdem_FieldDefAttributes doubleArrayAttr(ET::BDEM_DOUBLE_ARRAY);
        doubleArrayAttr.defaultValue().theModifiableDouble() = 0.125;
        level2->appendField(doubleArrayAttr,  "DoubleArray2.2");
        level2->appendField(ET::BDEM_DATE,         "Date2.3");

        bdem_RecordDef *level1 = schema->createRecord("Level1");
        level1->appendField(ET::BDEM_STRING,       "String1.1");
        level1->appendField(ET::BDEM_DOUBLE,       "Double1.2");
        level1->appendField(ET::BDEM_LIST, level2, "List1.3");
        level1->appendField(ET::BDEM_LIST, level1, "List1.4"); // Recursive

        bdem_RecordDef *table1Row = schema->createRecord("Table1Row");
        table1Row->appendField(ET::BDEM_STRING, "StringField");
        table1Row->appendField(ET::BDEM_DOUBLE, "DoubleField");

        bdem_RecordDef *choice1 = schema->createRecord(
            "Choice1", bdem_RecordDef::BDEM_CHOICE_RECORD);
        choice1->appendField(ET::BDEM_STRING, "NameSelection");
        choice1->appendField(ET::BDEM_INT,    "IdSelection");

        string stringValue = "Hello";
        vector<string> stringArray;
        stringArray.push_back(stringValue);
        double doubleValue = 3.4;
        vector<double> doubleArray;
        doubleArray.push_back(doubleValue);
        bdet_Date      date(2000, 10, 10);

        bdem_List      list2;
        list2.appendStringArray(stringArray);
        list2.appendDoubleArray(doubleArray);
        list2.appendDate(date);

        bdem_List      list1;
        list1.appendString(stringValue);
        list1.appendDouble(doubleValue);
        list1.appendList(list2);
        list1.appendList(list1);

        bdem_List      list3;
        list3.appendString(stringValue);
        list3.appendDouble(doubleValue);

        vector<ET::Type> ct;
        ct.push_back(ET::BDEM_STRING);
        ct.push_back(ET::BDEM_DOUBLE);
        bdem_Table     table(ct);
        table.appendRow(list3);

        ct.clear();
        ct.push_back(ET::BDEM_STRING);
        ct.push_back(ET::BDEM_INT);
        bdem_Choice choice(ct);
        choice.makeSelection(0).theModifiableString() = stringValue;
        {
            bslma_TestAllocator ta;

            if (verbose) cout << "Testing record def constructor"
                              << bsl::endl;
            {
                Obj agg1;
                agg1.setSchemaPointer(schema.ptr());
                agg1.setRecordDefPointer(schema->lookupRecord("Level1"));
                agg1.setDataType(ET::BDEM_LIST);
                agg1.setDataPointer(&list1);
                ASSERT(!agg1.isNull());

                Obj   agg2, agg3, agg4, agg5, agg6;
                Error error;
                int rc = agg1.getField(&agg2, &error, false, "Double1.2");
                ASSERT(!rc);
                double dbl1 = agg2.asDouble();
                ASSERT(3.4 == dbl1);
                P(agg1)

                rc = agg1.setField(&agg3,
                                   &error,
                                   "Double1.2",
                                   1.1);
                ASSERT(!rc);
                ASSERT(agg2.asDouble() == agg3.asDouble());

                double dbl2 = agg1.fieldByIndex(&agg4, &error, 1);
                ASSERT(1.1 == agg4.asDouble());
                ASSERT(!rc);

                bdet_Date d(2010, 1, 1);
                rc = agg1.setField(&agg5,
                                   &error,
                                   "List1.3",
                                   "Date2.3",
                                   d);
                ASSERT(!rc);
                rc = agg1.getField(&agg6, &error, false, "List1.3", "Date2.3");
                ASSERT(!rc);
                bdet_Date date1 = agg6.asDate();
                ASSERT(d == date1);
            }

            if (verbose) cout << "Testing list operations" << bsl::endl;
            {
                Obj agg1;
                agg1.setSchemaPointer(schema.ptr());
                agg1.setRecordDefPointer(schema->lookupRecord("Level1"));
                agg1.setDataType(ET::BDEM_LIST);
                agg1.setDataPointer(&list1);

                Obj   agg2, agg3, agg4;
                Error error;
                int rc = agg1.getField(&agg2, &error, false, "List1.4");
                ASSERT(!rc);
                ASSERT(!agg2.isNull());
                rc = agg1.getField(&agg3,
                                   &error,
                                   false,
                                   "List1.4",
                                   "String1.1");
                ASSERT(!rc);
                ASSERT(!agg3.isNull());
                ASSERT(!agg3.setValue(&error, "List in list"));

                rc = agg1.getField(&agg4,
                                   &error,
                                   false,
                                   "List1.4",
                                   "String1.1");
                ASSERT(!rc);
                ASSERT("List in list" == agg4.asString());
            }

            if (verbose) cout << "Testing array operations" << bsl::endl;
            {
                Obj agg1;
                agg1.setSchemaPointer(schema.ptr());
                agg1.setRecordDefPointer(schema->lookupRecord("Level2"));
                agg1.setDataType(ET::BDEM_LIST);
                agg1.setDataPointer(&list2);

                Obj   agg2, agg3, agg4;
                Error error;
                int rc = agg1.getField(&agg2, &error, false, "StringArray2.1");
                ASSERT(!rc);
                ASSERT(!agg2.isNull());
                rc = agg2.resize(&error, 2);
                ASSERT(!rc);
                ASSERT(2 == agg2.length());
                rc = agg2.arrayItem(&agg3, &error, 0);
                ASSERT(!rc);
                rc = agg3.setValue(&error, "Hello World");
                ASSERT(!rc);
                rc = agg2.arrayItem(&agg4, &error, 0);
                ASSERT(!rc);
                LOOP_ASSERT(agg4.asString(), "Hello World" == agg4.asString());

                Obj agg5;
                agg5.setSchemaPointer(schema.ptr());
                agg5.setRecordDefPointer(schema->lookupRecord("Level1"));
                agg5.setDataType(ET::BDEM_LIST);
                agg5.setDataPointer(&list1);

                Obj agg6, agg7, agg8;
                rc = agg5.getField(&agg6,
                                   &error,
                                   false,
                                   "List1.3",
                                   "DoubleArray2.2");
                ASSERT(!rc);
                ASSERT(1 == agg6.length());
                rc = agg6.insertItem(&agg7, &error, 0, 3.14);
                ASSERT(!rc);
                ASSERT(2 == agg6.length());
                rc = agg6.insertItem(&agg8, &error, 1, 1.2);
                ASSERT(!rc);

                agg7.setValue(&error, 100.23);
                ASSERT(100.23 == agg7.asDouble());
                ASSERT(1.2    == agg8.asDouble());

                agg6.resize(&error, 4);
                ASSERT(4      == agg6.length());
                ASSERT(100.23 == agg7.asDouble());
                ASSERT(1.2    == agg8.asDouble());

                if (veryVerbose) P(agg6);
                agg6.removeItems(&error, 1, 2);
                ASSERT(2 == agg6.length());
            }
        }

        {
            bslma_TestAllocator ta;

            if (verbose) cout << "Testing table operations" << bsl::endl;

            Obj agg1;
            agg1.setSchemaPointer(schema.ptr());
            agg1.setRecordDefPointer(schema->lookupRecord("Table1Row"));
            agg1.setDataType(ET::BDEM_TABLE);
            agg1.setDataPointer(&table);
            ASSERT(1 == agg1.length());

            Obj   agg2, agg3, agg4, agg5;
            Error error;

            int rc = agg1.insertItems(&error, 0, 2);
            ASSERT(!rc);
            ASSERT(3 == agg1.length());

            rc = agg1.setField(&agg2, &error, 0, "StringField", "Hello");
            ASSERT(!rc);
            ASSERT("Hello" == agg2.asString());
            rc = agg2.setValue(&error, "Hello World");
            ASSERT(!rc);

            rc = agg1.setField(&agg3, &error, 0, "DoubleField", 1.2);
            ASSERT(!rc);
            ASSERT(1.2 == agg3.asDouble());
            rc = agg3.setValue(&error, 3.4);
            ASSERT(!rc);

            Obj tmpAgg;
            tmpAgg.setSchemaPointer(schema.ptr());
            tmpAgg.setRecordDefPointer(schema->lookupRecord("Table1Row"));
            tmpAgg.setDataType(ET::BDEM_LIST);
            tmpAgg.setDataPointer(&list3);

            rc = agg1.insertItem(&agg5, &error, 0, tmpAgg);
            ASSERT(!rc);
            ASSERT(4 == agg1.length());

            Obj   agg6, agg7, agg8, agg9;
            rc = agg1.getField(&agg6, &error, false, 0, "StringField");
            ASSERT(!rc);
            ASSERT(stringValue == agg6.asString());
            rc = agg1.getField(&agg7, &error, false, 0, "DoubleField");
            ASSERT(!rc);
            ASSERT(doubleValue == agg7.asDouble());

            agg1.insertItems(&error, 1, 2);
            ASSERT(6 == agg1.length());
            agg1.removeItems(&error, 1, 4);
            ASSERT(2 == agg1.length());
            if (veryVerbose) P(agg1);
        }

        {
            bslma_TestAllocator ta;

            if (verbose) cout << "Testing choice operations" << bsl::endl;

            Obj agg1;
            agg1.setSchemaPointer(schema.ptr());
            agg1.setRecordDefPointer(schema->lookupRecord("Choice1"));
            agg1.setDataType(ET::BDEM_CHOICE);
            agg1.setDataPointer(&choice);

            Obj   agg2, agg3, agg4, agg5;
            Error error;

            ASSERT(2 == agg1.numSelections());
            ASSERT(streq("NameSelection", agg1.selector()));
            ASSERT(0 == agg1.selectorId());
            int rc = agg1.selection(&agg2, &error);
            ASSERT(!rc);
            if (veryVerbose) P(agg1);

            agg1.makeSelection(&agg2, &error, "IdSelection");
            ASSERT(streq("IdSelection", agg1.selector()));
            ASSERT(1 == agg1.selectorId());
            rc = agg1.selection(&agg2, &error);
            ASSERT(!rc);
            if (veryVerbose) P(agg1);
        }

        if (verbose) cout << "Testing clone" << bsl::endl;
        {
            bslma_TestAllocator da("da", veryVeryVerbose);
            bslma_TestAllocator ta1("ta1", veryVeryVerbose);
            bslma_TestAllocator ta2("ta2", veryVeryVerbose);
            const bsl::string recName = "Level1";

            bslma_DefaultAllocatorGuard allocGuard(&da);
            Obj agg1;
            agg1.setSchemaPointer(schema.ptr());
            agg1.setRecordDefPointer(schema->lookupRecord(recName.c_str()));
            agg1.setDataType(ET::BDEM_LIST);
            agg1.setDataPointer(&list1);

            Obj   agg2, agg3, agg4, agg5;
            Error error;
            int rc = agg1.setField(&agg2, &error, "Double1.2", 3.4);
            ASSERT(!rc);
            rc = agg1.setField(&agg2, &error, "List1.3", "Date2.3",
                               "2006-02-16");
            ASSERT(!rc);
            rc = agg1.getField(&agg2, &error, "List1.4");
            ASSERT(!rc);
            agg2.makeValue();
            rc = agg1.setField(&agg2, &error, "List1.4",
                               "String1.1", "List in list");
            ASSERT(!rc);

            const int ta1BlocksUsed = ta1.numBlocksInUse();
            ASSERT(0 == da.numBlocksInUse());
            {
                bcem_Aggregate agg2(agg1.clone(&ta2));

                // New memory came only from ta2
                ASSERT(0 != ta2.numBlocksInUse());
                ASSERT(ta1BlocksUsed == ta1.numBlocksInUse());
                ASSERT(0 == da.numBlocksInUse());

                ASSERT(! Obj::areIdentical(agg1, agg2));
                ASSERT(  Obj::areEquivalent(agg1, agg2));
                ASSERT(agg1.data() != agg2.data());
                ASSERT(&agg1.recordDef() != &agg2.recordDef());
                LOOP_ASSERT(agg2, Obj::areEquivalent(
                                agg1.field("Double1.2"),
                                agg2.field("Double1.2")));
                LOOP_ASSERT(agg2, Obj::areEquivalent(
                                agg1.field("List1.3","Date2.3"),
                                agg2.field("List1.3", "Date2.3")));
                LOOP_ASSERT(agg2, "List in list" ==
                            agg2.field("List1.4", "String1.1").asString());
            }
            ASSERT(0 == ta2.numBlocksInUse());

//             {
//                 bcem_Aggregate agg2(agg1.cloneData(&ta2));

//                 // New memory came only from ta2
//                 ASSERT(0 != ta2.numBlocksInUse());
//                 ASSERT(ta1BlocksUsed == ta1.numBlocksInUse());
//                 ASSERT(0 == da.numBlocksInUse());

//                 ASSERT(! Obj::areIdentical(agg1, agg2));
//                 ASSERT(  Obj::areEquivalent(agg1, agg2));
//                 ASSERT(agg1.data() != agg2.data());
//                 ASSERT(&agg1.recordDef() == &agg2.recordDef());
//                 LOOP_ASSERT(agg2, Obj::areEquivalent(
//                                 agg1.field("Double1.2"),
//                                 agg2.field("Double1.2")));
//                 LOOP_ASSERT(agg2, Obj::areEquivalent(
//                                 agg1.field("List1.3", "Date2.3"),
//                                 agg2.field("List1.3", "Date2.3")));
//                 LOOP_ASSERT(agg2, "List in list" ==
//                             agg2.field("List1.4", "String1.1").asString())
//             }
            ASSERT(0 == ta2.numBlocksInUse());
            ASSERT(0 == da.numBlocksInUse());
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}
