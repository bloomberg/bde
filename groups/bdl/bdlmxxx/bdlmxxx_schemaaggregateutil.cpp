// bdlmxxx_schemaaggregateutil.cpp                                       -*-C++-*-
#include <bdlmxxx_schemaaggregateutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_schemaaggregateutil_cpp,"$Id$ $CSID$")

#include <bdlmxxx_aggregate.h>
#include <bdlmxxx_elemtype.h>
#include <bdlmxxx_schema.h>
#include <bdlmxxx_schemaenumerationutil.h>

#include <bsls_assert.h>

#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS
static
bool canRowDataSatisfyRecord(const bdlmxxx::Row& row, const bdlmxxx::RecordDef& record)
    // Return 'true' if the specified 'row' can satisfy the specified
    // 'record', and 'false' otherwise.  The behavior is undefined unless
    // 'record' specifies a sequence type and the types of the initial
    // 'record.numFields()' fields in 'row' match the corresponding types in
    // 'record'.  Refer to the component-level documentation for further
    // information on how a 'bdlmxxx::Row' can satisfy a 'bdlmxxx::RecordDef'.
{
    const int numFields = record.numFields();

    for (int i = 0; i < numFields; ++i) {
        const bdlmxxx::FieldDef&     field   = record.field(i);
        const bdlmxxx::ConstElemRef& elemRef = row[i];

        if (! elemRef.isNull()) {
            const bdlmxxx::ElemType::Type  type           = field.elemType();
            const bdlmxxx::RecordDef      *rConstraintPtr =
                                                      field.recordConstraint();
            const bdlmxxx::EnumerationDef *eConstraintPtr =
                                                 field.enumerationConstraint();
            BSLS_ASSERT(type == elemRef.type());

            if (rConstraintPtr) {
                BSLS_ASSERT(!eConstraintPtr);
                BSLS_ASSERT(bdlmxxx::ElemType::isAggregateType(type));

                switch (type) {
                  case bdlmxxx::ElemType::BDEM_LIST: {
                    const bdlmxxx::List& subList = elemRef.theList();

                    if (!bdlmxxx::SchemaAggregateUtil::canSatisfyRecord(
                                                            subList,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_TABLE: {
                    const bdlmxxx::Table& subTable = elemRef.theTable();

                    if (!bdlmxxx::SchemaAggregateUtil::canSatisfyRecord(
                                                            subTable,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_CHOICE: {
                    const bdlmxxx::Choice& subChoice = elemRef.theChoice();

                    if (!bdlmxxx::SchemaAggregateUtil::canSatisfyRecord(
                                                            subChoice,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
                    const bdlmxxx::ChoiceArray& subChoiceArray =
                                                      elemRef.theChoiceArray();

                    if (!bdlmxxx::SchemaAggregateUtil::canSatisfyRecord(
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
                BSLS_ASSERT(bdlmxxx::EnumerationDef::canHaveEnumConstraint(type));

                switch (type) {
                  case bdlmxxx::ElemType::BDEM_INT: {
                    if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                                            elemRef.theInt(),
                                                            *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_STRING: {
                    if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                                           elemRef.theString(),
                                                           *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_INT_ARRAY: {
                    if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                                         elemRef.theIntArray(),
                                                         *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_STRING_ARRAY: {
                    if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
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
bool isRowDataConformant(const bdlmxxx::Row& row, const bdlmxxx::RecordDef& record)
    // Return 'true' if the specified 'row' is conformant to the specified
    // 'record', and 'false' otherwise.  The behavior is undefined unless
    // 'record' specifies a sequence type, and the number and types of the
    // elements in 'row' match those of the corresponding fields in 'record'.
    // Refer to the component-level documentation for further information on
    // how a 'bdlmxxx::Row' is conformant to a 'bdlmxxx::RecordDef'.
{
    const int numFields = record.numFields();

    for (int i = 0; i < numFields; ++i) {
        const bdlmxxx::FieldDef&     field   = record.field(i);
        const bdlmxxx::ConstElemRef& elemRef = row[i];

        if (! elemRef.isNull()) {
            const bdlmxxx::ElemType::Type  type           = field.elemType();
            const bdlmxxx::RecordDef      *rConstraintPtr =
                                                      field.recordConstraint();
            const bdlmxxx::EnumerationDef *eConstraintPtr =
                                                 field.enumerationConstraint();
            BSLS_ASSERT(type == elemRef.type());

            if (rConstraintPtr) {
                BSLS_ASSERT(!eConstraintPtr);
                BSLS_ASSERT(bdlmxxx::ElemType::isAggregateType(type));

                switch (type) {
                  case bdlmxxx::ElemType::BDEM_LIST: {
                    const bdlmxxx::List& subList = elemRef.theList();

                    if (!bdlmxxx::SchemaAggregateUtil::isListConformant(
                                                            subList,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_TABLE: {
                    const bdlmxxx::Table& subTable = elemRef.theTable();

                    if (!bdlmxxx::SchemaAggregateUtil::isTableConformant(
                                                            subTable,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_CHOICE: {
                    const bdlmxxx::Choice& subChoice = elemRef.theChoice();

                    if (!bdlmxxx::SchemaAggregateUtil::isChoiceConformant(
                                                            subChoice,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
                    const bdlmxxx::ChoiceArray& subChoiceArray =
                                                      elemRef.theChoiceArray();

                    if (!bdlmxxx::SchemaAggregateUtil::isChoiceArrayConformant(
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
                BSLS_ASSERT(bdlmxxx::EnumerationDef::canHaveEnumConstraint(type));

                switch (type) {
                  case bdlmxxx::ElemType::BDEM_INT: {
                    if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                                            elemRef.theInt(),
                                                            *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_STRING: {
                    if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                                           elemRef.theString(),
                                                           *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_INT_ARRAY: {
                    if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                                         elemRef.theIntArray(),
                                                         *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_STRING_ARRAY: {
                    if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
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
bool isRowDataDeepConformant(const bdlmxxx::Row& row, const bdlmxxx::RecordDef& record)
    // Return 'true' if the specified 'row' is deep conformant to the
    // specified 'record', and 'false' otherwise.  The behavior is undefined
    // unless 'record' specifies a sequence type, and the number and types of
    // the elements in 'row' exactly match the number and types of the
    // corresponding fields in 'record'.  Refer to the component-level
    // documentation for further information on how a 'bdlmxxx::Row' is deep
    // conformant to a 'bdlmxxx::RecordDef'.
{
    const int numFields = record.numFields();

    for (int i = 0; i < numFields; ++i) {
        const bdlmxxx::FieldDef&       field          = record.field(i);
        const bdlmxxx::ConstElemRef&   elemRef        = row[i];
        const bdlmxxx::ElemType::Type  type           = field.elemType();
        const bdlmxxx::RecordDef      *rConstraintPtr = field.recordConstraint();
        const bdlmxxx::EnumerationDef *eConstraintPtr =
                                                 field.enumerationConstraint();

        BSLS_ASSERT(type == elemRef.type());
        BSLS_ASSERT(!rConstraintPtr || !eConstraintPtr);

        if (elemRef.isNull()) {
            if (rConstraintPtr && bdlmxxx::ElemType::isAggregateType(type) &&
                                                        ! field.isNullable()) {
                return false;                                         // RETURN
            }
        }
        else {
            if (rConstraintPtr) {
                BSLS_ASSERT(bdlmxxx::ElemType::isAggregateType(type));

                switch (type) {
                  case bdlmxxx::ElemType::BDEM_LIST: {
                    const bdlmxxx::List& subList = elemRef.theList();

                    if (!bdlmxxx::SchemaAggregateUtil::isListDeepConformant(
                                                            subList,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_TABLE: {
                    const bdlmxxx::Table& subTable = elemRef.theTable();

                    if (!bdlmxxx::SchemaAggregateUtil::isTableDeepConformant(
                                                            subTable,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_CHOICE: {
                    const bdlmxxx::Choice& subChoice = elemRef.theChoice();

                    if (!bdlmxxx::SchemaAggregateUtil::isChoiceDeepConformant(
                                                            subChoice,
                                                            *rConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
                    const bdlmxxx::ChoiceArray& subChoiceArray =
                                                      elemRef.theChoiceArray();

                    if (!bdlmxxx::SchemaAggregateUtil::isChoiceArrayDeepConformant(
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
                BSLS_ASSERT(bdlmxxx::EnumerationDef::canHaveEnumConstraint(type));

                switch (type) {
                  case bdlmxxx::ElemType::BDEM_INT: {
                    if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                                            elemRef.theInt(),
                                                            *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_STRING: {
                    if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                                           elemRef.theString(),
                                                           *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_INT_ARRAY: {
                    if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                                         elemRef.theIntArray(),
                                                         *eConstraintPtr)) {
                        return false;                                 // RETURN
                    }
                  } break;
                  case bdlmxxx::ElemType::BDEM_STRING_ARRAY: {
                    if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
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
bool canSelectionDataSatisfyRecord(const bdlmxxx::ChoiceArrayItem& choiceArrayItem,
                                   const bdlmxxx::RecordDef&       record)
    // Return 'true' if the specified 'choiceArrayItem' can satisfy the
    // specified 'record', and 'false' otherwise.  The behavior is undefined
    // unless 'record' specifies a choice type, and the initial
    // 'record.numFields()' types in the types catalog of 'choiceArrayItem'
    // match the corresponding types in 'record'.  Refer to the component-level
    // documentation for further information on how a 'bdlmxxx::ChoiceArrayItem'
    // can satisfy a 'bdlmxxx::RecordDef'.
{
    const int numFields = record.numFields();
    const int selector  = choiceArrayItem.selector();

    if (numFields <= selector) {
        return false;                                                 // RETURN
    }

    if (-1 == selector) {
        return true;                                                  // RETURN
    }

    const bdlmxxx::ConstElemRef&  elemRef = choiceArrayItem.selection();
    const bdlmxxx::FieldDef&      field   = record.field(selector);
    const bdlmxxx::ElemType::Type type    = field.elemType();
    BSLS_ASSERT(type == elemRef.type());

    if (elemRef.isNull()) {
        return true;                                                  // RETURN
    }
    else {
        const bdlmxxx::RecordDef      *rConstraintPtr = field.recordConstraint();
        const bdlmxxx::EnumerationDef *eConstraintPtr =
                                                 field.enumerationConstraint();

        if (rConstraintPtr) {
            BSLS_ASSERT(!eConstraintPtr);
            BSLS_ASSERT(bdlmxxx::ElemType::isAggregateType(type));

            switch (type) {
              case bdlmxxx::ElemType::BDEM_LIST: {
                const bdlmxxx::List& subList = choiceArrayItem.theList();

                if (!bdlmxxx::SchemaAggregateUtil::canSatisfyRecord(
                                                            subList,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_TABLE: {
                const bdlmxxx::Table& subTable = choiceArrayItem.theTable();

                if (!bdlmxxx::SchemaAggregateUtil::canSatisfyRecord(
                                                            subTable,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_CHOICE: {
                const bdlmxxx::Choice& subChoice = choiceArrayItem.theChoice();

                if (!bdlmxxx::SchemaAggregateUtil::canSatisfyRecord(
                                                            subChoice,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
                const bdlmxxx::ChoiceArray& subChoiceArray =
                                              choiceArrayItem.theChoiceArray();

                if (!bdlmxxx::SchemaAggregateUtil::canSatisfyRecord(
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
            BSLS_ASSERT(bdlmxxx::EnumerationDef::canHaveEnumConstraint(type));

            switch (type) {
              case bdlmxxx::ElemType::BDEM_INT: {
                if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                          choiceArrayItem.selection().theInt(),
                                          *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_STRING: {
                if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                       choiceArrayItem.selection().theString(),
                                       *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_INT_ARRAY: {
                if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                     choiceArrayItem.selection().theIntArray(),
                                     *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_STRING_ARRAY: {
                if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
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
bool isSelectionDataConformant(const bdlmxxx::ChoiceArrayItem& choiceArrayItem,
                               const bdlmxxx::RecordDef&       record)
    // Return 'true' if the specified 'choiceArrayItem' is conformant to the
    // specified 'record', and 'false' otherwise.  The behavior is undefined
    // unless 'record' specifies a choice type and the number and types in the
    // types catalog of 'choiceArrayItem' match the number and types of the
    // corresponding fields in 'record'.  Refer to the component-level
    // documentation for further information on how a 'bdlmxxx::ChoiceArrayItem' is
    // conformant to a 'bdlmxxx::RecordDef'.
{
    const int selector = choiceArrayItem.selector();

    if (0 == record.numFields() || -1 == selector) {
        return true;                                                  // RETURN
    }

    if (choiceArrayItem.selection().isNull()) {
        return true;                                                  // RETURN
    }
    else {
        const bdlmxxx::FieldDef&       field          = record.field(selector);
        const bdlmxxx::ElemType::Type  type           = field.elemType();
        const bdlmxxx::RecordDef      *rConstraintPtr = field.recordConstraint();
        const bdlmxxx::EnumerationDef *eConstraintPtr =
                                                 field.enumerationConstraint();
        BSLS_ASSERT(type == choiceArrayItem.selectionType());

        if (rConstraintPtr) {
            BSLS_ASSERT(!eConstraintPtr);
            BSLS_ASSERT(bdlmxxx::ElemType::isAggregateType(type));

            switch (type) {
              case bdlmxxx::ElemType::BDEM_LIST: {
                const bdlmxxx::List& subList = choiceArrayItem.theList();

                if (!bdlmxxx::SchemaAggregateUtil::isListConformant(
                                                            subList,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_TABLE: {
                const bdlmxxx::Table& subTable = choiceArrayItem.theTable();

                if (!bdlmxxx::SchemaAggregateUtil::isTableConformant(
                                                            subTable,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_CHOICE: {
                const bdlmxxx::Choice& subChoice = choiceArrayItem.theChoice();

                if (!bdlmxxx::SchemaAggregateUtil::isChoiceConformant(
                                                            subChoice,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
                const bdlmxxx::ChoiceArray& subChoiceArray =
                                              choiceArrayItem.theChoiceArray();

                if (!bdlmxxx::SchemaAggregateUtil::isChoiceArrayConformant(
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
            BSLS_ASSERT(bdlmxxx::EnumerationDef::canHaveEnumConstraint(type));

            switch (type) {
              case bdlmxxx::ElemType::BDEM_INT: {
                if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                          choiceArrayItem.selection().theInt(),
                                          *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_STRING: {
                if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                       choiceArrayItem.selection().theString(),
                                       *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_INT_ARRAY: {
                if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                     choiceArrayItem.selection().theIntArray(),
                                     *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_STRING_ARRAY: {
                if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
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
bool isSelectionDataDeepConformant(const bdlmxxx::ChoiceArrayItem& choiceArrayItem,
                                   const bdlmxxx::RecordDef&       record)
    // Return 'true' if the specified 'choiceArrayItem' is deep conformant to
    // the specified 'record', and 'false' otherwise.  The behavior is
    // undefined unless 'record' specifies a choice type, and the number and
    // types in the types catalog of 'choiceArrayItem' match the
    // number and types of the corresponding fields in 'record'.  Refer to the
    // component-level documentation for further information on how a
    // 'bdlmxxx::ChoiceArrayItem' is deep conformant to a 'bdlmxxx::RecordDef'.
{
    const int selector = choiceArrayItem.selector();

    if (0 == record.numFields() || -1 == selector) {
        return true;                                                  // RETURN
    }

    const bdlmxxx::FieldDef&       field          = record.field(selector);
    const bdlmxxx::ConstElemRef&   elemRef        = choiceArrayItem.selection();
    const bdlmxxx::ElemType::Type  type           = field.elemType();
    const bdlmxxx::RecordDef      *rConstraintPtr = field.recordConstraint();
    const bdlmxxx::EnumerationDef *eConstraintPtr = field.enumerationConstraint();

    BSLS_ASSERT(type == elemRef.type());
    BSLS_ASSERT(!rConstraintPtr || !eConstraintPtr);

    if (elemRef.isNull()) {
        return ! rConstraintPtr || ! bdlmxxx::ElemType::isAggregateType(type) ||
                                                            field.isNullable();
                                                                      // RETURN
    }
    else {
        if (rConstraintPtr) {
            BSLS_ASSERT(bdlmxxx::ElemType::isAggregateType(type));

            switch (type) {
              case bdlmxxx::ElemType::BDEM_LIST: {
                const bdlmxxx::List& subList = choiceArrayItem.theList();

                if (!bdlmxxx::SchemaAggregateUtil::isListDeepConformant(
                                                            subList,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_TABLE: {
                const bdlmxxx::Table& subTable = choiceArrayItem.theTable();

                if (!bdlmxxx::SchemaAggregateUtil::isTableDeepConformant(
                                                            subTable,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_CHOICE: {
                const bdlmxxx::Choice& subChoice = choiceArrayItem.theChoice();

                if (!bdlmxxx::SchemaAggregateUtil::isChoiceDeepConformant(
                                                            subChoice,
                                                            *rConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
                const bdlmxxx::ChoiceArray& subChoiceArray =
                                              choiceArrayItem.theChoiceArray();

                if (!bdlmxxx::SchemaAggregateUtil::isChoiceArrayDeepConformant(
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
            BSLS_ASSERT(bdlmxxx::EnumerationDef::canHaveEnumConstraint(type));

            switch (type) {
              case bdlmxxx::ElemType::BDEM_INT: {
                if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                          choiceArrayItem.selection().theInt(),
                                          *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_STRING: {
                if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                       choiceArrayItem.selection().theString(),
                                       *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_INT_ARRAY: {
                if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
                                     choiceArrayItem.selection().theIntArray(),
                                     *eConstraintPtr)) {
                    return false;                                     // RETURN
                }
              } break;
              case bdlmxxx::ElemType::BDEM_STRING_ARRAY: {
                  if (!bdlmxxx::SchemaEnumerationUtil::isConformant(
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
void initListDeepImp(bdlmxxx::List             *,
                     const bdlmxxx::RecordDef&  ,
                     RecursionGuard         );
    // Forward declaration.

static
void initRowDeepImp(bdlmxxx::Row              *rowPtr,
                    const bdlmxxx::RecordDef&  record,
                    RecursionGuard         recursionGuard)
    // Initialize the row specified by 'rowPtr' to contain data deep
    // conformant to the specified 'record'.  Use the specified
    // 'recursionGuard' to ensure that malformed schemas are rejected.  Note
    // that any scalar elements to having default values are initialized to
    // those values and unconstrained aggregates, nullable aggregates, and
    // elements without default values are initialized to null.
{
    BSLS_ASSERT(rowPtr);
    BSLS_ASSERT(bdlmxxx::RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    bdlmxxx::SchemaAggregateUtil::initRowShallow(rowPtr, record);

    const int numFields = record.numFields();

    // Note that enumeration constraints are ignored here.

    for (int i = 0; i < numFields; ++i) {
        const bdlmxxx::FieldDef&      field   = record.field(i);
        const bdlmxxx::ElemRef&       elemRef = (*rowPtr)[i];
        const bdlmxxx::ElemType::Type type    = field.elemType();
        BSLS_ASSERT(type == elemRef.type());

        // Note that elements have already been set to their 'defaultValue' or
        // null by the call to 'initRowShallow' above.

        const bdlmxxx::RecordDef *rConstraintPtr = field.recordConstraint();

        if (rConstraintPtr && !field.isNullable()) {
            BSLS_ASSERT(bdlmxxx::ElemType::isAggregateType(type));

            switch (type) {
              case bdlmxxx::ElemType::BDEM_LIST: {
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
              case bdlmxxx::ElemType::BDEM_TABLE: {
                bdlmxxx::SchemaAggregateUtil::initTable(
                                                 &elemRef.theModifiableTable(),
                                                 *rConstraintPtr);
              } break;
              case bdlmxxx::ElemType::BDEM_CHOICE: {
                bdlmxxx::SchemaAggregateUtil::initChoice(
                                                &elemRef.theModifiableChoice(),
                                                *rConstraintPtr);
              } break;
              case bdlmxxx::ElemType::BDEM_CHOICE_ARRAY: {
                bdlmxxx::SchemaAggregateUtil::initChoiceArray(
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
void initListDeepImp(bdlmxxx::List             *listPtr,
                     const bdlmxxx::RecordDef&  record,
                     RecursionGuard         recursionGuard)
    // Initialize the list specified by 'listPtr' to contain data deep
    // conformant to the specified 'record'.  Use the specified
    // 'recursionGuard' to ensure that malformed schemas are rejected.  Note
    // that any scalar elements to having default values are initialized to
    // those values and unconstrained aggregates, nullable aggregates, and
    // elements without default values are initialized to null.
{
    BSLS_ASSERT(listPtr);
    BSLS_ASSERT(bdlmxxx::RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    bdlmxxx::SchemaAggregateUtil::initListAllNull(listPtr, record);
    initRowDeepImp(&listPtr->row(), record, recursionGuard);
}

namespace bdlmxxx {
                        // -------------------------------
                        // struct SchemaAggregateUtil
                        // -------------------------------

// CLASS METHODS
bool SchemaAggregateUtil::canSatisfyRecord(const Row&       row,
                                                const RecordDef& record)
{
    if (RecordDef::BDEM_SEQUENCE_RECORD != record.recordType()) {
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

bool SchemaAggregateUtil::canSatisfyRecord(const Table&     table,
                                                const RecordDef& record)
{
    if (RecordDef::BDEM_SEQUENCE_RECORD != record.recordType()) {
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

bool SchemaAggregateUtil::canSatisfyRecord(
                                           const ChoiceArray& choiceArray,
                                           const RecordDef&   record)
{
    if (RecordDef::BDEM_CHOICE_RECORD != record.recordType()) {
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

bool SchemaAggregateUtil::canSatisfyRecord(
                                   const ChoiceArrayItem& choiceArrayItem,
                                   const RecordDef&       record)
{
    if (RecordDef::BDEM_CHOICE_RECORD != record.recordType()) {
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

void SchemaAggregateUtil::initRowShallow(Row              *row,
                                              const RecordDef&  record)
{
    BSLS_ASSERT(row);
    BSLS_ASSERT(RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    const int numFields = record.numFields();
    BSLS_ASSERT(numFields == row->length());

    for (int i = 0; i < numFields; ++i) {
        const FieldDef&      fieldDef = record.field(i);
        const ElemRef&       elemRef  = (*row)[i];
        const ElemType::Type type     = fieldDef.elemType();

        BSLS_ASSERT(type == elemRef.type());

        if (ElemType::isScalarType(type) && fieldDef.hasDefaultValue()) {
            elemRef.replaceValue(fieldDef.defaultValue());
        }
        else {
            elemRef.makeNull();
        }
    }
}

void SchemaAggregateUtil::initRowDeep(Row              *row,
                                           const RecordDef&  record)
{
    BSLS_ASSERT(row);
    BSLS_ASSERT(RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    RecursionGuard recursionGuard(0, record.schema().numRecords());
    initRowDeepImp(row, record, recursionGuard);
}

void SchemaAggregateUtil::initListAllNull(List             *list,
                                               const RecordDef&  record)
{
    BSLS_ASSERT(list);
    BSLS_ASSERT(RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    const int numFields = record.numFields();
    bsl::vector<ElemType::Type> elemTypes;
    elemTypes.reserve(numFields);

    for (int i = 0; i < numFields; ++i) {
        elemTypes.push_back(record.field(i).elemType());
    }

    list->reset(elemTypes);
}

void SchemaAggregateUtil::initListShallow(List             *list,
                                               const RecordDef&  record)
{
    BSLS_ASSERT(list);
    BSLS_ASSERT(RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    // init types
    SchemaAggregateUtil::initListAllNull(list, record);

    // init default values
    SchemaAggregateUtil::initRowShallow(&list->row(), record);
}

void SchemaAggregateUtil::initListDeep(List             *list,
                                            const RecordDef&  record)
{
    BSLS_ASSERT(list);
    BSLS_ASSERT(RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    RecursionGuard recursionGuard(0, record.schema().numRecords());
    initListDeepImp(list, record, recursionGuard);
}

void SchemaAggregateUtil::initTable(Table            *table,
                                         const RecordDef&  record)
{
    BSLS_ASSERT(table);
    BSLS_ASSERT(RecordDef::BDEM_SEQUENCE_RECORD == record.recordType());

    const int numFields = record.numFields();
    bsl::vector<ElemType::Type> elemTypes;
    elemTypes.reserve(numFields);

    for (int i = 0; i < numFields; ++i) {
        elemTypes.push_back(record.field(i).elemType());
    }

    table->reset(elemTypes);
}

void SchemaAggregateUtil::initChoice(Choice           *choice,
                                          const RecordDef&  record)
{
    BSLS_ASSERT(choice);
    BSLS_ASSERT(RecordDef::BDEM_CHOICE_RECORD == record.recordType());

    const int numFields = record.numFields();
    bsl::vector<ElemType::Type> elemTypes;
    elemTypes.reserve(numFields);

    for (int i = 0; i < numFields; ++i) {
        elemTypes.push_back(record.field(i).elemType());
    }

    choice->reset(elemTypes);
}

void
SchemaAggregateUtil::initChoiceArray(ChoiceArray     *choiceArray,
                                          const RecordDef& record)
{
    BSLS_ASSERT(choiceArray);
    BSLS_ASSERT(RecordDef::BDEM_CHOICE_RECORD == record.recordType());

    const int numFields = record.numFields();
    bsl::vector<ElemType::Type> elemTypes;
    elemTypes.reserve(numFields);

    for (int i = 0; i < numFields; ++i) {
        elemTypes.push_back(record.field(i).elemType());
    }

    choiceArray->reset(elemTypes);
}

bool SchemaAggregateUtil::isRowShallowConformant(
                                                  const Row&       row,
                                                  const RecordDef& record)
{
    if (RecordDef::BDEM_SEQUENCE_RECORD != record.recordType()) {
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

bool SchemaAggregateUtil::isRowConformant(const Row&       row,
                                               const RecordDef& record)
{
    if (!SchemaAggregateUtil::isRowShallowConformant(row, record)) {
        return false;                                                 // RETURN
    }

    return isRowDataConformant(row, record);
}

bool SchemaAggregateUtil::isRowDeepConformant(
                                                  const Row&       row,
                                                  const RecordDef& record)
{
    if (!SchemaAggregateUtil::isRowShallowConformant(row, record)) {
        return false;                                                 // RETURN
    }

    return isRowDataDeepConformant(row, record);
}

bool SchemaAggregateUtil::isTableShallowConformant(
                                                  const Table&     table,
                                                  const RecordDef& record)
{
    if (RecordDef::BDEM_SEQUENCE_RECORD != record.recordType()) {
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

bool SchemaAggregateUtil::isTableConformant(const Table&     table,
                                                 const RecordDef& record)
{
    if (!SchemaAggregateUtil::isTableShallowConformant(table, record)) {
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

bool SchemaAggregateUtil::isTableDeepConformant(
                                                  const Table&     table,
                                                  const RecordDef& record)
{
    if (!SchemaAggregateUtil::isTableShallowConformant(table, record)) {
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

bool SchemaAggregateUtil::isChoiceArrayItemShallowConformant(
                                   const ChoiceArrayItem& choiceArrayItem,
                                   const RecordDef&       record)
{
    if (RecordDef::BDEM_CHOICE_RECORD != record.recordType()) {
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

bool SchemaAggregateUtil::isChoiceArrayItemConformant(
                                   const ChoiceArrayItem& choiceArrayItem,
                                   const RecordDef&       record)
{
    if (!SchemaAggregateUtil::isChoiceArrayItemShallowConformant(
                                                               choiceArrayItem,
                                                               record)) {
        return false;                                                 // RETURN
    }

    return isSelectionDataConformant(choiceArrayItem, record);
}

bool SchemaAggregateUtil::isChoiceArrayItemDeepConformant(
                                   const ChoiceArrayItem& choiceArrayItem,
                                   const RecordDef&       record)
{
    if (!SchemaAggregateUtil::isChoiceArrayItemShallowConformant(
                                                               choiceArrayItem,
                                                               record)) {
        return false;                                                 // RETURN
    }

    return isSelectionDataDeepConformant(choiceArrayItem, record);
}

bool SchemaAggregateUtil::isChoiceArrayShallowConformant(
                                           const ChoiceArray& choiceArray,
                                           const RecordDef&   record)
{
    if (RecordDef::BDEM_CHOICE_RECORD != record.recordType()) {
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

bool SchemaAggregateUtil::isChoiceArrayConformant(
                                           const ChoiceArray& choiceArray,
                                           const RecordDef&   record)
{
    if (!SchemaAggregateUtil::isChoiceArrayShallowConformant(choiceArray,
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

bool SchemaAggregateUtil::isChoiceArrayDeepConformant(
                                           const ChoiceArray& choiceArray,
                                           const RecordDef&   record)
{
    if (!SchemaAggregateUtil::isChoiceArrayShallowConformant(choiceArray,
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
SchemaAggregateUtil::print(bsl::ostream&             stream,
                                const ConstElemRef&  element,
                                const RecordDef     *constraint,
                                int                       level,
                                int                       spacesPerLevel)
{
    if (element.isNull()) {
        stream << "NULL";
        return stream;
    }

    if (constraint) {
        const ElemType::Type type = element.type();
        BSLS_ASSERT(ElemType::isAggregateType(type));

        switch (type) {
          case ElemType::BDEM_ROW: {
            return print(stream,
                         *(const Row*)element.data(),
                         *constraint,
                         level,
                         spacesPerLevel);
          }
          case ElemType::BDEM_CHOICE_ARRAY_ITEM: {
            return print(stream,
                         *(const ChoiceArrayItem*)element.data(),
                         *constraint,
                         level,
                         spacesPerLevel);
          }
          case ElemType::BDEM_LIST: {
            return print(stream,
                         element.theList(),
                         *constraint,
                         level,
                         spacesPerLevel);
          }
          case ElemType::BDEM_CHOICE: {
            return print(stream,
                         element.theChoice(),
                         *constraint,
                         level,
                         spacesPerLevel);
          }
          case ElemType::BDEM_TABLE: {
            return print(stream,
                         element.theTable(),
                         *constraint,
                         level,
                         spacesPerLevel);
          }
          case ElemType::BDEM_CHOICE_ARRAY: {
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
SchemaAggregateUtil::print(bsl::ostream&         stream,
                                const Row&       row,
                                const RecordDef& record,
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
        bdlb::Print::indent(stream, level, spacesPerLevel);
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
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        const char *fieldName = record.fieldName(i);
        stream << (fieldName ? fieldName : "(anonymous)")  << " = ";

        const FieldDef& fieldDef = record.field(i);
        ConstElemRef    elemRef  = row[i];

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

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << ']' << endOfField;

    return stream << bsl::flush;
}

bsl::ostream&
SchemaAggregateUtil::print(bsl::ostream&         stream,
                                const List&      list,
                                const RecordDef& record,
                                int                   level,
                                int                   spacesPerLevel)
{
    return print(stream, list.row(), record, level, spacesPerLevel);
}

bsl::ostream&
SchemaAggregateUtil::print(bsl::ostream&               stream,
                                const ChoiceArrayItem& item,
                                const RecordDef&       record,
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
        bdlb::Print::indent(stream, level, spacesPerLevel);
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
    bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);

    if (-1 == item.selector()) {
        stream << "NO SELECTION" << endOfField;
    }
    else {
        const char *fieldName = record.fieldName(item.selector());
        stream << (fieldName ? fieldName : "(anonymous)")  << " = ";

        const FieldDef& fieldDef = record.field(item.selector());
        ConstElemRef    elemRef  = item.selection();

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

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << ']' << endOfField;

    return stream << bsl::flush;
}

bsl::ostream&
SchemaAggregateUtil::print(bsl::ostream&         stream,
                                const Choice&    choice,
                                const RecordDef& record,
                                int                   level,
                                int                   spacesPerLevel)
{
    return print(stream, choice.item(), record, level, spacesPerLevel);
}

bsl::ostream&
SchemaAggregateUtil::print(bsl::ostream&         stream,
                                const Table&     table,
                                const RecordDef& record,
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
        bdlb::Print::indent(stream, level, spacesPerLevel);
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
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        print(stream, table[i], record, -levelPlus1, spacesPerLevel);
    }

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << ']' << endOfField;

    return stream << bsl::flush;
}

bsl::ostream&
SchemaAggregateUtil::print(bsl::ostream&           stream,
                                const ChoiceArray& array,
                                const RecordDef&   record,
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
        bdlb::Print::indent(stream, level, spacesPerLevel);
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
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        print(stream, array[i], record, -levelPlus1, spacesPerLevel);
    }

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << ']' << endOfField;

    return stream << bsl::flush;
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
