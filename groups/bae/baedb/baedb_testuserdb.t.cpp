// baedb_testuserdb.t.cpp   -*-C++-*-

#include <baedb_testuserdb.h>

#include <bdempu_aggregate.h>
#include <bdepu_ntypesparser.h>

#include <bdex_testinstream.h>              // for testing only
#include <bdex_testinstreamexception.h>     // for testing only
#include <bdex_testoutstream.h>             // for testing only

#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test is an implementation of the 'baedb_UserDb'
// protocol.  It is implemented almost entirely using the 'bdem_list'
// component, which has been fully tested.  Therefore, this test driver is
// designed to ensure that arguments to function calls are passed and returned
// correctly.  We also want to test that all value-semantic functionality (i.e.
// assignment operators, comparison operators, copy constructor etc) is
// implemented correctly.
//
// The component interface represents a user database as a 'bdem_List' object.
// The records are added into the database using the 'addRecords' method.  We
// need to verify that the 'addRecords' member function adds the records
// correctly and the 'view' method returns the correct database.
//
//      Primary Constructors, Primary Manipulators, and Basic Accessors
//      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Primary Constructors:
//   A 'baedb_TestUserDb' object is created with an empty database, by default.
//   An object's white-box state is exactly the same as its black-box state
//   (just a 'bdem_List' object), which can be modified using the primary
//   manipulator 'addRecords'.  The default constructor, in conjunction with
//   the primary manipulator, is sufficient to attain any achievable white-box
//   state.
//
//    o baedb_TestUserDb(bslma_Allocator *basicAllocator = 0);
//
// Primary Manipulators:
//   The 'addRecords' method comprises the minimal set of manipulators that
//   can, in conjunction with the default constructor, attain any achievable
//   white-box state.
//
//    o int addRecords(const char *inputRecords);
//
// Basic Accessors:
//   This is the maximal set of accessors that have direct contact with the
//   black-box representation of the object.  The 'numRecords' and 'view'
//   methods are obvious members of this set.  The protocol's lookup and
//   password verification methods are also part of this set.
//
//    o virtual int lookupFirmNumberByLogin(...) const;
//    o virtual int lookupFirmNumberByUserNumber(...) const;
//    o virtual int lookupFirmNumberByUUID(...) const;
//    o virtual int lookupLoginByUserNumber(...) const;
//    o virtual int lookupLoginByUUID(...) const;
//    o virtual int lookupUserNameByLogin(...) const;
//    o virtual int lookupUserNameByUserNumber(...) const;
//    o virtual int lookupUserNameByUUID(...) const;
//    o virtual int lookupUserNumberByLogin(...) const;
//    o virtual int lookupUserNumberByUUID(...) const;
//    o virtual int lookupUUIDByLogin(...) const;
//    o virtual int lookupUUIDByUserNumber(...) const;
//    o int numRecords() const;
//    o const bdem_List& view() const;
//
//-----------------------------------------------------------------------------
// CLASS METHODS
// [10] static int maxSupportedBdexVersion();
//
// CREATORS
// [ 2] baedb_TestUserDb(bslma_Allocator *basicAllocator = 0);
// [ 7] baedb_TestUserDb(const baedb_TestUserDb&  original,
//                       bslma_Allocator         *basicAllocator = 0);
// [12] baedb_TestUserDb(const char      *inputRecords,
//                       bslma_Allocator *basicAllocator = 0);
// [ 2] virtual ~baedb_TestUserDb();
//
// MANIPULATORS
// [11] int addRecords(const char *inputRecords);
// [ 3] int addRecord(const char*, int, int, int, int, const char*, int,
//                                                     const char*, int);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
// [ 9] const baedb_TestUserDb& operator=(const baedb_TestUserDb& rhs);
// [13] void removeAll();
// [16] int removeByLogin(const char *login, int loginLen);
// [15] int removeByUserNumber(int userNumber);
// [14] int removeByUUID(int uuid);
//
// ACCESSORS
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 4] virtual int lookupFirmNumberByLogin(...) const;
// [ 4] virtual int lookupFirmNumberByUserNumber(...) const;
// [ 4] virtual int lookupFirmNumberByUUID(...) const;
// [ 4] virtual int lookupLoginByUserNumber(...) const;
// [ 4] virtual int lookupLoginByUUID(...) const;
// [ 4] virtual int lookupUserNameByLogin(...) const;
// [ 4] virtual int lookupUserNameByUserNumber(...) const;
// [ 4] virtual int lookupUserNameByUUID(...) const;
// [ 4] virtual int lookupUserNumberByLogin(...) const;
// [ 4] virtual int lookupUserNumberByUUID(...) const;
// [ 4] virtual int lookupUUIDByLogin(...) const;
// [ 4] virtual int lookupUUIDByUserNumber(...) const;
// [ 4] int numRecords() const;
// [ 5] bsl::ostream& print(bsl::ostream& stream) const;
// [ 4] const bdem_List& view() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const baedb_TestUserDb&, const baedb_TestUserDb&);
// [ 6] bool operator!=(const baedb_TestUserDb&, const baedb_TestUserDb&);
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const baedb_TestUserDb&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] BOOTSTRAP: int addRecords(const char *inputRecords);
// [ 8] Obj g(const int *spec, const GeneratorData *data);
// [10] bdex_InStream& operator>>(bdex_InStream&, baedb_TestUserDb&);
// [10] bdex_OutStream& operator<<(bdex_OutStream&, const baedb_TestUserDb&);
// [17] USAGE EXAMPLE
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
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
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_  cout << "\t" << flush;            // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef baedb_TestUserDb   Obj;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

typedef struct {
    // This structure is used as an input to the 'g' generator function.  It
    // contains all the fields necessary to generate a user record.  It also
    // contains a source line number, which can be used for debugging purposes.

    int         d_lineNum;    // source line number
    const char *d_userName;   // user name
    int         d_uuid;       // uuid
    int         d_userNumber; // user number
    int         d_firmNumber; // firm number
    const char *d_login;      // login ID
    const char *d_password;   // password
} GeneratorData;

enum Fields {
    // This enumeration defines the 'Fields' in each user record.

    FIELD_USER_NAME = 0,  // user name
    FIELD_UUID,           // unique ID
    FIELD_USER_NUMBER,    // user number
    FIELD_FIRM_NUMBER,    // firm number
    FIELD_LOGIN,          // login ID
    FIELD_PASSWORD        // password
};

