// bcedb_incoredtagfactory.cpp   -*-C++-*-
#include <bcedb_incoredtagfactory.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcedb_incoredtagfactory_cpp,"$Id$ $CSID$")

#include <bcema_sharedptr.h>
#include <bcem_aggregate.h>

#include <bdem_fielddef.h>
#include <bdem_fielddefattributes.h>
#include <bdem_recorddef.h>
#include <bdem_schema.h>
#include <bdem_schemautil.h>
#include <bdeut_strtokeniter.h>
#include <bsls_assert.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {

                        // =========================
                        // PRIVATE UTILITY FUNCTIONS
                        // =========================

namespace {

int copyOut(bcem_Aggregate *dest, const bcem_Aggregate& src)
    // Copy data from 'src' to 'dest', where 'dest''s schema is a subset of
    // 'src''s.
{
    const bdem_RecordDef& dstRec = dest->recordDef();
    const bdem_RecordDef& srcRec = src.recordDef();

    BSLS_ASSERT(src.dataType() == bdem_ElemType::BDEM_LIST
            || src.dataType() == bdem_ElemType::BDEM_ROW);
    BSLS_ASSERT(dest->dataType() == bdem_ElemType::BDEM_LIST
            || dest->dataType() == bdem_ElemType::BDEM_ROW);

    for (int i = 0; i < dstRec.numFields(); ++i) {
        const char *name = dstRec.fieldName(i);
        int srcField = srcRec.fieldIndex(name);
        BSLS_ASSERT(srcField >= 0);
        BSLS_ASSERT(srcRec.field(srcField).elemType()
                == dstRec.field(i).elemType());

        dest->setField(name, src.field(name));
    }

    return 0;
}

int copyIn(bcem_Aggregate *dest, const bcem_Aggregate& src)
    // Copy data from 'src' to 'dest', where 'src''s schema is a subset of
    // 'dest''s.  The fields in 'dest' that are not part of that subset will
    // remain untouched.
{
    const bdem_RecordDef& dstRec = dest->recordDef();
    const bdem_RecordDef& srcRec = src.recordDef();

    BSLS_ASSERT(src.dataType() == bdem_ElemType::BDEM_LIST
            || src.dataType() == bdem_ElemType::BDEM_ROW);
    BSLS_ASSERT(dest->dataType() == bdem_ElemType::BDEM_LIST
            || dest->dataType() == bdem_ElemType::BDEM_ROW);

    for (int i = 0; i < srcRec.numFields(); ++i) {
        const char *name = srcRec.fieldName(i);
        int dstField = dstRec.fieldIndex(name);
        BSLS_ASSERT(dstField >= 0);
        BSLS_ASSERT(dstRec.field(dstField).elemType()
                == srcRec.field(i).elemType());

        bcem_Aggregate a = src.field(name);
        dest->setField(name, a);
    }

    return 0;
}

void createPartialSearch(
        bcem_Aggregate                                 *result,
        bsl::vector<int>                               *partials,
        const bcema_SharedPtr<const bdem_RecordDef>&    key,
        const char *                                    partialCode,
        bslma_Allocator                                *allocator)
    // Populates the private members 'd_search' and 'd_partials' for
    // 'InCoreDtagControlType'.
{
    partials->clear();

    if (partialCode) {
        bdeut_StrTokenIter it(partialCode, "", "+:");
        bcema_SharedPtr<bdem_Schema> schema;
        schema.createInplace(allocator);
        bdem_RecordDef *rdResult = schema->createRecord();

        int expected = 0;

        for (; it; ++it) {
            using bsl::strcmp;

            BSLS_ASSERT(expected < key->numFields());
            BSLS_ASSERT(!strcmp(it(), key->fieldName(expected)));

            const bdem_FieldDef& fldDef = key->field(expected++);
            bdem_FieldDefAttributes fieldAttributes(fldDef.elemType(),
                                                    fldDef.isNullable(),
                                                    fldDef.formattingMode());
            if (fldDef.hasDefaultValue()) {
                fieldAttributes.defaultValue().replaceValue(
                                                        fldDef.defaultValue());
            }
            if (fldDef.recordConstraint()) {
                rdResult->appendField(fieldAttributes,
                                      fldDef.recordConstraint(),
                                      it());
            }
            else if (fldDef.enumerationConstraint()) {
                rdResult->appendField(fieldAttributes,
                                      fldDef.enumerationConstraint(),
                                      it());
            }
            else {
                rdResult->appendField(fieldAttributes, it());
            }

            int partial = -1;

            if (!strcmp(it.delimiter(), ":")) {
                ++it;
                partial = bsl::atoi(it());
            }

            partials->push_back(partial);
        }

        *result = bcem_Aggregate(
                      bcema_SharedPtr<const bdem_RecordDef>(schema, rdResult));
    }

    else {
        *result = bcem_Aggregate(key);
        partials->resize(key->numFields(), -1);
    }
}

inline
const bdem_RecordDef *getTblDef(const bdem_FieldDef& tblEntry)
    // Locate the record defining a table, given its entry as a field from
    // the DB schema.  Basically, just extract the first field of the
    // intermediate record and do some sanity assertions.
{
    const bdem_RecordDef *tblInfo = tblEntry.recordConstraint();
    BSLS_ASSERT(tblInfo);
    BSLS_ASSERT(tblInfo->numFields() > 0);
    BSLS_ASSERT(tblInfo->field(0).elemType() == bdem_ElemType::BDEM_TABLE);

    return tblInfo->field(0).recordConstraint();
}

int comparePartial(const bcem_Aggregate&   super,
                   const bcem_Aggregate&   sub,
                   const bsl::vector<int>& partials)
    // Compare data in 'sub' to the data in 'super' that overlaps with 'sub',
    // respecting any non-negative partial lengths for string fields.  Returns
    // 0 for equality, -1 for not match, -2 for incorrectly specified partial
    // string (':0', see DRQS 16457966).
{
    BSLS_ASSERT(bdem_SchemaUtil::isSymbolicSuperset(super.recordDef(),
                sub.recordDef()));
    BSLS_ASSERT(partials.size() == (unsigned)sub.recordDef().numFields());

    for (int i = 0; i < sub.recordDef().numFields(); ++i) {
        const char *n = sub.recordDef().fieldName(i);

        if (partials[i] == 0) {
            return -2;                                                // RETURN
        }
        else if (partials[i] > 0) {
            BSLS_ASSERT(super.field(n).dataType() ==
                                                   bdem_ElemType::BDEM_STRING);
            BSLS_ASSERT(sub.field(n).dataType() == bdem_ElemType::BDEM_STRING);

            if (bsl::strncmp(super.field(n).asString().c_str(),
                        sub.field(n).asString().c_str(), partials[i])) {
                return -1;                                            // RETURN
            }
        }
        else if (! bcem_Aggregate::areEquivalent(super.field(n),
                                                 sub.field(n))) {
            return -1;                                                // RETURN
        }
    }

    return 0;
}

}  // close unnamed namespace

                        // -----------------------------
                        // class bcedb_InCoreDtagFactory
                        // -----------------------------

