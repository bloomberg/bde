// bdlmxxx_schemautil.cpp                                                -*-C++-*-
#include <bdlmxxx_schemautil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_schemautil_cpp,"$Id$ $CSID$")

#include <bdlmxxx_elemref.h>      // for testing only
#include <bdlmxxx_elemtype.h>
#include <bdlmxxx_schema.h>
#include <bdlcxxx_bitarray.h>

#include <bsls_assert.h>

#include <bsl_cstring.h>

namespace BloombergLP {

namespace {

                        // ----------------------
                        // local class BoolMatrix
                        // ----------------------

class BoolMatrix {
    // This class represents a two dimensional boolean matrix.  The class is
    // used as a recursion guard to protect against infinite recursion when
    // testing relationships between two record definitions.  Note that
    // 'BoolMatrix' is used only within the scope of a method, therefore, the
    // default behavior of using the currently installed default allocator
    // is desired.

    // DATA
    bdlcxxx::BitArray     d_array;      // 2-D matrix of flags, expressed linearly
    int               d_rowLength;  // length of each row

  public:
    // CREATORS
    BoolMatrix(int               numRows,
               int               numColumns,
               bslma::Allocator *basicAllocator = 0)
    : d_array(numRows * numColumns, false, basicAllocator)
    , d_rowLength(numColumns)
        // Create a 'BoolMatrix' with the specified 'numRows' and the specified
        // 'numColumns'.  Optionally specify 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.  Initialize all the booleans in the matrix to 'false'.
    {
    }

    // MANIPULATORS
    void set(int rowIndex, int colIndex)
        // Set the bit in this matrix at the specified 'rowIndex' and
        // 'colIndex' to 'true'.
    {
        d_array.set1(d_rowLength * rowIndex + colIndex, 1);
    }

