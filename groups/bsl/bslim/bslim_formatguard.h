// bslim_formatguard.h                                                -*-C++-*-
#ifndef INCLUDED_BSLIM_FORMATGUARD
#define INCLUDED_BSLIM_FORMATGUARD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a guard for saving the state of a stream object.
//
//@CLASSES:
//  bslim::FormatGuard: format guard for stream types.
//
//@DESCRIPTION: The `bslim::FormatGuard` type saves the configuration of a
// `basic_ostream` type, when the guard is created.  When the guard is
// destroyed, the stream is restored to the state it was in when the guard was
// created.
//
// The state that is saved is
// * The `fmtflags` state
// * The floating-point precision
// * The fill char
//
// Note that the `width` field is not saved, because it does not normally
// persist among multiple items output, but automatically resets to 0 after a
// single item is ouput.
//
///Usage
///-----
// In the following example we illustrate the usage of `FormatGuard`.
//
///Example 1: Saving Stream State to be Restored Later:
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to do some output to a stream for which we must change the
// state of the stream.
//
// First, we declare our stream:
// ```
// bsl::ostringstream oss;
// ```
// Then, we use a `FormatGuard` to save the state of `oss` before we
// reconfigure it, so that when the `FormatGuard` is destroyed it will resotre
// `oss` to its original state.
// ```
// {
//     bslim::FormatGuard guard(&oss);
// ```
// Then, we reconfigure out stream and do some output:
// ```
//     oss << "First line in hex: " << bsl::showbase << bsl::hex << 80 <<
//                                                                       endl;
// ```
// Next, we leave the block and the destructor of `guard` will restore `oss`
// to its original configuration:
// ```
// }
// ```
// Now, we do another line of output:
// ```
// oss << "Second line in decimal: " << 123 << endl;
// ```
// Finally, we observe that our guarded output was in hex, as desired, and the
// output afterward was in decimal, as desired:
// ```
// assert(oss.str() == "First line in hex: 0x50\n"
//                     "Second line in decimal: 123\n");
// ```

#include <bslscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_isintegral.h>
#include <bsls_keyword.h>
#include <bsls_types.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {

namespace bslim {

                              // =================
                              // class FormatGuard
                              // =================

/// This class implements a guard that saves the state of a `basic_ostream`
/// and restores that state upon destruction of the guard.
class FormatGuard {

    // PRIVATE TYPES
    typedef void (FormatGuard::*DestructorImpl_p)();

    // DATA
    bsl::ios_base * const          d_iosBase_p;        // base class of stream

    const bsl::ios_base::fmtflags  d_flags;            // stream format flags

    const bsl::streamsize          d_precision;        // precision of ostream

    const bsls::Types::Int64       d_fillChar;         // fill char of ostream

    const DestructorImpl_p         d_destructorImpl_p; // method pointer to
                                                       // implementation of
                                                       // destructor

  private:
    // NOT IMPLEMENTED
    FormatGuard(const FormatGuard&) BSLS_KEYWORD_DELETED;
    FormatGuard operator=(const FormatGuard&) BSLS_KEYWORD_DELETED;

  private:
    // PRIVATE MANIPULATORS

    /// Restore the format flags, precision, and fill character to the
    /// stream that was passed to the constructor.
    template <class CHAR_TYPE, class CHAR_TRAITS>
    void ostreamDestructorImpl();

  public:
    // CREATORS

    /// Create a `FormatGuard` object saving the state of the specified
    /// `stream`.
    template <class CHAR_TYPE, class CHAR_TRAITS>
    explicit
    FormatGuard(bsl::basic_ostream<CHAR_TYPE, CHAR_TRAITS> *stream);

    /// Restore all the saved state to the stream that was passed to the
    /// constructor.
    ~FormatGuard();
};

                              // -----------------
                              // class FormatGuard
                              // -----------------

// PRIVATE MANIPULATORS
template <class CHAR_TYPE, class CHAR_TRAITS>
void FormatGuard::ostreamDestructorImpl()
{
    // Note that 'basic_ostream' inherits from 'basic_ios' through virtual
    // inheritance, so we cannot cast from an 'ios_base *' to a
    // 'basic_ostream *'.  However, we can restore 100% of the state that we
    // intend to using a 'basic_ios *', which inherits non-virtually from
    // 'ios_base *'.

    typedef bsl::basic_ios<CHAR_TYPE, CHAR_TRAITS>    BasicIos;

    BasicIos *basicIos_p = static_cast<BasicIos *>(d_iosBase_p);

    basicIos_p->flags(d_flags);
    basicIos_p->precision(d_precision);
    basicIos_p->fill(static_cast<CHAR_TYPE>(d_fillChar));
}

// CREATORS
template <class CHAR_TYPE, class CHAR_TRAITS>
inline
FormatGuard::FormatGuard(bsl::basic_ostream<CHAR_TYPE, CHAR_TRAITS> *stream)
: d_iosBase_p(stream)
, d_flags(stream->flags())
, d_precision(stream->precision())
, d_fillChar(stream->fill())
, d_destructorImpl_p(
                   &FormatGuard::ostreamDestructorImpl<CHAR_TYPE, CHAR_TRAITS>)
{
    BSLMF_ASSERT(bsl::is_integral<CHAR_TYPE>::value);
    BSLMF_ASSERT(sizeof(CHAR_TYPE) <=
                 sizeof(bsls::Types::Int64));  // 'd_fillChar'
}

inline
FormatGuard::~FormatGuard()
{
    (this->*d_destructorImpl_p)();
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
