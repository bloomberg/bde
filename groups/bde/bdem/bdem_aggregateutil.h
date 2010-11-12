// bdem_aggregateutil.h                                               -*-C++-*-
#ifndef INCLUDED_BDEM_AGGREGATEUTIL
#define INCLUDED_BDEM_AGGREGATEUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a set of utility functions for 'bdem_aggregate' classes.
//
//@CLASSES:
//  bdem_AggregateUtil: namespace for a suite of 'bdem_aggregate' utilities
//
//@AUTHOR: John Lakos (jlakos)
//
//@SEE_ALSO: bdem_aggregate, bdeimp_fuzzy
//
//@DESCRIPTION: This component provides a suite of functions to compare two
// aggregates (i.e., two rows, lists, tables, choices, choice arrays, or
// 'const' element references) for "approximate equality" -- i.e., the usual
// functionality of 'operator==' except that exactly-matching types
// 'BDEM_FLOAT', 'BDEM_DOUBLE', 'BDEM_FLOAT_ARRAY', and 'BDEM_DOUBLE_ARRAY'
// (including such types contained in nested lists, tables, choices, and choice
// arrays) are compared using reasonable, implementation-dependent, default or
// user-supplied relative and absolute tolerances to determine *approximate*
// *equality*.
//
///Caveat
///------
// The "approximate equality" functions in this component are implemented using
// the "compare" methods implemented in the 'bdeimp_fuzzy' component, which is
// vulnerable to the limitations of the internal representations of the native
// floating-point values 'double' and 'float'.  Although the
// "are*ApproximatelyEqual" functions provided here are highly reliable for
// comparing final results such as prices, volumes, and interest rates, the
// algorithms used may fail if any floating-point value is "too close" to its
// limits of representation -- i.e., too close to the values of the relative
// and absolute tolerances specified for a given function call.  (See the
// documentation of the 'bdeimp_fuzzy' component for more details on the
// limits of applicability of these approximate-equality functions.)
//
///Usage
///-----
// As part of a transaction processing system consider a price book class that
// stores pricing quotes from various sources on a variety of fixed income
// securities.  For each security identified by its CUSIP number, the book
// stores the price, yield to maturity, coupon, and maturity date for multiple
// pricing sources.  Each pricing source is identified by a unique source id.
//
// The interface of the price book class allows adding securities, updating
// the pricing information of currently stored securities, and removing
// securities.  Additionally, the price book can be queried to find the
// pricing source that most closely matches a client's purchase price and
// acceptable tolerance levels.  Note that for illustration purposes the
// interface is intentionally kept very short.
//
// The following is a class definition for the price book:
//..
//  class PriceBook {
//      // This class stores the set of pricing information received from
//      // various pricing sources, and additionally provides a function that
//      // allows retrieval of the pricing source that supports the best match
//      // for the client's pricing request.
//
//      // TYPES
//      typedef bsl::pair<int, bdem_List>                        SourceQuote;
//      typedef bsl::map<bsl::string, bsl::vector<SourceQuote> > QuoteBook;
//
//      // DATA
//      QuoteBook d_quoteBook;
//
//    public:
//      // CREATORS
//      PriceBook(bslma_Allocator *basicAllocator = 0);
//          // Construct a price book.  Optionally specify a 'basicAllocator'
//          // used to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      ~PriceBook();
//          // Destroy this price book.
//
//      // MANIPULATORS
//      void addSecurity(const bsl::string& cusip,
//                       int                sourceId,
//                       const bdem_List&   pricingInfo);
//          // Add the security having the specified 'cusip' being sold by the
//          // pricing source indicated by the specified 'sourceId' with the
//          // specified 'pricingInfo'.
//
//      int updateSecurity(const bsl::string& cusip,
//                         int                sourceId,
//                         const bdem_List&   pricingInfo);
//          // Update the security having the specified 'cusip' being sold by
//          // the pricing source indicated by the specified 'sourceId' to have
//          // the specified 'pricingInfo'.  Return 0 on success, and a
//          // non-zero value otherwise.
//
//      int removeSecurity(const bsl::string& cusip, int sourceId);
//          // Remove the security having the specified 'cusip' being sold by
//          // the pricing source indicated by the specified 'sourceId'.
//          // Return 0 on success, and a non-zero value otherwise.
//
//      int loadMatchingSourceId(int                *sourceId,
//                               const bsl::string&  cusip,
//                               const bdem_List&    requestedPrice,
//                               double              relTolerance,
//                               double              absTolerance);
//          // Load into the specified 'sourceId' the id of the pricing source
//          // that matches the specified 'requestedPrice' for the specified
//          // 'cusip' within the specified 'relTolerance' and 'absTolerance'
//          // levels.
//  };
//..
// The function definitions for the price book class are provided below:
//..
//  // CREATORS
//  PriceBook::PriceBook(bslma_Allocator *basicAllocator)
//  : d_quoteBook(basicAllocator)
//  {
//  }
//
//  PriceBook::~PriceBook()
//  {
//  }
//
//  // MANIPULATORS
//  void PriceBook::addSecurity(const bsl::string& cusip,
//                              int                sourceId,
//                              const bdem_List&   pricingInfo)
//  {
//      QuoteBook::iterator iter = d_quoteBook.find(cusip);
//      if (iter == d_quoteBook.end()) {
//          bsl::vector<SourceQuote> v;
//          v.push_back(bsl::make_pair(sourceId, pricingInfo));
//          d_quoteBook[cusip] = v;
//      }
//      else {
//          iter->second.push_back(bsl::make_pair(sourceId, pricingInfo));
//      }
//  }
//
//  int PriceBook::updateSecurity(const bsl::string& cusip,
//                                int                sourceId,
//                                const bdem_List&   pricingInfo)
//  {
//      enum { FAILURE = -1, SUCCESS };
//
//      QuoteBook::iterator mapIter = d_quoteBook.find(cusip);
//      if (mapIter == d_quoteBook.end()) {
//          return FAILURE;                                           // RETURN
//      }
//
//      bsl::vector<SourceQuote>& priceVec  = mapIter->second;
//      const int                 numPrices = priceVec.size();
//      for (int i = 0; i < numPrices; ++i) {
//          if (sourceId == priceVec[0].first) {
//              priceVec[0].second = pricingInfo;
//          }
//      }
//      return SUCCESS;
//  }
//
//  int PriceBook::removeSecurity(const bsl::string& cusip, int sourceId)
//  {
//      enum { FAILURE = -1, SUCCESS };
//
//      QuoteBook::iterator mapIter = d_quoteBook.find(cusip);
//      if (mapIter == d_quoteBook.end()) {
//          return FAILURE;                                           // RETURN
//      }
//
//      bsl::vector<SourceQuote>& priceVec  = mapIter->second;
//      const int                 numPrices = priceVec.size();
//      for (int i = 0; i < numPrices; ++i) {
//          if (sourceId == priceVec[0].first) {
//              priceVec.erase(priceVec.begin() + i);
//          }
//      }
//      return SUCCESS;
//  }
//
//  int PriceBook::loadMatchingSourceId(int                *sourceId,
//                                      const bsl::string&  cusip,
//                                      const bdem_List&    requestedPrice,
//                                      double              relTolerance,
//                                      double              absTolerance)
//  {
//      enum { FAILURE = -1, SUCCESS };
//
//      QuoteBook::const_iterator iter = d_quoteBook.find(cusip);
//      const bsl::vector<SourceQuote>& priceVec = iter->second;
//      const int numPrices = priceVec.size();
//      for (int i = 0; i < numPrices; ++i) {
//          if (bdem_AggregateUtil::areListsApproximatelyEqual(
//                                                          requestedPrice,
//                                                          priceVec[i].second,
//                                                          relTolerance,
//                                                          absTolerance)) {
//              *sourceId = priceVec[i].first;
//              return SUCCESS;
//          }
//      }
//      return FAILURE;
//  }
//..
// Clients can use the price book class as follows:
//..
//   PriceBook                        priceBook;
//   bsl::vector<bdem_ElemType::Type> priceTypes;
//
//   priceTypes.push_back(bdem_ElemType::BDEM_DOUBLE);  // price
//   priceTypes.push_back(bdem_ElemType::BDEM_DOUBLE);  // yield to maturity
//   priceTypes.push_back(bdem_ElemType::BDEM_DOUBLE);  // coupon
//   priceTypes.push_back(bdem_ElemType::BDEM_INT);     // payment frequency
//   priceTypes.push_back(bdem_ElemType::BDEM_DATE);    // maturity date
//
//   const bsl::string CUSIP = "SL2S6Q3Y";
//
//   const int srcId1 = 1, srcId2 = 2;
//   bsl::vector<int> sourceIds;
//   bdem_List        quote1(priceTypes), quote2(priceTypes);
//
//   quote1[0].theModifiableDouble() = 101.53;
//   quote1[1].theModifiableDouble() = .06;
//   quote1[2].theModifiableDouble() = 70;
//   quote1[3].theModifiableDate()   = bdet_Date(2012, 12, 31);
//
//   quote2[0].theModifiableDouble() = 101.93;
//   quote2[1].theModifiableDouble() = .057;
//   quote2[2].theModifiableDouble() = 70;
//   quote2[3].theModifiableDate()   = bdet_Date(2012, 12, 31);
//
//   priceBook.addSecurity(CUSIP, srcId1, quote1);
//   priceBook.addSecurity(CUSIP, srcId2, quote2);
//
//   int srcId = -1;
//   bdem_List requestedQuote(priceTypes);
//   requestedQuote[0].theModifiableDouble() = 102;
//   requestedQuote[1].theModifiableDouble() = .055;
//   requestedQuote[2].theModifiableDouble() = 70;
//   requestedQuote[3].theModifiableDate()   = bdet_Date(2012, 12, 31);
//   int rc = priceBook.loadMatchingSourceId(&srcId,
//                                           CUSIP,
//                                           requestedQuote,
//                                           0,
//                                           0.1);
//   assert(0 == rc);
//   assert(2 == srcId);
//
//   rc = priceBook.loadMatchingSourceId(&srcId,
//                                       CUSIP,
//                                       requestedQuote,
//                                       0,
//                                       0.5);
//   assert(0 == rc);
//   assert(1 == srcId);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_LIST
#include <bdem_list.h>
#endif

