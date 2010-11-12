// bcedb_incoredtagfactory.h        -*-C++-*-
#ifndef INCLUDED_BCEDB_INCOREDTAGFACTORY
#define INCLUDED_BCEDB_INCOREDTAGFACTORY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an in-core db factory conforming to 'bcedb_dtagfactory'.
//
//@DEPRECATED: Use 'bsidb2_testdynamictagdriver' instead.
//
//@CLASSES:
//    bcedb_InCoreDtagFactory: provides factory for dynamic tag db objects
//     bcedb_InCoreDtagDbType: provides factory for for a DB binding to this db
//bcedb_InCoreDtagBindingType: provides factory for controls to this binding
//bcedb_InCoreDtagControlType: provides access to records in the database
//
//@AUTHOR: Nathan Dorfman (ndorfman)
//
//@DESCRIPTION: [!DEPRECATED!] This component implements a concrete in-core
// dynamic tag implementation.  Specifically, this component allows for the
// testing of a dynamic tag implementation of 'bcedb_Interface' without a real
// COMDB2 database.
//
// This component has been deprecated, clients should instead use the types
// defined in 'bsidb2_dyanamictagservice' and 'bsidb2_dynamictagdriver'
// (and the corresponding in-core test implementation in
// 'bsidb2_testdynamictagdriver').
//
///Usage
///-----
// In this example, we will create a schema for an InCoreDtagDbFactory
// defining two databases and three tables of different configurations.
// We start by creating the first table.  First create the shared pointer and
// schema object:
//..
//  bcema_SharedPtr<bdem_Schema> schema();
//  schema.createInplace(allocator);
//..
// Table records can be unnamed, as can all but the top-level ``database''
// records:
//..
//  bdem_RecordDef *dinnerTable = schema->createRecord();
//  dinnerTable->appendField(bdem_ElemType::BDEM_DOUBLE, "spoon");
//  dinnerTable->appendField(bdem_ElemType::BDEM_INT,    "fork");
//  dinnerTable->appendField(bdem_ElemType::BDEM_STRING, "knife");
//..
// Next, we create two keys (indexes) on the dinner table:
//..
//  bdem_RecordDef *dinnerKey1 = schema->createRecord();
//  dinnerKey1->appendField(bdem_ElemType::BDEM_STRING,  "knife");
//
//  bdem_RecordDef *dinnerKey2 = schema->createRecord();
//  dinnerKey1->appendField(bdem_ElemType::BDEM_INT,     "fork");
//  dinnerKey1->appendField(bdem_ElemType::BDEM_DOUBLE,  "spoon");
//..
// Next, we create a record to contain the table definition (the record
// definition referred to by 'dinnerTable', created above) and all of
// its keys (the record definition referred to by 'dinnerKey1' and
// 'dinnerKey2').  The names of the fields in this record will be used
// to look up keys by name through the interface of these classes.
// IMPORTANTLY, the table definition goes at field index 0:
//..
//  bdem_RecordDef *dinnerTblInfo = schema->createRecord();
//  dinnerTblInfo->appendField(bdem_ElemType::BDEM_TABLE, dinnerTable,
//                                                                    "table");
//  dinnerTblInfo->appendField(bdem_ElemType::BDEM_LIST, dinnerKey1,
//                                                                "knife_key");
//  dinnerTblInfo->appendField(bdem_ElemType::BDEM_LIST, dinnerKey2,
//                                                                "spork_key");
//..
// Now we are ready to create the database record.  The name of this
// record will be used to look up the database by the createDb() method
// of this factory class, and the names of the constrained list fields in
// this record will be used to look up table names by the createBinding()
// method:
//..
//  bdem_RecordDef *db1 = schema->createRecord("diningRoomDB");
//  db1->appendField(bdem_ElemType::BDEM_LIST, dinnerTblInfo, "dinner");
//..
// Of course, a database can have more than one table, and the factory
// is capable of creating more than one database.  So we will add another
// database containing two tables to this schema.  For simplicity, these
// two tables will only have one key each:
//..
//  bdem_RecordDef *coffeeTable = schema->createRecord();
//  coffeeTable->appendField(bdem_ElemType::BDEM_INT,    "saucer");
//  coffeeTable->appendField(bdem_ElemType::BDEM_DOUBLE, "cup");
//  coffeeTable->appendField(bdem_ElemType::BDEM_STRING, "magazine");
//
//  bdem_RecordDef *coffeeKey = schema->createRecord();
//  coffeeKey->appendField(bdem_ElemType::BDEM_STRING,   "magazine");
//
//  bdem_RecordDef *coffeeTblInfo = schema->createRecord();
//  coffeeTblInfo->appendField(bdem_ElemType::BDEM_TABLE, coffeeTable,
//                                                                    "table");
//  coffeeTblInfo->appendField(bdem_ElemType::BDEM_LIST, coffeeKey, "magkey");
//
//  bdem_RecordDef *pokerTable = schema->createRecord();
//  pokerTable->appendField(bdem_ElemType::BDEM_STRING,  "winner");
//  pokerTable->appendField(bdem_ElemType::BDEM_INT,     "cards");
//  pokerTable->appendField(bdem_ElemType::BDEM_DOUBLE,  "cash");
//
//  bdem_RecordDef *pokerKey = schema->createRecord();
//  pokerTable->appendField(bdem_ElemType::BDEM_INT,     "cards");
//
//  bdem_RecordDef *pokerTblInfo = schema->createRecord();
//  pokerTblInfo->appendField(bdem_ElemType::BDEM_TABLE, pokerTable, "table");
//  pokerTblInfo->appendField(bdem_ElemType::BDEM_LIST, pokerKey, "cardkey");
//
//  bdem_RecordDef *db2 = schema->createRecord("livingRoomDB");
//  db2->appendField(bdem_ElemType::BDEM_LIST, coffeeTblInfo, "coffee");
//  db2->appendField(bdem_ElemType::BDEM_LIST, pokerTblInfo, "poker");
//..
// We can now use the Factory and other classes in this component:
//..
//  bcedb_InCoreDtagFactory factory(schema, allocator);
//  bcedb_InCoreDtagDbType *dbHandle1 = factory.createDb("livingRoomDB");
//
//  bsl::vector<bsl::string> dbTables1;
//  dbHandle1->getDbTables(&dbTables1);
//  ASSERT(dbTables[0] == "coffee");
//  ASSERT(dbTables[1] == "poker");
//
//  bcedb_InCoreDtagBindingType *coffeeBinding1 =
//          dbHandle1->createBinding("coffee");
//..
// TBD: describe further

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEDB_DTAGFACTORY
#include <bcedb_dtagfactory.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEM_AGGREGATE
#include <bcem_aggregate.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif


