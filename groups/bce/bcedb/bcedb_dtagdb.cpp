// bcedb_dtagdb.cpp   -*-C++-*-
#include <bcedb_dtagdb.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcedb_dtagdb_cpp,"$Id$ $CSID$")

#include <bcedb_dtagfactory.h>
#include <bcem_aggregate.h>
#include <bdem_binding.h>
#include <bdema_managedptr.h>

#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_cfloat.h>

#include <bsl_c_limits.h>
#include <bsl_c_stdlib.h>

// #if 1 > 2 would work here if necessary to trick bde_build
#include <bcedb_incoredtagfactory.h>    // for testing only

// This is giving me a headache on linux.  Since we know this is 64 bit --
// i.e., we are using 'bdes_types' 64 bit types, this will work.
#define BCEDB_DTAGDB_LLONG_MAX   9223372036854775807LL
#define BCEDB_DTAGDB_LLONG_MIN   (-BCEDB_DTAGDB_LLONG_MAX - 1LL)

namespace BloombergLP {

namespace {

typedef bsl::vector<bsl::pair<bsl::string, int> > StrIntVec;

inline
StrIntVec::iterator findField(StrIntVec *data, const char *field)
{
    for (StrIntVec::iterator i = data->begin(); i != data->end(); ++i) {
        if (i->first == field) {
            return i;
        }
    }
    return data->end();
}

void setFieldsMin(const bcem_Aggregate&           agg,
                  const bsl::vector<bsl::string>& fields)
{
    for (unsigned int i = 0; i < fields.size(); ++i) {
        int fieldIndex = agg.recordDef().fieldIndex(fields[i].c_str());
        if (fieldIndex >= 0) {
          switch (agg.recordDef().field(fieldIndex).elemType()) {
              case bdem_ElemType::BDEM_INT: {
                agg.setField(fields[i], INT_MIN);
              } break;
              case bdem_ElemType::BDEM_SHORT: {
                agg.setField(fields[i], SHRT_MIN);
              } break;
              case bdem_ElemType::BDEM_INT64: {
                agg.setField(fields[i], BCEDB_DTAGDB_LLONG_MIN);
              } break;
              case bdem_ElemType::BDEM_FLOAT: {
                agg.setField(fields[i], FLT_MIN);
              } break;
              case bdem_ElemType::BDEM_DOUBLE: {
                agg.setField(fields[i], DBL_MIN);
              } break;
              case bdem_ElemType::BDEM_STRING: {
                // TBD: fix this to be better.  We know comdb2 max
                // is 128, but we should get this from somewhere.
                bsl::string tmp;
                tmp.insert(tmp.end(), 128, '\0');
                agg.setField(fields[i], tmp);
              } break;
              case bdem_ElemType::BDEM_CHAR_ARRAY: {
                // Since blobs can't be part of a key we know this is
                // a byte array.
                bsl::string tmp;
                tmp.insert(tmp.end(), 128, '\0');
                agg.setField(fields[i], tmp);
              } break;
              default:
              // error
              BSLS_ASSERT(!"invalid type");
          }
        }
    }
}

void setFieldsMax(const bcem_Aggregate&           agg,
                  const bsl::vector<bsl::string>& fields)
{
    for (unsigned int i = 0; i < fields.size(); ++i) {
        int fieldIndex = agg.recordDef().fieldIndex(fields[i].c_str());
        if (fieldIndex >= 0) {
            switch (agg.recordDef().field(fieldIndex).elemType()) {
              case bdem_ElemType::BDEM_INT: {
                agg.setField(fields[i], INT_MAX);
              } break;
              case bdem_ElemType::BDEM_SHORT: {
                agg.setField(fields[i], SHRT_MAX);
              } break;
              case bdem_ElemType::BDEM_INT64: {
                agg.setField(fields[i], BCEDB_DTAGDB_LLONG_MAX);
              } break;
              case bdem_ElemType::BDEM_FLOAT: {
                agg.setField(fields[i], FLT_MAX);
              } break;
              case bdem_ElemType::BDEM_DOUBLE: {
                agg.setField(fields[i], DBL_MAX);
              } break;
              case bdem_ElemType::BDEM_STRING: {
                // TBD: fix this to be better.  We know comdb2 max
                // is 128, but we should get this from somewhere.
                bsl::string tmp;
                tmp.insert(tmp.end(), 128, CHAR_MAX);
                agg.setField(fields[i], tmp);
              } break;
              case bdem_ElemType::BDEM_CHAR_ARRAY: {
                // Since blobs can't be part of a key we know this is
                // a byte array.
                bsl::string tmp;
                tmp.insert(tmp.end(), 128, UCHAR_MAX);
                agg.setField(fields[i], tmp);
              } break;
              default:
                // error
                BSLS_ASSERT(!"invalid type");
            }
        }
    }
}

void buildPartialString(bsl::string                     *partialKey,
                        bsl::vector<bsl::string>        *unsetMandatoryKeys,
                        StrIntVec                       *setFields,
                        const bsl::vector<bsl::string>&  partialString,
                        const bdem_RecordDef&            indexDef)
{
    // This is order dependent.  We need to make sure that the partial
    // key is generated in the correct order.  Go through each of the
    // index fields and add the set fields to the partial key, setting
    // the length as appropriate.  Note: because these vectors should
    // be small, we are just going to do a linear search.
    bsl::ostringstream pStream;
    int numFields = indexDef.numFields();
    int numSetFields = setFields->size();
    for (int i = 0; i < numFields; ++i) {
        const char *fName = indexDef.fieldName(i);
        StrIntVec::iterator it = findField(setFields, fName);
        pStream << fName;
        if (it != setFields->end()) {
            if (bsl::find(partialString.begin(), partialString.end(), fName)
                   != partialString.end()) {
                if (it->second) {  // adding :0 after a partial key will result
                                   // in return code 110 - bad request.
                    pStream << ':' << it->second;
                }
            }
            setFields->erase(it);
        } else {
            unsetMandatoryKeys->push_back(fName);
        }
        if (0 == setFields->size()) {
            break;
        }
        if (i < numFields - 1) {
            pStream << '+';
        }
    }
    *partialKey = pStream.str();
    if (setFields->size() == (unsigned)numSetFields) {
        // Nothing in the key is set.  Just do an empty length search.
        partialKey->clear();
        unsetMandatoryKeys->clear();
    }
}

bool allFieldsAreInDef(const bdem_RecordDef&           def,
                       const bsl::vector<bsl::string>& fields)
{
    for (unsigned int i = 0; i < fields.size(); ++i) {
        if (0 == def.lookupField(fields[i].c_str())) {
            return false;
        }
    }
    return true;
}

void findBestIndex(bsl::string           *indexName,
                   const bcem_Aggregate&  record,
                   const bdem_Schema&     indexSchema)
{
    // TBD: this algorithm is pretty simple.  We can make this more
    // sophisticated to find a better match based on field order.
    // Also, we may want to cache the set fields.  We use this in
    // other places as well.
    bsl::vector<bsl::string> fields;
    const bdem_RecordDef& rd = record.recordDef();
    for (int i = 0; i < rd.numFields(); ++i) {
        const char *fName = rd.fieldName(i);
        bdem_ElemRef data = record.fieldRef(fName);
        if (data.isNonNull()) {
            fields.push_back(fName);
        }
    }

    int bestMatch       = -1;
    int numFinBestMatch = 0;
    int len             = indexSchema.numRecords();
    for (int i = 0; i < len; ++i) {
        if (allFieldsAreInDef(indexSchema.record(i), fields)) {
            if (bestMatch < 0 ||
                indexSchema.record(i).numFields() < numFinBestMatch) {
                bestMatch       = i;
                numFinBestMatch = indexSchema.record(i).numFields();
            }
        }
    }

    if (bestMatch >= 0) {
        *indexName = indexSchema.recordName(bestMatch);
    }
}

}  // close unnamed namespace

