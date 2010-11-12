// baedb_userdb.h               -*-C++-*-
#ifndef INCLUDED_BAEDB_USERDB
#define INCLUDED_BAEDB_USERDB

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a protocol to access fields in a Bloomberg User Database.
//
//@DEPRECATED: Use 'bsiudb_phondb' instead.
//
//@CLASSES:
//    baedb_UserDb: Bloomberg User Database query protocol
//
//@SEE_ALSO: bsiudb_userdb
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: [!DEPRECATED!] This component defines a base-level protocol
// for retrieving information from a standard Bloomberg User Database.  This
// component has been deprecated, clients should instead use 'bsiudb_userdb'.
// Below is a schematic representation of a record in a user database showing
// the fields that this protocol is concerned with:
//..
//           +-- primary keys --+                               inaccessible
//           |         |        |                                    |
//           v         v        v                                    v
//    +-------------+------+----------+-----------+-------------+----------+
//    | User Number | UUID | Login ID | User Name | Firm Number | Password |
//    +-------------+------+----------+-----------+-------------+----------+
//..
// The protocol assumes that User Number, UUID (Universal Unique ID), and Login
// ID are unique (primary) database keys.  The 'baedb_UserDb' class in this
// component provides several retrieval ("lookup") methods, which, given a
// value for a unique key, return one of the other fields in the corresponding
// database record (e.g., 'lookupUserNameByLogin', 'lookupUUIDByUserNumber',
// etc.).  Although the password field may not be retrieved, 'baedb_UserDb'
// does provide three methods for password verification; given a user password,
// it can be verified against a given User Number, UUID, or Login ID (e.g.,
// 'verifyPasswordByUserNumber').
//
// This component has been deprecated, clients should instead use
// 'bsiudb_userdb'.
//
///String Parameters
///-----------------
// Several methods in the 'baedb_UserDb' class accept 'const char *' arguments.
// In all cases, arguments of such type need not be null-terminated.  To
// accommodate this paradigm, each 'const char *' argument is accompanied by an
// 'int' argument specifying the length of the string.
//
///Return Codes
///------------
// An appropriate status code is returned from each of the accessor methods of
// 'baedb_UserDb' to indicate success (a value of 0) or failure (a negative
// value).  For example, one possible failure mode is that a specified User
// Number, UUID, or Login ID does not exist in the database.
//
// The following return codes are defined in this component:
//..
//  * SUCCESS          =  0   // method was successful
//  * NOT_FOUND        = -1   // database record was not found
//  * INVALID_PASSWORD = -2   // password is invalid
//..
///Usage
///-----
// The following snippets of code illustrate basic use of the 'baedb_UserDb'
// protocol.  The sample code assumes that a concrete class implementing the
// protocol has been defined and an instance of it initialized appropriately.
//
// The following 'streamUserInfoFromUserNumber' function illustrates how to
// retrieve the User Name, UUID, Firm Number, and Login ID corresponding to a
// given User Number:
//..
//  void streamUserInfoFromUserNumber(const baedb_UserDb& userDb,
//                                    int                 userNumber,
//                                    bsl::ostream&       output)
//      // Retrieve from the specified 'userDb' the User Name, UUID, Firm
//      // Number, and Login ID of the user having the specified 'userNumber',
//      // and print the results to the specified 'output' stream.
//  {
//      bsl::string name, login;
//      int         uuid, firmNumber;
//
//      // retrieve the user name
//      int retCode = userDb.lookupUserNameByUserNumber(&name, userNumber);
//      if (baedb_UserDb::SUCCESS == retCode) {
//          output << "User Name: " << name << bsl::endl;
//      }
//      else {
//          output << "User Name Error: " << retCode << bsl::endl;
//      }
//
//      // retrieve the uuid
//      retCode = userDb.lookupUUIDbyUserNumber(&uuid, userNumber);
//      if (baedb_UserDb::SUCCESS == retCode) {
//          output << "UUID : " << uuid << bsl::endl;
//      }
//      else {
//          output << "UUID Error: " << retCode << bsl::endl;
//      }
//
//      // retrieve the firm number
//      retCode = userDb.lookupFirmNumberByUserNumber(&firmNumber, userNumber);
//      if (baedb_UserDb::SUCCESS == retCode) {
//          output << "Firm Number: " << firmNumber << bsl::endl;
//      }
//      else {
//          output << "Firm Number Error: " << retCode << bsl::endl;
//      }
//
//      // retrieve the login ID
//      retCode = userDb.lookupLoginByUserNumber(&login, userNumber);
//      if (baedb_UserDb::SUCCESS == retCode) {
//          output << "Login ID: " << login << bsl::endl;
//      }
//      else {
//          output << "Login ID Error: " << retCode << bsl::endl;
//      }
//  }
//..
// The following 'verifyPasswordForUserNumber' function illustrates how to
// verify whether a given password is valid for a given User Number:
//..
//  void verifyPasswordForUserNumber(const baedb_UserDb& userDb,
//                                   int                 userNumber,
//                                   const bsl::string&  password,
//                                   bsl::ostream&       output)
//      // Verify that the specified 'password' is valid for the specified
//      // 'userNumber' in the specified 'userDb', and print the results to the
//      // specified 'output' stream.
//  {
//      int retCode = userDb.verifyPasswordByUserNumber(password.data(),
//                                                      password.length(),
//                                                      userNumber);
//      if (baedb_UserDb::SUCCESS == retCode) {
//          output << "Password is valid." << bsl::endl;
//      }
//      else if (baedb_UserDb::NOT_FOUND == retCode) {
//          output << "User does not exist." << bsl::endl;
//      }
//      else if (baedb_UserDb::INVALID_PASSWORD == retCode) {
//          output << "Password is invalid." << bsl::endl;
//      }
//      else {
//          output << "Undefined verification error." << bsl::endl;
//      }
//  }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif


