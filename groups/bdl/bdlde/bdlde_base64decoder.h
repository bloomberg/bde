// bdlde_base64decoder.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLDE_BASE64DECODER
#define INCLUDED_BDLDE_BASE64DECODER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide automata for converting to and from Base64 encodings.
//
//@CLASSES:
//   bdlde::Base64Decoder: automata performing Base64 decoding operations
//
//@SEE_ALSO: 'bdlde_base64encoder'
//
//@DESCRIPTION: This component a 'class', 'bdlde::Base64Decoder', which
// provides a pair of template functions (each parameterized separately on both
// input and output iterators) that can be used respectively to encode and to
// decode byte sequences of arbitrary length into and from the printable Base64
// representation described in Section 6.8 "Base64 Content Transfer Encoding"
// of RFC 2045, "Multipurpose Internet Mail Extensions (MIME) Part One: Format
// of Internet Message Bodies."
//
// Each instance of either the encoder or decoder retains the state of the
// conversion from one supplied input to the next, enabling the processing of
// segmented input -- i.e., processing resumes where it left off with the next
// invocation on new input.  Instance methods are provided for both the
// encoder and decoder to (1) assert the end of input, (2) determine whether
// the input so far is currently acceptable, and (3) indicate whether a
// non-recoverable error has occurred.
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
// I)  There is a single byte of data, in which case there will be two Base64
// encoding characters (the second of which will be one of [AQgw]) followed by
// two equal ('=') signs.
//
// II)  There are exactly two bytes of data, in which case there will be
// three Base64 encoding characters (the third of which will be one of
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
//               more than two '=' characters appear (possibly separated by
//             non-Base64 characters), a numeric Base64 character other than
//             [AEIMQUYcgkosw048] precedes a single terminal '=' character,
//             or a character other than [AQgw] precedes a terminal pair of
//             consecutive '=' characters.
//..
// The 'isError' method is used to detect such anomalies, and the 'numIn'
// output parameter (indicating the number of input characters consumed)
// or possibly the iterator itself (for iterators with reference-semantics)
// identifies the offending character.
//
// Note that the existence of an '=' can be used to reliably indicate the end
// of the valid data, but no such assurance is possible when the length (in
// bytes) of the initial input data sequence before encoding was evenly
// divisible by 3.
//
///Usage
///-----
// The following example shows how to use a 'bdlde::Base64Decoder' object to
// implement a function, 'streamconverter', that reads text from a
// 'bsl::istream', decodes that text from base 64 representation, and writes
// the decoded text to a 'bsl::ostream'.  'streamconverter' returns 0 on
// success and a negative value if the input data could not be successfully
// decoded or if there is an I/O error.
//..
// streamdecoder.h                      -*-C++-*-
//
// int streamDecoder(bsl::ostream& os, bsl::istream& is);
//     // Read the entire contents of the specified input stream 'is', convert
//     // the input base-64 encoding into plain text, and write the decoded
//     // text to the specified output stream 'os'.  Return 0 on success, and a
//     // negative value otherwise.
//..
// We will use fixed-sized input and output buffers in the implementation, but,
// because of the flexibility of 'bsl::istream' and the output-buffer
// monitoring functionality of 'bdlde::Base64Decoder', the fixed buffer sizes
// do *not* limit the quantity of data that can be read, decoded, or written to
// the output stream.  The implementation file is as follows.
//..
// streamdecoder.cpp                    -*-C++-*-
//
// #include <streamdecoder.h>
//
// #include <bdlde_base64decoder.h>
//
// namespace BloombergLP {
//
// int streamDecoder(bsl::ostream& os, bsl::istream& is)
// {
//     enum {
//         SUCCESS      =  0,
//         DECODE_ERROR = -1,
//         IO_ERROR     = -2
//     };
//..
// We declare a 'bdlde::Base64Decoder' object 'converter', which will decode
// the input data.  Note that various internal buffers and cursors are used as
// needed without further comment.  We read as much data as is available from
// the user-supplied input stream 'is' *or* as much as will fit in
// 'inputBuffer' before beginning conversion.  To obtain unobstructedly the
// output that results from decoding the entire input stream (even in the case
// of errors), the base64 decoder is configured not to detect errors.
//..
//     bdlde::Base64Decoder converter(false);   // Do not report errors.
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
// loop to decode the input and write the decoded data to 'outputBuffer' (via
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
//                 return DECODE_ERROR;                               // RETURN
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
// We have now exited both the input and the "decode" loops.  'converter' may
// still hold decoded output characters, and so we call 'converter.endConvert'
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
//             return DECODE_ERROR;                                   // RETURN
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
// For ease of reading, we repeat the full content of the 'streamconverter.cpp'
// file without interruption.
//..
// streamdecoder.cpp                    -*-C++-*-
//
// #include <streamdecoder.h>
//
// #include <bdlde_base64decoder.h>
//
// namespace BloombergLP {
//
// int streamDecoder(bsl::ostream& os, bsl::istream& is)
// {
//     enum {
//         SUCCESS      =  0,
//         DECODE_ERROR = -1,
//         IO_ERROR     = -2
//     };
//
//     bdlde::Base64Decoder converter(false);   // Do not report errors.
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
//                 return DECODE_ERROR;                               // RETURN
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
//             return DECODE_ERROR;                                   // RETURN
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
// } // Close namespace BloombergLP
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
                            // class Base64Decoder
                            // ===================