                        // ------------------
                        // class bcedb_DtagDb
                        // ------------------

// PRIVATE MANIPULATORS
void bcedb_DtagDb::controlDeleter(bcedb_DtagControlType *c,
                                  bcedb_DtagBindingType *f)
{
    f->destroyControl(c);
}

void bcedb_DtagDb::bindingDeleter(bcedb_DtagBindingType *t,
                                  bcedb_DtagDbType      *f)
{
    f->destroyBinding(t);
}

void bcedb_DtagDb::constCursorDeleter(bcedb_ConstCursor *c, bslma_Allocator *a)
{
    bslma_Allocator *allocator = bslma_Default::allocator(a);
    if (c) {
        c->~bcedb_ConstCursor();
        allocator->deallocate(c);
    }
}

void bcedb_DtagDb::cursorDeleter(bcedb_Cursor *c, bslma_Allocator *a)
{
    bslma_Allocator *allocator = bslma_Default::allocator(a);
    if (c) {
        c->~bcedb_Cursor();
        allocator->deallocate(c);
    }
}

// CREATORS
bcedb_DtagDb::bcedb_DtagDb(bcedb_DtagFactory *factory,
                           bslma_Allocator   *basicAllocator)
: d_db(0)
, d_factory(factory)
, d_dbKeys(StrSchemaMap::key_compare(), basicAllocator)
, d_allocator(bslma_Default::allocator(basicAllocator))
{
}

bcedb_DtagDb::~bcedb_DtagDb()
{
    if (d_db) {
        d_factory->destroyDb(d_db);
    }
}

// PRIVATE ACCESSORS
int bcedb_DtagDb::generatePartialCode(
                         bsl::string                     *partialCode,
                         bsl::vector<bsl::string>        *mandatoryUnsetFields,
                         const char                      *table,
                         const bcem_Aggregate&            record,
                         const bsl::vector<bsl::string>&  partialStrings,
                         const char                      *index) const
{
    enum { GOOD = 0, BAD };

    // Find the fields that are set for the query so we can build a proper
    // partial key if necessary.
    StrIntVec setFields;
    const bdem_RecordDef& rd = record.recordDef();
    for (int i = 0; i < rd.numFields(); ++i) {
        const char *fName = rd.fieldName(i);
        bdem_ElemRef data = record.fieldRef(fName);
        if (data.isNonNull()) {
            bsl::pair<bsl::string, int> nameAndLength;
            if (data.type() == bdem_ElemType::BDEM_STRING) {
                nameAndLength = bsl::make_pair<bsl::string, int>(
                                                      fName,
                                                      data.theString().size());
            } else {
                nameAndLength = bsl::make_pair<bsl::string, int>(fName, 0);
            }
            setFields.push_back(nameAndLength);
        }
    }

    if (d_dbKeys.find(table) == d_dbKeys.end()) {
        d_db->getDbKeys(&d_dbKeys[table], table);
    }
    const bdem_Schema& keys = d_dbKeys[table];
    const bdem_RecordDef *def = keys.lookupRecord(index);

    // It should really be an error if def is 0, but we want to work
    // just a little when the comdb2 api is incomplete.
    if (!def) return BAD;

    buildPartialString(partialCode, mandatoryUnsetFields,
                       &setFields, partialStrings, *def);

    return GOOD;
}

// MANIPULATORS
int bcedb_DtagDb::open(const char *dbName)
{
    enum { GOOD = 0, BAD };

    // Note: the right thing to do here is to use some kind of shared
    // pointer to the db.
    if (d_db) {
        d_dbKeys.clear();
        d_factory->destroyDb(d_db);
    }

    d_db = d_factory->createDb(dbName);

    if (d_db) {
        return GOOD;
    }

    return BAD;
}

int bcedb_DtagDb::addRecord(const bcem_Aggregate& record, const char *table)
{
    enum { GOOD = 0, BAD };   // TBD: use better errors

    bcedb_DtagDbCursor c(record, table, d_db, d_allocator);

    if (!c) {
        return BAD;
    }

    return c.d_control->addRecordToDb();
}

bdema_ManagedPtr<bcedb_Cursor> bcedb_DtagDb::getFirstRecord(
                              bcema_SharedPtr<const bdem_RecordDef>  recordDef,
                              const char                            *table)
{
    bcedb_DtagDbCursor *c = new (*d_allocator) bcedb_DtagDbCursor(recordDef,
                                                                  table,
                                                                  d_db,
                                                                  d_allocator);
    if (!c || !*c) {
        return bdema_ManagedPtr<bcedb_Cursor>(c, d_allocator,
                                         &bcedb_DtagDb::cursorDeleter);
    }

    if (d_dbKeys.find(table) == d_dbKeys.end()) {
        d_db->getDbKeys(&d_dbKeys[table], table);
    }
    const bdem_Schema& keys = d_dbKeys[table];

    if (0 == keys.numRecords()) {
        c->invalidate();
        return bdema_ManagedPtr<bcedb_Cursor>(c, d_allocator,
                                         &bcedb_DtagDb::cursorDeleter);
    }

    // Find a key that has the fields in the provided record def
    int indexToUse = -1;
    for (int i = 0; i < keys.numRecords(); ++i) {
        const bdem_RecordDef& rec = keys.record(i);
        int numFields = rec.numFields();
        int j;
        for (j = 0; j < numFields; ++j) {
            if (0 > recordDef->fieldIndex(rec.fieldName(j))) {
               break;
            }
        }
        if (j == numFields) {
            indexToUse = i;
            break;
        }
    }

    if (indexToUse < 0) {
        c->invalidate();
        return bdema_ManagedPtr<bcedb_Cursor>(c, d_allocator,
                                              &bcedb_DtagDb::cursorDeleter);
    }

    bcedb_DtagControlType::FIND_STATUS_CODE rc =
                          c->d_control->findRecordInDb(
                                      keys.recordName(indexToUse), "");

    if (rc != bcedb_DtagControlType::FIND_STATUS_NONE) {
        c->d_isFirst = true;
        if (rc == bcedb_DtagControlType::FIND_STATUS_ONE) {
            c->d_isLast = true;
        }
    } else {
        c->invalidate();
    }

    return bdema_ManagedPtr<bcedb_Cursor>(c, d_allocator,
                                      &bcedb_DtagDb::cursorDeleter);

}

bdema_ManagedPtr<bcedb_Cursor> bcedb_DtagDb::query(
                                                 const bcem_Aggregate&  record,
                                                 const char            *table)
{
    return query(record, table, bsl::vector<bsl::string>());
}

bdema_ManagedPtr<bcedb_Cursor> bcedb_DtagDb::query(
                                const bcem_Aggregate&            record,
                                const char                      *table,
                                const bsl::vector<bsl::string>&  partialFields)
{
    if (d_dbKeys.find(table) == d_dbKeys.end()) {
        d_db->getDbKeys(&d_dbKeys[table], table);
    }
    const bdem_Schema& keys = d_dbKeys[table];
    bsl::string index;
    findBestIndex(&index, record, keys);
    if (!index.size()) {
        // TBD: error
    }

    return queryWithIndex(record, table, index.c_str(), partialFields);
}

bdema_ManagedPtr<bcedb_Cursor> bcedb_DtagDb::query(
                                              const bcem_Aggregate&  record,
                                              const bcem_Aggregate&  endRecord,
                                              const char            *table)
{
    return query(record, endRecord, table, bsl::vector<bsl::string>());
}

bdema_ManagedPtr<bcedb_Cursor> bcedb_DtagDb::query(
                                const bcem_Aggregate&            record,
                                const bcem_Aggregate&            endRecord,
                                const char                      *table,
                                const bsl::vector<bsl::string>&  partialFields)
{
    if (d_dbKeys.find(table) == d_dbKeys.end()) {
        d_db->getDbKeys(&d_dbKeys[table], table);
    }
    const bdem_Schema& keys = d_dbKeys[table];
    bsl::string index;
    findBestIndex(&index, record, keys);
    if (!index.size()) {
        // TBD: error
    }
    return queryWithIndex(record, endRecord, table, index.c_str(),
                          partialFields);
}

bdema_ManagedPtr<bcedb_Cursor> bcedb_DtagDb::queryWithIndex(
                                              const bcem_Aggregate&  record,
                                              const char            *table,
                                              const char            *indexName)
{
    return queryWithIndex(record,
                          table,
                          indexName,
                          bsl::vector<bsl::string>());
}

bdema_ManagedPtr<bcedb_Cursor> bcedb_DtagDb::queryWithIndex(
                                const bcem_Aggregate&            record,
                                const char                      *table,
                                const char                      *indexName,
                                const bsl::vector<bsl::string>&  partialFields)
{
    enum { GOOD = 0, BAD };

    const char *partialCode = 0;
    bsl::string partialStr;
    bsl::vector<bsl::string> mandatoryFields;
    if (0 == generatePartialCode(&partialStr, &mandatoryFields,
                                 table, record, partialFields, indexName)) {
        partialCode = partialStr.c_str();
    }

    if (mandatoryFields.size()) {
        bcem_Aggregate endRecord(record.cloneData(d_allocator));
        setFieldsMin(record, mandatoryFields);
        setFieldsMax(endRecord, mandatoryFields);
        // This is not very efficient, but it's factored nicely.
        return queryWithIndex(record, endRecord, table, indexName,
                              partialFields);
    }

    bcedb_DtagDbCursor *c = new (*d_allocator) bcedb_DtagDbCursor(record,
                                                                  table,
                                                                  d_db,
                                                                  d_allocator);
    if (!c || !*c) {
        return bdema_ManagedPtr<bcedb_Cursor>();
    }

    bcedb_DtagControlType::FIND_STATUS_CODE rc =
                                       c->d_control->findRecordInDb(
                                                            indexName,
                                                            partialCode);

    if (rc != bcedb_DtagControlType::FIND_STATUS_NONE) {
        c->d_isFirst = true;
        if (rc == bcedb_DtagControlType::FIND_STATUS_ONE) {
            c->d_isLast = true;
        }
    } else {
        c->invalidate();
    }

    return bdema_ManagedPtr<bcedb_Cursor>(c, d_allocator,
                                      &bcedb_DtagDb::cursorDeleter);
}

bdema_ManagedPtr<bcedb_Cursor> bcedb_DtagDb::queryWithIndex(
                                              const bcem_Aggregate&  record,
                                              const bcem_Aggregate&  endRecord,
                                              const char            *table,
                                              const char            *indexName)
{
    return queryWithIndex(record, endRecord,
                          table, indexName, bsl::vector<bsl::string>());
}

bdema_ManagedPtr<bcedb_Cursor> bcedb_DtagDb::queryWithIndex(
                                const bcem_Aggregate&            record,
                                const bcem_Aggregate&            endRecord,
                                const char                      *table,
                                const char                      *indexName,
                                const bsl::vector<bsl::string>&  partialFields)
{

    enum { GOOD = 0, BAD };

    bcem_Aggregate endAgg(endRecord.cloneData(d_allocator));
    bcedb_DtagBindingType *endTag = d_db->createBinding(table);
    endTag->bind(&endAgg);

    const char *partialCode = 0;
    bsl::string partialStr;
    bsl::vector<bsl::string> mandatoryFields;
    if (0 == generatePartialCode(&partialStr, &mandatoryFields,
                                 table, record, partialFields, indexName)) {
        partialCode = partialStr.c_str();
    }

    setFieldsMin(record, mandatoryFields);
    setFieldsMax(endRecord, mandatoryFields);

    bcedb_DtagDbCursor *c = new (*d_allocator) bcedb_DtagDbCursor(record,
                                                                  table,
                                                                  d_db,
                                                                  d_allocator);
    if (!c || !*c) {
        return bdema_ManagedPtr<bcedb_Cursor>();
    }

    bcedb_DtagControlType::FIND_STATUS_CODE rc =
                                      c->d_control->findRangeInDb(endTag,
                                                                  indexName,
                                                                  partialCode);

    if (rc != bcedb_DtagControlType::FIND_STATUS_NONE) {
        c->d_isFirst = true;
        if (rc == bcedb_DtagControlType::FIND_STATUS_ONE) {
            c->d_isLast = true;
        }
    } else {
        c->invalidate();
    }

    d_db->destroyBinding(endTag);

    return bdema_ManagedPtr<bcedb_Cursor>(c, d_allocator,
                                      &bcedb_DtagDb::cursorDeleter);
}

//ACCESSORS
bool bcedb_DtagDb::isOpen() const
{
    return !!d_db;
}

int bcedb_DtagDb::getDbSchema(bdem_Schema              *schema,
                              bsl::vector<bsl::string> *tableNames) const
{
    schema->removeAll();
    d_db->getDbTables(tableNames);
    for (unsigned int i = 0; i < tableNames->size(); ++i) {
        d_db->appendOnDiskTagInfo(schema, (*tableNames)[i].c_str());
        if ((unsigned int)schema->numRecords() <= i) {
            // If appendOnDiskTagInfo comes across an invalid field within a
            // record it will remove all of the records.  Therefore, if the
            // number of records in the schema is less then or equal to i, then
            // an invalid field was found.
            return 1;
        }
    }
    return 0;
}

int bcedb_DtagDb::getTableSchema(bdem_Schema *schema,
                                 const char  *tableName) const
{
    schema->removeAll();
    d_db->appendOnDiskTagInfo(schema, tableName);
    return 0;
}

int bcedb_DtagDb::getTableIndices(bdem_Schema *indexDescriptions,
                                  const char  *tableName) const
{
    StrSchemaMap::const_iterator it = d_dbKeys.find(tableName);
    if (it != d_dbKeys.end()) {
        *indexDescriptions = it->second;
        return 0;                                                     // RETURN
    }
   
    d_db->getDbKeys(indexDescriptions, tableName);
    d_dbKeys.insert(StrSchemaMap::value_type(tableName, *indexDescriptions));
    return 0;
}

bdema_ManagedPtr<bcedb_ConstCursor> bcedb_DtagDb::getFirstRecord(
                            bcema_SharedPtr<const bdem_RecordDef>  recordDef,
                            const char                            *table) const
{
    enum { GOOD = 0, BAD };

    bcedb_DtagDbConstCursor *c = new (*d_allocator) bcedb_DtagDbConstCursor(
                                                                  recordDef,
                                                                  table, d_db,
                                                                  d_allocator);
    if (!c || !*c) {
        return bdema_ManagedPtr<bcedb_ConstCursor>();
    }

    if (d_dbKeys.find(table) == d_dbKeys.end()) {
        d_db->getDbKeys(&d_dbKeys[table], table);
    }
    const bdem_Schema& keys = d_dbKeys[table];

    if (0 == keys.numRecords()) {
        c->invalidate();
        return bdema_ManagedPtr<bcedb_ConstCursor>(c, d_allocator,
                                         &bcedb_DtagDb::constCursorDeleter);
    }

    // Find a key that has the fields in the provided record def
    int indexToUse = -1;
    for (int i = 0; i < keys.numRecords(); ++i) {
        const bdem_RecordDef& rec = keys.record(i);
        int numFields = rec.numFields();
        int j;
        for (j = 0; j < numFields; ++j) {
            if (0 > recordDef->fieldIndex(rec.fieldName(j))) {
               break;
            }
        }
        if (j == numFields) {
            indexToUse = i;
            break;
        }
    }

    if (indexToUse < 0) {
        c->invalidate();
        return bdema_ManagedPtr<bcedb_ConstCursor>(c, d_allocator,
                                         &bcedb_DtagDb::constCursorDeleter);
    }

    bcedb_DtagControlType::FIND_STATUS_CODE rc =
                         c->d_control->findRecordInDb(
                                      keys.recordName(indexToUse), "");

    if (rc != bcedb_DtagControlType::FIND_STATUS_NONE) {
        c->d_isFirst = true;
        if (rc == bcedb_DtagControlType::FIND_STATUS_ONE) {
            c->d_isLast = true;
        }
    } else {
        c->invalidate();
    }

    return bdema_ManagedPtr<bcedb_ConstCursor>(c, d_allocator,
                                      &bcedb_DtagDb::constCursorDeleter);

}

bdema_ManagedPtr<bcedb_ConstCursor> bcedb_DtagDb::query(
                                            const bcem_Aggregate&  record,
                                            const char            *table) const
{
    return query(record, table, bsl::vector<bsl::string>());
}

bdema_ManagedPtr<bcedb_ConstCursor> bcedb_DtagDb::query(
                          const bcem_Aggregate&            record,
                          const char                      *table,
                          const bsl::vector<bsl::string>&  partialFields) const
{
    if (d_dbKeys.find(table) == d_dbKeys.end()) {
        d_db->getDbKeys(&d_dbKeys[table], table);
    }
    const bdem_Schema& keys = d_dbKeys[table];
    bsl::string index;
    findBestIndex(&index, record, keys);
    if (!index.size()) {
        // TBD: error
    }
    return queryWithIndex(record, table, index.c_str(), partialFields);
}

bdema_ManagedPtr<bcedb_ConstCursor> bcedb_DtagDb::query(
                                            const bcem_Aggregate&  record,
                                            const bcem_Aggregate&  endRecord,
                                            const char            *table) const
{
    return query(record, endRecord, table, bsl::vector<bsl::string>());
}

bdema_ManagedPtr<bcedb_ConstCursor> bcedb_DtagDb::query(
                          const bcem_Aggregate&            record,
                          const bcem_Aggregate&            endRecord,
                          const char                      *table,
                          const bsl::vector<bsl::string>&  partialFields) const
{
    if (d_dbKeys.find(table) == d_dbKeys.end()) {
        d_db->getDbKeys(&d_dbKeys[table], table);
    }
    const bdem_Schema& keys = d_dbKeys[table];
    bsl::string index;
    findBestIndex(&index, record, keys);
    if (!index.size()) {
        // TBD: error
    }
    return queryWithIndex(record, endRecord, table, index.c_str(),
                          partialFields);
}

bdema_ManagedPtr<bcedb_ConstCursor> bcedb_DtagDb::queryWithIndex(
                                        const bcem_Aggregate&  record,
                                        const char            *table,
                                        const char            *indexName) const
{
    return queryWithIndex(record,
                          table,
                          indexName,
                          bsl::vector<bsl::string>());
}

bdema_ManagedPtr<bcedb_ConstCursor> bcedb_DtagDb::queryWithIndex(
                          const bcem_Aggregate&            record,
                          const char                      *table,
                          const char                      *indexName,
                          const bsl::vector<bsl::string>&  partialFields) const
{
    enum { GOOD = 0, BAD };

    const char *partialCode = 0;
    bsl::string partialStr;
    bsl::vector<bsl::string> mandatoryFields;
    if (0 == generatePartialCode(&partialStr, &mandatoryFields,
                                 table, record, partialFields, indexName)) {
        partialCode = partialStr.c_str();
    }

    if (mandatoryFields.size()) {
        bcem_Aggregate endRecord(record.cloneData(d_allocator));
        setFieldsMin(record, mandatoryFields);
        setFieldsMax(endRecord, mandatoryFields);
        // This is not very efficient, but it's factored nicely.
        return queryWithIndex(record, endRecord, table, indexName,
                              partialFields);
    }

    bcedb_DtagDbConstCursor *c = new (*d_allocator) bcedb_DtagDbConstCursor(
                                                               record, table,
                                                               d_db,
                                                               d_allocator);
    if (!c || !*c) {
        return bdema_ManagedPtr<bcedb_ConstCursor>();
    }

    bcedb_DtagControlType::FIND_STATUS_CODE rc =
                         c->d_control->findRecordInDb(indexName,
                                                      partialCode);

    if (rc != bcedb_DtagControlType::FIND_STATUS_NONE) {
        c->d_isFirst = true;
        if (rc == bcedb_DtagControlType::FIND_STATUS_ONE) {
            c->d_isLast = true;
        }
    } else {
        c->invalidate();
    }

    return bdema_ManagedPtr<bcedb_ConstCursor>(c, d_allocator,
                                      &bcedb_DtagDb::constCursorDeleter);
}

bdema_ManagedPtr<bcedb_ConstCursor> bcedb_DtagDb::queryWithIndex(
                                        const bcem_Aggregate&  record,
                                        const bcem_Aggregate&  endRecord,
                                        const char            *table,
                                        const char            *indexName) const
{
    return queryWithIndex(record, endRecord,
                          table, indexName, bsl::vector<bsl::string>());
}

bdema_ManagedPtr<bcedb_ConstCursor> bcedb_DtagDb::queryWithIndex(
                          const bcem_Aggregate&            record,
                          const bcem_Aggregate&            endRecord,
                          const char                      *table,
                          const char                      *indexName,
                          const bsl::vector<bsl::string>&  partialFields) const
{
    enum { GOOD = 0, BAD };

    const char *partialCode = 0;
    bsl::string partialStr;
    bsl::vector<bsl::string> mandatoryFields;
    if (0 == generatePartialCode(&partialStr, &mandatoryFields,
                                 table, record, partialFields, indexName)) {
        partialCode = partialStr.c_str();
    }

    // The behavior is undefined if the index record def is not a subset
    // of the 'record' record def.

    setFieldsMin(record, mandatoryFields);
    setFieldsMax(endRecord, mandatoryFields);

    bcedb_DtagDbConstCursor *c = new (*d_allocator) bcedb_DtagDbConstCursor(
                                                     record,
                                                     table,
                                                     d_db,
                                                     d_allocator);
    if (!c || !*c) {
        return bdema_ManagedPtr<bcedb_ConstCursor>(c, d_allocator,
                                      &bcedb_DtagDb::constCursorDeleter);
    }

    bcem_Aggregate endAgg(endRecord.cloneData(d_allocator));
    bcedb_DtagBindingType *endTag = d_db->createBinding(table);
    endTag->bind(&endAgg);

    bcedb_DtagControlType::FIND_STATUS_CODE rc =
                                      c->d_control->findRangeInDb(endTag,
                                                                  indexName,
                                                                  partialCode);

    if (rc != bcedb_DtagControlType::FIND_STATUS_NONE) {
        c->d_isFirst = true;
        if (rc == bcedb_DtagControlType::FIND_STATUS_ONE) {
            c->d_isLast = true;
        }
    } else {
        c->invalidate();
    }

    d_db->destroyBinding(endTag);
    return bdema_ManagedPtr<bcedb_ConstCursor>(c, d_allocator,
                                      &bcedb_DtagDb::constCursorDeleter);
}

void bcedb_DtagDb::getErrorString(bsl::string *, int) const
{
}
                        // -----------------------------
                        // class bcedb_DtagDbConstCursor
                        // -----------------------------

