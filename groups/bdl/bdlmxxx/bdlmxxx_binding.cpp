// bdlmxxx_binding.cpp                                                -*-C++-*-
#include <bdlmxxx_binding.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_binding_cpp,"$Id$ $CSID$")

#include <bdlt_date.h>                  // for testing only
#include <bdlt_datetime.h>              // for testing only
#include <bdlt_datetimetz.h>            // for testing only
#include <bdlt_datetz.h>                // for testing only
#include <bdlt_time.h>                  // for testing only
#include <bdlt_timetz.h>                // for testing only

#include <bdltuxxx_unset.h>

#include <bsls_assert.h>

///IMPLEMENTATION NOTE
///-------------------
// The nullness of the relevant element need not be explicitly examined in the
// 'enumerationAsInt' and 'enumerationAsString' methods because the correct
// result is returned whether or not the 'INT' or 'STRING' element is null or
// simply has the "unset" value.  Note that per 'bdlmxxx_schema', the "unset"
// 'STRING' ("") is not a valid enumerator name, and the "unset" 'INT'
// (INT_MIN) is not a valid enumerator ID.

namespace BloombergLP {

// STATIC HELPER FUNCTIONS
static
const bdlmxxx::EnumerationDef *lookupEnumDef(const bdlmxxx::RecordDef *constraint,
                                         int                   index)
{
    BSLS_ASSERT(constraint);

    const bdlmxxx::FieldDef&       field   = constraint->field(index);
    const bdlmxxx::EnumerationDef *enumDef = field.enumerationConstraint();

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
    BSLS_ASSERT(elementName);

    const int index = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT(0 <= index);

    return enumerationAsString(index);
}

const char *
bdem_ConstRowBinding::enumerationAsString(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length());

    const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p, index);

    const bdlmxxx::ElemType::Type type = elemType(index);

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        return enumDef->lookupName(d_row_p->theInt(index));           // RETURN
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        return enumDef->lookupName(d_row_p->theString(index).c_str());
                                                                      // RETURN
    }
}

int bdem_ConstRowBinding::enumerationAsInt(const char *elementName) const
{
    BSLS_ASSERT(elementName);

    const int index = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT(0 <= index);

    return enumerationAsInt(index);
}

int bdem_ConstRowBinding::enumerationAsInt(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length());

    const bdlmxxx::ElemType::Type type = elemType(index);

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        return d_row_p->theInt(index);                                // RETURN
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                           index);

        return enumDef->lookupId(d_row_p->theString(index).c_str());  // RETURN
    }
}

                        // ---------------------
                        // class bdem_RowBinding
                        // ---------------------

// MANIPULATORS
int bdem_RowBinding::setEnumeration(const char         *elementName,
                                    const bsl::string&  enumeratorName) const
{
    BSLS_ASSERT(elementName);

    const int index = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT(0 <= index);

    return setEnumeration(index, enumeratorName);
}

int bdem_RowBinding::setEnumeration(int                 index,
                                    const bsl::string&  enumeratorName) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length());

    const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p, index);

    const int enumeratorId = enumDef->lookupId(enumeratorName.c_str());
    if (bdltuxxx::Unset<int>::unsetValue() == enumeratorId) {
        return 1;                                                     // RETURN
    }

    bdlmxxx::Row *row = const_cast<bdlmxxx::Row *>(d_row_p);

    const bdlmxxx::ElemType::Type type = elemType(index);

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        row->theModifiableInt(index)    = enumeratorId;
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        row->theModifiableString(index) = enumeratorName;
    }

    return 0;
}

int bdem_RowBinding::setEnumeration(const char *elementName,
                                    int         enumeratorId) const
{
    BSLS_ASSERT(elementName);

    const int index = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT(0 <= index);

    return setEnumeration(index, enumeratorId);
}

int bdem_RowBinding::setEnumeration(int index, int enumeratorId) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length());

    const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p, index);

    const char *enumeratorName = enumDef->lookupName(enumeratorId);
    if (!enumeratorName) {
        return 1;                                                     // RETURN
    }

    bdlmxxx::Row *row = const_cast<bdlmxxx::Row *>(d_row_p);

    const bdlmxxx::ElemType::Type type = elemType(index);

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        row->theModifiableInt(index)    = enumeratorId;
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        row->theModifiableString(index) = enumeratorName;
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
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex < numRows());
    BSLS_ASSERT(elementName);

    const int columnIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT(0 <= columnIndex);

    return enumerationAsString(rowIndex, columnIndex);
}

