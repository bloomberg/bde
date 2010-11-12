// bdem_binding.cpp                                                   -*-C++-*-
#include <bdem_binding.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_binding_cpp,"$Id$ $CSID$")

#include <bdet_datetime.h>              // for testing only
#include <bdet_datetimetz.h>            // for testing only
#include <bdet_date.h>                  // for testing only
#include <bdet_datetz.h>                // for testing only
#include <bdet_time.h>                  // for testing only
#include <bdet_timetz.h>                // for testing only

#include <bdetu_unset.h>

#include <bsls_assert.h>

///IMPLEMENTATION NOTE
///-------------------
// The nullness of the relevant element need not be explicitly examined in the
// 'enumerationAsInt' and 'enumerationAsString' methods because the correct
// result is returned whether or not the 'INT' or 'STRING' element is null or
// simply has the "unset" value.  Note that per 'bdem_schema', the "unset"
// 'STRING' ("") is not a valid enumerator name, and the "unset" 'INT'
// (INT_MIN) is not a valid enumerator ID.

namespace BloombergLP {

// STATIC HELPER FUNCTIONS
static
const bdem_EnumerationDef *lookupEnumDef(const bdem_RecordDef *constraint,
                                         int                   index)
{
    BSLS_ASSERT(constraint);

    const bdem_FieldDef&       field   = constraint->field(index);
    const bdem_EnumerationDef *enumDef = field.enumerationConstraint();
    BSLS_ASSERT(enumDef);

    return enumDef;
}

                        // --------------------------
                        // class bdem_ConstRowBinding
                        // --------------------------

// ACCESSORS
const char *
bdem_ConstRowBinding::enumerationAsString(const char *elementName) const
{
    const int columnIndex = d_constraint_p->fieldIndex(elementName);
    return enumerationAsString(columnIndex);
}

const char *
bdem_ConstRowBinding::enumerationAsString(int columnIndex) const
{
    const bdem_EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       columnIndex);

    if (bdem_ElemType::BDEM_INT == d_row_p->elemType(columnIndex)) {
        return enumDef->lookupName(d_row_p->theInt(columnIndex));
    }
    else {
        return enumDef->lookupName(d_row_p->theString(columnIndex).c_str());
    }
}

int bdem_ConstRowBinding::enumerationAsInt(const char *elementName) const
{
    const int columnIndex = d_constraint_p->fieldIndex(elementName);
    return enumerationAsInt(columnIndex);
}

int bdem_ConstRowBinding::enumerationAsInt(int columnIndex) const
{
    if (bdem_ElemType::BDEM_INT == d_row_p->elemType(columnIndex)) {
        return d_row_p->theInt(columnIndex);
    }
    else {
        const bdem_EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                           columnIndex);

        return enumDef->lookupId(d_row_p->theString(columnIndex).c_str());
    }
}

                        // ---------------------
                        // class bdem_RowBinding
                        // ---------------------

// MANIPULATORS
int bdem_RowBinding::setEnumeration(const char         *elementName,
                                    const bsl::string&  enumeratorName) const
{
    const int columnIndex = d_constraint_p->fieldIndex(elementName);
    return setEnumeration(columnIndex, enumeratorName);
}

int bdem_RowBinding::setEnumeration(int                 columnIndex,
                                    const bsl::string&  enumeratorName) const
{
    const bdem_EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       columnIndex);

    const int enumeratorId = enumDef->lookupId(enumeratorName.c_str());
    if (bdetu_Unset<int>::unsetValue() == enumeratorId) {
        return 1;
    }

    if (bdem_ElemType::BDEM_INT == d_row_p->elemType(columnIndex)) {
        const_cast<bdem_Row *>(d_row_p)->theModifiableInt(columnIndex) =
                                                                  enumeratorId;
    }
    else {
        const_cast<bdem_Row *>(d_row_p)->theModifiableString(columnIndex) =
                                                                enumeratorName;
    }

    return 0;
}

int bdem_RowBinding::setEnumeration(const char *elementName,
                                    int         enumeratorId) const
{
    const int columnIndex = d_constraint_p->fieldIndex(elementName);
    return setEnumeration(columnIndex, enumeratorId);
}

int bdem_RowBinding::setEnumeration(int columnIndex, int enumeratorId) const
{
    const bdem_EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       columnIndex);

    const char *enumeratorName = enumDef->lookupName(enumeratorId);
    if (!enumeratorName) {
        return 1;
    }

    if (bdem_ElemType::BDEM_INT == d_row_p->elemType(columnIndex)) {
        const_cast<bdem_Row *>(d_row_p)->theModifiableInt(columnIndex) =
                                                                  enumeratorId;
    }
    else {
        const_cast<bdem_Row *>(d_row_p)->theModifiableString(columnIndex) =
                                                                enumeratorName;
    }

    return 0;
}

                        // ----------------------------
                        // class bdem_ConstTableBinding
                        // ----------------------------

