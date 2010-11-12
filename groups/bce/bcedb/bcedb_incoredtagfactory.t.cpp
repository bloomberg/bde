// bcedb_incoredtagfactory.t.cpp  -*-C++-*-
#include <bcedb_incoredtagfactory.h>

#include <bcema_sharedptr.h>
#include <bdem_schema.h>
#include <bdem_schemautil.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

static const struct {
    double      d_spoon;
    int         d_fork;
    const char *d_knife;
} DINNER_TABLE_DATA[] = {
    { 0.199371337890625     ,    100  ,    "@=3-0N*P."          },
    { 0.035064697265625     ,    99   ,    "`u9/0~Z$7"          },
    { 0.13983154296875      ,    98   ,    "X!h>Mpu99YOG("      },
    { 0.00787353515625      ,    97   ,    ";}*a+rJ)7"          },
    { 0.109161376953125     ,    96   ,    "Oq0!>D<:tD"         },
    { 0.021697998046875     ,    95   ,    "Ojj#odO8l+ct"       },
    { 0.93487548828125      ,    94   ,    "~u}48oh8'"          },
    { 0.4381103515625       ,    93   ,    "!?bwqyYjKu ="       },
    { 0.123565673828125     ,    92   ,    "G*Sk]yd49ad"        },
    { 0.74957275390625      ,    91   ,    "*c`LU:3#|dpVZ<0"    },
    { 0.661041259765625     ,    90   ,    "Hello, world!"      },
    { 0.693817138671875     ,    89   ,    "YfpC!hZ\\d+6I"      },
    { 0.792877197265625     ,    88   ,    "=u1Tu}iV,"          },
    { 0.12799072265625      ,    87   ,    "YpSqQA!R."          },
    { 0.12860107421875      ,    86   ,    "#WY2hz}uZ^usR"      },
    { 0.847900390625        ,    85   ,    "9mvhc$.lHV[[I+"     },
    { 0.069366455078125     ,    84   ,    "3_^\nmlR;@$"        },
    { 0.79071044921875      ,    83   ,    "K`x?T:1j9a"         },
    { 0.984527587890625     ,    82   ,    "_)BW?pyuQO`"        },
    { 0.419677734375        ,    81   ,    "avm6IuC!"           },
    { 0.161529541015625     ,    80   ,    "Hellorhighwater"    },
    { 0.213104248046875     ,    79   ,    "xxxc@kVLb"          },
    { 0.706512451171875     ,    78   ,    "Mc,w6Ke$w(uYU"      },
    { 0.51385498046875      ,    77   ,    "-ZOdz!T\tZM"        },
    { 0.701751708984375     ,    76   ,    "+{BLAJlI3iz"        },
    { 0.891815185546875     ,    75   ,    "hl>Zm}9|K"          },
    { 0.97039794921875      ,    74   ,    "`)]<On+>H/Fs"       },
    { 0.9661865234375       ,    73   ,    "FY\6>CNbO"          },
    { 0.7705078125          ,    72   ,    "o?^UF~I%K}d"        },
    { 0.579010009765625     ,    71   ,    "+5q7e/Y:"           }
};

const int DINNER_TABLE_LEN =
                          sizeof DINNER_TABLE_DATA / sizeof *DINNER_TABLE_DATA;

//=============================================================================
//                           SET UP FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------