// CREATORS
bcedb_InCoreDtagFactory::bcedb_InCoreDtagFactory(
                    const bcema_SharedPtr<const bdem_RecordDef>&  schemaShp,
                    bslma_Allocator                              *allocator)
: d_data(schemaShp, allocator)
, d_dbs(allocator)
, d_allocator_p(bslma_Default::allocator(allocator))
{
}

bcedb_InCoreDtagFactory::~bcedb_InCoreDtagFactory()
{
    bsl::vector<bcedb_InCoreDtagDbType *>::iterator it = d_dbs.begin();
    for (; it != d_dbs.end(); ++it) {
        d_allocator_p->deleteObjectRaw(*it);
    }

    d_dbs.clear();
}

// MANIPULATORS
void bcedb_InCoreDtagFactory::destroyDb(bcedb_DtagDbType *db)
{
    bsl::vector<bcedb_InCoreDtagDbType *>::iterator it = d_dbs.begin();
    for (; it != d_dbs.end(); ++it) {
        if (*it == db) {
            d_dbs.erase(it);
            break;
        }
    }

    d_allocator_p->deleteObjectRaw(db);
}

bcedb_DtagDbType *bcedb_InCoreDtagFactory::createDb(const char *dbName)
{
    if (!d_data.hasField(dbName)) {
        return 0;                                                     // RETURN
    }

    BSLS_ASSERT(d_data.field(dbName).dataType() == bdem_ElemType::BDEM_LIST);

    bcedb_InCoreDtagDbType *newDb =
        new (*d_allocator_p) bcedb_InCoreDtagDbType(d_data.field(dbName),
                                                    d_allocator_p);

    d_dbs.push_back(newDb);

    return newDb;
}

                        // ----------------------------
                        // class bcedb_InCoreDtagDbType
                        // ----------------------------

