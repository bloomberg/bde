// baedb_testuserdb.cpp     -*-C++-*-
#include <baedb_testuserdb.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baedb_testuserdb_cpp,"$Id$ $CSID$")

#include <bdempu_aggregate.h>
#include <bsls_assert.h>
#include <bdeu_string.h>

#include <bsl_string.h>

namespace BloombergLP {

// CONSTANTS

const bdem_ElemType::Type TOP_LEVEL_ELEMENTS[] = {
    bdem_ElemType::BDEM_LIST,   // firm table
    bdem_ElemType::BDEM_LIST    // user table
};

const int NUM_TOP_LEVEL_ELEMENTS
                      = sizeof TOP_LEVEL_ELEMENTS / sizeof *TOP_LEVEL_ELEMENTS;

enum {
    // This enumeration defines the fields in each firm record.

    FIRM_FIELD_FIRM_NAME   = 0,  // firm name
    FIRM_FIELD_FIRM_NUMBER = 1   // firm number
};

const bdem_ElemType::Type FIRM_FIELD_TYPES[] = {
    bdem_ElemType::BDEM_STRING,
    bdem_ElemType::BDEM_INT
};

const int NUM_FIRM_FIELD_TYPES
                          = sizeof FIRM_FIELD_TYPES / sizeof *FIRM_FIELD_TYPES;

enum {
    // This enumeration defines the fields in each user record.

    USER_FIELD_USER_NAME   = 0,  // user name
    USER_FIELD_UUID        = 1,  // unique ID
    USER_FIELD_USER_NUMBER = 2,  // user number
    USER_FIELD_FIRM_NUMBER = 3,  // firm number
    USER_FIELD_LOGIN       = 4,  // login ID
    USER_FIELD_PASSWORD    = 5   // password
};

const bdem_ElemType::Type USER_FIELD_TYPES[] = {
    bdem_ElemType::BDEM_STRING,
    bdem_ElemType::BDEM_INT,
    bdem_ElemType::BDEM_INT,
    bdem_ElemType::BDEM_INT,
    bdem_ElemType::BDEM_STRING,
    bdem_ElemType::BDEM_STRING
};

const int NUM_USER_FIELD_TYPES
                          = sizeof USER_FIELD_TYPES / sizeof *USER_FIELD_TYPES;

