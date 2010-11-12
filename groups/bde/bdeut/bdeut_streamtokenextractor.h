// bdeut_streamtokenextractor.h -*-C++-*-
#ifndef INCLUDED_BDEUT_STREAMTOKENEXTRACTOR
#define INCLUDED_BDEUT_STREAMTOKENEXTRACTOR

//@PURPOSE: Facilitate extraction of user-described tokens from istreams.
//
//@CLASSES:
//   bdeut_StreamTokenExtractor: token extractot from ('istream') input streams
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component implements an input-stream token extractor 
// that provides sequential access to the user-described tokens in a given
// 'istream'.  The tokens are delimited by one or more user-described
// non-null characters in one or both of two delimiter sets: *soft* and *hard*.
//
///Definitions
///-----------
// A *delimiter* is a contiguous sequence of one or more delimiter characters
// that serves to separate or explicitly terminate user described tokens.
//
// A *soft* *delimiter* is a contiguous sequence of soft delimiter characters
// occurring in the user-specified input string between a token character and
// (1) the beginning of the input (also called the leader), (2) the end of
// the input, or (3) another token character.
//
// A *hard* *delimiter* is a maximal sequence of remaining (i.e., not 
// previously consumed) contiguous delimiter characters containing exactly 
// one hard delimiter character occurring in the user-specified input.
//
// A *non-null* *token* is a maximal sequence of one or more non-delimiter
// characters occurring in the user-specified input.
//
// A *null* *token* is a zero length sequence implied before a hard delimiter 
// character when that delimiter is not preceded by a token character in the 
// user-specified input.
//
///Overview
///--------
// Soft delimiters are used in applications where consecutive delimiter 
// characters need to be treated as one delimiter.  For example, declaring 
// white-space (" ") as the delimiter character and parsing
//..
//    "The   lazy dog"
//..
// should produce three tokens {"The", "lazy", "dog"} and not five tokens
// {"The", "", "", "lazy", "dog"}.  Hard delimiters are used in applications 
// where consecutive delimiter characters imply null tokens and need to be 
// treated as multiple delimiters.  For example, declaring forward-slash ("/")
// as the delimiter character and parsing the date "06//68" should produce 
// three tokens {"06", "", "68"}, and not two tokens {"06", "68"}.  To clarify
// the issue further consider the following stream:
//..
//    "\t These/are /  /\nthe   good-old  / \tdays:: \n"
//..
// Assuming " \t\n" are soft delimiter characters and "/:" are hard delimiter 
// characters, the following leader (described below) and sequence of
// token/delimiter pairs is produced:
//..
//    Leader   | Token      | Delimiter
//    ---------+------------+------------
//    "\t "    | "These"    | "/"
//             | "are"      | " /  "
//             | ""         | "/\n"
//             | "the"      | "   "
//             | "good-old" | "  / \t"
//             | "days"     | "!"
//             | ""         | "! \n"
//..
// Note that a contiguous sequence of soft delimiter characters between two
// hard delimiter characters (e.g., " /  /\n" in the example above) is always
// part of the first delimiter and not the second (i.e., {" /  ", "/\n"}
// rather than {" /", "  /\n"}).  Leader delimiter characters are any soft 
// delimiter characters found before the first token or hard delimiter in the
// input stream.  The bdeut_StreamTokenExtractor type supplies six elemental 
// functions along with two common-usage functions: 'get' and 'strip'.  
//
// It is sometimes desirable to interleave the use of multipal extractors on 
// a single input stream (see usage example below).  It is also not uncommon 
// to reuse a particular extractor on more than one stream.  Since an 
// extractor object is not inherently bound to any particular input stream, an
// input stream must be supplied each time any extraction function ('get' or 
// 'strip') is called.  The information in the internal buffer can be accessed
// with the '()' operator.  The 'isHard' method can be used to detect the 
// presence of a hard delimiter character after calling 'getDelimiter' (see 
// usage example below).  The 'hasSoft' method can be used to detect the 
// presence of a soft delimiter character after calling 'getDelimiter' (see 
// usage example below). 
//
///Elemental 'get' and 'strip' Functions
///-------------------------------------
// The three elemental 'get' functions, 'getLeader', 'getToken' and 
// 'getDelimiter', extract the respective characters from the input stream
// and load these characters into the internal buffer.  The corresponding 
// three elemental 'strip' functions, 'stripLeader', 'stripToken' and 
// 'stripDelimiter' strip the respective characters from the input stream 
// but do not alter the internal buffer.  A 'get' function applied to an 
// empty stream returns a non-zero value and also forces the stream to change
// to the 'eof' state.  A 'get' function applied to a non-empty stream may 
// produce a zero-length string in the internal buffer (e.g., 'getDelimiter' 
// applied to the input "no:delim") but does not cause the stream state to 
// change to the 'eof' state.  Under no circumstances will applying a 'strip' 
// function (even to an empty stream) cause that stream to change to the 'eof'
// state.  Assuming the same delimiter characters (soft: " \t\n", hard: "/:") 
// specified above, the following table illustrates the behavior of each of 
// the six elemental functions:
//..
//   function       | strip from input stream...  |store| e.g.,
//   ---------------+-----------------------------+-----+----------------------
//                  | all leading consecutive     |     | "  \t\tHello" // tok
//   getLeader      | soft delimiter characters   | yes |  ^^^^^^
//   _______________| up to the first token or    |_____| "\t  ::World" // hrd
//                  | hard delimiter character,   |     |  ^^^^
//   stripLeader    | or end of stream            | no  | "           " // eof
//                  |                             |     |  ^^^^^^^^^^^
//   ---------------+-----------------------------+-----+----------------------
//                  | all leading consecutive     |     | "Hello  \t: " // sft
//   getToken       | token characters  (i.e.,    | yes |  ^^^^^
//   _______________| non-delimiter characters)   |_____| "World::\t  " // hrd
//                  | up to the first soft or     |     |  ^^^^^
//   stripToken     | hard delimiter character,   | no  | "Again"       // eof
//                  | or end of stream            |     |  ^^^^^
//   ---------------+-----------------------------+-----+----------------------
//                  | all leading consecutive     |     | "\t  : Hello" // tok
//   getDelimiter   | delimiter characters up to  | yes |  ^^^^^^
//   _______________| the first token character,  |_____| ":   : World" // hrd
//                  | second hard delimiter       |     |  ^^^^
//   stripDelimiter | character, or end of stream | no  | "\t  :      " // eof
//                  |                             |     |  ^^^^^^^^^^^
//   ---------------+-----------------------------+-----+----------------------
//..
///Commonly Used (Composite) 'get' and 'strip' Functions
///-----------------------------------------------------
// The common-usage 'get' function combines 'stripLeader', 'getToken', and
// 'stripDelimiter', thus loading only the token characters in the internal
// buffer.  The common-usage 'strip' function combines 'stripLeader', 
// 'stripToken', and 'stripDelimiter', leaving the internal buffer unchanged.  
// (For additional examples of parsing strings with hard and soft delimiters,
// please refer to the usage section of the 'bdeut_strtokeniter' component.)  
//
///USAGE
///-----
// The following example illustrates the common-use 'get' function:
//..
//    #include <bdeut_streamtokenextractor.h>
//    #include <iostream>             // ostream
//
//    void listTokens(std::istream *in, const char *softDelimiterCharacters);
//    {
//        bdeut_StreamTokenExtractor it(softDelimiterCharacters);
//
//        for (;;) {
//            if (it.get(in)) break;
//
//            std::cout << it() << ":";      // print just the token followed by ':'
//        }
//    }
//..
// Assuming 'softDelimiterCharacters' " \t\n", applying the 'listTokens' 
// function to the following stream:
//..
//    "\n Now\t\tis the    time... "
//..
// produces the following output:
//..
//    Now:is:the:time...:
//..
// Had we desired, the entire input stream could have been parsed and 
// reconstructed by concatenating the values retrieved by successive calls to 
// 'getLeader', 'getToken' and 'getDelimiter' as follows:  
//..
//    #include <bdeut_streamtokenextractor.h>
//    #include <iostream>     
//
//    int pipe(std::ostream &out, std::istream *in, const char *soft, const char *hard)
//    {
//        bdeut_StreamTokenExtractor it(soft, hard);
//        if (it.getLeader(in)) return 0;
//        if (verbose) out << it();          // stream out leader
//
//        for (;;) {
//           if (it.getToken(in)) break;
//           out << it();                    // stream out token
//
//           if (it.getDelimiter(in)) break;
//           out << it();                    // stream out (trailing) delimiter
//        }
//
//        return it.isHard();                // Return 1 only if last token is
//                                           // followed by a hard delimiter.
//    }
//
//..
// As a final example, consider the following flat file of tabular information:
//..
//    Costello | Lou     | 456-789-0123 | ...... | Comedian    | .....
//    Einstein | Albert  | 345-678-9012 | ...... | Scientist   | .....
//    Harding  | Tonya   | 234-567-8901 | ...... | Ice Skater  | .....
//    Jordan   | Michael | 123-456-7890 | ...... | NBA Star    | .....
//    Rambo    | John    | 567-890-1234 | ...... | US Marines  | .....
//..
// suppose that our goal is to extract the last name and phone number from 
// each record (i.e., line) and display it on stdout as follows:
//..
//    Costello : 456-789-0123
//    Einstein : 345-678-9012
//     Harding : 234-567-8901
//      Jordan : 123-456-7890
//       Rambo : 567-890-1234
//..
// The following 'getNamePhone1' function uses an bdeut_StreamTokenExtractor 
// to parse records from the table and a nested bdeut_StrTokenIter to parse 
// individual fields from the resulting string to produce the desired output: 
//.. 
//    #include <bdeut_strtokeniter.h>
//    #include <bdeut_streamtokenextractor.h>
//    #include <iostream>
// 
//    void getNamePhone1(std::istream *inStream)
//    {
//        const char *const SOFT_DELIM = " \t";
//        const char *const HARD_DELIM = "|";
// 
//        bdeut_StreamTokenExtractor ex(0, "\n");
//        bdeut_StrTokenIter it(0, SOFT_DELIM, HARD_DELIM); // Note: null input
// 
//        for (;;) {    
//          if (ex.get(inStream)) break; // Note: beware of blank lines.
//          it.reset(ex());              // Set iterator to <Last Name>.
//          cout.width(10);              // Preset column width.
//          std::cout << it() << " : ";       // Append last name and separator.
//          if (it) ++it;                // Advance iterator to <First Name>.
//          if (it) ++it;                // Advance iterator to <phone Number>.
//          std::cout << it() << '\n';        // Append phone number and newline.
//       }
//    }
//..
// Alternatively, we can achieve the same result (perhaps more efficiently)
// by employing multiple (2) extractors concurrently on the same input stream.
// In this approach, One extractor is responsible for extracting the desired 
// fields while the other is used to strip the remaining fileds from the line.
//.. 
//    #include <bdeut_streamtokenextractor.h>
//    #include <iostream>
//
//    void getNamePhone2(std::istream *inStream)
//    {
//        const char *const SOFT_DELIM = " \t";
//        const char *const HARD_DELIM = "|";
//
//        bdeut_StreamTokenExtractor flush("\n");
//        bdeut_StreamTokenExtractor extract(SOFT_DELIM, HARD_DELIM);
//    
//        while (inStream && !inStream->eof()) {  // Note: empty lines ignored.
//            for (;;) {                              
//                flush.stripLeader(inStream);       // remove empty lines.
//                if (extract.get(inStream)) return; // Acquire <Last Name>. 
//                cout.width(10);                    // Preset column width.
//                std::cout << extract() << " : ";        // Append name & separatr.
//                extract.strip(inStream);           // Strip <First Name>. 
//                if (extract.get(inStream)) return; // Acquire <Phone #>. 
//                std::cout << extract() << '\n';         // Append <phone #> & \n.
//                flush.strip(inStream);             // Flush thru newlines.
//            }
//        }
//    }
//.. 

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDES_ASSERT
#include <bdes_assert.h>
#define INCLUDED_BDES_ASSERT
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>       // class std::istream
#define INCLUDE_IOSFWD
#endif

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif


                        // ================================
                        // class bdeut_StreamTokenExtractor
                        // ================================

