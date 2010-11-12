// bcedb_dtagdb.h             -*-C++-*-
#ifndef INCLUDED_BCEDB_DTAGDB
#define INCLUDED_BCEDB_DTAGDB

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a C++ abstraction for the COMDB2 database.
//
//@DEPRECATED: Use 'bsidb2_dynamictagservice' and 'bsidb2_cursor' instead.
//
//@CLASSES:
//              bcedb_DtagDb: abstraction for COMDB2
//        bcedb_DtagDbCursor: provides modifiable access to a record in a table
//   bcedb_DtagDbConstCursor: provides read only access to a record in a table
//
//@AUTHOR: Wayne Barlow (wbarlow1)
//
//@DESCRIPTION: [!DEPRECATED!] This component implements a C++ abstraction for
// a dynamic tag database.  A 'bcedb_DtagDb' object maintains a connection to
// a database and allows users to add, update, find and delete records in the
// database.  The underlying database must conform to the 'bcedb_DtagFactory'
// interface.
//
// This component has been deprecated, clients should instead use the types
// defined in 'bsidb2_dynamictagservice' and 'bsidb2_cursor'.
//
///Usage
///-----
// The following is an example that illustrates how to use the 'bcedb_DtagDb'
// object with a database with the following lrl and csc2 files:
//..
//  // exdb.lrl file
//  // ...
//
//  name        exdb
//  dbnum       12345
//  dir         /bb/data/example
//
//  table mytable1 /bb/bin/exdb_mytable1.csc2
//  table mytable2 /bb/bin/exdb_mytable2.csc2
//
//  // exdb_mytable1.csc2
//  // ...
//
//  tag ondisk
//  {
//      int         my_value
//      cstring     my_string[16]
//      int         my_value2
//      blob        my_blob
//  }
//
//  keys
//  {
//     recnums "KEY_STRING" = my_string
//  }
//..
// Using the above description of 'exdb' and 'mytable1', the following code
// example will create a connection to the database then add a record.
//..
//  MyComDb2Factory f;  // factory for comdb2
//  bcedb_DtagDb db(&f);
//  if (db.open("exdb")) return BAD;
//
//  bcema_SharedPtr<bdem_Schema> dbSchema;
//  dbSchema.createInplace();
//  bsl::vector<bsl::string> tables;
//  db.getDbSchema(&(*dbSchema), &tables);
//..
// Now we can create a record to add to 'mytable1'.
//..
//  bcema_Aggregate agg(dbSchema, "mytable1");
//  agg.setField("my_value", 5);
//  agg.setField("my_string", "cstring");
//  agg.setField("my_value2", 10);
//  bsl::vector<char> data;
//  populateData(&data);
//  agg.setField("my_blob", data);
//
//  if (db.addRecord(agg, "mytable1")) {
//      return BAD;
//  }
//..
// We can find a record by setting fields in the aggregate to the values we
// want to use for the search.  We need to add any strings where we want to do
// partial matching to an array.  For example, if we want to search for all
// strings in the 'c' field that start with 'cstr', we can do the following.
//..
//  agg.asElemRef().theList().makeAllNull();
//  agg.setField("my_string", "cstr");
//
//  bsl::vector<bsl::string> partials;
//  partials.append("my_string");
//
//  bdema_ManagedPtr<bcedb_Cursor> cursor = db.query(agg, "mytable1",
//                                                   partials);
//
//  assert(*cursor);  // we know at least one record matches the search
//..
// Now that we have search results, we can traverse the cursor to
// get to all the rows that matched.
//..
//  for (; *cursor; cursor->next()) {
//      bsl::cout << cursor->data().field("my_value").asInt() << bsl::endl;
//  }
//..
// We can also use a cursor to update the record.
//..
//  agg.setField("my_string", "cstring");
//  cursor = db.query(agg, "mytable1");
//  assert(cursor->isLast());         // we should have one and only one record
//  agg.setField("my_value2", 20);
//  cursor->updateRecord(agg);        // 'my_value2' changed from 10 to 20
//..
// Remove is done very similarly.  We can remove all records where 'my_string'
// starts with the characters "abc".
//..
//  agg.setField("my_string", "abc");
//  for (cursor = db.query(agg, "example", partials); *cursor; cursor->next()){
//      cursor->remove();
//  }
//..
// We can also query for only some of the fields by creating a new record
// that is a subset of the table.  This is useful if you have a large table
// and only care about a few fields.
//..
//  bcema_SharedPtr<bdem_Schema> newSchema;
//  newSchema.createInplace();
//
//  bdem_RecordDef *keyRec_p = keySchema->addRecord("example_key");
//  keyRec_p->appendField(bdem_ElemType::BDEM_STRING, "my_string");
//  keyRec_p->appendField(bdem_ElemType::BDEM_INT, "my_value");
//  bcema_SharedPtr<bdem_RecordDef> newDef(keySchema, keyRec_p);
//
//  bcema_Aggregate newAgg(keyDef, keyData);
//  newAgg.setField("my_string", "cstring");
//  cursor = db.query(newAgg, "example");
//  if (*cursor) {
//      assert(!c->data().hasField("my_value2"));  // the new search will not
//                                                 // return the 'my_value2'
//                                                 // field
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEDB_INTERFACE
#include <bcedb_interface.h>
#endif