                        // ----------------------
                        // class baedb_TestUserDb
                        // ----------------------

// PRIVATE CLASS METHODS

int baedb_TestUserDb::hasUniqueUserKeys(const bdem_List& userTable)
{
    const int len = userTable.length();

    for (int i = 0; i < len; ++i) {
        const bdem_List& recordi = userTable.theList(i);

        for (int j = i + 1; j < len; ++j) {
            const bdem_List& recordj = userTable.theList(j);

            if (recordi.theInt(USER_FIELD_UUID)
                                      == recordj.theInt(USER_FIELD_UUID)
             || recordi.theInt(USER_FIELD_USER_NUMBER)
                                      == recordj.theInt(USER_FIELD_USER_NUMBER)
             || bdeu_String::areEqualCaseless(
                                        recordi.theString(USER_FIELD_LOGIN),
                                        recordj.theString(USER_FIELD_LOGIN))) {
                return 0;
            }
        }
    }

    return 1;
}

int baedb_TestUserDb::hasUniqueFirmKeys(const bdem_List& firmTable)
{
    const int len = firmTable.length();

    for (int i = 0; i < len; ++i) {
        const bdem_List& recordi = firmTable.theList(i);

        for (int j = i + 1; j < len; ++j) {
            const bdem_List& recordj = firmTable.theList(j);

            if (recordi.theInt(FIRM_FIELD_FIRM_NUMBER)
                                   == recordj.theInt(FIRM_FIELD_FIRM_NUMBER)) {
                return 0;
            }
        }
    }

    return 1;
}

int baedb_TestUserDb::isValidUserRecord(const bdem_List& record)
{
    if (NUM_USER_FIELD_TYPES != record.length()) {
        return 0;
    }

    for (int i = 0; i < NUM_USER_FIELD_TYPES; ++i) {
        if (USER_FIELD_TYPES[i] != record.elemType(i)) {
            return 0;
        }
    }

    if (0 >= record.theInt(USER_FIELD_UUID)
     || 0 >= record.theInt(USER_FIELD_USER_NUMBER)
     || 0 == record.theString(USER_FIELD_LOGIN).length()) {
        return 0;
    }

    return 1;
}

int baedb_TestUserDb::isValidFirmRecord(const bdem_List& record)
{
    if (NUM_FIRM_FIELD_TYPES != record.length()) {
        return 0;
    }

    for (int i = 0; i < NUM_FIRM_FIELD_TYPES; ++i) {
        if (FIRM_FIELD_TYPES[i] != record.elemType(i)) {
            return 0;
        }
    }

    if (0 >= record.theInt(FIRM_FIELD_FIRM_NUMBER)) {
        return 0;
    }

    return 1;
}

// CREATORS

baedb_TestUserDb::baedb_TestUserDb(bslma_Allocator *basicAllocator)
: d_db(TOP_LEVEL_ELEMENTS, NUM_TOP_LEVEL_ELEMENTS, basicAllocator)
{
}

baedb_TestUserDb::baedb_TestUserDb(const char      *inputData,
                                   bslma_Allocator *basicAllocator)
: d_db(TOP_LEVEL_ELEMENTS, NUM_TOP_LEVEL_ELEMENTS, basicAllocator)
{
    addRecords(inputData);
}

baedb_TestUserDb::baedb_TestUserDb(const bdem_List&  inputUserRecords,
                                   bslma_Allocator  *basicAllocator)
: d_db(TOP_LEVEL_ELEMENTS, NUM_TOP_LEVEL_ELEMENTS, basicAllocator)
{
    {
        // Verify user records.

        const int len = inputUserRecords.length();

        for (int i = 0; i < len; ++i) {
            if (bdem_ElemType::BDEM_LIST != inputUserRecords.elemType(i)
             || !isValidUserRecord(inputUserRecords.theList(i))) {
                return;
            }
        }

        if (!hasUniqueUserKeys(inputUserRecords)) {
            return;
        }
    }

    d_db.theModifiableList(USER_TABLE) = inputUserRecords;
}

baedb_TestUserDb::baedb_TestUserDb(const bdem_List&  inputFirmRecords,
                                   const bdem_List&  inputUserRecords,
                                   bslma_Allocator  *basicAllocator)
: d_db(TOP_LEVEL_ELEMENTS, NUM_TOP_LEVEL_ELEMENTS, basicAllocator)
{
    {
        // Verify firm records.

        const int len = inputFirmRecords.length();

        for (int i = 0; i < len; ++i) {
            if (bdem_ElemType::BDEM_LIST != inputFirmRecords.elemType(i)
             || !isValidFirmRecord(inputFirmRecords.theList(i))) {
                return;
            }
        }

        if (!hasUniqueFirmKeys(inputFirmRecords)) {
            return;
        }
    }

    {
        // Verify user records.

        const int len = inputUserRecords.length();

        for (int i = 0; i < len; ++i) {
            if (bdem_ElemType::BDEM_LIST != inputUserRecords.elemType(i)
             || !isValidUserRecord(inputUserRecords.theList(i))) {
                return;
            }
        }

        if (!hasUniqueUserKeys(inputUserRecords)) {
            return;
        }
    }

    d_db.theModifiableList(FIRM_TABLE) = inputFirmRecords;
    d_db.theModifiableList(USER_TABLE) = inputUserRecords;
}

// MANIPULATORS

int baedb_TestUserDb::addFirmRecord(const char *firmName,
                                    int         firmNameLength,
                                    int         firmNumber)
{
    BSLS_ASSERT(firmName);
    BSLS_ASSERT(0 <= firmNameLength);
    BSLS_ASSERT(0 <  firmNumber);

    const int len = numFirms();

    bdem_List& firmTable = d_db.theModifiableList(FIRM_TABLE);

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = firmTable.theList(i);

        if (record.theInt(FIRM_FIELD_FIRM_NUMBER) == firmNumber) {
            return NONUNIQUE_KEY;
        }
    }

