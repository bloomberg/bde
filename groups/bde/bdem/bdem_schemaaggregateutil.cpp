// bdem_schemaaggregateutil.cpp                                       -*-C++-*-
#include <bdem_schemaaggregateutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_schemaaggregateutil_cpp,"$Id$ $CSID$")

#include <bdem_aggregate.h>
#include <bdem_elemtype.h>
#include <bdem_schema.h>
#include <bdem_schemaenumerationutil.h>

#include <bsls_assert.h>

#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS
static
bool canRowDataSatisfyRecord(const bdem_Row& row, const bdem_RecordDef& record)
    // Return 'true' if the specified 'row' can satisfy the specified
    // 'record', and 'false' otherwise.  The behavior is undefined unless
    // 'record' specifies a sequence type and the types of the initial
    // 'record.numFields()' fields in 'row' match the corresponding types in
    // 'record'.  Refer to the component-level documentation for further
    // information on how a 'bdem_Row' can satisfy a 'bdem_RecordDef'.
{
    const int numFields = record.numFields();

    for (int i = 0; i < numFields; ++i) {
        const bdem_FieldDef&     field   = record.field(i);
        const bdem_ConstElemRef& elemRef = row[i];

        if (! elemRef.isNull()) {
            const bdem_ElemType::Type  type           = field.elemType();
            const bdem_RecordDef      *rConstraintPtr =
                                                      field.recordConstraint();
            const bdem_EnumerationDef *eConstraintPtr =
                                                 field.enumerationConstraint();
            BSLS_ASSERT(type == elemRef.type());

            if (rConstraintPtr) {
                BSLS_ASSERT(!eConstraintPtr);
                BSLS_ASSERT(bdem_ElemType::isAggregateType(type));

                switch (type) {
                  case bdem_ElemType::BDEM_LIST: {
                    const bdem_List& subList = elemRef.theList();

                    if (!bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                            subList,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_TABLE: {
                    const bdem_Table& subTable = elemRef.theTable();

                    if (!bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                            subTable,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_CHOICE: {
                    const bdem_Choice& subChoice = elemRef.theChoice();

                    if (!bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                            subChoice,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_CHOICE_ARRAY: {
                    const bdem_ChoiceArray& subChoiceArray =
                                                      elemRef.theChoiceArray();

                    if (!bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                            subChoiceArray,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  default: {
                    BSLS_ASSERT("record constraint on non-aggregate" && 0);
                  } break;
                }
            }
            else if (eConstraintPtr) {
                BSLS_ASSERT(bdem_EnumerationDef::canHaveEnumConstraint(type));

                switch (type) {
                  case bdem_ElemType::BDEM_INT: {
                    if (!bdem_SchemaEnumerationUtil::isConformant(
                                                            elemRef.theInt(),
                                                            *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_STRING: {
                    if (!bdem_SchemaEnumerationUtil::isConformant(
                                                           elemRef.theString(),
                                                           *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_INT_ARRAY: {
                    if (!bdem_SchemaEnumerationUtil::isConformant(
                                                         elemRef.theIntArray(),
                                                         *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_STRING_ARRAY: {
                    if (!bdem_SchemaEnumerationUtil::isConformant(
                                                      elemRef.theStringArray(),
                                                      *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  default: {
                    BSLS_ASSERT("enum constraint on non-enum" && 0);
                  }
                }
            }
        }
    }

    return true;
}

static
bool isRowDataConformant(const bdem_Row& row, const bdem_RecordDef& record)
    // Return 'true' if the specified 'row' is conformant to the specified
    // 'record', and 'false' otherwise.  The behavior is undefined unless
    // 'record' specifies a sequence type, and the number and types of the
    // elements in 'row' match those of the corresponding fields in 'record'.
    // Refer to the component-level documentation for further information on
    // how a 'bdem_Row' is conformant to a 'bdem_RecordDef'.
{
    const int numFields = record.numFields();

    for (int i = 0; i < numFields; ++i) {
        const bdem_FieldDef&     field   = record.field(i);
        const bdem_ConstElemRef& elemRef = row[i];

        if (! elemRef.isNull()) {
            const bdem_ElemType::Type  type           = field.elemType();
            const bdem_RecordDef      *rConstraintPtr =
                                                      field.recordConstraint();
            const bdem_EnumerationDef *eConstraintPtr =
                                                 field.enumerationConstraint();
            BSLS_ASSERT(type == elemRef.type());

            if (rConstraintPtr) {
                BSLS_ASSERT(!eConstraintPtr);
                BSLS_ASSERT(bdem_ElemType::isAggregateType(type));

                switch (type) {
                  case bdem_ElemType::BDEM_LIST: {
                    const bdem_List& subList = elemRef.theList();

                    if (!bdem_SchemaAggregateUtil::isListConformant(
                                                            subList,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_TABLE: {
                    const bdem_Table& subTable = elemRef.theTable();

                    if (!bdem_SchemaAggregateUtil::isTableConformant(
                                                            subTable,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_CHOICE: {
                    const bdem_Choice& subChoice = elemRef.theChoice();

                    if (!bdem_SchemaAggregateUtil::isChoiceConformant(
                                                            subChoice,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_CHOICE_ARRAY: {
                    const bdem_ChoiceArray& subChoiceArray =
                                                      elemRef.theChoiceArray();

                    if (!bdem_SchemaAggregateUtil::isChoiceArrayConformant(
                                                            subChoiceArray,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  default: {
                    BSLS_ASSERT("record constraint on non-aggregate" && 0);
                  } break;
                }
            }
            else if (eConstraintPtr) {
                BSLS_ASSERT(bdem_EnumerationDef::canHaveEnumConstraint(type));

                switch (type) {
                  case bdem_ElemType::BDEM_INT: {
                    if (!bdem_SchemaEnumerationUtil::isConformant(
                                                            elemRef.theInt(),
                                                            *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_STRING: {
                    if (!bdem_SchemaEnumerationUtil::isConformant(
                                                           elemRef.theString(),
                                                           *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_INT_ARRAY: {
                    if (!bdem_SchemaEnumerationUtil::isConformant(
                                                         elemRef.theIntArray(),
                                                         *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_STRING_ARRAY: {
                    if (!bdem_SchemaEnumerationUtil::isConformant(
                                                      elemRef.theStringArray(),
                                                      *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  default: {
                    BSLS_ASSERT("enum constraint on non-enum" && 0);
                  }
                }
            }
        }
    }

    return true;
}

static
bool isRowDataDeepConformant(const bdem_Row& row, const bdem_RecordDef& record)
    // Return 'true' if the specified 'row' is deep conformant to the
    // specified 'record', and 'false' otherwise.  The behavior is undefined
    // unless 'record' specifies a sequence type, and the number and types of
    // the elements in 'row' exactly match the number and types of the
    // corresponding fields in 'record'.  Refer to the component-level
    // documentation for further information on how a 'bdem_Row' is deep
    // conformant to a 'bdem_RecordDef'.
{
    const int numFields = record.numFields();

    for (int i = 0; i < numFields; ++i) {
        const bdem_FieldDef&       field          = record.field(i);
        const bdem_ConstElemRef&   elemRef        = row[i];
        const bdem_ElemType::Type  type           = field.elemType();
        const bdem_RecordDef      *rConstraintPtr = field.recordConstraint();
        const bdem_EnumerationDef *eConstraintPtr =
                                                 field.enumerationConstraint();

        BSLS_ASSERT(type == elemRef.type());
        BSLS_ASSERT(!rConstraintPtr || !eConstraintPtr);

        if (elemRef.isNull()) {
            if (rConstraintPtr && bdem_ElemType::isAggregateType(type) &&
                                                        ! field.isNullable()) {
                return false;                                         // RETURN
            }
        }
        else {
            if (rConstraintPtr) {
                BSLS_ASSERT(bdem_ElemType::isAggregateType(type));

                switch (type) {
                  case bdem_ElemType::BDEM_LIST: {
                    const bdem_List& subList = elemRef.theList();

                    if (!bdem_SchemaAggregateUtil::isListDeepConformant(
                                                            subList,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_TABLE: {
                    const bdem_Table& subTable = elemRef.theTable();

                    if (!bdem_SchemaAggregateUtil::isTableDeepConformant(
                                                            subTable,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_CHOICE: {
                    const bdem_Choice& subChoice = elemRef.theChoice();

                    if (!bdem_SchemaAggregateUtil::isChoiceDeepConformant(
                                                            subChoice,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_CHOICE_ARRAY: {
                    const bdem_ChoiceArray& subChoiceArray =
                                                      elemRef.theChoiceArray();

                    if (!bdem_SchemaAggregateUtil::isChoiceArrayDeepConformant(
                                                            subChoiceArray,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  default: {
                    BSLS_ASSERT("record constraint on non-aggregate" && 0);
                  } break;
                }
            }
            else if (eConstraintPtr) {
                BSLS_ASSERT(bdem_EnumerationDef::canHaveEnumConstraint(type));

                switch (type) {
                  case bdem_ElemType::BDEM_INT: {
                    if (!bdem_SchemaEnumerationUtil::isConformant(
                                                            elemRef.theInt(),
                                                            *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_STRING: {
                    if (!bdem_SchemaEnumerationUtil::isConformant(
                                                           elemRef.theString(),
                                                           *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_INT_ARRAY: {
                    if (!bdem_SchemaEnumerationUtil::isConformant(
                                                         elemRef.theIntArray(),
                                                         *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdem_ElemType::BDEM_STRING_ARRAY: {
                    if (!bdem_SchemaEnumerationUtil::isConformant(
                                                      elemRef.theStringArray(),
                                                      *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  default: {
                    BSLS_ASSERT("enum constraint on non-enum" && 0);
                  }
                }
            }
        }
    }

    return true;
}

static
bool canSelectionDataSatisfyRecord(const bdem_ChoiceArrayItem& choiceArrayItem,
                                   const bdem_RecordDef&       record)
    // Return 'true' if the specified 'choiceArrayItem' can satisfy the
    // specified 'record', and 'false' otherwise.  The behavior is undefined
    // unless 'record' specifies a choice type, and the initial
    // 'record.numFields()' types in the types catalog of 'choiceArrayItem'
    // match the corresponding types in 'record'.  Refer to the component-level
    // documentation for further information on how a 'bdem_ChoiceArrayItem'
    // can satisfy a 'bdem_RecordDef'.
{
    const int numFields = record.numFields();
    const int selector  = choiceArrayItem.selector();

    if (numFields <= selector) {
        return false;                                                 // RETURN
    }

    if (-1 == selector) {
        return true;                                                  // RETURN
    }

    const bdem_ConstElemRef&  elemRef = choiceArrayItem.selection();
    const bdem_FieldDef&      field   = record.field(selector);
    const bdem_ElemType::Type type    = field.elemType();
    BSLS_ASSERT(type == elemRef.type());

    if (elemRef.isNull()) {
        return true;                                                  // RETURN
    }
    else {
        const bdem_RecordDef      *rConstraintPtr = field.recordConstraint();
        const bdem_EnumerationDef *eConstraintPtr =
                                                 field.enumerationConstraint();

        if (rConstraintPtr) {
            BSLS_ASSERT(!eConstraintPtr);
            BSLS_ASSERT(bdem_ElemType::isAggregateType(type));

            switch (type) {
              case bdem_ElemType::BDEM_LIST: {
                const bdem_List& subList = choiceArrayItem.theList();

                if (!bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                            subList,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_TABLE: {
                const bdem_Table& subTable = choiceArrayItem.theTable();

                if (!bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                            subTable,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_CHOICE: {
                const bdem_Choice& subChoice = choiceArrayItem.theChoice();

                if (!bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                            subChoice,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_CHOICE_ARRAY: {
                const bdem_ChoiceArray& subChoiceArray =
                                              choiceArrayItem.theChoiceArray();

                if (!bdem_SchemaAggregateUtil::canSatisfyRecord(
                                                            subChoiceArray,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              default: {
                BSLS_ASSERT("record constraint on non-aggregate" && 0);
              } break;
            }
        }
        else if (eConstraintPtr) {
            BSLS_ASSERT(bdem_EnumerationDef::canHaveEnumConstraint(type));

            switch (type) {
              case bdem_ElemType::BDEM_INT: {
                if (!bdem_SchemaEnumerationUtil::isConformant(
                                          choiceArrayItem.selection().theInt(),
                                          *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_STRING: {
                if (!bdem_SchemaEnumerationUtil::isConformant(
                                       choiceArrayItem.selection().theString(),
                                       *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_INT_ARRAY: {
                if (!bdem_SchemaEnumerationUtil::isConformant(
                                     choiceArrayItem.selection().theIntArray(),
                                     *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_STRING_ARRAY: {
                if (!bdem_SchemaEnumerationUtil::isConformant(
                                  choiceArrayItem.selection().theStringArray(),
                                  *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              default: {
                BSLS_ASSERT("enum constraint on non-enum" && 0);
              }
            }
        }
    }

    return true;
}

static
bool isSelectionDataConformant(const bdem_ChoiceArrayItem& choiceArrayItem,
                               const bdem_RecordDef&       record)
    // Return 'true' if the specified 'choiceArrayItem' is conformant to the
    // specified 'record', and 'false' otherwise.  The behavior is undefined
    // unless 'record' specifies a choice type and the number and types in the
    // types catalog of 'choiceArrayItem' match the number and types of the
    // corresponding fields in 'record'.  Refer to the component-level
    // documentation for further information on how a 'bdem_ChoiceArrayItem' is
    // conformant to a 'bdem_RecordDef'.
{
    const int selector = choiceArrayItem.selector();

    if (0 == record.numFields() || -1 == selector) {
        return true;                                                  // RETURN
    }

    if (choiceArrayItem.selection().isNull()) {
        return true;                                                  // RETURN
    }
    else {
        const bdem_FieldDef&       field          = record.field(selector);
        const bdem_ElemType::Type  type           = field.elemType();
        const bdem_RecordDef      *rConstraintPtr = field.recordConstraint();
        const bdem_EnumerationDef *eConstraintPtr =
                                                 field.enumerationConstraint();
        BSLS_ASSERT(type == choiceArrayItem.selectionType());

        if (rConstraintPtr) {
            BSLS_ASSERT(!eConstraintPtr);
            BSLS_ASSERT(bdem_ElemType::isAggregateType(type));

            switch (type) {
              case bdem_ElemType::BDEM_LIST: {
                const bdem_List& subList = choiceArrayItem.theList();

                if (!bdem_SchemaAggregateUtil::isListConformant(
                                                            subList,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_TABLE: {
                const bdem_Table& subTable = choiceArrayItem.theTable();

                if (!bdem_SchemaAggregateUtil::isTableConformant(
                                                            subTable,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_CHOICE: {
                const bdem_Choice& subChoice = choiceArrayItem.theChoice();

                if (!bdem_SchemaAggregateUtil::isChoiceConformant(
                                                            subChoice,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_CHOICE_ARRAY: {
                const bdem_ChoiceArray& subChoiceArray =
                                              choiceArrayItem.theChoiceArray();

                if (!bdem_SchemaAggregateUtil::isChoiceArrayConformant(
                                                            subChoiceArray,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              default: {
                BSLS_ASSERT("record constraint on non-aggregate" && 0);
              } break;
            }
        }
        else if (eConstraintPtr) {
            BSLS_ASSERT(bdem_EnumerationDef::canHaveEnumConstraint(type));

            switch (type) {
              case bdem_ElemType::BDEM_INT: {
                if (!bdem_SchemaEnumerationUtil::isConformant(
                                          choiceArrayItem.selection().theInt(),
                                          *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_STRING: {
                if (!bdem_SchemaEnumerationUtil::isConformant(
                                       choiceArrayItem.selection().theString(),
                                       *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_INT_ARRAY: {
                if (!bdem_SchemaEnumerationUtil::isConformant(
                                     choiceArrayItem.selection().theIntArray(),
                                     *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_STRING_ARRAY: {
                if (!bdem_SchemaEnumerationUtil::isConformant(
                                  choiceArrayItem.selection().theStringArray(),
                                  *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              default: {
                BSLS_ASSERT("enum constraint on non-enum" && 0);
              }
            }
        }
    }

    return true;
}

static
bool isSelectionDataDeepConformant(const bdem_ChoiceArrayItem& choiceArrayItem,
                                   const bdem_RecordDef&       record)
    // Return 'true' if the specified 'choiceArrayItem' is deep conformant to
    // the specified 'record', and 'false' otherwise.  The behavior is
    // undefined unless 'record' specifies a choice type, and the number and
    // types in the types catalog of 'choiceArrayItem' match the
    // number and types of the corresponding fields in 'record'.  Refer to the
    // component-level documentation for further information on how a
    // 'bdem_ChoiceArrayItem' is deep conformant to a 'bdem_RecordDef'.
{
    const int selector = choiceArrayItem.selector();

    if (0 == record.numFields() || -1 == selector) {
        return true;                                                  // RETURN
    }

    const bdem_FieldDef&       field          = record.field(selector);
    const bdem_ConstElemRef&   elemRef        = choiceArrayItem.selection();
    const bdem_ElemType::Type  type           = field.elemType();
    const bdem_RecordDef      *rConstraintPtr = field.recordConstraint();
    const bdem_EnumerationDef *eConstraintPtr = field.enumerationConstraint();

    BSLS_ASSERT(type == elemRef.type());
    BSLS_ASSERT(!rConstraintPtr || !eConstraintPtr);

    if (elemRef.isNull()) {
        return ! rConstraintPtr || ! bdem_ElemType::isAggregateType(type) ||
                                                            field.isNullable();
                                                                      // RETURN
    }
    else {
        if (rConstraintPtr) {
            BSLS_ASSERT(bdem_ElemType::isAggregateType(type));

            switch (type) {
              case bdem_ElemType::BDEM_LIST: {
                const bdem_List& subList = choiceArrayItem.theList();

                if (!bdem_SchemaAggregateUtil::isListDeepConformant(
                                                            subList,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_TABLE: {
                const bdem_Table& subTable = choiceArrayItem.theTable();

                if (!bdem_SchemaAggregateUtil::isTableDeepConformant(
                                                            subTable,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_CHOICE: {
                const bdem_Choice& subChoice = choiceArrayItem.theChoice();

                if (!bdem_SchemaAggregateUtil::isChoiceDeepConformant(
                                                            subChoice,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_CHOICE_ARRAY: {
                const bdem_ChoiceArray& subChoiceArray =
                                              choiceArrayItem.theChoiceArray();

                if (!bdem_SchemaAggregateUtil::isChoiceArrayDeepConformant(
                                                            subChoiceArray,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              default: {
                BSLS_ASSERT("record constraint on non-aggregate" && 0);
              } break;
            }
        }
        else if (eConstraintPtr) {
            BSLS_ASSERT(bdem_EnumerationDef::canHaveEnumConstraint(type));

            switch (type) {
              case bdem_ElemType::BDEM_INT: {
                if (!bdem_SchemaEnumerationUtil::isConformant(
                                          choiceArrayItem.selection().theInt(),
                                          *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_STRING: {
                if (!bdem_SchemaEnumerationUtil::isConformant(
                                       choiceArrayItem.selection().theString(),
                                       *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_INT_ARRAY: {
                if (!bdem_SchemaEnumerationUtil::isConformant(
                                     choiceArrayItem.selection().theIntArray(),
                                     *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdem_ElemType::BDEM_STRING_ARRAY: {
                  if (!bdem_SchemaEnumerationUtil::isConformant(
                                  choiceArrayItem.selection().theStringArray(),
                                  *eConstraintPtr)) {
                    return false;                                     // RETURN
                  }
              } break;
              default: {
                BSLS_ASSERT("enum constraint on non-enum" && 0);
              }
            }
        }
    }

    return true;
}

typedef bsl::pair<int, int> RecursionGuard;
    // An object of the 'RecursionGuard' 'bsl::pair' is used to prevent
    // infinite recursion on "misbehaving" (necessarily recursive) schemas
    // during potentially recursive initialization of rows and lists.
    // 'RecursionGuard.first' denotes the current depth of recursion, and
    // 'RecursionGuard.second' the maximum depth of recursion.  Note that a
    // well-behaved schema prevents run-away recursion by marking appropriate
    // fields of aggregate types to be nullable.

static
void initListDeepImp(bdem_List             *,
                     const bdem_RecordDef&  ,
                     RecursionGuard         );
    // Forward declaration.

static
void initRowDeepImp(bdem_Row              *rowPtr,
                    const bdem_RecordDef&  record,
                    RecursionGuard         recursionGuard)
    // Initialize the row specified by 'rowPtr' to contain data deep
    // conformant to the specified 'record'.  Use the specified
    // 'recursionGuard' to ensure that malformed schemas are rejected.  Note
    // that any scalar elements to having default values are initialized to
    // those values and unconstrained aggregates, nullable aggregates, and
    // elements without default values are initialized to null.
{
    BSLS_ASSERT(rowPtr);
    BSLS_ASSERT(bdem_RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    bdem_SchemaAggregateUtil::initRowShallow(rowPtr, record);

    const int numFields = record.numFields();

    // Note that enumeration constraints are ignored here.

    for (int i = 0; i < numFields; ++i) {
        const bdem_FieldDef&      field   = record.field(i);
        const bdem_ElemRef&       elemRef = (*rowPtr)[i];
        const bdem_ElemType::Type type    = field.elemType();
        BSLS_ASSERT(type == elemRef.type());

        // Note that elements have already been set to their 'defaultValue' or
        // null by the call to 'initRowShallow' above.

        const bdem_RecordDef *rConstraintPtr = field.recordConstraint();

        if (rConstraintPtr && !field.isNullable()) {
            BSLS_ASSERT(bdem_ElemType::isAggregateType(type));

            switch (type) {
              case bdem_ElemType::BDEM_LIST: {
                if (rConstraintPtr != &record) {
                    RecursionGuard guard(recursionGuard);
                    if (++guard.first >= guard.second) {
                        BSLS_ASSERT("Broken recursive schema." && 0);
                    }
                    initListDeepImp(&elemRef.theModifiableList(),
                                    *rConstraintPtr,
                                    guard);
                }
              } break;
              case bdem_ElemType::BDEM_TABLE: {
                bdem_SchemaAggregateUtil::initTable(
                                                 &elemRef.theModifiableTable(),
                                                 *rConstraintPtr);
              } break;
              case bdem_ElemType::BDEM_CHOICE: {
                bdem_SchemaAggregateUtil::initChoice(
                                                &elemRef.theModifiableChoice(),
                                                *rConstraintPtr);
              } break;
              case bdem_ElemType::BDEM_CHOICE_ARRAY: {
                bdem_SchemaAggregateUtil::initChoiceArray(
                                           &elemRef.theModifiableChoiceArray(),
                                           *rConstraintPtr);
              } break;
              default: {
                BSLS_ASSERT("record constraint on non-aggregate" && 0);
              } break;
            }
        }
    }
}

static
void initListDeepImp(bdem_List             *listPtr,
                     const bdem_RecordDef&  record,
                     RecursionGuard         recursionGuard)
    // Initialize the list specified by 'listPtr' to contain data deep
    // conformant to the specified 'record'.  Use the specified
    // 'recursionGuard' to ensure that malformed schemas are rejected.  Note
    // that any scalar elements to having default values are initialized to
    // those values and unconstrained aggregates, nullable aggregates, and
    // elements without default values are initialized to null.
{
    BSLS_ASSERT(listPtr);
    BSLS_ASSERT(bdem_RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    bdem_SchemaAggregateUtil::initListAllNull(listPtr, record);
    initRowDeepImp(&listPtr->row(), record, recursionGuard);
}

                        // -------------------------------
                        // struct bdem_SchemaAggregateUtil
                        // -------------------------------

// CLASS METHODS
bool bdem_SchemaAggregateUtil::canSatisfyRecord(const bdem_Row&       row,
                                                const bdem_RecordDef& record)
{
    if (bdem_RecordDef::BDEM_SEQUENCE_RECORD != record.recordType()) {
        return false;                                                 // RETURN
    }

    const int numFields = record.numFields();

    if (numFields > row.length()) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < numFields; ++i) {
        if (record.field(i).elemType() != row.elemType(i)) {
            return false;                                             // RETURN
        }
    }

    return canRowDataSatisfyRecord(row, record);
}

bool bdem_SchemaAggregateUtil::canSatisfyRecord(const bdem_Table&     table,
                                                const bdem_RecordDef& record)
{
    if (bdem_RecordDef::BDEM_SEQUENCE_RECORD != record.recordType()) {
        return false;                                                 // RETURN
    }

    const int numFields = record.numFields();

    if (numFields > table.numColumns()) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < numFields; ++i) {
        if (record.field(i).elemType() != table.columnType(i)) {
            return false;                                             // RETURN
        }
    }

    const int numRows = table.numRows();

    for (int i = 0; i < numRows; ++i) {
        if (!canRowDataSatisfyRecord(table[i], record)) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bool bdem_SchemaAggregateUtil::canSatisfyRecord(
                                           const bdem_ChoiceArray& choiceArray,
                                           const bdem_RecordDef&   record)
{
    if (bdem_RecordDef::BDEM_CHOICE_RECORD != record.recordType()) {
        return false;                                                 // RETURN
    }

    const int numFields = record.numFields();

    if (numFields > choiceArray.numSelections()) {
        return false;                                                 // RETURN
    }

    if (0 == numFields) {
        return true;                                                  // RETURN
    }

    for (int i = 0; i < numFields; ++i) {
        if (record.field(i).elemType() != choiceArray.selectionType(i)) {
            return false;                                             // RETURN
        }
    }

    const int numItems = choiceArray.length();

    for (int i = 0; i < numItems; ++i) {
        if (!canSelectionDataSatisfyRecord(choiceArray[i], record)) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bool bdem_SchemaAggregateUtil::canSatisfyRecord(
                                   const bdem_ChoiceArrayItem& choiceArrayItem,
                                   const bdem_RecordDef&       record)
{
    if (bdem_RecordDef::BDEM_CHOICE_RECORD != record.recordType()) {
        return false;                                                 // RETURN
    }

    const int numFields = record.numFields();

    if (numFields > choiceArrayItem.numSelections()) {
        return false;                                                 // RETURN
    }

    if (0 == numFields) {
        return true;                                                  // RETURN
    }

    for (int i = 0; i < numFields; ++i) {
        if (record.field(i).elemType() != choiceArrayItem.selectionType(i)) {
            return false;                                             // RETURN
        }
    }

    return canSelectionDataSatisfyRecord(choiceArrayItem, record);
}

void bdem_SchemaAggregateUtil::initRowShallow(bdem_Row              *row,
                                              const bdem_RecordDef&  record)
{
    BSLS_ASSERT(row);
    BSLS_ASSERT(bdem_RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    const int numFields = record.numFields();
    BSLS_ASSERT(numFields == row->length());

    for (int i = 0; i < numFields; ++i) {
        const bdem_FieldDef&      fieldDef = record.field(i);
        const bdem_ElemRef&       elemRef  = (*row)[i];
        const bdem_ElemType::Type type     = fieldDef.elemType();

        BSLS_ASSERT(type == elemRef.type());

        if (bdem_ElemType::isScalarType(type) && fieldDef.hasDefaultValue()) {
            elemRef.replaceValue(fieldDef.defaultValue());
        }
        else {
            elemRef.makeNull();
        }
    }
}

void bdem_SchemaAggregateUtil::initRowDeep(bdem_Row              *row,
                                           const bdem_RecordDef&  record)
{
    BSLS_ASSERT(row);
    BSLS_ASSERT(bdem_RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    RecursionGuard recursionGuard(0, record.schema().numRecords());
    initRowDeepImp(row, record, recursionGuard);
}

void bdem_SchemaAggregateUtil::initListAllNull(bdem_List             *list,
                                               const bdem_RecordDef&  record)
{
    BSLS_ASSERT(list);
    BSLS_ASSERT(bdem_RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    const int numFields = record.numFields();
    bsl::vector<bdem_ElemType::Type> elemTypes;
    elemTypes.reserve(numFields);

    for (int i = 0; i < numFields; ++i) {
        elemTypes.push_back(record.field(i).elemType());
    }

    list->reset(elemTypes);
}

void bdem_SchemaAggregateUtil::initListShallow(bdem_List             *list,
                                               const bdem_RecordDef&  record)
{
    BSLS_ASSERT(list);
    BSLS_ASSERT(bdem_RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    // init types
    bdem_SchemaAggregateUtil::initListAllNull(list, record);

    // init default values
    bdem_SchemaAggregateUtil::initRowShallow(&list->row(), record);
}

void bdem_SchemaAggregateUtil::initListDeep(bdem_List             *list,
                                            const bdem_RecordDef&  record)
{
    BSLS_ASSERT(list);
    BSLS_ASSERT(bdem_RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    RecursionGuard recursionGuard(0, record.schema().numRecords());
    initListDeepImp(list, record, recursionGuard);
}

void bdem_SchemaAggregateUtil::initTable(bdem_Table            *table,
                                         const bdem_RecordDef&  record)
{
    BSLS_ASSERT(table);
    BSLS_ASSERT(bdem_RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    const int numFields = record.numFields();
    bsl::vector<bdem_ElemType::Type> elemTypes;
    elemTypes.reserve(numFields);

    for (int i = 0; i < numFields; ++i) {
        elemTypes.push_back(record.field(i).elemType());
    }

    table->reset(elemTypes);
}

void bdem_SchemaAggregateUtil::initChoice(bdem_Choice           *choice,
                                          const bdem_RecordDef&  record)
{
    BSLS_ASSERT(choice);
    BSLS_ASSERT(bdem_RecordDef::BDEM_CHOICE_RECORD == record.recordType());

    const int numFields = record.numFields();
    bsl::vector<bdem_ElemType::Type> elemTypes;
    elemTypes.reserve(numFields);

    for (int i = 0; i < numFields; ++i) {
        elemTypes.push_back(record.field(i).elemType());
    }

    choice->reset(elemTypes);
}

void
bdem_SchemaAggregateUtil::initChoiceArray(bdem_ChoiceArray     *choiceArray,
                                          const bdem_RecordDef& record)
{
    BSLS_ASSERT(choiceArray);
    BSLS_ASSERT(bdem_RecordDef::BDEM_CHOICE_RECORD == record.recordType());

    const int numFields = record.numFields();
    bsl::vector<bdem_ElemType::Type> elemTypes;
    elemTypes.reserve(numFields);

    for (int i = 0; i < numFields; ++i) {
        elemTypes.push_back(record.field(i).elemType());
    }

    choiceArray->reset(elemTypes);
}

bool bdem_SchemaAggregateUtil::isRowShallowConformant(
                                                  const bdem_Row&       row,
                                                  const bdem_RecordDef& record)
{
    if (bdem_RecordDef::BDEM_SEQUENCE_RECORD != record.recordType()) {
        return false;                                                 // RETURN
    }

    const int numFields = record.numFields();

    if (numFields != row.length()) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < numFields; ++i) {
        if (row.elemType(i) != record.field(i).elemType()) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bool bdem_SchemaAggregateUtil::isRowConformant(const bdem_Row&       row,
                                               const bdem_RecordDef& record)
{
    if (!bdem_SchemaAggregateUtil::isRowShallowConformant(row, record)) {
        return false;                                                 // RETURN
    }

    return isRowDataConformant(row, record);
}

bool bdem_SchemaAggregateUtil::isRowDeepConformant(
                                                  const bdem_Row&       row,
                                                  const bdem_RecordDef& record)
{
    if (!bdem_SchemaAggregateUtil::isRowShallowConformant(row, record)) {
        return false;                                                 // RETURN
    }

    return isRowDataDeepConformant(row, record);
}

bool bdem_SchemaAggregateUtil::isTableShallowConformant(
                                                  const bdem_Table&     table,
                                                  const bdem_RecordDef& record)
{
    if (bdem_RecordDef::BDEM_SEQUENCE_RECORD != record.recordType()) {
        return false;                                                 // RETURN
    }

    const int numColumns = table.numColumns();

    if (record.numFields() != numColumns) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < numColumns; ++i) {
        if (table.columnType(i) != record.field(i).elemType()) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bool bdem_SchemaAggregateUtil::isTableConformant(const bdem_Table&     table,
                                                 const bdem_RecordDef& record)
{
    if (!bdem_SchemaAggregateUtil::isTableShallowConformant(table, record)) {
        return false;                                                 // RETURN
    }

    const int numRows = table.numRows();

    for (int i = 0; i < numRows; ++i) {
        if (!isRowDataConformant(table[i], record)) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bool bdem_SchemaAggregateUtil::isTableDeepConformant(
                                                  const bdem_Table&     table,
                                                  const bdem_RecordDef& record)
{
    if (!bdem_SchemaAggregateUtil::isTableShallowConformant(table, record)) {
        return false;                                                 // RETURN
    }

    const int numRows = table.numRows();

    for (int i = 0; i < numRows; ++i) {
        if (!isRowDataDeepConformant(table[i], record)) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bool bdem_SchemaAggregateUtil::isChoiceArrayItemShallowConformant(
                                   const bdem_ChoiceArrayItem& choiceArrayItem,
                                   const bdem_RecordDef&       record)
{
    if (bdem_RecordDef::BDEM_CHOICE_RECORD != record.recordType()) {
        return false;                                                 // RETURN
    }

    const int numFields = record.numFields();

    if (numFields != choiceArrayItem.numSelections()) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < numFields; ++i) {
        if (choiceArrayItem.selectionType(i) != record.field(i).elemType()) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bool bdem_SchemaAggregateUtil::isChoiceArrayItemConformant(
                                   const bdem_ChoiceArrayItem& choiceArrayItem,
                                   const bdem_RecordDef&       record)
{
    if (!bdem_SchemaAggregateUtil::isChoiceArrayItemShallowConformant(
                                                               choiceArrayItem,
                                                               record)) {
        return false;                                                 // RETURN
    }

    return isSelectionDataConformant(choiceArrayItem, record);
}

bool bdem_SchemaAggregateUtil::isChoiceArrayItemDeepConformant(
                                   const bdem_ChoiceArrayItem& choiceArrayItem,
                                   const bdem_RecordDef&       record)
{
    if (!bdem_SchemaAggregateUtil::isChoiceArrayItemShallowConformant(
                                                               choiceArrayItem,
                                                               record)) {
        return false;                                                 // RETURN
    }

    return isSelectionDataDeepConformant(choiceArrayItem, record);
}

bool bdem_SchemaAggregateUtil::isChoiceArrayShallowConformant(
                                           const bdem_ChoiceArray& choiceArray,
                                           const bdem_RecordDef&   record)
{
    if (bdem_RecordDef::BDEM_CHOICE_RECORD != record.recordType()) {
        return false;                                                 // RETURN
    }

    const int numFields = record.numFields();

    if (numFields != choiceArray.numSelections()) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < numFields; ++i) {
        if (choiceArray.selectionType(i) != record.field(i).elemType()) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bool bdem_SchemaAggregateUtil::isChoiceArrayConformant(
                                           const bdem_ChoiceArray& choiceArray,
                                           const bdem_RecordDef&   record)
{
    if (!bdem_SchemaAggregateUtil::isChoiceArrayShallowConformant(choiceArray,
                                                                  record)) {
        return false;                                                 // RETURN
    }

    const int numItems = choiceArray.length();

    for (int i = 0; i < numItems; ++i) {
        if (!isSelectionDataConformant(choiceArray[i], record)) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bool bdem_SchemaAggregateUtil::isChoiceArrayDeepConformant(
                                           const bdem_ChoiceArray& choiceArray,
                                           const bdem_RecordDef&   record)
{
    if (!bdem_SchemaAggregateUtil::isChoiceArrayShallowConformant(choiceArray,
                                                                  record)) {
        return false;                                                 // RETURN
    }

    const int numItems = choiceArray.length();

    for (int i = 0; i < numItems; ++i) {
        if (!isSelectionDataDeepConformant(choiceArray[i], record)) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bsl::ostream&
bdem_SchemaAggregateUtil::print(bsl::ostream&             stream,
                                const bdem_ConstElemRef&  element,
                                const bdem_RecordDef     *constraint,
                                int                       level,
                                int                       spacesPerLevel)
{
    if (element.isNull()) {
        stream << "NULL";
        return stream;
    }

    if (constraint) {
        const bdem_ElemType::Type type = element.type();
        BSLS_ASSERT(bdem_ElemType::isAggregateType(type));

        switch (type) {
          case bdem_ElemType::BDEM_ROW: {
            return print(stream,
                         *(const bdem_Row*)element.data(),
                         *constraint,
                         level,
                         spacesPerLevel);
          }
          case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM: {
            return print(stream,
                         *(const bdem_ChoiceArrayItem*)element.data(),
                         *constraint,
                         level,
                         spacesPerLevel);
          }
          case bdem_ElemType::BDEM_LIST: {
            return print(stream,
                         element.theList(),
                         *constraint,
                         level,
                         spacesPerLevel);
          }
          case bdem_ElemType::BDEM_CHOICE: {
            return print(stream,
                         element.theChoice(),
                         *constraint,
                         level,
                         spacesPerLevel);
          }
          case bdem_ElemType::BDEM_TABLE: {
            return print(stream,
                         element.theTable(),
                         *constraint,
                         level,
                         spacesPerLevel);
          }
          case bdem_ElemType::BDEM_CHOICE_ARRAY: {
            return print(stream,
                         element.theChoiceArray(),
                         *constraint,
                         level,
                         spacesPerLevel);
          }
          default: {
            BSLS_ASSERT("record constraint on non-aggregate" && 0);
          } break;
        }
    }

    return element.print(stream, level, spacesPerLevel);
}

bsl::ostream&
bdem_SchemaAggregateUtil::print(bsl::ostream&         stream,
                                const bdem_Row&       row,
                                const bdem_RecordDef& record,
                                int                   level,
                                int                   spacesPerLevel)
{
    if (stream.bad()) {
        return stream;
    }

    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    const char *endOfField = "\n";
    if (spacesPerLevel < 0) {
        // Suppress newlines and use single-space indentation.
        endOfField = "";
        level = 1;
        levelPlus1 = 1;
        spacesPerLevel = -1;
    }

    if (0 == row.length() && 0 != record.numFields()) {
        return stream << "NULL" << endOfField;
    }

    stream << '[' << endOfField;

    for (int i = 0; i < row.length(); ++i) {
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        const char *fieldName = record.fieldName(i);
        stream << (fieldName ? fieldName : "(anonymous)")  << " = ";

        const bdem_FieldDef& fieldDef = record.field(i);
        bdem_ConstElemRef    elemRef  = row[i];

        if (elemRef.isNull()) {
            stream << "NULL" << endOfField;
        }
        else {
            // If 'spacesPerLevel >= 0', then following 'print' will end with a
            // newline.
            print(stream,
                  elemRef,
                  fieldDef.recordConstraint(),
                  -levelPlus1,
                  spacesPerLevel);
        }
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << ']' << endOfField;

    return stream << bsl::flush;
}

bsl::ostream&
bdem_SchemaAggregateUtil::print(bsl::ostream&         stream,
                                const bdem_List&      list,
                                const bdem_RecordDef& record,
                                int                   level,
                                int                   spacesPerLevel)
{
    return print(stream, list.row(), record, level, spacesPerLevel);
}

bsl::ostream&
bdem_SchemaAggregateUtil::print(bsl::ostream&               stream,
                                const bdem_ChoiceArrayItem& item,
                                const bdem_RecordDef&       record,
                                int                         level,
                                int                         spacesPerLevel)
{
    if (stream.bad()) {
        return stream;
    }

    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    const char *endOfField = "\n";
    if (spacesPerLevel < 0) {
        // Suppress newlines and use single-space indentation.

        endOfField = "";
        level = 1;
        levelPlus1 = 1;
        spacesPerLevel = -1;
    }

    if (0 == item.numSelections() && 0 != record.numFields()) {
        return stream << "NULL" << endOfField;
    }

    stream << '[' << endOfField;
    bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);

    if (-1 == item.selector()) {
        stream << "NO SELECTION" << endOfField;
    }
    else {
        const char *fieldName = record.fieldName(item.selector());
        stream << (fieldName ? fieldName : "(anonymous)")  << " = ";

        const bdem_FieldDef& fieldDef = record.field(item.selector());
        bdem_ConstElemRef    elemRef  = item.selection();

        if (elemRef.isNull()) {
            stream << "NULL" << endOfField;
        }
        else {
            print(stream,
                  elemRef,
                  fieldDef.recordConstraint(),
                  -levelPlus1,
                  spacesPerLevel);
        }
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << ']' << endOfField;

    return stream << bsl::flush;
}

bsl::ostream&
bdem_SchemaAggregateUtil::print(bsl::ostream&         stream,
                                const bdem_Choice&    choice,
                                const bdem_RecordDef& record,
                                int                   level,
                                int                   spacesPerLevel)
{
    return print(stream, choice.item(), record, level, spacesPerLevel);
}

bsl::ostream&
bdem_SchemaAggregateUtil::print(bsl::ostream&         stream,
                                const bdem_Table&     table,
                                const bdem_RecordDef& record,
                                int                   level,
                                int                   spacesPerLevel)
{
    if (stream.bad()) {
        return stream;
    }

    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    const char *endOfField = "\n";
    if (spacesPerLevel < 0) {
        // Suppress newlines and use single-space indentation.

        endOfField = "";
        level = 1;
        levelPlus1 = 1;
        spacesPerLevel = -1;
    }

    if (0 == table.numColumns() && 0 != record.numFields()) {
        return stream << "NULL" << endOfField;
    }

    stream << '[' << endOfField;

    for (int i = 0; i < table.numRows(); ++i) {
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        print(stream, table[i], record, -levelPlus1, spacesPerLevel);
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << ']' << endOfField;

    return stream << bsl::flush;
}

bsl::ostream&
bdem_SchemaAggregateUtil::print(bsl::ostream&           stream,
                                const bdem_ChoiceArray& array,
                                const bdem_RecordDef&   record,
                                int                     level,
                                int                     spacesPerLevel)
{
    if (stream.bad()) {
        return stream;
    }

    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    const char *endOfField = "\n";
    if (spacesPerLevel < 0) {
        // Suppress newlines and use single-space indentation.

        endOfField = "";
        level = 1;
        levelPlus1 = 1;
        spacesPerLevel = -1;
    }

    if (0 == array.numSelections() && 0 != record.numFields()) {
        return stream << "NULL" << endOfField;
    }

    stream << '[' << endOfField;

    for (int i = 0; i < array.length(); ++i) {
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        print(stream, array[i], record, -levelPlus1, spacesPerLevel);
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << ']' << endOfField;

    return stream << bsl::flush;
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