#ifndef INCLUDED_BCEM_AGGREGATE
#include <bcem_aggregate.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif


namespace BloombergLP {

class bcedb_DtagBindingType;
class bcedb_DtagControlType;
class bcedb_DtagDbType;
class bcedb_DtagFactory;
class bslma_Allocator;
class bdem_Schema;

                        // ==================
                        // class bcedb_DtagDb
                        // ==================

class bcedb_DtagDb : public bcedb_Interface {
    // DEPRECATED: use 'bsidb2_dynamictagservice' instead.

    // PRIVATE TYPES
    typedef bsl::map<bsl::string, bdem_Schema> StrSchemaMap;

    // DATA
    mutable bcedb_DtagDbType *d_db;
    bcedb_DtagFactory        *d_factory;
    mutable StrSchemaMap      d_dbKeys;
    bslma_Allocator          *d_allocator;

    // NOT IMPLEMENTED
    bcedb_DtagDb(const bcedb_DtagDb&);
    bcedb_DtagDb& operator=(const bcedb_DtagDb&);

  private:
    // PRIVATE ACCESSORS
    int generatePartialCode(
                         bsl::string                     *partialCode,
                         bsl::vector<bsl::string>        *mandatoryUnsetFields,
                         const char                      *table,
                         const bcem_Aggregate&            record,
                         const bsl::vector<bsl::string>&  partialStrings,
                         const char                      *index) const;
        // Populated the specified 'partialCode' with the dynamic tag partial
        // string code that's appropriate for the specified 'record', 'index',
        // and 'partialStrings'.  Also populate 'mandatoryUnsetFields' with the
        // fields that are unset in the record, but mandatory for a key, which
        // indicates that a range find must be used.  Return 0 on success and
        // non-zero if the partialCode could not be determined.

  public:
    // CLASS METHODS
    static void controlDeleter(bcedb_DtagControlType *c,
                               bcedb_DtagBindingType *f);
        // Deallocate the specified 'c' control using the provided 'f' factory.

    static void bindingDeleter(bcedb_DtagBindingType *t, bcedb_DtagDbType *f);
        // Deallocate the specified 't' tag using the provided 'f' factory.

    static void cursorDeleter(bcedb_Cursor *c, bslma_Allocator *a);
        // Delete the specified cursor, 'c', using the provided allocator, 'a'.

    static void constCursorDeleter(bcedb_ConstCursor *c, bslma_Allocator *a);
        // Delete the specified cursor, 'c', using the provided allocator, 'a'.

    // CREATORS
    bcedb_DtagDb(bcedb_DtagFactory *factory, bslma_Allocator *allocator = 0);
        // Create a 'bcedb_DtagDb' object using the specified 'factory' to
        // interact with the underlying data.  Optionally specify 'allocator'
        // used to allocate memory used by this object.

    ~bcedb_DtagDb();
        // Destroy this object.

    // MANIPULATORS
    int open(const char *dbName);
        // Open a connection the database specified by 'dbName'.  Return
        // zero on success and non-zero otherwise.

    int addRecord(const bcem_Aggregate& record, const char *table);
        // Add the specified 'record' to the specified 'table' in this
        // database.  Return 0 on success and non-zero otherwise.  The
        // behavior is undefined if the record definition does not conform
        // to the schema for the specified 'table' in this database.

    bdema_ManagedPtr<bcedb_Cursor> getFirstRecord(
                              bcema_SharedPtr<const bdem_RecordDef>  recordDef,
                              const char                            *table);
        // Return a cursor, that conforms to the specified 'recordDef', to the
        // first record in the table such that iterating through this cursor
        // will provide access to each record in the specified 'table'.

