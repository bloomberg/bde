// baedb_userdb.t.cpp     -*-C++-*-

#include <baedb_userdb.h>

#include <bsl_strstream.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a pure protocol class.  We need to verify that a concrete
// derived class (1) can be invoked and (2) the method calls are forwarded
// appropriately when invoked through the protocol.  We create a sample derived
// class that provides a dummy implementation of the base class virtual
// methods.  We then verify that when a method is called through a base class
// instance pointer the appropriate method in the derived class instance is
// invoked.
//-----------------------------------------------------------------------------
// [ 1] virtual ~baedb_UserDb();
// [ 1] virtual int lookupFirmNumberByLogin(...) const = 0;
// [ 1] virtual int lookupFirmNumberByUserNumber(...) const = 0;
// [ 1] virtual int lookupFirmNumberByUUID(...) const = 0;
// [ 1] virtual int lookupLoginByUserNumber(...) const = 0;
// [ 1] virtual int lookupLoginByUUID(...) const = 0;
// [ 1] virtual int lookupUserNameByLogin(...) const = 0;
// [ 1] virtual int lookupUserNameByUserNumber(...) const = 0;
// [ 1] virtual int lookupUserNameByUUID(...) const = 0;
// [ 1] virtual int lookupUserNumberByLogin(...) const = 0;
// [ 1] virtual int lookupUserNumberByUUID(...) const = 0;
// [ 1] virtual int lookupUUIDByLogin(...) const = 0;
// [ 1] virtual int lookupUUIDByUserNumber(...) const = 0;
// [ 1] virtual int verifyPasswordByLogin(...) const = 0;
// [ 1] virtual int verifyPasswordByUserNumber(...) const = 0;
// [ 1] virtual int verifyPasswordByUUID(...) const = 0;
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
// [ 2] USAGE EXAMPLE: streamUserInfoFromUserNumber(...);
// [ 3] USAGE EXAMPLE: verifyPasswordForUserNumber(...);
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
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//============================================================================
//                      CONCRETE DERIVED TYPE
//----------------------------------------------------------------------------

static int my_UserDbDestructorCalled = 0;

class my_UserDb : public baedb_UserDb {
    // Test class used to verify protocol.

    int     d_fun;          // holds code describing (non-const) function:
                            // 16 lookupFirmNameByFirmNumber(...);
                            //  1 lookupFirmNumberByLogin(...);
                            //  2 lookupFirmNumberByUserNumber(...);
                            //  3 lookupFirmNumberByUUID(...);
                            //  4 lookupLoginByUserNumber(...);
                            //  5 lookupLoginByUUID(...);
                            //  6 lookupUserNameByLogin(...);
                            //  7 lookupUserNameByUserNumber(...);
                            //  8 lookupUserNameByUUID(...);
                            //  9 lookupUserNumberByLogin(...);
                            // 10 lookupUserNumberByUUID(...);
                            // 11 lookupUUIDByLogin(...);
                            // 12 lookupUUIDByUserNumber(...);
                            // 13 verifyPasswordByLogin(...);
                            // 14 verifyPasswordByUserNumber(...);
                            // 15 verifyPasswordByUUID(...);

    string  d_name;         // last value of name passed
    int     d_uuid;         // last value of uuid passed
    int     d_userNumber;   // last value of userNumber passed
    int     d_firmNumber;   // last value of firmNumber passed
    string  d_login;        // last value of login passed
    string  d_password;     // last value of password passed

  private:
    // NOT IMPLEMENTED
    my_UserDb(const my_UserDb& original);
    my_UserDb& operator=(const my_UserDb& rhs);

  public:
    // CREATORS
    my_UserDb()
    : d_fun(0)
    , d_uuid(0)
    , d_userNumber(0)
    , d_firmNumber(0)
    {
        my_UserDbDestructorCalled = 0;
    }
    ~my_UserDb()
    {
        my_UserDbDestructorCalled = 1;
    }

