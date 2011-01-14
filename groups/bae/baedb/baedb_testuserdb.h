// baedb_testuserdb.h   -*-C++-*-
#ifndef INCLUDED_BAEDB_TESTUSERDB
#define INCLUDED_BAEDB_TESTUSERDB

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a concrete Bloomberg User Database simulator.
//
//@DEPRECATED: Use 'bsiudb_testuserdb' (and 'bsiudb_userdb') instead.
//
//@CLASSES:
//  baedb_TestUserDb: implementation of 'baedb_UserDb' protocol for testing
//
//@SEE_ALSO: bsiudb_testuserdb
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: [!DEPRECATED] This component provides a class,
// 'baedb_TestUserDb', that implements the 'baedb_UserDb' protocol.  This
// component has been deprecated, clients should instead use 'bsiudb_userdb'
// and 'bsiudb_testuserdb'.
//..
//                  ( baedb_TestUserDb )
//                           |          maxSupportedBdexVersion
//                           |          ctors
//                           |          operator=
//                           |          addRecord
//                           |          addRecords
//                           |          bdexStreamIn
//                           |          removeAll
//                           |          removeByLogin
//                           |          removeByUserNumber
//                           |          removeByUUID
//                           |          bdexStreamOut
//                           |          numRecords
//                           |          print
//                           |          view
//                           V
//                    ( baedb_UserDb )
//                                      dtor
//                                      lookupFirmNumberByLogin
//                                      lookupFirmNumberByUserNumber
//                                      lookupFirmNumberByUUID
//                                      lookupLoginByUserNumber
//                                      lookupLoginByUUID
//                                      lookupUserNameByLogin
//                                      lookupUserNameByUserNumber
//                                      lookupUserNameByUUID
//                                      lookupUserNumberByLogin
//                                      lookupUserNumberByUUID
//                                      lookupUUIDByLogin
//                                      lookupUUIDByUserNumber
//..
// The 'baedb_TestUserDb' class simulates a Bloomberg User Database and is
// suitable for use in a test environment.
//
// The external view of a user record is as a 'bdem_List' that conforms to the
// following schema:
//..
//    +------------------------------------+
//    |{                                   |
//    |    RECORD "USER" {                 |
//    |        STRING      "User Name";    |
//    |        INT         "UUID";         |
//    |        INT         "User Number";  |
//    |        INT         "Firm Number";  |
//    |        STRING      "Login ID";     |
//    |        STRING      "Password";     |
//    |    }                               |
//    |}                                   |
//    +------------------------------------+
//           Fig. 1 -- "USER" Schema
//..
// A "USER" record is valid if "UUID" and "User Number" are greater than 0
// and "Login ID" is not the empty string ("UUID", "User Number", and
// "Login ID" are each primary keys).  Refer to the 'baedb_userdb'
// component-level documentation for more information on the fields in the
// user database.
//
// All occurrences of 'bdem_List' in the interface of 'baedb_TestUserDb' are
// assumed to contain elements that conform to the schema in Figure 1.  User
// records can also be defined in terms the 'bdempu_aggregate' '<LIST>'
// specification.  This feature is illustrated in the Usage example.
//
// Note that this implementation assumes case-insensitivity for "Login ID".
//
//
///Usage
///-----
// The following snippets of code illustrate basic use of 'baedb_TestUserDb'.
// This example uses the 'INPUT_DATA' below that is written in the language
// specified for '<LIST>' in the 'bdempu_aggregate' component-level
// documentation:
//..
//  const char INPUT_DATA[] =
//      "{\n"
//          "LIST {\n"
//              "STRING  \"User Name 0\",\n"
//              "INT     12345,          // uuid\n"
//              "INT     23456,          // user number\n"
//              "INT     45678,          // firm number\n"
//              "STRING  \"login0\",\n"
//              "STRING  \"password0\"\n"
//          "},\n"
//          "LIST {\n"
//              "STRING  \"User Name 1\",\n"
//              "INT     34567,          // uuid\n"
//              "INT     65432,          // user number\n"
//              "INT     92836,          // firm number\n"
//              "STRING  \"login1\",\n"
//              "STRING  \"password1\"\n"
//          "},\n"
//          "LIST {\n"
//              "STRING  \"User Name 2\",\n"
//              "INT     82621,          // uuid\n"
//              "INT     27653,          // user number\n"
//              "INT     45675,          // firm number\n"
//              "STRING  \"login2\",\n"
//              "STRING  \"password2\"\n"
//          "}\n"
//      "}\n";
//..
// 'INPUT_DATA' describes a sample user database containing three users.  We
// will load this data into a 'baedb_TestUserDb' object using the following
// method:
//..
//  void loadRecords(baedb_TestUserDb *userDb, const char *inputRecords)
//      // Parse the specified 'inputRecords' and store them in the
//      // specified 'userDb'.
//  {
//      int retCode = userDb->addRecords(inputRecords);
//      assert(3 == retCode);
//  }
//..
// We define a 'baedb_TestUserDb' object, 'myDb', and call our 'loadRecords'
// method to populate 'myDb' as follows:
//..
//  baedb_TestUserDb myDb;
//  loadRecords(&myDb, INPUT_DATA);
//..
// The database can now be queried using the lookup functions declared in the
// 'baedb_UserDb' protocol:
//..
//  void verifyRecords(const baedb_TestUserDb *userDb)
//      // Verify that the specified 'userDb' contains the records as specified
//      // in 'INPUT_DATA'.
//  {
//      bsl::string userName;
//      bsl::string login;
//      int         uuid = 12345;
//      int         userNumber;
//      int         firmNumber;
//      int         len;
//      int         retCode;
//
//      retCode = userDb->lookupUserNameByUUID(&userName, uuid);
//      assert(baedb_UserDb::SUCCESS == retCode);
//      assert("User Name 0"         == userName);
//
//      retCode = userDb->lookupUserNumberByUUID(&userNumber, uuid);
//      assert(baedb_UserDb::SUCCESS == retCode);
//      assert(23456                 == userNumber);
//
//      retCode = userDb->lookupFirmNumberByUUID(&firmNumber, uuid);
//      assert(baedb_UserDb::SUCCESS == retCode);
//      assert(45678                 == firmNumber);
//
//      retCode = userDb->lookupLoginByUUID(&login, uuid);
//      assert(baedb_UserDb::SUCCESS == retCode);
//      assert("login0"              == login);
//  }
//..
// The 'baedb_TestUserDb' class also provides 'addRecord' and 'remove' methods
// for manipulating individual user records.  The following function
// illustrates their use.  Note that the entire contents of the database can be
// examined using the 'view' accessor:
//..
//  void manipulate(baedb_TestUserDb *userDb)
//      // Illustrate the 'addRecord' and 'removeByLogin' methods.
//  {
//      const char *userName3   = "User Name 3";
//      const char *userName4   = "User Name 4";
//      const char *login3      = "login3";
//      const char *login4      = "login4";
//      const char *password3   = "password3";
//      const char *password4   = "password4";
//      const int   LOGIN_INDEX = 4;
//
//      userDb->addRecord(userName3,
//                        strlen(userName3),
//                        28172,
//                        19283,
//                        21772,
//                        login3,
//                        strlen(login3),
//                        password3,
//                        strlen(password3));
//      userDb->addRecord(userName4,
//                        strlen(userName4),
//                        32356,
//                        54333,
//                        33333,
//                        login4,
//                        strlen(login4),
//                        password4,
//                        strlen(password4));
//      assert(5 == userDb->numRecords());
//
//      userDb->removeByLogin(login3, strlen(login3));
//      assert(4        == userDb->view().length());
//      assert("login2" == userDb->view().theList(2).theString(LOGIN_INDEX));
//      assert(login4   == userDb->view().theList(3).theString(LOGIN_INDEX));
//  }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEDB_USERDB
#include <baedb_userdb.h>
#endif