namespace BloombergLP {

class bcedb_InCoreDtagBindingType;
class bcedb_InCoreDtagControlType;
class bcedb_InCoreDtagDbType;

                        // =============================
                        // class bcedb_InCoreDtagFactory
                        // =============================

class bcedb_InCoreDtagFactory : public bcedb_DtagFactory {
    // DEPRECATED: use 'bsidb2_dynamictagservice' and
    // 'bsidb2_testdynamictagdriver' instead.

    // DATA
    bcem_Aggregate                         d_data;
    bsl::vector<bcedb_InCoreDtagDbType *>  d_dbs;
    bslma_Allocator                       *d_allocator_p;

    // NOT IMPLEMENTED
    bcedb_InCoreDtagFactory(const bcedb_InCoreDtagFactory&);
    bcedb_InCoreDtagFactory& operator=(const bcedb_InCoreDtagFactory&);

  public:
    // CREATORS
    bcedb_InCoreDtagFactory(
            const bcema_SharedPtr<const bdem_RecordDef>&  schemaShp,
            bslma_Allocator                              *allocator = 0);
        // Create a 'bcedb_InCoreDtagFactory' using the specified
        // schema to describe the virtual databases and tables.

    virtual ~bcedb_InCoreDtagFactory();
        // Destroy this object

    // MANIPULATORS
    bcedb_DtagDbType *createDb(const char *dbName);
        // Create a 'bcedb_DtagDbType' object connected to the database with
        // the specified 'dbName'.  Return a modifiable pointer to
        // the newly created object or 0 if the object could not be created.
        // This object must be destroyed with
        // 'destroyDb(bcedb_DtagDbType *db)'.