    // ACCESSORS
    virtual int lookupFirmNameByFirmNumber(string *name,
                                           int     firmNumber) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_firmNumber = firmNumber;
        tmp->d_fun = 16;
        *name = "";
        return 0;
    }
    virtual int lookupFirmNumberByLogin(int        *firmNumber,
                                        const char *login,
                                        int         loginLen) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_login = string(login, loginLen);
        tmp->d_firmNumber = *firmNumber;
        tmp->d_fun = 1;
        *firmNumber = 0;
        return 0;
    }

    virtual int lookupFirmNumberByUserNumber(int *firmNumber,
                                             int  userNumber) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_userNumber = userNumber;
        tmp->d_firmNumber = *firmNumber;
        tmp->d_fun = 2;
        *firmNumber = 0;
        return 0;
    }

    virtual int lookupFirmNumberByUUID(int *firmNumber,
                                       int  uuid) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_uuid = uuid;
        tmp->d_firmNumber = *firmNumber;
        tmp->d_fun = 3;
        *firmNumber = 0;
        return 0;
    }

    virtual int lookupLoginByUserNumber(string *login,
                                        int     userNumber) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_userNumber = userNumber;
        tmp->d_login = *login;
        tmp->d_fun = 4;
        *login = "";
        return 0;
    }

    virtual int lookupLoginByUUID(string *login,
                                  int     uuid) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_uuid = uuid;
        tmp->d_login = *login;
        tmp->d_fun = 5;
        *login = "";
        return 0;
    }

    virtual int lookupUserNameByLogin(string     *name,
                                      const char *login,
                                      int         loginLen) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_login = string(login, loginLen);
        tmp->d_name = *name;
        tmp->d_fun = 6;
        *name = "";
        return 0;
    }

    virtual int lookupUserNameByUserNumber(string *name,
                                           int     userNumber) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_userNumber = userNumber;
        tmp->d_name = *name;
        tmp->d_fun = 7;
        *name = "";
        return 0;
    }

    virtual int lookupUserNameByUUID(string *name,
                                     int     uuid) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_uuid = uuid;
        tmp->d_name = *name;
        tmp->d_fun = 8;
        *name = "";
        return 0;
    }

    virtual int lookupUserNumberByLogin(int        *userNumber,
                                        const char *login,
                                        int         loginLen) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_userNumber = *userNumber;
        tmp->d_login = string(login, loginLen);
        tmp->d_fun = 9;
        *userNumber = 0;
        return 0;
    }

    virtual int lookupUserNumberByUUID(int *userNumber,
                                       int  uuid) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_uuid = uuid;
        tmp->d_userNumber = *userNumber;
        tmp->d_fun = 10;
        *userNumber = 0;
        return 0;
    }

    virtual int lookupUUIDByLogin(int        *uuid,
                                  const char *login,
                                  int         loginLen) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_login = string(login, loginLen);
        tmp->d_uuid = *uuid;
        tmp->d_fun = 11;
        *uuid = 0;
        return 0;
    }

    virtual int lookupUUIDByUserNumber(int *uuid,
                                       int  userNumber) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_userNumber = userNumber;
        tmp->d_uuid = *uuid;
        tmp->d_fun = 12;
        *uuid = 0;
        return 0;
    }

    virtual int verifyPasswordByLogin(const char *password,
                                      int         passwordLen,
                                      const char *login,
                                      int         loginLen) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_login = string(login, loginLen);
        tmp->d_password = string(password, passwordLen);
        tmp->d_fun = 13;
        return 0;
    }

    virtual int verifyPasswordByUserNumber(const char *password,
                                           int         passwordLen,
                                           int         userNumber) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_userNumber = userNumber;
        tmp->d_password = string(password, passwordLen);
        tmp->d_fun = 14;
        return 0;
    }

    virtual int verifyPasswordByUUID(const char *password,
                                     int         passwordLen,
                                     int         uuid) const
        // Dummy implementation of protocol method.
    {
        my_UserDb *const tmp = const_cast<my_UserDb *>(this);
        tmp->d_uuid = uuid;
        tmp->d_password = string(password, passwordLen);
        tmp->d_fun = 15;
        return 0;
    }

    // non-virtual functions for testing
    int         fun()        const { return d_fun; }
    const char *name()       const { return d_name.data(); }
    int         uuid()       const { return d_uuid; }
    int         userNumber() const { return d_userNumber; }
    int         firmNumber() const { return d_firmNumber; }
    const char *login()      const { return d_login.data(); }
    const char *password()   const { return d_password.data(); }
};