#ifndef INCLUDED_BDEM_LIST
#include <bdem_list.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif


namespace BloombergLP {

                        // ======================
                        // class baedb_TestUserDb
                        // ======================

class baedb_TestUserDb : public baedb_UserDb {
    // [!DEPRECATED!] This 'class' provides a simulator of a Bloomberg User
    // Database suitable for use in a testing environment.  The 'class'
    // implements the 'baedb_UserDb' protocol and also defines suitable
    // constructors and manipulators for initializing and modifying the
    // contents of the database.
    //
    // This classs has been deprecated, clients should instead use the types
    // defined in 'bsiudb_userdb' and 'bsiudb_testuserdb'.
    //
    // This class supports a complete set of *value* *semantic* operations,
    // including copy construction, assignment, equality comparison, 'ostream'
    // printing, and 'bdex' serialization.  A precise operational definition of
    // when two instances have the same value can be found in the description
    // of 'operator==' for the class.  This class is *exception* *neutral* with
    // no guarantee of rollback: if an exception is thrown during the
    // invocation of a method on a pre-existing instance, the class is left in
    // a valid state, but its value is undefined.  In no event is memory
    // leaked.  Finally, *aliasing* (e.g., using all or part of an object as
    // both source and destination) is supported in all cases.

    // PRIVATE CONSTANTS
    enum {
        FIRM_TABLE = 0,
        USER_TABLE = 1
    };

