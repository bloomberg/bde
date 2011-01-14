// bcedb_dtagfactory.cpp   -*-C++-*-
#include <bcedb_incoredtagfactory.h>
#include <bcema_sharedptr.h>
#include <bcem_aggregate.h>
#include <bdem_schema.h>
#include <bdem_schemautil.h>
#include <bdeut_strtokeniter.h>
#include <bdes_assert.h>

#include <cstdlib>
#include <cstring>
#include <ostream>
#include <string>
#include <vector>

namespace BloombergLP {

                        // =========================
                        // PRIVATE UTILITY FUNCTIONS
                        // =========================

namespace {

// Copy data from src to dest, where dest's schema is a subset of src's.

int copyOut(bcem_Aggregate *dest, const bcem_Aggregate& src)
{
    const bdem_RecordDef& dstRec = dest->recordDef();
    const bdem_RecordDef& srcRec = src.recordDef();

    BDE_ASSERT_CPP(src.dataType() == bdem_ElemType::LIST
            || src.dataType() == bdem_ElemType::ROW);
    BDE_ASSERT_CPP(dest->dataType() == bdem_ElemType::LIST
            || dest->dataType() == bdem_ElemType::ROW);

    for (int i = 0; i < dstRec.length(); ++i) {
        const char *name = dstRec.fieldName(i);
        int srcField = srcRec.fieldIndex(name);
        BDE_ASSERT_CPP(srcField >= 0);
        BDE_ASSERT_CPP(srcRec.field(srcField).elemType()
                == dstRec.field(i).elemType());

        dest->setField(name, src.field(name));
    }

    return 0;
}

// Copy data from src to dest, where src's schema is a subset of dest's.
// The fields in dest that are not part of that subset will remain
// untouched.

int copyIn(bcem_Aggregate *dest, const bcem_Aggregate& src)
{
    const bdem_RecordDef& dstRec = dest->recordDef();
    const bdem_RecordDef& srcRec = src.recordDef();

    BDE_ASSERT_CPP(src.dataType() == bdem_ElemType::LIST
            || src.dataType() == bdem_ElemType::ROW);
    BDE_ASSERT_CPP(dest->dataType() == bdem_ElemType::LIST
            || dest->dataType() == bdem_ElemType::ROW);

    for (int i = 0; i < srcRec.length(); ++i) {
        const char *name = srcRec.fieldName(i);
        int dstField = dstRec.fieldIndex(name);
        BDE_ASSERT_CPP(dstField >= 0);
        BDE_ASSERT_CPP(dstRec.field(dstField).elemType()
                == srcRec.field(i).elemType());

        bcem_Aggregate a = src.field(name);
        dest->setField(name, a);
    }

    return 0;
}

// Populates the private members d_search and d_partials for
// InCoreDtagControlType.

void createPartialSearch(
        bcem_Aggregate                                 *result,
        std::vector<int>                               *partials,
        const bcema_SharedPtr<const bdem_RecordDef>&    key,
        const char *                                    partialCode,
        bdema_Allocator                                *allocator)
{
    partials->clear();

    if (partialCode) {
        bdeut_StrTokenIter it(partialCode, "", "+:");
        bcema_SharedPtr<bdem_Schema> schema;
        schema.createInplace(allocator);
        bdem_RecordDef *rdResult = schema->createRecord();

        int expected = 0;

        for (; it; ++it) {
            using std::strcmp;

            BDE_ASSERT_CPP(expected < key->length());
            BDE_ASSERT_CPP(!strcmp(it(), key->fieldName(expected)));

            rdResult->appendField(key->field(expected++).fieldSpec(), it());

            int partial = -1;

            if (!strcmp(it.delimiter(), ":")) {
                ++it;
                partial = std::atoi(it());
            }

            partials->push_back(partial);
        }

        *result = bcem_Aggregate(
                bcema_SharedPtr<const bdem_RecordDef>(schema, rdResult)
                );
    }

    else {
        *result = bcem_Aggregate(key);
        partials->resize(key->length(), -1);
    }
}

// Locate the record defining a table, given its entry as a field from
// the DB schema. Basically, just extract the first field of the
// intermediate record and do some sanity assertions.

inline const bdem_RecordDef *getTblDef(const bdem_FieldDef& tblEntry)
{
    const bdem_RecordDef *tblInfo = tblEntry.constraint();
    BDE_ASSERT_CPP(tblInfo);
    BDE_ASSERT_CPP(tblInfo->length() > 0);
    BDE_ASSERT_CPP(tblInfo->field(0).elemType() == bdem_ElemType::TABLE);

    return tblInfo->field(0).constraint();
}

// Compare data in 'sub' to the data in 'super' that overlaps with 'sub',
// respecting any non-negative partial lengths for string fields. Returns
// 0 for equality, non-zero otherwise.

int comparePartial(
        const bcem_Aggregate& super,
        const bcem_Aggregate& sub,
        const std::vector<int>& partials)
{
    BDE_ASSERT_CPP(bdem_SchemaUtil::isSymbolicSuperset(super.recordDef(),
                sub.recordDef()));
    BDE_ASSERT_CPP(partials.size() == (unsigned)sub.recordDef().length());

    for (int i = 0; i < sub.recordDef().length(); ++i) {
        const char *n = sub.recordDef().fieldName(i);

        if (partials[i] >= 0) {
            BDE_ASSERT_CPP(super.field(n).dataType() == bdem_ElemType::STRING);
            BDE_ASSERT_CPP(sub.field(n).dataType() == bdem_ElemType::STRING);

            if (std::strncmp(super.field(n).asString().c_str(),
                        sub.field(n).asString().c_str(), partials[i]))
                return -1;
        }

        else if (! bcem_Aggregate::areEquivalent(super.field(n), sub.field(n)))
            return -1;
    }

    return 0;
}

} // close unnamed namespace

