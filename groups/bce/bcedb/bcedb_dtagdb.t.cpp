// bcedb_dtagdb.t.cpp  -*-C++-*-
#include <bcedb_dtagdb.h>
#include <bcedb_dtagfactory.h>
#include <bcedb_incoredtagfactory.h>
#include <bcem_aggregate.h>

#include <bdem_elemtype.h>
#include <bdeu_printmethods.h>
#include <bdeut_strtokeniter.h>

#include <bslma_testallocator.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdlib.h>
#include <bsl_c_stdlib.h>
#include <bsl_c_string.h>

using namespace BloombergLP;
using namespace bsl;

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
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bcedb_InCoreDtagFactory Factory;
typedef bcedb_InCoreDtagDbType  ICDbType;
typedef bcedb_DtagDbType        DbType;
typedef bcedb_DtagDb            Db;

static bcema_SharedPtr<bdem_Schema> gDbSchema;  // the in-core db schema, not
                                                // the schema returned from
                                                // the API

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static bdem_ElemType::Type gTypes[] = {
    bdem_ElemType::BDEM_SHORT,
    bdem_ElemType::BDEM_INT,
    bdem_ElemType::BDEM_INT64,
    bdem_ElemType::BDEM_FLOAT,
    bdem_ElemType::BDEM_DOUBLE,
    bdem_ElemType::BDEM_STRING,
    bdem_ElemType::BDEM_CHAR_ARRAY
};

