// bdem_schema.cpp                                                    -*-C++-*-
#include <bdem_schema.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_schema_cpp,"$Id$ $CSID$")

#include <bdem_enumerationdef.h>
#include <bdem_fielddef.h>
#include <bdem_fielddefattributes.h>
#include <bdem_recorddef.h>

#include <bdetu_unset.h>

#include <bdeu_print.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bsls_assert.h>
#include <bsls_objectbuffer.h>

#include <bsl_climits.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>

namespace BloombergLP {

namespace {

                     // =========================
                     // local class AutoEraseName
                     // =========================

class AutoEraseName {
    // This class provides a specialized proctor object that, upon destruction,
    // erases a managed key from a managed map.

    // PRIVATE TYPES
    typedef bsl::map<const char *,
                     int,
                     bdem_Schema_NameCompare> ManagedContainerType;
        // Alias for a mapping of C-strings to integers.  Note that
        // 'ManagedContainerType' defines the type of container whose elements
        // are proctored by a 'AutoEraseName' object.

    // DATA
    ManagedContainerType *d_fieldNames_p;  // managed map
    const char           *d_name_p;        // managed key in map

  public:
    // CREATORS
    AutoEraseName()
        // Create a proctor that initially manages no objects.
    : d_fieldNames_p(0)
    , d_name_p(0)
    {
    }

    ~AutoEraseName()
        // If any objects are under management by this proctor, then erase the
        // managed key from the managed map; otherwise do nothing.
    {
        if (d_fieldNames_p && d_name_p) {
            d_fieldNames_p->erase(d_name_p);
        }
    }

    // MANIPULATORS
    void manage(ManagedContainerType *fieldNames, const char *name)
        // Set this proctor to manage the specified 'fieldNames' map and the
        // specified 'name' key.  Any objects currently under management by
        // this proctor (if any) are released from management.  The behavior is
        // undefined unless 'name' is a key in 'fieldNames'.
    {
        d_fieldNames_p = fieldNames;
        d_name_p       = name;
    }

    void release()
        // Release from management any objects managed by this proctor.
    {
        d_fieldNames_p = 0;
        d_name_p       = 0;
    }
};

                     // =========================
                     // local class AutoRemoveAll
                     // =========================

class AutoRemoveAll {
    // This class provides a specialized 'bdem_Schema' proctor object that
    // removes (and destroys) all of the record definitions from its held
    // 'bdem_Schema' upon destruction unless its 'release' method is called.
    // !WARNING! This class relies on a stronger contract for the 'removeAll'
    // method than the 'bdem_Schema' interface advertizes.  Any change to the
    // implementation of the 'bdem_Schema::removeAll' method *must* be
    // checked against the functioning of this proctor class.
    //
    // Note that reliance on a stronger-than-advertized contract is tolerated
    // because the alternative is to pass into this class on instantiation the
    // five relevant private data members of the proctored schema.

    // DATA
    bdem_Schema *d_cargo_p;  // Temporarily managed schema.

  public:
    // CREATORS
    AutoRemoveAll(bdem_Schema *schema)
        // Create a proctor to manage the specified 'schema'.
    : d_cargo_p(schema)
    {
    }

    ~AutoRemoveAll()
        // Remove (and destroy) all records from the managed schema, if any.
    {
        if (d_cargo_p) {
            d_cargo_p->removeAll();
        }
    }

    // MANIPULATORS
    void release()
        // Release from management any managed contents of this object.
    {
        d_cargo_p = 0;
    }
};

}  // close unnamed namespace

// STATIC HELPER FUNCTIONS
static
char *copyName(const char *name, bslma_Allocator *basicAllocator)
{
    BSLS_ASSERT(name);
    BSLS_ASSERT(basicAllocator);

    const int len = bsl::strlen(name) + 1;
    char *newName = (char *)basicAllocator->allocate(len);
    bsl::memcpy(newName, name, len);

    return newName;
}