// CREATORS
bcedb_InCoreDtagDbType::bcedb_InCoreDtagDbType(const bcem_Aggregate& tableData,
                                               bslma_Allocator      *allocator)
: d_data(tableData)
, d_bindings(allocator)
, d_allocator_p(bslma_Default::allocator(allocator))
{
}

bcedb_InCoreDtagDbType::~bcedb_InCoreDtagDbType()
{
    bsl::vector<bcedb_InCoreDtagBindingType *>::iterator it =
                                                            d_bindings.begin();
    for (; it != d_bindings.end(); ++it) {
        d_allocator_p->deleteObjectRaw(*it);
    }

    d_bindings.clear();
}

// MANIPULATORS
void *bcedb_InCoreDtagDbType::getHandle()
{
    return (void *) this;
}

bcedb_DtagBindingType *bcedb_InCoreDtagDbType::createBinding(const char *table)
{
    const bdem_FieldDef *fldDef = d_data.recordDef().lookupField(table);
    if (0 == fldDef) {
        return 0;                                                     // RETURN
    }

    const bdem_RecordDef *tblInfo = fldDef->recordConstraint();
    BSLS_ASSERT(tblInfo);
    BSLS_ASSERT(tblInfo->numFields() > 0);
    BSLS_ASSERT(tblInfo->field(0).elemType() == bdem_ElemType::BDEM_TABLE);

    const char *tblDefName = tblInfo->fieldName(0);

    bcedb_InCoreDtagBindingType *b = new (*d_allocator_p)
        bcedb_InCoreDtagBindingType(
                d_data.field(table, tblDefName),
                bcema_SharedPtr<const bdem_RecordDef>(d_data.recordDefPtr(),
                                                      tblInfo),
                d_allocator_p);

    d_bindings.push_back(b);
    return b;
}

void bcedb_InCoreDtagDbType::destroyBinding(bcedb_DtagBindingType *binding)
{
    bsl::vector<bcedb_InCoreDtagBindingType *>::iterator it =
                                                            d_bindings.begin();
    for (; it != d_bindings.end(); ++it) {
        if (*it == binding) {
            d_bindings.erase(it);
            break;
        }
    }

    d_allocator_p->deleteObjectRaw(binding);
}

// ACCESSORS
void bcedb_InCoreDtagDbType::getDbTables(bsl::vector<bsl::string> *tables)
                                                                          const
{
    tables->clear();

    // Any "field" in the top db record that is a constrained list
    // is treated as a table.

    const bdem_RecordDef& dbDef = d_data.recordDef();
    for (int i = 0; i < dbDef.numFields(); ++i) {
        if (dbDef.field(i).elemType() == bdem_ElemType::BDEM_LIST
         && 0 != dbDef.field(i).recordConstraint()) {
            tables->push_back(bsl::string(dbDef.fieldName(i), d_allocator_p));
        }
    }
}