                        // =============================
                        // class bcedb_InCoreDtagFactory
                        // =============================

bcedb_InCoreDtagFactory::bcedb_InCoreDtagFactory(
        const bcema_SharedPtr<const bdem_RecordDef>& schemaShp,
        bdema_Allocator *allocator)
    : d_allocator_p(bdema_Default::allocator(allocator))
    , d_data(schemaShp, d_allocator_p)
    , d_dbs(d_allocator_p)
{
}

bcedb_InCoreDtagFactory::~bcedb_InCoreDtagFactory()
{
    std::vector<bcedb_InCoreDtagDbType *>::iterator it = d_dbs.begin();
    for (; it != d_dbs.end(); ++it) {
        d_allocator_p->deleteObject(*it);
    }

    d_dbs.clear();
}

void bcedb_InCoreDtagFactory::destroyDb(bcedb_DtagDbType *db)
{
    std::vector<bcedb_InCoreDtagDbType *>::iterator it = d_dbs.begin();
    for (; it != d_dbs.end(); ++it) {
        if (*it == db) {
            d_dbs.erase(it);
            break;
        }
    }

    d_allocator_p->deleteObject(db);
}

bcedb_DtagDbType *bcedb_InCoreDtagFactory::createDb(const char *dbName)
{
    if (!d_data.hasField(dbName)) return 0;
    BDE_ASSERT_CPP(d_data.field(dbName).dataType() == bdem_ElemType::LIST);

    bcedb_InCoreDtagDbType *newDb =
        new (*d_allocator_p) bcedb_InCoreDtagDbType(d_data.field(dbName),
                d_allocator_p);

    d_dbs.push_back(newDb);

    return newDb;
}