namespace BloombergLP {

class bdem_Choice;
class bdem_ChoiceArray;
class bdem_ConstElemRef;
class bdem_Row;
class bdem_Table;

                        // ========================
                        // class bdem_AggregateUtil
                        // ========================

class bdem_AggregateUtil {
    // This class provides a namespace for a suite of utility functions that
    // operate on objects defined in the 'bdem_aggregate' component (i.e.,
    // 'bdem_Row', 'bdem_List', 'bdem_Table', 'bdem_Choice', and
    // 'bdem_ChoiceArray'), as well as 'const' element references
    // ('bdem_ConstElemRef').  By default, reasonable, implementation-defined
    // tolerances are used; specifying explicit tolerances for 'double' values
    // apply to both 'double' and 'float' values, unless 'float' values are
    // supplied separately.

    // PRIVATE CLASS DATA
    static const double S_DEFAULT_REL_TOL;  // default relative tolerance
    static const double S_DEFAULT_ABS_TOL;  // default absolute tolerance

  public:
    // CLASS METHODS
    static bool areElemRefsApproximatelyEqual(const bdem_ConstElemRef& lhs,
                                              const bdem_ConstElemRef& rhs);
    static bool areElemRefsApproximatelyEqual(
                                  const bdem_ConstElemRef& lhs,
                                  const bdem_ConstElemRef& rhs,
                                  double                   doubleRelTolerance);
    static bool areElemRefsApproximatelyEqual(
                                  const bdem_ConstElemRef& lhs,
                                  const bdem_ConstElemRef& rhs,
                                  double                   doubleRelTolerance,
                                  double                   doubleAbsTolerance);
    static bool areElemRefsApproximatelyEqual(
                                   const bdem_ConstElemRef& lhs,
                                   const bdem_ConstElemRef& rhs,
                                   double                   doubleRelTolerance,
                                   double                   doubleAbsTolerance,
                                   double                   floatRelTolerance);
    static bool areElemRefsApproximatelyEqual(
                                   const bdem_ConstElemRef& lhs,
                                   const bdem_ConstElemRef& rhs,
                                   double                   doubleRelTolerance,
                                   double                   doubleAbsTolerance,
                                   double                   floatRelTolerance,
                                   double                   floatAbsTolerance);
        // Return 'true' if the specified 'lhs' and 'rhs' element references
        // are *approximately* *equal*, and 'false' otherwise.  Two element
        // references are approximately equal if they are bound to 'bdem'
        // elements having the same type and value, *except* for any values of
        // type 'double' or 'float': All such elements of *exactly* *matching*
        // 'bdem' type (including any arrays and nested aggregates containing
        // 'double' or 'float' elements) are compared for "fuzzy equality"
        // using the specified 'doubleRelTolerance', 'doubleAbsTolerance',
        // 'floatRelTolerance', and 'floatAbsTolerance' rather than for "true
        // equality" using the native '==' on 'double' and 'float' objects.
        // Note that if any supplied tolerance value is negative, that aspect
        // of fuzzy comparison is suppressed, but the result remains valid.
        // Also note that reasonable, implementation-dependent, default values
        // are used for any unspecified tolerances.