void bcedb_InCoreDtagDbType::getDbKeys(bdem_Schema *dest,
                                       const char  *tableName) const
{
    const bdem_FieldDef *fldDef = d_data.recordDef().lookupField(tableName);
    BSLS_ASSERT(fldDef);

    const bdem_RecordDef *tblInfo = fldDef->recordConstraint();
    BSLS_ASSERT(tblInfo);
    BSLS_ASSERT(tblInfo->numFields() > 1);

    dest->removeAll();

    // Skip the first field as it's the table definition.

    for (int i = 1; i < tblInfo->numFields(); ++i) {
        BSLS_ASSERT(tblInfo->field(i).elemType() == bdem_ElemType::BDEM_LIST);

        const bdem_RecordDef *keyRec = tblInfo->field(i).recordConstraint();
        BSLS_ASSERT(keyRec);

        bdem_RecordDef *newRec = dest->createRecord(tblInfo->fieldName(i));

        for (int j = 0; j < keyRec->numFields(); ++j) {
            const bdem_FieldDef& fldDef = keyRec->field(j);
            BSLS_ASSERT(0 == fldDef.recordConstraint());

            bdem_FieldDefAttributes fieldAttributes(fldDef.elemType(),
                                                    fldDef.isNullable(),
                                                    fldDef.formattingMode());
            if (fldDef.hasDefaultValue()) {
                fieldAttributes.defaultValue().replaceValue(
                                                        fldDef.defaultValue());
            }
            newRec->appendField(fieldAttributes,
                                fldDef.recordConstraint(),
                                keyRec->fieldName(j));
        }
    }
}

void bcedb_InCoreDtagDbType::appendOnDiskTagInfo(bdem_Schema *result,
                                                 const char  *table) const
{
    const bdem_FieldDef *fldDef = d_data.recordDef().lookupField(table);
    BSLS_ASSERT(fldDef);

    const bdem_RecordDef *tblRec = getTblDef(*fldDef);
    BSLS_ASSERT(tblRec);

    bdem_RecordDef *newRec = result->createRecord(table);

    for (int i = 0; i < tblRec->numFields(); ++i) {
        const bdem_FieldDef& fldDef = tblRec->field(i);
        BSLS_ASSERT(0 == fldDef.recordConstraint());

        bdem_FieldDefAttributes fieldAttributes(fldDef.elemType(),
                                                fldDef.isNullable(),
                                                fldDef.formattingMode());
        if (fldDef.hasDefaultValue()) {
            fieldAttributes.defaultValue().replaceValue(fldDef.defaultValue());
        }
        newRec->appendField(fieldAttributes,
                            fldDef.recordConstraint(),
                            tblRec->fieldName(i));
    }
}

                        // ---------------------------------
                        // class bcedb_InCoreDtagBindingType
                        // ---------------------------------

// CREATORS
bcedb_InCoreDtagBindingType::bcedb_InCoreDtagBindingType(
                       const bcem_Aggregate&                         table,
                       const bcema_SharedPtr<const bdem_RecordDef>&  keys,
                       bslma_Allocator                              *allocator)
: d_bound(0)
, d_controls(allocator)
, d_table(table)
, d_keys(keys)
, d_allocator_p(bslma_Default::allocator(allocator))
{
}

bcedb_InCoreDtagBindingType::~bcedb_InCoreDtagBindingType()
{
    bsl::vector<bcedb_InCoreDtagControlType *>::iterator it =
        d_controls.begin();
    for (; it != d_controls.end(); ++it) {
        d_allocator_p->deleteObjectRaw(*it);
    }

    d_controls.clear();
}

// MANIPULATORS
void *bcedb_InCoreDtagBindingType::getHandle()
{
    return (void *) this;
}