//=============================================================================
//                            HELPER FUNCTIONS
//-----------------------------------------------------------------------------
void listToChar(char *buffer, int bufferLen, const bdem_List& list)
    // Load into the specified 'buffer' of the specified 'bufferLen' the string
    // representation of the specified 'list'.  The resulting 'buffer' can be
    // used as input to the 'addRecords' method.
{
    int i;
    bsl::vector<char> vChar;
    bdempu_Aggregate::generateList(&vChar, list);
    for (i = 0; i < bufferLen-1 && '\0' != vChar[i]; ++i) {
        buffer[i] = vChar[i];
    }
    buffer[i] = '\0';
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

const char INPUT_DATA[] =
    "{\n"
        "LIST {\n"
            "STRING  \"User Name 0\",\n"
            "INT     12345,          // uuid\n"
            "INT     23456,          // user number\n"
            "INT     45678,          // firm number\n"
            "STRING  \"login0\",\n"
            "STRING  \"password0\"\n"
        "},\n"
        "LIST {\n"
            "STRING  \"User Name 1\",\n"
            "INT     34567,          // uuid\n"
            "INT     65432,          // user number\n"
            "INT     92836,          // firm number\n"
            "STRING  \"login1\",\n"
            "STRING  \"password1\"\n"
        "},\n"
        "LIST {\n"
            "STRING  \"User Name 2\",\n"
            "INT     82621,          // uuid\n"
            "INT     27653,          // user number\n"
            "INT     45675,          // firm number\n"
            "STRING  \"login2\",\n"
            "STRING  \"password2\"\n"
        "}\n"
    "}\n";

// The code above is a sample user database with three users.  This data can be
// loaded into a 'baedb_TestUserDb' object using the following code:

void loadRecords(baedb_TestUserDb *userDb, const char *inputRecords)
    // Parse the specified 'inputRecords' and store the records in the
    // specified 'userDb'.
{
    int retCode = userDb->addRecords(inputRecords);
    ASSERT(3 == retCode);
}

// The database can then be queried using any of the lookup functions declared
// in the protocol.  The records can also be examined using the 'view' method:

void verifyRecords(const baedb_TestUserDb *userDb)
    // Verify that the specified 'userDb' contains the records as specified in
    // the input data above.
{
    // verify using the protocol's lookup method
    bsl::string userName;
    bsl::string login;
    int         uuid;
    int         userNumber;
    int         firmNumber;
    int         len;
    int         retCode;

    uuid = 12345;

    retCode = userDb->lookupUserNameByUUID(&userName, uuid);
    ASSERT(baedb_UserDb::SUCCESS == retCode);
    ASSERT("User Name 0"         == userName);

    retCode = userDb->lookupUserNumberByUUID(&userNumber, uuid);
    ASSERT(baedb_UserDb::SUCCESS == retCode);
    ASSERT(23456                 == userNumber);

    retCode = userDb->lookupFirmNumberByUUID(&firmNumber, uuid);
    ASSERT(baedb_UserDb::SUCCESS == retCode);
    ASSERT(45678                 == firmNumber);

    retCode = userDb->lookupLoginByUUID(&login, uuid);
    ASSERT(baedb_UserDb::SUCCESS == retCode);
    ASSERT("login0"              == login);
}

// The 'baedb_TestUserDb' class also provides 'addRecord' and 'remove' methods
// for manipulating records individually:

void manipulate(baedb_TestUserDb *userDb)
    // Test the 'addRecord' and 'removeByLogin' functions.
{
    const char *userName3 = "User Name 3";
    const char *userName4 = "User Name 4";
    const char *login3    = "login3";
    const char *login4    = "login4";
    const char *password3 = "password3";
    const char *password4 = "password4";
    const int   LOGIN_POS = 4;

    userDb->addRecord(userName3,
                      strlen(userName3),
                      28172,
                      19283,
                      21772,
                      login3,
                      strlen(login3),
                      password3,
                      strlen(password3));
    userDb->addRecord(userName4,
                      strlen(userName4),
                      32356,
                      54333,
                      33333,
                      login4,
                      strlen(login4),
                      password4,
                      strlen(password4));
    ASSERT(5 == userDb->numRecords());

    userDb->removeByLogin(login3, strlen(login3));
    ASSERT(4        == userDb->numRecords());
    ASSERT("login2" == userDb->view().theList(2).theString(LOGIN_POS));
    ASSERT(login4   == userDb->view().theList(3).theString(LOGIN_POS));
}

//=============================================================================
//                   GENERATOR FUNCTION 'g' FOR TESTING
//-----------------------------------------------------------------------------
// The following function interprets each entry in the given 'spec' as an index
// into the given 'data' array.  For each entry 'i' in 'spec', the generator
// function inserts the record specified by 'data[i]' into the object.  A
// negative entry in 'spec' is treated as a terminator and the resulting object
// is returned by value.
//
// LANGUAGE SPECIFICATION
// ----------------------
//
// <SPEC>       ::= (<INT>)* <TERMINATOR>
//
// <INT>        ::= 0..INT_MAX
//
// <TERMINATOR> ::= INT_MIN..-1
//-----------------------------------------------------------------------------

Obj g(const int *spec, const GeneratorData *data)
    // Return, by value, a new object corresponding to the specified 'spec' and
    // 'data'.
{
    Obj mX;

    int i;
    while (0 <= (i = *spec)) {
        const int   LINE        = data[i].d_lineNum;
        const char *USER_NAME   = data[i].d_userName;
        const int   UUID        = data[i].d_uuid;
        const int   USER_NUMBER = data[i].d_userNumber;
        const int   FIRM_NUMBER = data[i].d_firmNumber;
        const char *LOGIN       = data[i].d_login;
        const char *PASSWORD    = data[i].d_password;

        mX.addRecord(USER_NAME, strlen(USER_NAME),
                     UUID, USER_NUMBER, FIRM_NUMBER,
                     LOGIN, strlen(LOGIN),
                     PASSWORD, strlen(PASSWORD));
        spec++;
    }

    return mX;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    static int test = argc > 1 ? atoi(argv[1]) : 0;
    static int verbose = argc > 2;
    static int veryVerbose = argc > 3;
    static int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 17: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //   This will test the usage example from the component's header file.
        //
        // Concerns:
        //   We want to make sure that the usage example from the component's
        //   header file compiles and runs correctly.
        //
        // Plan:
        //   Copy and paste the usage example from the component's header file.
        //   Change 'assert' to 'ASSERT' and put the input data into a
        //   'const char[]'.
        //
        // Testing:
        //   Usage Example.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE TEST"
                          << "\n==========" << endl;

        baedb_TestUserDb mX;  const baedb_TestUserDb& X = mX;

        loadRecords(&mX, INPUT_DATA);
        verifyRecords(&X);
        manipulate(&mX);

        if (verbose) cout << "\nEnd of Usage Test." << endl;
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'removeByLogin' METHOD
        //   This will thoroughly test the 'removeByLogin' method.
        //
        // Concerns:
        //   We want to make sure that the 'removeByLogin' method works
        //   correctly.  After the call, the database should not contain any
        //   record with the specified login ID, and must contain all other
        //   records.
        //
        // Plan:
        //   The 'removeByLogin' method should work no matter where the record
        //   is stored in the database.  For a database with 'NUM_DATA'
        //   records, attempt to remove the record at all possible 'removeIdx'
        //   where 0 <= removeIdx < NUM_DATA.  Verify that the record was
        //   removed successfully.
        //
        //   Also, test the 'removeByLogin' method with login IDs that do not
        //   exist in the database.  Verify that the database is not affected.
        //
        // Testing:
        //   int removeByLogin(const char *login, int loginLen);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeByLogin' Method"
                          << "\n==============================" << endl;

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_userName;         // user name
            int         d_uuid;             // uuid
            int         d_userNumber;       // user number
            int         d_firmNumber;       // firm number
            const char *d_login;            // login ID
            const char *d_password;         // password
        } DATA[] = {
            {
                // minimum bounds
                L_,                         // source line number
                "",                         // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "l",                        // login ID
                "",                         // password
            },
            {
                // maximum bounds
                L_,                         // source line number
                "",                         // user name
                INT_MAX,                    // uuid
                INT_MAX,                    // user number
                INT_MAX,                    // firm number
                "login1",                   // login ID
                "",                         // password
            },
            // Note, we cannot have orthogonal cases for uuid, user name
            // and login ID because these fields must *always* be unique.
            {
                // orthogonal: base case
                L_,                         // source line number
                "user name",                // user name
                11,                         // uuid
                11,                         // user number
                11,                         // firm number
                "login11",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: user name
                L_,                         // source line number
                "new user name",            // user name
                22,                         // uuid
                22,                         // user number
                1,                          // firm number
                "login22",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: firm number
                L_,                         // source line number
                "user name",                // user name
                33,                         // uuid
                33,                         // user number
                9999,                       // firm number
                "login33",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: password
                L_,                         // source line number
                "user name",                // user name
                44,                         // uuid
                44,                         // user number
                1,                          // firm number
                "login44",                  // login ID
                "new password",             // password
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with existing login IDs." << endl;
        {
            // Insert 'NUM_DATA' records into a database, and test the
            // 'removeByLogin' method using login IDs from each entry in DATA.

            for (int removeIdx = 0; removeIdx < NUM_DATA; ++removeIdx) {
                // Create a database with 'NUM_DATA' records, then remove the
                // record at the specified 'removeIdx'.  Then verify that the
                // record was correctly removed and the remaining records are
                // not affected.

                Obj mX; const Obj& X = mX;
                int i, j;

                // add 'NUM_DATA' records
                for (i = 0; i < NUM_DATA; ++i) {
                    const int   LINE        = DATA[i].d_lineNum;
                    const char *USER_NAME   = DATA[i].d_userName;
                    const int   UUID        = DATA[i].d_uuid;
                    const int   USER_NUMBER = DATA[i].d_userNumber;
                    const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                    const char *LOGIN       = DATA[i].d_login;
                    const char *PASSWORD    = DATA[i].d_password;

                    int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                              UUID, USER_NUMBER, FIRM_NUMBER,
                                              LOGIN, strlen(LOGIN),
                                              PASSWORD, strlen(PASSWORD));
                    LOOP2_ASSERT(LINE, result,
                                 baedb_UserDb::SUCCESS == result);
                }

                // remove the record at 'removeIdx'
                int result = mX.removeByLogin(DATA[removeIdx].d_login,
                                              strlen(DATA[removeIdx].d_login));
                LOOP2_ASSERT(DATA[removeIdx].d_lineNum, result,
                             baedb_UserDb::SUCCESS == result);
                LOOP3_ASSERT(removeIdx, NUM_DATA-1,   X.numRecords(),
                                        NUM_DATA-1 == X.numRecords());

                // verify the records
                for (i = 0, j = 0; j < NUM_DATA; ++j) {
                    if (j == removeIdx) {
                        continue;
                    }

                    const int    LINE        = DATA[j].d_lineNum;
                    const string USER_NAME   = DATA[j].d_userName;
                    const int    UUID        = DATA[j].d_uuid;
                    const int    USER_NUMBER = DATA[j].d_userNumber;
                    const int    FIRM_NUMBER = DATA[j].d_firmNumber;
                    const string LOGIN       = DATA[j].d_login;
                    const string PASSWORD    = DATA[j].d_password;

                    LOOP3_ASSERT(
                               LINE,
                               USER_NAME,
                               X.view().theList(i).theString(FIELD_USER_NAME),
                               USER_NAME ==
                               X.view().theList(i).theString(FIELD_USER_NAME));

                    LOOP3_ASSERT(LINE,
                                 UUID,
                                 X.view().theList(i).theInt(FIELD_UUID),
                                 UUID ==
                                 X.view().theList(i).theInt(FIELD_UUID));

                    LOOP3_ASSERT(
                                LINE,
                                USER_NUMBER,
                                X.view().theList(i).theInt(FIELD_USER_NUMBER),
                                USER_NUMBER ==
                                X.view().theList(i).theInt(FIELD_USER_NUMBER));

                    LOOP3_ASSERT(
                                LINE,
                                FIRM_NUMBER,
                                X.view().theList(i).theInt(FIELD_FIRM_NUMBER),
                                FIRM_NUMBER ==
                                X.view().theList(i).theInt(FIELD_FIRM_NUMBER));

                    LOOP3_ASSERT(LINE,
                                 LOGIN,
                                 X.view().theList(i).theString(FIELD_LOGIN),
                                 LOGIN ==
                                 X.view().theList(i).theString(FIELD_LOGIN));

                    LOOP3_ASSERT(
                                LINE,
                                PASSWORD,
                                X.view().theList(i).theString(FIELD_PASSWORD),
                                PASSWORD ==
                                X.view().theList(i).theString(FIELD_PASSWORD));
                    ++i;
                }
            }
        }

        if (verbose) cout << "\nTesting with non-existing login IDs." << endl;
        {
            // Insert 'NUM_DATA' records into a database, and test the
            // 'removeByLogin' method using arbitrary login IDs that are not in
            // the database.

            Obj mX; const Obj& X = mX;
            int i;

            // add 'NUM_DATA' records
            for (i = 0; i < NUM_DATA; ++i) {
                const int   LINE        = DATA[i].d_lineNum;
                const char *USER_NAME   = DATA[i].d_userName;
                const int   UUID        = DATA[i].d_uuid;
                const int   USER_NUMBER = DATA[i].d_userNumber;
                const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                const char *LOGIN       = DATA[i].d_login;
                const char *PASSWORD    = DATA[i].d_password;

                int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                          UUID, USER_NUMBER, FIRM_NUMBER,
                                          LOGIN, strlen(LOGIN),
                                          PASSWORD, strlen(PASSWORD));
                LOOP2_ASSERT(LINE, result,
                             baedb_UserDb::SUCCESS == result);
            }

            static const struct {
                int         d_lineNum;    // source line number
                const char *d_login;      // login ID to remove
            } INVALID_LOGINS[] = {
                //line  login
                //----  -----
                { L_,   "login98765" },
                { L_,   "login12345" },
                { L_,   "login10293" },
                { L_,   "login74839" },
                { L_,   "login45678" },
                { L_,   "login87654" },
            };
            const int NUM_INVALID_LOGINS = sizeof INVALID_LOGINS
                                           / sizeof *INVALID_LOGINS;

            for (i = 0; i < NUM_INVALID_LOGINS; ++i) {
                // Attempt to remove non-existing login ID, and verify that the
                // database is not affected.

                const int   INVALID_LINE  = INVALID_LOGINS[i].d_lineNum;
                const char *INVALID_LOGIN = INVALID_LOGINS[i].d_login;

                int result = mX.removeByLogin(INVALID_LOGIN,
                                              strlen(INVALID_LOGIN));
                LOOP2_ASSERT(INVALID_LINE, result,
                             baedb_UserDb::NOT_FOUND == result);
                LOOP3_ASSERT(INVALID_LINE, NUM_DATA,   X.numRecords(),
                                           NUM_DATA == X.numRecords());

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int    LINE        = DATA[j].d_lineNum;
                    const string USER_NAME   = DATA[j].d_userName;
                    const int    UUID        = DATA[j].d_uuid;
                    const int    USER_NUMBER = DATA[j].d_userNumber;
                    const int    FIRM_NUMBER = DATA[j].d_firmNumber;
                    const string LOGIN       = DATA[j].d_login;
                    const string PASSWORD    = DATA[j].d_password;

                    LOOP3_ASSERT(
                               LINE,
                               USER_NAME,
                               X.view().theList(j).theString(FIELD_USER_NAME),
                               USER_NAME ==
                               X.view().theList(j).theString(FIELD_USER_NAME));

                    LOOP3_ASSERT(LINE,
                                 UUID,
                                 X.view().theList(j).theInt(FIELD_UUID),
                                 UUID ==
                                 X.view().theList(j).theInt(FIELD_UUID));

                    LOOP3_ASSERT(
                                LINE,
                                USER_NUMBER,
                                X.view().theList(j).theInt(FIELD_USER_NUMBER),
                                USER_NUMBER ==
                                X.view().theList(j).theInt(FIELD_USER_NUMBER));

                    LOOP3_ASSERT(
                                LINE,
                                FIRM_NUMBER,
                                X.view().theList(j).theInt(FIELD_FIRM_NUMBER),
                                FIRM_NUMBER ==
                                X.view().theList(j).theInt(FIELD_FIRM_NUMBER));

                    LOOP3_ASSERT(LINE,
                                 LOGIN,
                                 X.view().theList(j).theString(FIELD_LOGIN),
                                 LOGIN ==
                                 X.view().theList(j).theString(FIELD_LOGIN));

                    LOOP3_ASSERT(
                                LINE,
                                PASSWORD,
                                X.view().theList(j).theString(FIELD_PASSWORD),
                                PASSWORD ==
                                X.view().theList(j).theString(FIELD_PASSWORD));
                }
            }
        }

        if (verbose) cout << "\nEnd of 'removeByLogin' Method Test." << endl;
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'removeByUserNumber' METHOD
        //   This will thoroughly test the 'removeByUserNumber' method.
        //
        // Concerns:
        //   We want to make sure that the 'removeByUserNumber' method works
        //   correctly.  After the call, the database should not contain any
        //   record with the specified user number, and must contain all other
        //   records.
        //
        // Plan:
        //   The 'removeByUserNumber' method should work no matter where the
        //   record is stored in the database.  For a database with 'NUM_DATA'
        //   records, attempt to remove the record at all possible 'removeIdx'
        //   where 0 <= removeIdx < NUM_DATA.  Verify that the record was
        //   removed successfully.
        //
        //   Also, test the 'removeByUserNumber' method with user numbers that
        //   do not exist in the database.  Verify that the database is not
        //   affected.
        //
        // Testing:
        //   int removeByUserNumber(int userNumber);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeByUserNumber' Method"
                          << "\n===================================" << endl;

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_userName;         // user name
            int         d_uuid;             // uuid
            int         d_userNumber;       // user number
            int         d_firmNumber;       // firm number
            const char *d_login;            // login ID
            const char *d_password;         // password
        } DATA[] = {
            {
                // minimum bounds
                L_,                         // source line number
                "",                         // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "l",                        // login ID
                "",                         // password
            },
            {
                // maximum bounds
                L_,                         // source line number
                "",                         // user name
                INT_MAX,                    // uuid
                INT_MAX,                    // user number
                INT_MAX,                    // firm number
                "login1",                   // login ID
                "",                         // password
            },
            // Note, we cannot have orthogonal cases for uuid, user name
            // and login ID because these fields must *always* be unique.
            {
                // orthogonal: base case
                L_,                         // source line number
                "user name",                // user name
                11,                         // uuid
                11,                         // user number
                11,                         // firm number
                "login11",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: user name
                L_,                         // source line number
                "new user name",            // user name
                22,                         // uuid
                22,                         // user number
                1,                          // firm number
                "login22",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: firm number
                L_,                         // source line number
                "user name",                // user name
                33,                         // uuid
                33,                         // user number
                9999,                       // firm number
                "login33",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: password
                L_,                         // source line number
                "user name",                // user name
                44,                         // uuid
                44,                         // user number
                1,                          // firm number
                "login44",                  // login ID
                "new password",             // password
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with existing user numbers." << endl;
        {
            // Insert 'NUM_DATA' records into a database, and test the
            // 'removeByUserNumber' method using user numbers from each entry
            // in DATA.

            for (int removeIdx = 0; removeIdx < NUM_DATA; ++removeIdx) {
                // Create a database with 'NUM_DATA' records, then remove the
                // record at the specified 'removeIdx'.  Then verify that the
                // record was correctly removed and the remaining records are
                // not affected.

                Obj mX; const Obj& X = mX;
                int i, j;

                // add 'NUM_DATA' records
                for (i = 0; i < NUM_DATA; ++i) {
                    const int   LINE        = DATA[i].d_lineNum;
                    const char *USER_NAME   = DATA[i].d_userName;
                    const int   UUID        = DATA[i].d_uuid;
                    const int   USER_NUMBER = DATA[i].d_userNumber;
                    const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                    const char *LOGIN       = DATA[i].d_login;
                    const char *PASSWORD    = DATA[i].d_password;

                    int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                              UUID, USER_NUMBER, FIRM_NUMBER,
                                              LOGIN, strlen(LOGIN),
                                              PASSWORD, strlen(PASSWORD));
                    LOOP2_ASSERT(LINE, result,
                                 baedb_UserDb::SUCCESS == result);
                }

                // remove the record at 'removeIdx'
                int result = mX.removeByUserNumber(
                                                 DATA[removeIdx].d_userNumber);
                LOOP2_ASSERT(DATA[removeIdx].d_lineNum, result,
                             baedb_UserDb::SUCCESS == result);
                LOOP3_ASSERT(removeIdx, NUM_DATA-1,   X.numRecords(),
                                        NUM_DATA-1 == X.numRecords());

                // verify the records
                for (i = 0, j = 0; j < NUM_DATA; ++j) {
                    if (j == removeIdx) {
                        continue;
                    }

                    const int    LINE        = DATA[j].d_lineNum;
                    const string USER_NAME   = DATA[j].d_userName;
                    const int    UUID        = DATA[j].d_uuid;
                    const int    USER_NUMBER = DATA[j].d_userNumber;
                    const int    FIRM_NUMBER = DATA[j].d_firmNumber;
                    const string LOGIN       = DATA[j].d_login;
                    const string PASSWORD    = DATA[j].d_password;

                    LOOP3_ASSERT(
                               LINE,
                               USER_NAME,
                               X.view().theList(i).theString(FIELD_USER_NAME),
                               USER_NAME ==
                               X.view().theList(i).theString(FIELD_USER_NAME));

                    LOOP3_ASSERT(LINE,
                                 UUID,
                                 X.view().theList(i).theInt(FIELD_UUID),
                                 UUID ==
                                 X.view().theList(i).theInt(FIELD_UUID));

                    LOOP3_ASSERT(
                                LINE,
                                USER_NUMBER,
                                X.view().theList(i).theInt(FIELD_USER_NUMBER),
                                USER_NUMBER ==
                                X.view().theList(i).theInt(FIELD_USER_NUMBER));

                    LOOP3_ASSERT(
                                LINE,
                                FIRM_NUMBER,
                                X.view().theList(i).theInt(FIELD_FIRM_NUMBER),
                                FIRM_NUMBER ==
                                X.view().theList(i).theInt(FIELD_FIRM_NUMBER));

                    LOOP3_ASSERT(LINE,
                                 LOGIN,
                                 X.view().theList(i).theString(FIELD_LOGIN),
                                 LOGIN ==
                                 X.view().theList(i).theString(FIELD_LOGIN));

                    LOOP3_ASSERT(
                                LINE,
                                PASSWORD,
                                X.view().theList(i).theString(FIELD_PASSWORD),
                                PASSWORD ==
                                X.view().theList(i).theString(FIELD_PASSWORD));
                    ++i;
                }
            }
        }

        if (verbose) cout << "\nTesting with non-existing user numbers."
                          << endl;
        {
            // Insert 'NUM_DATA' records into a database, and test the
            // 'removeByUserNumber' method using arbitrary user numbers that
            // are not in the database.

            Obj mX; const Obj& X = mX;
            int i;

            // add 'NUM_DATA' records
            for (i = 0; i < NUM_DATA; ++i) {
                const int   LINE        = DATA[i].d_lineNum;
                const char *USER_NAME   = DATA[i].d_userName;
                const int   UUID        = DATA[i].d_uuid;
                const int   USER_NUMBER = DATA[i].d_userNumber;
                const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                const char *LOGIN       = DATA[i].d_login;
                const char *PASSWORD    = DATA[i].d_password;

                int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                          UUID, USER_NUMBER, FIRM_NUMBER,
                                          LOGIN, strlen(LOGIN),
                                          PASSWORD, strlen(PASSWORD));
                LOOP2_ASSERT(LINE, result,
                             baedb_UserDb::SUCCESS == result);
            }

            static const struct {
                int d_lineNum;    // source line number
                int d_userNumber; // user number to remove
            } INVALID_USERNUMBERS[] = {
                //line  user number
                //----  -----------
                { L_,   98765       },
                { L_,   12345       },
                { L_,   10293       },
                { L_,   74839       },
                { L_,   45678       },
                { L_,   87654       },
            };
            const int NUM_INVALID_USERNUMBERS = sizeof INVALID_USERNUMBERS
                                                / sizeof *INVALID_USERNUMBERS;

            for (i = 0; i < NUM_INVALID_USERNUMBERS; ++i) {
                // Attempt to remove non-existing user number, and verify that
                // the database is not affected.

                const int INVALID_LINE   = INVALID_USERNUMBERS[i].d_lineNum;
                const int INVALID_USERNUMBER
                                         = INVALID_USERNUMBERS[i].d_userNumber;

                int result = mX.removeByUserNumber(INVALID_USERNUMBER);
                LOOP2_ASSERT(INVALID_LINE, result,
                             baedb_UserDb::NOT_FOUND == result);
                LOOP3_ASSERT(INVALID_LINE, NUM_DATA,   X.numRecords(),
                                           NUM_DATA == X.numRecords());

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int    LINE        = DATA[j].d_lineNum;
                    const string USER_NAME   = DATA[j].d_userName;
                    const int    UUID        = DATA[j].d_uuid;
                    const int    USER_NUMBER = DATA[j].d_userNumber;
                    const int    FIRM_NUMBER = DATA[j].d_firmNumber;
                    const string LOGIN       = DATA[j].d_login;
                    const string PASSWORD    = DATA[j].d_password;

                    LOOP3_ASSERT(
                               LINE,
                               USER_NAME,
                               X.view().theList(j).theString(FIELD_USER_NAME),
                               USER_NAME ==
                               X.view().theList(j).theString(FIELD_USER_NAME));

                    LOOP3_ASSERT(LINE,
                                 UUID,
                                 X.view().theList(j).theInt(FIELD_UUID),
                                 UUID ==
                                 X.view().theList(j).theInt(FIELD_UUID));

                    LOOP3_ASSERT(
                                LINE,
                                USER_NUMBER,
                                X.view().theList(j).theInt(FIELD_USER_NUMBER),
                                USER_NUMBER ==
                                X.view().theList(j).theInt(FIELD_USER_NUMBER));

                    LOOP3_ASSERT(
                                LINE,
                                FIRM_NUMBER,
                                X.view().theList(j).theInt(FIELD_FIRM_NUMBER),
                                FIRM_NUMBER ==
                                X.view().theList(j).theInt(FIELD_FIRM_NUMBER));

                    LOOP3_ASSERT(LINE,
                                 LOGIN,
                                 X.view().theList(j).theString(FIELD_LOGIN),
                                 LOGIN ==
                                 X.view().theList(j).theString(FIELD_LOGIN));

                    LOOP3_ASSERT(
                                LINE,
                                PASSWORD,
                                X.view().theList(j).theString(FIELD_PASSWORD),
                                PASSWORD ==
                                X.view().theList(j).theString(FIELD_PASSWORD));
                }
            }
        }

        if (verbose) cout << "\nEnd of 'removeByUserNumber' Method Test."
                          << endl;
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'removeByUUID' METHOD
        //   This will thoroughly test the 'removeByUUID' method.
        //
        // Concerns:
        //   We want to make sure that the 'removeByUUID' method works
        //   correctly.  After the call, the database should not contain any
        //   record with the specified uuid, and must contain all other
        //   records.
        //
        // Plan:
        //   The 'removeByUUID' method should work no matter where the record
        //   is stored in the database.  For a database with 'NUM_DATA'
        //   records, attempt to remove the record at all possible 'removeIdx'
        //   where 0 <= removeIdx < NUM_DATA.  Verify that the record was
        //   removed successfully.
        //
        //   Also, test the 'removeByUUID' method with UUIDs that do not exist
        //   in the database.  Verify that the database is not affected.
        //
        // Testing:
        //   int removeByUUID(int uuid);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeByUUID' Method"
                          << "\n=============================" << endl;

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_userName;         // user name
            int         d_uuid;             // uuid
            int         d_userNumber;       // user number
            int         d_firmNumber;       // firm number
            const char *d_login;            // login ID
            const char *d_password;         // password
        } DATA[] = {
            {
                // minimum bounds
                L_,                         // source line number
                "",                         // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "l",                        // login ID
                "",                         // password
            },
            {
                // maximum bounds
                L_,                         // source line number
                "",                         // user name
                INT_MAX,                    // uuid
                INT_MAX,                    // user number
                INT_MAX,                    // firm number
                "login1",                   // login ID
                "",                         // password
            },
            // Note, we cannot have orthogonal cases for uuid, user name
            // and login ID because these fields must *always* be unique.
            {
                // orthogonal: base case
                L_,                         // source line number
                "user name",                // user name
                11,                         // uuid
                11,                         // user number
                11,                         // firm number
                "login11",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: user name
                L_,                         // source line number
                "new user name",            // user name
                22,                         // uuid
                22,                         // user number
                1,                          // firm number
                "login22",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: firm number
                L_,                         // source line number
                "user name",                // user name
                33,                         // uuid
                33,                         // user number
                9999,                       // firm number
                "login33",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: password
                L_,                         // source line number
                "user name",                // user name
                44,                         // uuid
                44,                         // user number
                1,                          // firm number
                "login44",                  // login ID
                "new password",             // password
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with existing UUIDs." << endl;
        {
            // Insert 'NUM_DATA' records into a database, and test the
            // 'removeByUUID' method using UUIDs from each entry in DATA.

            for (int removeIdx = 0; removeIdx < NUM_DATA; ++removeIdx) {
                // Create a database with 'NUM_DATA' records, then remove the
                // record at the specified 'removeIdx'.  Then verify that the
                // record was correctly removed and the remaining records are
                // not affected.

                Obj mX; const Obj& X = mX;
                int i, j;

                // add 'NUM_DATA' records
                for (i = 0; i < NUM_DATA; ++i) {
                    const int   LINE        = DATA[i].d_lineNum;
                    const char *USER_NAME   = DATA[i].d_userName;
                    const int   UUID        = DATA[i].d_uuid;
                    const int   USER_NUMBER = DATA[i].d_userNumber;
                    const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                    const char *LOGIN       = DATA[i].d_login;
                    const char *PASSWORD    = DATA[i].d_password;

                    int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                              UUID, USER_NUMBER, FIRM_NUMBER,
                                              LOGIN, strlen(LOGIN),
                                              PASSWORD, strlen(PASSWORD));
                    LOOP2_ASSERT(LINE, result,
                                 baedb_UserDb::SUCCESS == result);
                }

                // remove the record at 'removeIdx'
                int result = mX.removeByUUID(DATA[removeIdx].d_uuid);
                LOOP2_ASSERT(DATA[removeIdx].d_lineNum, result,
                             baedb_UserDb::SUCCESS == result);
                LOOP3_ASSERT(removeIdx, NUM_DATA-1,   X.numRecords(),
                                        NUM_DATA-1 == X.numRecords());

                // verify the records
                for (i = 0, j = 0; j < NUM_DATA; ++j) {
                    if (j == removeIdx) {
                        continue;
                    }

                    const int    LINE        = DATA[j].d_lineNum;
                    const string USER_NAME   = DATA[j].d_userName;
                    const int    UUID        = DATA[j].d_uuid;
                    const int    USER_NUMBER = DATA[j].d_userNumber;
                    const int    FIRM_NUMBER = DATA[j].d_firmNumber;
                    const string LOGIN       = DATA[j].d_login;
                    const string PASSWORD    = DATA[j].d_password;

                    LOOP3_ASSERT(
                               LINE,
                               USER_NAME,
                               X.view().theList(i).theString(FIELD_USER_NAME),
                               USER_NAME ==
                               X.view().theList(i).theString(FIELD_USER_NAME));

                    LOOP3_ASSERT(LINE,
                                 UUID,
                                 X.view().theList(i).theInt(FIELD_UUID),
                                 UUID ==
                                 X.view().theList(i).theInt(FIELD_UUID));

                    LOOP3_ASSERT(
                                LINE,
                                USER_NUMBER,
                                X.view().theList(i).theInt(FIELD_USER_NUMBER),
                                USER_NUMBER ==
                                X.view().theList(i).theInt(FIELD_USER_NUMBER));

                    LOOP3_ASSERT(
                                LINE,
                                FIRM_NUMBER,
                                X.view().theList(i).theInt(FIELD_FIRM_NUMBER),
                                FIRM_NUMBER ==
                                X.view().theList(i).theInt(FIELD_FIRM_NUMBER));

                    LOOP3_ASSERT(LINE,
                                 LOGIN,
                                 X.view().theList(i).theString(FIELD_LOGIN),
                                 LOGIN ==
                                 X.view().theList(i).theString(FIELD_LOGIN));

                    LOOP3_ASSERT(
                                LINE,
                                PASSWORD,
                                X.view().theList(i).theString(FIELD_PASSWORD),
                                PASSWORD ==
                                X.view().theList(i).theString(FIELD_PASSWORD));
                    ++i;
                }
            }
        }

        if (verbose) cout << "\nTesting with non-existing UUIDs." << endl;
        {
            // Insert 'NUM_DATA' records into a database, and test the
            // 'removeByUUID' method using arbitrary UUIDs that are not in the
            // database.

            Obj mX; const Obj& X = mX;
            int i;

            // add 'NUM_DATA' records
            for (i = 0; i < NUM_DATA; ++i) {
                const int   LINE        = DATA[i].d_lineNum;
                const char *USER_NAME   = DATA[i].d_userName;
                const int   UUID        = DATA[i].d_uuid;
                const int   USER_NUMBER = DATA[i].d_userNumber;
                const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                const char *LOGIN       = DATA[i].d_login;
                const char *PASSWORD    = DATA[i].d_password;

                int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                          UUID, USER_NUMBER, FIRM_NUMBER,
                                          LOGIN, strlen(LOGIN),
                                          PASSWORD, strlen(PASSWORD));
                LOOP2_ASSERT(LINE, result,
                             baedb_UserDb::SUCCESS == result);
            }

            static const struct {
                int d_lineNum;  // source line number
                int d_uuid;     // UUID to remove
            } INVALID_UUIDS[] = {
                //line  uuid
                //----  ----
                { L_,   98765 },
                { L_,   12345 },
                { L_,   10293 },
                { L_,   74839 },
                { L_,   45678 },
                { L_,   87654 },
            };
            const int NUM_INVALID_UUIDS = sizeof INVALID_UUIDS
                                          / sizeof *INVALID_UUIDS;

            for (i = 0; i < NUM_INVALID_UUIDS; ++i) {
                // Attempt to remove non-existing UUID, and verify that the
                // database is not affected.

                const int INVALID_LINE = INVALID_UUIDS[i].d_lineNum;
                const int INVALID_UUID = INVALID_UUIDS[i].d_uuid;

                int result = mX.removeByUUID(INVALID_UUID);
                LOOP2_ASSERT(INVALID_LINE, result,
                             baedb_UserDb::NOT_FOUND == result);
                LOOP3_ASSERT(INVALID_LINE, NUM_DATA,   X.numRecords(),
                                           NUM_DATA == X.numRecords());

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int    LINE        = DATA[j].d_lineNum;
                    const string USER_NAME   = DATA[j].d_userName;
                    const int    UUID        = DATA[j].d_uuid;
                    const int    USER_NUMBER = DATA[j].d_userNumber;
                    const int    FIRM_NUMBER = DATA[j].d_firmNumber;
                    const string LOGIN       = DATA[j].d_login;
                    const string PASSWORD    = DATA[j].d_password;

                    LOOP3_ASSERT(
                               LINE,
                               USER_NAME,
                               X.view().theList(j).theString(FIELD_USER_NAME),
                               USER_NAME ==
                               X.view().theList(j).theString(FIELD_USER_NAME));

                    LOOP3_ASSERT(LINE,
                                 UUID,
                                 X.view().theList(j).theInt(FIELD_UUID),
                                 UUID ==
                                 X.view().theList(j).theInt(FIELD_UUID));

                    LOOP3_ASSERT(
                                LINE,
                                USER_NUMBER,
                                X.view().theList(j).theInt(FIELD_USER_NUMBER),
                                USER_NUMBER ==
                                X.view().theList(j).theInt(FIELD_USER_NUMBER));

                    LOOP3_ASSERT(
                                LINE,
                                FIRM_NUMBER,
                                X.view().theList(j).theInt(FIELD_FIRM_NUMBER),
                                FIRM_NUMBER ==
                                X.view().theList(j).theInt(FIELD_FIRM_NUMBER));

                    LOOP3_ASSERT(LINE,
                                 LOGIN,
                                 X.view().theList(j).theString(FIELD_LOGIN),
                                 LOGIN ==
                                 X.view().theList(j).theString(FIELD_LOGIN));

                    LOOP3_ASSERT(
                                LINE,
                                PASSWORD,
                                X.view().theList(j).theString(FIELD_PASSWORD),
                                PASSWORD ==
                                X.view().theList(j).theString(FIELD_PASSWORD));
                }
            }
        }

        if (verbose) cout << "\nEnd of 'removeByUUID' Method Test." << endl;
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'removeAll' METHOD
        //   This will thoroughly test the 'removeAll' method.
        //
        // Concerns:
        //   We want to make sure that the 'removeAll' method works correctly,
        //   with any number of records in the database.  The database should
        //   be empty after calling 'removeAll'.
        //
        // Plan:
        //   Create objects with increasing number of records.  For each
        //   object, call 'removeAll' and make sure the resulting object
        //   contains no records.
        //
        // Testing:
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeAll' Method"
                          << "\n==========================" << endl;

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_userName;         // user name
            int         d_uuid;             // uuid
            int         d_userNumber;       // user number
            int         d_firmNumber;       // firm number
            const char *d_login;            // login ID
            const char *d_password;         // password
        } DATA[] = {
            {
                // minimum bounds
                L_,                         // source line number
                "",                         // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "l",                        // login ID
                "",                         // password
            },
            {
                // maximum bounds
                L_,                         // source line number
                "",                         // user name
                INT_MAX,                    // uuid
                INT_MAX,                    // user number
                INT_MAX,                    // firm number
                "login1",                   // login ID
                "",                         // password
            },
            // Note, we cannot have orthogonal cases for uuid, user name
            // and login ID because these fields must *always* be unique.
            {
                // orthogonal: base case
                L_,                         // source line number
                "user name",                // user name
                11,                         // uuid
                11,                         // user number
                11,                         // firm number
                "login11",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: user name
                L_,                         // source line number
                "new user name",            // user name
                22,                         // uuid
                22,                         // user number
                1,                          // firm number
                "login22",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: firm number
                L_,                         // source line number
                "user name",                // user name
                33,                         // uuid
                33,                         // user number
                9999,                       // firm number
                "login33",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: password
                L_,                         // source line number
                "user name",                // user name
                44,                         // uuid
                44,                         // user number
                1,                          // firm number
                "login44",                  // login ID
                "new password",             // password
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting removeAll()." << endl;
        {
            // Test 'removeAll' using a database ranging from 0 to 'NUM_DATA'-1
            // records.  Verify that the resulting database is empty.

            for (int maxSize = 0; maxSize <= NUM_DATA; ++maxSize) {
                // Create a database with the specified 'maxSize' records.
                // Then call 'reset' and make sure the resulting database is
                // empty.

                Obj mX; const Obj& X = mX;

                // add 'maxSize' records
                for (int i = 0; i < maxSize; ++i) {
                    const int   LINE        = DATA[i].d_lineNum;
                    const char *USER_NAME   = DATA[i].d_userName;
                    const int   UUID        = DATA[i].d_uuid;
                    const int   USER_NUMBER = DATA[i].d_userNumber;
                    const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                    const char *LOGIN       = DATA[i].d_login;
                    const char *PASSWORD    = DATA[i].d_password;

                    int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                              UUID, USER_NUMBER, FIRM_NUMBER,
                                              LOGIN, strlen(LOGIN),
                                              PASSWORD, strlen(PASSWORD));
                    LOOP2_ASSERT(LINE, result,
                                 baedb_UserDb::SUCCESS == result);
                }

                LOOP2_ASSERT(X.numRecords(),   maxSize,
                             X.numRecords() == maxSize);

                // call 'removeAll'
                mX.removeAll();

                LOOP_ASSERT(X.numRecords(), 0 == X.numRecords());
            }
        }

        if (verbose) cout << "\nEnd of 'removeAll' Method Test." << endl;
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING SECONDARY CONSTRUCTOR
        //   This will test the secondary constructor.
        //
        // Concerns:
        //   The secondary constructor is implemented almost entirely using the
        //   'addRecords' method, which has already been thoroughly tested.  We
        //   only need to make sure that the 'inputRecords' argument is passed
        //   correctly to the 'addRecords' method.
        //
        // Plan:
        //   Generate a const char* representation of DATA that can be
        //   interpreted by the 'addRecords' method.  Create an object using
        //   the default constructor, then call 'addRecords' using the input
        //   data string.  Then construct a new object directly from the input
        //   data string and make sure that the two objects are identical.
        //
        // Testing:
        //   baedb_TestUserDb(const char      *inputRecords,
        //                    bslma_Allocator *basicAllocator = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Secondary Constructor"
                          << "\n=============================" << endl;

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_userName;         // user name
            int         d_uuid;             // uuid
            int         d_userNumber;       // user number
            int         d_firmNumber;       // firm number
            const char *d_login;            // login ID
            const char *d_password;         // password
        } DATA[] = {
            {
                // minimum bounds
                L_,                         // source line number
                "",                         // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "l",                        // login ID
                "",                         // password
            },
            {
                // maximum bounds
                L_,                         // source line number
                "",                         // user name
                INT_MAX,                    // uuid
                INT_MAX,                    // user number
                INT_MAX,                    // firm number
                "login1",                   // login ID
                "",                         // password
            },
            // Note, we cannot have orthogonal cases for uuid, user name
            // and login ID because these fields must *always* be unique.
            {
                // orthogonal: base case
                L_,                         // source line number
                "user name",                // user name
                11,                         // uuid
                11,                         // user number
                11,                         // firm number
                "login11",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: user name
                L_,                         // source line number
                "new user name",            // user name
                22,                         // uuid
                22,                         // user number
                1,                          // firm number
                "login22",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: firm number
                L_,                         // source line number
                "user name",                // user name
                33,                         // uuid
                33,                         // user number
                9999,                       // firm number
                "login33",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: password
                L_,                         // source line number
                "user name",                // user name
                44,                         // uuid
                44,                         // user number
                1,                          // firm number
                "login44",                  // login ID
                "new password",             // password
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting baedb_TestUserDb(const char*)."
                          << endl;
        {
            // generate a const char* from DATA
            string data;
            int i;

            data = "{";
            for (i = 0; i < NUM_DATA; ++i) {
                const string USER_NAME   = DATA[i].d_userName;
                const int    UUID        = DATA[i].d_uuid;
                const int    USER_NUMBER = DATA[i].d_userNumber;
                const int    FIRM_NUMBER = DATA[i].d_firmNumber;
                const string LOGIN       = DATA[i].d_login;
                const string PASSWORD    = DATA[i].d_password;

                data += "LIST { ";
                data += "STRING \"" + USER_NAME + "\",";
                {
                    vector<char> buffer;
                    bdepu_NTypesParser::generateInt(&buffer, UUID);
                    string str;
                    for (int i = 0; i < buffer.size()-1; ++i) {
                        str += buffer[i];
                    }
                    data += "INT " + str + ",";
                }
                {
                    vector<char> buffer;
                    bdepu_NTypesParser::generateInt(&buffer, USER_NUMBER);
                    string str;
                    for (int i = 0; i < buffer.size()-1; ++i) {
                        str += buffer[i];
                    }
                    data += "INT " + str + ",";
                }
                {
                    vector<char> buffer;
                    bdepu_NTypesParser::generateInt(&buffer, FIRM_NUMBER);
                    string str;
                    for (int i = 0; i < buffer.size()-1; ++i) {
                        str += buffer[i];
                    }
                    data += "INT " + str + ",";
                }
                data += "STRING \"" + LOGIN + "\",";
                data += "STRING \"" + PASSWORD + "\"}";
                if (i < NUM_DATA-1) {
                    data += ",";
                }
            }
            data += "}";

            const char *INPUT_DATA = data.data();

            Obj mX; const Obj& X = mX;
            mX.addRecords(INPUT_DATA);

            const Obj Y(INPUT_DATA);

            LOOP2_ASSERT(X, Y, X == Y);
        }

        if (verbose) cout << "\nEnd of Secondary Constructor Test." << endl;
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'addRecords' METHOD
        //   This will thoroughly test the 'addRecords' method.
        //
        // Concerns:
        //   We want to make sure that the 'addRecords' method works correctly.
        //   In particular, we want to check the following cases:
        //       1. The method succeeds when adding valid data.
        //       2. The method succeeds when adding valid data through multiple
        //          calls, and that the resulting object still contains all the
        //          records.
        //       3. The method fails when provided with invalid data.
        //       4. The method succeeds when some of the (otherwise valid)
        //          records in the input data are duplicates of existing data,
        //          but correctly discards only the duplicated records.
        //
        // Plan:
        //   Run the 'addRecords' method in each of these four scenarios and
        //   make sure the return value is as expected.  Also, make sure that
        //   the data contained in the resulting database is as expected.
        //
        // Testing:
        //   int addRecords(const char *inputData);
        // -------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'addRecords' Method"
                          << "\n===========================" << endl;

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_userName;         // user name
            int         d_uuid;             // uuid
            int         d_userNumber;       // user number
            int         d_firmNumber;       // firm number
            const char *d_login;            // login ID
            const char *d_password;         // password
        } DATA[] = {
            {
                // minimum bounds
                L_,                         // source line number
                "",                         // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "l",                        // login ID
                "",                         // password
            },
            {
                // maximum bounds
                L_,                         // source line number
                "",                         // user name
                INT_MAX,                    // uuid
                INT_MAX,                    // user number
                INT_MAX,                    // firm number
                "login1",                   // login ID
                "",                         // password
            },
            // Note, we cannot have orthogonal cases for uuid, user name
            // and login ID because these fields must *always* be unique.
            {
                // orthogonal: base case
                L_,                         // source line number
                "user name",                // user name
                11,                         // uuid
                11,                         // user number
                11,                         // firm number
                "login11",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: user name
                L_,                         // source line number
                "new user name",            // user name
                22,                         // uuid
                22,                         // user number
                1,                          // firm number
                "login22",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: firm number
                L_,                         // source line number
                "user name",                // user name
                33,                         // uuid
                33,                         // user number
                9999,                       // firm number
                "login33",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: password
                L_,                         // source line number
                "user name",                // user name
                44,                         // uuid
                44,                         // user number
                1,                          // firm number
                "login44",                  // login ID
                "new password",             // password
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\n1. Testing addRecords(const char*)." << endl;

        if (veryVerbose) cout << "\ta. Testing single call." << endl;
        {
            // generate a const char* from DATA
            string data;
            int i;

            data = "{";
            for (i = 0; i < NUM_DATA; ++i) {
                const int    LINE        = DATA[i].d_lineNum;
                const string USER_NAME   = DATA[i].d_userName;
                const int    UUID        = DATA[i].d_uuid;
                const int    USER_NUMBER = DATA[i].d_userNumber;
                const int    FIRM_NUMBER = DATA[i].d_firmNumber;
                const string LOGIN       = DATA[i].d_login;
                const string PASSWORD    = DATA[i].d_password;

                data += "LIST { ";
                data += "STRING \"" + USER_NAME + "\",";
                {
                    vector<char> buffer;
                    bdepu_NTypesParser::generateInt(&buffer, UUID);
                    string str;
                    for (int i = 0; i < buffer.size()-1; ++i) {
                        str += buffer[i];
                    }
                    data += "INT " + str + ",";
                }
                {
                    vector<char> buffer;
                    bdepu_NTypesParser::generateInt(&buffer, USER_NUMBER);
                    string str;
                    for (int i = 0; i < buffer.size()-1; ++i) {
                        str += buffer[i];
                    }
                    data += "INT " + str + ",";
                }
                {
                    vector<char> buffer;
                    bdepu_NTypesParser::generateInt(&buffer, FIRM_NUMBER);
                    string str;
                    for (int i = 0; i < buffer.size()-1; ++i) {
                        str += buffer[i];
                    }
                    data += "INT " + str + ",";
                }
                data += "STRING \"" + LOGIN + "\",";
                data += "STRING \"" + PASSWORD + "\"}";
                if (i < NUM_DATA-1) {
                    data += ",";
                }
            }
            data += "}";

            Obj mX; const Obj& X = mX;

            int result = mX.addRecords(data.data());
            LOOP2_ASSERT(NUM_DATA, result, NUM_DATA == result);

            // verify that the data was added properly
            for (i = 0; i < NUM_DATA; ++i) {
                const int       LINE        = DATA[i].d_lineNum;
                const string    USER_NAME   = DATA[i].d_userName;
                const int       UUID        = DATA[i].d_uuid;
                const int       USER_NUMBER = DATA[i].d_userNumber;
                const int       FIRM_NUMBER = DATA[i].d_firmNumber;
                const string    LOGIN       = DATA[i].d_login;
                const string    PASSWORD    = DATA[i].d_password;

                LOOP3_ASSERT(LINE,
                             USER_NAME,
                             X.view().theList(i).theString(FIELD_USER_NAME),
                             USER_NAME ==
                             X.view().theList(i).theString(FIELD_USER_NAME));

                LOOP3_ASSERT(LINE,
                             UUID,   X.view().theList(i).theInt(FIELD_UUID),
                             UUID == X.view().theList(i).theInt(FIELD_UUID));

                LOOP3_ASSERT(LINE,
                             USER_NUMBER,
                             X.view().theList(i).theInt(FIELD_USER_NUMBER),
                             USER_NUMBER ==
                             X.view().theList(i).theInt(FIELD_USER_NUMBER));

                LOOP3_ASSERT(LINE,
                             FIRM_NUMBER,
                             X.view().theList(i).theInt(FIELD_FIRM_NUMBER),
                             FIRM_NUMBER ==
                             X.view().theList(i).theInt(FIELD_FIRM_NUMBER));

                LOOP3_ASSERT(LINE,
                             LOGIN,
                             X.view().theList(i).theString(FIELD_LOGIN),
                             LOGIN ==
                             X.view().theList(i).theString(FIELD_LOGIN));

                LOOP3_ASSERT(LINE,
                             PASSWORD,
                             X.view().theList(i).theString(FIELD_PASSWORD),
                             PASSWORD ==
                             X.view().theList(i).theString(FIELD_PASSWORD));
            }
        }

        if (veryVerbose) cout << "\tb. Testing multiple calls." << endl;
        {
            // Divide 'DATA' into three divisions and generate a const char*
            // from each division.  Using a single 'baedb_TestUserDb' object,
            // exercise the 'addRecords' method using the const char* from each
            // division.  Then make sure that the 'baedb_TestUserDb' contains
            // all the records from 'DATA' in the correct order.

            const int firstDiv  = NUM_DATA/3;
            const int secondDiv = (NUM_DATA*2)/3;

            Obj mX; const Obj& X = mX;

            {
                // add first division
                string data = "{";
                for (int i = 0; i < firstDiv; ++i) {
                    const string    USER_NAME   = DATA[i].d_userName;
                    const int       UUID        = DATA[i].d_uuid;
                    const int       USER_NUMBER = DATA[i].d_userNumber;
                    const int       FIRM_NUMBER = DATA[i].d_firmNumber;
                    const string    LOGIN       = DATA[i].d_login;
                    const string    PASSWORD    = DATA[i].d_password;

                    data += "LIST { ";
                    data += "STRING \"" + USER_NAME + "\",";
                    {
                        vector<char> buffer;
                        bdepu_NTypesParser::generateInt(&buffer, UUID);
                        string str;
                        for (int i = 0; i < buffer.size()-1; ++i) {
                            str += buffer[i];
                        }
                        data += "INT " + str + ",";
                    }
                    {
                        vector<char> buffer;
                        bdepu_NTypesParser::generateInt(&buffer, USER_NUMBER);
                        string str;
                        for (int i = 0; i < buffer.size()-1; ++i) {
                            str += buffer[i];
                        }
                        data += "INT " + str + ",";
                    }
                    {
                        vector<char> buffer;
                        bdepu_NTypesParser::generateInt(&buffer, FIRM_NUMBER);
                        string str;
                        for (int i = 0; i < buffer.size()-1; ++i) {
                            str += buffer[i];
                        }
                        data += "INT " + str + ",";
                    }
                    data += "STRING \"" + LOGIN + "\",";
                    data += "STRING \"" + PASSWORD + "\"}";
                    if (i < firstDiv-1) {
                        data += ",";
                    }
                }
                data += "}";
                int result = mX.addRecords(data.data());
                LOOP2_ASSERT(firstDiv, result, firstDiv == result);
            }

            {
                // add second division
                string data = "{";
                for (int i = firstDiv; i < secondDiv; ++i) {
                    const string    USER_NAME   = DATA[i].d_userName;
                    const int       UUID        = DATA[i].d_uuid;
                    const int       USER_NUMBER = DATA[i].d_userNumber;
                    const int       FIRM_NUMBER = DATA[i].d_firmNumber;
                    const string    LOGIN       = DATA[i].d_login;
                    const string    PASSWORD    = DATA[i].d_password;

                    data += "LIST { ";
                    data += "STRING \"" + USER_NAME + "\",";
                    {
                        vector<char> buffer;
                        bdepu_NTypesParser::generateInt(&buffer, UUID);
                        string str;
                        for (int i = 0; i < buffer.size()-1; ++i) {
                            str += buffer[i];
                        }
                        data += "INT " + str + ",";
                    }
                    {
                        vector<char> buffer;
                        bdepu_NTypesParser::generateInt(&buffer, USER_NUMBER);
                        string str;
                        for (int i = 0; i < buffer.size()-1; ++i) {
                            str += buffer[i];
                        }
                        data += "INT " + str + ",";
                    }
                    {
                        vector<char> buffer;
                        bdepu_NTypesParser::generateInt(&buffer, FIRM_NUMBER);
                        string str;
                        for (int i = 0; i < buffer.size()-1; ++i) {
                            str += buffer[i];
                        }
                        data += "INT " + str + ",";
                    }
                    data += "STRING \"" + LOGIN + "\",";
                    data += "STRING \"" + PASSWORD + "\"}";
                    if (i < secondDiv-1) {
                        data += ",";
                    }
                }
                data += "}";
                int result = mX.addRecords(data.data());
                LOOP2_ASSERT(firstDiv, result, firstDiv == result);
            }

            {
                // add third division
                string data = "{";
                for (int i = secondDiv; i < NUM_DATA; ++i) {
                    const string    USER_NAME   = DATA[i].d_userName;
                    const int       UUID        = DATA[i].d_uuid;
                    const int       USER_NUMBER = DATA[i].d_userNumber;
                    const int       FIRM_NUMBER = DATA[i].d_firmNumber;
                    const string    LOGIN       = DATA[i].d_login;
                    const string    PASSWORD    = DATA[i].d_password;

                    data += "LIST { ";
                    data += "STRING \"" + USER_NAME + "\",";
                    {
                        vector<char> buffer;
                        bdepu_NTypesParser::generateInt(&buffer, UUID);
                        string str;
                        for (int i = 0; i < buffer.size()-1; ++i) {
                            str += buffer[i];
                        }
                        data += "INT " + str + ",";
                    }
                    {
                        vector<char> buffer;
                        bdepu_NTypesParser::generateInt(&buffer, USER_NUMBER);
                        string str;
                        for (int i = 0; i < buffer.size()-1; ++i) {
                            str += buffer[i];
                        }
                        data += "INT " + str + ",";
                    }
                    {
                        vector<char> buffer;
                        bdepu_NTypesParser::generateInt(&buffer, FIRM_NUMBER);
                        string str;
                        for (int i = 0; i < buffer.size()-1; ++i) {
                            str += buffer[i];
                        }
                        data += "INT " + str + ",";
                    }
                    data += "STRING \"" + LOGIN + "\",";
                    data += "STRING \"" + PASSWORD + "\"}";
                    if (i < NUM_DATA-1) {
                        data += ",";
                    }
                }
                data += "}";
                int result = mX.addRecords(data.data());
                LOOP2_ASSERT(firstDiv, result, firstDiv == result);
            }

            // Verify that the data was added correctly, and in the correct
            // order (one division after another).

            for (int i = 0; i < NUM_DATA; ++i) {
                const int       LINE        = DATA[i].d_lineNum;
                const string    USER_NAME   = DATA[i].d_userName;
                const int       UUID        = DATA[i].d_uuid;
                const int       USER_NUMBER = DATA[i].d_userNumber;
                const int       FIRM_NUMBER = DATA[i].d_firmNumber;
                const string    LOGIN       = DATA[i].d_login;
                const string    PASSWORD    = DATA[i].d_password;

                LOOP3_ASSERT(LINE,
                             USER_NAME,
                             X.view().theList(i).theString(FIELD_USER_NAME),
                             USER_NAME ==
                             X.view().theList(i).theString(FIELD_USER_NAME));

                LOOP3_ASSERT(LINE,
                             UUID,   X.view().theList(i).theInt(FIELD_UUID),
                             UUID == X.view().theList(i).theInt(FIELD_UUID));

                LOOP3_ASSERT(LINE,
                             USER_NUMBER,
                             X.view().theList(i).theInt(FIELD_USER_NUMBER),
                             USER_NUMBER ==
                             X.view().theList(i).theInt(FIELD_USER_NUMBER));

                LOOP3_ASSERT(LINE,
                             FIRM_NUMBER,
                             X.view().theList(i).theInt(FIELD_FIRM_NUMBER),
                             FIRM_NUMBER ==
                             X.view().theList(i).theInt(FIELD_FIRM_NUMBER));

                LOOP3_ASSERT(LINE,
                             LOGIN,
                             X.view().theList(i).theString(FIELD_LOGIN),
                             LOGIN ==
                             X.view().theList(i).theString(FIELD_LOGIN));

                LOOP3_ASSERT(LINE,
                             PASSWORD,
                             X.view().theList(i).theString(FIELD_PASSWORD),
                             PASSWORD ==
                             X.view().theList(i).theString(FIELD_PASSWORD));
            }
        }

        if (veryVerbose) cout << "\tc. Testing with invalid data." << endl;
        {
            // Call the 'addRecords' method using invalid input data.  All the
            // possible return error codes are exercised.

            static const struct {
                int         d_lineNum;         // source line number
                const char* d_inputData;       // source string
                int         d_expectedReturn;  // expected return value
            } INVALID_DATA[] = {
                {
                    L_,
                    "{LIST { STRING \"user\", INT 232, INT 34, INT 928, "
                    "STRING \"login\", STRING \"password\" } }",
                    1 // no error, control record
                },
                {
                    L_,
                    "STRING \"user\", INT 232, INT 34, INT 928, "
                    "STRING \"login\", STRING \"password\"",
                    baedb_TestUserDb::PARSING_ERROR
                },
                {
                    L_,
                    "{ STRING \"user\", INT 232, INT 34, INT 928, "
                    "STRING \"login\", STRING \"password\" }",
                    0  // non-unique key
                },
                {
                    L_,
                    "{LIST { STRING \"user\", INT 232, INT 34, INT 928, "
                    "STRING \"login\", STRING \"password\", INT 37 } }",
                    0  // too many fields
                },
                {
                    L_,
                    "{LIST { STRING \"user\", INT 232, INT 34, INT 928, "
                    "STRING \"login\", INT 37 } }",
                    0  // 'INT' password
                },
                {
                    L_,
                    "{LIST { STRING \"user\", INT 0, INT 34, INT 928, "
                    "STRING \"login\", STRING \"password\" } }",
                    0  // uuid == 0
                },
                {
                    L_,
                    "{LIST { STRING \"user\", INT 232, INT 0, INT 928, "
                    "STRING \"login\", STRING \"password\" } }",
                    0  // user number == 0
                },
                {
                    L_,
                    "{LIST { STRING \"user\", INT 232, INT 34, INT 928, "
                    "STRING \"\", STRING \"password\" } }",
                    0  // 0-length login id
                },
            };
            const int NUM_INVALID_DATA
                        = sizeof INVALID_DATA / sizeof *INVALID_DATA;

            for (int i = 0; i < NUM_INVALID_DATA; ++i) {
                const int   LINE            = INVALID_DATA[i].d_lineNum;
                const char *INPUT_DATA      = INVALID_DATA[i].d_inputData;
                const int   EXPECTED_RETURN = INVALID_DATA[i].d_expectedReturn;

                Obj mX; const Obj& X = mX;
                int result = mX.addRecords(INPUT_DATA);
                LOOP3_ASSERT(LINE, EXPECTED_RETURN,   result,
                                   EXPECTED_RETURN == result);
                if (EXPECTED_RETURN < 0) {
                    LOOP2_ASSERT(LINE, X.view().length(),
                                  0 == X.view().length());
                }
            }
        }

        if (veryVerbose) cout << "\td. Testing with duplicate data." << endl;
        {
            // Call the 'addRecords' method using valid input data, but some
            // records have duplicated unique fields.  Make sure that the
            // duplicated record is not added to the database and the
            // appropriate value is returned.

            static const struct {
                int         d_lineNum;          // source line number
                const char *d_inputData;        // source string
                int         d_insertCount;      // expected insert count
            } DUPLICATE_DATA[] = {
                {
                    L_, // source line number
                    "{LIST { STRING \"user\", INT 1, INT 10, INT 928, "
                    "STRING \"login1\", STRING \"password\" } }",
                    1  // one record should be inserted
                },
                {
                    L_, // source line number
                    "{LIST { STRING \"user\", INT 1, INT 20, INT 928, "
                    "STRING \"login2\", STRING \"password\" } }",
                    0  // zero records should be inserted
                },
                {
                    L_, // source line number
                    "{LIST { STRING \"user\", INT 2, INT 10, INT 928, "
                    "STRING \"login2\", STRING \"password\" } }",
                    0  // zero records should be inserted
                },
                {
                    L_, // source line number
                    "{LIST { STRING \"user\", INT 2, INT 20, INT 928, "
                    "STRING \"login1\", STRING \"password\" } }",
                    0  // zero records should be inserted
                },
                // also test with multiple records per input string
                {
                    L_, // source line number
                    "{LIST { STRING \"user\", INT 2, INT 20, INT 928, "
                    "STRING \"login2\", STRING \"password\" }, " // OK
                    "LIST { STRING \"user\", INT 1, INT 30, INT 928, "
                    "STRING \"login3\", STRING \"password\" }, " // NOT OK
                    "LIST { STRING \"user\", INT 2, INT 30, INT 928, "
                    "STRING \"login3\", STRING \"password\" }, " // NOT OK
                    "LIST { STRING \"user\", INT 3, INT 10, INT 928, "
                    "STRING \"login3\", STRING \"password\" }, " // NOT OK
                    "LIST { STRING \"user\", INT 3, INT 20, INT 928, "
                    "STRING \"login3\", STRING \"password\" }, " // NOT OK
                    "LIST { STRING \"user\", INT 3, INT 30, INT 928, "
                    "STRING \"login1\", STRING \"password\" }, " // NOT OK
                    "LIST { STRING \"user\", INT 3, INT 30, INT 928, "
                    "STRING \"login2\", STRING \"password\" }, " // NOT OK
                    "LIST { STRING \"user\", INT 3, INT 30, INT 928, "
                    "STRING \"login3\", STRING \"password\" }}", // OK
                    2  // two records should be inserted
                },
            };
            const int NUM_DUPLICATE_DATA = sizeof DUPLICATE_DATA
                                         / sizeof *DUPLICATE_DATA;

            Obj mX; const Obj& X = mX;

            for (int i = 0; i < NUM_DUPLICATE_DATA; ++i) {
                const int   LINE          = DUPLICATE_DATA[i].d_lineNum;
                const char *INPUT_DATA    = DUPLICATE_DATA[i].d_inputData;
                const int   INSERT_COUNT  = DUPLICATE_DATA[i].d_insertCount;

                const Obj PREV = X;

                int result = mX.addRecords(INPUT_DATA);

                LOOP3_ASSERT(LINE, INSERT_COUNT, result,
                                   INSERT_COUNT == result);
            }
        }

        if (verbose) cout << "\nEnd of 'addRecords' Method Test." << endl;
      } break;
      case 10: {
#if 0
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY
        //   The 'bdex' streaming concerns for this component are absolutely
        //   standard.
        //
        // Concerns:
        //   We need to probe the member functions 'bdexStreamIn' and
        //   'bdexStreamOut' in the manner of a "breathing test" to verify
        //   basic functionality, then we need to thoroughly test that
        //   functionality using the overloaded '<<' and '>>' free operators,
        //   which forward appropriate calls to the member functions.  We also
        //   want to step through the sequence of possible stream states
        //   (valid, empty, invalid, incomplete, and corrupted), appropriately
        //   selecting data sets as described below.  In all cases, we need to
        //   confirm exception neutrality using the specially instrumented
        //   'bdex_TestInStream' and a pair of standard macros,
        //   'BEGIN_BDEX_EXCEPTION_TEST' and 'END_BDEX_EXCEPTION_TEST', which
        //   configure the 'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   PRELIMINARY MEMBER FUNCTION TEST
        //     First perform a trivial direct test of the 'bdexStreamOut' and
        //     'bdexStreamIn' methods.  (The remaining tests will use the
        //     streaming << and >> operators.)
        //
        //   VALID STREAMS
        //     For the set S of globally-defined test values, use all
        //     combinations (u, v) in the cross product S X S, stream the
        //     value of v into (a temporary copy of) u and assert u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each u in S, create a copy and attempt to stream into it
        //     from an empty stream, and then an invalid stream.  Verify after
        //     each attempt that the object is unchanged and that the stream is
        //     invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct an input stream and attempt to read into objects
        //     initialized with distinct values.  Verify values of objects that
        //     are either successfully modified or left entirely unmodified,
        //     and that the stream became invalid immediately after the first
        //     incomplete read.  Finally ensure that each object streamed into
        //     is in some valid state by assigning it a distinct new value and
        //     testing for equality.
        //
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate an instance of
        //     a typical valid (control) stream and verify that it can be
        //     streamed in successfully.  Check for the following corrupted
        //     data:
        //         1. Bad version number.
        //         2. Non-'bdem_List' records.
        //         3. Records with the wrong number of fields.
        //         4. Fields of the wrong type.
        //         5. Fields that are out of bounds.
        //         6. Databases with duplicate records.
        //     After each test, verify that the object is in some valid state
        //     after streaming, and that the input stream has gone invalid.
        //
        // Testing:
        //   static int maxSupportedBdexVersion();
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        //   bdex_InStream& operator>>(bdex_InStream&, baedb_TestUserDb&);
        //   bdex_OutStream& operator<<(bdex_OutStream&,
        //                              const baedb_TestUserDb&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'bdex' Streaming Functionality"
                          << "\n======================================"
                          << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // raw data (to be fed into generator function)
        static const GeneratorData DATA[] = {
            {
                // minimum bounds
                L_,                         // source line number
                "",                         // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "l",                        // login ID
                "",                         // password
            },
            {
                // maximum bounds
                L_,                         // source line number
                "",                         // user name
                INT_MAX,                    // uuid
                INT_MAX,                    // user number
                INT_MAX,                    // firm number
                "login1",                   // login ID
                "",                         // password
            },
            {
                // orthogonal: base case
                L_,                         // source line number
                "user name",                // user name
                11,                         // uuid
                11,                         // user number
                11,                         // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: user name
                L_,                         // source line number
                "new user name",            // user name
                11,                         // uuid
                11,                         // user number
                11,                         // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: uuid
                L_,                         // source line number
                "user name",                // user name
                9999,                       // uuid
                11,                         // user number
                11,                         // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: user number
                L_,                         // source line number
                "user name",                // user name
                11,                         // uuid
                9999,                       // user number
                11,                         // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: firm number
                L_,                         // source line number
                "user name",                // user name
                11,                         // uuid
                11,                         // user number
                9999,                       // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: login ID
                L_,                         // source line number
                "user name",                // user name
                11,                         // uuid
                11,                         // user number
                11,                         // firm number
                "new login",                // login ID
                "password",                 // password
            },
            {
                // orthogonal: password
                L_,                         // source line number
                "user name",                // user name
                11,                         // uuid
                11,                         // user number
                11,                         // firm number
                "login",                    // login ID
                "new password",             // password
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int spec[2];
        int indexCounter = -1; // make the first database empty

        spec[1] = -1;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // scalar and array object values for various stream tests
        spec[0] = indexCounter++;
        const Obj VA(g(spec, DATA));
        spec[0] = indexCounter++;
        const Obj VB(g(spec, DATA));
        spec[0] = indexCounter++;
        const Obj VC(g(spec, DATA));
        spec[0] = indexCounter++;
        const Obj VD(g(spec, DATA));
        spec[0] = indexCounter++;
        const Obj VE(g(spec, DATA));
        spec[0] = indexCounter++;
        const Obj VF(g(spec, DATA));
        spec[0] = indexCounter++;
        const Obj VG(g(spec, DATA));
        spec[0] = indexCounter++;
        const Obj VH(g(spec, DATA));
        spec[0] = indexCounter++;
        const Obj VI(g(spec, DATA));

        const int NUM_VALUES = NUM_DATA;
        const Obj VALUES[NUM_VALUES] = { VA, VB, VC, VD, VE, VF, VG, VH, VI };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\nTesting 'bdexStreamOut' and (valid) "
                          << "'bdexStreamIn' functionality." << endl;
        {
            // testing 'bdexStreamOut' and 'bdexStreamIn' directly
            const int version = 2;
            const Obj X(VC);
            Out out;
            out.putVersion(version);
            X.bdexStreamOut(out, version);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In in(OD, LOD);
            ASSERT(in);                         ASSERT(!in.isEmpty());

            Obj t(VA);                          ASSERT(X != t);
            int versionRead;
            in.getVersion(versionRead);         ASSERT(2 == versionRead);
            t.bdexStreamIn(in, versionRead);    ASSERT(X == t);
            ASSERT(in);                         ASSERT(in.isEmpty());
        }

        if (verbose) cout << "\nThorough test of stream operators "
                          << "('<<' and '>>')." << endl;
        {
            // testing '<<' and '>>' operators thoroughly
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);
                Out out;
                out << X;

                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);  In &testInStream = in;
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj t(VALUES[j]);
                    BEGIN_BDEX_EXCEPTION_TEST {
                      in.reset();
                      LOOP2_ASSERT(i, j, X == t == (i == j));
                      in >> t;
                    } END_BDEX_EXCEPTION_TEST
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            // testing empty and invalid streams
            Out out;
            const char *const OD  = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);  In& testInStream = in;
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input
                // stream leaves the stream invalid and the target object
                // unchanged.

                const Obj X(VALUES[i]);  Obj t(X);  LOOP_ASSERT(i, X == t);
                BEGIN_BDEX_EXCEPTION_TEST {
                  in.reset();
                  in >> t;
                  LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, X == t);
                  in >> t;
                  LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, X == t);
                } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tOn incomplete (but otherwise valid) data."
                          << endl;
        {
            // testing incomplete (but otherwise valid) data
            const Obj W1 = VA, X1 = VB, Y1 = VC;
            const Obj W2 = VB, X2 = VC, Y2 = VD;
            const Obj W3 = VC, X3 = VD, Y3 = VE;

            Out out;
            out << X1; const int LOD1 = out.length();
            out << X2; const int LOD2 = out.length();
            out << X3; const int LOD  = out.length();
            const char *const    OD   = out.data();

            for (int i = 0; i < LOD; ++i) {
                In in(OD, i);  In& testInStream = in;
                BEGIN_BDEX_EXCEPTION_TEST {
                  in.reset();
                  LOOP_ASSERT(i, in);
                  LOOP_ASSERT(i, !i == in.isEmpty());
                  Obj t1(W1), t2(W2), t3(W3);

                  if (i < LOD1) {
                      in >> t1;
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W1 == t1);
                      in >> t2;
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W2 == t2);
                      in >> t3;
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                  } else if (i < LOD2) {
                      in >> t1;
                      LOOP_ASSERT(i, in);     LOOP_ASSERT(i, X1 == t1);
                      in >> t2;
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W2 == t2);
                      in >> t3;
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                  } else {
                      in >> t1;
                      LOOP_ASSERT(i, in);     LOOP_ASSERT(i, X1 == t1);
                      in >> t2;
                      LOOP_ASSERT(i, in);     LOOP_ASSERT(i, X2 == t2);
                      in >> t3;
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                  }

                              LOOP_ASSERT(i, Y1 != t1);
                  t1 = Y1;    LOOP_ASSERT(i, Y1 == t1);

                              LOOP_ASSERT(i, Y2 != t2);
                  t2 = Y2;    LOOP_ASSERT(i, Y2 == t2);

                              LOOP_ASSERT(i, Y3 != t3);
                  t3 = Y3;    LOOP_ASSERT(i, Y3 == t3);
                } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tOn corrupted data." << endl;

        int spec5[5];

        const Obj W;                  // default value
        spec5[0] = 0; spec5[1] = -1;
        const Obj X(g(spec5, DATA));  // original (control) value
        spec5[0] = 0; spec5[1] = 1; spec5[2] = 2; spec5[3] = -1;
        const Obj Y(g(spec5, DATA));  // new (streamed-out) value

        // internal representation of Obj Y
        const bdem_List& SERIAL_Y = Y.view();

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            // good version (for control)
            const char version = 1;

            Out out;
            out.putVersion(version);
            out << SERIAL_Y;

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in >> t;        ASSERT(in);
                            ASSERT(W != t); ASSERT(X != t); ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version number." << endl;
        {
            const char version = 0;  // too small ('version' must be >= 1)

            Out out;
            out.putVersion(version);
            out << SERIAL_Y; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            const char version = 2;  // too large (current version is 1)

            Out out;
            out.putVersion(version);
            out << SERIAL_Y; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tNon-'bdem_List' records." << endl;
        {
            if (veryVerbose) cout << "\t\t\tInvalid record at beginning of db."
                                  << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            modifiedY.insertInt(0, 123);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            if (veryVerbose) cout << "\t\t\tInvalid record in middle of db."
                                  << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            modifiedY.insertInt(modifiedY.length()/2, 123);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            if (veryVerbose) cout << "\t\t\tInvalid record at end of db."
                                  << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            modifiedY.appendInt(123);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tRecords with the wrong number of fields."
                          << endl;
        {
            if (veryVerbose) cout << "\t\t\tToo few fields." << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            modifiedY.theList(0).removeElement(0);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            if (veryVerbose) cout << "\t\t\tToo many fields." << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            modifiedY.theList(0).appendInt(123);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tFields of the wrong type." << endl;
        {
            if (veryVerbose) cout << "\t\t\tInvalid user name." << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            modifiedY.theList(0).removeElement(FIELD_USER_NAME);
            modifiedY.theList(0).insertDouble(FIELD_USER_NAME, 1.2);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            if (veryVerbose) cout << "\t\t\tInvalid uuid." << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            modifiedY.theList(0).removeElement(FIELD_UUID);
            modifiedY.theList(0).insertDouble(FIELD_UUID, 1.2);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            if (veryVerbose) cout << "\t\t\tInvalid user number." << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            modifiedY.theList(0).removeElement(FIELD_USER_NUMBER);
            modifiedY.theList(0).insertDouble(FIELD_USER_NUMBER, 1.2);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            if (veryVerbose) cout << "\t\t\tInvalid firm number." << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            modifiedY.theList(0).removeElement(FIELD_FIRM_NUMBER);
            modifiedY.theList(0).insertDouble(FIELD_FIRM_NUMBER, 1.2);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            if (veryVerbose) cout << "\t\t\tInvalid login." << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            modifiedY.theList(0).removeElement(FIELD_LOGIN);
            modifiedY.theList(0).insertDouble(FIELD_LOGIN, 1.2);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            if (veryVerbose) cout << "\t\t\tInvalid password." << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            modifiedY.theList(0).removeElement(FIELD_PASSWORD);
            modifiedY.theList(0).insertDouble(FIELD_PASSWORD, 1.2);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tFields that are out of bounds." << endl;
        {
            if (veryVerbose) cout << "\t\t\tInvalid uuid." << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            modifiedY.theList(0).removeElement(FIELD_UUID);
            modifiedY.theList(0).insertInt(FIELD_UUID, 0);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            if (veryVerbose) cout << "\t\t\tInvalid user number." << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            modifiedY.theList(0).removeElement(FIELD_USER_NUMBER);
            modifiedY.theList(0).insertInt(FIELD_USER_NUMBER, 0);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            if (veryVerbose) cout << "\t\t\tInvalid login." << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            modifiedY.theList(0).removeElement(FIELD_LOGIN);
            modifiedY.theList(0).insertString(FIELD_LOGIN, "");

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tDatabases with duplicate records." << endl;
        {
            if (veryVerbose) cout << "\t\t\tDuplicated UUID." << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            bdem_List duplicate = SERIAL_Y.theList(0);
            duplicate.theInt(FIELD_USER_NUMBER) = 1234;
            duplicate.theString(FIELD_LOGIN) = "Something Else";
            modifiedY.appendList(duplicate);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            if (veryVerbose) cout << "\t\t\tDuplicated User Number." << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            bdem_List duplicate = SERIAL_Y.theList(0);
            duplicate.theInt(FIELD_UUID) = 1234;
            duplicate.theString(FIELD_LOGIN) = "Something Else";
            modifiedY.appendList(duplicate);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            if (veryVerbose) cout << "\t\t\tDuplicated Login." << endl;

            const char version = 1;

            bdem_List modifiedY = SERIAL_Y;
            bdem_List duplicate = SERIAL_Y.theList(0);
            duplicate.theInt(FIELD_UUID) = 1234;
            duplicate.theInt(FIELD_USER_NUMBER) = 5678;
            modifiedY.appendList(duplicate);

            Out out;
            out.putVersion(version);
            out << modifiedY; // stream out "new" value

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVeryVerbose);
            in >> t;        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            // test 'maxSupportedBdexVersion()'
            if (verbose) cout << "\tusing object syntax:" << endl;
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion());
            if (verbose) cout << "\tusing class method syntax:" << endl;
            ASSERT(1 == Obj::maxSupportedBdexVersion());
        }

        if (verbose) cout << "\nEnd of Streaming Functionality Test." << endl;
#endif
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //   We need to test the assignment operator ('operator=').
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.   Also, any object must
        //   be assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S X S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   const baedb_TestUserDb& operator=(const baedb_TestUserDb& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_userName;         // user name
            int         d_uuid;             // uuid
            int         d_userNumber;       // user number
            int         d_firmNumber;       // firm number
            const char *d_login;            // login ID
            const char *d_password;         // password
        } DATA[] = {
            {
                // minimum bounds
                L_,                         // source line number
                "",                         // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "l",                        // login ID
                "",                         // password
            },
            {
                // maximum bounds
                L_,                         // source line number
                "",                         // user name
                INT_MAX,                    // uuid
                INT_MAX,                    // user number
                INT_MAX,                    // firm number
                "login1",                   // login ID
                "",                         // password
            },
            {
                // orthogonal: base case
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: user name
                L_,                         // source line number
                "new user name",            // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: uuid
                L_,                         // source line number
                "user name",                // user name
                9999,                       // uuid
                1,                          // user number
                1,                          // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: user number
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                9999,                       // user number
                1,                          // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: firm number
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                1,                          // user number
                9999,                       // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: login ID
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "new login",                // login ID
                "password",                 // password
            },
            {
                // orthogonal: password
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "login",                    // login ID
                "new password",             // password
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        {
            if (verbose) cout << "\nTesting Assignment u = V." << endl;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   V_LINE        = DATA[i].d_lineNum;
                const char *V_USER_NAME   = DATA[i].d_userName;
                const int   V_UUID        = DATA[i].d_uuid;
                const int   V_USER_NUMBER = DATA[i].d_userNumber;
                const int   V_FIRM_NUMBER = DATA[i].d_firmNumber;
                const char *V_LOGIN       = DATA[i].d_login;
                const char *V_PASSWORD    = DATA[i].d_password;

                Obj mV; const Obj& V = mV;
                int v_result = mV.addRecord(V_USER_NAME, strlen(V_USER_NAME),
                                            V_UUID,
                                            V_USER_NUMBER,
                                            V_FIRM_NUMBER,
                                            V_LOGIN, strlen(V_LOGIN),
                                            V_PASSWORD, strlen(V_PASSWORD));
                LOOP2_ASSERT(V_LINE, v_result,
                             baedb_UserDb::SUCCESS == v_result);

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int   U_LINE        = DATA[j].d_lineNum;
                    const char *U_USER_NAME   = DATA[j].d_userName;
                    const int   U_UUID        = DATA[j].d_uuid;
                    const int   U_USER_NUMBER = DATA[j].d_userNumber;
                    const int   U_FIRM_NUMBER = DATA[j].d_firmNumber;
                    const char *U_LOGIN       = DATA[j].d_login;
                    const char *U_PASSWORD    = DATA[j].d_password;

                    Obj mU; const Obj& U = mU;
                    int u_result = mU.addRecord(U_USER_NAME,
                                                strlen(U_USER_NAME),
                                                U_UUID,
                                                U_USER_NUMBER,
                                                U_FIRM_NUMBER,
                                                U_LOGIN,
                                                strlen(U_LOGIN),
                                                U_PASSWORD,
                                                strlen(U_PASSWORD));
                    LOOP2_ASSERT(U_LINE, u_result,
                                 baedb_UserDb::SUCCESS == u_result);

                    Obj mW(V); const Obj& W = mW; // control
                    mU = V;
                    LOOP4_ASSERT(V_LINE, U_LINE, W, U, W == U);
                    LOOP4_ASSERT(V_LINE, U_LINE, W, V, W == V);
                }
            }
        }

        {
            if (verbose) cout << "\nTesting assignment u = u (Aliasing)."
                              << endl;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE        = DATA[i].d_lineNum;
                const char *USER_NAME   = DATA[i].d_userName;
                const int   UUID        = DATA[i].d_uuid;
                const int   USER_NUMBER = DATA[i].d_userNumber;
                const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                const char *LOGIN       = DATA[i].d_login;
                const char *PASSWORD    = DATA[i].d_password;

                Obj mU; const Obj& U = mU;
                int result = mU.addRecord(USER_NAME, strlen(USER_NAME),
                                          UUID, USER_NUMBER, FIRM_NUMBER,
                                          LOGIN, strlen(LOGIN),
                                          PASSWORD, strlen(PASSWORD));
                LOOP2_ASSERT(LINE, result, baedb_UserDb::SUCCESS == result);

                Obj mW(U); const Obj& W = mW; // control
                mU = U;
                LOOP3_ASSERT(LINE, W, U, W == U);
            }
        }

        if (verbose) cout << "\nEnd of Assignment Operator Test." << endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'g'
        //   This will test the 'g' generator function.
        //
        // Concerns:
        //   We want to verify that:
        //       1. An object generated by the 'g' function produces the same
        //          result as an object created by calling 'addRecord'
        //          manually.
        //       2. The 'g' function returns an object by value.
        //
        // Plan:
        //   For each enumerated sequence of 'spec' values, ordered by
        //   increasing 'spec' length, use the generator function 'g' to create
        //   a new object.  Verify that 'g' returns an object with the same
        //   value as an object that was set up by calling 'addRecord'
        //   manually.  Also test that 'g' returns a distinct object by
        //   comparing memory addresses of two separate objects returned by
        //   'g'.
        //
        // Testing:
        //   Obj g(const int *spec, const GeneratorData *data);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Generator Function 'g'"
                          << "\n==============================" << endl;

        static const GeneratorData DATA[] = {
            {
                // minimum bounds
                L_,                         // source line number
                "",                         // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "l",                        // login ID
                "",                         // password
            },
            {
                // maximum bounds
                L_,                         // source line number
                "",                         // user name
                INT_MAX,                    // uuid
                INT_MAX,                    // user number
                INT_MAX,                    // firm number
                "login1",                   // login ID
                "",                         // password
            },
            // Note, we cannot have orthogonal cases for uuid, user name
            // and login ID because these fields must *always* be unique.
            {
                // orthogonal: base case
                L_,                         // source line number
                "user name",                // user name
                11,                         // uuid
                11,                         // user number
                11,                         // firm number
                "login11",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: user name
                L_,                         // source line number
                "new user name",            // user name
                22,                         // uuid
                22,                         // user number
                1,                          // firm number
                "login22",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: firm number
                L_,                         // source line number
                "user name",                // user name
                33,                         // uuid
                33,                         // user number
                9999,                       // firm number
                "login33",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: password
                L_,                         // source line number
                "user name",                // user name
                44,                         // uuid
                44,                         // user number
                1,                          // firm number
                "login44",                  // login ID
                "new password",             // password
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        static const struct {
            int  d_lineNum;     // source line number
            int  d_spec[7];     // spec
        } SPEC_DATA[] = {
            //line  spec
            //----  ----
            { L_,   { -1                         }  },
            { L_,   {  0, -1                     }  },
            { L_,   {  0,  1, -1                 }  },
            { L_,   {  0,  1,  2, -1             }  },
            { L_,   {  0,  1,  2,  3, -1         }  },
            { L_,   {  0,  1,  2,  3,  4, -1     }  },
            { L_,   {  0,  1,  2,  3,  4,  5, -1 }  },
        };
        const int NUM_SPEC_DATA = sizeof SPEC_DATA / sizeof *SPEC_DATA;

        if (verbose) cout << "\nCompare values produced by 'g' and 'addRecord'"
                          << " manually for various inputs." << endl;
        for (int i = 0; i < NUM_SPEC_DATA; ++i) {
            const int  LINE = SPEC_DATA[i].d_lineNum;
            const int *SPEC = SPEC_DATA[i].d_spec;

            const Obj X = g(SPEC, DATA);

            Obj mY; const Obj& Y = mY;
            for (int j; -1 != (j = *SPEC); ++SPEC) {
                const int   LINE2       = DATA[j].d_lineNum;
                const char *USER_NAME   = DATA[j].d_userName;
                const int   UUID        = DATA[j].d_uuid;
                const int   USER_NUMBER = DATA[j].d_userNumber;
                const int   FIRM_NUMBER = DATA[j].d_firmNumber;
                const char *LOGIN       = DATA[j].d_login;
                const char *PASSWORD    = DATA[j].d_password;

                int result = mY.addRecord(USER_NAME, strlen(USER_NAME),
                                          UUID, USER_NUMBER, FIRM_NUMBER,
                                          LOGIN, strlen(LOGIN),
                                          PASSWORD, strlen(PASSWORD));
                LOOP3_ASSERT(LINE, LINE2, result, 0 == result);
            }

            LOOP3_ASSERT(LINE, X, Y, X == Y);
        }

        if (verbose) cout << "\nConfirm return-by-value." << endl;
        {
            const int *SPEC = SPEC_DATA[1].d_spec;

            ASSERT(sizeof(Obj) == sizeof g(SPEC, DATA)); // compile-time fact

            const Obj& r1 = g(SPEC, DATA);               // runtime test
            const Obj& r2 = g(SPEC, DATA);
            ASSERT(&r1 != &r2);
        }

        if (verbose) cout << "\n End of Generator Function 'g' Test." << endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //   This will test the copy constructor.
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically-valued objects W and X using tested methods
        //   (in this case, the 'addRecord' method).  Then copy construct an
        //   object Y from X, and use the equality operator to assert that both
        //   X and Y have the same value as W.
        //
        // Testing:
        //   baedb_TestUserDb(const baedb_TestUserDb&  original,
        //                    bslma_Allocator         *basicAllocator = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_userName;         // user name
            int         d_uuid;             // uuid
            int         d_userNumber;       // user number
            int         d_firmNumber;       // firm number
            const char *d_login;            // login ID
            const char *d_password;         // password
        } DATA[] = {
            {
                // minimum bounds
                L_,                         // source line number
                "",                         // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "l",                        // login ID
                "",                         // password
            },
            {
                // maximum bounds
                L_,                         // source line number
                "",                         // user name
                INT_MAX,                    // uuid
                INT_MAX,                    // user number
                INT_MAX,                    // firm number
                "login1",                   // login ID
                "",                         // password
            },
            {
                // orthogonal: base case
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: user name
                L_,                         // source line number
                "new user name",            // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: uuid
                L_,                         // source line number
                "user name",                // user name
                9999,                       // uuid
                1,                          // user number
                1,                          // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: user number
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                9999,                       // user number
                1,                          // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: firm number
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                1,                          // user number
                9999,                       // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: login ID
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "new login",                // login ID
                "password",                 // password
            },
            {
                // orthogonal: password
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "login",                    // login ID
                "new password",             // password
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE        = DATA[i].d_lineNum;
            const char *USER_NAME   = DATA[i].d_userName;
            const int   UUID        = DATA[i].d_uuid;
            const int   USER_NUMBER = DATA[i].d_userNumber;
            const int   FIRM_NUMBER = DATA[i].d_firmNumber;
            const char *LOGIN       = DATA[i].d_login;
            const char *PASSWORD    = DATA[i].d_password;

            Obj mW; const Obj& W = mW;
            Obj mX; const Obj& X = mX;
            int result;

            result = mW.addRecord(USER_NAME, strlen(USER_NAME),
                                  UUID, USER_NUMBER, FIRM_NUMBER,
                                  LOGIN, strlen(LOGIN),
                                  PASSWORD, strlen(PASSWORD));
            LOOP2_ASSERT(LINE, result, baedb_UserDb::SUCCESS == result);

            result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                  UUID, USER_NUMBER, FIRM_NUMBER,
                                  LOGIN, strlen(LOGIN),
                                  PASSWORD, strlen(PASSWORD));
            LOOP2_ASSERT(LINE, result, baedb_UserDb::SUCCESS == result);

            Obj mY(X); const Obj& Y = mY;

            LOOP3_ASSERT(LINE, X, W, X == W);
            LOOP3_ASSERT(LINE, Y, W, Y == W);
        }

        if (verbose) cout << "\nEnd of Copy Constructor Test." << endl;
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //   This will test the equality operators ('operator==' and
        //   'operator!=').
        //
        // Concerns:
        //   We want to make sure that the free operators work correctly.  We
        //   want to make sure that 'operator==' returns false for objects that
        //   are very similar but still different, but returns true for objects
        //   that are exactly the same.  Likewise, we want to make sure that
        //   'operator!=' returns true for objects that are very similar but
        //   still different, but returns false for objects that are exactly
        //   the same.
        //
        // Plan:
        //   Create independent recordsets, each of them containing similar
        //   but different data.  For the cross product of these recordsets,
        //   exercise the equality operators and verify that their return
        //   values are as expected.
        //
        // Testing:
        //   bool operator==(const baedb_TestUserDb&, const baedb_TestUserDb&);
        //   bool operator!=(const baedb_TestUserDb&, const baedb_TestUserDb&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_userName;         // user name
            int         d_uuid;             // uuid
            int         d_userNumber;       // user number
            int         d_firmNumber;       // firm number
            const char *d_login;            // login ID
            const char *d_password;         // password
        } DATA[] = {
            {
                // minimum bounds
                L_,                         // source line number
                "",                         // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "l",                        // login ID
                "",                         // password
            },
            {
                // maximum bounds
                L_,                         // source line number
                "",                         // user name
                INT_MAX,                    // uuid
                INT_MAX,                    // user number
                INT_MAX,                    // firm number
                "login1",                   // login ID
                "",                         // password
            },
            {
                // orthogonal: base case
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: user name
                L_,                         // source line number
                "new user name",            // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: uuid
                L_,                         // source line number
                "user name",                // user name
                9999,                       // uuid
                1,                          // user number
                1,                          // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: user number
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                9999,                       // user number
                1,                          // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: firm number
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                1,                          // user number
                9999,                       // firm number
                "login",                    // login ID
                "password",                 // password
            },
            {
                // orthogonal: login ID
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "new login",                // login ID
                "password",                 // password
            },
            {
                // orthogonal: password
                L_,                         // source line number
                "user name",                // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "login",                    // login ID
                "new password",             // password
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nCompare each pair of similar values (u, v) in "
                          << "S X S." << endl;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   U_LINE        = DATA[i].d_lineNum;
            const char *U_USER_NAME   = DATA[i].d_userName;
            const int   U_UUID        = DATA[i].d_uuid;
            const int   U_USER_NUMBER = DATA[i].d_userNumber;
            const int   U_FIRM_NUMBER = DATA[i].d_firmNumber;
            const char *U_LOGIN       = DATA[i].d_login;
            const char *U_PASSWORD    = DATA[i].d_password;

            Obj mU; const Obj& U = mU;
            int u_result = mU.addRecord(U_USER_NAME, strlen(U_USER_NAME),
                                        U_UUID, U_USER_NUMBER, U_FIRM_NUMBER,
                                        U_LOGIN, strlen(U_LOGIN),
                                        U_PASSWORD, strlen(U_PASSWORD));
            LOOP2_ASSERT(U_LINE, u_result, baedb_UserDb::SUCCESS == u_result);

            for (int j = 0; j < NUM_DATA; ++j) {
                const int   V_LINE        = DATA[j].d_lineNum;
                const char *V_USER_NAME   = DATA[j].d_userName;
                const int   V_UUID        = DATA[j].d_uuid;
                const int   V_USER_NUMBER = DATA[j].d_userNumber;
                const int   V_FIRM_NUMBER = DATA[j].d_firmNumber;
                const char *V_LOGIN       = DATA[j].d_login;
                const char *V_PASSWORD    = DATA[j].d_password;

                Obj mV; const Obj& V = mV;
                int v_result = mV.addRecord(V_USER_NAME, strlen(V_USER_NAME),
                                            V_UUID,
                                            V_USER_NUMBER,
                                            V_FIRM_NUMBER,
                                            V_LOGIN, strlen(V_LOGIN),
                                            V_PASSWORD, strlen(V_PASSWORD));
                LOOP2_ASSERT(V_LINE, v_result,
                             baedb_UserDb::SUCCESS == v_result);

                const int isSame = (i == j);
                LOOP5_ASSERT(U_LINE, V_LINE, isSame,    U,   V,
                                             isSame == (U == V));
                LOOP5_ASSERT(U_LINE, V_LINE, isSame,    U,   V,
                                            !isSame == (U != V));
            }
        }

        if (verbose) cout << "\nEnd of Equality Operators Test" << endl;
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //   We need to test the '<<' operator.
        //
        // Concerns:
        //   The 'baedb_TestUserDb' output operator just dumps the internal
        //   'bdem_List' object into the stream.  We just need to make sure
        //   that the output operator is correctly forwarded to the 'bdem_List'
        //   output operator.
        //
        // Plan:
        //   The '<<' operator depends on the 'print' member function.  So, we
        //   need to test 'print' before testing 'operator<<'.  This test is
        //   broken up into two parts:
        //      1. Testing of 'print'.
        //      2. Testing of 'operator<<'.
        //
        //  Each test vector in DATA contains all the fields necessary to make
        //  up a record in a user database.  Construct an independent object
        //  'mX' and call 'addRecord' to insert the records contained in DATA.
        //  Create two 'ostrstream' objects, 'stream1' and 'stream2'.  Use the
        //  'print' method to stream 'mX' into 'stream1'.  Then use the
        //  'bdem_List' output operator to stream 'X.view()' into 'stream2'.
        //  Make sure that 'stream1' and 'stream2' contain the same data.  Run
        //  this test using 'mX' objects of increasing number of records.
        //
        //  To test the '<<' operator, repeat the above test using 'operator<<'
        //  instead of 'print'.
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& stream) const;
        //   bsl::ostream& operator<<(bsl::ostream&, const baedb_TestUserDb&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Output (<<) Operator"
                          << "\n============================" << endl;

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_userName;         // user name
            int         d_uuid;             // uuid
            int         d_userNumber;       // user number
            int         d_firmNumber;       // firm number
            const char *d_login;            // login ID
            const char *d_password;         // password
        } DATA[] = {
            {
                // minimum bounds
                L_,                         // source line number
                "",                         // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "l",                        // login ID
                "",                         // password
            },
            {
                // maximum bounds
                L_,                         // source line number
                "",                         // user name
                INT_MAX,                    // uuid
                INT_MAX,                    // user number
                INT_MAX,                    // firm number
                "login1",                   // login ID
                "",                         // password
            },
            // Note, we cannot have orthogonal cases for uuid, user name
            // and login ID because these fields must *always* be unique.
            {
                // orthogonal: base case
                L_,                         // source line number
                "user name",                // user name
                11,                         // uuid
                11,                         // user number
                11,                         // firm number
                "login11",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: user name
                L_,                         // source line number
                "new user name",            // user name
                22,                         // uuid
                22,                         // user number
                1,                          // firm number
                "login22",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: firm number
                L_,                         // source line number
                "user name",                // user name
                33,                         // uuid
                33,                         // user number
                9999,                       // firm number
                "login33",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: password
                L_,                         // source line number
                "user name",                // user name
                44,                         // uuid
                44,                         // user number
                1,                          // firm number
                "login44",                  // login ID
                "new password",             // password
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int maxSize;

        if (verbose) cout << "\n1. Testing 'print'." << endl;

        for (maxSize = 0; maxSize <= NUM_DATA; ++maxSize) {
            // Create a user database containing 'maxSize' records.  Then test
            // the 'print' method.

            Obj mX; const Obj& X = mX;

            for (int i = 0; i < maxSize; ++i) {
                // Add the record at position 'i'.

                const int   LINE        = DATA[i].d_lineNum;
                const char *USER_NAME   = DATA[i].d_userName;
                const int   UUID        = DATA[i].d_uuid;
                const int   USER_NUMBER = DATA[i].d_userNumber;
                const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                const char *LOGIN       = DATA[i].d_login;
                const char *PASSWORD    = DATA[i].d_password;

                int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                          UUID, USER_NUMBER, FIRM_NUMBER,
                                          LOGIN, strlen(LOGIN),
                                          PASSWORD, strlen(PASSWORD));
                LOOP2_ASSERT(LINE, result, baedb_UserDb::SUCCESS == result);
            }

            // Create the streams.

            const int SIZE = 2048;

            char buf1[SIZE];
            memset(buf1, 0, sizeof(buf1));
            ostrstream stream1(buf1, SIZE);

            char buf2[SIZE];
            memset(buf2, 0, sizeof(buf2));
            ostrstream stream2(buf2, SIZE);

            // Stream the objects and compare the results.

            X.print(stream1, 0, -1);
            stream2 << X;
            LOOP3_ASSERT(maxSize, buf1, buf2, 0 == strcmp(buf1, buf2));
        }

        if (verbose) cout << "\n2. Testing 'operator<<'." << endl;

        for (maxSize = 0; maxSize <= NUM_DATA; ++maxSize) {
            // Create a user database containing 'maxSize' records.  Then test
            // the '<<' operator.

            Obj mX; const Obj& X = mX;

            for (int i = 0; i < maxSize; ++i) {
                // Add the record at position 'i'.

                const int   LINE        = DATA[i].d_lineNum;
                const char *USER_NAME   = DATA[i].d_userName;
                const int   UUID        = DATA[i].d_uuid;
                const int   USER_NUMBER = DATA[i].d_userNumber;
                const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                const char *LOGIN       = DATA[i].d_login;
                const char *PASSWORD    = DATA[i].d_password;

                int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                          UUID, USER_NUMBER, FIRM_NUMBER,
                                          LOGIN, strlen(LOGIN),
                                          PASSWORD, strlen(PASSWORD));
                LOOP2_ASSERT(LINE, result, baedb_UserDb::SUCCESS == result);
            }

            // Create the streams.

            const int SIZE = 2048;

            char buf1[SIZE];
            memset(buf1, 0, sizeof(buf1));
            ostrstream stream1(buf1, SIZE);

            char buf2[SIZE];
            memset(buf2, 0, sizeof(buf2));
            ostrstream stream2(buf2, SIZE);

            // Stream the objects and compare the results.

            bdem_List db;

            const bdem_List& firmView = X.firmView();
            if (0 == firmView.length()) {
                db.appendNullList();
            }
            else {
                db.appendList(firmView);
            }

            const bdem_List& userView = X.userView();
            if (0 == userView.length()) {
                db.appendNullList();
            }
            else {
                db.appendList(userView);
            }

            stream1 << X;
            stream2 << db;
            LOOP3_ASSERT(maxSize, buf1, buf2, 0 == strcmp(buf1, buf2));
        }

        if (verbose) cout << "\nEnd of Output (<<) Operator Test." << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   This will test the basic accessors in the component.
        //
        // Concerns:
        //   We want to make sure that the basic accessors return the correct
        //   values.
        //
        // Plan:
        //   Create a set of test data, then load the data using the component
        //   manipulators (which have already been tested).  Exercise each of
        //   the accessors and verify that the returned values are as expected.
        //
        // Testing:
        //   virtual int lookupFirmNumberByLogin(...) const;
        //   virtual int lookupFirmNumberByUserNumber(...) const;
        //   virtual int lookupFirmNumberByUUID(...) const;
        //   virtual int lookupLoginByUserNumber(...) const;
        //   virtual int lookupLoginByUUID(...) const;
        //   virtual int lookupUserNameByLogin(...) const;
        //   virtual int lookupUserNameByUserNumber(...) const;
        //   virtual int lookupUserNameByUUID(...) const;
        //   virtual int lookupUserNumberByLogin(...) const;
        //   virtual int lookupUserNumberByUUID(...) const;
        //   virtual int lookupUUIDByLogin(...) const;
        //   virtual int lookupUUIDByUserNumber(...) const;
        //   int numRecords() const;
        //   const bdem_List& view() const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Basic Accessors"
                          << "\n=======================" << endl;

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_userName;         // user name
            int         d_uuid;             // uuid
            int         d_userNumber;       // user number
            int         d_firmNumber;       // firm number
            const char *d_login;            // login ID
            const char *d_password;         // password
        } DATA[] = {
            {
                // minimum bounds
                L_,                         // source line number
                "",                         // user name
                1,                          // uuid
                1,                          // user number
                1,                          // firm number
                "l",                        // login ID
                "",                         // password
            },
            {
                // maximum bounds
                L_,                         // source line number
                "",                         // user name
                INT_MAX,                    // uuid
                INT_MAX,                    // user number
                INT_MAX,                    // firm number
                "login1",                   // login ID
                "",                         // password
            },
            // Note, we cannot have orthogonal cases for uuid, user name
            // and login ID because these fields must *always* be unique.
            {
                // orthogonal: base case
                L_,                         // source line number
                "user name",                // user name
                11,                         // uuid
                11,                         // user number
                11,                         // firm number
                "login11",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: user name
                L_,                         // source line number
                "new user name",            // user name
                22,                         // uuid
                22,                         // user number
                1,                          // firm number
                "login22",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: firm number
                L_,                         // source line number
                "user name",                // user name
                33,                         // uuid
                33,                         // user number
                9999,                       // firm number
                "login33",                  // login ID
                "password",                 // password
            },
            {
                // orthogonal: password
                L_,                         // source line number
                "user name",                // user name
                44,                         // uuid
                44,                         // user number
                1,                          // firm number
                "login44",                  // login ID
                "new password",             // password
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\n1. Testing 'view'." << endl;
        {
            // Call the 'addRecord' method using 'DATA'.  Then verify that the
            // 'view' method returns a 'bdem_List' object that contains the
            // same records as 'DATA'.

            int i;
            Obj mX; const Obj& X = mX;

            for (i = 0; i < NUM_DATA; ++i) {
                const int   LINE        = DATA[i].d_lineNum;
                const char *USER_NAME   = DATA[i].d_userName;
                const int   UUID        = DATA[i].d_uuid;
                const int   USER_NUMBER = DATA[i].d_userNumber;
                const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                const char *LOGIN       = DATA[i].d_login;
                const char *PASSWORD    = DATA[i].d_password;

                int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                          UUID, USER_NUMBER, FIRM_NUMBER,
                                          LOGIN, strlen(LOGIN),
                                          PASSWORD, strlen(PASSWORD));
                LOOP2_ASSERT(LINE, result, baedb_UserDb::SUCCESS == result);
            }

            // make sure 'view' returns the same data
            for (i = 0; i < NUM_DATA; ++i) {
                const int       LINE        = DATA[i].d_lineNum;
                const string    USER_NAME   = DATA[i].d_userName;
                const int       UUID        = DATA[i].d_uuid;
                const int       USER_NUMBER = DATA[i].d_userNumber;
                const int       FIRM_NUMBER = DATA[i].d_firmNumber;
                const string    LOGIN       = DATA[i].d_login;
                const string    PASSWORD    = DATA[i].d_password;

                LOOP3_ASSERT(LINE,
                             USER_NAME,
                             X.view().theList(i).theString(FIELD_USER_NAME),
                             USER_NAME ==
                             X.view().theList(i).theString(FIELD_USER_NAME));

                LOOP3_ASSERT(LINE,
                             UUID,   X.view().theList(i).theInt(FIELD_UUID),
                             UUID == X.view().theList(i).theInt(FIELD_UUID));

                LOOP3_ASSERT(LINE,
                             USER_NUMBER,
                             X.view().theList(i).theInt(FIELD_USER_NUMBER),
                             USER_NUMBER ==
                             X.view().theList(i).theInt(FIELD_USER_NUMBER));

                LOOP3_ASSERT(LINE,
                             FIRM_NUMBER,
                             X.view().theList(i).theInt(FIELD_FIRM_NUMBER),
                             FIRM_NUMBER ==
                             X.view().theList(i).theInt(FIELD_FIRM_NUMBER));

                LOOP3_ASSERT(LINE,
                             LOGIN,
                             X.view().theList(i).theString(FIELD_LOGIN),
                             LOGIN ==
                             X.view().theList(i).theString(FIELD_LOGIN));

                LOOP3_ASSERT(LINE,
                             PASSWORD,
                             X.view().theList(i).theString(FIELD_PASSWORD),
                             PASSWORD ==
                             X.view().theList(i).theString(FIELD_PASSWORD));
            }
        }

        if (verbose) cout << "\n2. Testing 'numRecords'." << endl;
        {
            // Call the 'addRecord' method using 'DATA'.  Each time 'addRecord'
            // is called, check to make sure that 'numRecords' returns the
            // correct value.

            Obj mX; const Obj& X = mX;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE        = DATA[i].d_lineNum;
                const char *USER_NAME   = DATA[i].d_userName;
                const int   UUID        = DATA[i].d_uuid;
                const int   USER_NUMBER = DATA[i].d_userNumber;
                const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                const char *LOGIN       = DATA[i].d_login;
                const char *PASSWORD    = DATA[i].d_password;

                int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                          UUID, USER_NUMBER, FIRM_NUMBER,
                                          LOGIN, strlen(LOGIN),
                                          PASSWORD, strlen(PASSWORD));
                LOOP2_ASSERT(LINE, result, baedb_UserDb::SUCCESS == result);

                LOOP3_ASSERT(LINE, i+1,   X.numRecords(),
                                   i+1 == X.numRecords());
            }
        }

        if (verbose) cout << "\n3. Testing protocol accessors "
                          << "using valid data." << endl;
        {
            // fill the database
            int i;
            Obj mX; const Obj& X = mX;

            for (i = 0; i < NUM_DATA; ++i) {
                const int   LINE        = DATA[i].d_lineNum;
                const char *USER_NAME   = DATA[i].d_userName;
                const int   UUID        = DATA[i].d_uuid;
                const int   USER_NUMBER = DATA[i].d_userNumber;
                const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                const char *LOGIN       = DATA[i].d_login;
                const char *PASSWORD    = DATA[i].d_password;

                int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                          UUID, USER_NUMBER, FIRM_NUMBER,
                                          LOGIN, strlen(LOGIN),
                                          PASSWORD, strlen(PASSWORD));
                LOOP2_ASSERT(LINE, result, baedb_UserDb::SUCCESS == result);
            }

            // test the accessors
            string userName;
            string login;
            int    uuid;
            int    userNumber;
            int    firmNumber;
            int    retCode;

            for (i = 0; i < NUM_DATA; ++i) {
                const int    LINE        = DATA[i].d_lineNum;
                const string USER_NAME   = DATA[i].d_userName;
                const int    UUID        = DATA[i].d_uuid;
                const int    USER_NUMBER = DATA[i].d_userNumber;
                const int    FIRM_NUMBER = DATA[i].d_firmNumber;
                const string LOGIN       = DATA[i].d_login;
                const string PASSWORD    = DATA[i].d_password;

                if (veryVerbose) cout << "\ta. Testing by UUID." << endl;
                {
                    userName = "";
                    retCode = X.lookupUserNameByUUID(&userName, UUID);
                    LOOP2_ASSERT(LINE,                    retCode,
                                 baedb_UserDb::SUCCESS == retCode);
                    LOOP3_ASSERT(LINE, USER_NAME,   userName,
                                       USER_NAME == userName);

                    userNumber = 0;
                    retCode = X.lookupUserNumberByUUID(&userNumber, UUID);
                    LOOP2_ASSERT(LINE,                    retCode,
                                 baedb_UserDb::SUCCESS == retCode);
                    LOOP3_ASSERT(LINE, USER_NUMBER,   userNumber,
                                       USER_NUMBER == userNumber);

                    firmNumber = 0;
                    retCode = X.lookupFirmNumberByUUID(&firmNumber, UUID);
                    LOOP2_ASSERT(LINE,                    retCode,
                                 baedb_UserDb::SUCCESS == retCode);
                    LOOP3_ASSERT(LINE, FIRM_NUMBER,   firmNumber,
                                       FIRM_NUMBER == firmNumber);

                    login = "";
                    retCode = X.lookupLoginByUUID(&login, UUID);
                    LOOP2_ASSERT(LINE,                    retCode,
                                 baedb_UserDb::SUCCESS == retCode);
                    LOOP3_ASSERT(LINE, LOGIN,   login,
                                       LOGIN == login);
                }

                if (veryVerbose) cout << "\tb. Testing by User Number."
                                      << endl;
                {
                    userName = "";
                    retCode = X.lookupUserNameByUserNumber(&userName,
                                                            USER_NUMBER);
                    LOOP2_ASSERT(LINE,                    retCode,
                                 baedb_UserDb::SUCCESS == retCode);
                    LOOP3_ASSERT(LINE, USER_NAME,   userName,
                                       USER_NAME == userName);

                    uuid = 0;
                    retCode = X.lookupUUIDByUserNumber(&uuid, USER_NUMBER);
                    LOOP2_ASSERT(LINE,                    retCode,
                                 baedb_UserDb::SUCCESS == retCode);
                    LOOP3_ASSERT(LINE, UUID,   uuid,
                                       UUID == uuid);

                    firmNumber = 0;
                    retCode = X.lookupFirmNumberByUserNumber(&firmNumber,
                                                              USER_NUMBER);
                    LOOP2_ASSERT(LINE,                    retCode,
                                 baedb_UserDb::SUCCESS == retCode);
                    LOOP3_ASSERT(LINE, FIRM_NUMBER,   firmNumber,
                                       FIRM_NUMBER == firmNumber);

                    login = "";
                    retCode = X.lookupLoginByUserNumber(&login, USER_NUMBER);
                    LOOP2_ASSERT(LINE,                    retCode,
                                 baedb_UserDb::SUCCESS == retCode);
                    LOOP3_ASSERT(LINE, LOGIN,   login,
                                       LOGIN == login);
                }

                if (veryVerbose) cout << "\tc. Testing by Login." << endl;
                {
                    userName = "";
                    retCode = X.lookupUserNameByLogin(&userName,
                                                       LOGIN.data(),
                                                       LOGIN.length());
                    LOOP2_ASSERT(LINE,                    retCode,
                                 baedb_UserDb::SUCCESS == retCode);
                    LOOP3_ASSERT(LINE, USER_NAME,   userName,
                                       USER_NAME == userName);

                    uuid = 0;
                    retCode = X.lookupUUIDByLogin(&uuid,
                                                   LOGIN.data(),
                                                   LOGIN.length());
                    LOOP2_ASSERT(LINE,                    retCode,
                                 baedb_UserDb::SUCCESS == retCode);
                    LOOP3_ASSERT(LINE, UUID,   uuid,
                                       UUID == uuid);

                    userNumber = 0;
                    retCode = X.lookupUserNumberByLogin(&userNumber,
                                                         LOGIN.data(),
                                                         LOGIN.length());
                    LOOP2_ASSERT(LINE,                    retCode,
                                 baedb_UserDb::SUCCESS == retCode);
                    LOOP3_ASSERT(LINE, USER_NUMBER,   userNumber,
                                       USER_NUMBER == userNumber);

                    firmNumber = 0;
                    retCode = X.lookupFirmNumberByLogin(&firmNumber,
                                                         LOGIN.data(),
                                                         LOGIN.length());
                    LOOP2_ASSERT(LINE,                    retCode,
                                 baedb_UserDb::SUCCESS == retCode);
                    LOOP3_ASSERT(LINE, FIRM_NUMBER,   firmNumber,
                                       FIRM_NUMBER == firmNumber);
                }
            }
        }

        if (verbose) cout << "\n4. Testing protocol accessors "
                          << "using invalid data." << endl;
        {
            // fill the database
            int i;
            Obj mX; const Obj& X = mX;

            for (i = 0; i < NUM_DATA; ++i) {
                const int   LINE        = DATA[i].d_lineNum;
                const char *USER_NAME   = DATA[i].d_userName;
                const int   UUID        = DATA[i].d_uuid;
                const int   USER_NUMBER = DATA[i].d_userNumber;
                const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                const char *LOGIN       = DATA[i].d_login;
                const char *PASSWORD    = DATA[i].d_password;

                int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                          UUID, USER_NUMBER, FIRM_NUMBER,
                                          LOGIN, strlen(LOGIN),
                                          PASSWORD, strlen(PASSWORD));
                LOOP2_ASSERT(LINE, result, baedb_UserDb::SUCCESS == result);
            }

            // test the accessors
            string userName,   initUserName;
            string login,      initLogin;
            int    uuid,       initUuid;
            int    userNumber, initUserNumber;
            int    firmNumber, initFirmNumber;
            int    retCode;

            static const struct {
                int         d_lineNum;    // source line number
                int         d_uuid;       // uuid
                int         d_userNumber; // user number
                const char *d_login;      // login ID
                int         d_rv;         // expected return value
                int         d_rvLogin;    // expected return value for login
            } INVALID_DATA[] = {
                {
                    L_,                           // source line number
                    1111111,                      // uuid
                    2222222,                      // user number
                    "",                           // login ID
                    baedb_UserDb::NOT_FOUND,      // return value
                    baedb_UserDb::INVALID_LOGIN   // return value for login
                },
                {
                    L_,                           // source line number
                    9876543,                      // uuid
                    1234567,                      // user number
                    "some login",                 // login ID
                    baedb_UserDb::NOT_FOUND,      // return value
                    baedb_UserDb::NOT_FOUND       // return value for login
                },
            };
            const int NUM_INVALID_DATA = sizeof INVALID_DATA
                                         / sizeof *INVALID_DATA;

            for (int i = 0; i < NUM_INVALID_DATA; ++i) {
                const int   LINE        = INVALID_DATA[i].d_lineNum;
                const int   UUID        = INVALID_DATA[i].d_uuid;
                const int   USER_NUMBER = INVALID_DATA[i].d_userNumber;
                const char *LOGIN       = INVALID_DATA[i].d_login;
                const int   RV          = INVALID_DATA[i].d_rv;
                const int   RV_LOGIN    = INVALID_DATA[i].d_rvLogin;

                userName = initUserName = "username_uuid";
                retCode = X.lookupUserNameByUUID(&userName, UUID);
                LOOP2_ASSERT(LINE, retCode,
                             RV == retCode);
                LOOP3_ASSERT(LINE, userName,   initUserName,
                                   userName == initUserName);

                userNumber = initUserNumber = 12345;
                retCode = X.lookupUserNumberByUUID(&userNumber, UUID);
                LOOP2_ASSERT(LINE, retCode,
                             RV == retCode);
                LOOP3_ASSERT(LINE, userNumber,   initUserNumber,
                                   userNumber == initUserNumber);

                firmNumber = initFirmNumber = 23456;
                retCode = X.lookupFirmNumberByUUID(&firmNumber, UUID);
                LOOP2_ASSERT(LINE, retCode,
                             RV == retCode);
                LOOP3_ASSERT(LINE, firmNumber,   initFirmNumber,
                                   firmNumber == initFirmNumber);

                login = initLogin = "login_uuid";
                retCode = X.lookupLoginByUUID(&login, UUID);
                LOOP2_ASSERT(LINE, retCode,
                             RV == retCode);
                LOOP3_ASSERT(LINE, login,   initLogin,
                                   login == initLogin);

                userName = initUserName = "username_usernumber";
                retCode = X.lookupUserNameByUserNumber(&userName, USER_NUMBER);
                LOOP2_ASSERT(LINE, retCode,
                             RV == retCode);
                LOOP3_ASSERT(LINE, userName,   initUserName,
                                   userName == initUserName);

                uuid = initUuid = 34567;
                retCode = X.lookupUUIDByUserNumber(&uuid, USER_NUMBER);
                LOOP2_ASSERT(LINE, retCode,
                             RV == retCode);
                LOOP3_ASSERT(LINE, uuid,   initUuid,
                                   uuid == initUuid);

                firmNumber = initFirmNumber = 34567;
                retCode = X.lookupFirmNumberByUserNumber(&firmNumber,
                                                          USER_NUMBER);
                LOOP2_ASSERT(LINE, retCode,
                             RV == retCode);
                LOOP3_ASSERT(LINE, firmNumber,   initFirmNumber,
                                   firmNumber == initFirmNumber);

                login = initLogin = "login_usernumber";
                retCode = X.lookupLoginByUserNumber(&login, USER_NUMBER);
                LOOP2_ASSERT(LINE, retCode,
                             RV == retCode);
                LOOP3_ASSERT(LINE, login,   initLogin,
                                   login == initLogin);

                userName = initUserName = "username_login";
                retCode = X.lookupUserNameByLogin(&userName,
                                                   LOGIN, strlen(LOGIN));
                LOOP2_ASSERT(LINE,       retCode,
                             RV_LOGIN == retCode);
                LOOP3_ASSERT(LINE, userName,   initUserName,
                                   userName == initUserName);

                uuid = initUuid = 45678;
                retCode = X.lookupUUIDByLogin(&uuid, LOGIN, strlen(LOGIN));
                LOOP2_ASSERT(LINE,       retCode,
                             RV_LOGIN == retCode);
                LOOP3_ASSERT(LINE, uuid,   initUuid,
                                   uuid == initUuid);

                userNumber = initUserNumber = 56789;
                retCode = X.lookupUserNumberByLogin(&userNumber,
                                                     LOGIN, strlen(LOGIN));
                LOOP2_ASSERT(LINE,       retCode,
                             RV_LOGIN == retCode);
                LOOP3_ASSERT(LINE, userNumber,   initUserNumber,
                                   userNumber == initUserNumber);

                firmNumber = initFirmNumber = 67890;
                retCode = X.lookupFirmNumberByLogin(&firmNumber,
                                                     LOGIN, strlen(LOGIN));
                LOOP2_ASSERT(LINE,       retCode,
                             RV_LOGIN == retCode);
                LOOP3_ASSERT(LINE, firmNumber,   initFirmNumber,
                                   firmNumber == initFirmNumber);
            }
        }

        if (verbose) cout << "\nEnd of Basic Accessors Test." << endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'addRecord' METHOD
        //   This will test the 'addRecord' method.
        //
        // Concerns:
        //   The 'addRecord' method will be used by other test cases in this
        //   test driver because it is easier to use (for testing purposes)
        //   than the 'addRecords' method.  Before we can proceed with the
        //   other test cases, we need to thoroughly test this method to make
        //   sure it works correctly.  In particular, we are concerned with the
        //   following scenarios:
        //       1. Make sure that 'addRecord' succeeds with valid data.
        //       2. Make sure that 'addRecord' fails with invalid data.
        //       3. Make sure that 'addRecord' fails with valid, but duplicate,
        //          data.
        //
        // Plan:
        //   For each scenario, run the 'addRecord' method and make sure that
        //   it returns the expected status code.  Also, make sure that the
        //   object's state after the function call is as expected.
        //
        // Testing:
        //   int addRecord(const char*, int, int, int, int, const char*, int,
        //                                                  const char*, int);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'addRecord' Method"
                          << "\n==========================" << endl;

        if (verbose) cout << "\nTesting addRecord(const char*, int, int, int, "
                          << "int, const char*, int, const char*, int)."
                          << endl;

        if (veryVerbose) cout << "\ta. Testing with valid data." << endl;
        {
            // Call the 'addRecord' method using valid, non-duplicated data.
            // Then verify that all the records have been added.

            static const struct {
                int         d_lineNum;          // source line number
                const char *d_userName;         // user name
                int         d_uuid;             // uuid
                int         d_userNumber;       // user number
                int         d_firmNumber;       // firm number
                const char *d_login;            // login ID
                const char *d_password;         // password
            } DATA[] = {
                {
                    // minimum bounds
                    L_,                         // source line number
                    "",                         // user name
                    1,                          // uuid
                    1,                          // user number
                    1,                          // firm number
                    "l",                        // login ID
                    "",                         // password
                },
                {
                    // maximum bounds
                    L_,                         // source line number
                    "",                         // user name
                    INT_MAX,                    // uuid
                    INT_MAX,                    // user number
                    INT_MAX,                    // firm number
                    "login1",                   // login ID
                    "",                         // password
                },
                // Note, we cannot have orthogonal cases for uuid, user name
                // and login ID because these fields must *always* be unique.
                {
                    // orthogonal: base case
                    L_,                         // source line number
                    "user name",                // user name
                    11,                         // uuid
                    11,                         // user number
                    11,                         // firm number
                    "login11",                  // login ID
                    "password",                 // password
                },
                {
                    // orthogonal: user name
                    L_,                         // source line number
                    "new user name",            // user name
                    22,                         // uuid
                    22,                         // user number
                    1,                          // firm number
                    "login22",                  // login ID
                    "password",                 // password
                },
                {
                    // orthogonal: firm number
                    L_,                         // source line number
                    "user name",                // user name
                    33,                         // uuid
                    33,                         // user number
                    9999,                       // firm number
                    "login33",                  // login ID
                    "password",                 // password
                },
                {
                    // orthogonal: password
                    L_,                         // source line number
                    "user name",                // user name
                    44,                         // uuid
                    44,                         // user number
                    1,                          // firm number
                    "login44",                  // login ID
                    "new password",             // password
                },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int i;
            Obj mX; const Obj& X = mX;

            for (i = 0; i < NUM_DATA; ++i) {
                const int   LINE        = DATA[i].d_lineNum;
                const char *USER_NAME   = DATA[i].d_userName;
                const int   UUID        = DATA[i].d_uuid;
                const int   USER_NUMBER = DATA[i].d_userNumber;
                const int   FIRM_NUMBER = DATA[i].d_firmNumber;
                const char *LOGIN       = DATA[i].d_login;
                const char *PASSWORD    = DATA[i].d_password;

                int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                          UUID, USER_NUMBER, FIRM_NUMBER,
                                          LOGIN, strlen(LOGIN),
                                          PASSWORD, strlen(PASSWORD));
                LOOP2_ASSERT(LINE, result, 0 == result);
            }

            // verify that the data was added properly
            for (i = 0; i < NUM_DATA; ++i) {
                const int       LINE        = DATA[i].d_lineNum;
                const string    USER_NAME   = DATA[i].d_userName;
                const int       UUID        = DATA[i].d_uuid;
                const int       USER_NUMBER = DATA[i].d_userNumber;
                const int       FIRM_NUMBER = DATA[i].d_firmNumber;
                const string    LOGIN       = DATA[i].d_login;
                const string    PASSWORD    = DATA[i].d_password;

                LOOP3_ASSERT(LINE,
                             USER_NAME,
                             X.view().theList(i).theString(FIELD_USER_NAME),
                             USER_NAME ==
                             X.view().theList(i).theString(FIELD_USER_NAME));

                LOOP3_ASSERT(LINE,
                             UUID,   X.view().theList(i).theInt(FIELD_UUID),
                             UUID == X.view().theList(i).theInt(FIELD_UUID));

                LOOP3_ASSERT(LINE,
                             USER_NUMBER,
                             X.view().theList(i).theInt(FIELD_USER_NUMBER),
                             USER_NUMBER ==
                             X.view().theList(i).theInt(FIELD_USER_NUMBER));

                LOOP3_ASSERT(LINE,
                             FIRM_NUMBER,
                             X.view().theList(i).theInt(FIELD_FIRM_NUMBER),
                             FIRM_NUMBER ==
                             X.view().theList(i).theInt(FIELD_FIRM_NUMBER));

                LOOP3_ASSERT(LINE,
                             LOGIN,
                             X.view().theList(i).theString(FIELD_LOGIN),
                             LOGIN ==
                             X.view().theList(i).theString(FIELD_LOGIN));

                LOOP3_ASSERT(LINE,
                             PASSWORD,
                             X.view().theList(i).theString(FIELD_PASSWORD),
                             PASSWORD ==
                             X.view().theList(i).theString(FIELD_PASSWORD));
            }
        }

        if (veryVerbose) cout << "\tb. Testing with invalid data." << endl;
        {
            // Make sure that invalid records cannot be added to the database.
            // This means UUID and user number must be more than zero, and the
            // login ID cannot be empty.

            static const struct {
                int         d_lineNum;               // source line number
                const char *d_userName;              // user name
                int         d_uuid;                  // uuid
                int         d_userNumber;            // user number
                int         d_firmNumber;            // firm number
                const char *d_login;                 // login ID
                const char *d_password;              // password
                int         d_expectedReturn;        // expected return value
            } INVALID_DATA[] = {
                {
                    L_,                              // line number
                    "user name",                     // user name
                    1,                               // uuid
                    1,                               // user number
                    1,                               // password
                    "login1",                        // login ID
                    "password",                      // password
                    baedb_UserDb::SUCCESS            // no error (control)
                },
#if 0
                {
                    L_,                              // line number
                    "user name",                     // user name
                    0,                               // uuid
                    1,                               // user number
                    1,                               // password
                    "login1",                        // login ID
                    "password",                      // password
                    baedb_TestUserDb::INVALID_BOUNDS // invalid uuid
                },
                {
                    L_,                              // line number
                    "user name",                     // user name
                    1,                               // uuid
                    0,                               // user number
                    1,                               // password
                    "login1",                        // login ID
                    "password",                      // password
                    baedb_TestUserDb::INVALID_BOUNDS // invalid user number
                },
                {
                    L_,                              // line number
                    "user name",                     // user name
                    1,                               // uuid
                    1,                               // user number
                    1,                               // password
                    "",                              // login ID
                    "password",                      // password
                    baedb_TestUserDb::INVALID_BOUNDS // invalid login ID
                },
#endif
            };
            const int NUM_INVALID_DATA
                        = sizeof INVALID_DATA / sizeof *INVALID_DATA;

            for (int i = 0; i < NUM_INVALID_DATA; ++i) {
                const int   LINE            = INVALID_DATA[i].d_lineNum;
                const char *USER_NAME       = INVALID_DATA[i].d_userName;
                const int   UUID            = INVALID_DATA[i].d_uuid;
                const int   USER_NUMBER     = INVALID_DATA[i].d_userNumber;
                const int   FIRM_NUMBER     = INVALID_DATA[i].d_firmNumber;
                const char *LOGIN           = INVALID_DATA[i].d_login;
                const char *PASSWORD        = INVALID_DATA[i].d_password;
                const int   EXPECTED_RETURN = INVALID_DATA[i].d_expectedReturn;

                Obj mX;
                int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                          UUID, USER_NUMBER, FIRM_NUMBER,
                                          LOGIN, strlen(LOGIN),
                                          PASSWORD, strlen(PASSWORD));
                LOOP3_ASSERT(LINE, EXPECTED_RETURN,   result,
                                   EXPECTED_RETURN == result);
                if (EXPECTED_RETURN < 0) {
                    LOOP2_ASSERT(LINE, mX.numRecords(), 0 == mX.numRecords());
                }
            }
        }

        if (veryVerbose) cout << "\tc. Testing with duplicate data." << endl;
        {
            // Call the 'addRecord' method using valid input data, but some
            // records have duplicated unique fields.  Make sure that the
            // duplicated record is not added to the database and the
            // appropriate value is returned.

            static const struct {
                int         d_lineNum;          // source line number
                const char *d_userName;         // user name
                int         d_uuid;             // uuid
                int         d_userNumber;       // user number
                int         d_firmNumber;       // firm number
                const char *d_login;            // login ID
                const char *d_password;         // password
                int         d_expectedReturn;   // expected return value
            } DUPLICATE_DATA[] = {
                {
                    L_,                             // line number
                    "user",                         // user name
                    1,                              // uuid
                    10,                             // user number
                    928,                            // firm number
                    "login1",                       // login ID
                    "password",                     // password
                    baedb_UserDb::SUCCESS           // first record
                },
                {
                    L_,                             // line number
                    "user",                         // user name
                    1,                              // uuid
                    20,                             // user number
                    928,                            // firm number
                    "login2",                       // login ID
                    "password",                     // password
                    baedb_TestUserDb::NONUNIQUE_KEY // same uuid
                },
                {
                    L_,                             // line number
                    "user",                         // user name
                    2,                              // uuid
                    10,                             // user number
                    928,                            // firm number
                    "login2",                       // login ID
                    "password",                     // password
                    baedb_TestUserDb::NONUNIQUE_KEY // same user number
                },
                {
                    L_,                             // line number
                    "user",                         // user name
                    2,                              // uuid
                    20,                             // user number
                    928,                            // firm number
                    "login1",                       // login ID
                    "password",                     // password
                    baedb_TestUserDb::NONUNIQUE_KEY // same login ID
                },
                {
                    L_,                             // line number
                    "user",                         // user name
                    2,                              // uuid
                    20,                             // user number
                    928,                            // firm number
                    "login2",                       // login ID
                    "password",                     // password
                    baedb_UserDb::SUCCESS           // new record, no clash
                },
                {
                    L_,                             // line number
                    "user",                         // user name
                    1,                              // uuid
                    30,                             // user number
                    928,                            // firm number
                    "login3",                       // login ID
                    "password",                     // password
                    baedb_TestUserDb::NONUNIQUE_KEY // UUID clash
                },
                {
                    L_,                             // line number
                    "user",                         // user name
                    2,                              // uuid
                    30,                             // user number
                    928,                            // firm number
                    "login3",                       // login ID
                    "password",                     // password
                    baedb_TestUserDb::NONUNIQUE_KEY // UUID clash
                },
                {
                    L_,                             // line number
                    "user",                         // user name
                    3,                              // uuid
                    10,                             // user number
                    928,                            // firm number
                    "login3",                       // login ID
                    "password",                     // password
                    baedb_TestUserDb::NONUNIQUE_KEY // user number clash
                },
                {
                    L_,                             // line number
                    "user",                         // user name
                    3,                              // uuid
                    20,                             // user number
                    928,                            // firm number
                    "login3",                       // login ID
                    "password",                     // password
                    baedb_TestUserDb::NONUNIQUE_KEY // user number clash
                },
                {
                    L_,                             // line number
                    "user",                         // user name
                    3,                              // uuid
                    30,                             // user number
                    928,                            // firm number
                    "login1",                       // login ID
                    "password",                     // password
                    baedb_TestUserDb::NONUNIQUE_KEY // login ID clash
                },
                {
                    L_,                             // line number
                    "user",                         // user name
                    3,                              // uuid
                    30,                             // user number
                    928,                            // firm number
                    "login2",                       // login ID
                    "password",                     // password
                    baedb_TestUserDb::NONUNIQUE_KEY // login ID clash
                },
                {
                    L_,                             // line number
                    "user",                         // user name
                    3,                              // uuid
                    30,                             // user number
                    928,                            // firm number
                    "login3",                       // login ID
                    "password",                     // password
                    baedb_UserDb::SUCCESS           // new record, no clash
                },
            };
            const int NUM_DUPLICATE_DATA
                        = sizeof DUPLICATE_DATA / sizeof *DUPLICATE_DATA;

            Obj mX; const Obj& X = mX;
            int numRecords = 0;

            for (int i = 0; i < NUM_DUPLICATE_DATA; ++i) {
                const int   LINE        = DUPLICATE_DATA[i].d_lineNum;
                const char *USER_NAME   = DUPLICATE_DATA[i].d_userName;
                const int   UUID        = DUPLICATE_DATA[i].d_uuid;
                const int   USER_NUMBER = DUPLICATE_DATA[i].d_userNumber;
                const int   FIRM_NUMBER = DUPLICATE_DATA[i].d_firmNumber;
                const char *LOGIN       = DUPLICATE_DATA[i].d_login;
                const char *PASSWORD    = DUPLICATE_DATA[i].d_password;
                const int   EXPECTED_RETURN
                                        = DUPLICATE_DATA[i].d_expectedReturn;

                if (EXPECTED_RETURN == baedb_UserDb::SUCCESS) {
                    numRecords++;
                }

                int result = mX.addRecord(USER_NAME, strlen(USER_NAME),
                                          UUID, USER_NUMBER, FIRM_NUMBER,
                                          LOGIN, strlen(LOGIN),
                                          PASSWORD, strlen(PASSWORD));
                LOOP3_ASSERT(LINE, EXPECTED_RETURN,   result,
                                   EXPECTED_RETURN == result);
                LOOP3_ASSERT(LINE, X.numRecords(),   numRecords,
                                   X.numRecords() == numRecords);
            }
        }

        if (verbose) cout << "\nEnd of 'addRecord' Method Test." << endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //   This test will verify that the primary manipulators are working as
        //   expected.
        //
        // Concerns:
        //   When a valid input string is passed to the 'addRecords' method,
        //   the record contained in the input string should be added.  Also,
        //   the default constructor should create an object with no records.
        //   We also need to make sure that the destructor works.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, verify that the 'addRecords' member function works by
        //   constructing a series of independent objects using the default
        //   constructor and running 'addRecords' using increasing number of
        //   records.  Verify the contents of the object using the basic
        //   accessor.
        //
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   baedb_TestUserDb(bslma_Allocator *basicAllocator = 0);
        //   virtual ~baedb_TestUserDb();
        //   BOOTSTRAP: int addRecords(const char *inputRecords);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            const bdem_List defaultDb;
            Obj mX; const Obj& X = mX;
            if (veryVerbose) { T_ P(X); }
            ASSERT(defaultDb == X.view());
        }

        static const struct {
            int         d_lineNum;    // source line number
            const char *d_userName;   // user name
            int         d_uuid;       // uuid
            int         d_userNumber; // user number
            int         d_firmNumber; // firm number
            const char *d_login;      // login
            const char *d_password;   // password
        } DATA[] = {
            {
                L_,                   // source line number
                "user name 1",        // user name
                1,                    // uuid
                1,                    // user number
                1,                    // firm number
                "login1",             // login
                "password1"           // password
            },
            {
                L_,                   // source line number
                "user name 2",        // user name
                2,                    // uuid
                2,                    // user number
                2,                    // firm number
                "login2",             // login
                "password2"           // password
            },
            {
                L_,                   // source line number
                "user name 3",        // user name
                3,                    // uuid
                3,                    // user number
                3,                    // firm number
                "login3",             // login
                "password3"           // password
            },
            {
                L_,                   // source line number
                "user name 4",        // user name
                4,                    // uuid
                4,                    // user number
                4,                    // firm number
                "login4",             // login
                "password4"           // password
            },
            {
                L_,                   // source line number
                "user name 5",        // user name
                5,                    // uuid
                5,                    // user number
                5,                    // firm number
                "login5",             // login
                "password5"           // password
            },
        };

        const int   SIZE = 2048;
        char        src[SIZE];
        const char *SRC = src;

        int       i = 0;
        bdem_List db;

        if (verbose) cout << "\nTesting 'addRecords'." << endl;
        {
            if (veryVerbose) cout << "\tUsing data with length 0." << endl;

            listToChar(src, SIZE, db);
            Obj mX; const Obj& X = mX;
            mX.addRecords(SRC);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.view() == db);
        }

        {
            if (veryVerbose) cout << "\tUsing data with length 1." << endl;

            bdem_List rec;
            rec.appendString(DATA[i].d_userName);
            rec.appendInt(DATA[i].d_uuid);
            rec.appendInt(DATA[i].d_userNumber);
            rec.appendInt(DATA[i].d_firmNumber);
            rec.appendString(DATA[i].d_login);
            rec.appendString(DATA[i].d_password);
            ++i;
            db.appendList(rec);

            listToChar(src, SIZE, db);
            Obj mX; const Obj& X = mX;
            mX.addRecords(SRC);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.view() == db);
        }

        {
            if (veryVerbose) cout << "\tUsing src with length 2." << endl;

            bdem_List rec;
            rec.appendString(DATA[i].d_userName);
            rec.appendInt(DATA[i].d_uuid);
            rec.appendInt(DATA[i].d_userNumber);
            rec.appendInt(DATA[i].d_firmNumber);
            rec.appendString(DATA[i].d_login);
            rec.appendString(DATA[i].d_password);
            ++i;
            db.appendList(rec);

            listToChar(src, SIZE, db);
            Obj mX; const Obj& X = mX;
            mX.addRecords(SRC);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.view() == db);
        }

        {
            if (veryVerbose) cout << "\tUsing src with length 3." << endl;

            bdem_List rec;
            rec.appendString(DATA[i].d_userName);
            rec.appendInt(DATA[i].d_uuid);
            rec.appendInt(DATA[i].d_userNumber);
            rec.appendInt(DATA[i].d_firmNumber);
            rec.appendString(DATA[i].d_login);
            rec.appendString(DATA[i].d_password);
            ++i;
            db.appendList(rec);

            listToChar(src, SIZE, db);
            Obj mX; const Obj& X = mX;
            mX.addRecords(SRC);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.view() == db);
        }

        {
            if (veryVerbose) cout << "\tUsing src with length 4." << endl;

            bdem_List rec;
            rec.appendString(DATA[i].d_userName);
            rec.appendInt(DATA[i].d_uuid);
            rec.appendInt(DATA[i].d_userNumber);
            rec.appendInt(DATA[i].d_firmNumber);
            rec.appendString(DATA[i].d_login);
            rec.appendString(DATA[i].d_password);
            ++i;
            db.appendList(rec);

            listToChar(src, SIZE, db);
            Obj mX; const Obj& X = mX;
            mX.addRecords(SRC);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.view() == db);
        }

        {
            if (veryVerbose) cout << "\tUsing src with length 5." << endl;

            bdem_List rec;
            rec.appendString(DATA[i].d_userName);
            rec.appendInt(DATA[i].d_uuid);
            rec.appendInt(DATA[i].d_userNumber);
            rec.appendInt(DATA[i].d_firmNumber);
            rec.appendString(DATA[i].d_login);
            rec.appendString(DATA[i].d_password);
            ++i;
            db.appendList(rec);

            listToChar(src, SIZE, db);
            Obj mX; const Obj& X = mX;
            mX.addRecords(SRC);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.view() == db);
        }

        if (verbose) cout << "\nEnd of Primary Manipulators Test." << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //   We want to demonstrate a base-line level of correct operation of
        //   the following methods and operators:
        //     - default and copy constructors.
        //     - the assignment operator (including aliasing).
        //     - equality operators: 'operator==' and 'operator!='.
        //     - primary manipulators: 'addRecords' and 'removeAll'.
        //     - basic accessors: 'view'.
        //
        // Plan:
        //   Create four test objects using the default, initializing, and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using the test objects.  Invoke the primary
        //   manipulator [5, 6, 7], copy constructor [2, 4], assignment
        //   operator without [8, 9] and with [10] aliasing.  Use the basic
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB' and 'VC' denote
        //   unique, but otherwise arbitrary, object values, while 'U' denotes
        //   the valid, but "unknown", default object value.
        //
        //    1. Create an object x1 (init. to VA)  { x1:VA                  }
        //    2. Create an object x2 (copy of x1)   { x1:VA x2:VA            }
        //    3. Create an object x3 (default ctor) { x1:VA x2:VA x3:U       }
        //    4. Create an object x4 (copy of x3)   { x1:VA x2:VA x3:U  x4:U }
        //    5. Set x3 using 'addRecords' (set to VB)
        //                                          { x1:VA x2:VA x3:VB x4:U }
        //    6. Change x1 using 'removeAll'        { x1:U  x2:VA x3:VB x4:U }
        //    7. Change x1 ('addRecords', set to VC)
        //                                          { x1:VC x2:VA x3:VB x4:U }
        //    8. Assign x2 = x1                     { x1:VC x2:VC x3:VB x4:U }
        //    9. Assign x2 = x3                     { x1:VC x2:VB x3:VB x4:U }
        //   10. Assign x1 = x1 (aliasing)          { x1:VC x2:VB x3:VB x4:U }
        //
        //
        // Testing:
        //   This test case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBreathing Test"
                          << "\n==============" << endl;

        static const struct {
            int         d_lineNum;    // source line number
            const char *d_userName;   // user name
            int         d_uuid;       // uuid
            int         d_userNumber; // user number
            int         d_firmNumber; // firm number
            const char *d_login;      // login
            const char *d_password;   // password
        } DATA[] = {
            {
                L_,                   // source line number
                "user name 1",        // user name
                1,                    // uuid
                1,                    // user number
                1,                    // firm number
                "login1",             // login
                "password1"           // password
            },
            {
                L_,                   // source line number
                "user name 2",        // user name
                2,                    // uuid
                2,                    // user number
                2,                    // firm number
                "login2",             // login
                "password2"           // password
            },
            {
                L_,                   // source line number
                "user name 3",        // user name
                3,                    // uuid
                3,                    // user number
                3,                    // firm number
                "login3",             // login
                "password3"           // password
            },
        };

        const int SIZE = 256;
        char      sa[SIZE],
                  sb[SIZE],
                  sc[SIZE];

        const char *USER_NAMEA   = DATA[0].d_userName,
                   *USER_NAMEB   = DATA[1].d_userName,
                   *USER_NAMEC   = DATA[2].d_userName;
        const int   UUIDA        = DATA[0].d_uuid,
                    UUIDB        = DATA[1].d_uuid,
                    UUIDC        = DATA[2].d_uuid;
        const int   USER_NUMBERA = DATA[0].d_userNumber,
                    USER_NUMBERB = DATA[1].d_userNumber,
                    USER_NUMBERC = DATA[2].d_userNumber;
        const int   FIRM_NUMBERA = DATA[0].d_firmNumber,
                    FIRM_NUMBERB = DATA[1].d_firmNumber,
                    FIRM_NUMBERC = DATA[2].d_firmNumber;
        const char *LOGINA       = DATA[0].d_login,
                   *LOGINB       = DATA[1].d_login,
                   *LOGINC       = DATA[2].d_login;
        const char *PASSWORDA    = DATA[0].d_password,
                   *PASSWORDB    = DATA[1].d_password,
                   *PASSWORDC    = DATA[2].d_password;
        const char *SA           = sa,
                   *SB           = sb,
                   *SC           = sc;

        bdem_List db, rec;

        rec.appendString(USER_NAMEA);
        rec.appendInt(UUIDA);
        rec.appendInt(USER_NUMBERA);
        rec.appendInt(FIRM_NUMBERA);
        rec.appendString(LOGINA);
        rec.appendString(PASSWORDA);
        db.appendList(rec);
        const bdem_List CA = db;

        rec.removeAll();
        rec.appendString(USER_NAMEB);
        rec.appendInt(UUIDB);
        rec.appendInt(USER_NUMBERB);
        rec.appendInt(FIRM_NUMBERB);
        rec.appendString(LOGINB);
        rec.appendString(PASSWORDB);
        db.removeAll();
        db.appendList(rec);
        const bdem_List CB = db;

        rec.removeAll();
        rec.appendString(USER_NAMEC);
        rec.appendInt(UUIDC);
        rec.appendInt(USER_NUMBERC);
        rec.appendInt(FIRM_NUMBERC);
        rec.appendString(LOGINC);
        rec.appendString(PASSWORDC);
        db.removeAll();
        db.appendList(rec);
        const bdem_List CC = db;

        const bdem_List CU; // default is empty list

        listToChar(sa, SIZE, CA);
        listToChar(sb, SIZE, CB);
        listToChar(sc, SIZE, CC);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(SA);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        LOOP3_ASSERT(SA, CA, X1.view(), CA == X1.view());

        if (verbose) cout << "\tb. Try equality operators: x1 <op> x1."
                          << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy of x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t'; P(X2); }

        if (verbose) cout << "\ta. Check initial state of x2." << endl;
        ASSERT(CA == X2.view());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Create an object x3 (default ctor)."
                             "\t\t{ x1:VA x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (verbose) { cout << '\t'; P(X3); }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(CU == X3.view());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create an object x4 (copy of x3)."
                             "\t\t{ x1:VA x2:VA x3:U x4:U }" << endl;
        Obj mX4;  const Obj& X4 = mX4;
        if (verbose) { cout << '\t'; P(X4); }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(CU == X4.view());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x3 using 'addRecords' (set to VB)."
                             "\t\t{ x1:VA x2:VA x3:VB x4:U }" << endl;
        mX3.addRecords(SB);
        if (verbose) { cout << '\t'; P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(CB == X3.view());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Change x1 using 'removeAll'."
                             "\t\t{ x1:U x2:VA x3:VB x4:U }" << endl;
        mX1.removeAll();
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(CU == X1.view());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Change x1 ('addRecords', set to VC)."
                             "\t\t{ x1:VC x2:VA x3:VB x4:U }" << endl;
        mX1.addRecords(SC);
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(CC == X1.view());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x1."
                             "\t\t{ x1:VC x2:VC x3:VB x4:U }" << endl;
        mX2 = X1;
        if (verbose) { cout << '\t'; P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(CC == X2.view());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x3."
                             "\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX2 = X3;
        if (verbose) { cout << '\t'; P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(CB == X2.view());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Assign x1 = x1."
                             "\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX1 = X1;
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(CC == X1.view());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        if (verbose) cout << "\nEnd of Breathing Test." << endl;
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