    static bool areRowsApproximatelyEqual(const bdem_Row& lhs,
                                          const bdem_Row& rhs);
    static bool areRowsApproximatelyEqual(const bdem_Row& lhs,
                                          const bdem_Row& rhs,
                                          double          doubleRelTolerance);
    static bool areRowsApproximatelyEqual(const bdem_Row& lhs,
                                          const bdem_Row& rhs,
                                          double          doubleRelTolerance,
                                          double          doubleAbsTolerance);
    static bool areRowsApproximatelyEqual(const bdem_Row& lhs,
                                          const bdem_Row& rhs,
                                          double          doubleRelTolerance,
                                          double          doubleAbsTolerance,
                                          double          floatRelTolerance);
    static bool areRowsApproximatelyEqual(const bdem_Row& lhs,
                                          const bdem_Row& rhs,
                                          double          doubleRelTolerance,
                                          double          doubleAbsTolerance,
                                          double          floatRelTolerance,
                                          double          floatAbsTolerance);
        // Return 'true' if the specified 'lhs' and 'rhs' rows are
        // *approximately* *equal*, and 'false' otherwise.  Two rows are
        // approximately equal if they have the same number of elements and the
        // respective elements at each index position have the same type and
        // value, *except* for any elements of type 'double' or 'float': All
        // such elements of *exactly* *matching* 'bdem' type (including any
        // arrays and nested aggregates containing 'double' or 'float'
        // elements) are compared for "fuzzy equality" using the
        // optionally-specified 'doubleRelTolerance', 'doubleAbsTolerance',
        // 'floatRelTolerance', and 'floatAbsTolerance' (as appropriate) rather
        // than for "true equality" using the native '==' on 'double' and
        // 'float' objects.  Specifying a tolerance value for type 'double'
        // applies to both 'double' and 'float' elements unless the
        // corresponding 'float' tolerance is also supplied.  Note that if any
        // supplied tolerance value is negative, that aspect of fuzzy
        // comparison is suppressed, but the result remains valid.  Also note
        // that reasonable, implementation-dependent, default values are used
        // for any unspecified tolerances.

