// bdlb_variantprinter.h                                              -*-C++-*-
#ifndef INCLUDED_BDLB_VARIANTPRINTER
#define INCLUDED_BDLB_VARIANTPRINTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a suite of helper classes for printing `bsl::variant`.
//
//@CLASSES:
//  bdlb::VariantPrinter: utility for printing `bsl::variant`
//  bdlb::VariantPrinterUtil: factory for constructing `bdlb::VariantPrinter`
//
//@DESCRIPTION: This component provides utility classes `bdlb::VariantPrinter`
// and `bdlb::VariantPrinterUtil` for printing `bsl::variant`.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Printing `bsl::variant` to a Stream
/// - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to use `bdlb::VariantPrinterUtil` to
// print `bsl::variant` to a stream.
//
// First, we create a streamable type that throws `int` on copies.  This will
// enable us to put a `variant` into a valueless state by copying one into it:
// ```
//  struct ThrowOnCopy {
//      // DATA
//      int d_ii;
//
//      // CREATORS
//      ThrowOnCopy(int ii)
//      : d_ii(ii)
//      {}
//
//      ThrowOnCopy(const ThrowOnCopy& original)
//      : d_ii(original.d_ii)
//      {
//          BSLS_THROW(0);
//      }
//
//      // MANIPULATORS
//      ThrowOnCopy& operator=(const ThrowOnCopy& rhs)
//      {
//          d_ii = rhs.d_ii;
//
//          BSLS_THROW(0);
//
//          return *this;
//      }
//  };
//
//  bsl::ostream& operator<<(bsl::ostream&      stream,
//                           const ThrowOnCopy& value)
//  {
//      return stream << value.d_ii;
//  }
// ```
// Next, we create a shorthand for `VariantPrinterUtil` and our `variant` type:
// ```
//  typedef bdlb::VariantPrinterUtil                    Util;
//  typedef bsl::variant<int, bsl::string, ThrowOnCopy> VariantIST;
// ```
// Then, we declare a variable of `variant` type, which can hold, among other
// types, a `ThrowOnCopy`:
// ```
//  VariantIST        mV(107);
//  const VariantIST& V = mV;
//
//  ThrowOnCopy toc(4);
// ```
// Next, we start doing some output:
// ```
//  cout << toc << endl;  // prints "4\n"
//
//  cout << Util::makePrinter(V) << endl;  // prints "107\n"
//
//  mV = bsl::string("woof");
//  cout << Util::makePrinter(V) << endl;  // prints "woof\n"
//
//  mV = 27;
//  cout << Util::makePrinter(V) << endl;  // prints "27\n"
// ```
// Then, we see that the type `VariantPrinter<...>` returned by
// `makePrinter` has a standard BDE-style `print` accessor that will
// control formatting and indenting:
// ```
//  mV = bsl::string("bow");
//  Util::makePrinter(V).print(cout, 3, -2);  // prints "      bow"
//  mV = bsl::string("wow");
//  Util::makePrinter(V).print(cout, 1, 1);  // prints " wow\n"
// ```
// Now, we assign a `ThrowOnCopy` to the variant, which will throw when
// copied, leaving `mV` in a `valueless` state, and we observe what
// happens when we then print it.
// ```
//  assert(!V.valueless_by_exception());
//  BSLS_TRY
//  {
//      mV = toc;
//      assert(false);
//  }
//  BSLS_CATCH(...)
//  {
//  }
//  assert(V.valueless_by_exception());
//
//  cout << Util::makePrinter(V) << endl;  // prints "(valueless)\n"
// ```
// Finally, we see the output created by all this:
// ```
//  4
//  107
//  woof
//  27
//        bow wow
//  (valueless)
// ```


#include <bdlscm_version.h>

#include <bdlb_printmethods.h>

#include <bsls_assert.h>
#include <bsls_libraryfeatures.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlb {

                      // =============================
                      // struct VariantPrinter_Visitor
                      // =============================

/// Visitor for visiting `t_TYPE` held by a variant and printing it.
struct VariantPrinter_Visitor {
    // DATA
    bsl::ostream *d_stream_p;
    int           d_level;
    int           d_spacesPerLevel;

    // ACCESSORS

    /// Call `print` on `object` using the data fields of this `struct`.
    template <class t_TYPE>
    void operator()(const t_TYPE& object) const;
};