class Base64Decoder {
    // This class implements a mechanism capable of converting data of
    // arbitrary length from its corresponding Base64 representation.

    // PRIVATE TYPES
    enum {
        // Symbolic state values.

        e_ERROR_STATE      = -1, // input is irreparably invalid
        e_INPUT_STATE      =  0, // general input state
        e_NEED_EQUAL_STATE =  1, // need an '='
        e_SOFT_DONE_STATE  =  2, // only ignorable input and 'endConvert'
        e_DONE_STATE       =  3  // any additional input is an error
    };

    // CLASS DATA
    static const bool *const s_ignorableStrict_p; // Table identifying
                                                  // ignorable characters
                                                  // in strict mode
    static const bool *const s_ignorableRelaxed_p;// Table identifying
                                                  // ignorable characters
                                                  // in relaxed mode
    static const char *const s_decoding_p;        // a map from numeric Base64
                                                  // encoding characters to the
                                                  // corresponding 6-bit number

    // INSTANCE DATA
    const bool *const d_ignorable_p; // Selected table of ignorable characters
                                     // based on specified error-reporting mode

    signed char d_state;  // -1 = error state
                          //  0 = general input state
                          //  1 = need another '='
                          //  2 = soft done state - allow only ignorable input
                          //  3 = done state - no more input allowed

    int d_outputLength;          // total number of output characters

    // TBD doc

    int d_stack;
    int d_bitsInStack;

  private:
    // NOT IMPLEMENTED
    Base64Decoder(const Base64Decoder&);
    Base64Decoder& operator=(const Base64Decoder&);

  public:
    // CLASS METHODS
    static int maxDecodedLength(int inputLength);
        // Return the maximum number of decoded bytes that could result from an
        // input byte sequence of the specified 'inputLength' provided to the
        // 'convert' method of this decoder.  The behavior is undefined unless
        // '0 <= inputLength'.

    // CREATORS
    explicit
    Base64Decoder(bool unrecognizedIsErrorFlag);
        // Create a Base64 decoder in the initial state.  Unrecognized
        // characters (i.e., non-base64 characters other than whitespace) will
        // be treated as errors if the specified 'unrecognizedIsErrorFlag' is
        // 'true', and ignored otherwise.

    ~Base64Decoder();
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
        // Decode the sequence of input characters starting at the specified
        // 'begin' position up to, but not including, the specified 'end'
        // position, writing any resulting output characters to the specified
        // 'out' buffer.  Optionally specify the 'maxNumOut' limit on the
        // number of bytes to output; if 'maxNumOut' is negative, no limit is
        // imposed.  If the 'maxNumOut' limit is reached, no further input will
        // be consumed.  Load into the (optionally) specified 'numOut' and
        // 'numIn' the number of output bytes produced and input bytes
        // consumed, respectively.  Return a non-negative value on success, -1
        // on an input error, and -2 if the 'endConvert' method has already
        // been called without an intervening 'resetState' call.  A return
        // status of -1 indicates that the data at 'begin' + 'numIn'
        // constitutes an irrecoverably undecodable input sequence (i.e., the
        // data cannot be extended to form any valid encoding).  A positive
        // return status indicates the number of valid processed output bytes
        // retained by this decoder and not written to 'out' because
        // 'maxNumOut' has been reached; these bytes are available for output
        // if this method is called with appropriate input.  Note that it is
        // recommended that after all calls to 'convert' are finished, the
        // 'endConvert' method be called to complete the encoding of any
        // unprocessed input characters that do not complete a 3-byte sequence.