                        // -----------------
                        // class bdem_Schema
                        // -----------------

// Note regarding "minor" versions: the use of minor 'bdex' streaming versions
// is novel to 'bdem_Schema'.  The currently supported minor versions and
// the features that mandate use of them are as follows:
//..
//  minor version #   schema features
//  ---------------   --------------------------------------------------------
//     0              (corresponds to the "original" 'bdem_Schema' prior to
//                     the introduction of minor version numbers)
//
//     1              'BDEM_CHOICE_RECORD'; field IDs; 'CHOICE', 'BOOL', "TZ",
//                    and the other newer element types; forward references;
//                    fields having anything but the default nullability for
//                    their type; formatting modes; default values (i.e.,
//                    non-unset values only)
//
//     2              enumerations
//
//     3              default values that correspond to unset values
//..
// The maximum minor version number dictated by the features used within the
// schema must be used.

// PRIVATE MANIPULATORS
int bdem_Schema::setRecordConstraints(bdem_RecordDef    *recordDef,
                                      bsl::vector<int>&  constraintIndices)
{
    BSLS_ASSERT(recordDef);
    BSLS_ASSERT(static_cast<int>(constraintIndices.size()) ==
                                                       recordDef->numFields());

    for (int i = 0; i < recordDef->numFields(); ++i) {
        bdem_FieldDef *fieldDef =
                             const_cast<bdem_FieldDef *>(&recordDef->field(i));
        bdem_ElemType::Type elemType = fieldDef->elemType();
        int constraintIndex = constraintIndices[i];
        if (bdem_EnumerationDef::canHaveEnumConstraint(elemType)) {
            if (constraintIndex < -1
             || constraintIndex >= numEnumerations()) {
                return -1;                                            // RETURN
            }
            if (constraintIndex >= 0) {
                fieldDef->setConstraint(&enumeration(constraintIndex));
            }
        }
        else if (bdem_ElemType::isAggregateType(elemType)) {
            if (constraintIndex < -1
             || constraintIndex >= numRecords()) {
                    return -1;                                        // RETURN
            }
            if (constraintIndex >= 0) {
                if (!bdem_RecordDef::isValidConstraint(
                                          record(constraintIndex), elemType)) {
                    return -1;                                        // RETURN
                }
                fieldDef->setConstraint(&record(constraintIndex));
            }
        }
        else if (constraintIndex != -1) {
            // This field type may not be constrained.

            return -1;                                                // RETURN
        }
    }
    return 0;
}

// PRIVATE ACCESSORS
int bdem_Schema::bdexMinorVersion() const
{
    int minorVersion = 0;

    if (d_enumDefs.size()) {

        // If there are any enumerations, use stream version 1.2.

        minorVersion = 2;
    }

    const int numRecs = numRecords();
    for (int r = 0; r < numRecs; ++r) {
        const bdem_RecordDef& recordDef = record(r);
        if (minorVersion < 1) {
            if (bdem_RecordDef::BDEM_SEQUENCE_RECORD != recordDef.recordType()
             || recordDef.hasFieldIds()) {

                // If there are 'CHOICE' records, or record definitions with
                // assigned IDs, use stream version 1.1.

                minorVersion = 1;
            }
        }

        const int numFields = recordDef.numFields();
        for (int f = 0; f < numFields; ++f) {
            const bdem_FieldDef&      fieldDef = recordDef.field(f);
            const bdem_ElemType::Type type     = fieldDef.elemType();

            if (minorVersion < 1) {
                switch (type) {
                  case bdem_ElemType::BDEM_BOOL:
                  case bdem_ElemType::BDEM_DATETIMETZ:
                  case bdem_ElemType::BDEM_DATETZ:
                  case bdem_ElemType::BDEM_TIMETZ:
                  case bdem_ElemType::BDEM_BOOL_ARRAY:
                  case bdem_ElemType::BDEM_DATETIMETZ_ARRAY:
                  case bdem_ElemType::BDEM_DATETZ_ARRAY:
                  case bdem_ElemType::BDEM_TIMETZ_ARRAY:
                  case bdem_ElemType::BDEM_CHOICE:
                  case bdem_ElemType::BDEM_CHOICE_ARRAY: {

                    // If the schema uses any of the above types, use stream
                    // version 1.1.

                    minorVersion = 1;
                  } break;
                  default: {
                  } break;
                }

                const bdem_RecordDef *constraint = fieldDef.recordConstraint();
                const int recordIndex            = recordDef.recordIndex();
                const int constraintIndex        = constraint
                                                   ? constraint->recordIndex()
                                                   : -1;
                const bool isNullable = fieldDef.isNullable();
                if (constraintIndex > recordIndex) {

                    // If there are any forward references, use stream version
                    // 1.1.

                    minorVersion = 1;
                }
                else if ((bdem_ElemType::BDEM_LIST != type && !isNullable)
                      || (bdem_ElemType::BDEM_LIST == type &&  isNullable
                       && constraintIndex != recordIndex)) {

                    // If nullable constraint is non-default, use stream
                    // version 1.1.

                    minorVersion = 1;
                }
                else if (0 != fieldDef.formattingMode()
                      || fieldDef.hasDefaultValue()) {

                    // If either formatting modes or default values exist, use
                    // stream version 1.1.

                    minorVersion = 1;
                }
            }

            if (minorVersion < 3
             && !bdem_ElemType::isAggregateType(type)
             && fieldDef.hasDefaultValue()) {

                bdem_ConstElemRef elemRef(fieldDef.defaultValue());
                if (elemRef.descriptor()->isUnset(elemRef.data())) {

                    // If the (necessarily non-aggregate) field has a default
                    // value and that value is the "unset" value for its type,
                    // use stream version 1.3.  3 is the maximum possible minor
                    // version, so just return.

                    return 3;                                         // RETURN
                }
            }
        }
    }

    return minorVersion;  // Return the highest minor version required.
}

// CREATORS
bdem_Schema::bdem_Schema(bslma_Allocator *basicAllocator)
: d_writeOnceAlloc(basicAllocator)
, d_recordDefs(basicAllocator)
, d_recordNames(bdem_Schema_NameCompare(), basicAllocator)
, d_enumDefs(basicAllocator)
, d_enumNames(bdem_Schema_NameCompare(), basicAllocator)
{
}

bdem_Schema::bdem_Schema(const bdem_Schema&  original,
                         bslma_Allocator    *basicAllocator)
: d_writeOnceAlloc(basicAllocator)
, d_recordDefs(basicAllocator)
, d_recordNames(bdem_Schema_NameCompare(), basicAllocator)
, d_enumDefs(basicAllocator)
, d_enumNames(bdem_Schema_NameCompare(), basicAllocator)
{
    *this = original;
}

bdem_Schema::~bdem_Schema()
{
    removeAll();
}

// MANIPULATORS
bdem_Schema& bdem_Schema::operator=(const bdem_Schema& rhs)
{
    if (this == &rhs) {
        return *this;                                                 // RETURN
    }

    // Clear everything and return memory to the sequential allocator.

    removeAll();

    AutoRemoveAll autoRemover(this);

    const int numRecs  = rhs.numRecords();
    const int numEnums = rhs.numEnumerations();

    // Create all records and enumerations in advance to allow for forward
    // references.

    for (int r = 0; r < numRecs; ++r) {
        createRecord(rhs.recordName(r), rhs.record(r).recordType());
    }

    for (int e = 0; e < numEnums; ++e) {
        createEnumeration(rhs.enumerationName(e));
    }

    for (int r = 0; r < numRecs; ++r) {
        bdem_RecordDef const& rhsRecDef  = rhs.record(r);
        bdem_RecordDef&       thisRecDef = *d_recordDefs[r];

        const int numFields = rhsRecDef.numFields();
        for (int f = 0; f < numFields; ++f) {
            const bdem_FieldDef&      fieldDef = rhsRecDef.field(f);
            const bdem_ElemType::Type elemType = fieldDef.elemType();

            bdem_FieldDefAttributes attributes(elemType,
                                               fieldDef.isNullable(),
                                               fieldDef.formattingMode());

            const bdem_RecordDef      *recConstraint =
                                              fieldDef.recordConstraint();
            const bdem_EnumerationDef *enumConstraint =
                                              fieldDef.enumerationConstraint();

            // Copy default value (array or scalar only).

            if (!bdem_ElemType::isAggregateType(elemType)) {

                // Note that the 'replaceValue' method handles null values.

                attributes.defaultValue().replaceValue(
                                                      fieldDef.defaultValue());
            }

            const int rhsFieldId = rhsRecDef.hasFieldIds()
                                   ? rhsRecDef.fieldId(f)
                                   : bdem_RecordDef::BDEM_NULL_FIELD_ID;

            if (enumConstraint) {
                BSLS_ASSERT(
                         bdem_EnumerationDef::canHaveEnumConstraint(elemType));

                int enumIndex = enumConstraint->enumerationIndex();
                thisRecDef.appendField(attributes,
                                       d_enumDefs[enumIndex],
                                       rhsRecDef.fieldName(f),
                                       rhsFieldId);
            }
            else {
                // If 'enumConstraint' is 0, then 'rhsRecDef' must either be a
                // record constraint, or have no constraint at all.

                const bdem_RecordDef *newRecConstraint = 0;
                if (recConstraint) {
                    BSLS_ASSERT(bdem_ElemType::isAggregateType(elemType));

                    int recIndex = recConstraint->recordIndex();
                    newRecConstraint = d_recordDefs[recIndex];
                }
                thisRecDef.appendField(attributes,
                                       newRecConstraint,
                                       rhsRecDef.fieldName(f),
                                       rhsFieldId);
            }
        }
    }

    for (int e = 0; e < numEnums; ++e) {
        bdem_EnumerationDef const& rhsEnumDef  = rhs.enumeration(e);
        bdem_EnumerationDef&       thisEnumDef = *d_enumDefs[e];

        const int endEnumId = bdetu_Unset<int>::unsetValue();
        for (int enumId = rhsEnumDef.nextLargerId(INT_MIN).second;
             enumId != endEnumId;
             enumId = rhsEnumDef.nextLargerId(enumId).second) {
            thisEnumDef.addEnumerator(rhsEnumDef.lookupName(enumId), enumId);
        }
    }

    autoRemover.release();

    return *this;
}

bdem_RecordDef *bdem_Schema::createRecord(const char                 *name,
                                          bdem_RecordDef::RecordType  type)
{
    if (name && (lookupRecord(name) || lookupEnumeration(name))) {
        return 0;                                                     // RETURN
    }

    int recordIndex = d_recordDefs.size();

    // Ensure that vector has sufficient capacity by pushing then popping a
    // single element.  This is better than calling 'reserve' because it
    // preserves exponential growth.  After the 'pop_back', a 'push_back' is
    // guaranteed to succeed without throwing an exception.

    d_recordDefs.push_back(0);
    d_recordDefs.pop_back();

    char *recordName = name ? copyName(name, &d_writeOnceAlloc) : 0;

    AutoEraseName nameProctor;
    if (recordName) {
        d_recordNames[recordName] = recordIndex;
        nameProctor.manage(&d_recordNames, recordName);
    }

    bdem_RecordDef *pR =
          (bdem_RecordDef *) d_writeOnceAlloc.allocate(sizeof(bdem_RecordDef));

    new(pR) bdem_RecordDef(this,
                           recordIndex,
                           recordName,
                           type,
                           &d_writeOnceAlloc,
                           allocator());
    d_recordDefs.push_back(pR);  // Cannot throw because we reserved a slot.

    nameProctor.release();

    return pR;
}

bdem_EnumerationDef *bdem_Schema::createEnumeration(const char *name)
{
    if (name && (lookupRecord(name) || lookupEnumeration(name))) {
        return 0;                                                     // RETURN
    }

    int enumerationIndex = d_enumDefs.size();

    // Ensure that vector has sufficient capacity by pushing then popping a
    // single element.  This is better than calling 'reserve' because it
    // preserves exponential growth.  After the 'pop_back', a 'push_back' is
    // guaranteed to succeed without throwing an exception.

    d_enumDefs.push_back(0);
    d_enumDefs.pop_back();

    char *enumerationName = name ? copyName(name, &d_writeOnceAlloc) : 0;

    AutoEraseName nameProctor;
    if (enumerationName) {
        d_enumNames[enumerationName] = enumerationIndex;
        nameProctor.manage(&d_enumNames, enumerationName);
    }

    bdem_EnumerationDef *pE = (bdem_EnumerationDef *)
        d_writeOnceAlloc.allocate(sizeof(bdem_EnumerationDef));

    new (pE) bdem_EnumerationDef(this,
                                 enumerationIndex,
                                 enumerationName,
                                 &d_writeOnceAlloc);
    d_enumDefs.push_back(pE);  // Cannot throw because we reserved a slot.

    nameProctor.release();

    return pE;
}

void bdem_Schema::removeAll()
{
    // !WARNING! This implementation provides a stronger contract than that
    // specified in the interface, namely, this method will succeed even if
    // 'd_recordDefs' and 'd_recordNames' are mutually inconsistent with
    // respect to a valid schema value (but are themselves valid).  Similarly,
    // this method will succeed even if 'd_enumDefs' and 'd_enumNames' are
    // mutually inconsistent.  The proctor class 'AutoRemoveAll' implemented in
    // this file relies on the stronger contract.  !ANY! change to this
    // implementation must be analyzed for impact on the correct functioning of
    // 'AutoRemoveAll'.  See the documentation of 'AutoRemoveAll' for an
    // explanation of this unusual reliance, and for the reason why the
    // resulting (internal) cyclic dependency is tolerated.

    bsl::vector<bdem_RecordDef*>::iterator recIter = d_recordDefs.begin();
    while (recIter != d_recordDefs.end()) {
        (*recIter)->~bdem_RecordDef();
        ++recIter;
    }
    d_recordDefs.clear();
    d_recordNames.clear();

    bsl::vector<bdem_EnumerationDef*>::iterator enumIter = d_enumDefs.begin();
    while (enumIter != d_enumDefs.end()) {
        (*enumIter)->~bdem_EnumerationDef();
        ++enumIter;
    }
    d_enumDefs.clear();
    d_enumNames.clear();

    d_writeOnceAlloc.release();
}

// ACCESSORS
bsl::ostream&
bdem_Schema::print(bsl::ostream& stream,
                   int           level,
                   int           spacesPerLevel) const
{
    if (level > 0) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }
    else {
        level = -level;
    }