void gGetDbSchema(bdem_Schema *schema)
{
    // First DB is one of each elem.  Key for each combination.
    bdem_RecordDef *def = schema->createRecord("Test");
    def->appendField(bdem_ElemType::BDEM_SHORT, "A");
    def->appendField(bdem_ElemType::BDEM_INT, "B");
    def->appendField(bdem_ElemType::BDEM_INT64, "C");
    def->appendField(bdem_ElemType::BDEM_FLOAT, "D");
    def->appendField(bdem_ElemType::BDEM_DOUBLE, "E");
    def->appendField(bdem_ElemType::BDEM_STRING, "F");
    def->appendField(bdem_ElemType::BDEM_CHAR_ARRAY, "G");

    bdem_RecordDef *key1 = schema->createRecord("Key1");
    key1->appendField(bdem_ElemType::BDEM_SHORT, "A");

    bdem_RecordDef *key2 = schema->createRecord("Key2");
    key2->appendField(bdem_ElemType::BDEM_INT, "B");

    bdem_RecordDef *key3 = schema->createRecord("Key3");
    key3->appendField(bdem_ElemType::BDEM_INT64, "C");

    bdem_RecordDef *key4 = schema->createRecord("Key4");
    key4->appendField(bdem_ElemType::BDEM_FLOAT, "D");

    bdem_RecordDef *key5 = schema->createRecord("Key5");
    key5->appendField(bdem_ElemType::BDEM_DOUBLE, "E");

    bdem_RecordDef *key6 = schema->createRecord("Key6");
    key6->appendField(bdem_ElemType::BDEM_STRING, "F");

    bdem_RecordDef *key7 = schema->createRecord("Key7");
    key7->appendField(bdem_ElemType::BDEM_CHAR_ARRAY, "G");

    bdem_RecordDef *key8 = schema->createRecord("Key8");
    key8->appendField(bdem_ElemType::BDEM_SHORT, "A");
    key8->appendField(bdem_ElemType::BDEM_INT, "B");

    bdem_RecordDef *key9 = schema->createRecord("Key9");
    key9->appendField(bdem_ElemType::BDEM_SHORT, "A");
    key9->appendField(bdem_ElemType::BDEM_INT64, "C");

    bdem_RecordDef *key10 = schema->createRecord("Key10");
    key10->appendField(bdem_ElemType::BDEM_SHORT, "A");
    key10->appendField(bdem_ElemType::BDEM_FLOAT, "D");

    bdem_RecordDef *key11 = schema->createRecord("Key11");
    key11->appendField(bdem_ElemType::BDEM_SHORT, "A");
    key11->appendField(bdem_ElemType::BDEM_DOUBLE, "E");

    bdem_RecordDef *key12 = schema->createRecord("Key12");
    key12->appendField(bdem_ElemType::BDEM_SHORT, "A");
    key12->appendField(bdem_ElemType::BDEM_STRING, "F");

    bdem_RecordDef *key13 = schema->createRecord("Key13");
    key13->appendField(bdem_ElemType::BDEM_SHORT, "A");
    key13->appendField(bdem_ElemType::BDEM_CHAR_ARRAY, "G");

    bdem_RecordDef *key14 = schema->createRecord("Key14");
    key14->appendField(bdem_ElemType::BDEM_SHORT, "A");
    key14->appendField(bdem_ElemType::BDEM_INT, "B");
    key14->appendField(bdem_ElemType::BDEM_INT64, "C");

    bdem_RecordDef *kdef = schema->createRecord("Test_Keys");
    kdef->appendField(bdem_ElemType::BDEM_TABLE, def, "table");
    kdef->appendField(bdem_ElemType::BDEM_LIST, key1, "Key1");
    kdef->appendField(bdem_ElemType::BDEM_LIST, key2, "Key2");
    kdef->appendField(bdem_ElemType::BDEM_LIST, key3, "Key3");
    kdef->appendField(bdem_ElemType::BDEM_LIST, key4, "Key4");
    kdef->appendField(bdem_ElemType::BDEM_LIST, key5, "Key5");
    kdef->appendField(bdem_ElemType::BDEM_LIST, key6, "Key6");
    kdef->appendField(bdem_ElemType::BDEM_LIST, key7, "Key7");
    kdef->appendField(bdem_ElemType::BDEM_LIST, key8, "Key8");
    kdef->appendField(bdem_ElemType::BDEM_LIST, key9, "Key9");
    kdef->appendField(bdem_ElemType::BDEM_LIST, key10, "Key10");
    kdef->appendField(bdem_ElemType::BDEM_LIST, key11, "Key11");
    kdef->appendField(bdem_ElemType::BDEM_LIST, key12, "Key12");
    kdef->appendField(bdem_ElemType::BDEM_LIST, key13, "Key13");
    kdef->appendField(bdem_ElemType::BDEM_LIST, key14, "Key14");

    // Second DB is one of each elem.  Key only for combination.
    def = schema->createRecord("Test2");
    def->appendField(bdem_ElemType::BDEM_SHORT, "A");
    def->appendField(bdem_ElemType::BDEM_INT, "B");
    def->appendField(bdem_ElemType::BDEM_INT64, "C");
    def->appendField(bdem_ElemType::BDEM_FLOAT, "D");
    def->appendField(bdem_ElemType::BDEM_DOUBLE, "E");
    def->appendField(bdem_ElemType::BDEM_STRING, "F");
    def->appendField(bdem_ElemType::BDEM_CHAR_ARRAY, "G");

    key1 = schema->createRecord("TwoKey1");
    key1->appendField(bdem_ElemType::BDEM_SHORT, "A");
    key1->appendField(bdem_ElemType::BDEM_INT, "B");
    key1->appendField(bdem_ElemType::BDEM_INT64, "C");
    key1->appendField(bdem_ElemType::BDEM_FLOAT, "D");
    key1->appendField(bdem_ElemType::BDEM_DOUBLE, "E");
    key1->appendField(bdem_ElemType::BDEM_STRING, "F");
    key1->appendField(bdem_ElemType::BDEM_CHAR_ARRAY, "G");

    bdem_RecordDef *kdef2 = schema->createRecord("Test2_Keys");
    kdef2->appendField(bdem_ElemType::BDEM_TABLE, def, "table");
    kdef2->appendField(bdem_ElemType::BDEM_LIST, key1, "Key1");

    bdem_RecordDef *sdef = schema->createRecord("TestDb");
    sdef->appendField(bdem_ElemType::BDEM_LIST, kdef, "Test");
    sdef->appendField(bdem_ElemType::BDEM_LIST, kdef2, "Test2");

    bdem_RecordDef *tdef = schema->createRecord("TopLevel");
    tdef->appendField(bdem_ElemType::BDEM_LIST, sdef, "TestDb");
}