        // Create a cursor whose data conforms to the specified 'definition'
        // for the specified 'db' using the provided 'factory'.

bcedb_DtagDbConstCursor::bcedb_DtagDbConstCursor(
                         bcema_SharedPtr<const bdem_RecordDef>  definition,
                         const char                            *table,
                         bcedb_DtagDbType                      *db,
                         bslma_Allocator                       *basicAllocator)
: d_db(db)
, d_aggregate(definition, basicAllocator)
, d_isFirst(false)
, d_isLast(false)
, d_isDeleted(false)
{
    bcedb_DtagBindingType *t = db->createBinding(table);
    t->bind(&d_aggregate);
    bcedb_DtagControlType *c = t->createControl();
    d_control.load(c, t, &bcedb_DtagDb::controlDeleter);
    d_tag.load(t, d_db, &bcedb_DtagDb::bindingDeleter);
}

bcedb_DtagDbConstCursor::bcedb_DtagDbConstCursor(
                                         const bcem_Aggregate&  data,
                                         const char            *table,
                                         bcedb_DtagDbType      *db,
                                         bslma_Allocator       *basicAllocator)
: d_db(db)
, d_aggregate(data.cloneData(basicAllocator))
, d_isFirst(false)
, d_isLast(false)
, d_isDeleted(false)
{
    bcedb_DtagBindingType *t = db->createBinding(table);
    t->bind(&d_aggregate);
    bcedb_DtagControlType *c = t->createControl();
    d_control.load(c, t, &bcedb_DtagDb::controlDeleter);
    d_tag.load(t, db, &bcedb_DtagDb::bindingDeleter);
}

#if 0
bcedb_DtagDbConstCursor::bcedb_DtagDbConstCursor(
                                         const bcem_Aggregate&  data,
                                         const bcem_Aggregate&  endData,
                                         const char            *table,
                                         bcedb_DtagDbType      *db,
                                         bslma_Allocator       *basicAllocator)
: d_db(db)
, d_list(new (*bslma_Default::allocator(basicAllocator))
                                        bdem_List(*(bdem_List*)data.data(),
                                        basicAllocator))
, d_recordDef(data.recordDefPtr())
, d_isFirst(false)
, d_isLast(false)
, d_isDeleted(false)
{
    bcedb_DtagBindingType *tag = db->createBinding(table);
    bcem_Aggregate agg(d_recordDef, d_list);
    tag->bind(&agg);
    bcedb_DtagControlType *c = tag->createControl();
    d_control.load(c, tag, &bcedb_DtagDb::controlDeleter);
    d_tag.load(tag, db, &bcedb_DtagDb::bindingDeleter);
}
#endif

bcedb_DtagDbConstCursor::~bcedb_DtagDbConstCursor()
{
}

void bcedb_DtagDbConstCursor::invalidate()
{
    d_control.load(0);
}

// MANIPULATORS
bcedb_ConstCursor& bcedb_DtagDbConstCursor::next()
{
    bcedb_DtagControlType::FIND_STATUS_CODE rc = d_control->findNextRecord();
    if (rc != bcedb_DtagControlType::FIND_STATUS_NONE) {
        d_isFirst = false;
        if (rc == bcedb_DtagControlType::FIND_STATUS_ONE) {
            d_isLast = true;
        }
    } else {
        invalidate();
    }
    return *this;
}

bcedb_ConstCursor& bcedb_DtagDbConstCursor::prev()
{
    bcedb_DtagControlType::FIND_STATUS_CODE rc = d_control->findPrevRecord();
    if (rc != bcedb_DtagControlType::FIND_STATUS_NONE) {
        d_isLast = false;
        if (rc == bcedb_DtagControlType::FIND_STATUS_ONE) {
            d_isFirst = true;
        }
    } else {
        invalidate();
    }
    return *this;
}

// ACCESSORS
bcem_Aggregate bcedb_DtagDbConstCursor::data() const
{
    return d_aggregate;
}

bool bcedb_DtagDbConstCursor::isFirst() const
{
    return d_isFirst;
}

bool bcedb_DtagDbConstCursor::isLast() const
{
    return d_isLast;
}

bool bcedb_DtagDbConstCursor::isDeleted() const
{
    return d_isDeleted;
}

bcedb_DtagDbConstCursor::operator bool() const
{
    return !!d_control.ptr();
}

