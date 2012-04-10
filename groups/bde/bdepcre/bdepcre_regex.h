// bdepcre_regex.h                                                    -*-C++-*-
#ifndef INCLUDED_BDEPCRE_REGEX
#define INCLUDED_BDEPCRE_REGEX

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism for regular expression pattern matching.
//
//@CLASSES:
//  bdepcre_RegEx: mechanism for compiling and matching regular expressions
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: http://www.pcre.org/
//
//@DESCRIPTION: This component provides a mechanism, namely the 'bdepcre_RegEx'
// class, for compiling (or "preparing") regular expressions, and subsequently
// matching subject strings against a prepared expression.  The regular
// expressions supported by this component correspond approximately with Perl
// 5.8.  See the appendix entitled "Perl Compatibility" below for more
// information.
//
// Upon construction, a 'bdepcre_RegEx' object is initially not associated with
// a regular expression.  A regular expression pattern is compiled for use by
// the object using the 'prepare' method.  Subject strings may then be matched
// against the prepared pattern using the three overloaded 'match' methods.
// The first 'match' method simply returns 0 if a given subject string matches
// the prepared regular expression, and returns a non-zero value otherwise.
// The other two 'match' methods return the same status values, but the second
// variant also returns an 'bsl::pair<int, int>' holding the (offset, length)
// pair indicating the substring of the subject that matched.  The third
// variant returns an 'bsl::vector' of 'bsl::pair<int, int>'.  The first
// element of the vector holds the pair indicating the substring of the subject
// that matched the entire pattern.  Subsequent elements hold the pairs that
// indicate the substrings of the subject that matched respective sub-patterns.
// The Usage example below provides an example of sub-pattern matching.
//
///"Prepared" State
///----------------
// A 'bdepcre_RegEx' object must first be prepared with a valid regular
// expression before attempting to match subject strings.  We say that an
// instance of 'bdepcre_RegEx' is in the "prepared" state if the object holds
// a valid regular expression, in which case calls to the overloaded 'match'
// methods of that instance are valid.  Otherwise, the object is in the
// "unprepared" state.  Upon construction, an 'bdepcre_RegEx' object is in the
// "unprepared" state.  A successful call to the 'prepare' method puts the
// object into the "prepared" state.  The 'clear' method, as well as an
// unsuccessful call to 'prepare', puts the object into the "unprepared" state.
// The 'isPrepared' accessor may be used to determine whether an object is
// prepared.
//
///Prepare-Time Flags
///------------------
// A set of flags may be optionally supplied to the 'prepare' method to affect
// specific pattern matching behavior.  The flags recognized by 'prepare' are
// defined in an enumeration declared within the 'bdepcre_RegEx'.  The
// following describes these flags and their effects.
//
///Case-Insensitive Matching
///- - - - - - - - - - - - -
// If 'bdepcre_RegEx::BDEPCRE_FLAG_CASELESS' is included in the flags supplied
// to 'prepare', then letters in the regular expression pattern supplied to
// 'prepare' match both lower- and upper-case letters in subject strings
// subsequently supplied to 'match'.  This is equivalent to Perl's '/i' option,
// and can be turned off within a pattern by a '(?i)' option setting.
//
///Multi-Line Matching
///- - - - - - - - - -
// By default, a subject string supplied to 'match' is treated as consisting of
// a single line of characters (even if it actually contains '\n' characters).
// The start-of-line meta-character '^' matches only at the beginning of the
// string, and the end-of-line meta-character '$' matches only at the end of
// the string (or before a terminating '\n', if present).  This matches the
// behavior of Perl.
//
// If 'bdepcre_RegEx::BDEPCRE_FLAG_MULTILINE' is included in the flags supplied
// to 'prepare', then start-of-line and end-of-line meta-characters match
// immediately following or immediately before any '\n' characters in subject
// strings supplied to 'match', respectively (as well as at the very start and
// end of subject strings).  This is equivalent to Perl's '/m' option, and can
// be turned off within a pattern by a '(?m)' option setting.  If there are no
// '\n' characters in the subject string, or if there are no occurrences of '^'
// or '$' in the prepared pattern, then including 'BDEPCRE_FLAG_MULTILINE' has
// no effect.
//
///UTF-8 Support
///- - - - - - -
// If 'bdepcre_RegEx::BDEPCRE_FLAG_UTF8' is included in the flags supplied to
// 'prepare', then the regular expression pattern supplied to 'prepare', as
// well as the subject strings subsequently supplied to 'match', are
// interpreted as strings of UTF-8 characters instead of strings of ASCII
// characters.  If 'BDEPCRE_FLAG_UTF8' is used to prepare a regular expression,
// then any subject strings passed to the 'match' methods *must* be valid
// UTF-8; otherwise, unexpected behavior may result.
//
///Dot Matches All
///- - - - - - - -
// If 'bdepcre_RegEx::BDEPCRE_FLAG_DOTMATCHESALL' is included in the flags
// supplied to 'prepare', then a dot metacharacter in the pattern matches a
// character of any value, including one that indicates a newline.  However, it
// only ever matches one character, even if newlines are encoded as '\r\n'.
// If 'BDEPCRE_FLAG_DOTMATCHESALL' is not used to prepare a regular expression,
// a dot metacharacter will *not* match a newline; hence, patterns expected to
// match across lines will fail to do so.  This flag is equivalent to Perl's
// '/s' option, and can be changed within a pattern by a '(?s)' option setting.
// A negative class such as '[^a]' always matches newline characters,
// independent of the setting of this option.
//
///Usage
///-----
// The following snippets of code illustrate using this component to extract
// the text of the "Subject:" field from an Internet e-mail message (RFC822).
// The following 'parseSubject' function accepts an RFC822-compliant message of
// a specified length and returns the text of the message's subject in the
// 'result' "out" parameter:
//..
//  #include <bdepcre_regex.h>
//
//  using namespace BloombergLP;
//
//  int parseSubject(bsl::string *result,
//                   const char  *message,
//                   int          messageLength)
//      // Parse the specified 'message' of the specified 'messageLength' for
//      // the "Subject:" field of 'message'.  Return 0 on success and load the
//      // specified 'result' with the text of the subject of 'message'; return
//      // a non-zero value otherwise with no effect on 'result'.
//  {
//..
// The following is the regular expression that will be used to find the
// subject text of 'message'.  The "?P<subjectText>" syntax, borrowed from
// Python, allows us later to refer to a particular matched sub-pattern (i.e.,
// the text between the ':' and the '\r' in the "Subject:" field of the header)
// by the name "subjectText":
//..
//      const char PATTERN[] = "^subject:(?P<subjectText>[^\r]*)";
//..
// We first need to compile 'PATTERN', using the 'prepare' method, in order to
// match subject strings against it.  In the event that 'prepare' fails, the
// first two arguments will be loaded with diagnostic information (an
// informational string and an index into the pattern at which the error
// occurred, respectively).  Two flags, 'bdepcre_RegEx::BDEPCRE_FLAG_CASELESS'
// and 'bdepcre_RegEx::BDEPCRE_FLAG_MULTILINE', are used in preparing the
// pattern since Internet message headers contain case-insensitive content as
// well as '\n' characters.  The 'prepare' method returns 0 on success, and a
// non-zero value otherwise:
//..
//      bdepcre_RegEx regEx;
//      bsl::string   errorMessage;
//      int           errorOffset;
//
//      int returnValue = regEx.prepare(&errorMessage,
//                                      &errorOffset,
//                                      PATTERN,
//                                      bdepcre_RegEx::BDEPCRE_FLAG_CASELESS |
//                                      bdepcre_RegEx::BDEPCRE_FLAG_MULTILINE);
//      assert(0 == returnValue);
//..
// Next we call 'match' supplying 'message' and its length.  The 'matchVector'
// will be populated with (offset, length) pairs describing substrings in
// 'message' that match the prepared 'PATTERN'.  All variants of the overloaded
// 'match' method return 0 if a match is found, and return a non-zero value
// otherwise:
//..
//      bsl::vector<bsl::pair<int, int> > matchVector;
//      returnValue = regEx.match(&matchVector, message, messageLength);
//
//      if (0 != returnValue) {
//          return returnValue;  // no match
//      }
//..
// Next we pass "subjectText" to the 'subpatternIndex' method to obtain the
// index into 'matchVector' that describes how to locate the subject text
// within 'message'.  The text is then extracted from 'message' and assigned to
// the 'result' "out" parameter:
//..
//      const bsl::pair<int, int> capturedSubject =
//                           matchVector[regEx.subpatternIndex("subjectText")];
//
//      *result = bsl::string(&message[capturedSubject.first],
//                            capturedSubject.second);
//
//      return 0;
//  }
//..
// The following array contains the sample Internet e-mail message from which
// we will extract the subject:
//..
//  const char RFC822_MESSAGE[] =
//      "Received: ; Fri, 23 Apr 2004 14:30:00 -0400\r\n"
//      "Message-ID: <12345@mailgate.bloomberg.net>\r\n"
//      "Date: Fri, 23 Apr 2004 14:30:00 -0400\r\n"
//      "From: <someone@bloomberg.net>\r\n"
//      "To: <someone_else@bloomberg.net>\r\n"
//      "Subject: This is the subject text\r\n"
//      "MIME-Version: 1.0\r\n"
//      "Content-Type: text/plain\r\n"
//      "\r\n"
//      "This is the message body.\r\n"
//      ".\r\n";
//..
// Finally, the following 'main' function calls 'parseSubject' to extract the
// subject from 'RFC822_MESSAGE'.  The assertions verify that the subject of
// the message is correctly extracted and assigned to the local 'subject'
// variable:
//..
//  int main()
//  {
//      bsl::string subject;
//      const int   returnValue = parseSubject(&subject,
//                                             RFC822_MESSAGE,
//                                             sizeof(RFC822_MESSAGE) - 1);
//      assert(0 == returnValue);
//      assert(" This is the subject text" == subject);
//  }
//..
///Appendix - Perl Compatibility
///-----------------------------
// This section describes the differences between Perl 5.8 regular expressions
// and the regular expressions supported by this component.
//
// 1) This component does not allow repeat quantifiers on lookahead assertions.
// Perl permits them, but their meaning can be misleading.  For example,
// '(?!a){3}' does not assert that the next three characters are not 'a'.  It
// asserts three times that the next character is not 'a'.
//
// 2) Capturing sub-patterns that occur inside negative lookahead assertions
// are counted, but their entries in the result vector returned by the 'match'
// method are never set.  Perl sets its numerical variables from any such
// patterns that are matched before the assertion fails to match something
// (thereby succeeding), but only if the negative lookahead assertion contains
// just one branch.
//
// 3) Although null characters are supported in the subject string, they are
// not allowed in a pattern string because it is passed to the 'prepare' method
// as a null-terminated C-style string.  The escape sequence '\0' can be used
// in the pattern to represent a null character.
//
// 4) The following Perl escape sequences are not supported: '\l', '\u', '\L',
// '\U', and '\N'.  In fact, these are implemented by Perl's general
// string-handling and are not part of its pattern-matching engine.  If any of
// these are encountered by this component, an error is generated.
//
// 5) The Perl escape sequences '\p', '\P', and '\X' are supported only if the
// underlying PCRE library is built with Unicode character property support
// (which is not the case in this release).  The properties that can be tested
// with '\p' and '\P' are limited to the general category properties such as
// 'Lu' and 'Nd'.
//
// 6) This component does support the '\Q' ... '\E' escape for quoting
// substrings.  Enclosed characters are treated as literals.  This is slightly
// different from Perl in that '$' and '@' are also handled as literals inside
// the quotes.  In Perl, they cause variable interpolation (but, of course,
// this component does not have variable interpolation).  Note the following
// examples:
//..
//  Pattern           'bdepcre_RegEx' matches     Perl matches
//  - - - -           - - - - - - - - - - - -     - - - - - -
//  \Qabc$xyz\E       abc$xyz              abc followed by the contents of $xyz
//  \Qabc\$xyz\E      abc\$xyz                   abc\$xyz
//  \Qabc\E\$\Qxyz\E  abc$xyz                    abc$xyz
//..
// The '\Q' ... '\E' sequence is recognized both inside and outside character
// classes.
//
// 7) This component does not support the '(?{code})' and '(?p{code})'
// constructions.  However, there is support for recursive patterns using the
// non-Perl items '(?R)', '(?number)', and '(?P>name)'.
//
// 8) There are some differences that are concerned with the settings of
// captured strings when part of a pattern is repeated.  For example, matching
// "aba" against the pattern '/^(a(b)?)+$/' in Perl leaves '$2' unset, but this
// component sets it to "b".
//
// This component provides the following extensions to the Perl 5.8 regular
// expression facilities:
//
// 1) Although look-behind assertions must match fixed length strings, each
// alternative branch of a look-behind assertion can match a different length
// of string.  Perl requires them all to have the same length.
//
// 2) The '(?R)', '(?number)', and '(?P>name)' constructs allow for recursive
// pattern matching.  Perl can do this using the '(?p{code})' construct, which
// this component cannot support.
//
// 3) This component supports named capturing substrings using Python syntax
// (e.g., '?P<subpatternName>').
//
// 4) This component supports the possessive quantifier "++" syntax, taken from
// Sun's Java package.
//
// 5) An '(R)' condition is supported for testing recursion.
//
///Additional Copyright Notice
///---------------------------
// Copyright (c) 1997-2004 University of Cambridge
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the University of Cambridge nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>        // bsl::pair
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                             // ===================
                             // class bdepcre_RegEx
                             // ===================