void gg(DbType *db, const char *spec)
    // Append to the specified 'db', a row consisting of elements
    // according to the specified 'spec' string.  Valid input consists of
    // a series of CAPITAL letters from 'A' to 'G' followed by a colon ':'
    // and a value, followed by another ':' -- e.g., "A:1:D:1.5:F:test".
{

    const bdem_RecordDef *def = gDbSchema->lookupRecord("Test");
    ASSERT(def);


    bcema_SharedPtr<const bdem_RecordDef> sdef(gDbSchema, def);
    bcem_Aggregate agg(sdef);

    bdeut_StrTokenIter it (spec, "", ":");
    for (; it; ++it) {
        bsl::string field = it();
        bdem_ElemType::Type type = gTypes[*it() - 'A'];
        ++it;
        ASSERT(it);
        switch (type) {
            case bdem_ElemType::BDEM_SHORT: {
                agg.setField(field.c_str(),  (short)atoi(it()));
            } break;
            case bdem_ElemType::BDEM_INT: {
                agg.setField(field.c_str(),  atoi(it()));
            } break;
            case bdem_ElemType::BDEM_INT64: {
                agg.setField(field.c_str(),  atol(it()));
            } break;
            case bdem_ElemType::BDEM_FLOAT: {
                agg.setField(field.c_str(),  (float)atof(it()));
            } break;
            case bdem_ElemType::BDEM_DOUBLE: {
                agg.setField(field.c_str(),  atof(it()));
            } break;
            case bdem_ElemType::BDEM_STRING: {
                agg.setField(field.c_str(),  it());
            } break;
            case bdem_ElemType::BDEM_CHAR_ARRAY: {
                bsl::vector<char> ca;
                ca.resize(strlen(it()) + 1);
                bsl::memcpy(&ca[0], it(), strlen(it()) + 1);
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
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator  testAllocator(veryVeryVerbose);
    bslma_Allocator     *Z = &testAllocator;

    gDbSchema.createInplace();
    gGetDbSchema(&(*gDbSchema));

    Factory dtagFactory(
            bcema_SharedPtr<const bdem_RecordDef>(gDbSchema,
                &(gDbSchema->record(gDbSchema->numRecords() - 1))));

    switch (test) { case 0:  // zero is always the leading case
      case 3: {
        // --------------------------------------------------------------------
        // 'query' and 'queryWithIndex' TEST:
        //
        // Concerns:
        //   1. 'query' and 'queryWithIndex' return a valid cursor.
        //   2. 'query' and 'queryWithIndex' return a valid cursor when used
        //      with partial fields.
        //   3. 'query' using non-string data fields as partial key should
        //      not result in an error - see DRQS 16457966.
        //
        // Plan:
        //   Using 'bcedb_InCoreDtagFactory', we create an in-core
        //   representation of a test database to exercise the 'query' and
        //   'queryWithIndex' methods, with and without partial fields.
        //
        // Testing:
        //   query(const bcem_Aggregate&, const char *);
        //   query(const bcem_Aggregate&, const char *, const vector<string>&);
        //   queryWithIndex(const bcem_Aggregate&, const char *);
        //   queryWithIndex(const bcem_Aggregate&, const char *,
        //                                              const vector<string>&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Test query and queryWithIndex" << endl
                                  << "=============================" << endl;

        // Note that these are not really test vectors, but data used to
        // populate the database.
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            short       d_a;        // field A ('BDEM_SHORT')
            int         d_b;        // field B ('BDEM_INT')
            long long   d_c;        // field C ('BDEM_INT64')
            float       d_d;        // field D ('BDEM_FLOAT')
            double      d_e;        // field E ('BDEM_DOUBLE')
            const char *d_f;        // field F ('BDEM_STRING')
            const char *d_g;        // field G ('BDEM_CHAR_ARRAY')
        } DATA[] = {
    //LINE  SPEC                            A  B  C   D    E    F      G
    //----  ----                            -  -  -  ---  ---  -----  ---
    { L_,  "A:0:B:0:C:0:D:0:E:0:F::G::",    0, 0, 0, 0.0, 0.0,    "", "" },
    { L_,  "A:1:B:1:C:1:D:1:E:1:F:a:G:a",   1, 1, 1, 1.0, 1.0,   "a", "a"},
    { L_,  "A:1:B:2:C:4:D:5:E:6:F:c:G:d",   1, 2, 4, 5.0, 6.0,   "c", "d"},
    { L_,  "A:2:B:3:C:4:D:5:E:6:F:c:G:d",   2, 3, 4, 5.0, 6.0,   "c", "d"},

    // Used for partial fields testing.
    { L_,  "A:8:B:2:C:4:D:5:E:6:F:dad:G::", 8, 2, 4, 5.0, 6.0, "dad", "" },
    { L_,  "A:8:B:3:C:4:D:5:E:6:F:dcb:G::", 8, 3, 4, 5.0, 6.0, "dcb", "" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int PARTIALFIELDSTESTNUM = 4;  // partial fields test case

        DbType *dbType = dtagFactory.createDb("TestDb");
        ASSERT(dbType);

        // Populate database before testing.
        if (verbose) cout << "\tPopulating database." << endl;
        for (int ti = 0; ti < NUM_DATA;  ++ti) {
            const int   LINE = DATA[ti].d_lineNum;
            const char *SPEC = DATA[ti].d_spec_p;
            if (veryVerbose) { T_ T_ P_(LINE) P(SPEC) }

            gg(dbType, SPEC);
        }

        if (veryVerbose) {
            cout << "\t\tPopulated database." << endl;
            dynamic_cast<bcedb_InCoreDtagDbType *>(dbType)
                                                        ->printData(bsl::cout);
        }

        if (verbose) cout << "\tTesting 'query'." << endl;
        {
            const int        LINE = DATA[0].d_lineNum;
            const char      *SPEC = DATA[0].d_spec_p;
            const short      A    = DATA[0].d_a;
            const int        B    = DATA[0].d_b;
            const long long  C    = DATA[0].d_c;
            const float      D    = DATA[0].d_d;
            const double     E    = DATA[0].d_e;
            const char      *F    = DATA[0].d_f;
            const char      *G    = DATA[0].d_g;
            if (veryVerbose) {
                T_ T_ P_(LINE) P(SPEC)
                T_ T_ P_(A) P_(B) P_(C) P_(D) P_(E) P_(F) P(G)
            }

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

            bdema_ManagedPtr<bcedb_Cursor> cursor = db.query(agg, "Test");
            ASSERT(cursor);
            ASSERT(*cursor);
            ASSERT(cursor->isFirst());
            ASSERT(cursor->isLast());

            const bcem_Aggregate& cdata = cursor->data();

            LOOP2_ASSERT(LINE, SPEC, A == cdata.field("A").asShort());
            LOOP2_ASSERT(LINE, SPEC, B == cdata.field("B").asInt());
            LOOP2_ASSERT(LINE, SPEC, C == cdata.field("C").asInt64());
            LOOP2_ASSERT(LINE, SPEC, D == cdata.field("D").asFloat());
            LOOP2_ASSERT(LINE, SPEC, E == cdata.field("E").asDouble());
            LOOP2_ASSERT(LINE, SPEC, F == cdata.field("F").asString());
            LOOP2_ASSERT(LINE, SPEC,
                     strlen(G) + 1 == (unsigned int)cdata.field("G").length());

            for (int j = 0; j < cdata.field("G").length(); ++j) {
                LOOP2_ASSERT(LINE, j, cdata.field("G", j).asChar() == G[j]);
            }
        }

        if (verbose) cout << "\tTesting 'queryWithIndex'." << endl;
        {
            const int        LINE = DATA[0].d_lineNum;
            const char      *SPEC = DATA[0].d_spec_p;
            const short      A    = DATA[0].d_a;
            const int        B    = DATA[0].d_b;
            const long long  C    = DATA[0].d_c;
            const float      D    = DATA[0].d_d;
            const double     E    = DATA[0].d_e;
            const char      *F    = DATA[0].d_f;
            const char      *G    = DATA[0].d_g;
            if (veryVerbose) {
                T_ T_ P_(LINE) P(SPEC)
                T_ T_ P_(A) P_(B) P_(C) P_(D) P_(E) P_(F) P(G)
            }

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

            bdema_ManagedPtr<bcedb_Cursor> cursor = db.queryWithIndex(agg,
                                                                      "Test",
                                                                      "Key1");
            ASSERT(cursor);
            ASSERT(*cursor);
            ASSERT(cursor->isFirst());
            ASSERT(cursor->isLast());

            const bcem_Aggregate& cdata = cursor->data();

            LOOP2_ASSERT(LINE, SPEC, A == cdata.field("A").asShort());
            LOOP2_ASSERT(LINE, SPEC, B == cdata.field("B").asInt());
            LOOP2_ASSERT(LINE, SPEC, C == cdata.field("C").asInt64());
            LOOP2_ASSERT(LINE, SPEC, D == cdata.field("D").asFloat());
            LOOP2_ASSERT(LINE, SPEC, E == cdata.field("E").asDouble());
            LOOP2_ASSERT(LINE, SPEC, F == cdata.field("F").asString());
            LOOP2_ASSERT(LINE, SPEC,
                     strlen(G) + 1 == (unsigned int)cdata.field("G").length());

            for (int j = 0; j < cdata.field("G").length(); ++j) {
                LOOP2_ASSERT(LINE, j, cdata.field("G", j).asChar() == G[j]);
            }
        }

        if (verbose) cout << "\tTesting 'query' with partial fields." << endl;
        {
            bcedb_DtagDb db(&dtagFactory, Z);
            ASSERT(0 != db.open("BadTestDb"));
            ASSERT(!db.isOpen());

            ASSERT(0 == db.open("TestDb"));
            ASSERT(db.isOpen());

            const bdem_RecordDef *def = gDbSchema->lookupRecord("Test");
            ASSERT(def);

            bcema_SharedPtr<const bdem_RecordDef> sdef(gDbSchema, def);
            bcem_Aggregate agg(sdef);

            agg.setField("F", "d");
            vector<string> partials;
            partials.push_back("F");

            bdema_ManagedPtr<bcedb_Cursor> cursor = db.query(agg, "Test",
                                                             partials);
            ASSERT(cursor);

            for (int i = PARTIALFIELDSTESTNUM; *cursor; cursor->next(), ++i) {
                const int        LINE = DATA[i].d_lineNum;
                const char      *SPEC = DATA[i].d_spec_p;
                const short      A    = DATA[i].d_a;
                const int        B    = DATA[i].d_b;
                const long long  C    = DATA[i].d_c;
                const float      D    = DATA[i].d_d;
                const double     E    = DATA[i].d_e;
                const char      *F    = DATA[i].d_f;
                const char      *G    = DATA[i].d_g;
                if (veryVerbose) {
                    T_ T_ P_(LINE) P(SPEC)
                    T_ T_ P_(A) P_(B) P_(C) P_(D) P_(E) P_(F) P(G)
                }

                const bcem_Aggregate& cdata = cursor->data();

                LOOP2_ASSERT(LINE, SPEC, A == cdata.field("A").asShort());
                LOOP2_ASSERT(LINE, SPEC, B == cdata.field("B").asInt());
                LOOP2_ASSERT(LINE, SPEC, C == cdata.field("C").asInt64());
                LOOP2_ASSERT(LINE, SPEC, D == cdata.field("D").asFloat());
                LOOP2_ASSERT(LINE, SPEC, E == cdata.field("E").asDouble());
                LOOP2_ASSERT(LINE, SPEC, F == cdata.field("F").asString());
                LOOP2_ASSERT(LINE, SPEC,
                     strlen(G) + 1 == (unsigned int)cdata.field("G").length());

                for (int j = 0; j < cdata.field("G").length(); ++j) {
                    LOOP2_ASSERT(LINE, j,
                                 cdata.field("G", j).asChar() == G[j]);
                }
            }
        }

        if (verbose) cout << "\tTesting 'queryWithIndex' with partial fields."
                          << endl;
        {
            bcedb_DtagDb db(&dtagFactory, Z);
            ASSERT(0 != db.open("BadTestDb"));
            ASSERT(!db.isOpen());

            ASSERT(0 == db.open("TestDb"));
            ASSERT(db.isOpen());

            const bdem_RecordDef *def = gDbSchema->lookupRecord("Test");
            ASSERT(def);

            bcema_SharedPtr<const bdem_RecordDef> sdef(gDbSchema, def);
            bcem_Aggregate agg(sdef);

            agg.setField("F", "d");
            vector<string> partials;
            partials.push_back("F");

            bdema_ManagedPtr<bcedb_Cursor> cursor = db.queryWithIndex(agg,
                                                     "Test", "Key6", partials);
            ASSERT(cursor);
            ASSERT(*cursor);

            for (int i = PARTIALFIELDSTESTNUM; *cursor; cursor->next(), ++i) {
                const int        LINE = DATA[i].d_lineNum;
                const char      *SPEC = DATA[i].d_spec_p;
                const short      A    = DATA[i].d_a;
                const int        B    = DATA[i].d_b;
                const long long  C    = DATA[i].d_c;
                const float      D    = DATA[i].d_d;
                const double     E    = DATA[i].d_e;
                const char      *F    = DATA[i].d_f;
                const char      *G    = DATA[i].d_g;
                if (veryVerbose) {
                    T_ T_ P_(LINE) P(SPEC)
                    T_ T_ P_(A) P_(B) P_(C) P_(D) P_(E) P_(F) P(G)
                }

                const bcem_Aggregate& cdata = cursor->data();

                LOOP2_ASSERT(LINE, SPEC, A == cdata.field("A").asShort());
                LOOP2_ASSERT(LINE, SPEC, B == cdata.field("B").asInt());
                LOOP2_ASSERT(LINE, SPEC, C == cdata.field("C").asInt64());
                LOOP2_ASSERT(LINE, SPEC, D == cdata.field("D").asFloat());
                LOOP2_ASSERT(LINE, SPEC, E == cdata.field("E").asDouble());
                LOOP2_ASSERT(LINE, SPEC, F == cdata.field("F").asString());
                LOOP2_ASSERT(LINE, SPEC,
                     strlen(G) + 1 == (unsigned int)cdata.field("G").length());

                for (int j = 0; j < cdata.field("G").length(); ++j) {
                    LOOP2_ASSERT(LINE, j,
                                 cdata.field("G", j).asChar() == G[j]);
                }
            }
        }

        if (verbose) cout << "\tTesting 'query' with partial fields (short) "
                            "for DRQS 16457966" << endl;
        {
            bcedb_DtagDb db(&dtagFactory, Z);
            ASSERT(0 != db.open("BadTestDb"));
            ASSERT(!db.isOpen());

            ASSERT(0 == db.open("TestDb"));
            ASSERT(db.isOpen());

            const bdem_RecordDef *def = gDbSchema->lookupRecord("Test");
            ASSERT(def);

            bcema_SharedPtr<const bdem_RecordDef> sdef(gDbSchema, def);
            bcem_Aggregate agg(sdef);

            agg.setField("A", 8);
            vector<string> partials;
            partials.push_back("A");

            bdema_ManagedPtr<bcedb_Cursor> cursor = db.queryWithIndex(agg,
                                                    "Test", "Key12", partials);
            ASSERT(cursor);
            ASSERT(*cursor);  // this should fail if DRQS is not fixed

            for (int i = PARTIALFIELDSTESTNUM; *cursor; cursor->next(), ++i) {
                const int        LINE = DATA[i].d_lineNum;
                const char      *SPEC = DATA[i].d_spec_p;
                const short      A    = DATA[i].d_a;
                const int        B    = DATA[i].d_b;
                const long long  C    = DATA[i].d_c;
                const float      D    = DATA[i].d_d;
                const double     E    = DATA[i].d_e;
                const char      *F    = DATA[i].d_f;
                const char      *G    = DATA[i].d_g;
                if (veryVerbose) {
                    T_ T_ P_(LINE) P(SPEC)
                    T_ T_ P_(A) P_(B) P_(C) P_(D) P_(E) P_(F) P(G)
                }

                const bcem_Aggregate& cdata = cursor->data();

                LOOP2_ASSERT(LINE, SPEC, A == cdata.field("A").asShort());
                LOOP2_ASSERT(LINE, SPEC, B == cdata.field("B").asInt());
                LOOP2_ASSERT(LINE, SPEC, C == cdata.field("C").asInt64());
                LOOP2_ASSERT(LINE, SPEC, D == cdata.field("D").asFloat());
                LOOP2_ASSERT(LINE, SPEC, E == cdata.field("E").asDouble());
                LOOP2_ASSERT(LINE, SPEC, F == cdata.field("F").asString());
                LOOP2_ASSERT(LINE, SPEC,
                     strlen(G) + 1 == (unsigned int)cdata.field("G").length());

                for (int j = 0; j < cdata.field("G").length(); ++j) {
                    LOOP2_ASSERT(LINE, j,
                                 cdata.field("G", j).asChar() == G[j]);
                }
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Test open.
        // --------------------------------------------------------------------
        if (verbose) {
            bsl::cout << "Test findFirstRecord" << bsl::endl
                      << "====================" << bsl::endl;
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
            { L_,   "A:0:B:0:C:0:D:0:E:0:F::G::", 0, 0, 0, 0.0, 0.0,   "",  ""
                                                                             },
            { L_,   "A:1:B:1:C:1:D:1:E:1:F:a:G:a",1, 1, 1, 1.0, 1.0,  "a", "a"
                                                                             },
            { L_,   "A:2:B:3:C:4:D:5:E:6:F:c:G:d",2, 3, 4, 5.0, 6.0,  "c", "d"
                                                                             }
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
        bdema_ManagedPtr<bcedb_Cursor> cursor
                                             = db.getFirstRecord(sdef, "Test");

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

            LOOP2_ASSERT(LINE, SPEC, cdata.field("A").asShort() == A);
            LOOP2_ASSERT(LINE, SPEC, cdata.field("B").asInt() == B);
            LOOP2_ASSERT(LINE, SPEC, cdata.field("C").asInt64() == C);
            LOOP2_ASSERT(LINE, SPEC, cdata.field("D").asFloat() == D);
            LOOP2_ASSERT(LINE, SPEC, cdata.field("E").asDouble() == E);
            LOOP2_ASSERT(LINE, SPEC, cdata.field("F").asString() == F);
            LOOP2_ASSERT(LINE, SPEC,
                     (unsigned int)cdata.field("G").length() == strlen(G) + 1);

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
            bsl::cout << "Test Open" << bsl::endl
                      << "==========" << bsl::endl;
        }

        bcedb_DtagDb db(&dtagFactory, Z);
        ASSERT(0 != db.open("BadTestDb"));
        ASSERT(!db.isOpen());
        ASSERT(0 == db.open("TestDb"));
        ASSERT(db.isOpen());
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
