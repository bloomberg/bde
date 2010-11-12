// bcedb_interface.h          -*-C++-*-
#ifndef INCLUDED_BCEDB_INTERFACE
#define INCLUDED_BCEDB_INTERFACE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a pure protocol to access a database.
//
//@DEPRECATED: Use 'bsidb2_dynamictagservice' and 'bsidb2_cursor' instead.
//
//@CLASSES:
//   bcedb_Interface: provides pure protocol to access a database
//      bcedb_Cursor: provides a protocol for modifiable access to and
//                    iteration through records in a table
// bcedb_ConstCursor: provides a protocol for read only access to and iteration
//                    through records in a table
//
//@AUTHOR: Wayne Barlow (wbarlow1)
//
//@DESCRIPTION: [!DEPRECATED!] This component implements a set of pure
// protocols for database access.  The classes provided by this component
// allow for a common interface to inspect, modify, and search a database.
//
// This component has been deprecated, clients should instead use the types
// defined in 'bsidb2_dynamictagservice' and 'bsidb2_cursor'.
//
///Usage
///-----
// In this example, we will use this interface to open a connection to
// a database, find out information about the database then add a record,
// find a set of records, update a record, and delete a record.  This example
// will assume that a concrete implementation of this class called 'MyFileDb'
// has been implemented and a db with the name "test" exists with a table
// called "example" with integer, double, and string fields named 'a', 'b',
// and 'c' respectively.
//..
// class MyFileDb :: public bcedb_Interface
// {
//    ...
// };
//..
// After instantiating the class, open a connection to a database.
//..
// enum { GOOD = 0, BAD };
//
// MyFileDb db("/tmp/dbdata/");
// if (db.open("test")) return BAD;
//..
// We can inspect the tables in the database as follows.
//..
// bcema_SharedPtr<bdem_Schema> dbSchema;
// dbSchema.createInplace();
// bsl::vector<bsl::string> tables;
// db.getDbSchema(&(*dbSchema), &tables);
//..
// Now we can create a record to add.
//..
// bcema_Aggregate agg(dbSchema, "example");
// agg.setField("a", 5);
// agg.setField("b", 10.2);
// agg.setField("c", "cstring");
//
// if (db.addRecord(agg, "example"))
//     return BAD;
//..
// We can find a record by setting fields in the aggregate to the
// values we want to use for the search.  We need to add any strings
// where we want to do partial matching to an array.  For example, if we
// want to search for all strings in the 'c' field that start with 'cstr',
// we can do the following.
//..
// agg.asElemRef().theList().makeAllNull();
// agg.setField("c", "cstr");
//
// bsl::vector<bsl::string> partials;
// partials.append("c");
//
// bdema_ManagedPtr<bcedb_Cursor> cursor = db.query(agg, "example",
//                                                  partials);
// assert(*cursor);  // We know at least one record matches the search
//..
// Now that we have search results, we can traverse the cursor to
// get to all the rows that matched.
//..
// for (; *cursor; cursor->next()) {
//     bsl::cout << cursor->data().field("a").asInt() << bsl::endl;
// }
//..
// We can also use a cursor to update the record.
//..
// agg.setField("c", "cstring");      // do an exact match
// cursor = db.query(agg, "example");
// assert(cursor->isLast());          // we should have one and only one record
// agg.setField("a", 10);
// cursor->updateRecord(agg);         // 'a' changed from 5 to 10
//..
// Remove is done very similarly.  We can remove all records where 'c'
// starts with the characters "abc".
//..
// agg.setField("c", "abc");
// for (cursor = db.query(agg, "example", partials); *cursor; cursor->next()) {
//     cursor->remove();
// }
//..
// We can also query for only some of the fields by creating a new record
// that is a subset of the table.  This is useful if you have a large table
// and only care about a few fields.
//..
// bcema_SharedPtr<bdem_Schema> newSchema;
// newSchema.createInplace();
//
// bdem_RecordDef *keyRec_p = keySchema->addRecord("example_key");
// keyRec_p->appendField(bdem_ElemType::BDEM_STRING, "c");
// keyRec_p->appendField(bdem_ElemType::BDEM_INT, "a");
// bcema_SharedPtr<bdem_RecordDef> newDef(keySchema, keyRec_p);
//
// bcema_Aggregate newAgg(keyDef, keyData);
// newAgg.setField("c", "cstring");
// cursor = db.query(newAgg, "example");
// if (*cursor) {
//     assert(!c->data().hasField("b"));   // The new search will not return
//                                         // the 'b' field.
// }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTR
#include <bdema_managedptr.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif


namespace BloombergLP {

class bdem_RecordDef;
class bdem_Schema;
class bcem_Aggregate;

class bcedb_Cursor;
class bcedb_ConstCursor;

                        // =====================
                        // class bcedb_Interface
                        // =====================

class bcedb_Interface {
    // DEPRECATED: use 'bsidb2_dynamictagservice' instead.

  public:
    // TBD: do we want to define these here?
    enum DB_STATUS_CODE {
        STATUS_OK = 0,
        STATUS_INTERNAL_ERR, // Generic internal database failure
        STATUS_BAD_REQUEST,  // Bad request
        STATUS_COMM_ERR,     // Communication error
        STATUS_BAD_INDEX,    // Bad index
        STATUS_WRITE_FAIL,   // Failed to modify the database
        STATUS_BAD_DUP       // Transaction failed to execute due to duplicates
    };

  public:
    // CREATORS
    virtual ~bcedb_Interface();

    // MANIPULATORS
    virtual int open(const char *dbName) = 0;
        // Open a connection the database specified by 'dbName'.  Return
        // zero on success and non-zero otherwise.

    virtual int addRecord(const bcem_Aggregate&  record,
                          const char            *table) = 0;
        // Add the specified 'record' to the specified 'table' in this
        // database.  Return 0 on success and non-zero otherwise.  The
        // behavior is undefined if the record definition does not conform
        // to the schema for the specified 'table' in this database.

    virtual
    bdema_ManagedPtr<bcedb_Cursor> getFirstRecord(
                             bcema_SharedPtr<const bdem_RecordDef>  recordDef,
                             const char                            *table) = 0;
        // Return a cursor, that conforms to the specified 'recordDef', to the
        // first record in the table such that iterating through this cursor
        // will provide access to each record in the specified 'table'.

    virtual
    bdema_ManagedPtr<bcedb_Cursor> query(const bcem_Aggregate&  record,
                                         const char            *table) = 0;
    virtual
    bdema_ManagedPtr<bcedb_Cursor> query(
                           const bcem_Aggregate&            record,
                           const char                      *table,
                           const bsl::vector<bsl::string>&  partialFields) = 0;
    virtual
    bdema_ManagedPtr<bcedb_Cursor> query(const bcem_Aggregate&  record,
                                         const bcem_Aggregate&  endRecord,
                                         const char            *table) = 0;
    virtual
    bdema_ManagedPtr<bcedb_Cursor> query(
                           const bcem_Aggregate&            record,
                           const bcem_Aggregate&            endRecord,
                           const char                      *table,
                           const bsl::vector<bsl::string>&  partialFields) = 0;
        // Attempt to find the specified 'record' in the specified 'table' in
        // this database.  Optionally specify 'partialFields' to indicate
        // which fields should be used for partial string matching.  Also,
        // you can optionally specify 'endRecord' to indicate that a range
        // from 'record' to 'endRecord' should be queried.  'endRecord' must
        // conform to the same fields as 'record'.  Return a cursor pointing
        // to the first record that matches the criteria identified by
        // 'record'; or an invalid cursor if no records match.