class bdepcre_RegEx {
    // This class provides a mechanism for compiling and matching regular
    // expressions.  A regular expression approximately compatible with Perl
    // 5.8 is compiled with the 'prepare' method.  Subsequently, strings are
    // matched against the compiled (prepared) pattern using the overloaded
    // 'match' methods.  Note that the underlying implementation uses the
    // open-source Perl Compatible Regular Expressions (PCRE) library that was
    // developed at the University of Cambridge ('http://www.pcre.org/').

    // PRIVATE TYPES
    struct Pcre;  // opaque type for the PCRE library

    // PRIVATE DATA
    int              d_flags;        // prepare/match flags
    bsl::string      d_pattern;      // regular expression pattern
    Pcre            *d_pcre_p;       // PCRE's internal data structure (owned)
    bslma_Allocator *d_allocator_p;  // memory allocator (held, not owned)

    // NOT IMPLEMENTED
    bdepcre_RegEx(const bdepcre_RegEx& original);
    bdepcre_RegEx& operator=(const bdepcre_RegEx& rhs);

  public:
    // PUBLIC TYPES
    enum {
        BDEPCRE_FLAG_CASELESS         = 0x0001,  // case-insensitive matching

        BDEPCRE_FLAG_DOTMATCHESALL    = 0x0004,  // dot metacharacter matches
                                                 // all chars (including
                                                 // newlines)

