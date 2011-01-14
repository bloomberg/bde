// bdem_recorddef.cpp                                                 -*-C++-*-
#include <bdem_recorddef.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_recorddef_cpp,"$Id$ $CSID$")

#include <bdem_enumerationdef.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bdema_sequentialallocator.h>
#include <bdeu_print.h>
#include <bdeu_string.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace {

                        // ==========================
                        // local class AutoEraseEntry
                        // ==========================

template <class Container>
class AutoEraseEntry {
    // This class provides a specialized proctor object that, upon destruction,
    // erases a managed entry from a managed container.

    typedef          Container           ManagedContainerType;
    typedef typename Container::iterator ManagedIteratorType;

    ManagedContainerType *d_container_p;  // managed container
    ManagedIteratorType   d_iter;         // iterator to managed entry

  public:
    AutoEraseEntry()
        // Create a proctor that initially manages no objects.
    : d_container_p(0)
    {
    }

    ~AutoEraseEntry()
        // If any object is under management by this proctor, then erase the
        // managed entry from the managed container; otherwise do nothing.
    {
        if (d_container_p) {
            d_container_p->erase(d_iter);
        }
    }

    void manage(ManagedContainerType *container, ManagedIteratorType iter)
        // Set this proctor to manage the specified 'container' and the entry
        // referenced by the specified 'iter'.  Any object currently under
        // management by this proctor is released from management.  The
        // behavior is undefined unless 'iter' is an iterator belonging to
        // 'container'.
    {
        d_container_p = container;
        d_iter        = iter;
    }

    void release()
        // Release from management any object managed by this proctor.
    {
        d_container_p = 0;
    }
};

                        // =========================
                        // local class AutoEraseName
                        // =========================

class AutoEraseName {
    // This class provides a specialized proctor object that, upon destruction,
    // erases a managed key from a managed map.

    typedef bsl::map<const char *,
                     int,
                     bdem_RecordDef_NameCompare> ManagedContainerType;

    ManagedContainerType *d_fieldNames_p;  // managed map
    const char           *d_name_p;        // managed key in map

  public:
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

}  // close unnamed namespace

                      // --------------------
                      // class bdem_RecordDef
                      // --------------------

// CONSTANTS
const int bdem_RecordDef::BDEM_NULL_FIELD_ID = INT_MIN;
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
const int bdem_RecordDef::NullFieldId = INT_MIN;
#endif

// PRIVATE MANIPULATORS
template <typename CONSTRAINT_TYPE>
bdem_FieldDef *
bdem_RecordDef::append(const bdem_FieldDefAttributes&  attributes,
                       const CONSTRAINT_TYPE          *constraint,
                       const char                     *name,
                       int                             id)
{
    if (constraint
     && &constraint->schema() != d_schema_p) {  // "alien" constraint
        return 0;                                                     // RETURN
    }

    if (constraint && !isValidConstraint(*constraint, attributes.elemType())) {
        return 0;                                                     // RETURN
    }

    // Note that 'lookupField' treats 'id' as an index into the vector of field
    // definitions if this record definition has no field ids.

    if ((name && lookupField(name))
     || (!d_fieldIds.empty() && lookupField(id))) {
        return 0;                                                     // RETURN
    }

    int fieldIndex = numFields();

    // Ensure that vector has sufficient capacity by pushing then popping a
    // single element.  This is better than calling 'reserve' because it
    // retains exponential growth.  After the 'pop_back', a 'push_back' is
    // guaranteed to succeed without throwing an exception.

    d_fieldDefs.push_back(0);
    d_fieldDefs.pop_back();

    char *fieldName = name ? bdeu_String::copy(name, writeOnceAllocator()) : 0;

    AutoEraseName nameProctor;

    if (fieldName) {
        d_fieldNames[fieldName] = fieldIndex;
        nameProctor.manage(&d_fieldNames, fieldName);
    }

    AutoEraseEntry<bsl::map<int, int> > idProctor;

    if (bdem_RecordDef::BDEM_NULL_FIELD_ID != id) {
        // Insert 'fieldIndex' into 'd_fieldIds' with key 'id' and proctor the
        // results in case of a future exception.

        idProctor.manage(
            &d_fieldIds,
            d_fieldIds.insert(bsl::make_pair(id, fieldIndex)).first);
    }

    bdem_FieldDef *pF =
        (bdem_FieldDef*)
                writeOnceAllocator()->allocate(sizeof(bdem_FieldDef));

    new (pF) bdem_FieldDef(fieldName,
                           id,
                           attributes,
                           writeOnceAllocator());

    pF->setConstraint(constraint);
    d_fieldDefs.push_back(pF);  // Cannot throw since called 'reserve'.

    idProctor.release();
    nameProctor.release();

    return pF;
}