    static bool areListsApproximatelyEqual(const bdem_List& lhs,
                                           const bdem_List& rhs);
    static bool areListsApproximatelyEqual(
                                          const bdem_List& lhs,
                                          const bdem_List& rhs,
                                          double           doubleRelTolerance);
    static bool areListsApproximatelyEqual(
                                          const bdem_List& lhs,
                                          const bdem_List& rhs,
                                          double           doubleRelTolerance,
                                          double           doubleAbsTolerance);
    static bool areListsApproximatelyEqual(const bdem_List& lhs,
                                           const bdem_List& rhs,
                                           double           doubleRelTolerance,
                                           double           doubleAbsTolerance,
                                           double           floatRelTolerance);
    static bool areListsApproximatelyEqual(const bdem_List& lhs,
                                           const bdem_List& rhs,
                                           double           doubleRelTolerance,
                                           double           doubleAbsTolerance,
                                           double           floatRelTolerance,
                                           double           floatAbsTolerance);
        // Return 'true' if the specified 'lhs' and 'rhs' lists are
        // *approximately* *equal*, and 'false' otherwise.  Two lists are
        // approximately equal if they have the same number of elements and the
        // respective elements at each index position have the same type and
        // value, *except* for any elements of type 'double' or 'float': All
        // such elements of *exactly* *matching* 'bdem' type (including any
        // arrays and nested aggregates containing 'double' or 'float'
        // elements) are compared for "fuzzy equality" using the
        // optionally-specified 'doubleRelTolerance', 'doubleAbsTolerance',
        // 'floatRelTolerance', and 'floatAbsTolerance' (as appropriate) rather
        // than for "true equality" using the native '==' on 'double' and
        // 'float' objects.  Specifying a tolerance value for type 'double'
        // applies to both 'double' and 'float' elements unless the
        // corresponding 'float' tolerance is also supplied.  Note that if any
        // supplied tolerance value is negative, that aspect of fuzzy
        // comparison is suppressed, but the result remains valid.  Also note
        // that reasonable, implementation-dependent, default values are used
        // for any unspecified tolerances.

    static
    bool areTablesApproximatelyEqual(const bdem_Table& lhs,
                                     const bdem_Table& rhs);
    static
    bool areTablesApproximatelyEqual(const bdem_Table& lhs,
                                     const bdem_Table& rhs,
                                     double            doubleRelTolerance);
    static
    bool areTablesApproximatelyEqual(const bdem_Table& lhs,
                                     const bdem_Table& rhs,
                                     double            doubleRelTolerance,
                                     double            doubleAbsTolerance);
    static
    bool areTablesApproximatelyEqual(const bdem_Table& lhs,
                                     const bdem_Table& rhs,
                                     double            doubleRelTolerance,
                                     double            doubleAbsTolerance,
                                     double            floatRelTolerance);
    static
    bool areTablesApproximatelyEqual(const bdem_Table& lhs,
                                     const bdem_Table& rhs,
                                     double            doubleRelTolerance,
                                     double            doubleAbsTolerance,
                                     double            floatRelTolerance,
                                     double            floatAbsTolerance);
        // Return 'true' if the specified 'lhs' and 'rhs' tables are
        // *approximately* *equal*, and 'false' otherwise.  Two tables are
        // approximately equal if they have the same number of rows
        // and columns, corresponding column types are the same, and the
        // respective elements at each (row, column) index position have the
        // same value, *except* for any elements of type 'double' or 'float':
        // All such elements of *exactly* *matching* 'bdem' type (including any
        // arrays and nested aggregates containing 'double' or 'float'
        // elements) are compared for "fuzzy equality" using the
        // optionally-specified 'doubleRelTolerance', 'doubleAbsTolerance',
        // 'floatRelTolerance', and 'floatAbsTolerance' (as appropriate) rather
        // than for "true equality" using the native '==' on 'double' and
        // 'float' objects.  Specifying a tolerance value for type 'double'
        // applies to both 'double' and 'float' elements unless the
        // corresponding 'float' tolerance is also supplied.  Note that if any
        // supplied tolerance value is negative, that aspect of fuzzy
        // comparison is suppressed, but the result remains valid.  Also note
        // that reasonable, implementation-dependent, default values are used
        // for any unspecified tolerances.