    template <class OUTPUT_ITERATOR>
    int endConvert(OUTPUT_ITERATOR out);
    template <class OUTPUT_ITERATOR>
    int endConvert(OUTPUT_ITERATOR  out,
                   int             *numOut,
                   int              maxNumOut = -1);
        // Terminate decoding for this decoder; write any retained output
        // (e.g., from a previous call to 'convert' with a non-zero optionally
        // specified 'maxNumOut' argument) to the specified 'out' buffer;
        // encode any unprocessed input characters that do not complete a
        // 3-byte sequence.  The argument 'maxNumOut' is the limit on the
        // number of bytes to output; if 'maxNumOut' is negative, no limit is
        // imposed.  Load into the (optionally) specified 'numOut' the number
        // of output bytes produced.  Return 0 on success, the positive number
        // of bytes *still* retained by this decoder if the 'maxNumOut' limit
        // was reached, and a negative value otherwise.  Any retained bytes are
        // available on a subsequent call to 'endConvert'.  Once this method is
        // called, no additional input may be supplied without an intervening
        // call to 'resetState'; once this method returns a zero status, a
        // subsequent call will place this decoder in the error state, and
        // return an error status.

    void resetState();
        // Reset this instance to its initial state (i.e., as if no input had
        // been consumed).

    // ACCESSORS
    bool isAcceptable() const;
        // Return 'true' if the input read so far is considered syntactically
        // complete, and 'false' otherwise.  Note that the number of relevant
        // input characters must be divisible by 4.

    bool isDone() const;
        // Return 'true' if the current input is acceptable and any additional
        // input (including 'endConvert') would be an error, and 'false'
        // otherwise.  Note that if this decoder 'isDone' then all resulting
        // output has been emitted to 'out'.

    bool isError() const;
        // Return 'true' if there is no possibility of achieving an
        // "acceptable" result, and 'false' otherwise.

    bool isInitialState() const;
        // Return 'true' if this instance is in the initial state (i.e., as
        // if no input had been consumed), and 'false' otherwise.

    bool isMaximal() const;
        // Return 'true' if the current input is acceptable and any additional
        // input (other than 'endConvert') would be an error, and 'false'
        // otherwise.

    bool isMinimal() const;
        // Return 'true' if this instance is in the initial state (i.e., as
        // if no input had been consumed), and 'false' otherwise.

    bool isUnrecognizedAnError() const;
        // Return 'true' if this mechanism is currently configured to report an
        // error when an unrecognized character (i.e., a character other than
        // one of the 64 "numeric" base-64 characters, '=', or whitespace) is
        // encountered, and 'false' otherwise.