const char *
bdem_ConstTableBinding::enumerationAsString(int rowIndex,
                                            int columnIndex) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex    < numRows());
    BSLS_ASSERT(0 <= columnIndex);
    BSLS_ASSERT(     columnIndex < numColumns());

    const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       columnIndex);

    bdlmxxx::ConstElemRef ref = (*this)[rowIndex][columnIndex];

    const bdlmxxx::ElemType::Type type = elemType(columnIndex);

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        return enumDef->lookupName(ref.theInt());                     // RETURN
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        return enumDef->lookupName(ref.theString().c_str());          // RETURN
    }
}

int bdem_ConstTableBinding::enumerationAsInt(int         rowIndex,
                                             const char *elementName) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex < numRows());
    BSLS_ASSERT(elementName);

    const int columnIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT(0 <= columnIndex);

    return enumerationAsInt(rowIndex, columnIndex);
}

int
bdem_ConstTableBinding::enumerationAsInt(int rowIndex, int columnIndex) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex    < numRows());
    BSLS_ASSERT(0 <= columnIndex);
    BSLS_ASSERT(     columnIndex < numColumns());

    bdlmxxx::ConstElemRef ref = (*this)[rowIndex][columnIndex];

    const bdlmxxx::ElemType::Type type = elemType(columnIndex);

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        return ref.theInt();                                          // RETURN
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                           columnIndex);

        return enumDef->lookupId(ref.theString().c_str());            // RETURN
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
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex < numRows());
    BSLS_ASSERT(elementName);

    const int columnIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT(0 <= columnIndex);

    return setEnumeration(rowIndex, columnIndex, enumeratorName);
}

int bdem_TableBinding::setEnumeration(int                 rowIndex,
                                      int                 columnIndex,
                                      const bsl::string&  enumeratorName) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex    < numRows());
    BSLS_ASSERT(0 <= columnIndex);
    BSLS_ASSERT(     columnIndex < numColumns());

    const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       columnIndex);

    const int enumeratorId = enumDef->lookupId(enumeratorName.c_str());
    if (bdltuxxx::Unset<int>::unsetValue() == enumeratorId) {
        return 0;                                                     // RETURN
    }

    bdlmxxx::ElemRef ref = (*this)[rowIndex][columnIndex];

    const bdlmxxx::ElemType::Type type = elemType(columnIndex);

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        ref.theModifiableInt()    = enumeratorId;
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        ref.theModifiableString() = enumeratorName;
    }

    return 1;
}

int bdem_TableBinding::setEnumeration(int         rowIndex,
                                      const char *elementName,
                                      int         enumeratorId) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex < numRows());
    BSLS_ASSERT(elementName);

    const int columnIndex = d_constraint_p->fieldIndex(elementName);

    BSLS_ASSERT(0 <= columnIndex);

    return setEnumeration(rowIndex, columnIndex, enumeratorId);
}

int bdem_TableBinding::setEnumeration(int rowIndex,
                                      int columnIndex,
                                      int enumeratorId) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex    < numRows());
    BSLS_ASSERT(0 <= columnIndex);
    BSLS_ASSERT(     columnIndex < numColumns());

    const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       columnIndex);

    const char *enumeratorName = enumDef->lookupName(enumeratorId);
    if (!enumeratorName) {
        return 1;                                                     // RETURN
    }

    bdlmxxx::ElemRef ref = (*this)[rowIndex][columnIndex];

    const bdlmxxx::ElemType::Type type = elemType(columnIndex);

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        ref.theModifiableInt()    = enumeratorId;
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        ref.theModifiableString() = enumeratorName;
    }

    return 0;
}

                        // -----------------------------
                        // class bdem_ConstColumnBinding
                        // -----------------------------

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
// PRIVATE ACCESSORS
bool bdem_ConstColumnBinding::canSatisfyColumn(
                                              const bdlmxxx::Table    *table,
                                              int                  columnIndex,
                                              const bdlmxxx::FieldDef *field)
{
    const bdlmxxx::ElemType::Type type = table->columnType(columnIndex);

    if (!bdlmxxx::ElemType::isAggregateType(type)) {
        return true;                                                  // RETURN
    }

    const bdlmxxx::RecordDef *constraint = field->recordConstraint();

    if (!constraint || type != field->elemType()) {
        return false;                                                 // RETURN
    }

    const int numRows = table->numRows();

    switch (type) {
      case bdlmxxx::ElemType::BDEM_LIST: {
        for (int i = 0; i < numRows; ++i) {
            if (!bdlmxxx::SchemaAggregateUtil::canSatisfyRecord(
                                              (*table)[i].theList(columnIndex),
                                              *constraint)) {
                return false;                                         // RETURN
            }
        }
      } break;
      case bdlmxxx::ElemType::BDEM_TABLE: {
        for (int i = 0; i < numRows; ++i) {
            if (!bdlmxxx::SchemaAggregateUtil::canSatisfyRecord(
                                             (*table)[i].theTable(columnIndex),
                                             *constraint)) {
                return false;                                         // RETURN
            }
        }
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE: {
        for (int i = 0; i < numRows; ++i) {
            if (!bdlmxxx::SchemaAggregateUtil::canSatisfyRecord(
                                            (*table)[i].theChoice(columnIndex),
                                            *constraint)) {
                return false;                                         // RETURN
            }
        }
      } break;
      case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
        for (int i = 0; i < numRows; ++i) {
            if (!bdlmxxx::SchemaAggregateUtil::canSatisfyRecord(
                                       (*table)[i].theChoiceArray(columnIndex),
                                       *constraint)) {
                return false;                                         // RETURN
            }
        }
      } break;
      default: {
        BSLS_ASSERT(0);  // Should *not* reach here!
      } break;
    }

    return true;
}
#endif

