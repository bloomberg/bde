// bdlde_base64encoder.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLDE_BASE64ENCODER
#define INCLUDED_BDLDE_BASE64ENCODER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide automata for converting to and from Base64 encodings.
//
//@CLASSES:
//   bdlde::Base64Encoder: automata performing Base64 encoding operations
//
//@SEE_ALSO: bdlde_base64decoder
//
//@DESCRIPTION: This component provides a 'class', 'bdlde::Base64Encoder',
// which provides a pair of template functions (each parameterized separately
// on both input and output iterators) that can be used respectively to encode
// and to decode byte sequences of arbitrary length into and from the printable
// Base64 representation described in Section 6.8 "Base64 Content Transfer
// Encoding" of RFC 2045, "Multipurpose Internet Mail Extensions (MIME) Part
// One: Format of Internet Message Bodies."
//
// Each instance of either the encoder or decoder retains the state of the
// conversion from one supplied input to the next, enabling the processing of
// segmented input -- i.e., processing resumes where it left off with the next
// invocation on new input.  Instance methods are provided for both the encoder
// and decoder to (1) assert the end of input, (2) determine whether the input
// so far is currently acceptable, and (3) indicate whether a non-recoverable
// error has occurred.
//
///Base 64 Encoding
///----------------
// The data stream is processed three bytes at a time from left to right (a
// final quantum consisting of one or two bytes, as discussed below, is handled
// specially).  Each sequence of three 8-bit quantities
//..
//      7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
//     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//     |               |               |               |
//     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      `------v------' `------v------' `------v------'
//           Byte2           Byte1           Byte0
//..
// is segmented into four intermediate 6-bit quantities.
//..
//      5 4 3 2 1 0 5 4 3 2 1 0 5 4 3 2 1 0 5 4 3 2 1 0
//     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//     |           |           |           |           |
//     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      `----v----' `----v----' `----v----' `----v----'
//         char3       char2       char1        char0
//..
// Each 6-bit quantity is in turn used as an index into the following character
// table to generate an 8-bit character.  The four resulting characters hence
// form the encoding for the original 3-byte sequence.
//..
//     ======================================================================
//     *              Table of Numeric BASE-64 Encoding Characters          *
//     ----------------------------------------------------------------------
//     Val Enc  Val Enc  Val Enc  Val Enc  Val Enc  Val Enc  Val Enc  Val Enc
//     --- ---  --- ---  --- ---  --- ---  --- ---  --- ---  --- ---  --- ---
//       0 'A'    8 'I'   16 'Q'   24 'Y'   32 'g'   40 'o'   48 'w'   56 '4'
//       1 'B'    9 'J'   17 'R'   25 'Z'   33 'h'   41 'p'   49 'x'   57 '5'
//       2 'C'   10 'K'   18 'S'   26 'a'   34 'i'   42 'q'   50 'y'   58 '6'
//       3 'D'   11 'L'   19 'T'   27 'b'   35 'j'   43 'r'   51 'z'   59 '7'
//       4 'E'   12 'M'   20 'U'   28 'c'   36 'k'   44 's'   52 '0'   60 '8'
//       5 'F'   13 'N'   21 'V'   29 'd'   37 'l'   45 't'   53 '1'   61 '9'
//       6 'G'   14 'O'   21 'W'   30 'e'   38 'm'   46 'u'   54 '2'   62 '+'
//       7 'H'   15 'P'   22 'X'   31 'f'   39 'n'   47 'v'   55 '3'   63 '/'
//     ======================================================================
//..
// The 3-byte grouping of the input is only a design of convenience and not a
// requirement.  When the number of bytes in the input stream is not divisible
// by 3, sufficient 0 bits are padded on the right to achieve an integral
// number of 6-bit character indices.  Then one of two special cases will apply
// for the final processing step:
//
// I) There is a single byte of data, in which case there will be two Base64
// encoding characters (the second of which will be one of [AQgw]) followed by
// two equal ('=') signs.
//
// II) There are exactly two bytes of data, in which case there will be three
// Base64 encoding characters (the third of which will be one of
// [AEIMQUYcgkosw048] followed by a single equal ('=') sign.
//
// The MIME standard requires that the maximum line length of emitted text not
// exceed 76 characters exclusive of CRLF.  The caller may override this
// default if desired.
//
// Input values of increasing length along with their corresponding Base64
// encodings are illustrated below:
//..
//        Data: /* nothing */
//    Encoding: /* nothing */
//
//        Data: 0x01
//    Encoding: AQ==
//
//        Data: 0x01 0x02
//    Encoding: AQI=
//
//        Data: 0x01 0x02 0x03
//    Encoding: AQID
//
//        Data: 0x01 0x02 0x03 0x04
//    Encoding: AQIDBA==
//..
// In order for a Base64 encoding to be valid, the input data must be either of
// length a multiple of three (constituting maximal input), or have been
// terminated explicitly by the 'endConvert' method (initiating bit padding
// when necessary).
//
///Base 64 Decoding
///----------------
// The degree to which decoding detects errors can significantly affect
// performance.  The standard permits all non-Base64 characters to be treated
// as whitespace.  One variant mode of this decoder does just that; the other
// reports an error if a bad (i.e., non-whitespace) character is detected.  The
// mode of the instance is configurable.  The standard imposes a maximum of 76
// characters exclusive of CRLF; however, the decoder implemented in this
// component will handle lines of arbitrary length.
//
// The following kinds of errors can occur during decoding and are reported
// with the following priority:
//..
// BAD DATA: A character (other than whitespace) that is not a member of the
//           Base64 character set (including '=').  Note that this error
//           is detected only if the 'decoder' is explicitly configured (at
//           construction) to do so.
//
// BAD FORMAT: An '=' character precedes a valid numeric Base64 character,
//             more than two '=' characters appear (possibly separated by
//             non-Base64 characters), a numeric Base64 character other than
//             [AEIMQUYcgkosw048] precedes a single terminal '=' character,
//             or a character other than [AQgw] precedes a terminal pair of
//             consecutive '=' characters.
//..
// The 'isError' method is used to detect such anomalies, and the 'numIn'
// output parameter (indicating the number of input characters consumed) or
// possibly the iterator itself (for iterators with reference-semantics)
// identifies the offending character.
//
// Note that the existence of an '=' can be used to reliably indicate the end
// of the valid data, but no such assurance is possible when the length (in
// bytes) of the initial input data sequence before encoding was evenly
// divisible by 3.
//
///Usage
///-----
// The following example shows how to use a 'bdlde::Base64Encoder' object to
// implement a function, 'streamEncoder', that reads text from a
// 'bsl::istream', encodes that text in base 64 representation, and writes the
// encoded text to a 'bsl::ostream'.  'streamEncoder' returns 0 on success
// and a negative value if the input data could not be successfully encoded or
// if there is an I/O error.
//..
// streamencoder.h                    -*-C++-*-
//
// int streamEncoder(bsl::ostream& os, bsl::istream& is);
//     // Read the entire contents of the specified input stream 'is', convert
//     // the input plain text to base 64 encoding, and write the encoded text
//     // to the specified output stream 'os'.  Return 0 on success, and a
//     // negative value otherwise.
//..
// We will use fixed-sized input and output buffers in the implementation, but,
// because of the flexibility of 'bsl::istream' and the output-buffer
// monitoring functionality of 'bdlde::Base64Encoder', the fixed buffer sizes
// do *not* limit the quantity of data that can be read, encoded, or written to
// the output stream.  The implementation file is as follows.
//..
// streamencoder.cpp                  -*-C++-*-
//
// #include <streamencoder.h>
//
// #include <bdlde_base64encoder.h>
//
// namespace BloombergLP {
//
// int streamEncoder(bsl::ostream& os, bsl::istream& is)
// {
//     enum {
//         SUCCESS      =  0,
//         ENCODE_ERROR = -1,
//         IO_ERROR     = -2
//     };
//..
// We declare a 'bdlde::Base64Encoder' object 'converter', which will encode
// the input data.  Note that various internal buffers and cursors are used as
// needed without further comment.  We read as much data as is available from
// the user-supplied input stream 'is' *or* as much as will fit in
// 'inputBuffer' before beginning conversion.
//..
//     bdlde::Base64Encoder converter;
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
//     while (1) {
//
//         int numOut;
//
//         int more = converter.endConvert(output, &numOut, outputEnd-output);
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
//     return (is.eof() && os.good()) ? SUCCESS : IO_ERROR;
// }
//
// } // Close namespace BloombergLP
//..
// For ease of reading, we repeat the full content of the 'streamencoder.cpp'
// file without interruption.
//..
// streamencoder.cpp                  -*-C++-*-
//
// #include <streamencoder.h>
//
// #include <bdlde_base64encoder.h>
//
// namespace BloombergLP {
//
// int streamEncoder(bsl::ostream& os, bsl::istream& is)
// {
//     enum {
//         SUCCESS      =  0,
//         ENCODE_ERROR = -1,
//         IO_ERROR     = -2
//     };
//
//     bdlde::Base64Encoder converter;
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
//     while (1) {
//
//         int numOut;
//
//         int more = converter.endConvert(output, &numOut, outputEnd-output);
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
//     return (is.eof() && os.good()) ? SUCCESS : IO_ERROR;
// }
//
// }  // close namespace BloombergLP
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