namespace BloombergLP {

class bdeut_StreamTokenExtractor {
    // Extract user-described tokens from an istream.

    enum { TABLE_SIZE = 256 };

    std::vector<char> d_buf;        // internal buffer 
    char d_delimTable[TABLE_SIZE];  // table of delimiter characters

    // not implemented
    bdeut_StreamTokenExtractor(const bdeut_StreamTokenExtractor&);     
    bdeut_StreamTokenExtractor& operator=(const bdeut_StreamTokenExtractor&);

  public:
    // CREATORS
    bdeut_StreamTokenExtractor(const char *softDelims, 
                               const char *hardDelims = 0);
        // Create a token extractor in which each token is delimited by one 
        // or more (non-null) characters specified in one or both of the two 
        // delimiter string arguments: 'softDelims' and 'hardDelims'.  The hard
        // delimiter string need not be specified if no hard delimiters are 
        // required.  Each of the two constructor arguments may be empty ("") 
        // or null (0).  Note that any character specified as both a soft and 
        // a hard delimiter character is regarded as a hard delimiter character
        // (repeating a character in the same set is redundant and has no 
        // additional effect).
        
    ~bdeut_StreamTokenExtractor();

    // MANIPULATORS
    int getLeader(std::istream *input);
        // Extract from the specified 'input' stream any leading consecutive 
        // soft delimiter characters up to the first token character, hard 
        // delimiter character, or end of stream and load them in the internal
        // buffer.  If there are no soft delimiter characters at the head of 
        // the stream, no characters are extracted and the internal buffer is 
        // set to the empty string.  A non-zero value is returned only when 
        // the input stream is initially empty or otherwise invalid.

