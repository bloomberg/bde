// bcedb_incoredtagfactory.h        -*-C++-*-
#ifndef INCLUDED_BCEDB_INCOREDTAGFACTORY
#define INCLUDED_BCEDB_INCOREDTAGFACTORY

//@PURPOSE: Provide an in-core db factory conforming to 'bcedb_dtagfactory'.
//
//@CLASSES:
//    bcedb_InCoreDtagFactory: provides factory for dynamic tag db objects
//     bcedb_InCoreDtagDbType: provides factory for for a DB binding to this db
//bcedb_InCoreDtagBindingType: provides factory for controls to this binding
//bcedb_InCoreDtagControlType: provides access to records in the database
//
//@AUTHOR: Nathan Dorfman (ndorfman@bloomberg.net)
//
//@DESCRIPTION: This component implements a concrete in-core dynamic
//  tag implementation.  Specifically, this component allows for the testing of
//  a dynamic tag implementation of 'bcedb_Interface' without a real COMDB2
//  database.
//
///Usage
///-----
// In this example, we will create a schema for an InCoreDtagDbFactory
// defining two databases and three tables of different configurations.
// We start by creating the first table:
//..
// // Create the shared pointer and schema object:
// bcema_SharedPtr<bdem_Schema> schema();
// schema.createInPlace(allocator);
//
// // Table records can be unnamed, as can all but the top-level
// // ``database'' records:
// bdem_RecordDef *dinnerTable = schema->createRecord();
// dinnerTable->appendField(bdem_ElemType::DOUBLE, "spoon");
// dinnerTable->appendField(bdem_ElemType::INT,    "fork");
// dinnerTable->appendField(bdem_ElemType::STRING, "knife");
//
// // Next, we create two keys (indexes) on the dinner table:
// bdem_RecordDef *dinnerKey1 = schema->createRecord();
// dinnerKey1->appendField(bdem_ElemType::STRING,  "knife");
//
// bdem_RecordDef *dinnerKey2 = schema->createRecord();
// dinnerKey1->appendField(bdem_ElemType::INT,     "fork");
// dinnerKey1->appendField(bdem_ElemType::DOUBLE,  "spoon");
//
// // Next, we create a record to contain the table definition (the record
// // definition referred to by 'dinnerTable', created above) and all of
// // its keys (the record definition referred to by 'dinnerKey1' and
// // 'dinnerKey2'). The names of the fields in this record will be used
// // to look up keys by name through the interface of these classes.
// // IMPORTANTLY, the table definition goes at field index 0:
//
// bdem_RecordDef *dinnerTblInfo = schema->createRecord();
// dinnerTblInfo->appendField(bdem_ElemType::TABLE, dinnerTable, "table");
// dinnerTblInfo->appendField(bdem_ElemType::LIST, dinnerKey1, "knife_key");
// dinnerTblInfo->appendField(bdem_ElemType::LIST, dinnerKey2, "spork_key");
//
// // Now we are ready to create the database record. The name of this
// // record will be used to look up the database by the createDb() method
// // of this factory class, and the names of the constrained list fields in
// // this record will be used to look up table names by the createBinding()
// // method:
//
// bdem_RecordDef *db1 = schema->createRecord("diningRoomDB");
// db1->appendField(bdem_ElemType::LIST, dinnerTblInfo, "dinner");
//
// // Of course, a database can have more than one table, and the factory
// // is capable of creating more than one database. So we will add another
// // database containing two tables to this schema. For simplicity, these
// // two tables will only have one key each:
//
// bdem_RecordDef *coffeeTable = schema->createRecord();
// coffeeTable->appendField(bdem_ElemType::INT,    "saucer");
// coffeeTable->appendField(bdem_ElemType::DOUBLE, "cup");
// coffeeTable->appendField(bdem_ElemType::STRING, "magazine");
//
// bdem_RecordDef *coffeeKey = schema->createRecord();
// coffeeKey->appendField(bdem_ElemType::STRING,   "magazine");
//
// bdem_RecordDef *coffeeTblInfo = schema->createRecord();
// coffeeTblInfo->appendField(bdem_ElemType::TABLE, coffeeTable, "table");
// coffeeTblInfo->appendField(bdem_ElemType::LIST, coffeeKey, "magkey");
//
// bdem_RecordDef *pokerTable = schema->createRecord();
// pokerTable->appendField(bdem_ElemType::STRING,  "winner");
// pokerTable->appendField(bdem_ElemType::INT,     "cards");
// pokerTable->appendField(bdem_ElemType::DOUBLE,  "cash");
//
// bdem_RecordDef *pokerKey = schema->createRecord();
// pokerTable->appendField(bdem_ElemType::INT,     "cards");
//
// bdem_RecordDef *pokerTblInfo = schema->createRecord();
// pokerTblInfo->appendField(bdem_ElemType::TABLE, pokerTable, "table");
// pokerTblInfo->appendField(bdem_ElemType::LIST, pokerKey, "cardkey");
//
// bdem_RecordDef *db2 = schema->createRecord("livingRoomDB");
// db2->appendField(bdem_ElemType::LIST, coffeeTblInfo, "coffee");
// db2->appendField(bdem_ElemType::LIST, pokerTblInfo, "poker");
//
// // We can now use the Factory and other classes in this component:
//
// bcedb_InCoreDtagFactory factory(schema, allocator);
// bcedb_InCoreDtagDbType *dbHandle1 = factory.createDb("livingRoomDB");
//
// std::vector<std::string> dbTables1;
// dbHandle1->getDbTables(&dbTables1);
// ASSERT(dbTables[0] == "coffee");
// ASSERT(dbTables[1] == "poker");
//
// bcedb_InCoreDtagBindingType *coffeeBinding1 =
//         dbHandle1->createBinding("coffee");
//
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

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