//============================================================================
//                         USAGE EXAMPLES
//----------------------------------------------------------------------------

// The following 'streamUserInfoFromUserNumber' function illustrates how to
// retrieve the User Name, UUID, Firm Number, and Login ID corresponding to a
// given User Number:

void streamUserInfoFromUserNumber(const baedb_UserDb& userDb,
                                  int                 userNumber,
                                  bsl::ostream&       output)
    // Retrieve from the specified 'userDb' the User Name, UUID, Firm
    // Number, and Login ID of the user having the specified 'userNumber',
    // and print the results to the specified 'output' stream.
{
    bsl::string name, login;
    int         uuid, firmNumber;

    // retrieve the user name
    int retCode = userDb.lookupUserNameByUserNumber(&name, userNumber);
    if (baedb_UserDb::SUCCESS == retCode) {
        output << "User Name: " << name << bsl::endl;
    }
    else {
        output << "User Name Error: " << retCode << bsl::endl;
    }

    // retrieve the uuid
    retCode = userDb.lookupUUIDByUserNumber(&uuid, userNumber);
    if (baedb_UserDb::SUCCESS == retCode) {
        output << "UUID : " << uuid << bsl::endl;
    }
    else {
        output << "UUID Error: "  << retCode << bsl::endl;
    }

    // retrieve the firm number
    retCode = userDb.lookupFirmNumberByUserNumber(&firmNumber, userNumber);
    if (baedb_UserDb::SUCCESS == retCode) {
        output << "Firm Number: " << firmNumber << bsl::endl;
    }
    else {
        output << "Firm Number Error: " << retCode << bsl::endl;
    }

    // retrieve the login ID
    retCode = userDb.lookupLoginByUserNumber(&login, userNumber);
    if (baedb_UserDb::SUCCESS == retCode) {
        output << "Login ID: " << login << bsl::endl;
    }
    else {
        output << "Login ID Error: " << retCode << bsl::endl;
    }
}

// The following 'verifyPasswordForUserNumber' function illustrates how to
// verify whether a given password is valid for a given User Number:

void verifyPasswordForUserNumber(const baedb_UserDb& userDb,
                                 int                 userNumber,
                                 const bsl::string&  password,
                                 bsl::ostream&       output)
    // Verify that the specified 'password' is valid for the specified
    // 'userNumber' in the specified 'userDb', and print the results to the
    // specified 'output' stream.
{
    int retCode = userDb.verifyPasswordByUserNumber(password.data(),
                                                    password.length(),
                                                    userNumber);
    if (baedb_UserDb::SUCCESS == retCode) {
        output << "Password is valid." << bsl::endl;
    }
    else if (baedb_UserDb::NOT_FOUND == retCode) {
        output << "User does not exist." << bsl::endl;
    }
    else if (baedb_UserDb::INVALID_PASSWORD == retCode) {
        output << "Password is invalid." << bsl::endl;
    }
    else {
        output << "Undefined verification error." << bsl::endl;
    }
}

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
        //   This will test the fourth usage example in the header file.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link and run on all platforms as shown.
        //
        // Plan:
        //   Test the code provided in the component header file.  Create an
        //   instance of 'my_UserDb' and invoke the functions under test by
        //   passing the 'my_UserDb' object to the function.
        //
        // Testing:
        //   USAGE EXAMPLE: verifyPasswordForUserNumber(...);
        //---------------------------------------------------------------------
        if (verbose) cout << "\nTESTING USAGE EXAMPLE 2"
                          << "\n=======================" << endl;

        const int SIZE = 128;

        char buf[SIZE];
        memset(buf, 0, sizeof(buf));
        ostrstream outbuf(buf, sizeof(buf));

        my_UserDb mX;  const baedb_UserDb& X = mX;
        const bsl::string  password("testpass1");

        verifyPasswordForUserNumber(X, 9876543, password, outbuf);

        if (veryVerbose) {
            cout << outbuf.str();
        }

        if (verbose) cout << "\nEnd of Usage Example 2 Test." << endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //   This will test the first usage example in the header file.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link and run on all platforms as shown.
        //
        // Plan:
        //   Test the code provided in the component header file.  Create an
        //   instance of 'my_UserDb' and invoke the function under test by
        //   passing the 'my_UserDb' object to the function.
        //
        // Testing:
        //   USAGE EXAMPLE: streamUserInfoFromUserNumber(...);
        //---------------------------------------------------------------------
        if (verbose) cout << "\nTESTING USAGE EXAMPLE 1"
                          << "\n=======================" << endl;

        char buf[128];
        memset(buf, 0, sizeof(buf));
        ostrstream outbuf(buf, sizeof(buf));

        my_UserDb mX;  const baedb_UserDb& X = mX;

        streamUserInfoFromUserNumber(X, 12345678, outbuf);

        if (veryVerbose) {
            cout << outbuf.str();
        }

        if (verbose) cout << "\nEnd of Usage Example 1 Test." << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST
        //   This tests the protocol class.
        //
        // Concerns:
        //   All we need to do is make sure that a concrete subclass of the
        //   'baedb_UserDb' class compiles and links when all virtual functions
        //   are defined.
        //
        // Plan:
        //   Create a 'my_UserDb' object and obtain a 'baedb_UserDb' reference.
        //   Exercise all the virtual methods in the class and verify the
        //   values passed and returned are as expected.
        //
        // Testing:
        //   virtual ~baedb_UserDb();
        //   virtual int lookupFirmNameByFirmNumber(...) const = 0;
        //   virtual int lookupFirmNumberByLogin(...) const = 0;
        //   virtual int lookupFirmNumberByUserNumber(...) const = 0;
        //   virtual int lookupFirmNumberByUUID(...) const = 0;
        //   virtual int lookupLoginByUserNumber(...) const = 0;
        //   virtual int lookupLoginByUUID(...) const = 0;
        //   virtual int lookupUserNameByLogin(...) const = 0;
        //   virtual int lookupUserNameByUserNumber(...) const = 0;
        //   virtual int lookupUserNameByUUID(...) const = 0;
        //   virtual int lookupUserNumberByLogin(...) const = 0;
        //   virtual int lookupUserNumberByUUID(...) const = 0;
        //   virtual int lookupUUIDByLogin(...) const = 0;
        //   virtual int lookupUUIDByUserNumber(...) const = 0;
        //   virtual int verifyPasswordByLogin(...) const = 0;
        //   virtual int verifyPasswordByUserNumber(...) const = 0;
        //   virtual int verifyPasswordByUUID(...) const = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nPROTOCOL TEST"
                          << "\n=============" << endl;


        // Ensure that an instance of the derived class can be created.

        my_UserDb mX;  const baedb_UserDb& X = mX;
        ASSERT(0 == mX.fun());

        // variables used for testing
        int    uuid,
               olduuid;
        int    userNumber,
               oldUserNumber;
        int    firmNumber,
               oldFirmNumber;
        string name,
               oldName;
        string login,
               oldLogin;
        string password;
        string firmName;

        if (verbose) cout << "\nTesting protocol interface." << endl;
        {
            // Invoke different methods view the base class reference and
            // check if the derived class method is invoked.

            login = "input1";
            firmNumber = oldFirmNumber = 9876;
            firmName = "firmName1";

            X.lookupFirmNumberByLogin(&firmNumber,
                                       login.data(),
                                       login.length());
            ASSERT(mX.fun() == 1);
            ASSERT(mX.login() == login);
            ASSERT(mX.firmNumber() == oldFirmNumber);
            ASSERT(0 == firmNumber);

            userNumber = 8765;
            firmNumber = oldFirmNumber = 7654;
            X.lookupFirmNumberByUserNumber(&firmNumber, userNumber);
            ASSERT(mX.fun() == 2);
            ASSERT(mX.userNumber() == userNumber);
            ASSERT(mX.firmNumber() == oldFirmNumber);
            ASSERT(0 == firmNumber);

            uuid = 6543;
            firmNumber = oldFirmNumber = 5432;
            X.lookupFirmNumberByUUID(&firmNumber, uuid);
            ASSERT(mX.fun() == 3);
            ASSERT(mX.uuid() == uuid);
            ASSERT(mX.firmNumber() == oldFirmNumber);
            ASSERT(0 == firmNumber);

            userNumber = 4321;
            login = oldLogin = "input2";
            X.lookupLoginByUserNumber(&login, userNumber);
            ASSERT(mX.fun() == 4);
            ASSERT(mX.userNumber() == userNumber);
            ASSERT(mX.login() == oldLogin);
            ASSERT("" == login);

            uuid = 3210;
            login = oldLogin = "input3";
            X.lookupLoginByUUID(&login, uuid);
            ASSERT(mX.fun() == 5);
            ASSERT(mX.uuid() == uuid);
            ASSERT(mX.login() == oldLogin);
            ASSERT("" == login);

            login = "input4";
            name = oldName = "input5";
            X.lookupUserNameByLogin(&name, login.data(), login.length());
            ASSERT(mX.fun() == 6);
            ASSERT(mX.login() == login);
            ASSERT(mX.name() == oldName);
            ASSERT("" == name);

            userNumber = 2109;
            name = oldName = "input6";
            X.lookupUserNameByUserNumber(&name, userNumber);
            ASSERT(mX.fun() == 7);
            ASSERT(mX.userNumber() == userNumber);
            ASSERT(mX.name() == oldName);
            ASSERT("" == name);

            uuid = 1098;
            name = oldName = "input7";
            X.lookupUserNameByUUID(&name, uuid);
            ASSERT(mX.fun() == 8);
            ASSERT(mX.uuid() == uuid);
            ASSERT(mX.name() == oldName);
            ASSERT("" == name);

            login = "input8";
            userNumber = oldUserNumber = 9876;
            X.lookupUserNumberByLogin(&userNumber,
                                       login.data(),
                                       login.length());
            ASSERT(mX.fun() == 9);
            ASSERT(mX.login() == login);
            ASSERT(mX.userNumber() == oldUserNumber);
            ASSERT(0 == userNumber);

            uuid = 8765;
            userNumber = oldUserNumber = 7654;
            X.lookupUserNumberByUUID(&userNumber, uuid);
            ASSERT(mX.fun() == 10);
            ASSERT(mX.uuid() == uuid);
            ASSERT(mX.userNumber() == oldUserNumber);
            ASSERT(0 == userNumber);

            login = "input9";
            uuid = olduuid = 6543;
            X.lookupUUIDByLogin(&uuid, login.data(), login.length());
            ASSERT(mX.fun() == 11);
            ASSERT(mX.login() == login);
            ASSERT(mX.uuid() == olduuid);
            ASSERT(0 == uuid);

            userNumber = 5432;
            uuid = olduuid = 4321;
            X.lookupUUIDByUserNumber(&uuid, userNumber);
            ASSERT(mX.fun() == 12);
            ASSERT(mX.userNumber() == userNumber);
            ASSERT(mX.uuid() == olduuid);
            ASSERT(0 == uuid);

            login = "input10";
            password =  "input11";
            X.verifyPasswordByLogin(password.data(), password.length(),
                                    login.data(),    login.length());
            ASSERT(mX.fun() == 13);
            ASSERT(mX.login() == login);
            ASSERT(mX.password() == password);

            password = "input12";
            userNumber = 3210;
            X.verifyPasswordByUserNumber(password.data(),
                                         password.length(),
                                         userNumber);
            ASSERT(mX.fun() == 14);
            ASSERT(mX.userNumber() == userNumber);
            ASSERT(mX.password() == password);

            password = "input13";
            uuid = 2109;
            X.verifyPasswordByUUID(password.data(), password.length(), uuid);
            ASSERT(mX.fun() == 15);
            ASSERT(mX.uuid() == uuid);
            ASSERT(mX.password() == password);

            X.lookupFirmNameByFirmNumber(&firmName, firmNumber);
            ASSERT(mX.fun() == 16);
            ASSERT(mX.firmNumber() == firmNumber);
            ASSERT(firmName == "");


        }

        if (verbose) cout << "\nTesting destructor." << endl;
        {
            baedb_UserDb* udb_p = new my_UserDb;
            ASSERT(0 == my_UserDbDestructorCalled);
            delete udb_p;
            ASSERT(1 == my_UserDbDestructorCalled);
        }

        if (verbose) cout << "\nEnd of Protocol Test." << endl;
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
