// ball_recordformatterfunctor.h                                      -*-C++-*-
#ifndef INCLUDED_BALL_RECORDFORMATTERFUNCTOR
#define INCLUDED_BALL_RECORDFORMATTERFUNCTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a typedef for the record formatter functor.
//
//@CLASSES:
//  ball::RecordFormatterFunctor: struct containing formatter functor typedef
//
//@SEE_ALSO: ball_record, ball_recordformatterregistryutil
//
//@DESCRIPTION: This component provides a `struct`,
// `ball::RecordFormatterFunctor`, that contains a single typedef, `Type`,
// which is the type of the functor used for formatting log records to a
// stream.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Defining a Record Formatter Function
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a simple function that formats log records.  We
// can use `ball::RecordFormatterFunctor::Type` to declare a functor that
// conforms to the expected signature.
//
// First, we include the necessary headers and define a simple formatting
// function:
// ```
//  void myFormatter(bsl::ostream& stream, const ball::Record& record)
//  {
//      stream << "Log: " << record.fixedFields().message();
//  }
// ```
// Then, we can assign this function to a variable of the functor type:
// ```
//  ball::RecordFormatterFunctor::Type formatter = &myFormatter;
// ```
// Finally, we can use this functor to format a record:
// ```
//  ball::RecordAttributes attr;
//  attr.setMessage("Hello");
//  ball::Record record(attr, ball::UserFields());
//
//  bsl::ostringstream oss;
//  formatter(oss, record);
//  assert(oss.str() == "Log: Hello");
// ```

#include <balscm_version.h>

#include <bsl_functional.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ball {

class Record;

                       // =============================
                       // struct RecordFormatterFunctor
                       // =============================

/// This struct provides a namespace for a typedef of the functor used for
/// formatting log records to a stream.
struct RecordFormatterFunctor {

    /// `Type` is the type of the functor used for formatting log records
    /// to a stream.
    typedef bsl::function<void(bsl::ostream&, const Record&)> Type;
};

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BALL_RECORDFORMATTERFUNCTOR

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