namespace bdlde {
                            // ===================
                            // class Base64Encoder
                            // ===================

class Base64Encoder {
    // This class implements a mechanism capable of converting data of
    // arbitrary length to its corresponding Base64 representation.

    // PRIVATE TYPES
    enum {
        // Symbolic state values.

        e_ERROR_STATE     = -1, // Input is irreparably invalid.
        e_INITIAL_STATE   =  0, // Ready to accept input.
        e_DONE_STATE      =  1  // Any additional input is an error.
    };

    // CLASS DATA
    static const char *const s_encodedChars_p;        // 6-bit map of Base64
                                                      // encodings
    static const int         s_defaultMaxLineLength;  // Default maximum line
                                                      // length

    // INSTANCE DATA
    signed char d_state;  // state as per above enum

    int d_maxLineLength;  // maximum length of output line
    int d_lineLength;     // current length of output line
    int d_outputLength;   // total number of output characters
    int d_stack;          // storage of non-emitted input
    int d_bitsInStack;    // number of bits in 'd_stack'

  private:
    // NOT IMPLEMENTED
    Base64Encoder(const Base64Encoder&);
    Base64Encoder& operator=(const Base64Encoder&);

    // PRIVATE MANIPULATORS
    template <class OUTPUT_ITERATOR>
    void append(OUTPUT_ITERATOR *out, char character, int maxLength);
        // Append a soft new line to the specified 'out' if necessary and then
        // append the specified 'character' without the total number of emitted
        // characters equaling the specified 'maxLength'.  The behavior is
        // undefined unless the total number of emitted characters does not
        // equal 'maxLength' at entry to this method.