                        // ============================
                        // class bcedb_InCoreDtagDbType
                        // ============================

bcedb_InCoreDtagDbType::bcedb_InCoreDtagDbType(
            bcem_Aggregate tableData,
            bdema_Allocator *allocator)
    : d_allocator_p(bdema_Default::allocator(allocator))
    , d_data(tableData)
    , d_bindings(d_allocator_p)
{
}

bcedb_InCoreDtagDbType::~bcedb_InCoreDtagDbType()
{
    std::vector<bcedb_InCoreDtagBindingType *>::iterator it = d_bindings.begin();
    for (; it != d_bindings.end(); ++it) {
        d_allocator_p->deleteObject(*it);
    }
    
    d_bindings.clear();
}

void *bcedb_InCoreDtagDbType::getHandle()
{
    return (void *) this;
}

void bcedb_InCoreDtagDbType::getDbTables(std::vector<std::string> *tables) const
{
    tables->clear();

    // Any "field" in the top db record that is a constrained list
    // is treated as a table:

    const bdem_RecordDef& dbDef = d_data.recordDef();
    for (int i = 0; i < dbDef.length(); ++i) {
        if (dbDef.field(i).elemType() == bdem_ElemType::LIST
                && 0 != dbDef.field(i).constraint())
            tables->push_back(std::string(dbDef.fieldName(i), d_allocator_p));
    }
}

void bcedb_InCoreDtagDbType::getDbKeys(
        bdem_Schema *dest, const char *tableName) const
{
    const bdem_FieldDef *fldDef = d_data.recordDef().lookupField(tableName);
    BDE_ASSERT_CPP(fldDef);

    const bdem_RecordDef *tblInfo = fldDef->constraint();
    BDE_ASSERT_CPP(tblInfo);
    BDE_ASSERT_CPP(tblInfo->length() > 1);

    dest->removeAll();

    // skip the first field as it's the table definition
    for (int i = 1; i < tblInfo->length(); ++i) {
        BDE_ASSERT_CPP(tblInfo->field(i).elemType() == bdem_ElemType::LIST);

        const bdem_RecordDef *keyRec = tblInfo->field(i).constraint();
        BDE_ASSERT_CPP(keyRec);

        bdem_RecordDef *newRec = dest->createRecord(tblInfo->fieldName(i));

        for (int j = 0; j < keyRec->length(); ++j) {
            BDE_ASSERT_CPP(0 == keyRec->field(j).constraint());
            newRec->appendField(keyRec->field(j).fieldSpec(),
                    keyRec->fieldName(j));
        }
    }
}

void bcedb_InCoreDtagDbType::appendOnDiskTagInfo(
        bdem_Schema *result, const char *table) const
{
    const bdem_FieldDef *fldDef = d_data.recordDef().lookupField(table);
    BDE_ASSERT_CPP(fldDef);

    const bdem_RecordDef *tblRec = getTblDef(*fldDef);
    BDE_ASSERT_CPP(tblRec);

    bdem_RecordDef *newRec = result->createRecord(table);

    for (int i = 0; i < tblRec->length(); ++i) {
        const bdem_FieldDef& fldDef = tblRec->field(i);
        BDE_ASSERT_CPP(0 == fldDef.constraint());

        newRec->appendField(fldDef.fieldSpec(), tblRec->fieldName(i));
    }
}

void bcedb_InCoreDtagDbType::destroyBinding(bcedb_DtagBindingType *binding)
{
    std::vector<bcedb_InCoreDtagBindingType *>::iterator it = d_bindings.begin();
    for (; it != d_bindings.end(); ++it) {
        if (*it == binding) {
            d_bindings.erase(it);
            break;
        }
    }

    d_allocator_p->deleteObject(binding);
}

bcedb_DtagBindingType *bcedb_InCoreDtagDbType::createBinding(const char *table)
{
    const bdem_FieldDef *fldDef = d_data.recordDef().lookupField(table);
    if (0 == fldDef) return 0;

    const bdem_RecordDef *tblInfo = fldDef->constraint();
    BDE_ASSERT_CPP(tblInfo);
    BDE_ASSERT_CPP(tblInfo->length() > 0);
    BDE_ASSERT_CPP(tblInfo->field(0).elemType() == bdem_ElemType::TABLE);

    const char *tblDefName = tblInfo->fieldName(0);

    bcedb_InCoreDtagBindingType *b = new (*d_allocator_p)
        bcedb_InCoreDtagBindingType(
                d_data.field(table, tblDefName),
                bcema_SharedPtr<const bdem_RecordDef>(
                    d_data.recordDefPtr(), tblInfo),
                d_allocator_p);

    d_bindings.push_back(b);
    return b;
}

                        // =================================
                        // class bcedb_InCoreDtagBindingType
                        // =================================

bcedb_InCoreDtagBindingType::bcedb_InCoreDtagBindingType(
        const bcem_Aggregate& table,
        const bcema_SharedPtr<const bdem_RecordDef>& keys,
        bdema_Allocator *allocator)
    : d_allocator_p(bdema_Default::allocator(allocator))
    , d_bound(0)
    , d_controls(d_allocator_p)
    , d_table(table)
    , d_keys(keys)
{
}

void *bcedb_InCoreDtagBindingType::getHandle()
{
    return (void *) this;
}

int bcedb_InCoreDtagBindingType::bind(bcem_Aggregate *tag)
{
    enum { GOOD = 0, BAD };

    if (d_bound || !bdem_SchemaUtil::isSymbolicSuperset(
                d_table.recordDef(), tag->recordDef())) {
        return BAD;
    }

    d_bound = tag;
    return GOOD;
}

const bcem_Aggregate *bcedb_InCoreDtagBindingType::getData() const
{
    return d_bound;
}

int bcedb_InCoreDtagBindingType::maxLength(const char *f) const
{
    return -1;      // bcem string fields are variable length
}

bcedb_InCoreDtagBindingType::~bcedb_InCoreDtagBindingType()
{
    std::vector<bcedb_InCoreDtagControlType *>::iterator it =
        d_controls.begin();
    for (; it != d_controls.end(); ++it) {
        d_allocator_p->deleteObject(*it);
    }

    d_controls.clear();
}

void bcedb_InCoreDtagBindingType::destroyControl(bcedb_DtagControlType *c)
{
    std::vector<bcedb_InCoreDtagControlType *>::iterator it =
        d_controls.begin();
    for (; it != d_controls.end(); ++it) {
        if (*it == c) {
            d_controls.erase(it);
            break;
        }
    }

    d_allocator_p->deleteObject(c);
}

bcedb_DtagControlType *bcedb_InCoreDtagBindingType::createControl()
{
    bcedb_InCoreDtagControlType *c = new (*d_allocator_p)
        bcedb_InCoreDtagControlType(this, d_allocator_p);
    d_controls.push_back(c);
    return c;
}

