// balxml_errorinfo.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_ERRORINFO
#define INCLUDED_BALXML_ERRORINFO

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide common error information for XML components.
//
//@CLASSES:
//   balxml::ErrorInfo: details of XML parsing errors
//
//@SEE_ALSO: baexml_xmlreader
//
//@DESCRIPTION: This component provides an in-core value-semantic class,
// 'balxml::ErrorInfo', that contains the following diagnostic information:
//..
//  - severity  (WARNING, ERROR, FATAL ERROR)
//  - line number
//  - column number
//  - document source (name of document containing the error)
//  - error message (error description)
//..
// Parsing components in the 'baexml' package make use of 'balxml::ErrorInfo'
// as a standard way to report errors and warnings back to the caller.  The
// information contained within a 'balxml::ErrorInfo' object is sufficient to
// report a single diagnostic in an input document.
//
///Usage
///-----
// In this example, we create a parser for a simple file of percentages.  The
// file is formatted as a sequence of lines, with each line containing a
// decimal number in the range "0" to "100", inclusive.  Leading whitespace and
// blank lines are ignored.  When an error occurs during parsing, the error
// data is stored in a 'balxml::ErrorInfo' object.  Our parser's interface is
// as follows:
//..
//  #include <balxml_errorinfo.h>
//  #include <sstream>
//  #include <cstdlib>
//  #include <cstring>
//  #include <climits>
//
//  class PercentParser {
//      // Parse a document stream consisting of a sequence of integral
//      // percentages (0 to 100) in decimal text format, one per line.
//
//      bsl::istream *d_input;    // Input document stream
//      bsl::string   d_docName;  // Document name
//      int           d_line;     // Current line number
//
//    public:
//      PercentParser(bsl::istream       *input,
//                    const bsl::string&  docName = "INPUT");
//          // Construct a parser to parse the data in the specified 'input'
//          // stream having the (optional) specified 'docName'.  A valid
//          // 'input' stream contains a sequence of integers in the range 0
//          // to 100, one per line, in decimal text format.  Each line may
//          // contain leading but not trailing tabs and spaces.  Characters
//          // after the 20th character on each line are ignored and will
//          // generate a warning.
//
//      int parseNext(balxml::ErrorInfo *errorInfo);
//          // Read and parse the next percentage in the input stream and
//          // return the percentage or -1 on eof or -2 on error.  Set the
//          // value of the specified 'errorInfo' structure on error or
//          // warning and leave it unchanged otherwise.  Do nothing and
//          // return -2 if 'errorInfo->severity()' >= 'BAEXML_ERROR'.
//  };
//..
// The constructor is straight-forward:
//..
//  PercentParser::PercentParser(bsl::istream       *input,
//                               const bsl::string&  docName)
//  : d_input(input), d_docName(docName), d_line(0)
//  {
//  }
//..
// The 'parseNext' function begins by testing if a previous error occurred.  By
// testing this condition, we can call 'parseNext' several times, knowing that
// the first error will stop the parse operation.
//..
//  int PercentParser::parseNext(balxml::ErrorInfo *errorInfo)
//  {
//      static const int MAX_LINE = 20;
//
//      if (errorInfo->isAnyError()) {
//          // Don't advance if errorInfo shows a previous error.
//          return -2;
//      }
//..
// The parser skips leading whitespace and lines containing only whitespace.
// It loops until a non-empty line is found:
//..
//      char buffer[MAX_LINE + 1];
//      buffer[0] = '\0';
//
//      // Skip empty lines empty line
//      int len = 0;
//      int startColumn = 0;
//      while (startColumn == len) {
//          ++d_line;
//          d_input->getline(buffer, MAX_LINE + 1, '\n');
//          len = bsl::strlen(buffer);
//..
// The input stream reports that the input line is longer than 'MAX_LINE' by
// setting the fail() condition.  In this case, we set the error object to a
// warning state, indicating the line and column where the problem occurred.
// Then we clear the stream condition and discard the rest of the line.
//..
//          if (MAX_LINE == len && d_input->fail()) {
//              // 20 characters read without encountering newline.
//              // Warn about long line and discard rest of line.
//              errorInfo->setError(balxml::ErrorInfo::BAEXML_WARNING,
//                                  d_line, len, d_docName,
//                                  "Text after 20th column was discarded");
//              d_input->clear();
//              d_input->ignore(INT_MAX, '\n');
//          }
//..
// If we detect an EOF condition, we just return -1.  Otherwise, we skip the
// leading whitespace and go on.
//..
//          else if (0 == len && d_input->eof()) {
//              // Encountered eof before any other characters.
//              return -1;
//          }
//
//          // Skip leading whitespace
//          startColumn = bsl::strspn(buffer, " \t");
//      }
//
//..
// Now we perform two more error checks: one or superfluous characters after
// the integer, the other for an out-of-range integer.  If the 'errorInfo'
// object is already in warning state, either of these errors will overwrite
// the existing warning with the new error condition.
//..
//      char *endp = 0;
//      long result = bsl::strtol(buffer + startColumn, &endp, 10);
//      int endColumn = endp - buffer;
//      if (endColumn < len) {
//          // Conversion did not consume rest of buffer.
//          errorInfo->setError(balxml::ErrorInfo::BAEXML_ERROR,
//                              d_line, endColumn + 1, d_docName,
//                              "Bad input character");
//          return -2;
//      } else if (result < 0 || 100 < result) {
//          // Range error.
//          errorInfo->setError(balxml::ErrorInfo::BAEXML_ERROR,
//                              d_line, startColumn + 1, d_docName,
//                              "Value is not between 0 and 100");
//          return -2;
//      }
//..
// If there were no errors, return the result.  Note that the 'errorInfo'
// object may contain a warning, but warnings typically do not cause a change
// in the error value.
//..
//      return result;
//  }
//..
// The main program uses the 'PercentParser' class to parse a list of values
// and compute the average.  Typically, the data would be stored in a file,
// but we'll use a literal string for demonstration purposes:
//..
//  int main()
//  {
//      static const char INPUTS[] =
//          "    20\n"                  // OK
//          "                   30\n"   // Warning ('0' truncated)
//          "  \n"                      // Skipped: empty line
//          "99x\n"                     // Error: bad character
//          "     101\n"                // Error: out of range
//          "                 1010\n";  // Out-of-range overrides warning
//..
// We convert the string into a stream and initialize the parser.  We name our
// input stream "Inputs" for the purpose of error handling.  We also
// initialize our working variables:
//..
//      bsl::istringstream inputstream(INPUTS);
//      PercentParser parser(&inputstream, "Inputs");
//      int result;
//      int sum = 0;
//      int numValues = 0;
//..
// Any error in parsing will be stored in the 'errorInfo' object.  When first
// constructed, it has a severity of 'BAEXML_NO_ERROR'.
//..
//      balxml::ErrorInfo errorInfo;
//      assert(errorInfo.isNoError());
//..
// Normally, parsing would proceed in a loop.  However, to illustrate the
// different error-handling situations, we have unrolled the loop below.
//
// The first parse succeeds, and no error is reported:
//..
//      result = parser.parseNext(&errorInfo);
//      assert(20 == result);
//      assert(errorInfo.isNoError());
//      sum += result;
//      ++numValues;
//..
// The next parse also succeeds but, because the input line was very long, a
// warning was generated:
//..
//      result = parser.parseNext(&errorInfo);
//      assert(3 == result);  // Truncated at 20th column
//      assert(errorInfo.isWarning());
//      assert(2 == errorInfo.lineNumber());
//      assert(20 == errorInfo.columnNumber());
//      assert("Text after 20th column was discarded" == errorInfo.message());
//      sum += result;
//      ++numValues;
//..
// After resetting the 'errorInfo' object, the we call 'nextParse' again.  This
// time it fails with an error.  The line, column, and source of the error are
// reported in the object.
//..
//      errorInfo.reset();
//      result = parser.parseNext(&errorInfo);
//      assert(-2 == result);
//      assert("Inputs" == errorInfo.source());
//      assert(errorInfo.isError());
//      assert(4 == errorInfo.lineNumber());
//      assert(3 == errorInfo.columnNumber());
//      assert("Bad input character" == errorInfo.message());
//..
// If the 'errorInfo' object is not reset, calling 'parseNext' becomes a
// no-op:
//..
//      result = parser.parseNext(&errorInfo);
//      assert(-2 == result);
//      assert(errorInfo.isError());
//      assert(4 == errorInfo.lineNumber());
//      assert(3 == errorInfo.columnNumber());
//      assert("Bad input character" == errorInfo.message());
//..
// After calling 'reset', the next call to 'parseNext' produces a different
// error message:
//..
//      errorInfo.reset();
//      result = parser.parseNext(&errorInfo);
//      assert(-2 == result);
//      assert(errorInfo.isError());
//      assert(5 == errorInfo.lineNumber());
//      assert(6 == errorInfo.columnNumber());
//      assert("Value is not between 0 and 100" == errorInfo.message());
//..
// The last line of the file contains two problems: a long line, which would
// produce a warning, and a range error, which would produce an error.  The
// warning message is overwritten by the error message because the error has a
// higher severity.  Therefore, on return from 'parseNext', only the error
// message is stored in 'errorInfo' and the warning is lost:
//..
//      errorInfo.reset();
//      result = parser.parseNext(&errorInfo);
//      assert(-2 == result);
//      assert(errorInfo.isError());
//      assert(6 == errorInfo.lineNumber());
//      assert(18 == errorInfo.columnNumber());
//      assert("Value is not between 0 and 100" == errorInfo.message());
//..
// Writing the 'errorInfo' object to a log or file will produce a readable
// error message:
//..
//      bsl::cerr << errorInfo << bsl::endl;
//..
// The resulting message to standard error looks as follows:
//..
//  Inputs:6.18: Error: Value is not between 0 and 100
//..
// Finally, we reach the end of the input stream and can compute our average.
//..
//      errorInfo.reset();
//      result = parser.parseNext(&errorInfo);
//      assert(-1 == result);
//      assert(errorInfo.isNoError());
//
//      int average = sum / numValues;
//      assert(11 == average);  // (20 + 3) / 2
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP  {

namespace balxml {
                              // ===============
                              // class ErrorInfo
                              // ===============
class ErrorInfo
{
    // This class provides detailed information for errors encounted during
    // XML parsing.  Such information is common for most parsers and contains
    // the following data: line number, column number, severity code,
    // identification of source document and the parser error message.

  public:
    // PUBLIC TYPES
    enum Severity
    {
        // Error severity level.  Each severity level is considered more severe
        // than the one before.  Client software will typically continue
        // processing on 'BAEXML_WARNING' and will stop processing on
        // 'BAEXML_ERROR' or 'BAEXML_FATAL_ERROR'.  The distinction between the
        // latter two severities is somewhat arbitrary.  A component that sets
        // the severity of a 'ErrorInfo' object can use 'BAEXML_FATAL_ERROR' to
        // discard a less-severe error with 'BAEXML_ERROR'.  As a general
        // guideline, 'BAEXML_ERROR' means that processing could continue,
        // albeit with compromised results and 'BAEXML_FATAL_ERROR' means that
        // processing could not continue.  For example, a constraint error
        // would typically have 'BAEXML_ERROR' whereas a parsing (syntax) error
        // would have 'BAEXML_FATAL_ERROR'.
        e_NO_ERROR,
        e_WARNING,
        e_ERROR,
        e_FATAL_ERROR
    };

  private:
    // PRIVATE DATA MEMBERS
    Severity          d_severity;
    int               d_lineNumber;
    int               d_columnNumber;
    bsl::string       d_source;
    bsl::string       d_message;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ErrorInfo, bslma::UsesBslmaAllocator);

    // CREATORS
    ErrorInfo(bslma::Allocator *basicAllocator = 0);
        // Construct an error info object using the (optionally) specified
        // 'basicAllocator', or the default allocator of none is specified.
        // After construction, 'severity()' will return 'BAEXML_NO_ERROR',
        // 'lineNumber()' and 'columnNumber()' will each return 0, and
        // 'source()' and 'message()' will each return an empty string.

    ErrorInfo(const ErrorInfo& other, bslma::Allocator *basicAllocator = 0);
        // Construct a copy of the specified 'other' object using the
        // (optionally) specified 'basicAllocator', or the default allocator
        // of none is specified.

    ~ErrorInfo();
        // Destroy this object.

    // MANIPULATORS
    ErrorInfo& operator=(const ErrorInfo& rhs);
        // Copy the value of the specified 'rhs' object into this object and
        // return a modifiable reference to this object.

    void setError(Severity                 severity,
                  int                      lineNumber,
                  int                      columnNumber,
                  const bslstl::StringRef& source,
                  const bslstl::StringRef& errorMsg);
        // If the specified 'severity' is greater than the current value of
        // 'this->severity()', then set this object's severity to 'severity',
        // line number to the specified 'lineNumber', column number to the
        // specified 'columnNumber', source name to the specified 'source', and
        // error message to the specified 'errorMsg', otherwise do nothing.

    void setError(const ErrorInfo& other);
        // If the severity of the specified 'other' object is greater than the
        // current value of 'this->severity()', then assign this object the
        // value of 'other', otherwise do nothing.

    void reset();
        // Reset this object to initial state, as if it were default
        // constructed.

    // ACCESSORS
    bool isNoError() const;
        // Return true if the 'severity() == 'BAEXML_NO_ERROR' and false
        // otherwise.

    bool isWarning() const;
        // Return true if the 'severity() == 'BAEXML_WARNING' and false
        // otherwise.

    bool isError() const;
        // Return true if the 'severity() == 'BAEXML_ERROR' and false
        // otherwise.

    bool isFatalError() const;
        // Return true if the 'severity() == 'BAEXML_FATAL_ERROR' and false
        // otherwise.

    bool isAnyError() const;
        // Return true if the 'severity() >= 'BAEXML_ERROR' (i.e.,
        // 'BAEXML_ERROR' or 'BAEXML_FATAL_ERROR') and false otherwise.

    Severity severity() const;
        // Return the severity level.

    int lineNumber() const;
        // Return the line number of the warning or error.  By convention, the
        // first line is numbered 1.  The constructors and 'reset' functions
        // set the line number to 0, since there is no error line to report.

    int columnNumber() const;
        // Return the column number.  By convention, the first column is
        // numbered 1.  The constructors and 'reset' functions set the column
        // number to 0, since there is no error column to report.

    const bsl::string& source() const;
        // Return the string that identifies the document being parsed.

    const bsl::string& message() const;
        // Return the string describing the error or warning.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// FREE OPERATORS
bool operator==(const ErrorInfo& lhs, const ErrorInfo& rhs);
    // Return true if the specified 'lhs' object has the same value as the
    // specified 'rhs' object.  The two objects have the same value if
    // 'severity()', 'lineNumber()', 'columnNumber()', 'source()', and
    // 'message()' return equal values for both.

inline
bool operator!=(const ErrorInfo& lhs, const ErrorInfo& rhs);
    // Return true if the specified 'lhs' object does not have the same value
    // as the specified 'rhs' object.  The two objects have the same value if
    // 'severity()', 'lineNumber()', 'columnNumber()', 'source()', and
    // 'message()' return equal values for both.

bsl::ostream& operator<<(bsl::ostream& stream, const ErrorInfo& errInfo);
    // Print the specified 'errInfo' object to the specified 'stream' in
    // human-readable form and return a modifiable reference to 'stream'.  The
    // output is one-line without a terminating newline.

// MANIPULATORS
inline void
ErrorInfo::setError(const ErrorInfo& other)
{
    if (other.d_severity > d_severity) {
        *this = other;
    }
}

// ACCESSORS
inline ErrorInfo::Severity
ErrorInfo::severity() const
{
    return d_severity;
}

inline bool
ErrorInfo::isNoError() const
{
    return d_severity == e_NO_ERROR;
}

inline bool
ErrorInfo::isWarning() const
{
    return d_severity == e_WARNING;
}

inline bool
ErrorInfo::isError() const
{
    return d_severity == e_ERROR;
}

inline bool
ErrorInfo::isFatalError() const
{
    return d_severity == e_FATAL_ERROR;
}

inline bool
ErrorInfo::isAnyError() const
{
    return d_severity >= e_ERROR;
}

inline int
ErrorInfo::lineNumber() const
{
    return d_lineNumber;
}

inline int
ErrorInfo::columnNumber() const
{
    return d_columnNumber;
}

inline const bsl::string &
ErrorInfo::source() const
{
    return d_source;
}

inline const bsl::string &
ErrorInfo::message() const
{
    return d_message;
}
}  // close package namespace

// FREE OPERATORS
inline
bool balxml::operator!=(const ErrorInfo& lhs, const ErrorInfo& rhs)
{
    return ! (lhs == rhs);
}

}  // close enterprise namespace

#endif // INCLUDED_BALXML_ERRORINFO

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