        BDEPCRE_FLAG_MULTILINE        = 0x0002,  // multi-line matching

        BDEPCRE_FLAG_UTF8             = 0x0800   // UTF-8 support

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , FLAG_CASELESS  = BDEPCRE_FLAG_CASELESS
      , FLAG_MULTILINE = BDEPCRE_FLAG_MULTILINE
      , FLAG_UTF8      = BDEPCRE_FLAG_UTF8
#endif
    };
        // This enumeration defines the flags that may be supplied to the
        // 'prepare' method to effect specific pattern matching behavior.

    // CREATORS
    bdepcre_RegEx(bslma_Allocator *basicAllocator = 0);
        // Create a regular-expression object in the "unprepared" state.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~bdepcre_RegEx();
        // Destroy this regular-expression object.

    // MANIPULATORS
    void clear();
        // Free resources used by this regular-expression object and put this
        // object into the "unprepared" state.  This method has no effect if
        // this object is already in the "unprepared" state.

    int prepare(bsl::string *errorMessage,
                int         *errorOffset,
                const char  *pattern,
                int          flags = 0);
        // Prepare this regular-expression object with the specified 'pattern'
        // and the optionally-specified 'flags'.  On success, put this object
        // into the "prepared" state and return 0, with no effect on the
        // specified 'errorMessage' and 'errorOffset'.  Otherwise, (1) put this
        // object into the "unprepared" state, (2) load 'errorMessage' (if
        // non-null) with a string describing the error detected, (3) load
        // 'errorOffset' (if non-null) with the offset in 'pattern' at which
        // the error was detected, and (4) return a non-zero value.  The
        // behavior is undefined unless 'flags' is the bit-wise inclusive-or of
        // 0 or more of the following values:
        //..
        //  BDEPCRE_FLAG_CASELESS
        //  BDEPCRE_FLAG_DOTMATCHESALL
        //  BDEPCRE_FLAG_MULTILINE
        //  BDEPCRE_FLAG_UTF8
        //..