    int getToken(std::istream *input);
        // Extract from the specified 'input' stream any leading consecutive 
        // token characters up to the first soft or hard delimiter character, 
        // or end of stream and load them in the internal buffer.  If there 
        // are no token characters at the head of the stream, no characters 
        // are extracted and the internal buffer is set to the empty string.  
        // A non-zero value is returned only when the input stream is initially
        // empty or otherwise invalid.  

    int getDelimiter(std::istream *input);
        // Extract from the specified 'input' stream any leading consecutive 
        // delimiter characters up to the first token character, second hard 
        // delimiter character, or end of stream and load them in the internal
        // buffer.  If there are no delimiter characters at the head of the 
        // stream, no characters are extracted and the internal buffer is set 
        // to the empty string.  A non-zero value is returned only when the 
        // input stream is empty or otherwise invalid.

    int get(std::istream *input);
        // Strip from the specified 'input' stream and discard any leading 
        // consecutive soft delimiter characters up to the first token 
        // character, hard delimiter character or end of stream.  Next, 
        // extract the consecutive token characters up to the first delimiter 
        // character (or end of stream) and load them in the internal buffer.
        // Finally, extract and discard any subsequent consecutive delimiter 
        // characters up to the first token character, second hard delimiter 
        // character, or end of stream.  Return non-zero only if end of stream
        // is encountered before seeing any token or hard delimiter characters.
        // The behavior of this method is equivalent to (but is potentially 
        // more efficient than) the following implementation:
        //..
        // int get(std::istream *input) 
        // {
        //     stripLeader(input);
        //     int status = getToken(input);
        //     stripDelimiter(input);
        // }
        //..