    bdema_ManagedPtr<bcedb_Cursor> query(const bcem_Aggregate&  record,
                                         const char            *table);
    bdema_ManagedPtr<bcedb_Cursor> query(
                               const bcem_Aggregate&            record,
                               const char                      *table,
                               const bsl::vector<bsl::string>&  partialFields);
    bdema_ManagedPtr<bcedb_Cursor> query(const bcem_Aggregate&  record,
                                         const bcem_Aggregate&  endRecord,
                                         const char             *table);
    bdema_ManagedPtr<bcedb_Cursor> query(
                               const bcem_Aggregate&            record,
                               const bcem_Aggregate&            endRecord,
                               const char                      *table,
                               const bsl::vector<bsl::string>&  partialFields);
        // Attempt to find the specified 'record' in the specified 'table' in
        // this database.  Optionally specify 'partialFields' to indicate
        // which fields should be used for partial string matching.  Also,
        // you can optionally specify 'endRecord' to indicate that a range
        // from 'record' to 'endRecord' should be queried.  'endRecord' must
        // conform to the same fields as 'record'.  Return a cursor pointing
        // to the first record that matches the criteria identified by
        // 'record', or an invalid cursor if no records match.

    bdema_ManagedPtr<bcedb_Cursor> queryWithIndex(
                               const bcem_Aggregate&            record,
                               const char                      *table,
                               const char                      *indexName);
    bdema_ManagedPtr<bcedb_Cursor> queryWithIndex(
                               const bcem_Aggregate&            record,
                               const char                      *table,
                               const char                      *indexName,
                               const bsl::vector<bsl::string>&  partialFields);
    bdema_ManagedPtr<bcedb_Cursor> queryWithIndex(
                               const bcem_Aggregate&            record,
                               const bcem_Aggregate&            endRecord,
                               const char                      *table,
                               const char                      *indexName);
    bdema_ManagedPtr<bcedb_Cursor> queryWithIndex(
                               const bcem_Aggregate&            record,
                               const bcem_Aggregate&            endRecord,
                               const char                      *table,
                               const char                      *indexName,
                               const bsl::vector<bsl::string>&  partialFields);
        // Attempt to find the specified 'record' in the specified 'table' in
        // this database using the specified 'indexName' where 'indexName'
        // indicates which database index (set 'getTableIndices') to use.
        // Optionally specify 'partialFields' to indicate which fields should
        // be used for partial string matching.   Also, you can optionally
        // specify 'endRecord' to indicate that a range from 'record' to
        // 'endRecord' should be queried.  'endRecord' must conform to the same
        // fields as 'record'.  Return a cursor pointing to the first record
        // that matches the criteria identified by 'record' or an invalid
        // cursor if no records match.

    //ACCESSORS
    bool isOpen() const;
        // Return true if connection is open to the database and false
        // otherwise.

    int getDbSchema(bdem_Schema              *schema,
                    bsl::vector<bsl::string> *tableNames) const;
        // Populate the specified 'schema' with records that describe each of
        // the underlying tables in this database.  Also, populate 'tableNames'
        // with the names of each of the tables described in the schema.
        // Return 0 on success and non-zero otherwise.

    int getTableSchema(bdem_Schema *schema, const char *tableName) const;
        // Populate the specified 'schema' with the definition of the
        // specified 'tableName' in the underlying database.  Return 0 on
        // success and non-zero otherwise.

    int getTableIndices(bdem_Schema *indexDescriptions,
                        const char  *tableName) const;
        // Populate the specified 'indexDescriptions' with the definition of
        // the indices for the specified 'tableName' in the underlying
        // database.  Each record name should represent a valid index for this
        // table.  Return 0 on success and non-zero otherwise.

    bdema_ManagedPtr<bcedb_ConstCursor> getFirstRecord(
                           bcema_SharedPtr<const bdem_RecordDef>  recordDef,
                           const char                            *table) const;
        // Return a cursor, that conforms to the specified 'recordDef', to the
        // first record in the table such that iterating through this cursor
        // will provide access to each record in the specified 'table'.

