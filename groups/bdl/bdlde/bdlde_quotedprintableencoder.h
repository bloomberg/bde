// bdlde_quotedprintableencoder.h                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLDE_QUOTEDPRINTABLEENCODER
#define INCLUDED_BDLDE_QUOTEDPRINTABLEENCODER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide automata converting to and from Quoted-Printable encodings.
//
//@CLASSES:
// bdlde::QuotedPrintableEncoder: automaton for Quoted-Printable encoding
//
//@SEE_ALSO: 'bdlde::QuotedPrintableDecoder'
//
//@DESCRIPTION: This component provides a class that can be used to encode byte
// sequences of arbitrary length into the Quoted Printable representation
// described in Section 6.7 "Quoted-Printable Content Transfer Encoding" of RFC
// 2045, "Multipurpose Internet Mail Extensions (MIME) Part One: Format of
// Internet Message Bodies."
//
// Each instance of the encoder retains the state of the conversion from one
// supplied input to the next, enabling the processing of segmented input --
// i.e., processing resumes where it left off with the next invocation on new
// input.  Instance methods are provided for the encoder to (1) assert the end
// of input, (2) determine whether the input so far is currently acceptable,
// and (3) indicate whether a non-recoverable error has occurred.
//
///Quoted-Printable Encoding
///-------------------------
// This encoding scheme is suitable for encoding arbitrary data consisting
// primarily of printable text characters.  Additionally, this scheme seeks to
// preserve the integrity of the byte stream during transfer by making it
// difficult for any intermediate interpreting software in the path of the
// transfer to disruptively change its content (e.g., because of trailing
// whitespace and line breaks).  For binary data, Base64 encoding may be a more
// appropriate scheme (see 'bdede_base64').
//
// The data stream is processed one byte at a time from left to right as
// follows:
//
///General 8-Bit Representation
/// - - - - - - - - - - - - - -
// Any 8-bit input character, except a CR or LF, *may* be represented by an "="
// followed by a 2-digit hexadecimal representation of its ASCII value.  Only
// uppercase hexadecimal digits are allowed.  For example, the letter 'n' can
// be encoded into '=6E'.
//
///Literal Representation
/// - - - - - - - - - - -
// Characters with decimal values in the range [33..126], with the exception of
// 61 ('='), *may* be represented literally as they appear before encoding.
// Hence, in addition to [0-9][a-z][A-Z], the following characters may
// propagate to the encoded stream unchanged.
//..
// [!"#$%&'()*+,-./:;<>?@[\]^_`{|}~]
//..
//
///Whitespace
/// - - - - -
// Space and tab *may* be represented literally, unless they appear at the end
// of an encoded line, in which case they must be followed by a '=' character
// serving as a soft line break (see rule #5), or they must be encoded
// according to rule #1.  It follows that any trailing whitespace encountered
// in a Quoted-Printable body must necessarily be added by intermediate
// transport agents and must be deleted during decoding.
//
///Line Breaks
///- - - - - -
// A line break must be represented in the Quoted-Printable encoding as in rule
// number 1, i.e., LF -> =0A; CR -> =0D.
//
///Soft Line Breaks
/// - - - - - - - -
// Encoded lines are required to be no longer than 76 characters in this
// encoding scheme.  Soft line breaks in the form of an '=' sign placed at the
// end of an encoded line are used to break up longer lines, either necessarily
// when the number of encoded characters, including any '=' characters but not
// counting the trailing CRLF, reaches the limit of 76, or at the user's
// discretion -- e.g., during manual encoding.  Soft line breaks are to be
// removed during decoding as they are not part of the original content.
//
// The Quoted-Printable encoding scheme allows one or two forms of encoding
// depending on the value of the character to be encoded as well as its
// location with respect to the end of line.  When both forms are permissible,
// the choice is discretionary.  For example, the word 'From' is often used as
// a message separator in the standard UNIX mail folder format.  To reduce the
// chance of a message getting broken, a sentence such as "From point A
// to ..." is often best encoded as "=46rom point A to ...", although "From
// point A to ..." is also a valid encoding.
//
// This implementation by default prefers literal encoding to Quoted-Printable
// encoding.  In the case of a space or tab character happening at the end of
// an encoded line, if there is more input to follow, a soft line break is
// inserted; otherwise, the last line of encoding should be terminated with the
// Quoted Printable encoding of space or tab (a line break is both redundant
// and contrived).
//
// In situations where it is desirable to specify certain characters to be
// encoded to their numeric form, the encoder in this implementation also
// offers a means to specify these characters through the first parameter to
// the following constructor
//..
//   bdlde::QuotedPrintableEncoder(
//      const char *extraCharsToEncode,
//      bdlde::QuotedPrintableEncoder::LineBreakMode lineBreakMode
//                        = bdlde::QuotedPrintableEncoder::BDEDE_CRLF_MODE,
//      int maxLineLength =
//                         bdlde::QuotedPrintableEncoder::DEFAULT_MAX_LINELEN);
//..
//
// The following examples demonstrate the above rules per the design choices
// made for this implementation.  Note that there is a hard line break at the
// 77th character position, immediately after "dozing".
//
///Example 1
///- - - - -
// Data:
//..
// From point A to point B, the distance is 1245.56 miles.  Driving at a dozing
// speed of 15mph, it will take 2 hours to complete the trip.
//..
//
// Encoding:
//..
// =46rom point A to point B, the distance is 1245.56 miles.  Driving at a doz=
// ing=0D=0A speed of 15mph, =
// it will take 2 hours=
// to complete the trip.
//..
//
///Example 2
///- - - - -
// Data:
//..
// Hello, world.
//..
// (The last line of input ends with a whitespace.)
//
// Encoding:
//..
// Hello, world.=20
//..
// (In this case, a Quoted Printable is preferred to soft line break as there
// should only be one encoded line.)
//
// The above encoding is acceptable, although it is by no means unique.
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
//:
//: 4 removing the soft line breaks including the '=' prefix (i.e.,
//:   concatenating broken sentences) (rule #5).
//
// The standard imposes a maximum of 76 characters exclusive of CRLF; however,
// the decoder implemented in this component will handle lines of arbitrary
// length.
//
// The decoder also provides support for two error-reporting modes,
// configurable at construction: the strict mode and the relaxed mode.  A
// strict-mode decoder stops decoding at the first offending character
// encountered, while a relaxed-mode decoder continues decoding to the end of
// the input, allowing straight pass-through of character sets that cannot be
// interpreted.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Encoding
///- - - - - - - - - -
// The following example shows how to use a 'bdlde::QuotedPrintableEncoder'
// object to implement a function, 'streamconverter', that reads text from a
// 'bsl::istream', encodes that text in Quoted-Printable representation, and
// write928s the encoded text to a 'bsl::ostream'.  'streamconverter' returns 0
// on success, and a negative value if the input data could not be successfully
// encoded or if there is an I/O error.
//..
// streamconverter.h                    -*-C++-*-
//
// int streamconverter(bsl::ostream& os, bsl::istream& is);
//     // Read the entire contents of the specified input stream 'is', convert
//     // the input plain text to quoted-printable encoding, and write the
//     // encoded text to the specified output stream 'os'.  Return 0 on
//     // success, and a negative value otherwise.
//..
// We will use fixed-sized input and output buffers in the implementation, but,
// because of the flexibility of 'bsl::istream' and the output-buffer
// monitoring functionality of 'QuotedPrintableEncoder', the fixed buffer sizes
// do *not* limit the quantity of data that can be read, encoded, or written to
// the output stream.  The implementation file is as follows.
//..
// streamconverter.cpp                  -*-C++-*-
//
// #include <streamconverter.h>
//
// #include <bdlde_quotedprintableencoder.h>
//
// namespace BloombergLP {
//
// int streamconverter(bsl::ostream& os, bsl::istream& is)
// {
//     enum {
//         SUCCESS      =  0,
//         ENCODE_ERROR = -1,
//         IO_ERROR     = -2
//     };
//..
// We declare a 'bdlde::QuotedPrintableEncoder' object 'converter', which will
// encode the input data.  Note that various internal buffers and cursors are
// used as needed without further comment.  We read as much data as is
// available from the user-supplied input stream 'is' *or* as much as will fit
// in 'inputBuffer' before beginning conversion.
//..
//     bdlde::QuotedPrintableEncoder converter;
//
//     const int INBUFFER_SIZE  = 1 << 10;
//     const int OUTBUFFER_SIZE = 1 << 10;
//
//     char inputBuffer[INBUFFER_SIZE];
//     char outputBuffer[OUTBUFFER_SIZE];
//
//     char *output    = outputBuffer;
//     char *outputEnd = outputBuffer + sizeof outputBuffer;
//
//     while (is.good()) {  // input stream not exhausted
//
//         is.read(inputBuffer, sizeof inputBuffer);
//..
// With 'inputBuffer' now populated, we'll use 'converter' in an inner 'while'
// loop to encode the input and write the encoded data to 'outputBuffer' (via
// the 'output' cursor').  Note that if the call to 'converter.convert' fails,
// our function terminates with a negative status.
//..
//         const char *input    = inputBuffer;
//         const char *inputEnd = input + is.gcount();
//
//         while (input < inputEnd) { // input encoding not complete
//
//             int numOut;
//             int numIn;
//
//             int status = converter.convert(output, &numOut, &numIn,
//                                            input,   inputEnd,
//                                            outputEnd - output);
//             if (status < 0) {
//                 return ENCODE_ERROR;                               // RETURN
//             }
//..
// If the call to 'converter.convert' returns successfully, we'll see if the
// output buffer is full, and if so, write its contents to the user-supplied
// output stream 'os'.  Note how we use the values of 'numOut' and 'numIn'
// generated by 'convert' to update the relevant cursors.
//..
//             output += numOut;
//             input  += numIn;
//
//             if (output == outputEnd) {  // output buffer full; write data
//                 os.write (outputBuffer, sizeof outputBuffer);
//                 if (os.fail()) {
//                     return IO_ERROR;                               // RETURN
//                 }
//                 output = outputBuffer;
//             }
//         }
//     }
//..
// We have now exited both the input and the "encode" loops.  'converter' may
// still hold encoded output characters, and so we call 'converter.endConvert'
// to emit any retained output.  To guarantee correct behavior, we call this
// method in an infinite loop, because it is possible that the retained output
// can fill the output buffer.  In that case, we solve the problem by writing
// the contents of the output buffer to 'os' within the loop.  The most likely
// case, however, is that 'endConvert' will return 0, in which case we exit the
// loop and write any data remaining in 'outputBuffer' to 'os'.  As above, if
// 'endConvert' fails, we exit the function with a negative return status.
//..
//     for (;;) {
//
//         int more =
//                   converter.endConvert(output, &numOut, outputEnd - output);
//         if (more < 0) {
//             return ENCODE_ERROR;                                   // RETURN
//         }
//
//         output += numOut;
//
//         if (!more) { // no more output
//             break;
//         }
//
//         assert (output == outputEnd);  // output buffer is full
//
//         os.write (outputBuffer, sizeof outputBuffer);  // write buffer
//         if (os.fail()) {
//             return IO_ERROR;                                       // RETURN
//         }
//         output = outputBuffer;
//     }
//
//     if (output > outputBuffer) { // still data in output buffer; write it
//                                  // all
//         os.write(outputBuffer, output - outputBuffer);
//     }
//
//     return is.eof() && os.good() ? SUCCESS : IO_ERROR;
// }
//
// } // Close namespace BloombergLP
//..
// For ease of reading, we repeat the full content of the 'streamconverter.cpp'
// file without interruption.
//..
// streamconverter.cpp                  -*-C++-*-
//
// #include <streamconverter.h>
//
// #include <bdlde_quotedprintableencoder.h>
//
// namespace BloombergLP {
//
// int streamconverter(bsl::ostream& os, bsl::istream& is)
// {
//     enum {
//         SUCCESS      =  0,
//         ENCODE_ERROR = -1,
//         IO_ERROR     = -2
//     };
//
//     bdlde::QuotedPrintableEncoder converter;
//
//     const int INBUFFER_SIZE  = 1 << 10;
//     const int OUTBUFFER_SIZE = 1 << 10;
//
//     char inputBuffer[INBUFFER_SIZE];
//     char outputBuffer[OUTBUFFER_SIZE];
//
//     char *output    = outputBuffer;
//     char *outputEnd = outputBuffer + sizeof outputBuffer;
//
//     while (is.good()) {  // input stream not exhausted
//
//         is.read(inputBuffer, sizeof inputBuffer);
//
//         const char *input    = inputBuffer;
//         const char *inputEnd = input + is.gcount();
//
//         while (input < inputEnd) { // input encoding not complete
//
//             int numOut;
//             int numIn;
//
//             int status = converter.convert(output, &numOut, &numIn,
//                                            input,   inputEnd,
//                                            outputEnd - output);
//             if (status < 0) {
//                 return ENCODE_ERROR;                               // RETURN
//             }
//
//             output += numOut;
//             input  += numIn;
//
//             if (output == outputEnd) {  // output buffer full; write data
//                 os.write(outputBuffer, sizeof outputBuffer);
//                 if (os.fail()) {
//                     return IO_ERROR;                               // RETURN
//                 }
//                 output = outputBuffer;
//             }
//         }
//     }
//
//     for (;;) {
//
//         int more =
//                   converter.endConvert(output, &numOut, outputEnd - output);
//         if (more < 0) {
//             return ENCODE_ERROR;                                   // RETURN
//         }
//
//         output += numOut;
//
//         if (!more) { // no more output
//             break;
//         }
//
//         assert (output == outputEnd);  // output buffer is full
//
//         os.write (outputBuffer, sizeof outputBuffer);  // write buffer
//         if (os.fail()) {
//             return IO_ERROR;                                       // RETURN
//         }
//         output = outputBuffer;
//     }
//
//     if (output > outputBuffer) {
//         os.write (outputBuffer, output - outputBuffer);
//     }
//
//     return is.eof() && os.good() ? SUCCESS : IO_ERROR;
// }
//
// } // Close namespace BloombergLP
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_QUEUE
#include <bsl_queue.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>  // INT_MAX
#endif