    // PRIVATE DATA MEMBERS
    bdem_List d_db;  // internal representation of Bloomberg user database
                     // It contains 2 list elements:
                     //   - firm table
                     //   - user table

  public:
    // TYPES
    enum StatusCodes {
        // This enumeration defines the extended 'StatusCodes' that may be
        // returned from the methods in this class.  Note that methods may also
        // return 'baedb_UserDb::SUCCESS', 'baedb_UserDb::NOT_FOUND',
        // 'baedb_UserDb::INVALID_LOGIN', and 'baedb_UserDb::INVALID_PASSWORD'.

        NONUNIQUE_KEY = -1001,  // one of the key fields was not unique
        PARSING_ERROR = -1002   // there was a 'bdempu' parsing error
    };

  private:
    // PRIVATE CLASS METHODS
    static int hasUniqueUserKeys(const bdem_List& userTable);
        // Return 1 if the specified 'userDb' has unique keys, and return 0
        // otherwise.  A database has unique keys if it contains no two records
        // having the same User Number, the same UUID, or the same Login ID.
        // The behavior is undefined unless each element in 'userDb' is a
        // 'bdem_List' that satisfies the schema in Figure 1.

    static int hasUniqueFirmKeys(const bdem_List& firmTable);
        // TBD: doc

    static int isValidUserRecord(const bdem_List& record);
        // Return 1 if the specified 'record' satisfies the schema in Figure 1,
        // and that record.theInt(1) > 0, record.theInt(2) > 0, and
        // record.theString(4).length() > 0 (i.e., "UUID" and "User Number"
        // must be greater than 0, and "Login ID" cannot be the empty string);
        // return 0 otherwise.

    static int isValidFirmRecord(const bdem_List& record);
        // TBD: doc

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    baedb_TestUserDb(bslma_Allocator *basicAllocator = 0);
        // Create an empty user database.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // null, the currently installed default allocator is used.

    baedb_TestUserDb(const bdem_List&  inputUserRecords,
                     bslma_Allocator  *basicAllocator = 0);
        // Create a user database that contains the specified 'inputRecords'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is null, the currently installed default allocator
        // is used.  The behavior is undefined unless each element in
        // 'inputRecords' is a 'bdem_List' that satisfies the schema in Figure
        // 1, the contents of each user record is such that "UUID" and "User
        // Number" are greater than 0, and "Login ID" is not the empty string,
        // and the user records have unique keys.

    baedb_TestUserDb(const bdem_List&  inputFirmRecords,
                     const bdem_List&  inputUserRecords,
                     bslma_Allocator  *basicAllocator = 0);
        // Create a user database that contains the specified 'inputRecords'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is null, the currently installed default allocator
        // is used.  The behavior is undefined unless each element in
        // 'inputRecords' is a 'bdem_List' that satisfies the schema in Figure
        // 1, the contents of each user record is such that "UUID" and "User
        // Number" are greater than 0, and "Login ID" is not the empty string,
        // and the user records have unique keys.

