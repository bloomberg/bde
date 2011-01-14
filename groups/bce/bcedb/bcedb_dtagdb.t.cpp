// bcedb_dtagdb.t.cpp  -*-C++-*-
#include <bcedb_dtagdb.h>
// bcedb_dtagdb.t.cpp  -*-C++-*-
#include <bcedb_dtagdb.h>
#include <bcedb_incoredtagfactory.h>
#include <bcedb_dtagfactory.h>
#include <bcem_aggregate.h>
#include <bdem_elemtype.h>
#include <bdema_testallocator.h>
#include <bdeut_strtokeniter.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// 
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
        std::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << std::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { std::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { std::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { std::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) std::cout << #X " = " << (X) << std::endl;
                                              // Print identifier and value.
#define Q(X) std::cout << "<| " #X " |>" << std::endl;
                                              // Quote identifier literally.
#define P_(X) std::cout << #X " = " << (X) << ", " << std::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bcedb_InCoreDtagFactory Factory;
typedef bcedb_InCoreDtagDbType  ICDbType;
typedef bcedb_DtagDbType        DbType;
typedef bcedb_DtagDb            Db;

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

static bcema_SharedPtr<bdem_Schema> gDbSchema; // the in-core db schema, not 
                                               // the schema returned from 
                                               // the API


//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static bdem_ElemType::Type gTypes[] = {
    bdem_ElemType::SHORT,
    bdem_ElemType::INT,
    bdem_ElemType::INT64,
    bdem_ElemType::FLOAT,
    bdem_ElemType::DOUBLE,
    bdem_ElemType::STRING,
    bdem_ElemType::CHAR_ARRAY
};

void gGetDbSchema(bdem_Schema *schema)
{
    // First DB is one of each elem.  Key for each combination.
    bdem_RecordDef *def = schema->createRecord("Test");
    def->appendField(bdem_ElemType::SHORT, "A");
    def->appendField(bdem_ElemType::INT, "B");
    def->appendField(bdem_ElemType::INT64, "C");
    def->appendField(bdem_ElemType::FLOAT, "D");
    def->appendField(bdem_ElemType::DOUBLE, "E");
    def->appendField(bdem_ElemType::STRING, "F");
    def->appendField(bdem_ElemType::CHAR_ARRAY, "G");

    bdem_RecordDef *key1 = schema->createRecord("Key1");
    key1->appendField(bdem_ElemType::SHORT, "A");

    bdem_RecordDef *key2 = schema->createRecord("Key2");
    key2->appendField(bdem_ElemType::INT, "B");

    bdem_RecordDef *key3 = schema->createRecord("Key3");
    key3->appendField(bdem_ElemType::INT64, "C");

    bdem_RecordDef *key4 = schema->createRecord("Key4");
    key4->appendField(bdem_ElemType::FLOAT, "D");

    bdem_RecordDef *key5 = schema->createRecord("Key5");
    key5->appendField(bdem_ElemType::DOUBLE, "E");

    bdem_RecordDef *key6 = schema->createRecord("Key6");
    key6->appendField(bdem_ElemType::STRING, "F");

    bdem_RecordDef *key7 = schema->createRecord("Key7");
    key7->appendField(bdem_ElemType::CHAR_ARRAY, "G");

    bdem_RecordDef *key8 = schema->createRecord("Key8");
    key8->appendField(bdem_ElemType::SHORT, "A");
    key8->appendField(bdem_ElemType::INT, "B");

    bdem_RecordDef *key9 = schema->createRecord("Key9");
    key9->appendField(bdem_ElemType::SHORT, "A");
    key9->appendField(bdem_ElemType::INT64, "C");

    bdem_RecordDef *key10 = schema->createRecord("Key10");
    key10->appendField(bdem_ElemType::SHORT, "A");
    key10->appendField(bdem_ElemType::FLOAT, "D");

    bdem_RecordDef *key11 = schema->createRecord("Key11");
    key11->appendField(bdem_ElemType::SHORT, "A");
    key11->appendField(bdem_ElemType::DOUBLE, "E");

    bdem_RecordDef *key12 = schema->createRecord("Key12");
    key12->appendField(bdem_ElemType::SHORT, "A");
    key12->appendField(bdem_ElemType::STRING, "F");

    bdem_RecordDef *key13 = schema->createRecord("Key13");
    key13->appendField(bdem_ElemType::SHORT, "A");
    key13->appendField(bdem_ElemType::CHAR_ARRAY, "G");

    bdem_RecordDef *key14 = schema->createRecord("Key14");
    key14->appendField(bdem_ElemType::SHORT, "A");
    key14->appendField(bdem_ElemType::INT, "B");
    key14->appendField(bdem_ElemType::INT64, "C");

    bdem_RecordDef *kdef = schema->createRecord("Test_Keys");
    kdef->appendField(bdem_ElemType::TABLE, def, "table");
    kdef->appendField(bdem_ElemType::LIST, key1, "Key1");
    kdef->appendField(bdem_ElemType::LIST, key2, "Key2");
    kdef->appendField(bdem_ElemType::LIST, key3, "Key3");
    kdef->appendField(bdem_ElemType::LIST, key4, "Key4");
    kdef->appendField(bdem_ElemType::LIST, key5, "Key5");
    kdef->appendField(bdem_ElemType::LIST, key6, "Key6");
    kdef->appendField(bdem_ElemType::LIST, key7, "Key7");
    kdef->appendField(bdem_ElemType::LIST, key8, "Key8");
    kdef->appendField(bdem_ElemType::LIST, key9, "Key9");
    kdef->appendField(bdem_ElemType::LIST, key10, "Key10");
    kdef->appendField(bdem_ElemType::LIST, key11, "Key11");
    kdef->appendField(bdem_ElemType::LIST, key12, "Key12");
    kdef->appendField(bdem_ElemType::LIST, key13, "Key13");
    kdef->appendField(bdem_ElemType::LIST, key14, "Key14");

    // Second DB is one of each elem.  Key only for combination.
    def = schema->createRecord("Test2");
    def->appendField(bdem_ElemType::SHORT, "A");
    def->appendField(bdem_ElemType::INT, "B");
    def->appendField(bdem_ElemType::INT64, "C");
    def->appendField(bdem_ElemType::FLOAT, "D");
    def->appendField(bdem_ElemType::DOUBLE, "E");
    def->appendField(bdem_ElemType::STRING, "F");
    def->appendField(bdem_ElemType::CHAR_ARRAY, "G");

    key1 = schema->createRecord("TwoKey1");
    key1->appendField(bdem_ElemType::SHORT, "A");
    key1->appendField(bdem_ElemType::INT, "B");
    key1->appendField(bdem_ElemType::INT64, "C");
    key1->appendField(bdem_ElemType::FLOAT, "D");
    key1->appendField(bdem_ElemType::DOUBLE, "E");
    key1->appendField(bdem_ElemType::STRING, "F");
    key1->appendField(bdem_ElemType::CHAR_ARRAY, "G");

    bdem_RecordDef *kdef2 = schema->createRecord("Test2_Keys");
    kdef2->appendField(bdem_ElemType::TABLE, def, "table");
    kdef2->appendField(bdem_ElemType::LIST, key1, "Key1");

    bdem_RecordDef *sdef = schema->createRecord("TestDb");
    sdef->appendField(bdem_ElemType::LIST, kdef, "Test");
    sdef->appendField(bdem_ElemType::LIST, kdef2, "Test2");

    bdem_RecordDef *tdef = schema->createRecord("TopLevel");
    tdef->appendField(bdem_ElemType::LIST, sdef, "TestDb");
}

void gg(DbType *db, const char *spec)
    // Append to the specified 'db', a row consisting of elements
    // according to the specified 'spec' string.  Valid input consists of 
    // a series of CAPITAL letters from 'A' to 'G' followed by a colon ':'
    // and a value, follow by another ':' -- e.g. "A:1:D:1.5:F:test".  
{
  
    const bdem_RecordDef *def = gDbSchema->lookupRecord("Test");
    ASSERT(def);


    bcema_SharedPtr<const bdem_RecordDef> sdef(gDbSchema, def);
    bcem_Aggregate agg(sdef);

    bdeut_StrTokenIter it (spec, "", ":");
    for (; it; ++it) {
        std::string field = it();
        bdem_ElemType::Type type = gTypes[*it() - 'A'];  
        ++it;
        ASSERT(it);
        switch (type) {
            case bdem_ElemType::SHORT: {
                agg.setField(field.c_str(),  (short)atoi(it()));
            } break;
            case bdem_ElemType::INT: {
                agg.setField(field.c_str(),  atoi(it()));
            } break;
            case bdem_ElemType::INT64: {
                agg.setField(field.c_str(),  atol(it()));
            } break;
            case bdem_ElemType::FLOAT: {
                agg.setField(field.c_str(),  (float)atof(it()));
            } break;
            case bdem_ElemType::DOUBLE: {
                agg.setField(field.c_str(),  atof(it()));
            } break;
            case bdem_ElemType::STRING: {
                agg.setField(field.c_str(),  it());
            } break;
            case bdem_ElemType::CHAR_ARRAY: {
                std::vector<char> ca;
                ca.resize(strlen(it()) + 1);
                std::memcpy(&ca[0], it(), strlen(it()) + 1); 
                agg.setField(field.c_str(),  ca);
            } break;
            default: {
              ASSERT(!!"Bad spec");        
            }
        }
    }  

    bcedb_DtagBindingType *b = db->createBinding("Test");
    b->bind(&agg);
    bcedb_DtagControlType *ctrl = b->createControl();
    ctrl->addRecordToDb();
}


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    std::cout << "TEST " << __FILE__ << " CASE " << test << std::endl;;

    bdema_TestAllocator  testAllocator(veryVeryVerbose);
    bdema_Allocator     *Z = &testAllocator;

    gDbSchema.createInplace();
    gGetDbSchema(&(*gDbSchema));

    Factory dtagFactory(
            bcema_SharedPtr<const bdem_RecordDef>(gDbSchema,
                &(gDbSchema->record(gDbSchema->length() - 1))
        ));

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // Test open. 
        // --------------------------------------------------------------------
        if (verbose) {
            std::cout << "Test query and queryWithIndex" << std::endl
                      << "=============================" << std::endl;
        }

        static const struct {
            int         d_lineNum;   // source line number
            const char *d_spec_p;    // specification string
            short       d_a;
            int         d_b;
            long long   d_c;
            float       d_d;
            double      d_e;
            const char *d_f;
            const char *d_g;
        } DATA[] = {
            //line  spec                          a  b  c    d    e    f     g
            //----  ----                          -  -  -  ---  ---   ---  ---
            { L_,   "A:0:B:0:C:0:D:0:E:0:F::G::", 0, 0, 0, 0.0, 0.0,   "",  ""},
            { L_,   "A:1:B:1:C:1:D:1:E:1:F:a:G:a",0, 0, 0, 0.0, 0.0,  "a", "a"},
            { L_,   "A:1:B:2:C:4:D:5:E:6:F:c:G:d",2, 3, 4, 5.0, 6.0,  "b", "c"},
            { L_,   "A:2:B:3:C:4:D:5:E:6:F:c:G:d",2, 3, 4, 5.0, 6.0,  "b", "c"}
        };


        DbType *dbType = dtagFactory.createDb("TestDb");
        ASSERT(dbType);

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // dtagFactory.clear();
        for (int ti = 0; ti < NUM_DATA;  ++ti) {
            gg(dbType, DATA[ti].d_spec_p);
        }

        if (veryVerbose)
            dynamic_cast<bcedb_InCoreDtagDbType *>(dbType)
                ->printData(std::cout);

        bcedb_DtagDb db(&dtagFactory, Z);
        ASSERT(0 != db.open("BadTestDb"));
        ASSERT(!db.isOpen());
        ASSERT(0 == db.open("TestDb"));
        ASSERT(db.isOpen());

        const bdem_RecordDef *def = gDbSchema->lookupRecord("Test");
        ASSERT(def);

        bcema_SharedPtr<const bdem_RecordDef> sdef(gDbSchema, def);
        bcem_Aggregate agg(sdef);
        agg.setField("A", DATA[0].d_a);
        bdema_ManagedPtr<bcedb_Cursor> c = db.query(agg, "Test");
        ASSERT(c);
        ASSERT(*c);
        ASSERT(c->isFirst());
        ASSERT(c->isLast());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Test open. 
        // --------------------------------------------------------------------
        if (verbose) {
            std::cout << "Test findFirstRecord" << std::endl
                      << "====================" << std::endl;
        }

        static const struct {
            int         d_lineNum;   // source line number
            const char *d_spec_p;    // specification string
            short       d_a;
            int         d_b;
            long long   d_c;
            float       d_d;
            double      d_e;
            const char *d_f;
            const char *d_g;
        } DATA[] = {
            //line  spec                          a  b  c    d    e    f     g
            //----  ----                          -  -  -  ---  ---   ---  ---
            { L_,   "A:0:B:0:C:0:D:0:E:0:F::G::", 0, 0, 0, 0.0, 0.0,   "",  ""},
            { L_,   "A:1:B:1:C:1:D:1:E:1:F:a:G:a",1, 1, 1, 1.0, 1.0,  "a", "a"},
            { L_,   "A:2:B:3:C:4:D:5:E:6:F:c:G:d",2, 3, 4, 5.0, 6.0,  "c", "d"}
        };


        DbType *dbType = dtagFactory.createDb("TestDb");
        ASSERT(dbType);

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        //dtagFactory.clear();
        for (int ti = 0; ti < NUM_DATA;  ++ti) {
            gg(dbType, DATA[ti].d_spec_p);
        }

        bcedb_DtagDb db(&dtagFactory, Z);
        ASSERT(0 != db.open("BadTestDb"));
        ASSERT(!db.isOpen());
        ASSERT(0 == db.open("TestDb"));
        ASSERT(db.isOpen());

        const bdem_RecordDef *def = gDbSchema->lookupRecord("Test");
        ASSERT(def);

        bcema_SharedPtr<const bdem_RecordDef> sdef(gDbSchema, def);
        int num = 0;
        bdema_ManagedPtr<bcedb_Cursor> cursor = db.getFirstRecord(sdef, "Test");

        for (; *cursor && num < 3;  ++num, cursor->next()) {
            const int         LINE   = DATA[num].d_lineNum;
            const char *const SPEC   = DATA[num].d_spec_p;
            const short       A      = DATA[num].d_a;
            const int         B      = DATA[num].d_b;
            const long long   C      = DATA[num].d_c;
            const float       D      = DATA[num].d_d;
            const double      E      = DATA[num].d_e;
            const char *const F      = DATA[num].d_f;
            const char *const G      = DATA[num].d_g;

            const bcem_Aggregate& cdata = cursor->data();

            LOOP_ASSERT(LINE, cdata.field("A").asShort() == A);
            LOOP_ASSERT(LINE, cdata.field("B").asInt() == B);
            LOOP_ASSERT(LINE, cdata.field("C").asInt64() == C);
            LOOP_ASSERT(LINE, cdata.field("D").asFloat() == D);
            LOOP_ASSERT(LINE, cdata.field("E").asDouble() == E);
            LOOP_ASSERT(LINE, cdata.field("F").asString() == F);
            LOOP_ASSERT(LINE, cdata.field("G").length() == strlen(G) + 1);
            for (int j = 0; j < cdata.field("G").length(); ++j) {
                LOOP2_ASSERT(LINE, j, cdata.field("G", j).asChar() == G[j]);
            }
        }
        ASSERT(!*cursor);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test open. 
        // --------------------------------------------------------------------
        if (verbose) {
            std::cout << "Test Open" << std::endl
                      << "==========" << std::endl;
        }

        bcedb_DtagDb db(&dtagFactory, Z);
        ASSERT(0 != db.open("BadTestDb"));
        ASSERT(!db.isOpen());
        ASSERT(0 == db.open("TestDb"));
        ASSERT(db.isOpen());
      } break;
      default: {
        std::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << std::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::cerr << "Error, non-zero test status = " << testStatus << "."
                  << std::endl;
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