    template <class OUTPUT_ITERATOR>
    void encode(OUTPUT_ITERATOR *out, int maxLength);
        // Append a soft new line to the specified 'out' if necessary and then
        // emit an internally buffered character without the total number of
        // emitted characters equaling the specified 'maxLength'.  The
        // behavior is undefined unless the total number of emitted characters
        // does not equal 'maxLength' at entry to this method and the internal
        // buffer contains at least one character of output.

  public:
    // CLASS METHODS
    static int encodedLength(int inputLength);
        // Return the exact number of encoded bytes that would result from an
        // input byte sequence of the specified 'inputLength' provided to the
        // 'convert' method of an encoder with the maximum allowable
        // line-length of the output being 76 characters (as recommended by the
        // MIME standard).  The behavior is undefined unless
        // '0 <= inputLength'.

    static int encodedLength(int inputLength, int maxLineLength);
        // Return the exact number of encoded bytes that would result from an
        // input byte sequence of the specified 'inputLength' provided to the
        // 'convert' method of an encoder configured with the specified
        // 'maxLineLength'.  The behavior is undefined unless
        // '0 <= inputLength' and '0 <= maxLineLength'.  Note that if
        // 'maxLineLength' is 0, no CRLF characters will appear in the output.
        // Note also that the number of encoded bytes need not be the number of
        // *output* bytes.

