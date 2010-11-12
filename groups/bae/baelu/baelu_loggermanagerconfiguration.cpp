// baelu_loggermanagerconfiguration.cpp       -*-C++-*-

#include <baelu_loggermanagerconfiguration.h>

#include <bael_loggermanagerconfiguration.h>
#include <bael_severity.h>

#include <bdem_binding.h>
#include <bdem_elemref.h>
#include <bdem_list.h>
#include <bdem_schema.h>
#include <bdem_schemaaggregateutil.h>

#include <bdef_vfunc2.h>
#include <bdefu_vfunc2.h>

#include <bdes_assert.h>


namespace BloombergLP {

// FREE FUNCTION

static void populatorFunctionOneInt(bdem_List          *list,
                                    const bdem_Schema&  schema,
                                    const char         *recordName,
                                    const char         *fieldName,
                                    const int          *value)
{
    // Populate the 'bdem_ElemType::INT' element corresponding to the
    // specified 'fieldName' in the specified 'list' with the specified
    // '*value', where 'fieldName' names a field in the record having the
    // specified 'recordName' in the specified 'schema'.  If 'list' is
    // empty on entry, then 'list' is first initialized to satisfy the
    // record in 'schema' named 'recordName'; if 'list' is not-empty on
    // entry and it does not satisfy the record in 'schema' named
    // 'recordName', then this function has no effect.  The behavior is
    // undefined unless 'list' and 'value' are non-null, 'recordName' and
    // 'fieldName' are null-terminated (C-style) strings, and
    // isValidSchemaOneInt(schema, recordName, fieldName) == true.
    //
    // This implementation allows clients to augment the incoming list,
    // consistent with the logger's schema.  The telltale sign of such
    // augmentation is that '*list' is not empty (see
    // 'bael_Logger::getRecord()'), in which case the list is *not*
    // reinitialized here.  If the list is not empty, consistency with the
    // logger's schema (record) is validated.  Note that the logic of the 'if'
    // blocks is chosen so that the "normal" case incurs minimal overhead.

    BDE_ASSERT_CPP(list);
    BDE_ASSERT_CPP(recordName);
    BDE_ASSERT_CPP(fieldName);
    BDE_ASSERT_CPP(value);

    const bdem_RecordDef *record = schema.lookupRecord(recordName);

    if (0 == list->length()) {
        bdem_SchemaAggregateUtil::initListShallow(list, *record);
        bdem_RowBinding rowBinding(list, &schema, recordName);
        rowBinding.theInt(fieldName) = *value;
    } else if (bdem_SchemaAggregateUtil::canSatisfyRecord(*list, *record)) {
        bdem_RowBinding rowBinding(list, &schema, recordName);
        rowBinding.theInt(fieldName) = *value;
    }
}


class bdema_Allocator;

// CLASS METHODS

bool baelu_LoggerManagerConfiguration::isValidSchemaOneInt(
                                                const bdem_Schema&  schema,
                                                const char         *recordName,
                                                const char         *fieldName)
{
    const bdem_RecordDef *record = schema.lookupRecord(recordName);
    if (!record) {
        return false;                                                 // RETURN
    }

    const bdem_FieldDef *field = record->lookupField(fieldName);
    if (!field) {
        return false;                                                 // RETURN
    }

    return bdem_ElemType::INT == field->elemType();
}

bael_LoggerManagerConfiguration
baelu_LoggerManagerConfiguration::makeLmcOneInt(
                                            const bdem_Schema&  schema,
                                            const char         *recordName,
                                            const char         *fieldName,
                                            const int          *value,
                                            bdema_Allocator    *basicAllocator)
{
    BDE_ASSERT_CPP(recordName);
    BDE_ASSERT_CPP(fieldName);
    BDE_ASSERT_CPP(value);
    BDE_ASSERT_CPP(baelu_LoggerManagerConfiguration::isValidSchemaOneInt(
                                                                   schema,
                                                                   recordName,
                                                                   fieldName));

    bdef_Vfunc2<bdem_List *, bdem_Schema> populatorFunctorOneInt;
    bdefu_Vfunc2::makeF(basicAllocator,
                        &populatorFunctorOneInt,
                        populatorFunctionOneInt,
                        recordName,
                        fieldName,
                        value);

    bael_LoggerManagerConfiguration configuration;        // default config.
    configuration.setUserFields(schema,                   // add populator
                                populatorFunctorOneInt);
    return configuration;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