    bdem_List record(FIRM_FIELD_TYPES, NUM_FIRM_FIELD_TYPES);

    record.theModifiableString(FIRM_FIELD_FIRM_NAME).assign(firmName,
                                                               firmNameLength);
    record.theModifiableInt(FIRM_FIELD_FIRM_NUMBER) = firmNumber;

    firmTable.appendList(record);

    return baedb_UserDb::SUCCESS;
}

int baedb_TestUserDb::addUserRecord(const char *name,
                                    int         nameLength,
                                    int         uuid,
                                    int         userNumber,
                                    int         firmNumber,
                                    const char *login,
                                    int         loginLength,
                                    const char *password,
                                    int         passwordLength)
{
    BSLS_ASSERT(name);
    BSLS_ASSERT(login);
    BSLS_ASSERT(password);
    BSLS_ASSERT(0 <= nameLength);
    BSLS_ASSERT(0 <  uuid);
    BSLS_ASSERT(0 <  userNumber);
    BSLS_ASSERT(0 <  loginLength);
    BSLS_ASSERT(0 <= passwordLength);

    const bsl::string loginString(login, loginLength);
    const int         len = numRecords();

    bdem_List& userTable = d_db.theModifiableList(USER_TABLE);

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (record.theInt(USER_FIELD_UUID)        == uuid
         || record.theInt(USER_FIELD_USER_NUMBER) == userNumber
         || bdeu_String::areEqualCaseless(record.theString(USER_FIELD_LOGIN),
                                          loginString)) {
            return NONUNIQUE_KEY;
        }
    }

    bdem_List record(USER_FIELD_TYPES, NUM_USER_FIELD_TYPES);

    record.theModifiableString(USER_FIELD_USER_NAME).assign(name, nameLength);
    record.theModifiableInt(USER_FIELD_UUID)        = uuid;
    record.theModifiableInt(USER_FIELD_USER_NUMBER) = userNumber;
    record.theModifiableInt(USER_FIELD_FIRM_NUMBER) = firmNumber;
    record.theModifiableString(USER_FIELD_LOGIN).assign(loginString);
    record.theModifiableString(USER_FIELD_PASSWORD).assign(password,
                                                               passwordLength);

    userTable.appendList(record);

    return baedb_UserDb::SUCCESS;
}

int baedb_TestUserDb::addRecords(const char *inputRecords)
    // DEPRECATED
{
    BSLS_ASSERT(inputRecords);

    bdem_List list;

    if (0 != bdempu_Aggregate::parseList(&inputRecords, &list, inputRecords)) {
        return PARSING_ERROR;
    }

    return addUserRecords(list);
}

int baedb_TestUserDb::addFirmRecords(const bdem_List& inputRecords)
{
    // Verify that each element in 'inputRecords' is a 'bdem_List' defining
    // a valid firm record.

    const int listLength = inputRecords.length();

    for (int i = 0; i < listLength; ++i) {
        if (bdem_ElemType::BDEM_LIST != inputRecords.elemType(i)
         || !isValidFirmRecord(inputRecords.theList(i))) {
            return 0;
        }
    }

    // All records are OK.  Add each record to the database that does *not*
    // introduce a duplicate key.

    int recordsAdded = 0;

    bdem_List& firmTable = d_db.theModifiableList(FIRM_TABLE);

    for (int i = 0; i < listLength; ++i) {
        const bdem_List& newRecord = inputRecords.theList(i);
        const int        dbLength  = numRecords();

        bool uniqueKey = true;
        for (int j = 0; j < dbLength; ++j) {
            const bdem_List& record = firmTable.theList(j);

            if (newRecord.theInt(FIRM_FIELD_FIRM_NUMBER)
                                    == record.theInt(FIRM_FIELD_FIRM_NUMBER)) {
                uniqueKey = false;
                break;
            }
        }

        if (uniqueKey) {
            firmTable.appendList(newRecord);
            recordsAdded++;
        }
    }

    return recordsAdded;
}