    static int encodedLines(int inputLength);
        // Return the exact number of encoded lines that would result from an
        // input byte sequence of the specified 'inputLength' provided to the
        // 'convert' method of an encoder with the maximum allowable
        // line-length of the output being 76 characters (as recommended by the
        // MIME standard).  The behavior is undefined unless
        // '0 <= inputLength'.  Note also that the number of encoded bytes need
        // not be the number of *output* bytes.

    static int encodedLines(int inputLength, int maxLineLength);
        // Return the exact number of encoded lines that would result from an
        // input byte sequence of the specified 'inputLength' provided to the
        // 'convert' method of an encoder configured with the specified
        // 'maxLineLength'.  The behavior is undefined unless
        // '0 <= inputLength' and '0 <= maxLineLength'.  Note that if
        // 'maxLineLength' is 0, no CRLF characters will appear in the output.
        // Note also that the number of encoded bytes need not be the number of
        // *output* bytes.

    static bool isResidualOutput(int numBytes, int maxLineLength);
        // Return 'true' if an output sequence of the specified 'numBytes'
        // from an encoder having the specified 'maxLineLength' would be an
        // acceptable input to a 'Base64Decoder', and 'false' otherwise.

    // CREATORS
    Base64Encoder();
        // Create a Base64 encoder in the initial state, defaulting the maximum
        // allowable line-length of the output to 76 (as recommended by the
        // MIME standard).  Note that the 'convert' and 'endConvert' methods of
        // this encoder will insert a CRLF to prevent each line of the output
        // from exceeding 76 characters.

    explicit
    Base64Encoder(int maxLineLength);
        // Create a Base64 encoder in the initial state, setting the maximum
        // allowable line-length of the output to the specified
        // 'maxLineLength'.  Specifying 0 for 'maxLineLength' will result in a
        // single output line (i.e., one with no CRLF in it).  The behavior is
        // undefined unless '0 <= maxLineLength'.  Note that when
        // 'maxLineLength' is positive, the 'convert' and 'endConvert' methods
        // of this encoder will insert a CRLF to prevent each line of the
        // output from exceeding 'maxLineLength'.

    ~Base64Encoder();
        // Destroy this object.

    // MANIPULATORS
    template <class OUTPUT_ITERATOR, class INPUT_ITERATOR>
    int convert(OUTPUT_ITERATOR out,
                INPUT_ITERATOR  begin,
                INPUT_ITERATOR  end);
    template <class OUTPUT_ITERATOR, class INPUT_ITERATOR>
    int convert(OUTPUT_ITERATOR  out,
                int             *numOut,
                int             *numIn,
                INPUT_ITERATOR   begin,
                INPUT_ITERATOR   end,
                int              maxNumOut = -1);
        // Encode the sequence of input characters starting at the specified
        // 'begin' position up to, but not including, the specified 'end'
        // position, writing any resulting output characters to the specified
        // 'out' buffer.  Optionally specify the 'maxNumOut' limit on the
        // number of bytes to output; if 'maxNumOut' is negative, no limit is
        // imposed.  If the 'maxNumOut' limit is reached, no further input will
        // be consumed.  Load into the (optionally) specified 'numOut' and
        // 'numIn' the number of output bytes produced and input bytes
        // consumed, respectively.  Return a non-negative value on success and
        // a negative value otherwise.  A positive return status indicates the
        // number of valid processed output bytes retained by this encoder and
        // not written to 'out' because 'maxNumOut' has been reached; these
        // bytes are available for output if this method is called with
        // appropriate input.  Note that calling this method after 'endConvert'
        // has been invoked without an intervening 'resetState' call will place
        // this instance in an error state, and return an error status.  Note
        // also that it is recommended that after all calls to 'convert' are
        // finished, the 'endConvert' method be called to complete the encoding
        // of any unprocessed input characters that do not complete a 3-byte
        // sequence.