    baedb_TestUserDb(const char      *inputUserRecords,
                     bslma_Allocator *basicAllocator = 0);
        // Create a user database containing the records defined in the
        // specified 'inputRecords' string; 'inputRecords' must contain records
        // written in the '<LIST>' specification in the 'bdempu_aggregate'
        // component-level documentation.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // null, the currently installed default allocator is used.  The
        // behavior is undefined unless each element in the '<LIST>' defined by
        // 'inputRecords' is a 'bdem_List' that satisfies the schema in Figure
        // 1; the contents of each user record is such that "UUID" and "User
        // Number" are greater than 0; "Login ID" is not the empty string; and
        // the user records have unique keys.
        //
        // DEPRECATED: Use the 'bdem_List' versions above.

    baedb_TestUserDb(const baedb_TestUserDb&  original,
                     bslma_Allocator         *basicAllocator = 0);
        // Create a user database having the same value as the specified
        // 'original' database.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is null, the currently installed
        // default allocator is used.

    virtual ~baedb_TestUserDb();
        // Destroy this user database.

    // MANIPULATORS
    const baedb_TestUserDb& operator=(const baedb_TestUserDb& rhs);
        // Assign to this user database the contents of the specified 'rhs'
        // database.

    int addRecord(const char *name,
                  int         nameLength,
                  int         uuid,
                  int         userNumber,
                  int         firmNumber,
                  const char *login,
                  int         loginLength,
                  const char *password,
                  int         passwordLength);
        // Add a record to this user database having the specified 'name' of
        // the specified 'nameLength', the specified 'uuid', the specified
        // 'userNumber', the specified 'firmNumber', the specified 'login' of
        // the specified 'loginLength', and the specified 'password' of the
        // specified 'passwordLength'.  Return 0 on success, -1001 if the
        // record would not have a unique key if it were added to this
        // database, and a more negative value otherwise.  The database is not
        // modified by an unsuccessful call.  The behavior is undefined unless
        // 0 <= nameLength, 0 < uuid, 0 < userNumber, 0 < loginLength, and
        // 0 <= passwordLength.
        //
        // DEPRECATED: Use 'addUserRecord' instead.

    int addFirmRecord(const char *firmName,
                      int         firmNameLength,
                      int         firmNumber);
        // TBD: doc

    int addUserRecord(const char *name,
                      int         nameLength,
                      int         uuid,
                      int         userNumber,
                      int         firmNumber,
                      const char *login,
                      int         loginLength,
                      const char *password,
                      int         passwordLength);
        // Add a record to this user database having the specified 'name' of
        // the specified 'nameLength', the specified 'uuid', the specified
        // 'userNumber', the specified 'firmNumber', the specified 'login' of
        // the specified 'loginLength', and the specified 'password' of the
        // specified 'passwordLength'.  Return 0 on success, -1001 if the
        // record would not have a unique key if it were added to this
        // database, and a more negative value otherwise.  The database is not
        // modified by an unsuccessful call.  The behavior is undefined unless
        // 0 <= nameLength, 0 < uuid, 0 < userNumber, 0 < loginLength, and
        // 0 <= passwordLength.

    int addRecords(const char *inputRecords);
        // Add the user records defined in the specified 'inputRecords' string
        // to this user database; 'inputRecords' must contain records written
        // in the '<LIST>' specification in the 'bdempu_aggregate'
        // component-level documentation.  Return the number of records added
        // on success, -1002 if there was an error parsing 'inputRecords', and
        // a more negative value otherwise.  The database is not modified by an
        // unsuccessful call.  A record is added to the database only if "UUID"
        // and "User Number" are greater than 0, "Login ID" is not the empty
        // string, and the record has a unique key.  The behavior is undefined
        // unless each element in the '<LIST>' defined by 'inputRecords' is a
        // 'bdem_List' that satisfies the schema in Figure 1.
        //
        // DEPRECATED: Use 'addUserRecords' instead.