bcedb_DtagControlType *bcedb_InCoreDtagBindingType::createControl()
{
    bcedb_InCoreDtagControlType *c = new (*d_allocator_p)
        bcedb_InCoreDtagControlType(this, d_allocator_p);
    d_controls.push_back(c);
    return c;
}

void bcedb_InCoreDtagBindingType::destroyControl(bcedb_DtagControlType *c)
{
    bsl::vector<bcedb_InCoreDtagControlType *>::iterator it =
        d_controls.begin();
    for (; it != d_controls.end(); ++it) {
        if (*it == c) {
            d_controls.erase(it);
            break;
        }
    }

    d_allocator_p->deleteObjectRaw(c);
}

int bcedb_InCoreDtagBindingType::bind(bcem_Aggregate *tag)
{
    enum { GOOD = 0, BAD };

    if (d_bound || !bdem_SchemaUtil::isSymbolicSuperset(
                d_table.recordDef(), tag->recordDef())) {
        return BAD;                                                   // RETURN
    }

    d_bound = tag;
    return GOOD;
}

// ACCESSORS
const bcem_Aggregate *bcedb_InCoreDtagBindingType::getData() const
{
    return d_bound;
}

const char *bcedb_InCoreDtagBindingType::getSchemaFieldName(
                                                   const char *fieldName) const
{
    return fieldName;
}

int bcedb_InCoreDtagBindingType::maxLength(const char *) const
{
    return -1;  // bcem string fields are variable length
}

                        // ---------------------------------
                        // class bcedb_InCoreDtagControlType
                        // ---------------------------------

// CREATORS
bcedb_InCoreDtagControlType::bcedb_InCoreDtagControlType(
                                        bcedb_InCoreDtagBindingType *binding,
                                        bslma_Allocator             *allocator)
: d_binding_p(binding)
, d_position(-1)
, d_allocator_p(bslma_Default::allocator(allocator))
{
}

bcedb_InCoreDtagControlType::~bcedb_InCoreDtagControlType()
{
}

// MANIPULATORS
void *bcedb_InCoreDtagControlType::getHandle()
{
    return (void *) this;
}

int bcedb_InCoreDtagControlType::addRecordToDb()
{
    int p = d_binding_p->d_table.length();

    d_binding_p->d_table.appendItems(1);
    bcem_Aggregate a = d_binding_p->d_table[p];
    copyIn(&a, *(d_binding_p->d_bound));

    return 0;
}

int bcedb_InCoreDtagBindingType::updateData(const bcem_Aggregate& a)
{
    copyIn(d_bound, a);
    return 0;
}

int bcedb_InCoreDtagControlType::updateRecordInDb()
{
    BSLS_ASSERT(0 <= d_position);
    BSLS_ASSERT(d_position < d_binding_p->d_table.length());

    bcem_Aggregate tmp = d_binding_p->d_table[d_position];
    copyIn(&tmp, *(d_binding_p->d_bound));
    return 0;
}

int bcedb_InCoreDtagControlType::deleteRecordFromDb()
{
    BSLS_ASSERT(0 <= d_position);
    BSLS_ASSERT(d_position < d_binding_p->d_table.length());

    d_binding_p->d_table.removeItems(d_position--, 1);
    return 0;
}

// ACCESSORS
bcedb_DtagControlType::FIND_STATUS_CODE
bcedb_InCoreDtagControlType::findRecordInDb(const char *key,
                                            const char *partialCode) const
{
    return findRangeInDb(0, key, partialCode);
}