    void destroyDb(bcedb_DtagDbType *db);
        // Destroy a 'bcedb_DtagDbType' object.  The behavior is undefined if
        // this 'bcedb_DtagDbType' object was not created by the factory used
        // to do the destruction.
};

                        // ============================
                        // class bcedb_InCoreDtagDbType
                        // ============================

class bcedb_InCoreDtagDbType : public bcedb_DtagDbType {
    // DEPRECATED: use 'bsidb2_dynamictagservice' and
    // 'bsidb2_testdynamictagdriver' instead.

    // DATA
    bcem_Aggregate                              d_data;
    bsl::vector<bcedb_InCoreDtagBindingType *>  d_bindings;
    bslma_Allocator                            *d_allocator_p;

    // FRIENDS
    friend class bcedb_InCoreDtagFactory;

    // NOT IMPLEMENTED
    bcedb_InCoreDtagDbType(const bcedb_InCoreDtagDbType&);
    bcedb_InCoreDtagDbType& operator=(const bcedb_InCoreDtagDbType&);

  private:
    // PRIVATE CREATORS
    bcedb_InCoreDtagDbType(const bcem_Aggregate&  tableData,
                           bslma_Allocator       *basicAllocator = 0);

    // PRIVATE MANIPULATORS
    void *getHandle();
        // Return an opaque handle to this 'bcedb_DtagDbType' object.

  public:
    // CREATORS
    virtual ~bcedb_InCoreDtagDbType();
        // Destroy this object

    // MANIPULATORS
    bcedb_DtagBindingType *createBinding(const char *tableName);
        // Create a 'bcedb_DtagBindingType' object associated with the
        // specified 'tableName' and return a modifiable pointer to the newly
        // created object or 0 if the object could not be created.  This object
        // must be destroyed with 'destroyDtag(bcedb_DtagBindingType *db)'.

    void destroyBinding(bcedb_DtagBindingType *binding);
        // Destroy a 'bcedb_DtagBindingType' object.  The behavior is undefined
        // if this 'bcedb_DtagBindingType' object was not created by the
        // factory used to do the destruction.

    // ACCESSORS
    void getDbTables(bsl::vector<bsl::string> *tables) const;
        // Populate the specified 'tables' with the list of tables available
        // in this db.

    void getDbKeys(bdem_Schema *keyInfo,
                   const char  *table) const;
        // Populate the specified 'keyInfo' with record definitions that
        // represent the available index keys for the specified 'table'.
        // The record names identify the key names and the record elements
        // represent the fields used by the key.

    void appendOnDiskTagInfo(bdem_Schema *tagInfo,
                             const char  *table) const;
        // Append to the end of the specified 'tagInfo', the 'on disk' schema
        // a record definition representing the specified 'table'.  The
        // 'tagInfo' record will share the same name as 'table'.

    void printData(bsl::ostream& os) const;
        // Print the current in-core database to the specified 'os'.
};

                        // =================================
                        // class bcedb_InCoreDtagBindingType
                        // =================================

class bcedb_InCoreDtagBindingType : public bcedb_DtagBindingType {
    // DEPRECATED: use 'bsidb2_dynamictagservice' and
    // 'bsidb2_testdynamictagdriver' instead.

    // PRIVATE TYPES
    typedef bsl::map<bsl::string, bsl::string>  StrToStrMap;


    // DATA
    bcem_Aggregate                             *d_bound;
    bsl::vector<bcedb_InCoreDtagControlType *>  d_controls;
    bcem_Aggregate                              d_table;
    int                                         d_rowIndex;
    bcema_SharedPtr<const bdem_RecordDef>       d_keys;
    bslma_Allocator                            *d_allocator_p;

    // FRIENDS
    friend class bcedb_InCoreDtagDbType;
    friend class bcedb_InCoreDtagControlType;

    // NOT IMPLEMENTED
    bcedb_InCoreDtagBindingType(const bcedb_InCoreDtagBindingType&);
    bcedb_InCoreDtagBindingType& operator=(const bcedb_InCoreDtagBindingType&);