#ifndef INCLUDED_STRING
#include <string>
#define INCLUDED_STRING
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

namespace BloombergLP {

class bcedb_InCoreDtagDbType;
class bcedb_InCoreDtagBindingType;
class bcedb_InCoreDtagControlType;

                        // =============================
                        // class bcedb_InCoreDtagFactory
                        // =============================
class bcedb_InCoreDtagFactory : public bcedb_DtagFactory
{
    // TBD: document

    bdema_Allocator                         *d_allocator_p;
    bcem_Aggregate                          d_data;
    std::vector<bcedb_InCoreDtagDbType *>   d_dbs;

  public:
    // CREATORS
    bcedb_InCoreDtagFactory(
            const bcema_SharedPtr<const bdem_RecordDef>& schemaShp,
            bdema_Allocator *allocator = 0);
        // Create a 'bcedb_InCoreDtagFactory' using the specified
        // schema to describe the virtual databases and tables.

    virtual ~bcedb_InCoreDtagFactory();
        // Destroy this object

    // MANIPULATORS
    bcedb_DtagDbType *createDb(const char *dbName);
        // Create a 'bcedb_DtagDbType' object connected to the database with the
        // specified 'dbName'.  Return a modifiable pointer to
        // the newly created object or 0 if the object could not be created.
        // This object must be destroyed with 'destroyDb(bcedb_DtagDbType *db)'.

    void destroyDb(bcedb_DtagDbType *db);
        // Destroy a 'bcedb_DtagDbType' object.  The behavior is undefined if
        // this 'bcedb_DtagDbType' object was not created by the factory used
        // to do the destruction.

};

                        // ============================
                        // class bcedb_InCoreDtagDbType
                        // ============================

class bcedb_InCoreDtagDbType : public bcedb_DtagDbType
{
    bdema_Allocator                             *d_allocator_p;
    bcem_Aggregate                              d_data;
    std::vector<bcedb_InCoreDtagBindingType *>  d_bindings;

  private:
    // PRIVATE CREATORS
    bcedb_InCoreDtagDbType(
            bcem_Aggregate tableData,
            bdema_Allocator *basicAllocator = 0);

    friend class bcedb_InCoreDtagFactory;

    // PRIVATE MANIPULATORS
    void *getHandle();
        // Return an opaque handle to this 'bcedb_DtagDbType' object.

  public:
    bcedb_DtagBindingType *createBinding(const char *tableName);
        // Create a 'bcedb_DtagBindingType' object associated with the specified
        // 'tableName' and return a modifiable pointer  to the newly created
        // object or 0 if the object could not be created.  This object must
        // be destroyed with 'destroyDtag(bcedb_DtagBindingType *db)'.

    void destroyBinding(bcedb_DtagBindingType *binding);
        // Destroy a 'bcedb_DtagBindingType' object.  The behavior is undefined
        // if this 'bcedb_DtagBindingType' object was not created by the
        // factory used  to do the destruction.

    virtual ~bcedb_InCoreDtagDbType();
        // Destroy this object

    // ACCESSORS
    void getDbTables(std::vector<std::string> *tables) const;
        // Populate the specified 'tables' with the list of tables available
        // in this db.

    void getDbKeys(bdem_Schema *keyInfo,
                           const char *table) const;
        // Populate the specified 'keyInfo' with record definitions that
        // represent the available index keys for the specified 'table'.
        // The record names identify the key names and the record elements
        // represent the fields used by the key.

    void appendOnDiskTagInfo(bdem_Schema *tagInfo,
                                  const char *table) const;
        // Apeend to the end of the specified 'tagInfo', the 'on disk' schema
        // a record definition representing  the specified 'table'.  The
        // 'tagInfo' record will share the same name as 'table'.

    void printData(std::ostream& os);
        // Print the current in-core database to the specified 'os'.

};

                        // =================================
                        // class bcedb_InCoreDtagBindingType
                        // =================================

class bcedb_InCoreDtagBindingType : public bcedb_DtagBindingType
{
    bdema_Allocator                            *d_allocator_p;
    bcem_Aggregate                             *d_bound;
    std::vector<bcedb_InCoreDtagControlType *>  d_controls;
    bcem_Aggregate                              d_table;
    int                                         d_rowIndex;
    bcema_SharedPtr<const bdem_RecordDef>       d_keys;
    //bcema_SharedPtr<bdem_RecordDef> d_rowDef;
    //bcema_SharedPtr<bdem_List>      d_list;
    //bcedb_InCoreDtagDbType         *d_db;