// ACCESSORS
const char *bdem_ConstColumnBinding::enumerationAsString(int rowIndex) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex < numRows());

    const bdlmxxx::EnumerationDef *enumDef =
                                       d_constraint_p->enumerationConstraint();
    BSLS_ASSERT(enumDef);

    bdlmxxx::ConstElemRef ref = (*this)[rowIndex];

    const bdlmxxx::ElemType::Type type = elemType();

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        return enumDef->lookupName(ref.theInt());                     // RETURN
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        return enumDef->lookupName(ref.theString().c_str());          // RETURN
    }
}

int bdem_ConstColumnBinding::enumerationAsInt(int rowIndex) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex < numRows());

    bdlmxxx::ConstElemRef ref = (*this)[rowIndex];

    const bdlmxxx::ElemType::Type type = elemType();

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        return ref.theInt();                                          // RETURN
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        const bdlmxxx::EnumerationDef *enumDef =
                                       d_constraint_p->enumerationConstraint();
        BSLS_ASSERT(enumDef);

        return enumDef->lookupId(ref.theString().c_str());            // RETURN
    }
}

// FREE OPERATORS
bool operator==(const bdem_ConstColumnBinding& lhs,
                const bdem_ConstColumnBinding& rhs)
{
    const int numRows = lhs.numRows();

    const bdlmxxx::RecordDef *lhsConstraint =
                                        lhs.d_constraint_p->recordConstraint();
    const bdlmxxx::RecordDef *rhsConstraint =
                                        rhs.d_constraint_p->recordConstraint();

    if (numRows        != rhs.numRows()
     || lhs.elemType() != rhs.elemType()
     || !lhsConstraint != !rhsConstraint
     || (lhsConstraint && rhsConstraint
     &&  !bdlmxxx::SchemaUtil::areEquivalent(*lhsConstraint, *rhsConstraint))) {
        return false;                                                 // RETURN
    }

    for (int rowIndex = 0; rowIndex < numRows; ++rowIndex) {
        if ((*lhs.d_table_p)[rowIndex][lhs.d_columnIndex]
                            != (*rhs.d_table_p)[rowIndex][rhs.d_columnIndex]) {
            return false;                                             // RETURN
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
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex < numRows());

    const bdlmxxx::EnumerationDef *enumDef =
                                       d_constraint_p->enumerationConstraint();
    BSLS_ASSERT(enumDef);

    const int enumeratorId = enumDef->lookupId(enumeratorName.c_str());
    if (bdltuxxx::Unset<int>::unsetValue() == enumeratorId) {
        return 1;                                                     // RETURN
    }

    bdlmxxx::ElemRef ref = (*this)[rowIndex];

    const bdlmxxx::ElemType::Type type = elemType();

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        ref.theModifiableInt()    = enumeratorId;
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        ref.theModifiableString() = enumeratorName;
    }

    return 0;
}