// CLASS METHODS
bool bdem_RecordDef::isValidConstraint(const bdem_RecordDef& constraint,
                                       bdem_ElemType::Type   type)
{
    switch (type) {
      case bdem_ElemType::BDEM_LIST:
      case bdem_ElemType::BDEM_TABLE: {
        return BDEM_SEQUENCE_RECORD == constraint.recordType();
      } break;
      case bdem_ElemType::BDEM_CHOICE:
      case bdem_ElemType::BDEM_CHOICE_ARRAY: {
        return BDEM_CHOICE_RECORD == constraint.recordType();
      } break;
      default: {
      } break;
    }
    return false;
}

// CREATORS
bdem_RecordDef::bdem_RecordDef(const bdem_Schema         *schema,
                               int                        index,
                               const char                *name,
                               RecordType                 recordType,
                               bdema_SequentialAllocator *writeOnceAllocator,
                               bslma_Allocator            *basicAllocator)
: d_schema_p(schema)
, d_recordIndex(index)
, d_recordName_p(name)  // held, not owned
, d_recordType(recordType)
, d_fieldDefs(basicAllocator)
, d_fieldNames(bdem_RecordDef_NameCompare(), writeOnceAllocator)
, d_fieldIds(bsl::less<int>(), writeOnceAllocator)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(BDEM_SEQUENCE_RECORD == recordType
             || BDEM_CHOICE_RECORD   == recordType);
}

bdem_RecordDef::~bdem_RecordDef()
{
    const int n = d_fieldDefs.size();

    // Destroy the sequence of field definitions.  Note that, while
    // 'd_fieldDefs' uses a standard allocator, the addresses of the
    // 'bdem_FieldDef' objects it contains were allocated using a write-once
    // allocator (see 'append'), so they don't need to be deallocated.

    for (int i = 0; i < n; ++i) {
        d_fieldDefs[i]->~bdem_FieldDef();
    }
}

// MANIPULATORS
const bdem_FieldDef *bdem_RecordDef::appendField(
                                              const bdem_FieldSpec&  fieldSpec,
                                              const char            *name,
                                              int                    id)
{
    if (fieldSpec.enumerationConstraint()) {
        return appendField(fieldSpec.fieldAttributes(),
                           fieldSpec.enumerationConstraint(),
                           name,
                           id);
    }
    return appendField(fieldSpec.fieldAttributes(),
                       fieldSpec.recordConstraint(),
                       name,
                       id);
}

const bdem_FieldDef *
bdem_RecordDef::appendField(const bdem_FieldDefAttributes&  attributes,
                            const bdem_RecordDef           *constraint,
                            const char                     *name,
                            int                             id)
{
    bdem_FieldDef *field = append(attributes, constraint, name, id);

    // By default, 'LIST' fields have their nullability attribute set to
    // 'false'.  This *must* be toggled to 'true' if the 'LIST' is constrained
    // by the *same* record that contains the field (direct recursion).

    if (0 != field && bdem_ElemType::BDEM_LIST == attributes.elemType()
                   && constraint
                   && recordIndex() == constraint->recordIndex()) {
        field->setIsNullable(true);
    }

    return field;
}