namespace BloombergLP {
namespace bdlde {

                        // ============================
                        // class QuotedPrintableEncoder
                        // ============================

class QuotedPrintableEncoder {
    // This class implements a mechanism capable of converting data of
    // arbitrary length to its corresponding Quoted-Printable representation.

    // PRIVATE TYPES
    enum States {
        // symbolic state values for the encoder

        e_ERROR_STATE   = -1,  // input is irreparably invalid
        e_INITIAL_STATE = 0,   // require no more input
        e_INPUT_STATE   = 1,   // general input state
        e_DONE_STATE    = 2,   // accepting; any additional input is error
        e_SAW_CR_STATE  = 3    // TBD doc
    };

    enum {
        e_DEFAULT_MAX_LINELEN = -1   // a flag to indicate that the default
                                   // s_defaultMaxLineLength is to be used;
                                   // this device prevents recompilation of the
                                   // client's code even in the unlikely event
                                   // that the s_defaultMaxLineLength should
                                   // change
    };

  public:
    // PUBLIC TYPES
    enum EquivalenceClass {
        // The input equivalence classes

        e_PC = 0,  // printable character - copy straight to output
        e_CR,      // carriage return     - wait for more input
        e_LF,      // line feed           - complete linebreak
        e_WS,      // whitespace          - buffer; wait for more input
        e_CC       // control character   - encode to Quoted Printable
    };