    void stripLeader(std::istream *input);
        // Strip from the specified 'input' stream and discard any leading 
        // consecutive soft delimiter characters up to the first token 
        // character, hard delimiter character, or end of stream (leaving 
        // the internal buffer unchanged).  If there are no soft delimiter 
        // characters at the head of  the stream, no characters are stripped. 
        // Note that (unlike 'getLeader'), this method never causes a valid 
        // stream to attain an invalid state.

    void stripToken(std::istream *input);
        // Strip from the specified 'input' stream and discard any leading 
        // consecutive token characters up to the first soft or hard delimiter 
        // character, or end of stream (leaving the internal buffer unchanged).
        // If there are no token characters at the head of the stream, no 
        // characters are stripped.  Note that (unlike 'getToken'), this method
        // never causes a valid stream to attain an invalid state.

    void stripDelimiter(std::istream *input);
        // Strip from the specified 'input' stream and discard any leading 
        // consecutive delimiter characters up to the first token character, 
        // second hard delimiter character, or end of stream (leaving the 
        // internal buffer unchanged).  If there are no delimiter characters 
        // at the head of the stream, no characters are stripped.  Note that 
        // (unlike 'getToken'), this method never causes a valid stream to 
        // attain an invalid state.

    void strip(std::istream *input);
        // Strip from the specified 'input' stream and discard any leading
        // consecutive soft delimiter characters up to the first token 
        // character, hard delimiter character or end of stream.  Next, strip 
        // and discard the consecutive token characters up to the first 
        // delimiter character, or end of stream.  Finally, strip and discard 
        // any subsequent consecutive delimiter characters up to the first 
        // token character, second hard delimiter character, or end of stream.
        // Note that (unlike 'get'), this method never causes a valid stream to
        // attain an invalid state.  The behavior of this method is equivalent 
        // to (but is potentially more efficient than) the following 
        // implementation:
        //..
        // void strip(std::istream *input)
        // {
        //     stripLeader(input);
        //     stripToken(input);
        //     stripDelimiter(input);
        // }
        //..

    // ACCESSORS
    const char *operator()() const;
        // Return the contents of the internal buffer.  

    int isHard() const;
        // Return 1 if there is a hard delimiter character in the internal
        // buffer, and 0 otherwise.

    int hasSoft() const;
        // Return 1 if there are any soft delimiter characters in the internal
        // buffer, and 0 otherwise.
};
    
// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

inline
const char *bdeut_StreamTokenExtractor::operator()() const
{
    BDE_ASSERT_H(!d_buf.empty());
    return &d_buf.front();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
