// bdld_datumutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLD_DATUMUTIL
#define INCLUDED_BDLD_DATUMUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide extra functions that operate on `bdld::Datum` objects.
//
//@CLASSES:
//  bdld::DatumUtil: namespace for extra functions for `bdld::Datum`
//
//@SEE_ALSO: bdld_datum
//
//@DESCRIPTION: This component provides a struct, `bdld::DatumUtil` that serves
// as a namespace for utility functions that operate on `bdld::Datum` objects.
// The functions provided are `typedPrint` and `safeTypedPrint`.  They are a
// variation on the standard BDE-style `print` function but print the `Datum`
// values in a manner that disambiguates the type when the value itself would
// not do so.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Showing the Difference Between an Integer and a Double Value
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are testing a system with operations that result in `bdld::Datum`
// values.  We verify that those results are what we have expected, *including*
// that their type matches.  After a getting an unexpected value, we use normal
// printing and get the following test failure: "Expected 1, got 1".
// Obviously, the type difference is not visible.  Instead, we can use
// `bdld::DatumUtil::typedPrint` to display the type as well as the value.
//
// First, let us define two `bdld::Datum` objects that have the same value, but
// use different types to represent them:
// ```
// bdld::Datum expected = bdld::Datum::createInteger(1);
// bdld::Datum actual   = bdld::Datum::createDouble(1.);
//
// assert(expected != actual);
// ```
// Next, we demonstrate that printing these results in the same printout:
// ```
// bsl::ostringstream os;
//
// os << expected;
// bsl::string expectedStr = os.str();
//
// os.str("");
// os.clear();
// os << actual;
// bsl::string actualStr = os.str();
//
// assert(expectedStr == actualStr);  // "1" is equal to "1"
// ```
//  Then, we create a shorthand for `bdld::DatumUtil::typedPrint`:
// ```
// void printWithType(bsl::ostream& outStream, const bdld::Datum& object)
// {
//     bdld::DatumUtil::typedPrint(outStream, object, 0, -1);
// }
// ```
// The 0 `level` and -1 `spacesPerLevel` results in single-line printout
// without a trailing newline, just like the stream output operator works.
//
// Finally, we verify that now we get a different printout for the two values:
// ```
// os.str("");
// os.clear();
// printWithType(os, expected);
// expectedStr = os.str();
//
// os.str("");
// os.clear();
// printWithType(os, actual);
// actualStr = os.str();
//
// assert(expectedStr != actualStr);  // "1i" is *not* equal to "1."
// ```
//
///Example 2: Avoiding Endless Printing of Data with Cycles
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we are testing a system that creates a complex data structure
// that it stores is `bdld::Datum` objects.  Suppose that such a system doesn't
// use the fail-safe `Datum` builders for optimization purposes (for example it
// stores all map entries in one big allocation), and so it may be able to
// create a self-referential data structure.
//
// It is not easy to legitimately create self-referential data structures so we
// won't even attempt it in a short example code.
//
// First, we use a `bdld::DatumMaker` with a local allocator so we can ignore
// any cleanup and allocation:
// ```
// bdlma::LocalSequentialAllocator<1024> lsa;
// bdld::DatumMaker dm(&lsa);
// ```
// Next, we create two array datums with a Nil element each:
// ```
// bdld::Datum arr1 = dm.a(dm());
// bdld::Datum arr2 = dm.a(dm());
// ```
// Then, we circumvent the type system to initialize their single elements to
// "contain" each other:
// ```
// const_cast<bdld::Datum&>(arr1.theArray()[0]) = arr2;
// const_cast<bdld::Datum&>(arr2.theArray()[0]) = arr1;
// ```
// Finally, we use the safe printing on this trapdoor of an endless loop to
// nevertheless safely print them:
// ```
// bsl::ostringstream os;
// bdld::DatumUtil::safeTypedPrint(os, arr1);
// ```
// Were we to print the results standard out, say
// ```
// bdld::DatumUtil::safeTypedPrint(cout, arr2);
// ```
// we would see something akin to:
// ```
// <array@000000EFE4CFF928[
//     <array@000000EFE4CFF908[
//         <array@000000EFE4CFF928[!CYCLE!]>
//     ]>
// ]>
// ```
// The hexadecimal numbers above identify the arrays (and maps or int-maps) so
// we can clearly see that the cycle "points" back to the top-level array.

#include <bdlscm_version.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdld {

class Datum;
                          // ================
                          // struct DatumUtil
                          // ================

/// This struct serves as a namespace for utility functions that operate on
/// `bdld::Datum` objects.
struct DatumUtil {

    // CLASS METHODS

    /// Write the value of the specified `object` to the specified
    /// `outputStream` in a human-readable format that is non-ambiguous
    /// regarding the type of the `bdld::Datum` value printed, and return a
    /// reference to the modifiable `outputStream`.  Optionally specify an
    /// initial indentation `level`, whose absolute value is incremented
    /// recursively for nested objects.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, whose absolute value indicates the number
    /// of spaces per indentation level for this and all of its nested
    /// objects.  If `level` is negative, suppress indentation of the first
    /// line.  If `spacesPerLevel` is negative, format the entire output on
    /// one line, suppressing all but the initial indentation (as governed
    /// by `level`).  If `stream` is not valid on entry, this operation has
    /// no effect.  Note that `safeTypedPrint` explicitly allows self-
    /// referential data structures, or in other words data structures with
    /// cycles, to be printed.  When a cycle is detected, that subgraph is
    /// not printed again.  Also note that the human-readable format is not
    /// fully specified, and can change without notice.
    static bsl::ostream& safeTypedPrint(bsl::ostream& outputStream,
                                        const Datum&  object,
                                        int           level = 0,
                                        int           spacesPerLevel = 4);

    /// Write the value of the specified `object` to the specified
    /// `outputStream` in a human-readable format that is non-ambiguous
    /// regarding the type of the `bdld::Datum` value printed, and return a
    /// reference to the modifiable `outputStream`.  Optionally specify an
    /// initial indentation `level`, whose absolute value is incremented
    /// recursively for nested objects.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, whose absolute value indicates the number
    /// of spaces per indentation level for this and all of its nested
    /// objects.  If `level` is negative, suppress indentation of the first
    /// line.  If `spacesPerLevel` is negative, format the entire output on
    /// one line, suppressing all but the initial indentation (as governed
    /// by `level`).  If `stream` is not valid on entry, this operation has
    /// no effect.  The behavior is undefined unless the data structure
    /// represented by `object` is a graph without any cycles.  In case of
    /// an untrusted data structure, use `safeTypedPrint`.  Note that the
    /// human-readable format is not fully specified, and can change without
    /// notice.
    static bsl::ostream& typedPrint(bsl::ostream& outputStream,
                                    const Datum&  object,
                                    int           level = 0,
                                    int           spacesPerLevel = 4);
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