    int addFirmRecords(const bdem_List& inputRecords);
        // TBD: doc

    int addUserRecords(const bdem_List& inputRecords);
        // TBD: doc

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If the
        // specified 'version' is not supported, 'stream' is marked invalid,
        // but this object is unaltered.  Note that no version is read from
        // 'stream'.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    void removeAll();
        // Remove all records from this database.

    void removeAllFirms();
        // Remove all firm records from this database.

    void removeAllUsers();
        // Remove all user records from this database.

    int removeByLogin(const char *login,
                      int         loginLength);
        // Remove the record having the specified 'login' of the specified
        // 'loginLength' from this user database.  Return 0 on success, -1 if
        // 'login' is not in this database, and a more negative value
        // otherwise.  The database is not modified by an unsuccessful call.
        // The behavior is undefined unless 0 < loginLength.
        //
        // DEPRECATED: Use 'removeUserByLogin' instead.

    int removeByUserNumber(int userNumber);
        // Remove the record having the specified 'userNumber' from this user
        // database.  Return 0 on success, -1 if 'userNumber' is not in this
        // database, and a more negative value otherwise.  The database is not
        // modified by an unsuccessful call.  The behavior is undefined unless
        // 0 < userNumber.
        //
        // DEPRECATED: Use 'removeUserByUserNumber' instead.

    int removeByUUID(int uuid);
        // Remove the record having the specified 'uuid' from this user
        // database.  Return 0 on success, -1 if 'uuid' is not in this
        // database, and a more negative value otherwise.  The database is not
        // modified by an unsuccessful call.  The behavior is undefined unless
        // 0 < uuid.
        //
        // DEPRECATED: Use 'removeUserByUUID' instead.

    int removeFirmByFirmNumber(int firmNumber);
        // Remove the record having the specified 'firmNumber' from the firm
        // table.  Return 0 on success, -1 if 'firmNumber' is not in this
        // database, and a more negative value otherwise.  The database is not
        // modified by an unsuccessful call.  The behavior is undefined unless
        // 0 < firmNumber.

    int removeUserByLogin(const char *login,
                          int         loginLength);
        // Remove the record having the specified 'login' of the specified
        // 'loginLength' from this user database.  Return 0 on success, -1 if
        // 'login' is not in this database, and a more negative value
        // otherwise.  The database is not modified by an unsuccessful call.
        // The behavior is undefined unless 0 < loginLength.

    int removeUserByUserNumber(int userNumber);
        // Remove the record having the specified 'userNumber' from this user
        // database.  Return 0 on success, -1 if 'userNumber' is not in this
        // database, and a more negative value otherwise.  The database is not
        // modified by an unsuccessful call.  The behavior is undefined unless
        // 0 < userNumber.

    int removeUserByUUID(int uuid);
        // Remove the record having the specified 'uuid' from this user
        // database.  Return 0 on success, -1 if 'uuid' is not in this
        // database, and a more negative value otherwise.  The database is not
        // modified by an unsuccessful call.  The behavior is undefined unless
        // 0 < uuid.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' and return a
        // reference to the modifiable 'stream'.  Optionally specify an
        // explicit 'version' format; by default, the maximum supported version
        // is written to 'stream' and used as the format.  If 'version' is
        // specified, that format is used, but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).

    virtual int lookupFirmNameByFirmNumber(bsl::string *name,
                                           int          firmNumber) const;
        // Load into the specified 'name' the name of the firm having the
        // specified 'firmNumber'.  Return 0 on success, -1 if 'firmNumber' is
        // not in this database, and a more negative value otherwise.  'name'
        // is not modified by an unsuccessful call.  The behavior is undefined
        // unless 0 < firmNumber.