    static
    bool areChoicesApproximatelyEqual(const bdem_Choice& lhs,
                                      const bdem_Choice& rhs);
    static
    bool areChoicesApproximatelyEqual(const bdem_Choice& lhs,
                                      const bdem_Choice& rhs,
                                      double             doubleRelTolerance);
    static
    bool areChoicesApproximatelyEqual(const bdem_Choice& lhs,
                                      const bdem_Choice& rhs,
                                      double             doubleRelTolerance,
                                      double             doubleAbsTolerance);
    static
    bool areChoicesApproximatelyEqual(const bdem_Choice& lhs,
                                      const bdem_Choice& rhs,
                                      double             doubleRelTolerance,
                                      double             doubleAbsTolerance,
                                      double             floatRelTolerance);
    static
    bool areChoicesApproximatelyEqual(const bdem_Choice& lhs,
                                      const bdem_Choice& rhs,
                                      double             doubleRelTolerance,
                                      double             doubleAbsTolerance,
                                      double             floatRelTolerance,
                                      double             floatAbsTolerance);
        // Return 'true' if the specified 'lhs' and 'rhs' choices are
        // *approximately* *equal*, and 'false' otherwise.  Two choices are
        // approximately equal if they have the same types catalog, the same
        // selector, and the same selection value *except* for any elements of
        // type 'double' or 'float': All such elements of *exactly* *matching*
        // 'bdem' type (including any arrays and nested aggregates containing
        // 'double' or 'float' elements) are compared for "fuzzy equality"
        // using the optionally-specified 'doubleRelTolerance',
        // 'doubleAbsTolerance', 'floatRelTolerance', and 'floatAbsTolerance'
        // (as appropriate) rather than for "true equality" using the native
        // '==' on 'double' and 'float' objects.  Specifying a tolerance value
        // for type 'double' applies to both 'double' and 'float' elements
        // unless the corresponding 'float' tolerance is also supplied.  Note
        // that if any supplied tolerance value is negative, that aspect of
        // fuzzy comparison is suppressed, but the result remains valid.  Also
        // note that reasonable, implementation-dependent, default values are
        // used for any unspecified tolerances.