    // ACCESSORS
    int flags() const;
        // Return the flags that were supplied to the most recent successful
        // call to the 'prepare' method of this regular-expression object.  The
        // behavior is undefined unless 'isPrepared() == true'.  Note that the
        // returned value will be the bit-wise inclusive-or of 0 or more of the
        // following values:
        //..
        //  BDEPCRE_FLAG_CASELESS
        //  BDEPCRE_FLAG_DOTMATCHESALL
        //  BDEPCRE_FLAG_MULTILINE
        //  BDEPCRE_FLAG_UTF8
        //..

    bool isPrepared() const;
        // Return 'true' if this regular-expression object is in the "prepared"
        // state, and 'false' otherwise.

    int match(const char *subject,
              int         subjectLength,
              int         subjectOffset = 0) const;
        // Match the specified 'subject', having the specified 'subjectLength',
        // against the pattern held by this regular-expression object
        // ('pattern()').  Begin matching at the optionally-specified
        // 'subjectOffset' in 'subject'.  If 'subjectOffset' is not specified,
        // then begin matching from the start of 'subject'.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless 'isPrepared() == true', '0 <= subjectLength',
        // '0 <= subjectOffset', and 'subjectOffset <= subjectLength'.  The
        // behavior is also undefined if 'pattern()' was prepared with
        // 'BDEPCRE_FLAG_UTF8', but 'subject' is not valid UTF-8.  Note that
        // 'subject' need not be null-terminated and may contain embedded null
        // characters.

