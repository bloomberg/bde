// bdlmxxx_aggregateutil.cpp                                          -*-C++-*-
#include <bdlmxxx_aggregateutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_aggregateutil_cpp,"$Id$ $CSID$")

#include <bdlmxxx_aggregate.h>
#include <bdlmxxx_choicearrayitem.h>
#include <bdlmxxx_elemref.h>
#include <bdlmxxx_elemtype.h>

#include <bdlimpxxx_fuzzy.h>

#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_date.h>
#include <bdlt_datetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bsls_assert.h>

#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {

                        // ------------------------
                        // class bdlmxxx::AggregateUtil
                        // ------------------------

// Note that the constants used as default tolerances in this component should
// be kept consistent with the implementation-dependent default tolerance
// values defined in 'bdlimpxxx_fuzzy'.

// PRIVATE CLASS DATA
const double bdlmxxx::AggregateUtil::S_DEFAULT_REL_TOL = 1e-12;
const double bdlmxxx::AggregateUtil::S_DEFAULT_ABS_TOL = 1e-24;

namespace bdlmxxx {
// CLASS METHODS
bool AggregateUtil::areElemRefsApproximatelyEqual(
                                   const ConstElemRef& lhs,
                                   const ConstElemRef& rhs,
                                   double                   doubleRelTolerance,
                                   double                   doubleAbsTolerance,
                                   double                   floatRelTolerance,
                                   double                   floatAbsTolerance)
{
    const ElemType::Type elemType = lhs.type();

    if (elemType != rhs.type()) {
        return false;                                                 // RETURN
    }

    switch (elemType) {
      case ElemType::BDEM_CHAR: {
        if (lhs.theChar() != rhs.theChar()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_SHORT: {
        if (lhs.theShort() != rhs.theShort()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_INT: {
        if (lhs.theInt() != rhs.theInt()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_INT64: {
        if (lhs.theInt64() != rhs.theInt64()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_FLOAT: {
        if (bdlimpxxx::Fuzzy::ne(lhs.theFloat(),
                             rhs.theFloat(),
                             floatRelTolerance,
                             floatAbsTolerance)) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_DOUBLE: {
        if (bdlimpxxx::Fuzzy::ne(lhs.theDouble(),
                             rhs.theDouble(),
                             doubleRelTolerance,
                             doubleAbsTolerance)) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_STRING: {
        if (lhs.theString() != rhs.theString()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_DATETIME: {
        if (lhs.theDatetime() != rhs.theDatetime()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_DATE: {
        if (lhs.theDate() != rhs.theDate()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_TIME: {
        if (lhs.theTime() != rhs.theTime()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_CHAR_ARRAY: {
        if (lhs.theCharArray() != rhs.theCharArray()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_SHORT_ARRAY: {
        if (lhs.theShortArray() != rhs.theShortArray()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_INT_ARRAY: {
        if (lhs.theIntArray() != rhs.theIntArray()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_INT64_ARRAY: {
        if (lhs.theInt64Array() != rhs.theInt64Array()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_FLOAT_ARRAY: {

        const bsl::vector<float>& lhsArray = lhs.theFloatArray();
        const bsl::vector<float>& rhsArray = rhs.theFloatArray();
        const int lhsLength = lhsArray.size();

        if (lhsLength != (int)rhsArray.size()) {
            return false;                                             // RETURN
        }

        for (int j = 0; j < lhsLength; ++j) {
            if (bdlimpxxx::Fuzzy::ne(lhsArray[j],
                                 rhsArray[j],
                                 floatRelTolerance,
                                 floatAbsTolerance)) {
                return false;                                         // RETURN
            }
        }
      } break;
      case ElemType::BDEM_DOUBLE_ARRAY: {

        const bsl::vector<double>& lhsArray = lhs.theDoubleArray();
        const bsl::vector<double>& rhsArray = rhs.theDoubleArray();
        const int lhsLength = lhsArray.size();

        if (lhsLength != (int)rhsArray.size()) {
            return false;                                             // RETURN
        }

        for (int j = 0; j < lhsLength; ++j) {
            if (bdlimpxxx::Fuzzy::ne(lhsArray[j],
                                 rhsArray[j],
                                 doubleRelTolerance,
                                 doubleAbsTolerance)) {
                return false;                                         // RETURN
            }
        }
      } break;
      case ElemType::BDEM_STRING_ARRAY: {
        if (lhs.theStringArray() != rhs.theStringArray()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_DATETIME_ARRAY: {
        if (lhs.theDatetimeArray() != rhs.theDatetimeArray()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_DATE_ARRAY: {
        if (lhs.theDateArray() != rhs.theDateArray()) {
                return false;                                         // RETURN
        }
      } break;
      case ElemType::BDEM_TIME_ARRAY: {
        if (lhs.theTimeArray() != rhs.theTimeArray()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_LIST: {
        if (!AggregateUtil::areListsApproximatelyEqual(
                                                          lhs.theList(),
                                                          rhs.theList(),
                                                          doubleRelTolerance,
                                                          doubleAbsTolerance,
                                                          floatRelTolerance,
                                                          floatAbsTolerance)) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_TABLE: {
        if (!AggregateUtil::areTablesApproximatelyEqual(
                                                          lhs.theTable(),
                                                          rhs.theTable(),
                                                          doubleRelTolerance,
                                                          doubleAbsTolerance,
                                                          floatRelTolerance,
                                                          floatAbsTolerance)) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_BOOL: {
        if (lhs.theBool() != rhs.theBool()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_DATETIMETZ: {
        if (lhs.theDatetimeTz() != rhs.theDatetimeTz()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_DATETZ: {
        if (lhs.theDateTz() != rhs.theDateTz()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_TIMETZ: {
        if (lhs.theTimeTz() != rhs.theTimeTz()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_BOOL_ARRAY: {
        if (lhs.theBoolArray() != rhs.theBoolArray()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_DATETIMETZ_ARRAY: {
        if (lhs.theDatetimeTzArray() != rhs.theDatetimeTzArray()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_DATETZ_ARRAY: {
        if (lhs.theDateTzArray() != rhs.theDateTzArray()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_TIMETZ_ARRAY: {
        if (lhs.theTimeTzArray() != rhs.theTimeTzArray()) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_CHOICE: {
        if (!AggregateUtil::areChoicesApproximatelyEqual(
                                                          lhs.theChoice(),
                                                          rhs.theChoice(),
                                                          doubleRelTolerance,
                                                          doubleAbsTolerance,
                                                          floatRelTolerance,
                                                          floatAbsTolerance)) {
            return false;                                             // RETURN
        }
      } break;
      case ElemType::BDEM_CHOICE_ARRAY: {
        if (!AggregateUtil::areChoiceArraysApproximatelyEqual(
                                                         lhs.theChoiceArray(),
                                                         rhs.theChoiceArray(),
                                                         doubleRelTolerance,
                                                         doubleAbsTolerance,
                                                         floatRelTolerance,
                                                         floatAbsTolerance)) {
            return false;                                             // RETURN
        }
      } break;
      default: {
        BSLS_ASSERT_OPT(0);  // We should never get here!
      } break;
    }

    return true;
}

bool AggregateUtil::areRowsApproximatelyEqual(
                                            const Row& lhs,
                                            const Row& rhs,
                                            double          doubleRelTolerance,
                                            double          doubleAbsTolerance,
                                            double          floatRelTolerance,
                                            double          floatAbsTolerance)
{
    const int len = lhs.length();

    if (len != rhs.length()) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < len; ++i) {
        if (!areElemRefsApproximatelyEqual(lhs[i],
                                           rhs[i],
                                           doubleRelTolerance,
                                           doubleAbsTolerance,
                                           floatRelTolerance,
                                           floatAbsTolerance)) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bool AggregateUtil::areTablesApproximatelyEqual(
                                          const Table& lhs,
                                          const Table& rhs,
                                          double            doubleRelTolerance,
                                          double            doubleAbsTolerance,
                                          double            floatRelTolerance,
                                          double            floatAbsTolerance)
{
    const int len = lhs.numRows();

    if (len != rhs.numRows()) {
        return false;                                                 // RETURN
    }

    const int numColumns = lhs.numColumns();

    if (numColumns != rhs.numColumns()) {
        return false;                                                 // RETURN
    }

    for (int col = 0; col < numColumns; ++col) {
        if (lhs.columnType(col) != rhs.columnType(col)) {
            return false;                                             // RETURN
        }
    }

    for (int row = 0; row < len; ++row) {
        if (!AggregateUtil::areRowsApproximatelyEqual(lhs[row],
                                                           rhs[row],
                                                           doubleRelTolerance,
                                                           doubleAbsTolerance,
                                                           floatRelTolerance,
                                                           floatAbsTolerance)){
            return false;                                             // RETURN
        }
    }

    return true;
}

bool AggregateUtil::areChoicesApproximatelyEqual(
                                         const Choice& lhs,
                                         const Choice& rhs,
                                         double             doubleRelTolerance,
                                         double             doubleAbsTolerance,
                                         double             floatRelTolerance,
                                         double             floatAbsTolerance)
{
    const int lhsNumSelections = lhs.numSelections();

    if (lhsNumSelections != rhs.numSelections()) {
        return false;                                                 // RETURN
    }

    // Compare the respective type catalogs for equivalence.

    for (int i = 0; i < lhsNumSelections; ++i) {
        if (lhs.selectionType(i) != rhs.selectionType(i)) {
            return false;                                             // RETURN
        }
    }

    const int lhsSelector = lhs.selector();

    if (lhsSelector != rhs.selector()) {
        return false;                                                 // RETURN
    }

    if (-1 == lhsSelector) {  // If true, both are the "null choice".
        return true;                                                  // RETURN
    }

    if (!areElemRefsApproximatelyEqual(lhs.selection(),
                                       rhs.selection(),
                                       doubleRelTolerance,
                                       doubleAbsTolerance,
                                       floatRelTolerance,
                                       floatAbsTolerance)) {
        return false;                                                 // RETURN
    }

    return true;
}

bool AggregateUtil::areChoiceArraysApproximatelyEqual(
                                    const ChoiceArray& lhs,
                                    const ChoiceArray& rhs,
                                    double                  doubleRelTolerance,
                                    double                  doubleAbsTolerance,
                                    double                  floatRelTolerance,
                                    double                  floatAbsTolerance)
{
    const int len = lhs.length();

    if (len != rhs.length()) {
        return false;                                                 // RETURN
    }

    const int lhsNumSelections = lhs.numSelections();

    if (lhsNumSelections != rhs.numSelections()) {
        return false;                                                 // RETURN
    }

    // Compare the respective type catalogs for equivalence.

    for (int i = 0; i < lhsNumSelections; ++i) {
        if (lhs.selectionType(i) != rhs.selectionType(i)) {
            return false;                                             // RETURN
        }
    }

    for (int i = 0; i < len; ++i) {
        const ChoiceArrayItem& lhsChoiceArrayItem = lhs[i];
        const ChoiceArrayItem& rhsChoiceArrayItem = rhs[i];

        const int lhsSelector = lhsChoiceArrayItem.selector();

        if (lhsSelector != rhsChoiceArrayItem.selector()) {
            return false;                                             // RETURN
        }

        if (-1 == lhsSelector) {  // If true, both are the "null choice".
            continue;
        }

        if (!areElemRefsApproximatelyEqual(lhsChoiceArrayItem.selection(),
                                           rhsChoiceArrayItem.selection(),
                                           doubleRelTolerance,
                                           doubleAbsTolerance,
                                           floatRelTolerance,
                                           floatAbsTolerance)) {
            return false;                                             // RETURN
        }
    }

    return true;
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