  private:
    // PRIVATE CREATORS
    bcedb_InCoreDtagBindingType(
            const bcem_Aggregate& table,
            const bcema_SharedPtr<const bdem_RecordDef>& keys,
            bdema_Allocator *allocator = 0);
        // TBD: document

    friend class bcedb_InCoreDtagDbType;
    friend class bcedb_InCoreDtagControlType;

    void *getHandle();
        // Return an opaque handle to this 'bcedb_DtagBindingType' object.

  public:
    virtual ~bcedb_InCoreDtagBindingType();
        // Destroy this object

    // MANIPULATORS
    bcedb_DtagControlType *createControl();
        // Create a 'bcedb_DtagControlType' object for this binding object
        // for the database assocatied with this object.  Return a
        // modifiable pointer to the newly created object or 0 if the object
        // could not be created.  This object must be destroyed with
        // 'destroyControl(bcedb_DtagControlType *db)'.

    void destroyControl(bcedb_DtagControlType *control);
        // Destroy a 'bcedb_DtagControlType' object.  The behavior is undefined
        // if  this 'bcedb_DtagControlType' object was not created by the
        // factory used  to do the destruction.

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

    int maxLength(const char *fieldName) const;
        // Return the maximum length allowed for the specified 'fieldName'
        // field.  Return a value less than zero if fieldName is variable
        // length (blob) and 0 if 'fieldName' is not a valid string-type field.
};

                        // =================================
                        // class bcedb_InCoreDtagControlType
                        // =================================

class bcedb_InCoreDtagControlType : public bcedb_DtagControlType
{
    bdema_Allocator                *d_allocator_p;
    bcedb_InCoreDtagBindingType    *d_binding_p;
    mutable bcem_Aggregate          d_search;
    mutable std::vector<int>        d_partials;
    mutable int                     d_position;
    mutable bcedb_DtagBindingType  *d_range;

    // PRIVATE CREATORS
    bcedb_InCoreDtagControlType(
            bcedb_InCoreDtagBindingType *binding,
            bdema_Allocator *allocator = 0);

    friend class bcedb_InCoreDtagBindingType;

  public:
    void *getHandle();
        // Return an opaque handle to this 'bcedb_DtagControlType' object.

    virtual ~bcedb_InCoreDtagControlType();
        // Destroy this object

    // MANIPULATORS
    int addRecordToDb();
        // Add the data associated with the underlying tag binding
        // to the table associated with this control.  Return 0 on success
        // and non-zero otherwise.

    int updateRecordInDb();
        // Update the record at the specified 'control' with the data
        // assocaited with the underlying 'bcedb_DtagBinding' object.  Return 0
        // on success and non-zero otherwise.

    int deleteRecordFromDb();
        // Delete the record pointed to by this control. Return 0 on success
        // and non-zero otherwise.

    // ACCESSORS
    bcedb_DtagControlType::FIND_STATUS_CODE findRecordInDb(const char *key,
                                         const char *partialCode = 0) const;
        // Find the record in the db using the underlying tag as input
        // using only the fields from the specified 'key' for the lookup.
        // Populate the same underlying tag binding with the result of the
        // find if successful.  Return the status of the find as one of
        // 'FIND_STATUS_CODE'.  Optionally specify 'partialCode' to indicate
        // the partial search key where "" means a zero length search. The
        // partial search key should take the form:
        // keyfield1[:num]+keyfield2[:num]+...+keyfieldn[:num]
        // where keyfield1-n are any number of fields in the specified
        // 'key' and num is optional specified to indicate the number of
        // characters to use for the search.

    bcedb_DtagControlType::FIND_STATUS_CODE findNextRecord() const;
        // Find the record after the one pointed to by this control
        // that matches the search criteria.  Return the appropriate
        // 'FIND_STATUS_CODE'.

    bcedb_DtagControlType::FIND_STATUS_CODE findRangeInDb(
            bcedb_DtagBindingType *endRecord,
            const char *key,
            const char *partialCode = 0) const;
        // Find the range of records in the db using the underlying binding as
        // the first record and the specified 'endRecord' to indicate the
        // range. Using only the fields from the specified 'key' for the
        // lookup.  Populate the same underlying binding with the result of
        // the find if successful.  Return the status of the find as one of
        // 'FIND_STATUS_CODE'.  Optionally specify 'partialCode' to indicate
        // the partial search key where "" means a zero length search. The
        // partial search key should take the form:
        // keyfield1[:num]+keyfield2[:num]+...+keyfieldn[:num]
        // where keyfield1-n are any number of fields in the specified
        // 'key' and num is optional specified to indicate the number of
        // characters to use for the search.

    bcedb_DtagControlType::FIND_STATUS_CODE findPrevRecord() const;
        // Find the record before the record pointed to by this control
        // that matches the search criteria.  Return the appropriate
        // 'FIND_STATUS_CODE'.


};

}
#endif