    enum LineBreakMode {
        // Configuration governing how various forms of line breaks are to be
        // interpreted

        e_CRLF_MODE = 0,  // only allow "\r\n" linebreaks

        e_LF_MODE,        // only allow '\n' linebreaks (without the '\r'
                              // prefix)

        e_MIXED_MODE      // allow both "\r\n" and '\n'
    };

  private:
    // CLASS DATA
    static const char  *s_defaultEquivClass_p;      // default map of 'unsigned
                                                    // char' to equivalent
                                                    // class
    static const int    s_defaultMaxLineLength;     // default max line length
    static const char  *s_lineBreakModeName[];      // names of line break mode

    // INSTANCE DATA
    LineBreakMode
            d_lineBreakMode;    // linebreak mode

    int     d_maxLineLength;    // maximum length of output line
    int     d_outputLength;     // total number of output characters
    int     d_lineLength;       // number of characters on the current line

    char   *d_equivClass_p;     // map of 'unsigned char' to input equivalence
                                // class; dynamically allocated if the default
                                // map is to be modified; otherwise it is
                                // assigned; compare with static address to
                                // know whether to delete

    char    d_lastInputChar;    // stores an input space or tab if it happens
                                // at the end of input

    bsl::queue<char> d_outBuf;  // buffer holding output exceeding the capacity
                                // of the output buffer from the previous
                                // operation