    bdema_ManagedPtr<bcedb_ConstCursor> query(
                         const bcem_Aggregate&            record,
                         const char                      *table)         const;
    bdema_ManagedPtr<bcedb_ConstCursor> query(
                         const bcem_Aggregate&            record,
                         const char                      *table,
                         const bsl::vector<bsl::string>&  partialFields) const;
    bdema_ManagedPtr<bcedb_ConstCursor> query(
                         const bcem_Aggregate&            record,
                         const bcem_Aggregate&            endRecord,
                         const char                      *table)         const;
    bdema_ManagedPtr<bcedb_ConstCursor> query(
                         const bcem_Aggregate&            record,
                         const bcem_Aggregate&            endRecord,
                         const char                      *table,
                         const bsl::vector<bsl::string>&  partialFields) const;
        // Attempt to find the specified 'record' in the specified 'table' in
        // this database.  Optionally specify 'partialFields' to indicate
        // which fields should be used for partial string matching.  Also,
        // you can optionally specify 'endRecord' to indicate that a range
        // from 'record' to 'endRecord' should be queried.  'endRecord' must
        // conform to the same fields as 'record'.  Return a read only cursor
        // pointing to the first record that matches the criteria identified by
        // 'record' or an invalid cursor if no records match.

    bdema_ManagedPtr<bcedb_ConstCursor> queryWithIndex(
                         const bcem_Aggregate&            record,
                         const char                      *table,
                         const char                      *indexName)     const;
    bdema_ManagedPtr<bcedb_ConstCursor> queryWithIndex(
                         const bcem_Aggregate&            record,
                         const char                      *table,
                         const char                      *indexName,
                         const bsl::vector<bsl::string>&  partialFields) const;
    bdema_ManagedPtr<bcedb_ConstCursor> queryWithIndex(
                         const bcem_Aggregate&            record,
                         const bcem_Aggregate&            endRecord,
                         const char                      *table,
                         const char                      *indexName)     const;
    bdema_ManagedPtr<bcedb_ConstCursor> queryWithIndex(
                         const bcem_Aggregate&            record,
                         const bcem_Aggregate&            endRecord,
                         const char                      *table,
                         const char                      *indexName,
                         const bsl::vector<bsl::string>&  partialFields) const;
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

    void getErrorString(bsl::string *result, int code) const;
        // Populate the specified 'result' with a descriptive string for
        // the provided error 'code'.
};

                        // =============================
                        // class bcedb_DtagDbConstCursor
                        // =============================

class bcedb_DtagDbConstCursor : public bcedb_ConstCursor {
    // Provide a read only access cursor to the underlying data
    // for a specific table in the underlying database object.
    // The cursor can be moved forward or backward to access the
    // results of a query.
    //
    // DEPRECATED: use 'bsidb2_cursor' instead.

    // DATA
    bcedb_DtagDbType                         *d_db;
    bdema_ManagedPtr<bcedb_DtagBindingType>   d_tag;  // tag must be deleted
                                                      // after control!
    bdema_ManagedPtr<bcedb_DtagControlType>   d_control;
    bcem_Aggregate                            d_aggregate;
    bool                                      d_isFirst;
    bool                                      d_isLast;
    bool                                      d_isDeleted;

    // FRIENDS
    friend class bcedb_DtagDb;

    // NOT IMPLEMENTED
    bcedb_DtagDbConstCursor(const bcedb_DtagDbConstCursor&);
    bcedb_DtagDbConstCursor& operator=(const bcedb_DtagDbConstCursor&);

  private:
    // PRIVATE CREATORS
    bcedb_DtagDbConstCursor(
                    bcema_SharedPtr<const bdem_RecordDef>  definition,
                    const char                            *table,
                    bcedb_DtagDbType                      *db,
                    bslma_Allocator                       *basicAllocator = 0);
        // Create a cursor whose data conforms to the specified 'definition'
        // for the specified 'table' in the provided 'db'.  Using the provided
        // 'factory' to access the underlying database.  Optionally
        // specify the 'basicAllocator' used to supply memory.

    bcedb_DtagDbConstCursor(const bcem_Aggregate&  data,
                            const char            *table,
                            bcedb_DtagDbType      *db,
                            bslma_Allocator       *basicAllocator = 0);

    // PRIVATE MANIPULATORS
    void invalidate();
        // Make this cursor invalid.

  public:
    // CREATORS
    ~bcedb_DtagDbConstCursor();
        // Destroy this 'bcedb_DtagDbConstCursor' object.

    // MANIPULATORS
    bcedb_ConstCursor& next();
        // Advance this cursor to refer to the next record that matches the
        // search criteria.  Return a modifiable reference to this object.
        // Calling 'next()' on the last record in the search will return
        // an invalid cursor.  The behavior is undefined is the cursor is
        // invalid.