namespace BloombergLP {

                        // ==================
                        // class baedb_UserDb
                        // ==================

class baedb_UserDb {
    // [!DEPRECATED!] This class provides a protocol (or pure interface) for
    // retrieving information from a standard Bloomberg User Database in which
    // User Number, UUID, and Login ID are each primary keys.  Methods are also
    // provided to perform password verification.  All accessors in this
    // protocol return an integer status value to indicate success (0) or
    // failure (a negative value).
    //
    // This classs has been deprecated, clients should instead use the types
    // defined in 'bsiudb_userdb'.


  public:
    // TYPES
    enum StatusCodes {
        // This enumeration defines the base-level 'StatusCodes' that may be
        // returned from the methods declared in this protocol.  Concrete
        // implementations are free to specify additional (more negative)
        // status codes as appropriate.

        SUCCESS          =  0,  // method was successful
        NOT_FOUND        = -1,  // database record was not found
        INVALID_LOGIN    = -2,  // login is invalid
        INVALID_PASSWORD = -3   // password is invalid
    };

  public:
    // CREATORS
    virtual ~baedb_UserDb();
        // Destroy this user DB object.

    // ACCESSORS
    virtual int lookupFirmNameByFirmNumber(bsl::string *name,
                                           int          firmNumber) const = 0;
        // Load into the specified 'name' the name of the firm having the
        // specified 'firmNumber'.  Return 0 on success, -1 if 'firmNumber' is
        // not in this database, and a more negative value otherwise.  'name'
        // is not modified by an unsuccessful call.  The behavior is undefined
        // unless 0 < firmNumber.

    virtual int lookupFirmNumberByLogin(int        *firmNumber,
                                        const char *login,
                                        int         loginLength) const = 0;
        // Load into the specified 'firmNumber' the firm number of the user
        // having the specified 'login' of the specified 'loginLength'.  Return
        // 0 on success, -1 if 'login' is not in this database, and a more
        // negative value otherwise.  'firmNumber' is not modified by an
        // unsuccessful call.  The behavior is undefined unless
        // 0 <= loginLength.  Note that 'login' need not be null-terminated.

    virtual int lookupFirmNumberByUserNumber(int *firmNumber,
                                             int  userNumber) const = 0;
        // Load into the specified 'firmNumber' the firm number of the user
        // having the specified 'userNumber'.  Return 0 on success, -1 if
        // 'userNumber' is not in this database, and a more negative value
        // otherwise.  'firmNumber' is not modified by an unsuccessful call.
        // The behavior is undefined unless 0 < userNumber.

    virtual int lookupFirmNumberByUUID(int *firmNumber,
                                       int  uuid) const = 0;
        // Load into the specified 'firmNumber' the firm number of the user
        // having the specified 'uuid'.  Return 0 on success, -1 if 'uuid' is
        // not in this database, and a more negative value otherwise.
        // 'firmNumber' is not modified by an unsuccessful call.  The behavior
        // is undefined unless 0 < uuid.

    virtual int lookupLoginByUserNumber(bsl::string *login,
                                        int          userNumber) const = 0;
        // Load into the specified 'login' the login ID of the user having the
        // specified 'userNumber'.  Return 0 on success, -1 if 'userNumber' is
        // not in this database, and a more negative value otherwise.  'login'
        // is not modified by an unsuccessful call.  The behavior is undefined
        // unless 0 < userNumber.

    virtual int lookupLoginByUUID(bsl::string *login,
                                  int          uuid) const = 0;
        // Load into the specified 'login' the login ID of the user having the
        // specified 'uuid'.  Return 0 on success, -1 if 'uuid' is not in this
        // database, and a more negative value otherwise.  'login' is not
        // modified by an unsuccessful call.  The behavior is undefined unless
        // 0 < uuid.