// ACCESSORS
const char *
bdem_ConstTableBinding::enumerationAsString(int         rowIndex,
                                            const char *elementName) const
{
    const int columnIndex = d_constraint_p->fieldIndex(elementName);
    return enumerationAsString(rowIndex, columnIndex);
}

const char *
bdem_ConstTableBinding::enumerationAsString(int rowIndex,
                                            int columnIndex) const
{
    const bdem_EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       columnIndex);

    if (bdem_ElemType::BDEM_INT == d_table_p->columnType(columnIndex)) {
        return enumDef->lookupName(
                                 (*d_table_p)[rowIndex][columnIndex].theInt());
    }
    else {
        return enumDef->lookupName(
                      (*d_table_p)[rowIndex][columnIndex].theString().c_str());
    }
}

int bdem_ConstTableBinding::enumerationAsInt(int         rowIndex,
                                             const char *elementName) const
{
    const int columnIndex = d_constraint_p->fieldIndex(elementName);
    return enumerationAsInt(rowIndex, columnIndex);
}

int
bdem_ConstTableBinding::enumerationAsInt(int rowIndex,
                                         int columnIndex) const
{
    if (bdem_ElemType::BDEM_INT == d_table_p->columnType(columnIndex)) {
        return (*d_table_p)[rowIndex][columnIndex].theInt();
    }
    else {
        const bdem_EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                           columnIndex);

        return enumDef->lookupId(
                      (*d_table_p)[rowIndex][columnIndex].theString().c_str());
    }
}

                        // -----------------------
                        // class bdem_TableBinding
                        // -----------------------

// ACCESSORS
int
bdem_TableBinding::setEnumeration(int                 rowIndex,
                                  const char         *elementName,
                                  const bsl::string&  enumeratorName) const
{
    const int columnIndex = d_constraint_p->fieldIndex(elementName);
    return setEnumeration(rowIndex, columnIndex, enumeratorName);
}

int bdem_TableBinding::setEnumeration(int                 rowIndex,
                                      int                 columnIndex,
                                      const bsl::string&  enumeratorName) const
{
    const bdem_EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       columnIndex);

    const int enumeratorId = enumDef->lookupId(enumeratorName.c_str());
    if (bdetu_Unset<int>::unsetValue() == enumeratorId) {
        return 0;
    }

    bdem_ElemRef ref = (*const_cast<bdem_Table *>(d_table_p)).
                                       theModifiableRow(rowIndex)[columnIndex];

    if (bdem_ElemType::BDEM_INT == d_table_p->columnType(columnIndex)) {
        ref.theModifiableInt()    = enumeratorId;
    }
    else {
        ref.theModifiableString() = enumeratorName;
    }

    return 1;
}

int bdem_TableBinding::setEnumeration(int         rowIndex,
                                      const char *elementName,
                                      int         enumeratorId) const
{
    const int columnIndex = d_constraint_p->fieldIndex(elementName);
    return setEnumeration(rowIndex, columnIndex, enumeratorId);
}

int bdem_TableBinding::setEnumeration(int rowIndex,
                                      int columnIndex,
                                      int enumeratorId) const
{
    const bdem_EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       columnIndex);

    const char *enumeratorName = enumDef->lookupName(enumeratorId);
    if (!enumeratorName) {
        return 1;
    }

    bdem_ElemRef ref = (*const_cast<bdem_Table *>(d_table_p)).
                                       theModifiableRow(rowIndex)[columnIndex];

    if (bdem_ElemType::BDEM_INT == d_table_p->columnType(columnIndex)) {
        ref.theModifiableInt()    = enumeratorId;
    }
    else {
        ref.theModifiableString() = enumeratorName;
    }

    return 0;
}

                        // -----------------------------
                        // class bdem_ConstColumnBinding
                        // -----------------------------

// ACCESSORS
const char *
bdem_ConstColumnBinding::enumerationAsString(int rowIndex) const
{
    const bdem_EnumerationDef *enumDef =
                                       d_constraint_p->enumerationConstraint();
    BSLS_ASSERT(enumDef);

    if (bdem_ElemType::BDEM_INT == d_constraint_p->elemType()) {
        return enumDef->lookupName(
                               (*d_table_p)[rowIndex][d_columnIndex].theInt());
    }
    else {
        return enumDef->lookupName(
                    (*d_table_p)[rowIndex][d_columnIndex].theString().c_str());
    }
}