bcedb_DtagControlType::FIND_STATUS_CODE
bcedb_InCoreDtagControlType::findNextRecord() const
{
    int foundPos = d_position;
    d_position = -1;

    while (++foundPos < d_binding_p->d_table.length()) {
        int ret = comparePartial(d_binding_p->d_table[foundPos], d_search,
                                                                   d_partials);
        // Check for incorrect partial string specification
        if (-2 == ret) {
            return FIND_STATUS_NONE;                                  // RETURN
        }

        if (0 == ret) {
            if (d_position >= 0) {
                return FIND_STATUS_MANY;                              // RETURN
            }

            d_position = foundPos;
            copyOut(d_binding_p->d_bound, d_binding_p->d_table[d_position]);
        }

        if (d_range) {
            const bcem_Aggregate& bound = *(d_binding_p->d_bound);
            const bcem_Aggregate& end = *(d_range->getData());
            const bcem_Aggregate data = d_binding_p->d_table[foundPos];

            for (int i = 0; i < bound.recordDef().numFields(); ++i) {
                const char *n = bound.recordDef().fieldName(i);
                if (!bcem_Aggregate::areEquivalent(data.field(n),
                                                   end.field(n))) {
                    break;
                }

                if (i + 1 == bound.recordDef().numFields()) {
                    return d_position >= 0 ? FIND_STATUS_ONE
                                           : FIND_STATUS_NONE;        // RETURN
                }
            }
        }
    }

    return d_position >= 0 ? FIND_STATUS_ONE : FIND_STATUS_NONE;
}

bcedb_DtagControlType::FIND_STATUS_CODE
bcedb_InCoreDtagControlType::findPrevRecord() const
{
    int foundPos = d_position;
    d_position = -1;

    while (--d_position >= 0) {
        int ret = comparePartial(d_binding_p->d_table[d_position], d_search,
                                                                   d_partials);
        // Check for incorrect partial string specification
        if (-2 == ret) {
            return FIND_STATUS_NONE;                                  // RETURN
        }

        if (0 == ret) {
            if (d_position >= 0) {
                return FIND_STATUS_MANY;                              // RETURN
            }

            d_position = foundPos;
            copyOut(d_binding_p->d_bound, d_binding_p->d_table[d_position]);
        }
    }

    return d_position >= 0 ? FIND_STATUS_ONE : FIND_STATUS_NONE;
}

bcedb_DtagControlType::FIND_STATUS_CODE
bcedb_InCoreDtagControlType::findRangeInDbImp(
                                     int                   *count,
                                     bcedb_DtagBindingType *end,
                                     const char            *key,
                                     const char            *partialCode,
                                     bool                   doRangeCount) const
{
    if (end) {
        BSLS_ASSERT(end->getData());
        BSLS_ASSERT(bdem_SchemaUtil::isSymbolicSuperset(
                    end->getData()->recordDef(),
                    d_binding_p->d_bound->recordDef()));
    }

    if (doRangeCount) {
        // TBD: this component is only used to test the interface in the
        // absence of a real db.  It does not support passing back a count at
        // this point.
        *count = 0; // TBD
    }
    d_range = end;

    if (d_binding_p->d_table.length() == 0) {
        return bcedb_DtagControlType::FIND_STATUS_NONE;               // RETURN
    }

    // Find key record.
    const bdem_FieldDef *keyFld = d_binding_p->d_keys->lookupField(key);
    BSLS_ASSERT(keyFld);
    BSLS_ASSERT(keyFld->elemType() == bdem_ElemType::BDEM_LIST);

    const bdem_RecordDef *keyRec = keyFld->recordConstraint();
    BSLS_ASSERT(keyRec);
    BSLS_ASSERT(keyRec->numFields() > 0);

    // Create search aggregate (key rec + user query data).
    bcema_SharedPtr<const bdem_RecordDef> spKeyRec(d_binding_p->d_keys,
                                                   keyRec);

    createPartialSearch(&d_search, &d_partials,
            spKeyRec, partialCode, d_allocator_p);
    copyOut(&d_search, *(d_binding_p->d_bound));

    // Restart at beginning if end reached already.
    if (d_position >= d_binding_p->d_table.length())
        d_position = -1;

    return findNextRecord();
}

void bcedb_InCoreDtagDbType::printData(bsl::ostream& os) const
{
    os << "printData(): " << d_data << bsl::endl;
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
