// bdlde_quotedprintabledecoder.h                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLDE_QUOTEDPRINTABLEDECODER
#define INCLUDED_BDLDE_QUOTEDPRINTABLEDECODER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide automata converting to and from Quoted-Printable encodings.
//
//@CLASSES:
//  bdlde::QuotedPrintableDecoder: automata for Quoted-Printable decoding
//
//@SEE_ALSO: 'bdlde::QuotedPrintableEncoder'
//
//@DESCRIPTION: This component provides a template class (parameterized
// separately on both input and output iterators) that can be used to decode
// byte sequences of arbitrary length from the Quoted Printable representation
// described in Section 6.7 "Quoted-Printable Content Transfer Encoding" of RFC
// 2045, "Multipurpose Internet Mail Extensions (MIME) Part One: Format of
// Internet Message Bodies."
//
// Each instance of the decoder retains the state of the conversion from one
// supplied input to the next, enabling the processing of segmented input --
// i.e., processing resumes where it left off with the next invocation on new
// input.  Instance methods are provided for the decoder to (1) assert the end
// of input, (2) determine whether the input so far is currently acceptable,
// and (3) indicate whether a non-recoverable error has occurred.
//
///Quoted-Printable Decoding
///-------------------------
// (In the following, all rules mentioned refer to those listed in the encoder
// section above.)
//
// The decoding process for this encoding scheme involves:
//
//: 1 transforming any encoded character triplets back into their original
//:   representation (rule #1 and rule #4).
//:
//: 2 literally writing out characters that have not been changed (rule #2).
//:
//: 3 deleting any trailing whitespace at the end of an encoded line (rule #3).
//
//: 4 removing the soft line breaks including the '=' prefix (i.e.,
//:   concatenating broken sentences) (rule #5).
//
// The standard imposes a maximum of 76 characters exclusive of CRLF; however,
// the decoder implemented in this component will handle lines of arbitrary
// length.
//
// The decoder also provides support for 2 error-reporting modes: the strict
// mode and the relaxed mode (configurable at construction).  A strict-mode
// decoder stops decoding at the first offending character encountered, while a
// relaxed-mode decoder would continue decoding to the end of the input,
// allowing straight pass-through of character sets that cannot be interpreted.
//
// The following kinds of errors can be encountered during decoding, listed in
// order of decreasing order of precedence:
//..
//  E1. BAD_DATA
//..
// An '=' character is not followed by either two uppercase hexadecimal digits,
// or a soft line break -- e.g.,
//..
//   '=4=' (only one hexadecimal)
//   '=K3' (K3 is not a hexadecimal number)
//   '=1f' (lower case f is a literally encoded character)
//..
//
// Note that:
//
//: 1 In the relaxed error-reporting mode of this implementation, lowercase
//:   hexadecimal digits are treated as valid numerals.
//:
//: 2 E1 can be caused by a missing or corrupted numeric, a corrupted character
//:   disguised as an '=', or an accidental insertion of a '=' that does not
//:   belong.
//:
//: 3 The case where a seemingly valid character is found in place of a missing
//:   numeric cannot be detected, e.g., '=4F' where 'F' is actually a literally
//:   encoded character.
//:
//: 4 An erroneous occurrence of a '=' character preceding 2 seemingly valid
//:   hexadecimal numerics is also undetectable, e.g., '=4F' where '=' was
//:   actually a 't' corrupted during transmission.
//..
//  E2. BAD_LINEBREAK
//..
// A '\r' is not followed by a '\n'.  In the relaxed mode, each stand-alone
// '\r' or '\n' will be copied straight through to the output.  For soft line
// breaks, whitespace is ignored between the '=' character and the CRLF as they
// are to be treated and removed as transport padding.
//..
//  E3. BAD_LINELENTH
//..
// An encoded line exceeds the specified maximum line length with missing soft
// line breaks.  (Because input of flexible line lengths is allowed in this
// implementation, this error is not detected or reported.)
//
// In the relaxed-mode, errors of the types E1 and E2 would be copied straight
// to output and type E3 ignored.  Decoded lines will be broken even when a
// bare CRLF is encountered in this mode.  Users can still be alerted to the
// the unreported errors as offending characters are copied straight through to
// the output stream, which can be observed.
//
// The 'isError' method is used to detect the above anomalies, while for the
// 'convert' method, a 'numIn' output parameter (indicating the number of input
// characters consumed) or possibly the iterator itself (for iterators with
// reference-semantics) identifies the offending character.
//
///Usage
///- - -
// TBD

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_QUEUE
#include <bsl_queue.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace bdlde {