    static
    bool areChoiceArraysApproximatelyEqual(const bdem_ChoiceArray& lhs,
                                           const bdem_ChoiceArray& rhs);
    static
    bool areChoiceArraysApproximatelyEqual(
                                   const bdem_ChoiceArray& lhs,
                                   const bdem_ChoiceArray& rhs,
                                   double                  doubleRelTolerance);
    static
    bool areChoiceArraysApproximatelyEqual(
                                   const bdem_ChoiceArray& lhs,
                                   const bdem_ChoiceArray& rhs,
                                   double                  doubleRelTolerance,
                                   double                  doubleAbsTolerance);
    static
    bool areChoiceArraysApproximatelyEqual(
                                    const bdem_ChoiceArray& lhs,
                                    const bdem_ChoiceArray& rhs,
                                    double                  doubleRelTolerance,
                                    double                  doubleAbsTolerance,
                                    double                  floatRelTolerance);
    static
    bool areChoiceArraysApproximatelyEqual(
                                    const bdem_ChoiceArray& lhs,
                                    const bdem_ChoiceArray& rhs,
                                    double                  doubleRelTolerance,
                                    double                  doubleAbsTolerance,
                                    double                  floatRelTolerance,
                                    double                  floatAbsTolerance);
        // Return 'true' if the specified 'lhs' and 'rhs' choice arrays are
        // *approximately* *equal*, and 'false' otherwise.  Two choice arrays
        // are approximately equal if they have the same types catalog, the
        // same number of items, and each item in 'lhs' has the same selector
        // and the same selection value of the respective item in 'rhs'
        // *except* for any elements of type 'double' or 'float': All such
        // elements of *exactly* *matching* 'bdem' type (including any arrays
        // and nested aggregates containing 'double' or 'float' elements) are
        // compared for "fuzzy equality" using the optionally-specified
        // 'doubleRelTolerance', 'doubleAbsTolerance', 'floatRelTolerance',
        // and 'floatAbsTolerance' (as appropriate) rather than for "true
        // equality" using the native '==' on 'double' and 'float' objects.
        // Specifying a tolerance value for type 'double' applies to both
        // 'double' and 'float' elements unless the corresponding 'float'
        // tolerance is also supplied.  Note that if any supplied tolerance
        // value is negative, that aspect of fuzzy comparison is suppressed,
        // but the result remains valid.  Also note that reasonable,
        // implementation-dependent, default values are used for any
        // unspecified tolerances.
};

// ============================================================================
//                       INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class bdem_AggregateUtil
                        // ------------------------

// CLASS METHODS
inline
bool bdem_AggregateUtil::areElemRefsApproximatelyEqual(
                                                  const bdem_ConstElemRef& lhs,
                                                  const bdem_ConstElemRef& rhs)
{
    return bdem_AggregateUtil::areElemRefsApproximatelyEqual(
                                                            lhs,
                                                            rhs,
                                                            S_DEFAULT_REL_TOL,
                                                            S_DEFAULT_ABS_TOL,
                                                            S_DEFAULT_REL_TOL,
                                                            S_DEFAULT_ABS_TOL);
}

inline
bool bdem_AggregateUtil::areElemRefsApproximatelyEqual(
                                   const bdem_ConstElemRef& lhs,
                                   const bdem_ConstElemRef& rhs,
                                   double                   doubleRelTolerance)
{
    return bdem_AggregateUtil::areElemRefsApproximatelyEqual(
                                                            lhs,
                                                            rhs,
                                                            doubleRelTolerance,
                                                            S_DEFAULT_ABS_TOL,
                                                            doubleRelTolerance,
                                                            S_DEFAULT_ABS_TOL);
}

inline
bool bdem_AggregateUtil::areElemRefsApproximatelyEqual(
                                   const bdem_ConstElemRef& lhs,
                                   const bdem_ConstElemRef& rhs,
                                   double                   doubleRelTolerance,
                                   double                   doubleAbsTolerance)
{
    return bdem_AggregateUtil::areElemRefsApproximatelyEqual(
                                                         lhs,
                                                         rhs,
                                                         doubleRelTolerance,
                                                         doubleAbsTolerance,
                                                         doubleRelTolerance,
                                                         doubleAbsTolerance);
}

inline
bool bdem_AggregateUtil::areElemRefsApproximatelyEqual(
                                   const bdem_ConstElemRef& lhs,
                                   const bdem_ConstElemRef& rhs,
                                   double                   doubleRelTolerance,
                                   double                   doubleAbsTolerance,
                                   double                   floatRelTolerance)
{
    return bdem_AggregateUtil::areElemRefsApproximatelyEqual(
                                                           lhs,
                                                           rhs,
                                                           doubleRelTolerance,
                                                           doubleAbsTolerance,
                                                           floatRelTolerance,
                                                           doubleAbsTolerance);
}

inline
bool bdem_AggregateUtil::areRowsApproximatelyEqual(const bdem_Row& lhs,
                                                   const bdem_Row& rhs)
{
    return bdem_AggregateUtil::areRowsApproximatelyEqual(lhs,
                                                         rhs,
                                                         S_DEFAULT_REL_TOL,
                                                         S_DEFAULT_ABS_TOL,
                                                         S_DEFAULT_REL_TOL,
                                                         S_DEFAULT_ABS_TOL);
}

inline
bool bdem_AggregateUtil::areRowsApproximatelyEqual(
                                            const bdem_Row& lhs,
                                            const bdem_Row& rhs,
                                            double          doubleRelTolerance)
{
    return bdem_AggregateUtil::areRowsApproximatelyEqual(lhs,
                                                         rhs,
                                                         doubleRelTolerance,
                                                         S_DEFAULT_ABS_TOL,
                                                         doubleRelTolerance,
                                                         S_DEFAULT_ABS_TOL);
}

inline
bool bdem_AggregateUtil::areRowsApproximatelyEqual(
                                            const bdem_Row& lhs,
                                            const bdem_Row& rhs,
                                            double          doubleRelTolerance,
                                            double          doubleAbsTolerance)
{
    return bdem_AggregateUtil::areRowsApproximatelyEqual(lhs,
                                                         rhs,
                                                         doubleRelTolerance,
                                                         doubleAbsTolerance,
                                                         doubleRelTolerance,
                                                         doubleAbsTolerance);
}

inline
bool bdem_AggregateUtil::areRowsApproximatelyEqual(
                                            const bdem_Row& lhs,
                                            const bdem_Row& rhs,
                                            double          doubleRelTolerance,
                                            double          doubleAbsTolerance,
                                            double          floatRelTolerance)
{
    return bdem_AggregateUtil::areRowsApproximatelyEqual(lhs,
                                                         rhs,
                                                         doubleRelTolerance,
                                                         doubleAbsTolerance,
                                                         floatRelTolerance,
                                                         doubleAbsTolerance);
}

inline
bool bdem_AggregateUtil::areListsApproximatelyEqual(const bdem_List& lhs,
                                                    const bdem_List& rhs)
{
    return bdem_AggregateUtil::areRowsApproximatelyEqual(lhs.row(),
                                                         rhs.row(),
                                                         S_DEFAULT_REL_TOL,
                                                         S_DEFAULT_ABS_TOL,
                                                         S_DEFAULT_REL_TOL,
                                                         S_DEFAULT_ABS_TOL);
}

inline
bool bdem_AggregateUtil::areListsApproximatelyEqual(
                                           const bdem_List& lhs,
                                           const bdem_List& rhs,
                                           double           doubleRelTolerance)
{
    return bdem_AggregateUtil::areRowsApproximatelyEqual(lhs.row(),
                                                         rhs.row(),
                                                         doubleRelTolerance,
                                                         S_DEFAULT_ABS_TOL,
                                                         doubleRelTolerance,
                                                         S_DEFAULT_ABS_TOL);
}

inline
bool bdem_AggregateUtil::areListsApproximatelyEqual(
                                           const bdem_List& lhs,
                                           const bdem_List& rhs,
                                           double           doubleRelTolerance,
                                           double           doubleAbsTolerance)
{
    return bdem_AggregateUtil::areRowsApproximatelyEqual(lhs.row(),
                                                         rhs.row(),
                                                         doubleRelTolerance,
                                                         doubleAbsTolerance,
                                                         doubleRelTolerance,
                                                         doubleAbsTolerance);
}

inline
bool bdem_AggregateUtil::areListsApproximatelyEqual(
                                           const bdem_List& lhs,
                                           const bdem_List& rhs,
                                           double           doubleRelTolerance,
                                           double           doubleAbsTolerance,
                                           double           floatRelTolerance)
{
    return bdem_AggregateUtil::areRowsApproximatelyEqual(lhs.row(),
                                                         rhs.row(),
                                                         doubleRelTolerance,
                                                         doubleAbsTolerance,
                                                         floatRelTolerance,
                                                         doubleAbsTolerance);
}

inline
bool bdem_AggregateUtil::areListsApproximatelyEqual(
                                           const bdem_List& lhs,
                                           const bdem_List& rhs,
                                           double           doubleRelTolerance,
                                           double           doubleAbsTolerance,
                                           double           floatRelTolerance,
                                           double           floatAbsTolerance)
{
    return bdem_AggregateUtil::areRowsApproximatelyEqual(lhs.row(),
                                                         rhs.row(),
                                                         doubleRelTolerance,
                                                         doubleAbsTolerance,
                                                         floatRelTolerance,
                                                         floatAbsTolerance);
}

inline
bool bdem_AggregateUtil::areTablesApproximatelyEqual(const bdem_Table& lhs,
                                                     const bdem_Table& rhs)
{
    return bdem_AggregateUtil::areTablesApproximatelyEqual(lhs,
                                                           rhs,
                                                           S_DEFAULT_REL_TOL,
                                                           S_DEFAULT_ABS_TOL,
                                                           S_DEFAULT_REL_TOL,
                                                           S_DEFAULT_ABS_TOL);
}

inline
bool bdem_AggregateUtil::areTablesApproximatelyEqual(
                                          const bdem_Table& lhs,
                                          const bdem_Table& rhs,
                                          double            doubleRelTolerance)
{
    return bdem_AggregateUtil::areTablesApproximatelyEqual(lhs,
                                                           rhs,
                                                           doubleRelTolerance,
                                                           S_DEFAULT_ABS_TOL,
                                                           doubleRelTolerance,
                                                           S_DEFAULT_ABS_TOL);
}

inline
bool bdem_AggregateUtil::areTablesApproximatelyEqual(
                                          const bdem_Table& lhs,
                                          const bdem_Table& rhs,
                                          double            doubleRelTolerance,
                                          double            doubleAbsTolerance)
{
    return bdem_AggregateUtil::areTablesApproximatelyEqual(lhs,
                                                           rhs,
                                                           doubleRelTolerance,
                                                           doubleAbsTolerance,
                                                           doubleRelTolerance,
                                                           doubleAbsTolerance);
}

inline
bool bdem_AggregateUtil::areTablesApproximatelyEqual(
                                          const bdem_Table& lhs,
                                          const bdem_Table& rhs,
                                          double            doubleRelTolerance,
                                          double            doubleAbsTolerance,
                                          double            floatRelTolerance)
{
    return bdem_AggregateUtil::areTablesApproximatelyEqual(lhs,
                                                           rhs,
                                                           doubleRelTolerance,
                                                           doubleAbsTolerance,
                                                           floatRelTolerance,
                                                           doubleAbsTolerance);
}

inline
bool bdem_AggregateUtil::areChoicesApproximatelyEqual(const bdem_Choice& lhs,
                                                      const bdem_Choice& rhs)
{
    return bdem_AggregateUtil::areChoicesApproximatelyEqual(lhs,
                                                            rhs,
                                                            S_DEFAULT_REL_TOL,
                                                            S_DEFAULT_ABS_TOL,
                                                            S_DEFAULT_REL_TOL,
                                                            S_DEFAULT_ABS_TOL);
}

inline
bool bdem_AggregateUtil::areChoicesApproximatelyEqual(
                                         const bdem_Choice& lhs,
                                         const bdem_Choice& rhs,
                                         double             doubleRelTolerance)
{
    return bdem_AggregateUtil::areChoicesApproximatelyEqual(lhs,
                                                            rhs,
                                                            doubleRelTolerance,
                                                            S_DEFAULT_ABS_TOL,
                                                            doubleRelTolerance,
                                                            S_DEFAULT_ABS_TOL);
}

inline
bool bdem_AggregateUtil::areChoicesApproximatelyEqual(
                                         const bdem_Choice& lhs,
                                         const bdem_Choice& rhs,
                                         double             doubleRelTolerance,
                                         double             doubleAbsTolerance)
{
    return bdem_AggregateUtil::areChoicesApproximatelyEqual(
                                                           lhs,
                                                           rhs,
                                                           doubleRelTolerance,
                                                           doubleAbsTolerance,
                                                           doubleRelTolerance,
                                                           doubleAbsTolerance);
}

inline
bool bdem_AggregateUtil::areChoicesApproximatelyEqual(
                                         const bdem_Choice& lhs,
                                         const bdem_Choice& rhs,
                                         double             doubleRelTolerance,
                                         double             doubleAbsTolerance,
                                         double             floatRelTolerance)
{
    return bdem_AggregateUtil::areChoicesApproximatelyEqual(
                                                           lhs,
                                                           rhs,
                                                           doubleRelTolerance,
                                                           doubleAbsTolerance,
                                                           floatRelTolerance,
                                                           doubleAbsTolerance);
}

inline
bool bdem_AggregateUtil::areChoiceArraysApproximatelyEqual(
                                                   const bdem_ChoiceArray& lhs,
                                                   const bdem_ChoiceArray& rhs)
{
    return bdem_AggregateUtil::areChoiceArraysApproximatelyEqual(
                                                            lhs,
                                                            rhs,
                                                            S_DEFAULT_REL_TOL,
                                                            S_DEFAULT_ABS_TOL,
                                                            S_DEFAULT_REL_TOL,
                                                            S_DEFAULT_ABS_TOL);
}

inline
bool bdem_AggregateUtil::areChoiceArraysApproximatelyEqual(
                                    const bdem_ChoiceArray& lhs,
                                    const bdem_ChoiceArray& rhs,
                                    double                  doubleRelTolerance)
{
    return bdem_AggregateUtil::areChoiceArraysApproximatelyEqual(
                                                            lhs,
                                                            rhs,
                                                            doubleRelTolerance,
                                                            S_DEFAULT_ABS_TOL,
                                                            doubleRelTolerance,
                                                            S_DEFAULT_ABS_TOL);
}

inline
bool bdem_AggregateUtil::areChoiceArraysApproximatelyEqual(
                                    const bdem_ChoiceArray& lhs,
                                    const bdem_ChoiceArray& rhs,
                                    double                  doubleRelTolerance,
                                    double                  doubleAbsTolerance)
{
    return bdem_AggregateUtil::areChoiceArraysApproximatelyEqual(
                                                           lhs,
                                                           rhs,
                                                           doubleRelTolerance,
                                                           doubleAbsTolerance,
                                                           doubleRelTolerance,
                                                           doubleAbsTolerance);
}

inline
bool bdem_AggregateUtil::areChoiceArraysApproximatelyEqual(
                                    const bdem_ChoiceArray& lhs,
                                    const bdem_ChoiceArray& rhs,
                                    double                  doubleRelTolerance,
                                    double                  doubleAbsTolerance,
                                    double                  floatRelTolerance)
{
    return bdem_AggregateUtil::areChoiceArraysApproximatelyEqual(
                                                           lhs,
                                                           rhs,
                                                           doubleRelTolerance,
                                                           doubleAbsTolerance,
                                                           floatRelTolerance,
                                                           doubleAbsTolerance);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