int bdem_ConstColumnBinding::enumerationAsInt(int rowIndex) const
{
    if (bdem_ElemType::BDEM_INT == d_constraint_p->elemType()) {
        return (*d_table_p)[rowIndex][d_columnIndex].theInt();
    }
    else {
        const bdem_EnumerationDef *enumDef =
                                       d_constraint_p->enumerationConstraint();
        BSLS_ASSERT(enumDef);

        return enumDef->lookupId(
                    (*d_table_p)[rowIndex][d_columnIndex].theString().c_str());
    }
}

// FREE OPERATORS
bool operator==(const bdem_ConstColumnBinding& lhs,
                const bdem_ConstColumnBinding& rhs)
{
    const int             columnLength  = lhs.numRows();
    const bdem_RecordDef *lhsConstraint =
                                        lhs.d_constraint_p->recordConstraint();
    const bdem_RecordDef *rhsConstraint =
                                        rhs.d_constraint_p->recordConstraint();

    if (columnLength                   != rhs.numRows()
     || lhs.d_constraint_p->elemType() != rhs.d_constraint_p->elemType()
     || !lhsConstraint                 != !rhsConstraint
     || (lhsConstraint && rhsConstraint
     &&  !bdem_SchemaUtil::areEquivalent(*lhsConstraint, *rhsConstraint))) {
        return false;
    }
    for (int rowIndex = 0; rowIndex < columnLength; ++rowIndex) {
        if ((*lhs.d_table_p)[rowIndex][lhs.d_columnIndex]
                            != (*rhs.d_table_p)[rowIndex][rhs.d_columnIndex]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const bdem_ConstColumnBinding& lhs,
                const bdem_ConstColumnBinding& rhs)
{
    return !(lhs == rhs);
}

                        // ------------------------
                        // class bdem_ColumnBinding
                        // ------------------------

// ACCESSORS
int bdem_ColumnBinding::setEnumeration(int                rowIndex,
                                       const bsl::string& enumeratorName) const
{
    const bdem_EnumerationDef *enumDef =
                                       d_constraint_p->enumerationConstraint();
    BSLS_ASSERT(enumDef);

    const int enumeratorId = enumDef->lookupId(enumeratorName.c_str());
    if (bdetu_Unset<int>::unsetValue() == enumeratorId) {
        return 1;
    }

    bdem_ElemRef ref = (*const_cast<bdem_Table *>(d_table_p)).
                                     theModifiableRow(rowIndex)[d_columnIndex];

    if (bdem_ElemType::BDEM_INT == d_constraint_p->elemType()) {
        ref.theModifiableInt()    = enumeratorId;
    }
    else {
        ref.theModifiableString() = enumeratorName;
    }

    return 0;
}

int bdem_ColumnBinding::setEnumeration(int rowIndex,
                                       int enumeratorId) const
{
    const bdem_EnumerationDef *enumDef =
                                       d_constraint_p->enumerationConstraint();
    BSLS_ASSERT(enumDef);

    const char *enumeratorName = enumDef->lookupName(enumeratorId);
    if (!enumeratorName) {
        return 1;
    }

    bdem_ElemRef ref = (*const_cast<bdem_Table *>(d_table_p)).
                                     theModifiableRow(rowIndex)[d_columnIndex];

    if (bdem_ElemType::BDEM_INT == d_constraint_p->elemType()) {
        ref.theModifiableInt()    = enumeratorId;
    }
    else {
        ref.theModifiableString() = enumeratorName;
    }

    return 0;
}

                        // -----------------------------
                        // class bdem_ConstChoiceBinding
                        // -----------------------------

// ACCESSORS
const char *bdem_ConstChoiceBinding::enumerationAsString() const
{
    const bdem_EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       d_item_p->selector());

    if (bdem_ElemType::BDEM_INT == d_item_p->selectionType()) {
        return enumDef->lookupName(d_item_p->theInt());
    }
    else {
        return enumDef->lookupName(d_item_p->theString().c_str());
    }
}

int bdem_ConstChoiceBinding::enumerationAsInt() const
{
    if (bdem_ElemType::BDEM_INT == d_item_p->selectionType()) {
        return d_item_p->theInt();
    }
    else {
        const bdem_EnumerationDef *enumDef = lookupEnumDef(
                                                         d_constraint_p,
                                                         d_item_p->selector());

        return enumDef->lookupId(d_item_p->theString().c_str());
    }
}

                        // ------------------------
                        // class bdem_ChoiceBinding
                        // ------------------------

// ACCESSORS
int bdem_ChoiceBinding::setEnumeration(const bsl::string& enumeratorName) const
{
    const bdem_EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       d_item_p->selector());

    const int enumeratorId = enumDef->lookupId(enumeratorName.c_str());
    if (bdetu_Unset<int>::unsetValue() == enumeratorId) {
        return 1;
    }

    if (bdem_ElemType::BDEM_INT == d_item_p->selectionType()) {
        const_cast<bdem_ChoiceArrayItem *>(d_item_p)->theModifiableInt() =
                                                                  enumeratorId;
    }
    else {
        const_cast<bdem_ChoiceArrayItem *>(d_item_p)->theModifiableString() =
                                                                enumeratorName;
    }

    return 0;
}