class QuotedPrintableDecoder {
    // This class implements a mechanism capable of converting data of
    // arbitrary length from its corresponding Quoted-Printable representation.

    // PRIVATE TYPES
    enum {
        // Symbolic state values.

        e_ERROR_STATE        = -1, // input is irreparably invalid
        e_INPUT_STATE        =  0, // general input state
        e_SAW_EQUAL_STATE    =  1, // need two hexadecimal values or CR LF
        e_SAW_WS_STATE       =  2, // saw a whitespace
        e_NEED_HEX_STATE     =  3, // need one hexadecimal value
        e_NEED_SOFT_LF_STATE =  4, // need soft new line
        e_NEED_HARD_LF_STATE =  5, // need soft new line
        e_DONE_STATE         =  6  // any additional input is an error
    };

  public:

    enum EquivalenceClasses {
        // This enumeration type enumerates the input equivalence classes.
        // Separate enums are given to variants resulting from different modes
        // of operation to eliminate an extra step of mode checking inside the
        // main decoding loop.

                       // Regular character - copy straight to output
        e_RC_ = 0, // strict mode
        e_RC,      // relaxed mode

                       // Hexadecimal digit - numeral only when preceded by
                       // '='; otherwise a regular character
        e_HX_,     // strict mode
        e_HX,      // relaxed mode

                       // '=' - wait for more input
        e_EQ_,     // strict mode
        e_EQ,      // relaxed mode

                       // Whitespace        - buffer; wait for more input
        e_WS_,     // strict mode
        e_WS,      // relaxed mode

                       // Carriage return
        e_CR_,     // strict mode       - wait for further input
        e_CR,      // relaxed mode      - wait for further input

                       // Line Feed Strict mode
                       // ------------
        e_LC_,     // CRLF_MODE         - decode to "\r\n" if preceded by
                       // '\r'; report error otherwise
        e_LL_,     // LF_MODE           - decode to '\n' if preceded by
                       // '\r' report error otherwise Relaxed mode
                       // ------------
        e_LC,      // CRLF_MODE         - decode to "\r\n" if preceded by
                       // '\r'; ignore otherwise
        e_LL,      // LF_MODE           - decode to "\n" if preceded by
                       // '\r'; ignore otherwise

                       // Unrecognized char - halt and report error
        e_UC_,     // strict mode       - Ignore and halt decoding
        e_UC       // relaxed mode      - Ignore but continue decoding
    };

    enum LineBreakMode {
        // Configuration governing how line breaks are decoded.

        e_CRLF_MODE,       // "\r\n" are decoded to "\r\n".
        e_LF_MODE          // "\r\n" are decoded to "\n".
    };

    // CLASS DATA
    static const char s_componentName[];
        // Name of component used when reporting errors.

    static const bool s_defaultUnrecognizedIsErrorFlag;
        // Default error reporting mode

    static const char *s_defaultEquivClassStrict_p;
        // Default map of 'unsigned char' to equivalence class for strict mode

    static const char *s_defaultEquivClassCRLF_p;
        // Default map of 'unsigned char' to equivalence class for CRLF line
        // break mode

    static const unsigned char *const s_decodingMap_p;
        // Character map used for converting an ASCII character to the
        // hexadecimal value it is representing.

    static const int   s_defaultMaxLineLength; // Default maximum line length
    static const char* s_lineBreakModeName[];  // Names of line break mode

    // INSTANCE DATA
    bool d_unrecognizedIsErrorFlag;  // If true, fail on "bad" characters
    LineBreakMode d_lineBreakMode;   // Line break mode
    int d_state;  // TBD doc
    char d_buffer[90]; // TBD doc
    int d_bufferLength; // TBD doc
    char d_hexBuffer; // TBD doc
    int d_outputLength;   // Total number of output characters
    char *d_equivClass_p; // Map of 'unsigned char' to input equivalence class;
                          // dynamically allocated because there is no default
                          // complete configuration.