int baedb_TestUserDb::addUserRecords(const bdem_List& inputRecords)
{
    // Verify that each element in 'inputRecords' is a 'bdem_List' defining
    // a valid user record.

    const int listLength = inputRecords.length();

    for (int i = 0; i < listLength; ++i) {
        if (bdem_ElemType::BDEM_LIST != inputRecords.elemType(i)
         || !isValidUserRecord(inputRecords.theList(i))) {
            return 0;
        }
    }

    // All records are OK.  Add each record to the database that does *not*
    // introduce a duplicate key.

    int recordsAdded = 0;

    bdem_List& userTable = d_db.theModifiableList(USER_TABLE);

    for (int i = 0; i < listLength; ++i) {
        const bdem_List& newRecord = inputRecords.theList(i);
        const int        dbLength  = numRecords();

        bool uniqueKey = true;
        for (int j = 0; j < dbLength; ++j) {
            const bdem_List& record = userTable.theList(j);

            if (newRecord.theInt(USER_FIELD_UUID)
                                       == record.theInt(USER_FIELD_UUID)
             || newRecord.theInt(USER_FIELD_USER_NUMBER)
                                       == record.theInt(USER_FIELD_USER_NUMBER)
             || bdeu_String::areEqualCaseless(
                                         newRecord.theString(USER_FIELD_LOGIN),
                                         record.theString(USER_FIELD_LOGIN))) {
                uniqueKey = false;
                break;
            }
        }

        if (uniqueKey) {
            userTable.appendList(newRecord);
            recordsAdded++;
        }
    }

    return recordsAdded;
}