    int match(bsl::pair<int, int> *result,
              const char          *subject,
              int                  subjectLength,
              int                  subjectOffset = 0) const;
        // Match the specified 'subject', having the specified 'subjectLength',
        // against the pattern held by this regular-expression object
        // ('pattern()').  Begin matching at the optionally-specified
        // 'subjectOffset' in 'subject'.  If 'subjectOffset' is not specified,
        // then begin matching from the start of 'subject'.  On success, load
        // the specified 'result' with the '(offset, length)' pair indicating
        // the leftmost match of 'pattern()', and return 0.  Otherwise, return
        // a non-zero value with no effect on 'result'.  The behavior is
        // undefined unless 'isPrepared() == true', '0 <= subjectLength',
        // '0 <= subjectOffset', and 'subjectOffset <= subjectLength'.  The
        // behavior is also undefined if 'pattern()' was prepared with
        // 'BDEPCRE_FLAG_UTF8', but 'subject' is not valid UTF-8.  Note that
        // 'subject' need not be null-terminated and may contain embedded null
        // characters.

    int match(bsl::vector<bsl::pair<int, int> > *result,
              const char                        *subject,
              int                                subjectLength,
              int                                subjectOffset = 0) const;
        // Match the specified 'subject', having the specified 'subjectLength',
        // against the pattern held by this regular-expression object
        // ('pattern()').  Begin matching at the optionally-specified
        // 'subjectOffset' in 'subject'.  If 'subjectOffset' is not specified,
        // then begin matching from the start of 'subject'.  On success, (1)
        // load the first element of the specified 'result' with the
        // '(offset, length)' pair indicating the leftmost match of
        // 'pattern()', (2) load elements of 'result' in the range
        // '[ 1 .. numSubpatterns() ]' with the pairs indicating the respective
        // matches of sub-patterns (unmatched sub-patterns have their
        // respective 'result' elements loaded with '(-1, 0)'; sub-patterns
        // matching multiple times have their respective 'result' elements
        // loaded with the pairs indicating the rightmost match), and (3)
        // return 0.  Otherwise, return a non-zero value with no effect on
        // 'result'.  The behavior is undefined unless 'isPrepared() == true',
        // '0 <= subjectLength', '0 <= subjectOffset', and
        // 'subjectOffset <= subjectLength'.  The behavior is also undefined if
        // 'pattern()' was prepared with 'BDEPCRE_FLAG_UTF8', but 'subject' is
        // not valid UTF-8.  Note that 'subject' need not be null-terminated
        // and may contain embedded null characters.  Also note that after a
        // successful call, 'result' will contain exactly
        // 'numSubpatterns() + 1' elements.

    int numSubpatterns() const;
        // Return the number of sub-patterns in the pattern held by this
        // regular-expression object ('pattern()').  The behavior is undefined
        // unless 'isPrepared() == true'.

    const bsl::string& pattern() const;
        // Return a reference to the non-modifiable pattern held by this
        // regular-expression object.  The behavior is undefined unless
        // 'isPrepared() == true'.

    int subpatternIndex(const char *name) const;
        // Return the 1-based index of the sub-pattern having the specified
        // 'name' in the pattern held by this regular-expression object
        // ('pattern()'); return -1 if 'pattern()' does not contain a
        // sub-pattern identified by 'name'.  The behavior is undefined unless
        // 'isPrepared() == true'.  Note that the returned value is intended to
        // be used as an index into the 'bsl::vector<bsl::pair<int, int> >'
        // returned by 'match'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // -------------------
                             // class bdepcre_RegEx
                             // -------------------

// CREATORS
inline
bdepcre_RegEx::~bdepcre_RegEx()
{
    clear();
}

// ACCESSORS
inline
int bdepcre_RegEx::flags() const
{
    return d_flags;
}

inline
bool bdepcre_RegEx::isPrepared() const
{
    return 0 != d_pcre_p;
}

inline
const bsl::string& bdepcre_RegEx::pattern() const
{
    return d_pattern;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