  private:
    // NOT IMPLEMENTED
    QuotedPrintableDecoder(const QuotedPrintableDecoder&);
    QuotedPrintableDecoder& operator=(const QuotedPrintableDecoder&);

  public:
    // CLASS METHODS
    static const char* lineBreakModeToAscii(LineBreakMode mode);
        // Return the ASCII string describing the specified 'mode' governing
        // the decoding of hard linebreaks ("\r\n").  The behavior is undefined
        // unless 'mode' is either e_CRLF_MODE or e_LF_MODE.

    // CREATORS
    explicit
    QuotedPrintableDecoder(
        bool                                  detectError,
        QuotedPrintableDecoder::LineBreakMode lineBreakMode =
                                          QuotedPrintableDecoder::e_CRLF_MODE);
        // Create a Quoted-Printable decoder in the initial state, set to the
        // strict or relaxed error-reporting mode according to whether the
        // specified 'detectError' flag is 'true' or 'false', respectively, and
        // also configured to the specified 'lineBreakMode'.  The behavior is
        // undefined unless 'lineBreakMode' is either e_CRLF_MODE or
        // e_LF_MODE.  Note that the decoder reports errors in the strict
        // mode and output offending characters in the relaxed mode.  Hard line
        // breaks ("\r\n") are decoded to "\r\n" in e_CRLF_MODE (default)
        // and to '\n' in e_LF_MODE.

    ~QuotedPrintableDecoder();
        // Destroy this object.

    // MANIPULATORS
    int convert(char       *out,
                int        *numOut,
                int        *numIn,
                const char *begin,
                const char *end,
                int         maxNumOut = -1);
        // Append to the buffer addressed by the specified 'out' all pending
        // output (if there is any) up to the optionally specified 'maxNumOut'
        // limit (default is negative, meaning no limit) and, when there is no
        // pending output and 'maxNumOut' is still not reached, begin to
        // consume and decode a sequence of input characters starting at the
        // specified 'begin' position, up to but not including the specified
        // 'end' position, writing any resulting output in the specified
        // 'output' buffer up to the (cumulative) 'maxNumOut' limit.  If
        // 'maxNumOut' limit is reached, no further input will be consumed.
        // Load into the specified 'numOut' and 'numIn' the number of output
        // bytes produced and input bytes consumed, respectively.  Return a
        // non-negative value on success and a negative value otherwise.  A
        // successful return status indicates the number of characters that
        // would be output if 'endConvert' were called with no output limit
        // immediately upon exit from this method.  These bytes are also
        // available for output if this method is called with a sufficiently
        // large 'maxNumOut'.  Note that calling this method after 'endConvert'
        // has been invoked without an intervening 'reset' call will place this
        // instance in an error state, and return an error status.  Note also
        // that it is recommended that after all calls to 'convert' are
        // finished, the 'endConvert' method be called to complete the decoding
        // of any unprocessed input characters (e.g., whitespace).

    int endConvert(char *out, int *numOut, int maxNumOut = -1);
        // Terminate encoding for this decoder; write any retained output
        // (e.g., from a previous call to 'convert' with a non-zero 'maxNumOut'
        // argument) to the specified 'out' buffer.  Optionally specify the
        // 'maxNumOut' limit on the number of bytes to output; if 'maxNumOut'
        // is negative, no limit is imposed.  Load into the specified 'numOut'
        // the number of output bytes produced.  Return 0 on success with no
        // pending output, the positive number of bytes (if any) that would be
        // output if 'endConvert' were called with no output limit immediately
        // upon exit from this method, and a negative value otherwise.  Any
        // retained bytes are available on a subsequent call to 'endConvert'.
        // Once this method is called, no additional input may be supplied
        // without an intervening call to 'reset'; once this method returns a
        // zero status, a subsequent call will place this decoder in the error
        // state, and return an error status.

    void reset();
        // Reset this decoder to its initial state (i.e., as if no input had
        // been consumed).

    // ACCESSORS
    bool isAccepting() const;
        // Return 'true' if the input read so far by this decoder is considered
        // syntactically complete and all resulting output has been emitted;
        // return 'false' otherwise.  Note that there must not be any
        // unprocessed characters accumulated in the input buffer of this
        // decoder.

    bool isDone() const;
        // Return 'true' if this decoder is in the done state (i.e.,
        // 'endConvert' has been called and any additional input will result in
        // an error), and if there is no pending output; return 'false'
        // otherwise.