    int  d_state;               // TBD doc
    char d_buffer[3];           // TBD doc
    int  d_bufferLength;        // TBD doc
    int  d_lineStart;           // TBD doc

  private:
    // NOT IMPLEMENTED
    QuotedPrintableEncoder(const QuotedPrintableEncoder&);
    QuotedPrintableEncoder& operator=(const QuotedPrintableEncoder&);

  public:
    // CLASS METHODS
    static const char* lineBreakModeToAscii(
                             QuotedPrintableEncoder::LineBreakMode mode);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumerator 'mode'.

    // CREATORS
    explicit
    QuotedPrintableEncoder(LineBreakMode lineBreakMode = e_CRLF_MODE,
                           int           maxLineLength =
                                                        e_DEFAULT_MAX_LINELEN);
        // Create a Quoted-Printable encoder in the initial state, configured
        // to accept hard line breaks based on the specified 'lineBreakMode',
        // and to insert soft line breaks when the line length exceeds the
        // specified 'maxLineLength' (default is the recommended standard 76; 0
        // means single-line output).  The behavior is undefined unless either
        // 0 == 'maxLineLength' or 4 <= 'maxLineLength'.  Note that
        // BDEDE_CRLF_MODE passes "\r\n" straight to output and converts '\n';
        // BDEDE_LF_MODE passes '\n' and converts '\r'; BDEDE_MIXED_MODE passes
        // both "\r\n" and '\n'.