/// Utility for printing `bsl::variant` to standard output streams.  This class
/// has `operator<<` defined for it, so it can be used, for example, in `ball`
/// logs.
template <class t_VARIANT>
class VariantPrinter {
    // DATA
    const t_VARIANT* d_data_p;

  public:
    // CREATORS

    /// Create `VariantPrinter` with the specified `data`.
    explicit
    VariantPrinter(const t_VARIANT *data);

    // ACCESSORS

    /// Format this object to the specified output `stream` at the (absolute
    /// value of) the optionally specified indentation `level` and return a
    /// reference to `stream`.  If `level` is specified, optionally specify
    /// `spacesPerLevel`, the number of spaces per indentation level for this
    /// and all of its nested objects.  If `level` is negative, suppress
    /// indentation of the first line.  If `spacesPerLevel` is negative, format
    /// the entire output on one line, suppressing all but the initial
    /// indentation (as governed by `level`).  If `stream` is not valid on
    /// entry, this operation has no effect.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Write the value of the specified `printer` object to the specified
/// output `stream` in a single-line format, and return a reference to
/// `stream`.  If `stream` is not valid on entry, this operation has no
/// effect.  Note that this human-readable format is not fully specified,
/// can change without notice, and is logically equivalent to:
/// ```
/// print(stream, 0, -1);
/// ```
template <class t_VARIANT>
bsl::ostream&
operator<<(bsl::ostream&                    stream,
           const VariantPrinter<t_VARIANT>& printer);

                          // =========================
                          // struct VariantPrinterUtil
                          // =========================

/// This utility `struct` provides a namespace for a function that creates a
/// `bdlb::VariantPrinter` with its template argument deduced from a given
/// instance of `bsl::variant`.
struct VariantPrinterUtil {
  public:
    // CLASS METHODS

    /// Return an `VariantPrinter` that prints the specified `data`.
    template <class t_VARIANT>
    static VariantPrinter<t_VARIANT> makePrinter(const t_VARIANT& data);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                      // -----------------------------
                      // struct VariantPrinter_Visitor
                      // -----------------------------

template <class t_TYPE>
inline
void VariantPrinter_Visitor::operator()(const t_TYPE& object) const
{
    // Note that we don't return the reference to `*d_stream_p` returned by
    // `PrintMethods::print` because we're called by `bdl::visit` which on
    // C++03 returns a const ref while `VariantPrinter::print` need to return a
    // non-const reference which doesn't compile, so in `VariantPrinter::print`
    // we explicitly return the reference in a separate statement after the
    // call to `visit`.

    bdlb::PrintMethods::print(*d_stream_p, object, d_level, d_spacesPerLevel);
}

                          // ---------------------
                          // struct VariantPrinter
                          // ---------------------

// CREATORS
template <class t_VARIANT>
VariantPrinter<t_VARIANT>::VariantPrinter(const t_VARIANT* data)
: d_data_p(data)
{
    BSLS_ASSERT(data);
}

// ACCESSORS
template <class t_VARIANT>
bsl::ostream& VariantPrinter<t_VARIANT>::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    if (d_data_p->valueless_by_exception()) {
        return bdlb::PrintMethods::print(stream,
                                         "(valueless)",
                                         level,
                                         spacesPerLevel);             // RETURN
    }

    VariantPrinter_Visitor visitor = { &stream, level, spacesPerLevel };

    visit(visitor, *d_data_p);

    // If `visitor()` returns `stream&`, `bsl::visit` on C++03 turns that into
    // a const ref causing compile errors.  So we just have `visitor()` return
    // void and explicitly return `stream` after the `visit` call.

    return stream;
}

                          // -------------------------
                          // struct VariantPrinterUtil
                          // -------------------------

// CLASS METHODS
template <class t_VARIANT>
inline
VariantPrinter<t_VARIANT>
VariantPrinterUtil::makePrinter(const t_VARIANT& data)
{
    return VariantPrinter<t_VARIANT>(&data);
}

}  // close package namespace

// FREE OPERATORS
template <class t_VARIANT>
bsl::ostream& bdlb::operator<<(
                         bsl::ostream&                          stream,
                         const bdlb::VariantPrinter<t_VARIANT>& object)
{
    return object.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif  // INCLUDED_BDLB_VARIANTPRINTER

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