  private:
    // PRIVATE CREATORS
    bcedb_InCoreDtagBindingType(
                  const bcem_Aggregate&                         table,
                  const bcema_SharedPtr<const bdem_RecordDef>&  keys,
                  bslma_Allocator                              *allocator = 0);
        // TBD: document

    // PRIVATE MANIPULATORS
    void *getHandle();
        // Return an opaque handle to this 'bcedb_DtagBindingType' object.

  public:
    // CREATORS
    virtual ~bcedb_InCoreDtagBindingType();
        // Destroy this object

    // MANIPULATORS
    bcedb_DtagControlType *createControl();
        // Create a 'bcedb_DtagControlType' object for this binding object
        // for the database associated with this object.  Return a
        // modifiable pointer to the newly created object or 0 if the object
        // could not be created.  This object must be destroyed with
        // 'destroyControl(bcedb_DtagControlType *db)'.

    void destroyControl(bcedb_DtagControlType *control);
        // Destroy a 'bcedb_DtagControlType' object.  The behavior is undefined
        // if this 'bcedb_DtagControlType' object was not created by the
        // factory used to do the destruction.

    int bind(bcem_Aggregate *tag);
        // Create a binding between this 'bcedb_DtagBindingType' and the
        // members of the list in the specified 'tag' using its record
        // definition to build the binding.  This 'tag' object's life span
        // must be greater than that of the 'bcedb_DtagBindingType' object.
        // Return 0 on success and/ non-zero otherwise.

    int updateData(const bcem_Aggregate& newData);
        // Update the data associated with this tag with the specified
        // 'newData'

    // ACCESSORS
    const bcem_Aggregate *getData() const;
        // Return the data currently associated with this tag.

    const char *getSchemaFieldName(const char *fieldName) const;
        // Return the specified 'fieldName' in the case as described by the
        // schema, if found, and 0 otherwise.  Note that this function is
        // intended to resolve the difference (if any) in database access
        // between the case-sensitivity of 'bcem_Aggregate' when using
        // 'setField' and the (possible) case-insensitivity of the database
        // when using the database's API directly.  Also note that this
        // implementation simply returns 'fieldName'.

    int maxLength(const char *fieldName) const;
        // Return the maximum length allowed for the specified 'fieldName'
        // field.  Return a value less than zero if fieldName is variable
        // length (blob) and 0 if 'fieldName' is not a valid string-type field.
};

                        // =================================
                        // class bcedb_InCoreDtagControlType
                        // =================================

class bcedb_InCoreDtagControlType : public bcedb_DtagControlType {
    // DEPRECATED: use 'bsidb2_dynamictagservice' and
    // 'bsidb2_testdynamictagdriver' instead.

    // DATA
    bcedb_InCoreDtagBindingType    *d_binding_p;
    mutable bcem_Aggregate          d_search;
    mutable bsl::vector<int>        d_partials;
    mutable int                     d_position;
    mutable bcedb_DtagBindingType  *d_range;
    bslma_Allocator                *d_allocator_p;

    // FRIENDS
    friend class bcedb_InCoreDtagBindingType;

    // NOT IMPLEMENTED
    bcedb_InCoreDtagControlType(const bcedb_InCoreDtagControlType&);
    bcedb_InCoreDtagControlType& operator=(const bcedb_InCoreDtagControlType&);

  private:
    // PRIVATE CREATORS
    bcedb_InCoreDtagControlType(
            bcedb_InCoreDtagBindingType *binding,
            bslma_Allocator *allocator = 0);

    // PRIVATE ACCESSORS
    FIND_STATUS_CODE
    findRangeInDbImp(int                   *count,
                     bcedb_DtagBindingType *endRecord,
                     const char            *key,
                     const char            *partialCode,
                     bool                   doRangeCount) const;
    // Private method that actually does the work for the range extract.  See
    // the 'findRangeInDb' method in this class for full documentation.
    // Depending on how the db is set up counts will either be free, or
    // monstrously expensive, so enabling them for all range extract calls
    // would be a bad idea.  If 'true == doRangeCount' then 'count' will be
    // modified to contain the number of records in the range, otherwise it is
    // not modified.

  public:
    // CREATORS
    virtual ~bcedb_InCoreDtagControlType();
        // Destroy this object

    // MANIPULATORS
    void *getHandle();
        // Return an opaque handle to this 'bcedb_DtagControlType' object.

