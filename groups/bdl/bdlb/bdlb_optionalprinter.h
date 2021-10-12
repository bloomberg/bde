// bdlb_optionalprinter.h                                             -*-C++-*-
#ifndef INCLUDED_BDLB_OPTIONALPRINTER
#define INCLUDED_BDLB_OPTIONALPRINTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a suite of helper classes for printing 'bsl::optional'.
//
//@CLASSES:
//  bdlb::OptionalPrinter: utility for printing 'bsl::optional'
//  bdlb::OptionalPrinterUtil: factory for constructing 'bdlb::OptionalPrinter'
//
//@DESCRIPTION: This component provides utility classes 'bdlb::OptionalPrinter'
// and 'bdlb::OptionalPrinterUtil' for printing 'bsl::optional'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Printing 'bsl::optional' to a stream
///- - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to use 'bdlb::OptionalPrinterUtil' to
// print 'bsl::optional' to a stream:
//..
//  bsl::optional<int> value(42);
//  bsl::cout << bdlb::OptionalPrinterUtil::makePrinter(value);
//..

#include <bdlscm_version.h>

#include <bdlb_printmethods.h>

#include <bsls_assert.h>

#include <bsl_optional.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlb {

                          // ======================
                          // struct OptionalPrinter
                          // ======================
template <class TYPE>
class OptionalPrinter {
    // Utility for printing 'bsl::optional' to standard output streams.  This
    // class has 'operator<<' defined for it, so it can be used, for example,
    // in 'ball' logs.

    // DATA
    const bsl::optional<TYPE>* d_data_p;

  public:
    // CREATORS
    explicit OptionalPrinter(const bsl::optional<TYPE> *data);
        // Create 'OptionalPrinter' with the specified 'data'.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream&              stream,
                        int                        level          = 0,
                        int                        spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
template <class TYPE>
bsl::ostream&
operator<<(bsl::ostream& stream, const OptionalPrinter<TYPE>& printer);
    // Write the value of the specified 'printer' object to the specified
    // output 'stream' in a single-line format, and return a reference to
    // 'stream'.  If 'stream' is not valid on entry, this operation has no
    // effect.  Note that this human-readable format is not fully specified,
    // can change without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

                          // ==========================
                          // struct OptionalPrinterUtil
                          // ==========================

struct OptionalPrinterUtil {
    // This utility 'struct' provides a namespace for a function that creates a
    // 'bdlb::OptionalPrinter' with its template argument deduced from a given
    // instance of 'bsl::optional'.
  public:
    // CLASS METHODS
    template <class TYPE>
    static OptionalPrinter<TYPE> makePrinter(const bsl::optional<TYPE>& data);
        // Return an 'OptionalPrinter' that prints the specified 'data'.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // ----------------------
                          // struct OptionalPrinter
                          // ----------------------

// CREATORS
template <class TYPE>
OptionalPrinter<TYPE>::OptionalPrinter(const bsl::optional<TYPE>* data)
: d_data_p(data)
{
    BSLS_ASSERT(data);
}

// ACCESSORS
template <class TYPE>
bsl::ostream& OptionalPrinter<TYPE>::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    if (!d_data_p->has_value()) {
        return bdlb::PrintMethods::print(stream,
                                         "NULL",
                                         level,
                                         spacesPerLevel);             // RETURN
    }

    return bdlb::PrintMethods::print(stream,
                                     *(*d_data_p),
                                     level,
                                     spacesPerLevel);
}

                          // --------------------------
                          // struct OptionalPrinterUtil
                          // --------------------------

// CLASS METHODS
template <class TYPE>
OptionalPrinter<TYPE>
OptionalPrinterUtil::makePrinter(const bsl::optional<TYPE>& data)
{
    return OptionalPrinter<TYPE>(&data);
}

}  // close package namespace

// FREE OPERATORS
template <class TYPE>
bsl::ostream& bdlb::operator<<(bsl::ostream&                      stream,
                               const bdlb::OptionalPrinter<TYPE>& object)
{
    return object.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif  // INCLUDED_BDLB_OPTIONALPRINTER

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