    virtual int lookupUserNameByLogin(bsl::string *name,
                                      const char  *login,
                                      int          loginLength) const = 0;
        // Load into the specified 'name' the name of the user having the
        // specified 'login' of the specified 'loginLength'.  Return 0 on
        // success, -1 if 'login' is not in this database, and a more negative
        // value otherwise.  'name' is not modified by an unsuccessful call.
        // The behavior is undefined unless 0 <= loginLength.  Note that
        // 'login' need not be null-terminated.

    virtual int lookupUserNameByUserNumber(bsl::string *name,
                                           int          userNumber) const = 0;
        // Load into the specified 'name' the name of the user having the
        // specified 'userNumber'.  Return 0 on success, -1 if 'userNumber' is
        // not in this database, and a more negative value otherwise.  'name'
        // is not modified by an unsuccessful call.  The behavior is undefined
        // unless 0 < userNumber.

    virtual int lookupUserNameByUUID(bsl::string *name,
                                     int          uuid) const = 0;
        // Load into the specified 'name' the name of the user having the
        // specified 'uuid'.  Return 0 on success, -1 if 'uuid' is not in this
        // database, and a more negative value otherwise.  'name' is not
        // modified by an unsuccessful call.  The behavior is undefined unless
        // 0 < uuid.

    virtual int lookupUserNumberByLogin(int        *userNumber,
                                        const char *login,
                                        int         loginLength) const = 0;
        // Load into the specified 'userNumber' the user number of the user
        // having the specified 'login' of the specified 'loginLength'.  Return
        // 0 on success, -1 if 'login' is not in this database, and a more
        // negative value otherwise.  'userNumber' is not modified by an
        // unsuccessful call.  The behavior is undefined unless
        // 0 <= loginLength.  Note that 'login' need not be null-terminated.

    virtual int lookupUserNumberByUUID(int *userNumber,
                                       int  uuid) const = 0;
        // Load into the specified 'userNumber' the user number of the user
        // having the specified 'uuid'.  Return 0 on success, -1 if 'uuid' is
        // not in this database, and a more negative value otherwise.
        // 'userNumber' is not modified by an unsuccessful call.  The behavior
        // is undefined unless 0 < uuid.

    virtual int lookupUUIDByLogin(int        *uuid,
                                  const char *login,
                                  int         loginLength) const = 0;
        // Load into the specified 'uuid' the UUID of the user having the
        // specified 'login' of the specified 'loginLength'.  Return 0 on
        // success, -1 if 'login' is not in this database, and a more negative
        // value otherwise.  'uuid' is not modified by an unsuccessful call.
        // The behavior is undefined unless 0 <= loginLength.  Note that
        // 'login' need not be null-terminated.

    virtual int lookupUUIDByUserNumber(int *uuid,
                                       int  userNumber) const = 0;
        // Load into the specified 'uuid' the UUID of the user having the
        // specified 'userNumber'.  Return 0 on success, -1 if 'userNumber' is
        // not in this database, and a more negative value otherwise.  'uuid'
        // is not modified by an unsuccessful call.  The behavior is undefined
        // unless 0 < userNumber.

    virtual int verifyPasswordByLogin(const char *password,
                                      int         passwordLength,
                                      const char *login,
                                      int         loginLength) const = 0;
        // Verify that the specified 'password' of the specified
        // 'passwordLength' is valid for the user having the specified 'login'
        // of the specified 'loginLength'.  Return 0 on success, -1 if 'login'
        // is not in this database, and a more negative value otherwise.  The
        // behavior is undefined unless 0 <= loginLength and
        // 0 <= passwordLength.  Note that neither 'login' nor 'password' need
        // be null-terminated.

    virtual int verifyPasswordByUserNumber(const char *password,
                                           int         passwordLength,
                                           int         userNumber) const = 0;
        // Verify that the specified 'password' of the specified
        // 'passwordLength' is valid for the user having the specified
        // 'userNumber'.  Return 0 on success, -1 if 'userNumber' is not in
        // this database, and a more negative value otherwise.  The behavior is
        // undefined unless 0 < userNumber and 0 <= passwordLength.  Note that
        // 'password' need not be null-terminated.

    virtual int verifyPasswordByUUID(const char *password,
                                     int         passwordLength,
                                     int         uuid) const = 0;
        // Verify that the specified 'password' of the specified
        // 'passwordLength' is valid for the user having the specified 'uuid'.
        // Return 0 on success, -1 if 'uuid' is not in this database, and a
        // more negative value otherwise.  The behavior is undefined unless
        // 0 < uuid and 0 <= passwordLength.  Note that 'password' need not be
        // null-terminated.
};

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