int baedb_TestUserDb::removeFirmByFirmNumber(int firmNumber)
{
    BSLS_ASSERT(0 < firmNumber);

    const int len = numFirms();

    bdem_List& firmTable = d_db.theModifiableList(FIRM_TABLE);

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = firmTable.theList(i);

        if (record.theInt(FIRM_FIELD_FIRM_NUMBER) == firmNumber) {
            firmTable.removeElement(i);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::removeUserByLogin(const char *login,
                                        int         loginLength)
{
    BSLS_ASSERT(login);
    BSLS_ASSERT(0 < loginLength);

    const bsl::string loginString(login, loginLength);
    const int         len = numRecords();

    bdem_List& userTable = d_db.theModifiableList(USER_TABLE);

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (bdeu_String::areEqualCaseless(record.theString(USER_FIELD_LOGIN),
                                          loginString)) {
            userTable.removeElement(i);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::removeUserByUserNumber(int userNumber)
{
    BSLS_ASSERT(0 < userNumber);

    const int len = numRecords();

    bdem_List& userTable = d_db.theModifiableList(USER_TABLE);

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (record.theInt(USER_FIELD_USER_NUMBER) == userNumber) {
            userTable.removeElement(i);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::removeUserByUUID(int uuid)
{
    BSLS_ASSERT(0 < uuid);

    const int len = numRecords();

    bdem_List& userTable = d_db.theModifiableList(USER_TABLE);

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (record.theInt(USER_FIELD_UUID) == uuid) {
            userTable.removeElement(i);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

// ACCESSORS

int baedb_TestUserDb::lookupFirmNameByFirmNumber(bsl::string *name,
                                                 int          firmNumber) const
{
    BSLS_ASSERT(name);
    BSLS_ASSERT(0 < firmNumber);

    const int len = numFirms();

    const bdem_List& firmTable = firmView();

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = firmTable.theList(i);

        if (record.theInt(FIRM_FIELD_FIRM_NUMBER) == firmNumber) {
            *name = record.theString(FIRM_FIELD_FIRM_NAME);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::lookupFirmNumberByLogin(int        *firmNumber,
                                              const char *login,
                                              int         loginLength) const
{
    BSLS_ASSERT(firmNumber);
    BSLS_ASSERT(login);
    BSLS_ASSERT(0 <= loginLength);

    if (0 == *login || 0 == loginLength) {
        return baedb_UserDb::INVALID_LOGIN;
    }

    const bsl::string loginString(login, loginLength);
    const int         len = numRecords();

    const bdem_List& userTable = userView();

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (bdeu_String::areEqualCaseless(record.theString(USER_FIELD_LOGIN),
                                          loginString)) {
            *firmNumber = record.theInt(USER_FIELD_FIRM_NUMBER);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::lookupFirmNumberByUserNumber(int *firmNumber,
                                                   int  userNumber) const
{
    BSLS_ASSERT(firmNumber);
    BSLS_ASSERT(0 < userNumber);

    const int len = numRecords();

    const bdem_List& userTable = userView();

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (record.theInt(USER_FIELD_USER_NUMBER) == userNumber) {
            *firmNumber = record.theInt(USER_FIELD_FIRM_NUMBER);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::lookupFirmNumberByUUID(int *firmNumber,
                                             int  uuid) const
{
    BSLS_ASSERT(firmNumber);
    BSLS_ASSERT(0 < uuid);

    const int len = numRecords();

    const bdem_List& userTable = userView();

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (record.theInt(USER_FIELD_UUID) == uuid) {
            *firmNumber = record.theInt(USER_FIELD_FIRM_NUMBER);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::lookupLoginByUserNumber(bsl::string *login,
                                              int          userNumber) const
{
    BSLS_ASSERT(login);
    BSLS_ASSERT(0 < userNumber);

    const int len = numRecords();

    const bdem_List& userTable = userView();

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (record.theInt(USER_FIELD_USER_NUMBER) == userNumber) {
            *login = record.theString(USER_FIELD_LOGIN);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::lookupLoginByUUID(bsl::string *login,
                                        int          uuid) const
{
    BSLS_ASSERT(login);
    BSLS_ASSERT(0 < uuid);

    const int len = numRecords();

    const bdem_List& userTable = userView();

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (record.theInt(USER_FIELD_UUID) == uuid) {
            *login = record.theString(USER_FIELD_LOGIN);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::lookupUserNameByLogin(bsl::string *name,
                                            const char  *login,
                                            int          loginLength) const
{
    BSLS_ASSERT(name);
    BSLS_ASSERT(login);
    BSLS_ASSERT(0 <= loginLength);

    if (0 == *login || 0 == loginLength) {
        return baedb_UserDb::INVALID_LOGIN;
    }

    const bsl::string loginString(login, loginLength);
    const int         len = numRecords();

    const bdem_List& userTable = userView();

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (bdeu_String::areEqualCaseless(record.theString(USER_FIELD_LOGIN),
                                          loginString)) {
            *name = record.theString(USER_FIELD_USER_NAME);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::lookupUserNameByUserNumber(bsl::string *name,
                                                 int          userNumber) const
{
    BSLS_ASSERT(name);
    BSLS_ASSERT(0 < userNumber);

    const int len = numRecords();

    const bdem_List& userTable = userView();

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (record.theInt(USER_FIELD_USER_NUMBER) == userNumber) {
            *name = record.theString(USER_FIELD_USER_NAME);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::lookupUserNameByUUID(bsl::string *name,
                                           int          uuid) const
{
    BSLS_ASSERT(name);
    BSLS_ASSERT(0 < uuid);

    const int len = numRecords();

    const bdem_List& userTable = userView();

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (record.theInt(USER_FIELD_UUID) == uuid) {
            *name = record.theString(USER_FIELD_USER_NAME);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::lookupUserNumberByLogin(int        *userNumber,
                                              const char *login,
                                              int         loginLength) const
{
    BSLS_ASSERT(userNumber);
    BSLS_ASSERT(login);
    BSLS_ASSERT(0 <= loginLength);

    if (0 == *login || 0 == loginLength) {
        return baedb_UserDb::INVALID_LOGIN;
    }

    const bsl::string loginString(login, loginLength);
    const int         len = numRecords();

    const bdem_List& userTable = userView();

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (bdeu_String::areEqualCaseless(record.theString(USER_FIELD_LOGIN),
                                          loginString)) {
            *userNumber = record.theInt(USER_FIELD_USER_NUMBER);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::lookupUserNumberByUUID(int *userNumber,
                                             int  uuid) const
{
    BSLS_ASSERT(userNumber);
    BSLS_ASSERT(0 < uuid);

    const int len = numRecords();

    const bdem_List& userTable = userView();

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (record.theInt(USER_FIELD_UUID) == uuid) {
            *userNumber = record.theInt(USER_FIELD_USER_NUMBER);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::lookupUUIDByLogin(int        *uuid,
                                        const char *login,
                                        int         loginLength) const
{
    BSLS_ASSERT(uuid);
    BSLS_ASSERT(login);
    BSLS_ASSERT(0 <= loginLength);

    if (0 == *login || 0 == loginLength) {
        return baedb_UserDb::INVALID_LOGIN;
    }

    const bsl::string loginString(login, loginLength);
    const int         len = numRecords();

    const bdem_List& userTable = userView();

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (bdeu_String::areEqualCaseless(record.theString(USER_FIELD_LOGIN),
                                          loginString)) {
            *uuid = record.theInt(USER_FIELD_UUID);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

int baedb_TestUserDb::lookupUUIDByUserNumber(int *uuid,
                                             int  userNumber) const
{
    BSLS_ASSERT(uuid);
    BSLS_ASSERT(0 < userNumber);

    const int len = numRecords();

    const bdem_List& userTable = userView();

    for (int i = 0; i < len; ++i) {
        const bdem_List& record = userTable.theList(i);

        if (record.theInt(USER_FIELD_USER_NUMBER) == userNumber) {
            *uuid = record.theInt(USER_FIELD_UUID);
            return baedb_UserDb::SUCCESS;
        }
    }

    return baedb_UserDb::NOT_FOUND;
}

// FREE OPERATORS

bool operator==(const baedb_TestUserDb& lhs, const baedb_TestUserDb& rhs)
{
    // TBD: refactor the following code

    {
        // Compare firm table.

        const int lhsLen = lhs.numFirms();
        const int rhsLen = rhs.numFirms();

        if (lhsLen != rhsLen) {
            return 0;
        }

        const bdem_List& lhsDb = lhs.firmView();
        const bdem_List& rhsDb = rhs.firmView();

        for (int i = 0; i < lhsLen; ++i) {
            const bdem_List& lhsRecord = lhsDb.theList(i);

            bool found = false;
            for (int j = 0; j < rhsLen; ++j) {
                if (lhsRecord == rhsDb.theList(j)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return 0;
            }
        }
    }
    {
        // Compare user table.

        const int lhsLen = lhs.numUsers();
        const int rhsLen = rhs.numUsers();

        if (lhsLen != rhsLen) {
            return 0;
        }

        const bdem_List& lhsDb = lhs.userView();
        const bdem_List& rhsDb = rhs.userView();

        for (int i = 0; i < lhsLen; ++i) {
            const bdem_List& lhsRecord = lhsDb.theList(i);

            bool found = false;
            for (int j = 0; j < rhsLen; ++j) {
                if (lhsRecord == rhsDb.theList(j)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return 0;
            }
        }
    }

    return 1;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