    virtual int lookupFirmNumberByLogin(int        *firmNumber,
                                        const char *login,
                                        int         loginLength) const;
        // Load into the specified 'firmNumber' the firm number of the user
        // having the specified 'login' of the specified 'loginLength'.  Return
        // 0 on success, -1 if 'login' is not in this database, and a more
        // negative value otherwise.  'firmNumber' is not modified by an
        // unsuccessful call.  The behavior is undefined unless
        // 0 <= loginLength.  Note that 'login' need not be null-terminated.

    virtual int lookupFirmNumberByUserNumber(int *firmNumber,
                                             int  userNumber) const;
        // Load into the specified 'firmNumber' the firm number of the user
        // having the specified 'userNumber'.  Return 0 on success, -1 if
        // 'userNumber' is not in this database, and a more negative value
        // otherwise.  'firmNumber' is not modified by an unsuccessful call.
        // The behavior is undefined unless 0 < userNumber.

    virtual int lookupFirmNumberByUUID(int *firmNumber,
                                       int  uuid) const;
        // Load into the specified 'firmNumber' the firm number of the user
        // having the specified 'uuid'.  Return 0 on success, -1 if 'uuid' is
        // not in this database, and a more negative value otherwise.
        // 'firmNumber' is not modified by an unsuccessful call.  The behavior
        // is undefined unless 0 < uuid.

    virtual int lookupLoginByUserNumber(bsl::string *login,
                                        int          userNumber) const;
        // Load into the specified 'login' the login ID of the user having the
        // specified 'userNumber'.  Return 0 on success, -1 if 'userNumber' is
        // not in this database, and a more negative value otherwise.  'login'
        // is not modified by an unsuccessful call.  The behavior is undefined
        // unless 0 < userNumber.

    virtual int lookupLoginByUUID(bsl::string *login,
                                  int          uuid) const;
        // Load into the specified 'login' the login ID of the user having the
        // specified 'uuid'.  Return 0 on success, -1 if 'uuid' is not in this
        // database, and a more negative value otherwise.  'login' is not
        // modified by an unsuccessful call.  The behavior is undefined unless
        // 0 < uuid.

    virtual int lookupUserNameByLogin(bsl::string *name,
                                      const char  *login,
                                      int          loginLength) const;
        // Load into the specified 'name' the name of the user having the
        // specified 'login' of the specified 'loginLength'.  Return 0 on
        // success, -1 if 'login' is not in this database, and a more negative
        // value otherwise.  'name' is not modified by an unsuccessful call.
        // The behavior is undefined unless 0 <= loginLength.  Note that
        // 'login' need not be null-terminated.

    virtual int lookupUserNameByUserNumber(bsl::string *name,
                                           int          userNumber) const;
        // Load into the specified 'name' the name of the user having the
        // specified 'userNumber'.  Return 0 on success, -1 if 'userNumber' is
        // not in this database, and a more negative value otherwise.  'name'
        // is not modified by an unsuccessful call.  The behavior is undefined
        // unless 0 < userNumber.

    virtual int lookupUserNameByUUID(bsl::string *name,
                                     int          uuid) const;
        // Load into the specified 'name' the name of the user having the
        // specified 'uuid'.  Return 0 on success, -1 if 'uuid' is not in this
        // database, and a more negative value otherwise.  'name' is not
        // modified by an unsuccessful call.  The behavior is undefined unless
        // 0 < uuid.

    virtual int lookupUserNumberByLogin(int        *userNumber,
                                        const char *login,
                                        int         loginLength) const;
        // Load into the specified 'userNumber' the user number of the user
        // having the specified 'login' of the specified 'loginLength'.  Return
        // 0 on success, -1 if 'login' is not in this database, and a more
        // negative value otherwise.  'userNumber' is not modified by an
        // unsuccessful call.  The behavior is undefined unless
        // 0 <= loginLength.  Note that 'login' need not be null-terminated.

    virtual int lookupUserNumberByUUID(int *userNumber,
                                       int  uuid) const;
        // Load into the specified 'userNumber' the user number of the user
        // having the specified 'uuid'.  Return 0 on success, -1 if 'uuid' is
        // not in this database, and a more negative value otherwise.
        // 'userNumber' is not modified by an unsuccessful call.  The behavior
        // is undefined unless 0 < uuid.