    virtual
    bdema_ManagedPtr<bcedb_Cursor> queryWithIndex(
                                         const bcem_Aggregate&  record,
                                         const char            *table,
                                         const char            *indexName) = 0;
    virtual
    bdema_ManagedPtr<bcedb_Cursor> queryWithIndex(
                           const bcem_Aggregate&            record,
                           const char                      *table,
                           const char                      *indexName,
                           const bsl::vector<bsl::string>&  partialFields) = 0;
    virtual
    bdema_ManagedPtr<bcedb_Cursor> queryWithIndex(
                                         const bcem_Aggregate&  record,
                                         const bcem_Aggregate&  endRecord,
                                         const char            *table,
                                         const char            *indexName) = 0;
    virtual
    bdema_ManagedPtr<bcedb_Cursor> queryWithIndex(
                           const bcem_Aggregate&            record,
                           const bcem_Aggregate&            endRecord,
                           const char                      *table,
                           const char                      *indexName,
                           const bsl::vector<bsl::string>&  partialFields) = 0;
        // Attempt to find the specified 'record' in the specified 'table' in
        // this database using the specified 'indexName' where 'indexName'
        // indicates which database index (set 'getTableIndices') to use.
        // Optionally specify 'partialFields' to indicate which fields should
        // be used for partial string matching.   Also, you can optionally
        // specify 'endRecord' to indicate that a range from 'record' to
        // 'endRecord' should be queried.  'endRecord' must conform to the
        // same fields as 'record'.  Return a cursor pointing to the first
        // record that matches the criteria identified by 'record'
        // or an invalid cursor if no records match.

    //ACCESSORS
    virtual bool isOpen() const = 0;
        // Return true if connection is open to the database and false
        // otherwise.

    virtual
    int getDbSchema(bdem_Schema              *schema,
                    bsl::vector<bsl::string> *tableNames) const = 0;
        // Populate the specified 'schema' with records that describe each
        // of the underlying tables in this database.  Also, populate
        // 'tableNames' with the names of each of the tables described in
        // the schema.  Return 0 on success and non-zero otherwise.

    virtual
    int getTableSchema(bdem_Schema *schema, const char *tableName) const = 0;
        // Populate the specified 'schema' with the definition of the
        // specified 'tableName' in the underlying database.  Return 0 on
        // success and non-zero otherwise.

    virtual
    int getTableIndices(bdem_Schema *indexDesciptions,
                        const char  *tableName) const = 0;
        // Populate the specified 'indexDescriptions' with the definition of
        // the indices for the specified 'tableName' in the underlying
        // database.  Each record name should represent a valid index for this
        // table.  Return 0 on success and non-zero otherwise.

    virtual
    bdema_ManagedPtr<bcedb_ConstCursor> getFirstRecord(
                       bcema_SharedPtr<const bdem_RecordDef>  recordDef,
                       const char                            *table) const = 0;
        // Return a cursor, that conforms to the specified 'recordDef', to the
        // first record in the table such that iterating through this cursor
        // will provide access to each record in the specified 'table'.

    virtual
    bdema_ManagedPtr<bcedb_ConstCursor> query(
                                       const bcem_Aggregate&  record,
                                       const char            *table) const = 0;
    virtual
    bdema_ManagedPtr<bcedb_ConstCursor> query(
                     const bcem_Aggregate&            record,
                     const char                      *table,
                     const bsl::vector<bsl::string>&  partialFields) const = 0;
    virtual
    bdema_ManagedPtr<bcedb_ConstCursor> query(
                                       const bcem_Aggregate&  record,
                                       const bcem_Aggregate&  endRecord,
                                       const char            *table) const = 0;
    virtual
    bdema_ManagedPtr<bcedb_ConstCursor> query(
                     const bcem_Aggregate&            record,
                     const bcem_Aggregate&            endRecord,
                     const char                      *table,
                     const bsl::vector<bsl::string>&  partialFields) const = 0;
        // Attempt to find the specified 'record' in the specified 'table' in
        // this database.  Optionally specify 'partialFields' to indicate
        // which fields should be used for partial string matching.  Also,
        // you can optionally specify 'endRecord' to indicate that a range
        // from 'record' to 'endRecord' should be queried.  'endRecord' must
        // conform to the same fields as 'record'.  Return a read only cursor
        // pointing to the first record that matches the criteria identified by
        // 'record' or an invalid cursor if no records match.