    int outputLength() const;
        // Return the total length of the output emitted thus far.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // -------------------
                            // class Base64Decoder
                            // -------------------

// CLASS METHODS
inline
int Base64Decoder::maxDecodedLength(int inputLength)
{
    BSLS_ASSERT_SAFE(0 <= inputLength);

    return (inputLength + 3) / 4 * 3;
}

// CREATORS
inline
Base64Decoder::Base64Decoder(bool unrecognizedIsErrorFlag)
: d_ignorable_p(unrecognizedIsErrorFlag ? s_ignorableStrict_p
                                        : s_ignorableRelaxed_p)
, d_state(e_INPUT_STATE)
, d_outputLength(0)
, d_stack(0)
, d_bitsInStack(0)
{
}

// MANIPULATORS
template <class OUTPUT_ITERATOR, class INPUT_ITERATOR>
int Base64Decoder::convert(OUTPUT_ITERATOR out,
                           INPUT_ITERATOR  begin,
                           INPUT_ITERATOR  end)
{
    int dummyNumOut;
    int dummyNumIn;

    return convert(out, &dummyNumOut, &dummyNumIn, begin, end, -1);
}

template <class OUTPUT_ITERATOR, class INPUT_ITERATOR>
int Base64Decoder::convert(OUTPUT_ITERATOR  out,
                           int             *numOut,
                           int             *numIn,
                           INPUT_ITERATOR   begin,
                           INPUT_ITERATOR   end,
                           int              maxNumOut)
{
    BSLS_ASSERT(numOut);
    BSLS_ASSERT(numIn);

    if (e_ERROR_STATE == d_state || e_DONE_STATE == d_state) {
        int rv = e_DONE_STATE == d_state ? -2 : -1;
        d_state = e_ERROR_STATE;
        *numOut = 0;
        *numIn = 0;
        return rv;                                                    // RETURN
    }

    int numEmitted = 0;

    // Emit as many output bytes as possible.

    while (8 <= d_bitsInStack && numEmitted != maxNumOut) {
        d_bitsInStack -= 8;
        *out = static_cast<char>((d_stack >> d_bitsInStack) & 0xff);
        ++out;
        ++numEmitted;
    }

    // Consume as many input bytes as possible.

    *numIn = 0;

    if (e_INPUT_STATE == d_state) {
        while (18 >= d_bitsInStack && begin != end) {
            const unsigned char byte = static_cast<unsigned char>(*begin);

            ++begin;
            ++*numIn;

            unsigned char converted = static_cast<unsigned char>(
                                                           s_decoding_p[byte]);

            if (converted < 64) {
                d_stack = (d_stack << 6) | converted;
                d_bitsInStack += 6;
                if (8 <= d_bitsInStack && numEmitted != maxNumOut) {
                    d_bitsInStack -= 8;
                    *out = static_cast<char>(
                                            (d_stack >> d_bitsInStack) & 0xff);
                    ++out;
                    ++numEmitted;
                }
            }
            else if (!d_ignorable_p[byte]) {
                if ('=' == byte) {
                    const int residualBits =
                                       (((d_outputLength + numEmitted) % 3) * 8
                                                         + d_bitsInStack) % 24;
                    if (12 == residualBits && 0 == (d_stack & 0xf)) {
                        d_stack = d_stack >> 4;
                        d_bitsInStack -= 4;
                        d_state = e_NEED_EQUAL_STATE;
                    }
                    else if (18 == residualBits && 0 == (d_stack & 0x3)) {
                        d_stack = d_stack >> 2;
                        d_bitsInStack -= 2;
                        d_state = e_SOFT_DONE_STATE;
                    }
                    else {
                        d_state = e_ERROR_STATE;
                    }
                }
                else {
                    d_state = e_ERROR_STATE;
                }
                break;
            }
        }
    }
    if (e_NEED_EQUAL_STATE == d_state) {
        while (begin != end) {
            const unsigned char byte = static_cast<unsigned char>(*begin);

            ++begin;
            ++*numIn;

            if (!d_ignorable_p[byte]) {
                if ('=' == byte) {
                    d_state = e_SOFT_DONE_STATE;
                }
                else {
                    d_state = e_ERROR_STATE;
                }
                break;
            }
        }
    }
    if (e_SOFT_DONE_STATE == d_state) {
        while (begin != end) {
            const unsigned char byte = static_cast<unsigned char>(*begin);

            ++begin;
            ++*numIn;

            if (!d_ignorable_p[byte]) {
                d_state = e_ERROR_STATE;
                break;
            }
        }
    }

    *numOut = numEmitted;
    d_outputLength += numEmitted;

    return e_ERROR_STATE == d_state ? -1 : d_bitsInStack / 8;
}

template <class OUTPUT_ITERATOR>
int Base64Decoder::endConvert(OUTPUT_ITERATOR out)
{
    int dummyNumOut;

    return endConvert(out, &dummyNumOut, -1);
}

template <class OUTPUT_ITERATOR>
int Base64Decoder::endConvert(OUTPUT_ITERATOR  out,
                              int             *numOut,
                              int              maxNumOut)
{
    BSLS_ASSERT(numOut);

    const int residualBits = ((d_outputLength % 3) * 8 + d_bitsInStack) % 24;
    if (e_ERROR_STATE == d_state || e_NEED_EQUAL_STATE == d_state ||
                        (e_DONE_STATE == d_state && 0 == d_bitsInStack) ||
                                  (e_INPUT_STATE == d_state && residualBits)) {
        *numOut = 0;
        d_state = e_ERROR_STATE;
        return -1;                                                    // RETURN
    }

    d_state = e_DONE_STATE;

    int numEmitted = 0;
    while (8 <= d_bitsInStack && numEmitted != maxNumOut) {
        d_bitsInStack -= 8;
        *out = static_cast<char>((d_stack >> d_bitsInStack) & 0xff);
        ++out;
        ++numEmitted;
    }

    *numOut = numEmitted;
    d_outputLength += numEmitted;

    return e_ERROR_STATE == d_state ? -1 : d_bitsInStack / 8;
}

inline
void Base64Decoder::resetState()
{
    d_state        = e_INPUT_STATE;
    d_outputLength = 0;
    d_bitsInStack  = 0;
}

// ACCESSORS
inline
bool Base64Decoder::isAcceptable() const
{
    const int residualBits = ((d_outputLength % 3) * 8 + d_bitsInStack) % 24;
    return (0 == residualBits && e_INPUT_STATE == d_state) ||
                       e_SOFT_DONE_STATE == d_state || e_DONE_STATE == d_state;
}

inline
bool Base64Decoder::isDone() const
{
    return !d_bitsInStack && e_DONE_STATE == d_state;
}

inline
bool Base64Decoder::isError() const
{
    return e_ERROR_STATE == d_state;
}

inline
bool Base64Decoder::isInitialState() const
{
    return e_INPUT_STATE == d_state
        && 0 == d_bitsInStack
        && 0 == d_outputLength;
}

inline
bool Base64Decoder::isMaximal() const
{
    return e_SOFT_DONE_STATE == d_state
                                 || (d_bitsInStack && e_DONE_STATE == d_state);
}

inline
bool Base64Decoder::isUnrecognizedAnError() const
{
    return d_ignorable_p == s_ignorableStrict_p;
}

inline
int Base64Decoder::outputLength() const
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