const bdem_FieldDef *
bdem_RecordDef::appendField(const bdem_FieldDefAttributes&  attributes,
                            LowPrecedenceEnumPtr            constraint,
                            const char                     *name,
                            int                             id)
{
    return append(attributes, constraint.d_ptr, name, id);
}

// ACCESSORS
int bdem_RecordDef::fieldIndexExtended(const char *name) const
{
    int ret = fieldIndex(name);
    if (ret >= 0 || 0 == numAnonymousFields()) {
        return ret;                                                   // RETURN
    }

    // Traverse record definitions, looking at anonymous fields.

    bsl::vector<bdem_FieldDef*>::const_iterator fldIter = d_fieldDefs.begin();
    while (fldIter != d_fieldDefs.end()) {
        if (0 == (*fldIter)->fieldName()) {
            // Recursively look for 'name' in anonymous field's constraint.

            const bdem_RecordDef *constraint = (*fldIter)->recordConstraint();

            if (constraint && constraint->fieldIndexExtended(name) >= 0) {
                // Found 'name' recursively within an anonymous field's
                // constraining record.  Return the index of that anonymous
                // field definition within this record definition.

                return fldIter - d_fieldDefs.begin();                 // RETURN
            }
        }
        ++fldIter;
    }

    return -1;  // field not found
}

bsl::ostream& bdem_RecordDef::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << (BDEM_CHOICE_RECORD == d_recordType ? "CHOICE" : "SEQUENCE")
           << " RECORD ";

    if (d_recordName_p) {
        stream << '\"' << d_recordName_p << "\" ";
    }

    if (level < 0) {
        level = -level;
    }

    stream << '{';

    const int numFields = d_fieldDefs.size();

    for (int i = 0; i < numFields; ++i) {
        const bdem_FieldDef *fieldDef  = d_fieldDefs[i];
        const bdem_ElemType::Type fieldType = fieldDef->elemType();

        bdeu_Print::newlineAndIndent(stream, level + 1, spacesPerLevel);
        stream << bdem_ElemType::toAscii(fieldType);

        const bdem_RecordDef      *recConstraint  =
                                             fieldDef->recordConstraint();
        const bdem_EnumerationDef *enumConstraint =
                                             fieldDef->enumerationConstraint();
        if (recConstraint) {
            BSLS_ASSERT(bdem_ElemType::isAggregateType(fieldType));

            const char *recordName = recConstraint->recordName();
            if (recordName) {
                stream << "<\"" << recordName << "\">";
            }
            else {
                stream << '[' << recConstraint->recordIndex() << ']';
            }
        }
        else if (enumConstraint) {
            BSLS_ASSERT(
               bdem_EnumerationDef::canHaveEnumConstraint(fieldType));

            stream << " ENUM";
            const char *enumName = enumConstraint->enumerationName();
            if (enumName) {
                stream << "<\"" << enumName << "\">";
            }
            else {
                stream << '[' << enumConstraint->enumerationIndex() << ']';
            }
        }

        const char *fieldName = fieldDef->fieldName();

        if (fieldName) {
            stream << " \"" << fieldName << '\"';
        }

        const int fieldId = hasFieldIds()
                          ? fieldDef->fieldId()
                          : bdem_RecordDef::BDEM_NULL_FIELD_ID;

        if (bdem_RecordDef::BDEM_NULL_FIELD_ID != fieldId) {
            stream << (fieldName ? ", " : " ") << fieldId;
        }

        stream << (fieldDef->isNullable() ? " { " : " { !") << "nullable";

        stream << " 0x" << bsl::hex << fieldDef->formattingMode() << bsl::dec;

        if (fieldDef->hasDefaultValue()) {
            BSLS_ASSERT(!bdem_ElemType::isAggregateType(fieldType));
            stream << ' ';
            fieldDef->defaultValue().print(stream, 0, -1);
        }

        stream << " }";
    }

    bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);

    stream << '}';

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