    virtual
    bdema_ManagedPtr<bcedb_ConstCursor> queryWithIndex(
                                   const bcem_Aggregate&  record,
                                   const char            *table,
                                   const char            *indexName) const = 0;
    virtual
    bdema_ManagedPtr<bcedb_ConstCursor> queryWithIndex(
                     const bcem_Aggregate&            record,
                     const char                      *table,
                     const char                      *indexName,
                     const bsl::vector<bsl::string>&  partialFields) const = 0;
    virtual
    bdema_ManagedPtr<bcedb_ConstCursor> queryWithIndex(
                                   const bcem_Aggregate&  record,
                                   const bcem_Aggregate&  endRecord,
                                   const char            *table,
                                   const char            *indexName) const = 0;
    virtual
    bdema_ManagedPtr<bcedb_ConstCursor> queryWithIndex(
                     const bcem_Aggregate&            record,
                     const bcem_Aggregate&            endRecord,
                     const char                      *table,
                     const char                      *indexName,
                     const bsl::vector<bsl::string>&  partialFields) const = 0;
        // Attempt to find the specified 'record' in the specified 'table' in
        // this database using the specified 'indexName' where 'indexName'
        // indicates which database index (set 'getTableIndices') to use.
        // Optionally specify 'partialFields' to indicate which fields should
        // be used for partial string matching.   Also, you can optionally
        // specify 'endRecord' to indicate that a range from 'record' to
        // 'endRecord' should be queried.  'endRecord' must conform to the
        // same fields as 'record'.  Return a read only cursor pointing to the
        // first record that matches the criteria identified by 'record'
        // or an invalid cursor if no records match.

    virtual void getErrorString(bsl::string *result, int code) const = 0;
        // Populate the specified 'result' with a descriptive string for
        // the provided error 'code'.
};

                        // =======================
                        // class bcedb_ConstCursor
                        // =======================

class bcedb_ConstCursor {
    // Provide a modifiable access cursor to the underlying data
    // for a specific table in the underlying database object.
    // The cursor can be moved forward or backward to access the
    // results of a query.
    //
    // DEPRECATED: use 'bsidb2_cursor' instead.

  public:
    virtual ~bcedb_ConstCursor();
        // Destroy this 'bcedb_ConstCursor' object.

    // MANIPULATORS
    virtual bcedb_ConstCursor& next() = 0;
        // Advance this cursor to refer to the next record that matches the
        // search criteria.  Return a modifiable reference to this object.
        // Calling 'next()' on the last record in the search will return
        // an invalid cursor.  The behavior is undefined is the cursor is
        // invalid.

    virtual bcedb_ConstCursor& prev() = 0;
        // Advance this cursor to refer to the previous row that matches
        // the search criteria.  Return a modifiable reference to this
        // object.  The behavior is undefined is the cursor is invalid.

    // ACCESSORS
    virtual bcem_Aggregate data() const = 0;
        // Return the 'data' currently associated with this cursor.

    virtual bool isFirst() const = 0;
        // Return 'true' if this cursor is the first cursor in the list.

    virtual bool isLast() const = 0;
        // Return 'true' if this cursor is the last cursor in the list.

    virtual bool isDeleted() const = 0;
        // Return 'true' if this cursor has been deleted.

    virtual operator bool() const = 0;
        // Return true if the cursor is valid and false otherwise.
};

                        // ==================
                        // class bcedb_Cursor
                        // ==================

class bcedb_Cursor : public bcedb_ConstCursor {
    // Provide a modifiable access cursor to the underlying data
    // for a specific table in the underlying database object.
    // The cursor can be moved forward or backward to access the
    // results of a query.
    //
    // DEPRECATED: use 'bsidb2_cursor' instead.

  public:
    virtual ~bcedb_Cursor();
        // Destroy this 'bcedb_Cursor' object.

    // MANIPULATORS
    virtual int update(const bcem_Aggregate& newRecord) = 0;
        // Update this record with the specified 'newRecord'.  Return 0
        // on success and non-zero otherwise.  The behavior is undefined
        // if the record definition does not conform to the schema for the
        // specified 'table' in this database.

    virtual int remove() = 0;
        // Remove the record pointed to by this cursor.  The cursor remains
        // valid, but the record is invalid and 'isDeleted()' will return
        // 'true' for this cursor.  Return 0 on success, and non-zero
        // otherwise.  The behavior is undefined if this object does not point
        // to a valid position in the table.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