int bdem_ColumnBinding::setEnumeration(int rowIndex, int enumeratorId) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex < numRows());

    const bdlmxxx::EnumerationDef *enumDef =
                                       d_constraint_p->enumerationConstraint();
    BSLS_ASSERT(enumDef);

    const char *enumeratorName = enumDef->lookupName(enumeratorId);
    if (!enumeratorName) {
        return 1;                                                     // RETURN
    }

    bdlmxxx::ElemRef ref = (*this)[rowIndex];

    const bdlmxxx::ElemType::Type type = elemType();

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        ref.theModifiableInt()    = enumeratorId;
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

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
    const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       d_item_p->selector());

    const bdlmxxx::ElemType::Type type = selectionType();

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        return enumDef->lookupName(theInt());                         // RETURN
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        return enumDef->lookupName(theString().c_str());              // RETURN
    }
}

int bdem_ConstChoiceBinding::enumerationAsInt() const
{
    const bdlmxxx::ElemType::Type type = selectionType();

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        return theInt();                                              // RETURN
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                           selector());

        return enumDef->lookupId(theString().c_str());                // RETURN
    }
}

                        // ------------------------
                        // class bdem_ChoiceBinding
                        // ------------------------

// ACCESSORS
int bdem_ChoiceBinding::setEnumeration(const bsl::string& enumeratorName) const
{
    const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       d_item_p->selector());

    const int enumeratorId = enumDef->lookupId(enumeratorName.c_str());
    if (bdltuxxx::Unset<int>::unsetValue() == enumeratorId) {
        return 1;                                                     // RETURN
    }

    const bdlmxxx::ElemType::Type type = selectionType();

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        theModifiableInt()    = enumeratorId;
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        theModifiableString() = enumeratorName;
    }

    return 0;
}

int bdem_ChoiceBinding::setEnumeration(int enumeratorId) const
{
    const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       d_item_p->selector());

    const char *enumeratorName = enumDef->lookupName(enumeratorId);
    if (!enumeratorName) {
        return 1;                                                     // RETURN
    }

    const bdlmxxx::ElemType::Type type = selectionType();

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        theModifiableInt()    = enumeratorId;
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        theModifiableString() = enumeratorName;
    }

    return 0;
}

                        // ----------------------------------
                        // class bdem_ConstChoiceArrayBinding
                        // ----------------------------------

// ACCESSORS
const char *
bdem_ConstChoiceArrayBinding::enumerationAsString(int itemIndex) const
{
    BSLS_ASSERT(0 <= itemIndex);
    BSLS_ASSERT(     itemIndex < length());

    const bdlmxxx::ChoiceArrayItem& item = (*this)[itemIndex];

    const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       item.selector());

    const bdlmxxx::ElemType::Type type = item.selectionType();

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        return enumDef->lookupName(item.theInt());                    // RETURN
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        return enumDef->lookupName(item.theString().c_str());         // RETURN
    }
}

int bdem_ConstChoiceArrayBinding::enumerationAsInt(int itemIndex) const
{
    BSLS_ASSERT(0 <= itemIndex);
    BSLS_ASSERT(     itemIndex < length());

    const bdlmxxx::ChoiceArrayItem& item = (*this)[itemIndex];

    const bdlmxxx::ElemType::Type type = item.selectionType();

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        return item.theInt();                                         // RETURN
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                           item.selector());

        return enumDef->lookupId(item.theString().c_str());           // RETURN
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
    BSLS_ASSERT(0 <= itemIndex);
    BSLS_ASSERT(     itemIndex < length());

    bdlmxxx::ChoiceArrayItem& item = (*this)[itemIndex];

    const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       item.selector());

    const int enumeratorId = enumDef->lookupId(enumeratorName.c_str());
    if (bdltuxxx::Unset<int>::unsetValue() == enumeratorId) {
        return 1;                                                     // RETURN
    }

    const bdlmxxx::ElemType::Type type = item.selectionType();

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        item.theModifiableInt()    = enumeratorId;
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        item.theModifiableString() = enumeratorName;
    }

    return 0;
}

int
bdem_ChoiceArrayBinding::setEnumeration(int itemIndex, int enumeratorId) const
{
    bdlmxxx::ChoiceArrayItem& item = (*this)[itemIndex];

    const bdlmxxx::EnumerationDef *enumDef = lookupEnumDef(d_constraint_p,
                                                       item.selector());

    const char *enumeratorName = enumDef->lookupName(enumeratorId);
    if (!enumeratorName) {
        return 1;                                                     // RETURN
    }

    const bdlmxxx::ElemType::Type type = item.selectionType();

    if (bdlmxxx::ElemType::BDEM_INT == type) {
        item.theModifiableInt()    = enumeratorId;
    }
    else {
        BSLS_ASSERT(bdlmxxx::ElemType::BDEM_STRING == type);

        item.theModifiableString() = enumeratorName;
    }

    return 0;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