    virtual int lookupUUIDByLogin(int        *uuid,
                                  const char *login,
                                  int         loginLength) const;
        // Load into the specified 'uuid' the UUID of the user having the
        // specified 'login' of the specified 'loginLength'.  Return 0 on
        // success, -1 if 'login' is not in this database, and a more negative
        // value otherwise.  'uuid' is not modified by an unsuccessful call.
        // The behavior is undefined unless 0 <= loginLength.  Note that
        // 'login' need not be null-terminated.

    virtual int lookupUUIDByUserNumber(int *uuid,
                                       int  userNumber) const;
        // Load into the specified 'uuid' the UUID of the user having the
        // specified 'userNumber'.  Return 0 on success, -1 if 'userNumber' is
        // not in this database, and a more negative value otherwise.  'uuid'
        // is not modified by an unsuccessful call.  The behavior is undefined
        // unless 0 < userNumber.

    int numRecords() const;
        // Return the number of records contained in this user database.
        //
        // DEPRECATED: Use 'numUsers' instead.

    int numFirms() const;
        // Return the number of firm records contained in this database.

    int numUsers() const;
        // Return the number of user records contained in this database.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this database object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level' and
        // return a reference to 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, suppress all indentation AND format the entire output on
        // one line.  If 'stream' is not valid on entry, this operation has no
        // effect.

    const bdem_List& view() const;
        // Return a reference to the non-modifiable user database managed by
        // this object.
        //
        // DEPRECATED: Use 'userView' instead.

    const bdem_List& firmView() const;
        // Return a reference to the non-modifiable firm database managed by
        // this object.

    const bdem_List& userView() const;
        // Return a reference to the non-modifiable user database managed by
        // this object.
};

// FREE OPERATORS
bool operator==(const baedb_TestUserDb& lhs, const baedb_TestUserDb& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' user databases have the
    // same value, and 'false' otherwise.  Two user databases have the same
    // value if they contain the same number of records and each record
    // contained in 'lhs' is also contained in 'rhs'.

inline
bool operator!=(const baedb_TestUserDb& lhs, const baedb_TestUserDb& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' databases do not have the
    // same value, and 'false' otherwise.  Two user databases do not have the
    // same value if they contain a differing number of records, or a record
    // contained in 'lhs' is not also contained in 'rhs'.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const baedb_TestUserDb& rhs);
    // Write to the specified output 'stream' the specified 'rhs' user database
    // and return a reference to the modifiable 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CLASS METHODS

inline
int baedb_TestUserDb::maxSupportedBdexVersion()
{
    return 2;
}

// CREATORS

inline
baedb_TestUserDb::baedb_TestUserDb(const baedb_TestUserDb&  original,
                                   bslma_Allocator         *basicAllocator)
: d_db(original.d_db, basicAllocator)
{
}

inline
baedb_TestUserDb::~baedb_TestUserDb()
{
}

// MANIPULATORS

inline
const baedb_TestUserDb&
baedb_TestUserDb::operator=(const baedb_TestUserDb& rhs)
{
    if (&rhs != this) {
        d_db = rhs.d_db;
    }
    return *this;
}

inline
int baedb_TestUserDb::addRecord(const char *name,
                                int         nameLength,
                                int         uuid,
                                int         userNumber,
                                int         firmNumber,
                                const char *login,
                                int         loginLength,
                                const char *password,
                                int         passwordLength)
    // DEPRECATED
{
    return addUserRecord(name,
                         nameLength,
                         uuid,
                         userNumber,
                         firmNumber,
                         login,
                         loginLength,
                         password,
                         passwordLength);
}

template <class STREAM>
inline
STREAM& baedb_TestUserDb::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 2: {
            bdem_List userDb;
            stream >> userDb;
            if (!stream) {
                return stream;
            }

            if (2                        != userDb.length()
             || bdem_ElemType::BDEM_LIST != userDb.elemType(FIRM_TABLE)
             || bdem_ElemType::BDEM_LIST != userDb.elemType(USER_TABLE)) {
                stream.invalidate();
                return stream;
            }

            {
                // Verify firm table.

                const bdem_List& firmTable = userDb.theList(FIRM_TABLE);

                const int len = firmTable.length();
                for (int i = 0; i < len; ++i) {
                    if (bdem_ElemType::BDEM_LIST != firmTable.elemType(i)) {
                        stream.invalidate();
                        return stream;
                    }
                    if (!isValidFirmRecord(firmTable.theList(i))) {
                        stream.invalidate();
                        return stream;
                    }
                }
                if (!hasUniqueFirmKeys(firmTable)) {
                    stream.invalidate();
                    return stream;
                }
            }
            {
                // Verify user table.

                const bdem_List& userTable = userDb.theList(USER_TABLE);

                const int len = userTable.length();
                for (int i = 0; i < len; ++i) {
                    if (bdem_ElemType::BDEM_LIST != userTable.elemType(i)) {
                        stream.invalidate();
                        return stream;
                    }
                    if (!isValidUserRecord(userTable.theList(i))) {
                        stream.invalidate();
                        return stream;
                    }
                }
                if (!hasUniqueUserKeys(userTable)) {
                    stream.invalidate();
                    return stream;
                }
            }

            d_db = userDb;
          } break;
          case 1: {
            bdem_List userTable;
            stream >> userTable;
            if (!stream) {
                return stream;
            }

            const int len = userTable.length();
            for (int i = 0; i < len; ++i) {
                if (bdem_ElemType::BDEM_LIST != userTable.elemType(i)) {
                    stream.invalidate();
                    return stream;
                }
                if (!isValidUserRecord(userTable.theList(i))) {
                    stream.invalidate();
                    return stream;
                }
            }
            if (!hasUniqueUserKeys(userTable)) {
                stream.invalidate();
                return stream;
            }

            removeAllFirms();
            d_db.theList(USER_TABLE) = userTable;
          } break;
          default: {
            stream.invalidate();
          } break;
        }
    }
    return stream;
}