    // ACCESSORS
    bool get(int rowIndex, int colIndex) const
        // Return 'true' if the bit in this matrix at the specified 'rowIndex'
        // and 'colIndex' is set, and 'false' otherwise.
    {
        return d_array[d_rowLength * rowIndex + colIndex];
    }
};

}  // close unnamed namespace

// STATIC HELPER FUNCTIONS
static
bool areEquivalentImp(const bdlmxxx::RecordDef& rd1,
                      const bdlmxxx::RecordDef& rd2,
                      BoolMatrix&           recursionGuard)
    // Return 'true' if the specified 'rd1' and 'rd2' record definitions are
    // equivalent, and 'false' otherwise.  Use the specified 'recursionGuard'
    // to guard against infinite recursion of record definition.  Note that the
    // meaning of equivalent is the same as defined in
    // 'bdlmxxx::SchemaUtil::areEquivalent'.
{
    // First check whether we have been here before.

    if (recursionGuard.get(rd1.recordIndex(), rd2.recordIndex())) {
        return true;                                                  // RETURN
    }

    recursionGuard.set(rd1.recordIndex(), rd2.recordIndex());

    if (rd2.recordType() != rd1.recordType()) {
        return false;                                                 // RETURN
    }

    const int rd2Length = rd2.numFields();

    if (rd2Length != rd1.numFields()) {  // cheap and somewhat likely
        return false;                                                 // RETURN
    }

    if (&rd2 == &rd1) {  // also cheap, but less likely
        return true;                                                  // RETURN
    }

    for (int fieldIdx = 0; fieldIdx < rd2Length; ++fieldIdx) {
        if (rd2.fieldId(fieldIdx) != rd1.fieldId(fieldIdx)) {
            return false;                                             // RETURN
        }

        const char *rd1FieldName = rd1.fieldName(fieldIdx);
        const char *rd2FieldName = rd2.fieldName(fieldIdx);

        if (rd2FieldName) {
            if (!rd1FieldName
             || 0 != bsl::strcmp(rd2FieldName, rd1FieldName)) {
                return false;                                         // RETURN
            }
        }
        else {
            if (rd1FieldName) {
                return false;                                         // RETURN
            }
        }

        const bdlmxxx::FieldDef& rd1Field = rd1.field(fieldIdx);
        const bdlmxxx::FieldDef& rd2Field = rd2.field(fieldIdx);

        const bdlmxxx::ElemType::Type rd2ElemType = rd2Field.elemType();

        if (rd2ElemType               != rd1Field.elemType()
         || rd2Field.formattingMode() != rd1Field.formattingMode()
         || rd2Field.isNullable()     != rd1Field.isNullable()) {
            return false;                                             // RETURN
        }

        bool rd2HasDefault = rd2Field.hasDefaultValue();

        if (rd2HasDefault != rd1Field.hasDefaultValue()
         || (rd2HasDefault
             && rd2Field.defaultValue() != rd1Field.defaultValue())) {
            return false;                                             // RETURN
        }

        const bdlmxxx::RecordDef *const rd2Constraint   =
                                                   rd2Field.recordConstraint();
        const bdlmxxx::RecordDef *const rd1Constraint =
                                                 rd1Field.recordConstraint();

        if (rd2Constraint) {
            if (!rd1Constraint) {
                return false;                                         // RETURN
            }

            // Imp Note: If both 'rd1' and 'rd2' are self-constrained, it is
            // OK to skip this test and let the rest of the method resolve the
            // issue of the "constraints" being equivalent

            if ((rd1Constraint != &rd1 && rd2Constraint != &rd2)
             && 0 == areEquivalentImp(*rd1Constraint,
                                      *rd2Constraint,
                                      recursionGuard)) {
                return false;                                         // RETURN
            }
        }
        else {
            if (rd1Constraint) {
                return false;                                         // RETURN
            }
        }

        const bdlmxxx::EnumerationDef *const rd2Enumeration =
                                              rd2Field.enumerationConstraint();
        const bdlmxxx::EnumerationDef *const rd1Enumeration =
                                            rd1Field.enumerationConstraint();

        if (rd2Enumeration) {
            if (!rd1Enumeration
             || 0 == bdlmxxx::SchemaUtil::areEquivalent(*rd1Enumeration,
                                                    *rd2Enumeration)) {
                return false;                                         // RETURN
            }
        }
        else {
            if (rd1Enumeration) {
                return false;                                         // RETURN
            }
        }
    }
    return true;
}

static
bool areStructurallyEquivalentImp(const bdlmxxx::RecordDef& rd1,
                                  const bdlmxxx::RecordDef& rd2,
                                  int                   attributeMask,
                                  BoolMatrix&           recursionGuard)
    // Return 'true' if the specified 'rd1' and 'rd2' record definitions are
    // structurally equivalent, and 'false' otherwise.  Use the specified
    // 'recursionGuard' to guard against infinite recursion of record
    // definition.  Note that the meaning of structurally equivalent is the
    // same as defined in 'bdlmxxx::SchemaUtil::areStructurallyEquivalent'.
{
    // First check whether we have been here before.

    if (recursionGuard.get(rd1.recordIndex(), rd2.recordIndex())) {
        return true;                                                  // RETURN
    }

    recursionGuard.set(rd1.recordIndex(), rd2.recordIndex());

    if (rd2.recordType() != rd1.recordType()) {
        return false;                                                 // RETURN
    }

    const int rd2Length = rd2.numFields();

    if (rd2Length != rd1.numFields()) {  // cheap and somewhat likely
        return false;                                                 // RETURN
    }

    if (&rd2 == &rd1) {  // also cheap, but less likely
        return true;                                                  // RETURN
    }

    for (int fieldIdx = 0; fieldIdx < rd2Length; ++fieldIdx) {
        const bdlmxxx::FieldDef& rd1Field = rd1.field(fieldIdx);
        const bdlmxxx::FieldDef& rd2Field = rd2.field(fieldIdx);

        const bdlmxxx::ElemType::Type rd2ElemType = rd2Field.elemType();

        if (rd2ElemType           != rd1Field.elemType()
         || rd2Field.isNullable() != rd1Field.isNullable()) {
            return false;                                             // RETURN
        }

        if ((bdlmxxx::SchemaUtil::BDEM_FORMATTING_MODE & attributeMask)
         && rd2Field.formattingMode() != rd1Field.formattingMode()) {
            return false;                                             // RETURN
        }

        if (bdlmxxx::SchemaUtil::BDEM_DEFAULT_VALUE & attributeMask) {
            bool rd2HasDefault = rd2Field.hasDefaultValue();

            if (rd2HasDefault != rd1Field.hasDefaultValue()
             || (rd2HasDefault
                 && rd2Field.defaultValue() != rd1Field.defaultValue())) {
                return false;                                         // RETURN
            }
        }

        if (bdlmxxx::SchemaUtil::BDEM_CHECK_ENUMERATION & attributeMask) {
            const bdlmxxx::EnumerationDef *const rd1Enumeration =
                                              rd1Field.enumerationConstraint();
            const bdlmxxx::EnumerationDef *const rd2Enumeration =
                                              rd2Field.enumerationConstraint();

            if (rd2Enumeration) {
                if (!rd1Enumeration
                 || 0 == bdlmxxx::SchemaUtil::areEquivalent(*rd1Enumeration,
                                                        *rd2Enumeration)) {
                        return false;                                 // RETURN
                }
            }
            else {
                if (rd1Enumeration) {
                    return false;                                     // RETURN
                }
            }
        }

        const bdlmxxx::RecordDef *const rd1Constraint =
                                                   rd1Field.recordConstraint();
        const bdlmxxx::RecordDef *const rd2Constraint =
                                                   rd2Field.recordConstraint();

        if (rd2Constraint) {
            if (!rd1Constraint) {
                return false;                                         // RETURN
            }

            // Imp Note: If both 'rd1' and 'rd2' are self-constrained, it is
            // OK to skip this test and let the rest of the method resolve the
            // issue of the "constraints" being equivalent

            if ((rd1Constraint != &rd1 && rd2Constraint != &rd2)
             && 0 == areStructurallyEquivalentImp(*rd1Constraint,
                                                  *rd2Constraint,
                                                  attributeMask,
                                                  recursionGuard)) {
                return false;                                         // RETURN
            }
        }
        else {
            if (rd1Constraint) {
                return false;                                         // RETURN
            }
        }
    }
    return true;
}

static
bool isSupersetImp(const bdlmxxx::RecordDef& super,
                   const bdlmxxx::RecordDef& sub,
                   BoolMatrix&           recursionGuard)
    // Return 'true' if the specified 'super' record definition is a superset
    // of the specified 'sub' record definition, and 'false' otherwise.  Use
    // the specified 'recursionGuard' to guard against infinite recursion of
    // record definition.  Note that the meaning of superset is the same as
    // defined in 'bdlmxxx::SchemaUtil::isSuperset'.
{
    // First check whether we have been here before.

    if (recursionGuard.get(super.recordIndex(), sub.recordIndex())) {
        return true;                                                  // RETURN
    }

    recursionGuard.set(super.recordIndex(), sub.recordIndex());

    if (sub.recordType() != super.recordType()) {
        return false;                                                 // RETURN
    }

    const int subLength = sub.numFields();

    if (subLength > super.numFields()) {  // cheap and somewhat likely
        return false;                                                 // RETURN
    }

    if (&sub == &super) {  // also cheap, but less likely
        return true;                                                  // RETURN
    }

    for (int fieldIdx = 0; fieldIdx < subLength; ++fieldIdx) {
        if (sub.fieldId(fieldIdx) != super.fieldId(fieldIdx)) {
            return false;                                             // RETURN
        }

        const char *superFieldName = super.fieldName(fieldIdx);
        const char *subFieldName   = sub.fieldName(fieldIdx);

        if (subFieldName) {
            if (!superFieldName
             || 0 != bsl::strcmp(subFieldName, superFieldName)) {
                return false;                                         // RETURN
            }
        }
        else {
            if (superFieldName) {
                return false;                                         // RETURN
            }
        }

        const bdlmxxx::FieldDef& superField = super.field(fieldIdx);
        const bdlmxxx::FieldDef& subField   = sub.field(fieldIdx);

        const bdlmxxx::ElemType::Type subElemType = subField.elemType();

        if (subElemType               != superField.elemType()
         || subField.formattingMode() != superField.formattingMode()
         || subField.isNullable()     != superField.isNullable()) {
            return false;                                             // RETURN
        }

        bool subHasDefault = subField.hasDefaultValue();

        if (subHasDefault != superField.hasDefaultValue()
         || (subHasDefault
             && subField.defaultValue() != superField.defaultValue())) {
            return false;                                             // RETURN
        }

        const bdlmxxx::RecordDef *const subConstraint =
                                                   subField.recordConstraint();

        if (subConstraint) {
            const bdlmxxx::RecordDef *const superConstraint =
                                                 superField.recordConstraint();

            if (!superConstraint) {
                return false;                                         // RETURN
            }

            // Imp Note: If both 'super' and 'sub' are self-constrained, it is
            // OK to skip this test and let the rest of the method resolve the
            // issue of the "constraints" being super/sub-sets.

            if ((superConstraint != &super && subConstraint != &sub)
             && 0 == isSupersetImp(*superConstraint,
                                   *subConstraint,
                                   recursionGuard)) {
                return false;                                         // RETURN
            }
        }

        const bdlmxxx::EnumerationDef *const subEnumeration =
                                              subField.enumerationConstraint();

        if (subEnumeration) {
            const bdlmxxx::EnumerationDef *const superEnumeration =
                                            superField.enumerationConstraint();

            if (!superEnumeration
             || 0 == bdlmxxx::SchemaUtil::isSuperset(*superEnumeration,
                                                 *subEnumeration)) {
                return false;                                         // RETURN
            }
        }
    }
    return true;
}

static
bool isStructuralSupersetImp(const bdlmxxx::RecordDef& super,
                             const bdlmxxx::RecordDef& sub,
                             int                   attributeMask,
                             BoolMatrix&           recursionGuard)
    // Return 'true' if the specified 'super' record definition is a structural
    // superset of the specified 'sub' record definition, and 'false'
    // otherwise.  Use the specified 'attributeMask' to indicate that
    // corresponding fields in 'super' and 'sub' must also have equivalent
    // default values and formatting modes.  Use the specified 'recursionGuard'
    // to guard against infinite recursion of record definition.  Note that the
    // meaning of structural superset is the same as defined in
    // 'bdlmxxx::SchemaUtil::isStructuralSuperset'.
{
    // First check whether we have been here before.

    if (recursionGuard.get(super.recordIndex(), sub.recordIndex())) {
        return true;                                                  // RETURN
    }

    recursionGuard.set(super.recordIndex(), sub.recordIndex());

    if (sub.recordType() != super.recordType()) {
        return false;                                                 // RETURN
    }

    const int subLength = sub.numFields();

    if (subLength > super.numFields()) {  // cheap and somewhat likely
        return false;                                                 // RETURN
    }

    if (&sub == &super) {  // also cheap, but less likely
        return true;                                                  // RETURN
    }

    for (int fieldIdx = 0; fieldIdx < subLength; ++fieldIdx) {
        const bdlmxxx::FieldDef& superField = super.field(fieldIdx);
        const bdlmxxx::FieldDef& subField   = sub.field(fieldIdx);

        const bdlmxxx::ElemType::Type subElemType = subField.elemType();

        if (subElemType           != superField.elemType()
         || subField.isNullable() != superField.isNullable()) {
            return false;                                             // RETURN
        }

        if ((bdlmxxx::SchemaUtil::BDEM_FORMATTING_MODE & attributeMask)
         && subField.formattingMode() != superField.formattingMode()) {
            return false;                                             // RETURN
        }

        if (bdlmxxx::SchemaUtil::BDEM_DEFAULT_VALUE & attributeMask) {
            bool subHasDefault = subField.hasDefaultValue();

            if (subHasDefault != superField.hasDefaultValue()
             || (subHasDefault
                 && subField.defaultValue() != superField.defaultValue())) {
                return false;                                         // RETURN
            }
        }

        if (bdlmxxx::SchemaUtil::BDEM_CHECK_ENUMERATION & attributeMask) {
            const bdlmxxx::EnumerationDef *const subEnumeration =
                                              subField.enumerationConstraint();

            if (subEnumeration) {
                const bdlmxxx::EnumerationDef *const superEnumeration =
                                            superField.enumerationConstraint();

                if (!superEnumeration
                 || 0 == bdlmxxx::SchemaUtil::isSuperset(*superEnumeration,
                                                     *subEnumeration)) {
                    return false;                                     // RETURN
                }
            }
        }

        const bdlmxxx::RecordDef *const superConstraint =
                                                 superField.recordConstraint();
        const bdlmxxx::RecordDef *const subConstraint   =
                                                   subField.recordConstraint();

        if (subConstraint) {
            if (!superConstraint) {
                return false;                                         // RETURN
            }

            // Imp Note: If both 'super' and 'sub' are self-constrained, it is
            // OK to skip this test and let the rest of the method resolve the
            // issue of the "constraints" being super/sub-sets.

            if ((superConstraint != &super && subConstraint != &sub)
             && 0 == isStructuralSupersetImp(*superConstraint,
                                             *subConstraint,
                                             attributeMask,
                                             recursionGuard)) {
                return false;                                         // RETURN
            }
        }
    }
    return true;
}

static
bool isSymbolicSupersetImp(const bdlmxxx::RecordDef& super,
                           const bdlmxxx::RecordDef& sub,
                           int                   attributeMask,
                           BoolMatrix&           recursionGuard)
    // Return 'true' if the specified 'super' record definition is a symbolic
    // superset of the specified 'sub' record definition, and 'false'
    // otherwise.  Use the specified 'attributeMask' to indicate that
    // corresponding fields in 'super' and 'sub' must also have equivalent
    // default values and formatting modes.  Use the specified 'recursionGuard'
    // to guard against infinite recursion of record definition.  Note that the
    // meaning of symbolic superset is the same as defined in
    // 'bdlmxxx::SchemaUtil::isSymbolicSuperset'.
{
    // First check whether we have been here before.

    if (recursionGuard.get(super.recordIndex(), sub.recordIndex())) {
        return true;                                                  // RETURN
    }

    recursionGuard.set(super.recordIndex(), sub.recordIndex());

    if (sub.recordType() != super.recordType()) {
        return false;                                                 // RETURN
    }

    const int subLength = sub.numFields();

    for (int fieldIdx = 0; fieldIdx < subLength; ++fieldIdx) {
        const bdlmxxx::FieldDef& subField = sub.field(fieldIdx);

        const char *subFieldName = sub.fieldName(fieldIdx);
        const int subFieldId = sub.hasFieldIds()
                               ? sub.fieldId(fieldIdx)
                               : bdlmxxx::RecordDef::BDEM_NULL_FIELD_ID;

        if (!subFieldName
         && bdlmxxx::RecordDef::BDEM_NULL_FIELD_ID == subFieldId) {
            // No name or id.

            continue;
        }

        int superIndex = -1;

        if (subFieldName) {  // has name, may also have id
            superIndex = super.fieldIndex(subFieldName);

            if (superIndex < 0) {  // no corresponding field in super
                return false;                                         // RETURN
            }

            const int superFieldId = super.hasFieldIds()
                                   ? super.fieldId(superIndex)
                                   : bdlmxxx::RecordDef::BDEM_NULL_FIELD_ID;

            if (subFieldId != superFieldId) {  // ids don't match
                return false;                                         // RETURN
            }
        }
        else {  // no name, has id
            superIndex = super.fieldIndex(subFieldId);

            // 'super' must have an unnamed field with the same (non-default)
            // id.

            if (superIndex < 0 || !super.hasFieldIds()
             || super.fieldName(superIndex)) {
                return false;                                         // RETURN
            }
        }

        // At this point, 'subField' and 'superField' are consistent with
        // respect to field names and ids.

        BSLS_ASSERT(0 <= superIndex);

        const bdlmxxx::FieldDef& superField = super.field(superIndex);

        const bdlmxxx::ElemType::Type subElemType = subField.elemType();

        if (subElemType           != superField.elemType()
         || subField.isNullable() != superField.isNullable()) {
            return false;                                             // RETURN
        }

        if ((bdlmxxx::SchemaUtil::BDEM_FORMATTING_MODE & attributeMask)
         && subField.formattingMode() != superField.formattingMode()) {
            return false;                                             // RETURN
        }

        if (bdlmxxx::SchemaUtil::BDEM_DEFAULT_VALUE & attributeMask) {
            bool subHasDefault = subField.hasDefaultValue();

            if (subHasDefault != superField.hasDefaultValue()
             || (subHasDefault
                 && subField.defaultValue() != superField.defaultValue())) {
                return false;                                         // RETURN
            }
        }

        if (bdlmxxx::SchemaUtil::BDEM_CHECK_ENUMERATION & attributeMask) {
            const bdlmxxx::EnumerationDef *const subEnumeration =
                                              subField.enumerationConstraint();

            if (subEnumeration) {
                const bdlmxxx::EnumerationDef *const superEnumeration =
                                            superField.enumerationConstraint();

                if (!superEnumeration
                 || 0 == bdlmxxx::SchemaUtil::isSuperset(*superEnumeration,
                                                     *subEnumeration)) {
                  return false;                                       // RETURN
                }
            }
        }

        const bdlmxxx::RecordDef *superConstraint = superField.recordConstraint();
        const bdlmxxx::RecordDef *subConstraint   = subField.recordConstraint();

        if (subConstraint) {
            if (!superConstraint) {
                return false;                                         // RETURN
            }

            // Imp Note: If both 'super' and 'sub' are self-constrained, it is
            // OK to skip this test and let the rest of the method resolve the
            // issue of the "constraints" being super/sub-sets.

            if ((superConstraint != &super && subConstraint != &sub)
             && 0 == isSymbolicSupersetImp(*superConstraint,
                                           *subConstraint,
                                           attributeMask,
                                           recursionGuard)) {
                return false;                                         // RETURN
            }
        }
    }

    return true;
}

namespace bdlmxxx {
                            // ---------------------
                            // class SchemaUtil
                            // ---------------------

// CLASS METHODS
bool SchemaUtil::areEquivalent(const EnumerationDef& e1,
                                    const EnumerationDef& e2)
{
    return EnumerationDef::areEquivalent(e1, e2);
}

bool SchemaUtil::areEquivalent(const RecordDef& r1,
                                    const RecordDef& r2)
{
    const int len1 = r1.schema().numRecords();
    const int len2 = r2.schema().numRecords();

    BoolMatrix recursionGuard(len1, len2);

    return areEquivalentImp(r1, r2, recursionGuard);
}

bool SchemaUtil::areStructurallyEquivalent(const RecordDef& r1,
                                                const RecordDef& r2)
{
    const int len1 = r1.schema().numRecords();
    const int len2 = r2.schema().numRecords();

    BoolMatrix recursionGuard(len1, len2);

    return areStructurallyEquivalentImp(r1, r2, 0, recursionGuard);
}

bool SchemaUtil::areStructurallyEquivalent(
                                           const RecordDef& r1,
                                           const RecordDef& r2,
                                           int                   attributeMask)
{
    const int len1 = r1.schema().numRecords();
    const int len2 = r2.schema().numRecords();

    BoolMatrix recursionGuard(len1, len2);

    return areStructurallyEquivalentImp(r1, r2, attributeMask, recursionGuard);
}

bool SchemaUtil::areSymbolicallyEquivalent(const RecordDef& r1,
                                                const RecordDef& r2)
{
    // Imp Note: This imp choice is quicker to develop and less error-prone,
    // but also less efficient.

    return isSymbolicSuperset(r1, r2) && isSymbolicSuperset(r2, r1);
}

bool SchemaUtil::areSymbolicallyEquivalent(
                                           const RecordDef& r1,
                                           const RecordDef& r2,
                                           int                   attributeMask)
{
    // Imp Note: This imp choice is quicker to develop and less error-prone,
    // but also less efficient.

    return isSymbolicSuperset(r1, r2, attributeMask)
        && isSymbolicSuperset(r2, r1, attributeMask);
}

bool SchemaUtil::isSuperset(const EnumerationDef& super,
                                 const EnumerationDef& sub)
{
    return EnumerationDef::isSuperset(super, sub);
}

bool SchemaUtil::isSuperset(const RecordDef& super,
                                 const RecordDef& sub)
{
    const int superLen = super.schema().numRecords();
    const int subLen   = sub.schema().numRecords();

    BoolMatrix recursionGuard(superLen, subLen);

    return isSupersetImp(super, sub, recursionGuard);
}

bool SchemaUtil::isSuperset(const Schema& super,
                                 const Schema& sub)
{
    const int subNumRecords = sub.numRecords();

    if (subNumRecords > super.numRecords()) {  // cheap and somewhat likely
        return false;                                                 // RETURN
    }

    const int subNumEnumerations = sub.numEnumerations();

    if (subNumEnumerations > super.numEnumerations()) {

        // Cheap and somewhat likely.

        return false;                                                 // RETURN
    }

    if (&super == &sub) {  // also cheap, but less likely
        return true;                                                  // RETURN
    }

    for (int i = 0; i < subNumEnumerations; ++i) {
        const char *subEnumerationName   = sub.enumerationName(i);
        const char *superEnumerationName = super.enumerationName(i);

        if (subEnumerationName) {
            if (!superEnumerationName
              || 0 != bsl::strcmp(subEnumerationName, superEnumerationName)) {
                return false;                                         // RETURN
            }
        }
        else {
            if (superEnumerationName) {
                return false;                                         // RETURN
            }
        }

        const EnumerationDef& superEnumeration = super.enumeration(i);
        const EnumerationDef& subEnumeration   = sub.enumeration(i);

        SchemaUtil::isSuperset(superEnumeration, subEnumeration);
    }

    for (int i = 0; i < subNumRecords; ++i) {
        const RecordDef& subRecord   = sub.record(i);
        const RecordDef& superRecord = super.record(i);

        if (subRecord.recordType() != superRecord.recordType()) {
            return false;                                             // RETURN
        }

        const int subRecordLength = subRecord.numFields();

        if (subRecordLength > superRecord.numFields()) {
            return false;                                             // RETURN
        }

        const char *superRecordName = super.recordName(i);
        const char *subRecordName   = sub.recordName(i);

        if (subRecordName) {
            if (!superRecordName
             || 0 != bsl::strcmp(subRecordName, superRecordName)) {
                return false;                                         // RETURN
            }
        }
        else {
            if (superRecordName) {
                return false;                                         // RETURN
            }
        }

        for (int j = 0; j < subRecordLength; ++j) {
            if (subRecord.fieldId(j) != superRecord.fieldId(j)) {
                return false;                                         // RETURN
            }

            const char *superFieldName = superRecord.fieldName(j);
            const char *subFieldName = subRecord.fieldName(j);

            if (subFieldName) {
                if (!superFieldName
                 || 0 != bsl::strcmp(subFieldName, superFieldName)) {
                    return false;                                     // RETURN
                }
            }
            else {
                if (superFieldName) {
                    return false;                                     // RETURN
                }
            }

            const FieldDef& superField = superRecord.field(j);
            const FieldDef& subField   = subRecord.field(j);

            const ElemType::Type subElemType = subField.elemType();

            if (subElemType               != superField.elemType()
             || subField.formattingMode() != superField.formattingMode()
             || subField.isNullable()     != superField.isNullable()) {
                return false;                                         // RETURN
            }

            bool subHasDefault = subField.hasDefaultValue();

            if (subHasDefault != superField.hasDefaultValue()
             || (subHasDefault
                 && subField.defaultValue() != superField.defaultValue())) {
                return false;                                         // RETURN
            }

            const RecordDef *const superConstraint =
                                                 superField.recordConstraint();
            const RecordDef *const subConstraint   =
                                                   subField.recordConstraint();

            if (subConstraint) {
                if (!superConstraint
                 || subConstraint->recordIndex()
                                           != superConstraint->recordIndex()) {
                    return false;                                     // RETURN
                }
            }
            else {
                if (superConstraint) {
                    return false;                                     // RETURN
                }
            }
        }
    }

    return true;
}

bool SchemaUtil::isStructuralSuperset(const RecordDef& super,
                                           const RecordDef& sub)
{
    const int superLen = super.schema().numRecords();
    const int subLen   = sub.schema().numRecords();

    BoolMatrix recursionGuard(superLen, subLen);

    return isStructuralSupersetImp(super, sub, 0, recursionGuard);
}

bool SchemaUtil::isStructuralSuperset(const RecordDef& super,
                                           const RecordDef& sub,
                                           int                   attributeMask)
{
    const int superLen = super.schema().numRecords();
    const int subLen   = sub.schema().numRecords();

    BoolMatrix recursionGuard(superLen, subLen);

    return isStructuralSupersetImp(super, sub, attributeMask, recursionGuard);
}

bool SchemaUtil::isSymbolicSuperset(const RecordDef& super,
                                         const RecordDef& sub)
{
    const int superLen = super.schema().numRecords();
    const int subLen   = sub.schema().numRecords();

    BoolMatrix recursionGuard(superLen, subLen);

    return isSymbolicSupersetImp(super, sub, 0, recursionGuard);
}

bool SchemaUtil::isSymbolicSuperset(const RecordDef& super,
                                         const RecordDef& sub,
                                         int                   attributeMask)
{
    const int superLen = super.schema().numRecords();
    const int subLen   = sub.schema().numRecords();

    BoolMatrix recursionGuard(superLen, subLen);

    return isSymbolicSupersetImp(super, sub, attributeMask, recursionGuard);
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