int bdem_ChoiceBinding::setEnumeration(int enumeratorId) const
{
    const bdem_EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       d_item_p->selector());

    const char *enumeratorName = enumDef->lookupName(enumeratorId);
    if (!enumeratorName) {
        return 1;
    }

    if (bdem_ElemType::BDEM_INT == d_item_p->selectionType()) {
        const_cast<bdem_ChoiceArrayItem *>(d_item_p)->theModifiableInt() =
                                                                  enumeratorId;
    }
    else {
        const_cast<bdem_ChoiceArrayItem *>(d_item_p)->theModifiableString() =
                                                                enumeratorName;
    }

    return 0;
}

                        // ----------------------------------
                        // class bdem_ConstChoiceArrayBinding
                        // ----------------------------------

// ACCESSORS
const char *
bdem_ConstChoiceArrayBinding::enumerationAsString(int rowIndex) const
{
    const bdem_EnumerationDef *enumDef = lookupEnumDef(
                                      d_constraint_p,
                                      (*d_choiceArray_p)[rowIndex].selector());

    if (bdem_ElemType::BDEM_INT ==
                                (*d_choiceArray_p)[rowIndex].selectionType()) {
        return enumDef->lookupName((*d_choiceArray_p)[rowIndex].theInt());
    }
    else {
        return enumDef->lookupName(
                             (*d_choiceArray_p)[rowIndex].theString().c_str());
    }
}

int bdem_ConstChoiceArrayBinding::enumerationAsInt(int rowIndex) const
{
    if (bdem_ElemType::BDEM_INT ==
                                (*d_choiceArray_p)[rowIndex].selectionType()) {
        return (*d_choiceArray_p)[rowIndex].theInt();
    }
    else {
        const bdem_EnumerationDef *enumDef = lookupEnumDef(
                                      d_constraint_p,
                                      (*d_choiceArray_p)[rowIndex].selector());

        return enumDef->lookupId(
                             (*d_choiceArray_p)[rowIndex].theString().c_str());
    }
}

                        // -----------------------------
                        // class bdem_ChoiceArrayBinding
                        // -----------------------------

// ACCESSORS
int bdem_ChoiceArrayBinding::setEnumeration(
                                      int                 itemIndex,
                                      const bsl::string&  enumeratorName) const
{
    const bdem_EnumerationDef *enumDef = lookupEnumDef(
                                     d_constraint_p,
                                     (*d_choiceArray_p)[itemIndex].selector());

    const int enumeratorId = enumDef->lookupId(enumeratorName.c_str());
    if (bdetu_Unset<int>::unsetValue() == enumeratorId) {
        return 1;
    }

    bdem_ChoiceArrayItem& item =
                            (*const_cast<bdem_ChoiceArray *>(d_choiceArray_p)).
                                                  theModifiableItem(itemIndex);

    if (bdem_ElemType::BDEM_INT ==
                               (*d_choiceArray_p)[itemIndex].selectionType()) {
        item.theModifiableInt()    = enumeratorId;
    }
    else {
        item.theModifiableString() = enumeratorName;
    }

    return 0;
}

int
bdem_ChoiceArrayBinding::setEnumeration(int itemIndex,
                                        int enumeratorId) const
{
    const bdem_EnumerationDef *enumDef = lookupEnumDef(
                                     d_constraint_p,
                                     (*d_choiceArray_p)[itemIndex].selector());

    const char *enumeratorName = enumDef->lookupName(enumeratorId);
    if (!enumeratorName) {
        return 1;
    }

    bdem_ChoiceArrayItem& item =
                            (*const_cast<bdem_ChoiceArray *>(d_choiceArray_p)).
                                                  theModifiableItem(itemIndex);

    if (bdem_ElemType::BDEM_INT ==
                               (*d_choiceArray_p)[itemIndex].selectionType()) {
        item.theModifiableInt()    = enumeratorId;
    }
    else {
        item.theModifiableString() = enumeratorName;
    }

    return 0;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