    stream << '{';
    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    ++level;

    // Print record definitions.

    bsl::vector<bdem_RecordDef*>::const_iterator recIter =
                                                          d_recordDefs.begin();
    while (recIter != d_recordDefs.end()) {

        (*recIter)->print(stream, level, spacesPerLevel);

        if (spacesPerLevel < 0) {
            stream << ' ';
        }

        ++recIter;
    }

    // Print enumeration definitions.

    bsl::vector<bdem_EnumerationDef*>::const_iterator enumIter =
                                                            d_enumDefs.begin();
    while (enumIter != d_enumDefs.end()) {

        (*enumIter)->print(stream, level, spacesPerLevel);

        if (spacesPerLevel < 0) {
            stream << ' ';
        }

        ++enumIter;
    }

    --level;

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '}';
    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
    return stream;
}

// FREE OPERATORS
bool operator==(const bdem_Schema& lhs, const bdem_Schema& rhs)
{
    const int numRecords = lhs.numRecords();
    const int numEnums   = lhs.numEnumerations();

    if (numRecords != rhs.numRecords()
     || numEnums != rhs.numEnumerations()) {  // Do likely-to-fail test first.
        return false;                                                 // RETURN
    }

    if (&lhs == &rhs) {  // "Identically equal"; cheap but less likely.
        return true;                                                  // RETURN
    }

    // Compare the enumeration definitions first, because they are quicker.

    for (int ei = 0; ei < numEnums; ++ei) {
        const bdem_EnumerationDef& lhsEnum = lhs.enumeration(ei);
        const bdem_EnumerationDef& rhsEnum = rhs.enumeration(ei);

        const char *lhsEnumName = lhs.enumerationName(ei);
        const char *rhsEnumName = rhs.enumerationName(ei);

        if (lhsEnumName) {
            if (!rhsEnumName || bsl::strcmp(lhsEnumName, rhsEnumName)) {
                return false;                                         // RETURN
            }
        }
        else {
            if (rhsEnumName) {
                return false;                                         // RETURN
            }
        }

        if (!bdem_EnumerationDef::areEquivalent(lhsEnum, rhsEnum)) {
            return false;                                             // RETURN
        }
    }

    // Finally, compare the record definitions.

    for (int ri = 0; ri < numRecords; ++ri) {
        const bdem_RecordDef& lhsRecord = lhs.record(ri);
        const bdem_RecordDef& rhsRecord = rhs.record(ri);

        const int numFields = lhsRecord.numFields();

        if (numFields != rhsRecord.numFields()
         || lhsRecord.recordType() != rhsRecord.recordType()) {
            return false;                                             // RETURN
        }

        const char *lhsRecordName = lhs.recordName(lhsRecord.recordIndex());
        const char *rhsRecordName = rhs.recordName(rhsRecord.recordIndex());

        if (lhsRecordName) {
            if (!rhsRecordName || bsl::strcmp(lhsRecordName, rhsRecordName)) {
                return false;                                         // RETURN
            }
        }
        else {
            if (rhsRecordName) {
                return false;                                         // RETURN
            }
        }

        for (int fi = 0; fi < numFields; ++fi) {
            const bdem_FieldDef& lhsField   = lhsRecord.field(fi);
            const bdem_FieldDef& rhsField   = rhsRecord.field(fi);

            const int            lhsFieldId = lhsRecord.fieldId(fi);
            const int            rhsFieldId = rhsRecord.fieldId(fi);

            const bdem_ElemType::Type type = lhsField.elemType();

            if (type                      != rhsField.elemType()
             || lhsField.formattingMode() != rhsField.formattingMode()
             || lhsField.isNullable()     != rhsField.isNullable()
             || lhsFieldId                != rhsFieldId) {
                return false;                                         // RETURN
            }

            if (lhsField.hasDefaultValue() != rhsField.hasDefaultValue()
             || (lhsField.hasDefaultValue()
              && lhsField.defaultValue() != rhsField.defaultValue())) {
                return false;                                         // RETURN
            }

            const int lhsRecConstraintIdx = lhsField.recordConstraint()
                         ? lhsField.recordConstraint()->recordIndex()
                         : -1;
            const int rhsRecConstraintIdx = rhsField.recordConstraint()
                         ? rhsField.recordConstraint()->recordIndex()
                         : -1;

            const int lhsEnumConstraintIdx = lhsField.enumerationConstraint()
                         ? lhsField.enumerationConstraint()->enumerationIndex()
                         : -1;
            const int rhsEnumConstraintIdx = rhsField.enumerationConstraint()
                         ? rhsField.enumerationConstraint()->enumerationIndex()
                         : -1;

            if (lhsRecConstraintIdx  != rhsRecConstraintIdx
             || lhsEnumConstraintIdx != rhsEnumConstraintIdx) {
                return false;                                         // RETURN
            }

            const char *const lhsFieldName = lhsRecord.fieldName(fi);
            const char *const rhsFieldName = rhsRecord.fieldName(fi);

            if (lhsFieldName) {
                if (!rhsFieldName || bsl::strcmp(lhsFieldName, rhsFieldName)) {
                    return false;                                     // RETURN
                }
            }
            else {
                if (rhsFieldName) {
                    return false;                                     // RETURN
                }
            }
        }
    }

    return true;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