    bcedb_ConstCursor& prev();
        // Advance this cursor to refer to the previous row that matches
        // the search criteria.  Return a modifiable reference to this
        // object.  The behavior is undefined is the cursor is invalid.

    // ACCESSORS
    bcem_Aggregate data() const;
        // Return the 'data' currently associated with this cursor.

    bool isFirst() const;
        // Return 'true' if this cursor is the first cursor in the list.

    bool isLast() const;
        // Return 'true' if this cursor is the last cursor in the list.

    bool isDeleted() const;
        // Return 'true' if this cursor has been deleted.

    operator bool() const;
        // Return true if the cursor is valid and false otherwise.
};

                        // ========================
                        // class bcedb_DtagDbCursor
                        // ========================

class bcedb_DtagDbCursor : public bcedb_Cursor {
    // Provide a modifiable access cursor to the underlying data
    // for a specific table in the underlying database object.
    // The cursor can be moved forward or backward to access the
    // results of a query.
    //
    // DEPRECATED: use 'bsidb2_cursor' instead.

    // DATA
    bcedb_DtagDbType                        *d_db;
    bdema_ManagedPtr<bcedb_DtagBindingType>  d_tag;  // tag must be deleted
                                                     // after control!
    bdema_ManagedPtr<bcedb_DtagControlType>  d_control;
    bcem_Aggregate                           d_aggregate;
    bool                                     d_isFirst;
    bool                                     d_isLast;
    bool                                     d_isDeleted;

    // FRIENDS
    friend class bcedb_DtagDb;

    // NOT IMPLEMENTED
    bcedb_DtagDbCursor(const bcedb_DtagDbCursor&);
    bcedb_DtagDbCursor& operator=(const bcedb_DtagDbCursor&);

  private:
    // PRIVATE CREATORS
    bcedb_DtagDbCursor(
                    bcema_SharedPtr<const bdem_RecordDef>  definition,
                    const char                            *table,
                    bcedb_DtagDbType                      *db,
                    bslma_Allocator                       *basicAllocator = 0);
        // Create a cursor whose data conforms to the specified 'definition'
        // for the specified 'table' in the provided 'db'.  Using the provided
        // 'factory' to access the underlying database.  Optionally
        // specify the 'basicAllocator' used to supply memory.

    bcedb_DtagDbCursor(const bcem_Aggregate&  data,
                       const char            *table,
                       bcedb_DtagDbType      *db,
                       bslma_Allocator       *basicAllocator = 0);

    // PRIVATE MANIPULATORS
    void invalidate();
        // Make this cursor invalid.

    // PRIVATE ACCESSORS
    bcedb_DtagControlType& getControl();

  public:
    // CREATORS
    ~bcedb_DtagDbCursor();
        // Destroy this 'bcedb_DtagDbCursor' object.

    // MANIPULATORS
    bcedb_Cursor& next();
        // Advance this cursor to refer to the next record that matches the
        // search criteria.  Return a modifiable reference to this object.
        // Calling 'next()' on the last record in the search will return
        // an invalid cursor.  The behavior is undefined is the cursor is
        // invalid.

    bcedb_Cursor& prev();
        // Advance this cursor to refer to the previous row that matches
        // the search criteria.  Return a modifiable reference to this
        // object.  The behavior is undefined is the cursor is invalid.

    int update(const bcem_Aggregate& newRecord);
        // Update this record with the specified 'newRecord'.  Return 0
        // on success and non-zero otherwise.  The behavior is undefined
        // if the record definition does not conform to the schema for the
        // specified 'table' in this database.

    int remove();
        // Remove the record pointed to by this cursor.  The cursor remains
        // valid, but the record is invalid and 'isDeleted()' will return
        // 'true' for this cursor.  Return 0 on success, and non-zero
        // otherwise.  The behavior is undefined if this object does not point
        // to a valid position in the table.

    // ACCESSORS
    bcem_Aggregate data() const;
        // Return the 'data' currently associated with this cursor.

    bool isFirst() const;
        // Return 'true' if this cursor is the first cursor in the list.

    bool isLast() const;
        // Return 'true' if this cursor is the last cursor in the list.

    bool isDeleted() const;
        // Return 'true' if this cursor has been deleted.

    operator bool() const;
        // Return true if the cursor is valid and false otherwise.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