    template <class OUTPUT_ITERATOR>
    int endConvert(OUTPUT_ITERATOR out);
    template <class OUTPUT_ITERATOR>
    int endConvert(OUTPUT_ITERATOR out, int *numOut, int maxNumOut = -1);
        // Terminate encoding for this encoder; write any retained output
        // (e.g., from a previous call to 'convert') to the specified 'out'
        // buffer.  Optionally specify the 'maxNumOut' limit on the number of
        // bytes to output; if 'maxNumOut' is negative, no limit is imposed.
        // Load into the (optionally) specified 'numOut' the number of output
        // bytes produced.  Return 0 on success, the positive number of bytes
        // *still* retained by this encoder if the 'maxNumOut' limit was
        // reached, and a negative value otherwise.  Any retained bytes are
        // available on a subsequent call to 'endConvert'.  Once this method is
        // called, no additional input may be supplied without an intervening
        // call to 'resetState'; once this method returns a zero status, a
        // subsequent call will place this encoder in the error state, and
        // return an error status.

    void resetState();
        // Reset this instance to its initial state (i.e., as if no input had
        // been consumed).

    // ACCESSORS
    bool isAcceptable() const;
        // Return 'true' if the input read so far is considered syntactically
        // complete and all resulting output has been emitted to 'out', and
        // 'false' otherwise.  Note that 'endConvert' must be called if the
        // total length of all data processed is not divisible by 3.

    bool isDone() const;
        // Return 'true' if the current input is acceptable and any additional
        // input (including 'endConvert') would be an error, and 'false'
        // otherwise.  Note that if this decoder 'isDone' then all resulting
        // output has been emitted to 'out'.

    bool isError() const;
        // Return 'true' if the there is no possibility of achieving an
        // "acceptable" result, and 'false' otherwise.

    bool isInitialState() const;
        // Return 'true' if this instance is in the initial state (i.e., as if
        // no input had been consumed), and 'false' otherwise.

    bool isMaximal() const;
        // Return 'true' if the current input is acceptable and any additional
        // input (other than 'endConvert') would be an error, and 'false'
        // otherwise.

    int maxLineLength() const;
        // Return the currently installed value for the maximum line length.