static void usageTest()
{
    // Create a TestAllocator to profile memory usage, and
    // create a bad allocator to ensure custom allocator is always used.
    bslma_TestAllocator allocator;
    // AlwaysFailAllocator badAllocator;

    int rc;

    //bslma_Default::setDefaultAllocatorRaw(&badAllocator);

    // Create the shared pointer and schema object:
    bcema_SharedPtr<bdem_Schema> schema;
    schema.createInplace(&allocator, &allocator);

    // Table records can be unnamed, as can all but the top-level
    // ``database'' records:
    bdem_RecordDef *dinnerTable = schema->createRecord();
    dinnerTable->appendField(bdem_ElemType::BDEM_DOUBLE, "spoon");
    dinnerTable->appendField(bdem_ElemType::BDEM_INT,    "fork");
    dinnerTable->appendField(bdem_ElemType::BDEM_STRING, "knife");

    // Next, we create two keys (indexes) on the dinner table:
    bdem_RecordDef *dinnerKey1 = schema->createRecord();
    dinnerKey1->appendField(bdem_ElemType::BDEM_STRING,  "knife");

    bdem_RecordDef *dinnerKey2 = schema->createRecord();
    dinnerKey2->appendField(bdem_ElemType::BDEM_INT,     "fork");
    dinnerKey2->appendField(bdem_ElemType::BDEM_DOUBLE,  "spoon");

    // Next, we create a record to contain the table definition (the record
    // definition referred to by 'dinnerTable', created above) and all of
    // its keys (the record definition referred to by 'dinnerKey1' and
    // 'dinnerKey2').  The names of the fields in this record will be used
    // to look up keys by name through the interface of these classes.
    // IMPORTANTLY, the table definition goes at field index 0:

    bdem_RecordDef *dinnerTblInfo = schema->createRecord();
    dinnerTblInfo->appendField(bdem_ElemType::BDEM_TABLE, dinnerTable,
                               "table");
    dinnerTblInfo->appendField(bdem_ElemType::BDEM_LIST, dinnerKey1,
                               "knife_key");
    dinnerTblInfo->appendField(bdem_ElemType::BDEM_LIST, dinnerKey2,
                               "spork_key");

    // Now we are ready to create the database record.  The name of this
    // record will be used to look up the database by the createDb() method
    // of this factory class, and the names of the constrained list fields in
    // this record will be used to look up table names by the createBinding()
    // method:

    bdem_RecordDef *db1 = schema->createRecord("diningRoomDB");
    db1->appendField(bdem_ElemType::BDEM_LIST, dinnerTblInfo, "dinner");

    // Of course, a database can have more than one table, and the factory
    // is capable of creating more than one database.  So we will add another
    // database containing two tables to this schema.  For simplicity, these
    // two tables will only have one key each:

    bdem_RecordDef *coffeeTable = schema->createRecord();
    coffeeTable->appendField(bdem_ElemType::BDEM_INT,    "saucer");
    coffeeTable->appendField(bdem_ElemType::BDEM_DOUBLE, "cup");
    coffeeTable->appendField(bdem_ElemType::BDEM_STRING, "magazine");

    bdem_RecordDef *coffeeKey = schema->createRecord();
    coffeeKey->appendField(bdem_ElemType::BDEM_STRING,   "magazine");

    bdem_RecordDef *coffeeTblInfo = schema->createRecord();
    coffeeTblInfo->appendField(bdem_ElemType::BDEM_TABLE, coffeeTable,
                               "table");
    coffeeTblInfo->appendField(bdem_ElemType::BDEM_LIST, coffeeKey, "magkey");

    bdem_RecordDef *pokerTable = schema->createRecord();
    pokerTable->appendField(bdem_ElemType::BDEM_STRING,  "winner");
    pokerTable->appendField(bdem_ElemType::BDEM_INT,     "cards");
    pokerTable->appendField(bdem_ElemType::BDEM_DOUBLE,  "cash");

    bdem_RecordDef *pokerKey = schema->createRecord();
    pokerTable->appendField(bdem_ElemType::BDEM_INT,     "cards");

    bdem_RecordDef *pokerTblInfo = schema->createRecord();
    pokerTblInfo->appendField(bdem_ElemType::BDEM_TABLE, pokerTable, "table");
    pokerTblInfo->appendField(bdem_ElemType::BDEM_LIST, pokerKey, "cardkey");

    bdem_RecordDef *db2 = schema->createRecord("livingRoomDB");
    db2->appendField(bdem_ElemType::BDEM_LIST, coffeeTblInfo, "coffee");
    db2->appendField(bdem_ElemType::BDEM_LIST, pokerTblInfo, "poker");

    bdem_RecordDef *top = schema->createRecord("toplevel");
    top->appendField(bdem_ElemType::BDEM_LIST, db2, "livingRoomDB");
    top->appendField(bdem_ElemType::BDEM_LIST, db1, "diningRoomDB");

    if (veryVerbose) P(*schema);

    // We can now use the Factory and other classes in this component:

    // test createDb()

    bcema_SharedPtr<const bdem_RecordDef> spTop(schema, top);

    bcedb_InCoreDtagFactory factory(spTop, &allocator);

    bcedb_DtagDbType *dbHandle1 = factory.createDb("livingRoomDB");
    ASSERT(dbHandle1);

    // test getDbTables()

    bsl::vector<bsl::string> dbTables1(&allocator);

    dbHandle1->getDbTables(&dbTables1);
    ASSERT(dbTables1.size() == 2);
    ASSERT(dbTables1[0] == "coffee");
    ASSERT(dbTables1[1] == "poker");

    bcedb_DtagDbType *dbHandle2 = factory.createDb("diningRoomDB");
    ASSERT(dbHandle2);

    // test getDbTables(), reusing destination vector

    dbHandle2->getDbTables(&dbTables1);
    ASSERT(dbTables1.size() == 1);
    ASSERT(dbTables1[0] == "dinner");

    // test appendOnDiskTagInfo()

    bdem_Schema tagInfo(&allocator);

    dbHandle2->appendOnDiskTagInfo(&tagInfo, "dinner");
    ASSERT(tagInfo.numRecords() == 1);
    ASSERT(!bsl::strcmp(tagInfo.recordName(0), "dinner"));
    ASSERT(bdem_SchemaUtil::areEquivalent(tagInfo.record(0), *dinnerTable));

    dbHandle1->appendOnDiskTagInfo(&tagInfo, "poker");
    ASSERT(tagInfo.numRecords() == 2);
    ASSERT(!bsl::strcmp(tagInfo.recordName(1), "poker"));
    ASSERT(bdem_SchemaUtil::areEquivalent(tagInfo.record(1), *pokerTable));

    // test getDbKeys()

    bdem_Schema tableKeys(&allocator);

    dbHandle2->getDbKeys(&tableKeys, "dinner");
    ASSERT(tableKeys.numRecords() == 2);
    ASSERT(!bsl::strcmp(tableKeys.recordName(0), "knife_key"));
    ASSERT(bdem_SchemaUtil::areEquivalent(tableKeys.record(0), *dinnerKey1));
    ASSERT(!bsl::strcmp(tableKeys.recordName(1), "spork_key"));
    ASSERT(bdem_SchemaUtil::areEquivalent(tableKeys.record(1), *dinnerKey2));

    dbHandle1->getDbKeys(&tableKeys, "coffee");
    ASSERT(tableKeys.numRecords() == 1);
    ASSERT(!bsl::strcmp(tableKeys.recordName(0), "magkey"));
    ASSERT(bdem_SchemaUtil::areEquivalent(tableKeys.record(0), *coffeeKey));

    // test createBinding()

    bcedb_DtagBindingType *dinnerBinding1 =
        dbHandle2->createBinding("dinner");
    ASSERT(dinnerBinding1);

    // test bind() and getData()

    bcem_Aggregate dinnerBound1(
            bcema_SharedPtr<const bdem_RecordDef>(schema, dinnerTable),
            &allocator
    );

    rc = dinnerBinding1->bind(&dinnerBound1);
    ASSERT(0 == rc);
    ASSERT(&dinnerBound1 == dinnerBinding1->getData());

    // test updateData()
    // note that *dinnerTable is a superset subset of *dinnerKey1

    bcem_Aggregate dinnerBound2(
            bcema_SharedPtr<const bdem_RecordDef>(schema, dinnerKey1),
            &allocator);

    dinnerBound2.setField("knife",  "just testing");
    dinnerBound1.setField("fork",   42);

    rc = dinnerBinding1->updateData(dinnerBound2);
    ASSERT(0 == rc);
    ASSERT(dinnerBound1.recordDef().numFields() == 3);
    ASSERT(dinnerBound1.field("knife").asString() == "just testing");
    ASSERT(dinnerBound1.field("fork").asInt() == 42);
    ASSERT(dinnerBound1.field("spoon").asElemRef().isNull());

    // test createControl()

    bcedb_DtagControlType *dinnerCtrl1 = dinnerBinding1->createControl();
    ASSERT(dinnerCtrl1);

    // test addRecordToDb() ... repeatedly

    for (int i = 0; i < DINNER_TABLE_LEN; ++i) {
        dinnerBound1.setField("spoon", DINNER_TABLE_DATA[i].d_spoon);
        dinnerBound1.setField("fork",  DINNER_TABLE_DATA[i].d_fork );
        dinnerBound1.setField("knife", DINNER_TABLE_DATA[i].d_knife);

        rc = dinnerCtrl1->addRecordToDb();
        LOOP_ASSERT(i, 0 == rc);
    }

    // test findRecordInDb()

    bcedb_DtagBindingType *dinnerBinding2 = dbHandle2->createBinding("dinner");
    ASSERT(dinnerBinding2);

    dinnerBound2.setField("knife", "Ojj#odO8l+ct");

    rc = dinnerBinding2->bind(&dinnerBound2);
    ASSERT(0 == rc);

    bcedb_DtagControlType::FIND_STATUS_CODE find_rc;
    bcedb_DtagControlType *dinnerCtrl2 = dinnerBinding2->createControl();
    ASSERT(dinnerCtrl2);

    find_rc = dinnerCtrl2->findRecordInDb("knife_key");
    ASSERT(bcedb_DtagControlType::FIND_STATUS_NONE != find_rc);

    dinnerBound2.setField("knife", "~u}48oh8'");
    find_rc = dinnerCtrl2->findRecordInDb("knife_key");
    ASSERT(bcedb_DtagControlType::FIND_STATUS_NONE != find_rc);

    dinnerBound2.setField("knife", "nosuchthing");
    find_rc = dinnerCtrl2->findRecordInDb("knife_key");
    ASSERT(bcedb_DtagControlType::FIND_STATUS_NONE == find_rc);

    // test destroyBinding()

    dbHandle2->destroyBinding(dinnerBinding2);

    dinnerBinding2 = dbHandle2->createBinding("dinner");
    ASSERT(dinnerBinding2);
    rc = dinnerBinding2->bind(&dinnerBound1);
    ASSERT(0 == rc);

    dinnerCtrl2 = dinnerBinding2->createControl();
    ASSERT(dinnerCtrl2);

    // test findRecordInDb() with partial index

    dinnerBound1.setField("fork", 95);
    dinnerBound1.setField("spoon", 1.01);   // not in table

    find_rc = dinnerCtrl2->findRecordInDb("spork_key", "fork");
    ASSERT(bcedb_DtagControlType::FIND_STATUS_ONE == find_rc);

    // test findRecordInDb() with partial string key

    dinnerCtrl2 = dinnerBinding2->createControl();
    ASSERT(dinnerCtrl2);

    dinnerBound1.setField("knife", "Hello");
    find_rc = dinnerCtrl2->findRecordInDb("knife_key", "knife:5");
    ASSERT(bcedb_DtagControlType::FIND_STATUS_MANY == find_rc);
    ASSERT(dinnerBound1.field("knife").asString() == "Hello, world!");
    ASSERT(dinnerBound1.field("fork").asInt() == 90);

    // test findNextRecord()

    find_rc = dinnerCtrl2->findNextRecord();
    ASSERT(bcedb_DtagControlType::FIND_STATUS_ONE == find_rc);
    ASSERT(dinnerBound1.field("knife").asString() == "Hellorhighwater");
    ASSERT(dinnerBound1.field("fork").asInt() == 80);

    // test findRangeInDb()

    dbHandle2->destroyBinding(dinnerBinding2);

    dinnerBinding2 = dbHandle2->createBinding("dinner");
    ASSERT(dinnerBinding2);
    rc = dinnerBinding2->bind(&dinnerBound2);
    ASSERT(0 == rc);

    dinnerCtrl2 = dinnerBinding2->createControl();
    ASSERT(dinnerCtrl2);

    dinnerBound2.setField("knife", "Hello");
    dinnerBound1.setField("knife", "avm6IuC!");

    find_rc = dinnerCtrl2->findRangeInDb(dinnerBinding1,
                                         "knife_key", "knife:5");
    ASSERT(bcedb_DtagControlType::FIND_STATUS_ONE == find_rc);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // Usage example test (along with all methods but hey)
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "Usage Example Test" << bsl::endl
                      << "==================" << bsl::endl;
        }

        usageTest();

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

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