inline
void baedb_TestUserDb::removeAll()
{
    removeAllFirms();
    removeAllUsers();
}

inline
void baedb_TestUserDb::removeAllFirms()
{
    d_db.theModifiableList(FIRM_TABLE).removeAll();
}

inline
void baedb_TestUserDb::removeAllUsers()
{
    d_db.theModifiableList(USER_TABLE).removeAll();
}

inline
int baedb_TestUserDb::removeByLogin(const char *login,
                                    int         loginLength)
    // DEPRECATED
{
    return removeUserByLogin(login, loginLength);
}

inline
int baedb_TestUserDb::removeByUserNumber(int userNumber)
    // DEPRECATED
{
    return removeUserByUserNumber(userNumber);
}

inline
int baedb_TestUserDb::removeByUUID(int uuid)
    // DEPRECATED
{
    return removeUserByUUID(uuid);
}

// ACCESSORS

template <class STREAM>
inline
STREAM& baedb_TestUserDb::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 2: {
        stream << d_db;
      } break;
      case 1: {
        stream << d_db.theList(USER_TABLE);
      } break;
    }
    return stream;
}

inline
int baedb_TestUserDb::numRecords() const
    // DEPRECATED
{
    return numUsers();
}

inline
int baedb_TestUserDb::numFirms() const
{
    return firmView().length();
}

inline
int baedb_TestUserDb::numUsers() const
{
    return userView().length();
}

inline
bsl::ostream& baedb_TestUserDb::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    return d_db.print(stream, level, spacesPerLevel);
}

inline
const bdem_List& baedb_TestUserDb::view() const
    // DEPRECATED
{
    return userView();
}

inline
const bdem_List& baedb_TestUserDb::firmView() const
{
    return d_db.theList(FIRM_TABLE);
}

inline
const bdem_List& baedb_TestUserDb::userView() const
{
    return d_db.theList(USER_TABLE);
}

// FREE OPERATORS

inline
bool operator!=(const baedb_TestUserDb& lhs, const baedb_TestUserDb& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const baedb_TestUserDb& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
