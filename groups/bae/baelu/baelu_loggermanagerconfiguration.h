// baelu_loggermanagerconfiguration.h         -*-C++-*-
#ifndef INCLUDED_BAELU_LOGGERMANAGERCONFIGURATION
#define INCLUDED_BAELU_LOGGERMANAGERCONFIGURATION

//@PURPOSE: Provide utility functions for logger manager configuration.
//
//@CLASSES:
//    baelu_LoggerManagerConfiguration: namespace for configuration utilities
//
//@AUTHOR: Steven Breitstein (sbreitst)
//
//@SEE_ALSO: bael_loggermanagerconfiguration
//
//@DESCRIPTION: This component provides a suite of pure procedures that
// complement the 'bael_LoggerManagerConfiguration' component.  In particular,
// the 'baelu_LoggerManagerConfiguration' 'struct' in this component provides a
// 'makeLmcOneInt' function that returns a 'bael_LoggerManagerConfiguration'
// object having a populator that populates log records according to a
// specified schema with the integer value at a specified address, and a
// 'isValidSchemaOneInt' procedure to validate the arguments passed to
// 'makeLmcOneInt'.
//
///Synopsis
///--------
// The following is a list of 'static' functions available in the
// 'baelu_LoggerManagerConfiguration' 'struct' of this component:
//..
//    bool isValidSchemaOneInt(const bdem_Schema&  schema,
//                             const char         *recordName,
//                             const char         *fieldName);
//
//    bael_LoggerManagerConfiguration makeLmcOneInt(
//                                     const bdem_Schema&  schema,
//                                     const char         *recordName,
//                                     const char         *fieldName,
//                                     const int          *value,
//                                     bdema_Allocator    *basicAllocator = 0);
//..
///Usage
///-----
// The following snippets of code illustrate how to use 'isValidSchemaOneInt'
// and 'makeLmcOneInt' to compose a configuration for a logger manager that
// adds to each log record the integer value at a specified address.  First we
// create the required 'schema':
//..
//    // Create 'schema' for 'populator'.
//    bdem_Schema schema;
//    bdem_RecordDef *record = schema.createRecord("ROOT-RECORD");
//    record->appendField(bdem_ElemType::INT, "luw");
//    assert(baelu_LoggerManagerConfiguration::isValidSchemaOneInt(
//                                                               schema,
//                                                               "ROOT-RECORD",
//                                                               "luw"));
//..
// Next we extract and record 'fieldIndex' for later use:
//..
//    const int fieldIndex = record->fieldIndex("luw");
//    assert(0 <= fieldIndex);
//..
// Next we define and initialize an integer with a known value:
//..
//    int referenceValue = 0xcafecafe;
//..
// Now we use the 'schema' constructed above, the record and field names
// ("ROOT-RECORD" and "luw", respectively), and our 'referenceValue' to compose
// the logger manager configuration:
//..
//    // Create configuration.
//    bael_LoggerManagerConfiguration lmc =
//            baelu_LoggerManagerConfiguration::makeLmcOneInt(schema,
//                                                            "ROOT-RECORD",
//                                                            "luw",
//                                                            &referenceValue);
//..
// The following code confirms that the schema in the returned configuration
// matches the schema supplied to 'makeLmcOneInt':
//..
//    const bdem_Schema& userSchema = lmc.userSchema();
//    assert(schema == userSchema);
//..
// To test that the populator in that configuration behaves correctly we obtain
// a reference to it and exercise it in the same manner as it may be used
// (indirectly) by an instance of 'bael_Logger':
//..
//    const bdef_Vfunc2<bdem_List *, bdem_Schema>& populator
//                                                       = lmc.userPopulator();
//    bdem_List list;
//    populator(&list, schema);
//..
// The following code confirms that the expected value has been added to 'list'
// by 'populator':
//..
//    int testValue = list.theInt(fieldIndex);
//    assert(referenceValue == testValue);
//..
// Now we update 'referenceValue' and re-engage 'populator':
//..
//    ++referenceValue;
//    populator(&list, schema);
//..
// Lastly we confirm that 'list' now holds the new value in 'referenceValue':
//..
//    testValue = list.theInt(fieldIndex);
//    assert(referenceValue == testValue);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_LOGGERMANAGERCONFIGURATION
#include <bael_loggermanagerconfiguration.h>
#endif

#ifndef INCLUDED_BDEM_LIST
#include <bdem_list.h>
#endif

#ifndef INCLUDED_BDEM_SCHEMA
#include <bdem_schema.h>
#endif

namespace BloombergLP {

                      // =======================================
                      // struct baelu_LoggerManagerConfiguration
                      // =======================================

class bdema_Allocator;

struct baelu_LoggerManagerConfiguration {
    // This 'struct' provides a namespace for pure procedures that facilitate
    // the use of 'bael_LoggerManagerConfiguration'.
    //
    // The functions provided in this 'struct' are neither thread-safe nor
    // thread-enabled.

    // CLASS METHODS
    static
    bool isValidSchemaOneInt(const bdem_Schema&  schema,
                             const char         *recordName,
                             const char         *fieldName);
        // Return 'true' if the specified 'schema' has a record with the
        // specified 'recordName', that record has a field with the specified
        // 'fieldName', and that field is of type 'bdem_ElemType::INT'; return
        // 'false' otherwise.  The behavior is undefined unless 'recordName'
        // and 'fieldName' are null-terminated (C-style) strings.

    static
    bael_LoggerManagerConfiguration makeLmcOneInt(
                                       const bdem_Schema&  schema,
                                       const char         *recordName,
                                       const char         *fieldName,
                                       const int          *value,
                                       bdema_Allocator    *basicAllocator = 0);
        // Return a logger manager configuration object having a populator that
        // can populate the 'bdem_ElemType::INT' element corresponding to the
        // specified 'fieldName' in a 'bdem_List' with the specified '*value';
        // the list must satisfy the record having the specified 'recordName'
        // in the specified 'schema', and that record must have a field named
        // 'fieldName'.  The optionally specified 'basicAllocator' is used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // 'recordName' and 'fieldName' are null-terminated (C-style) strings,
        // isValidSchemaOneInt(schema, recordName, fieldName) == true, and
        // 'value' is non-null.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