    int addRecordToDb();
        // Add the data associated with the underlying tag binding
        // to the table associated with this control.  Return 0 on success
        // and non-zero otherwise.

    int updateRecordInDb();
        // Update the record at the specified 'control' with the data
        // associated with the underlying 'bcedb_DtagBinding' object.  Return 0
        // on success and non-zero otherwise.

    int deleteRecordFromDb();
        // Delete the record pointed to by this control.  Return 0 on success
        // and non-zero otherwise.

    // ACCESSORS
    FIND_STATUS_CODE findRecordInDb(const char *key,
                                    const char *partialCode = 0) const;
        // Find the record in the db using the underlying tag as input
        // using only the fields from the specified 'key' for the lookup.
        // Populate the same underlying tag binding with the result of the
        // find if successful.  Return the status of the find as one of
        // 'FIND_STATUS_CODE'.  Optionally specify 'partialCode' to indicate
        // the partial search key where "" means a zero length search.  The
        // partial search key should take the form:
        // keyfield1[:num]+keyfield2[:num]+...+keyfieldn[:num]
        // where keyfield1-n are any number of fields in the specified
        // 'key' and num is optional specified to indicate the number of
        // characters to use for the search.

    FIND_STATUS_CODE findNextRecord() const;
        // Find the record after the one pointed to by this control
        // that matches the search criteria.  Return the appropriate
        // 'FIND_STATUS_CODE'.

    FIND_STATUS_CODE findRangeInDb(bcedb_DtagBindingType *endRecord,
                                   const char            *key,
                                   const char            *partialCode = 0)
                                                                         const;
        // Populate the underlying binding with the result of the find, if
        // successful, otherwise the underlying binding is not modified.  The
        // range of the find is specified using the underlying binding (as the
        // first record) and the specified 'endRecord', matching all the fields
        // in the specified 'key' and the criteria provided by the optionally
        // specified 'partialCode'.  Return the status of the find as one of
        // 'FIND_STATUS_CODE'.  The partial search key should take the form:
        // keyfield1[:num]+keyfield2[:num]+...+keyfieldn[:num] where
        // keyfield1-n are any number of fields in the specified 'key' and num
        // is optional specified to indicate the number of characters to use
        // for the search, "" means a zero length search.

    FIND_STATUS_CODE findRangeInDb(
            int                   *count,
            bcedb_DtagBindingType *endRecord,
            const char            *key,
            const char            *partialCode = 0) const;
        // Populate the underlying binding with the result of the find and
        // modify the specified 'count' to contain the number of records in the
        // range, if successful, otherwise the underlying binding and count are
        // not modified.  The range of the find is specified using the
        // underlying binding (as the first record) and the specified
        // 'endRecord', matching all the fields in the specified 'key' and the
        // criteria provided by the optionally specified 'partialCode'.  Return
        // the status of the find as one of 'FIND_STATUS_CODE'.  The partial
        // search key should take the form:
        // keyfield1[:num]+keyfield2[:num]+...+keyfieldn[:num] where
        // keyfield1-n are any number of fields in the specified 'key' and num
        // is optional specified to indicate the number of characters to use
        // for the search, "" means a zero length search.

    FIND_STATUS_CODE findPrevRecord() const;
        // Find the record before the record pointed to by this control
        // that matches the search criteria.  Return the appropriate
        // 'FIND_STATUS_CODE'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------------------
                        // class bcedb_InCoreDtagControlType
                        // ---------------------------------

inline
bcedb_DtagControlType::FIND_STATUS_CODE
bcedb_InCoreDtagControlType::findRangeInDb(
                                      bcedb_DtagBindingType *endRecord,
                                      const char            *key,
                                      const char            *partialCode) const
{
    return findRangeInDbImp(0, endRecord, key, partialCode, false);
}

inline
bcedb_DtagControlType::FIND_STATUS_CODE
bcedb_InCoreDtagControlType::findRangeInDb(
                                      int                   *count,
                                      bcedb_DtagBindingType *endRecord,
                                      const char            *key,
                                      const char            *partialCode) const
{
    return findRangeInDbImp(count, endRecord, key, partialCode, true);
}

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