    explicit
    QuotedPrintableEncoder(
                         const char    *extraCharsToEncode,
                         LineBreakMode  lineBreakMode = e_CRLF_MODE,
                         int            maxLineLength = e_DEFAULT_MAX_LINELEN);
        // Create a Quoted-Printable encoder in the initial state, configured
        // to convert to the form "=XX" any input character matching those in
        // the specified 'extraCharsToEncode' array (as opposed to the default
        // setting of passing the input character straight to output), to
        // accept hard linebreaks based on the specified 'lineBreakMode', and
        // to insert soft linebreaks when the line length exceeds the specified
        // 'maxLineLength' (default is the recommended standard 76; 0 means
        // single-line output).  The behavior is undefined unless either
        // 0 == 'maxLineLength' or 4 <= 'maxLineLength'.  Note that
        // BDEDE_CRLF_MODE passes "\r\n" straight to output and converts '\n';
        // BDEDE_LF_MODE passes '\n' and converts '\r'; BDEDE_MIXED_MODE passes
        // both "\r\n" and '\n'.

    ~QuotedPrintableEncoder();
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
        // consume and encode a sequence of input characters starting at the
        // specified 'begin' position, up to but not including the specified
        // 'end' position, writing any resulting output in the specified
        // 'output' buffer up to the (cumulative) 'maxNumOut' limit.  If
        // 'maxNumOut' limit is reached, no further input will be consumed.
        // Load into the specified 'numOut' and 'numIn' the number of output
        // bytes produced and input bytes consumed, respectively.  Return a
        // non-negative value on success and a negative value otherwise.  A
        // successful return status indicates the number of characters that
        // would be output if 'endConvert' were called subsequently with no
        // output limit.  These bytes are also available for output if this
        // method is called with a sufficiently large 'maxNumOut'.  Note that
        // calling this method after 'endConvert' has been invoked without an
        // intervening 'reset' call will place this instance in an error state,
        // and return an error status.  Note also that it is recommended that
        // after all calls to 'convert' are finished, the 'endConvert' method
        // be called to complete the encoding of any unprocessed input
        // characters (e.g., whitespace).

