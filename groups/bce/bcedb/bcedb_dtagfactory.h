// bcedb_dtagfactory.h        -*-C++-*-
#ifndef INCLUDED_BCEDB_DTAGFACTORY
#define INCLUDED_BCEDB_DTAGFACTORY

//@PURPOSE: Provide a pure protocol to a dynamic tag database api.
//
//@CLASSES:
//    bcedb_DtagFactory: protocol for providing dynamic tag objects.
//     bcedb_DtagDbType: protocol for a DB type. 
//bcedb_DtagBindingType: protocol for a DB dynamic tag binding type.
//bcedb_DtagControlType: protocol for a DB control type. 
//
//@AUTHOR: Wayne Barlow (wbarlow1@bloomberg.net)
//
//@DESCRIPTION: This component implements a pure protocol for a dynamic
//  tag api.  Specifically, this component allows for the testing of 
//  a COMDB2 implementation of 'bcedb_Interface' without a real COMDB2
//  database.  
#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif 

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

#ifndef INCLUDED_STRING
#include <string>
#define INCLUDED_STRING
#endif

namespace BloombergLP {

class bdem_Schema;
class bcem_Aggregate;

class bcedb_DtagDbType;
class bcedb_DtagBindingType;
class bcedb_DtagControlType;

                        // =======================
                        // class bcedb_DtagFactory
                        // =======================
class bcedb_DtagFactory
{
  // This factory class is pure protocol for handling the creation of 
  // 'bcedb_DtagDbType', 'bcedb_DtagControlType' and 'bcedb_DtagBindingType'  
  // objects.  The objects should be managed such that if the factory is 
  // destroyed, all comdb2 objects should be deallocated.

  public:
    virtual ~bcedb_DtagFactory();
        // Destroy this object

    virtual bcedb_DtagDbType *createDb(const char *dbName) = 0;
        // Create a 'bcedb_DtagDbType' object connected to the database with the
        // specified 'dbName'.  Return a modifiable pointer to
        // the newly created object or 0 if the object could not be created.
        // This object must be destroyed with 'destroyDb(bcedb_DtagDbType *db)'.

    virtual void destroyDb(bcedb_DtagDbType *db) = 0;
        // Destroy a 'bcedb_DtagDbType' object.  The behavior is undefined if 
        // this 'bcedb_DtagDbType' object was not created by the factory used 
        // to do the destruction.

};

                        // ======================
                        // class bcedb_DtagDbType
                        // ======================
class bcedb_DtagDbType
{
  public:
    // MANIPULATORS
    virtual void *getHandle() = 0;
        // Return an opaque handle to this 'bcedb_DtagDbType' object.

    virtual bcedb_DtagBindingType *createBinding(const char *tableName) = 0;
        // Create a 'bcedb_DtagBindingType' object associated with the specified
        // 'tableName' and return a modifiable pointer  to the newly created 
        // object or 0 if the object could not be created.  This object must 
        // be destroyed with 'destroyDtag(bcedb_DtagBindingType *db)'.

    virtual void destroyBinding(bcedb_DtagBindingType *binding) = 0;
        // Destroy a 'DtagBindingType' object.  The behavior is undefined if 
        // this 'DtagBindingType' object was not created by this object.

    virtual ~bcedb_DtagDbType();
        // Destroy this object

    // ACCESSORS
    virtual void getDbTables(std::vector<std::string> *tables) const = 0;
        // Populate the specified 'tables' with the list of tables available
        // in this db.
                            
    virtual void getDbKeys(bdem_Schema *keyInfo,
                           const char *table) const = 0;
        // Populate the specified 'keyInfo' with record definitions that
        // represent the available index keys for the specified 'table'. 
        // The record names identify the key names and the record elements
        // represent the fields used by the key.
                            
    virtual void appendOnDiskTagInfo(bdem_Schema *tagInfo,
                                  const char *table) const = 0; 
        // Apeend to the end of the specified 'tagInfo', the 'on disk' schema 
        // a record definition representing  the specified 'table'.  The 
        // 'tagInfo' record will share the same name as 'table'.
        
};

class bcedb_DtagBindingType
{
  public:  
    virtual ~bcedb_DtagBindingType();
        // Destroy this object

    // MANIPULATORS
    virtual void *getHandle() = 0;
        // Return an opaque handle to this 'bcedb_DtagBindingType' object.

    virtual bcedb_DtagControlType *createControl() = 0;
        // Create a 'bcedb_DtagControlType' object for this binding object
        // for the database assocatied with this object.  Return a 
        // modifiable pointer to the newly created object or 0 if the object 
        // could not be created.  This object must be destroyed with 
        // 'destroyControl(bcedb_DtagControlType *db)'.

    virtual void destroyControl(bcedb_DtagControlType *control) = 0;
        // Destroy a 'bcedb_DtagControlType' object.  The behavior is undefined
        // if  this 'bcedb_DtagControlType' object was not created by the 
        // factory used  to do the destruction.

    virtual int bind(bcem_Aggregate *data) = 0;
        // Create a binding between this 'bcedb_DtagBindingType' and the 
        // members of the list in the specified 'data' using its record 
        // definition to build the binding.  This 'data' object's lifetime  
        // must be greater than that of the 'bcedb_DtagBindingType' object.  
        // Return 0 on success and non-zero otherwise.

    virtual int updateData(const bcem_Aggregate& newData) = 0;
        // Update the data bound to this object with the specified
        // 'newData'.  Return 0 on success and non-zero otherwise.
    
    // ACCESSORS
    virtual const bcem_Aggregate *getData() const = 0;
        // Return a read only pointer to the data currently associated with 
        // this binding.

    virtual int maxLength(const char *fieldName) const = 0;
        // Return the maximum length allowed for the specified 'fieldName'
        // field.  Return a value less than zero if fieldName is variable
        // length and 0 if 'fieldName' is not a valid string-type field.

};

class bcedb_DtagControlType
{
  public:
     enum FIND_STATUS_CODE {
        FIND_STATUS_NONE = 0, 
        FIND_STATUS_ONE,
        FIND_STATUS_MANY
     };

    virtual void *getHandle() = 0;
        // Return an opaque handle to this 'bcedb_DtagControlType' object.

    virtual ~bcedb_DtagControlType();
        // Destroy this object

    // MANIPULATORS
    virtual int addRecordToDb() = 0;
        // Add the data associated with the underlying tag binding
        // to the table associated with this control.  Return 0 on success 
        // and non-zero otherwise.

    virtual int updateRecordInDb() = 0;
        // Update the record at the specified 'control' with the data 
        // assocaited with the underlying 'bcedb_DtagBinding' object.  Return 0 
        // on success and non-zero otherwise.

    virtual int deleteRecordFromDb() = 0;
        // Delete the record pointed to by this control. Return 0 on success
        // and non-zero otherwise.

    // ACCESSORS
    virtual FIND_STATUS_CODE findRecordInDb(const char *key, 
                                         const char *partialCode = 0) const = 0;
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

    virtual FIND_STATUS_CODE findRangeInDb(bcedb_DtagBindingType *endRecord,
                                         const char *key, 
                                         const char *partialCode = 0) const = 0;
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

    virtual FIND_STATUS_CODE findNextRecord() const = 0;
        // Find the record after the one pointed to by this control
        // that matches the search criteria.  Return the appropriate 
        // 'FIND_STATUS_CODE'. 

    virtual FIND_STATUS_CODE findPrevRecord() const = 0;
        // Find the record before the record pointed to by this control 
        // that matches the search criteria.  Return the appropriate 
        // 'FIND_STATUS_CODE'. 

};


}
#endif