    bool isError() const;
        // Return 'true' if this decoder has encountered an irrecoverable error
        // and 'false' otherwise.  An irrecoverable error is one for which
        // there is no subsequent possibility of achieving an "acceptable"
        // result (as defined by the 'isAccepting' method).

    bool isInitialState() const;
        // Return 'true' if this decoder is in the initial state (i.e., as if
        // no input had been consumed) and 'false' otherwise.

    bool isMaximal() const;
        // Return 'true' if the input to this decoder is maximal (i.e., the
        // input contains an end-of-input sentinel, signaling that no further
        // input should be expected).  *Always* returns 'false' for
        // Quoted-Printable decoders since the encoding scheme does not specify
        // an end-of-input sentinel.

    bool isUnrecognizedAnError() const;
        // Return 'true' if this decoder is currently configured to detect an
        // error when an unrecognizable encoding is encountered, and 'false'
        // otherwise.

    LineBreakMode lineBreakMode() const;
        // Return the line break mode specified for this decoder.

    int numOutputPending() const;
        // Return the number of output bytes retained by this decoder and not
        // emitted because 'maxNumOut' has been reached.

    int outputLength() const;
        // Return the total length of the output emitted by this decoder
        // (possibly after several calls to the 'convert' or the 'input'
        // methods) since its initial construction or the latest 'reset'.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
//
// ============================================================================

// CLASS METHODS
inline
const char* QuotedPrintableDecoder::lineBreakModeToAscii(
    LineBreakMode mode)
{
    return s_lineBreakModeName[mode];
}

// CREATORS
inline
QuotedPrintableDecoder::QuotedPrintableDecoder(
                 bool                                  unrecognizedIsErrorFlag,
                 QuotedPrintableDecoder::LineBreakMode lineBreakMode)
: d_unrecognizedIsErrorFlag(unrecognizedIsErrorFlag)
, d_lineBreakMode(lineBreakMode)
, d_state(e_INPUT_STATE)
, d_bufferLength(0)
, d_outputLength(0)
{
    if (unrecognizedIsErrorFlag) {
        // Strict mode
        d_equivClass_p = const_cast<char*>(s_defaultEquivClassStrict_p);
    }
    else {
        if (lineBreakMode == e_CRLF_MODE) {
            d_equivClass_p = const_cast<char*>(s_defaultEquivClassCRLF_p);
        }
        else {
            // First copy the map of equivalence classes for the
            // e_CRLF_MODE to the strict error-report mode.

            int len = sizeof(*s_defaultEquivClassCRLF_p) * 256;
            d_equivClass_p = new char[len];
            bsl::memcpy(d_equivClass_p, s_defaultEquivClassCRLF_p, len);
            d_equivClass_p['\n'] = e_LL;  // output '\n' instead if preceded
                                              // by '='.
        }
    }
}

// MANIPULATORS
inline
void QuotedPrintableDecoder::reset()
{
    d_state = e_INPUT_STATE;
    d_outputLength = 0;
    d_bufferLength = 0;
}

// ACCESSORS
inline
bool QuotedPrintableDecoder::isAccepting() const
{
    return e_INPUT_STATE == d_state || e_DONE_STATE == d_state;
}

inline
bool QuotedPrintableDecoder::isDone() const
{
    return e_DONE_STATE == d_state && 0 == d_bufferLength;
}

inline
bool QuotedPrintableDecoder::isError() const
{
    return e_ERROR_STATE == d_state;
}

inline
bool QuotedPrintableDecoder::isInitialState() const
{
    return e_INPUT_STATE == d_state && 0 == d_outputLength;
}

inline
bool QuotedPrintableDecoder::isMaximal() const
{
    return false;
}

inline
bool QuotedPrintableDecoder::isUnrecognizedAnError() const
{
    return d_unrecognizedIsErrorFlag;
}

inline
QuotedPrintableDecoder::LineBreakMode
QuotedPrintableDecoder::lineBreakMode() const
{
    return d_lineBreakMode;
}

inline
int QuotedPrintableDecoder::numOutputPending() const
{
    return d_bufferLength;
}

inline
int QuotedPrintableDecoder::outputLength() const
{
    return d_outputLength;
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