                        // =================================
                        // class bcedb_InCoreDtagControlType
                        // =================================

bcedb_InCoreDtagControlType::bcedb_InCoreDtagControlType(
            bcedb_InCoreDtagBindingType *binding,
            bdema_Allocator *allocator)
    : d_allocator_p(bdema_Default::allocator(allocator))
    , d_binding_p(binding)
    , d_position(-1)
{
}

bcedb_InCoreDtagControlType::~bcedb_InCoreDtagControlType()
{
}

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

bcedb_DtagControlType::FIND_STATUS_CODE
bcedb_InCoreDtagControlType::findRecordInDb(
        const char *key,
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
        if (0 == comparePartial(
                    d_binding_p->d_table[foundPos],
                    d_search, d_partials)) {
            if (d_position >= 0) {
                return FIND_STATUS_MANY;
            }

            d_position = foundPos;
            copyOut(d_binding_p->d_bound, d_binding_p->d_table[d_position]);
        }

        if (d_range) {
            const bcem_Aggregate& bound = *(d_binding_p->d_bound);
            const bcem_Aggregate& end = *(d_range->getData());
            const bcem_Aggregate data = d_binding_p->d_table[foundPos];

            for (int i = 0; i < bound.recordDef().length(); ++i) {
                const char *n = bound.recordDef().fieldName(i);
                if (!bcem_Aggregate::areEquivalent(data.field(n),end.field(n)))
                    break;
                if (i + 1 == bound.recordDef().length())
                    return d_position >= 0 ? FIND_STATUS_ONE : FIND_STATUS_NONE;
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
        if (0 == comparePartial(
                    d_binding_p->d_table[d_position],
                    d_search, d_partials)) {
            if (d_position >= 0) {
                return FIND_STATUS_MANY;
            }

            d_position = foundPos;
            copyOut(d_binding_p->d_bound, d_binding_p->d_table[d_position]);
        }
    }

    return d_position >= 0 ? FIND_STATUS_ONE : FIND_STATUS_NONE;
}

bcedb_DtagControlType::FIND_STATUS_CODE
bcedb_InCoreDtagControlType::findRangeInDb(
        bcedb_DtagBindingType *end,
        const char *key,
        const char *partialCode) const
{
    if (end) {
        BDE_ASSERT_CPP(end->getData());
        BDE_ASSERT_CPP(bdem_SchemaUtil::isSymbolicSuperset(
                    end->getData()->recordDef(),
                    d_binding_p->d_bound->recordDef()));
    }

    d_range = end;

    if (d_binding_p->d_table.length() == 0)
        return bcedb_DtagControlType::FIND_STATUS_NONE;

    // find key record
    const bdem_FieldDef *keyFld = d_binding_p->d_keys->lookupField(key);
    BDE_ASSERT_CPP(keyFld);
    BDE_ASSERT_CPP(keyFld->elemType() == bdem_ElemType::LIST);

    const bdem_RecordDef *keyRec = keyFld->constraint();
    BDE_ASSERT_CPP(keyRec);
    BDE_ASSERT_CPP(keyRec->length() > 0);

    // create search aggregate (key rec + user query data)
    bcema_SharedPtr<const bdem_RecordDef> spKeyRec(
            d_binding_p->d_keys, keyRec);

    createPartialSearch(&d_search, &d_partials,
            spKeyRec, partialCode, d_allocator_p);
    copyOut(&d_search, *(d_binding_p->d_bound));

    // restart at beginning if end reached already
    if (d_position >= d_binding_p->d_table.length())
        d_position = -1;

    // go
    return findNextRecord();
}

int bcedb_InCoreDtagBindingType::updateData(const bcem_Aggregate& a)
{
    copyIn(d_bound, a);
    return 0;
}

int bcedb_InCoreDtagControlType::updateRecordInDb()
{
    BDE_ASSERT_CPP(0 <= d_position);
    BDE_ASSERT_CPP(d_position < d_binding_p->d_table.length());

    bcem_Aggregate tmp = d_binding_p->d_table[d_position];
    copyIn(&tmp, *(d_binding_p->d_bound));
    return 0;
}

int bcedb_InCoreDtagControlType::deleteRecordFromDb()
{
    BDE_ASSERT_CPP(0 <= d_position);
    BDE_ASSERT_CPP(d_position < d_binding_p->d_table.length());

    d_binding_p->d_table.removeItems(d_position--, 1);
    return 0;
}

void bcedb_InCoreDtagDbType::printData(std::ostream& os)
{
    os << "printData(): " << d_data << std::endl;
}

}