                        // ------------------------
                        // class bcedb_DtagDbCursor
                        // ------------------------

bcedb_DtagDbCursor::bcedb_DtagDbCursor(
                         bcema_SharedPtr<const bdem_RecordDef>  definition,
                         const char                            *table,
                         bcedb_DtagDbType                      *db,
                         bslma_Allocator                       *basicAllocator)
: d_db(db)
, d_aggregate(definition, basicAllocator)
, d_isFirst(true)
, d_isLast(false)
, d_isDeleted(false)
{
    bcedb_DtagBindingType *t = db->createBinding(table);
    t->bind(&d_aggregate);
    bcedb_DtagControlType *c = t->createControl();
    d_control.load(c, t, &bcedb_DtagDb::controlDeleter);
    d_tag.load(t, db, &bcedb_DtagDb::bindingDeleter);
}

bcedb_DtagDbCursor::bcedb_DtagDbCursor(const bcem_Aggregate&  data,
                                       const char            *table,
                                       bcedb_DtagDbType      *db,
                                       bslma_Allocator       *basicAllocator)
: d_db(db)
, d_aggregate(data.cloneData(basicAllocator))
, d_isFirst(true)
, d_isLast(false)
, d_isDeleted(false)
{
    bcedb_DtagBindingType *t = db->createBinding(table);
    t->bind(&d_aggregate);
    bcedb_DtagControlType *c = t->createControl();
    d_control.load(c, t, &bcedb_DtagDb::controlDeleter);
    d_tag.load(t, db, &bcedb_DtagDb::bindingDeleter);
}

bcedb_DtagDbCursor::~bcedb_DtagDbCursor()
{
}

void bcedb_DtagDbCursor::invalidate()
{
    d_control.load(0);
}

// MANIPULATORS
bcedb_Cursor& bcedb_DtagDbCursor::next()
{
    bcedb_DtagControlType::FIND_STATUS_CODE rc = d_control->findNextRecord();
    if (rc != bcedb_DtagControlType::FIND_STATUS_NONE) {
        d_isFirst = false;
        if (rc == bcedb_DtagControlType::FIND_STATUS_ONE) {
            d_isLast = true;
        }
    } else {
        invalidate();
    }
    return *this;
}

bcedb_Cursor& bcedb_DtagDbCursor::prev()
{
    bcedb_DtagControlType::FIND_STATUS_CODE rc = d_control->findPrevRecord();
    if (rc != bcedb_DtagControlType::FIND_STATUS_NONE) {
        d_isLast = false;
        if (rc == bcedb_DtagControlType::FIND_STATUS_ONE) {
            d_isFirst = true;
        }
    } else {
        invalidate();
    }
    return *this;
}

int bcedb_DtagDbCursor::update(const bcem_Aggregate& newRecord)
{
    enum { GOOD = 0, BAD };

    d_tag->updateData(newRecord);
    return d_control->updateRecordInDb();
}

int bcedb_DtagDbCursor::remove()
{
    BSLS_ASSERT(d_control);
    int rc = d_control->deleteRecordFromDb();
    d_isDeleted = true;
    return rc;
}

// ACCESSORS
bcem_Aggregate bcedb_DtagDbCursor::data() const
{
    return d_aggregate;
}

bool bcedb_DtagDbCursor::isFirst() const
{
    return d_isFirst;
}

bool bcedb_DtagDbCursor::isLast() const
{
    return d_isLast;
}

bool bcedb_DtagDbCursor::isDeleted() const
{
    return d_isDeleted;
}

bcedb_DtagDbCursor::operator bool() const
{
    return !!d_control.ptr();
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