    int endConvert(char *out, int *numOut, int maxNumOut = -1);
        // Terminate encoding for this encoder; write any retained output
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
        // zero status, a subsequent call will place this encoder in the error
        // state, and return an error status.

    void reset();
        // Reset this encoder to its initial state (i.e., as if no input had
        // been consumed).

    // ACCESSORS
    bool isAccepting() const;
        // Return 'true' if the input read so far by this encoder is considered
        // syntactically complete, all resulting output has been emitted, and
        // there is no internally buffered unprocessed input; return 'false'
        // otherwise.

    bool isDone() const;
        // Return 'true' if this encoder is in the done state (i.e.,
        // 'endConvert' has been called and any additional input will result in
        // an error), and if there is no pending output; return 'false'
        // otherwise.

    bool isError() const;
        // Return 'true' if this encoder is in an error state, and false
        // otherwise.  Note that for an encoder, no input can cause an error;
        // the possible possible errors result either from a call to the
        // 'convert' method after the 'endConvert' method is called the first
        // time, or from a call to either the 'convert' or the 'endConvert'
        // methods after the 'endConvert' method has returned successfully.

    bool isInitialState() const;
        // Return 'true' if this encoder is in the initial state (i.e., as if
        // no input had been consumed), and ' false' otherwise.

    LineBreakMode lineBreakMode() const;
        // Return the line break mode configured at the construction of this
        // encoder.

    int maxLineLength() const;
        // Return the value for the maximum line length configured at the
        // construction of this encoder.

    int numOutputPending() const;
        // Return the number of output bytes retained by this encoder and not
        // emitted because 'maxNumOut' has been reached.

    int outputLength() const;
        // Return the total length of the output emitted by this encoder
        // (possibly after one or more calls to the 'convert' or the 'input'
        // methods) since its initial construction or the latest 'reset'.  Note
        // that soft line breaks are included in the counts if added.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CLASS METHODS
inline
const char* QuotedPrintableEncoder::lineBreakModeToAscii(LineBreakMode mode)
{
    return s_lineBreakModeName[mode];
}

// MANIPULATORS
inline
void QuotedPrintableEncoder::reset()
{
    d_state = e_INITIAL_STATE;
    d_outputLength = 0;
    d_lineLength = 0;
    d_bufferLength = 0;
    while (!d_outBuf.empty()) {
        d_outBuf.pop();
    }
}

// ACCESSORS
inline
bool QuotedPrintableEncoder::isAccepting() const
{
    return e_INITIAL_STATE == d_state || e_DONE_STATE == d_state;
}

inline
bool QuotedPrintableEncoder::isDone() const
{
    return e_DONE_STATE == d_state && 0 == d_outBuf.size();
}

inline
bool QuotedPrintableEncoder::isError() const
{
    return e_ERROR_STATE == d_state;
}

inline
bool QuotedPrintableEncoder::isInitialState() const
{
    return e_INITIAL_STATE == d_state && 0 == d_outputLength;
}

inline
QuotedPrintableEncoder::LineBreakMode
QuotedPrintableEncoder::lineBreakMode() const
{
    return d_lineBreakMode;
}

inline
int QuotedPrintableEncoder::maxLineLength() const
{
    return d_maxLineLength;
}

inline
int QuotedPrintableEncoder::numOutputPending() const
{
    return static_cast<int>(d_outBuf.size());
}

inline
int QuotedPrintableEncoder::outputLength() const
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