    int outputLength() const;
        // Return the total length of the output emitted thus far (including
        // soft line breaks where appropriate).
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // -------------------
                            // class Base64Encoder
                            // -------------------

// PRIVATE MANIPULATORS
template <class OUTPUT_ITERATOR>
void Base64Encoder::append(OUTPUT_ITERATOR *out,
                           char             value,
                           int              maxLength)
{
    BSLS_ASSERT(out);

    if (d_maxLineLength && d_lineLength >= d_maxLineLength) {
        if (d_lineLength == d_maxLineLength) {
            **out = '\r';
            ++*out;
            ++d_outputLength;
            ++d_lineLength;
            if (d_outputLength == maxLength) {
                return;                                               // RETURN
            }
        }
        **out = '\n';
        ++*out;
        ++d_outputLength;
        d_lineLength = 0;
        if (d_outputLength == maxLength) {
            return;                                                   // RETURN
        }
    }
    **out = value;
    ++*out;
    ++d_outputLength;
    ++d_lineLength;
}

template <class OUTPUT_ITERATOR>
void Base64Encoder::encode(OUTPUT_ITERATOR *out, int maxLength)
{
    BSLS_ASSERT(out);

    if (d_maxLineLength && d_lineLength >= d_maxLineLength) {
        if (d_lineLength == d_maxLineLength) {
            **out = '\r';
            ++*out;
            ++d_outputLength;
            ++d_lineLength;
            if (d_outputLength == maxLength) {
                return;                                               // RETURN
            }
        }
        **out = '\n';
        ++*out;
        ++d_outputLength;
        d_lineLength = 0;
        if (d_outputLength == maxLength) {
            return;                                                   // RETURN
        }
    }
    d_bitsInStack -= 6;
    **out = s_encodedChars_p[(d_stack >> d_bitsInStack) & 0x3f];
    ++*out;
    ++d_outputLength;
    ++d_lineLength;
}

// CLASS METHODS
inline
int Base64Encoder::encodedLength(int inputLength, int maxLineLength)
{
    BSLS_ASSERT_SAFE(0 <= inputLength);
    BSLS_ASSERT_SAFE(0 <= maxLineLength);

    int lineLength = (inputLength + 2) / 3 * 4;

    // Note that when lineLength > maxLineLength, the added cost is given by
    // n = (lineLength - maxLineLength)
    //..
    // addedCost = 2 * ((n + maxLineLength - 1) / maxLineLength)
    //           = 2 * (    lineLength - 1    ) / maxLineLength)
    //..

    return 0 == maxLineLength || lineLength <= maxLineLength
           ? lineLength
           : lineLength + 2 * ((lineLength - 1) / maxLineLength);
}

inline
int Base64Encoder::encodedLength(int inputLength)
{
    BSLS_ASSERT_SAFE(0 <= inputLength);

    return encodedLength(inputLength, 76);
}

inline
int Base64Encoder::encodedLines(int inputLength, int maxLineLength)
{
    BSLS_ASSERT_SAFE(0 <= inputLength);
    BSLS_ASSERT_SAFE(0 <= maxLineLength);

    const int length = (inputLength + 2) / 3 * 4;

    return 0 == maxLineLength ? 1 : 1 + length / maxLineLength;
}

inline
int Base64Encoder::encodedLines(int inputLength)
{
    BSLS_ASSERT_SAFE(0 <= inputLength);

    return encodedLines(inputLength, 76);
}

inline
bool Base64Encoder::isResidualOutput(int numBytes, int maxLineLength)
{
    BSLS_ASSERT_SAFE(0 <= numBytes);
    BSLS_ASSERT_SAFE(0 <= maxLineLength);

    const int adj = maxLineLength ? 2 * (numBytes / (maxLineLength + 2)) : 0;
    return 0 != (numBytes - adj) % 4;
}

// CREATORS
inline
Base64Encoder::Base64Encoder()
: d_state(e_INITIAL_STATE)
, d_maxLineLength(s_defaultMaxLineLength)
, d_lineLength(0)
, d_outputLength(0)
, d_stack(0)
, d_bitsInStack(0)
{
}

inline
Base64Encoder::Base64Encoder(int maxLineLength)
: d_state(e_INITIAL_STATE)
, d_maxLineLength(maxLineLength)
, d_lineLength(0)
, d_outputLength(0)
, d_stack(0)
, d_bitsInStack(0)
{
    BSLS_ASSERT_SAFE(0 <= maxLineLength);
}

// MANIPULATORS
template <class OUTPUT_ITERATOR, class INPUT_ITERATOR>
int Base64Encoder::convert(OUTPUT_ITERATOR out,
                           INPUT_ITERATOR  begin,
                           INPUT_ITERATOR  end)
{
    int dummyNumOut;
    int dummyNumIn;

    return convert(out, &dummyNumOut, &dummyNumIn, begin, end, -1);
}

template <class OUTPUT_ITERATOR, class INPUT_ITERATOR>
int Base64Encoder::convert(OUTPUT_ITERATOR  out,
                           int             *numOut,
                           int             *numIn,
                           INPUT_ITERATOR   begin,
                           INPUT_ITERATOR   end,
                           int              maxNumOut)
{
    int dummyNumOut;
    if (!numOut) {
        numOut = &dummyNumOut;
    }
    int dummyNumIn;
    if (!numIn) {
        numIn  = &dummyNumIn;
    }

    if (e_ERROR_STATE == d_state || e_DONE_STATE == d_state) {
        d_state = e_ERROR_STATE;
        *numOut = 0;
        *numIn  = 0;
        return -1;                                                    // RETURN
    }

    const int initialLength = d_outputLength;
    const int maxLength     = d_outputLength + maxNumOut;

    // Emit as many output bytes as possible.

    while (6 <= d_bitsInStack && d_outputLength != maxLength) {
        encode(&out, maxLength);
    }

    // Consume as many input bytes as possible.

    int tmpNumIn = 0;

    while (4 >= d_bitsInStack && begin != end) {
        const unsigned char byte = static_cast<unsigned char>(*begin);

        ++begin;
        ++tmpNumIn;

        d_stack        = (d_stack << 8) | byte;
        d_bitsInStack += 8;

        if (d_outputLength != maxLength) {
            encode(&out, maxLength);
            if (6 <= d_bitsInStack && d_outputLength != maxLength) {
                encode(&out, maxLength);
            }
        }
    }

    *numIn  = tmpNumIn;
    *numOut = d_outputLength - initialLength;

    return 0;
}

template <class OUTPUT_ITERATOR>
int Base64Encoder::endConvert(OUTPUT_ITERATOR out)
{
    int dummyNumOut;

    return endConvert(out, &dummyNumOut, -1);
}

template <class OUTPUT_ITERATOR>
int Base64Encoder::endConvert(OUTPUT_ITERATOR  out,
                              int             *numOut,
                              int              maxNumOut)
{
    BSLS_ASSERT(numOut);

    if (e_ERROR_STATE == d_state || isDone()) {
        d_state = e_ERROR_STATE;
        *numOut = 0;
        return -1;                                                    // RETURN
    }

    d_state = e_DONE_STATE;

    const int initialLength = d_outputLength;
    const int maxLength = d_outputLength + maxNumOut;

    // Handle trailing bits.

    const int residualBits = d_bitsInStack % 6;
    if (residualBits) {
        const int shift = 6 - residualBits;
        d_stack = d_stack << shift;
        d_bitsInStack += shift;
    }

    // Emit as many output bytes as possible.

    while (6 <= d_bitsInStack && d_outputLength != maxLength) {
        encode(&out, maxLength);
    }

    // Append trailing '=' as necessary.

    if (0 == d_bitsInStack) {
        while (isResidualOutput(d_outputLength, d_maxLineLength) &&
                                                 d_outputLength != maxLength) {
            append(&out, '=', maxLength);
        }
    }

    *numOut = d_outputLength - initialLength;

    return 0;
}

inline
void Base64Encoder::resetState()
{
    d_state        = e_INITIAL_STATE;
    d_outputLength = 0;
    d_lineLength   = 0;
    d_stack        = 0;
    d_bitsInStack  = 0;
}

// ACCESSORS
inline
bool Base64Encoder::isAcceptable() const
{
    return e_ERROR_STATE != d_state;
}

inline
bool Base64Encoder::isDone() const
{
    return e_DONE_STATE == d_state
        && !d_bitsInStack
        && !isResidualOutput(d_outputLength, d_maxLineLength);
}

inline
bool Base64Encoder::isError() const
{
    return e_ERROR_STATE == d_state;
}

inline
bool Base64Encoder::isInitialState() const
{
    return 0 == d_outputLength && e_INITIAL_STATE == d_state;
}

inline
bool Base64Encoder::isMaximal() const
{
    return e_DONE_STATE == d_state &&
          (d_bitsInStack || isResidualOutput(d_outputLength, d_maxLineLength));
}

inline
int Base64Encoder::maxLineLength() const
{
    return d_maxLineLength;
}

inline
int Base64Encoder::outputLength() const
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
