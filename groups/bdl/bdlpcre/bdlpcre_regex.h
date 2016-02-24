// bdlpcre_regex.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLPCRE_REGEX
#define INCLUDED_BDLPCRE_REGEX

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a mechanism for regular expression pattern matching.
//
//@CLASSES:
//  bdlpcre::RegEx: mechanism for compiling and matching regular expressions
//
//@SEE_ALSO: http://www.pcre.org/
//
//@DESCRIPTION: This component provides a mechanism, 'bdlpcre::RegEx', for
// compiling (or "preparing") regular expressions, and subsequently matching
// subject strings against a prepared expression.  The regular expressions
// supported by this component correspond approximately with Perl 5.10.  See
// the appendix entitled "Perl Compatibility" below for more information.
//
// Upon construction, a 'bdlpcre::RegEx' object is initially not associated
// with a regular expression.  A regular expression pattern is compiled for use
// by the object using the 'prepare' method.  Subject strings may then be
// matched against the prepared pattern using the three overloaded 'match'
// methods.  The first 'match' method simply returns 0 if a given subject
// string matches the prepared regular expression, and returns a non-zero value
// otherwise.  The other two 'match' methods return the same status values, but
// the second variant also returns an 'bsl::pair<int, int>' holding the
// (offset, length) pair indicating the substring of the subject that matched.
// The third variant returns an 'bsl::vector' of 'bsl::pair<int, int>'.  The
// first element of the vector holds the pair indicating the substring of the
// subject that matched the entire pattern.  Subsequent elements hold the pairs
// that indicate the substrings of the subject that matched respective
// sub-patterns.  The Usage example below provides an example of sub-pattern
// matching.
//
///"Prepared" State
///----------------
// A 'bdlpcre::RegEx' object must first be prepared with a valid regular
// expression before attempting to match subject strings.  We say that an
// instance of 'bdlpcre::RegEx' is in the "prepared" state if the object holds
// a valid regular expression, in which case calls to the overloaded 'match'
// methods of that instance are valid.  Otherwise, the object is in the
// "unprepared" state.  Upon construction, an 'bdlpcre::RegEx' object is in the
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
// defined in an enumeration declared within the 'bdlpcre::RegEx'.  The
// following describes these flags and their effects.
//
///Case-Insensitive Matching
///- - - - - - - - - - - - -
// If 'RegEx::k_FLAG_CASELESS' is included in the flags supplied to 'prepare',
// then letters in the regular expression pattern supplied to 'prepare' match
// both lower- and upper-case letters in subject strings subsequently supplied
// to 'match'.  This is equivalent to Perl's '/i' option, and can be turned off
// within a pattern by a '(?i)' option setting.
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
// If 'RegEx::k_FLAG_MULTILINE' is included in the flags supplied to 'prepare',
// then start-of-line and end-of-line meta-characters match immediately
// following or immediately before any '\n' characters in subject strings
// supplied to 'match', respectively (as well as at the very start and end of
// subject strings).  This is equivalent to Perl's '/m' option, and can be
// turned off within a pattern by a '(?m)' option setting.  If there are no
// '\n' characters in the subject string, or if there are no occurrences of '^'
// or '$' in the prepared pattern, then including 'k_FLAG_MULTILINE' has no
// effect.
//
///UTF-8 Support
///- - - - - - -
// If 'RegEx::k_FLAG_UTF8' is included in the flags supplied to 'prepare', then
// the regular expression pattern supplied to 'prepare', as well as the subject
// strings subsequently supplied to 'match', are interpreted as strings of
// UTF-8 characters instead of strings of ASCII characters.  The behavior of
// 'match' methods is undefined if 'pattern()' was prepared with 'k_FLAG_UTF8',
// but 'subject' is not a valid UTF-8 string.
//
///Dot Matches All
///- - - - - - - -
// If 'RegEx::k_FLAG_DOTMATCHESALL' is included in the flags supplied to
// 'prepare', then a dot metacharacter in the pattern matches a character of
// any value, including one that indicates a newline.  However, it only ever
// matches one character, even if newlines are encoded as '\r\n'.  If
// 'k_FLAG_DOTMATCHESALL' is not used to prepare a regular expression, a dot
// metacharacter will *not* match a newline; hence, patterns expected to match
// across lines will fail to do so.  This flag is equivalent to Perl's '/s'
// option, and can be changed within a pattern by a '(?s)' option setting.  A
// negative class such as '[^a]' always matches newline characters, independent
// of the setting of this option.
//
///Usage
///-----
// The following snippets of code illustrate using this component to extract
// the text of the "Subject:" field from an Internet e-mail message (RFC822).
// The following 'parseSubject' function accepts an RFC822-compliant message of
// a specified length and returns the text of the message's subject in the
// 'result' "out" parameter:
//..
//  int parseSubject(bsl::string *result,
//                   const char  *message,
//                   bsl::size_t  messageLength)
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
// First we compile the 'PATTERN', using the 'prepare' method, in order to
// match subject strings against it.  In the event that 'prepare' fails, the
// first two arguments will be loaded with diagnostic information (an
// informational string and an index into the pattern at which the error
// occurred, respectively).  Two flags, 'RegEx::k_FLAG_CASELESS' and
// 'RegEx::k_FLAG_MULTILINE', are used in preparing the pattern since Internet
// message headers contain case-insensitive content as well as '\n' characters.
// The 'prepare' method returns 0 on success, and a non-zero value otherwise:
//..
//      RegEx       regEx;
//      bsl::string errorMessage;
//      size_t      errorOffset;
//
//      int returnValue = regEx.prepare(&errorMessage,
//                                      &errorOffset,
//                                      PATTERN,
//                                      RegEx::k_FLAG_CASELESS |
//                                      RegEx::k_FLAG_MULTILINE);
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
// Then we pass "subjectText" to the 'subpatternIndex' method to obtain the
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
// Finally, we call 'parseSubject' to extract the subject from
// 'RFC822_MESSAGE'.  The assertions verify that the subject of the message is
// correctly extracted and assigned to the local 'subject' variable:
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
//
///Appendix - Perl Compatibility
///- - - - - - - - - - - - - - -
// This section describes the differences in the ways that PCRE2 and Perl
// handle regular expressions.  The differences described here are with respect
// to Perl versions 5.10 and above.
//
// 1) PCRE2 has only a subset of Perl's Unicode support.
//
// 2) PCRE2 allows repeat quantifiers only on parenthesized assertions, but
// they do not mean what you might think.  For example, (?!a){3} does not
// assert that the next three characters are not "a".  It just asserts that the
// next character is not "a" three times (in principle: PCRE2 optimizes this to
// run the assertion just once).  Perl allows repeat quantifiers on other
// assertions such as \b, but these do not seem to have any use.
//
// 3) Capturing subpatterns that occur inside negative lookahead assertions are
// counted, but their entries in the offsets vector are never set.  Perl
// sometimes (but not always) sets its numerical variables from inside negative
// assertions.
//
// 4) The following Perl escape sequences are not supported: \l, \u, \L, \U,
// and \N when followed by a character name or Unicode value.  (\N on its own,
// matching a non-newline character, is supported.)  In fact these are
// implemented by Perl's general string-handling and are not part of its
// pattern matching engine.  If any of these are encountered by PCRE2, an error
// is generated by default.
//
// 5) The Perl escape sequences \p, \P, and \X are supported only if PCRE2 is
// built with Unicode support.  The properties that can be tested with \p and
// \P are limited to the general category properties such as Lu and Nd, script
// names such as Greek or Han, and the derived properties Any and L&.  PCRE2
// does support the Cs (surrogate) property, which Perl does not; the Perl
// documentation says "Because Perl hides the need for the user to understand
// the internal representation of Unicode characters, there is no need to
// implement the somewhat messy concept of surrogates."
//
// 6) PCRE2 does support the \Q...\E escape for quoting substrings.  Characters
// in between are treated as literals.  This is slightly different from Perl in
// that $ and @ are also handled as literals inside the quotes.  In Perl, they
// cause variable interpolation (but of course PCRE2 does not have variables).
// Note the following examples:
//
// Pattern            PCRE2 matches  Perl matches
// ----------------   -------------  ------------------------------------
// \Qabc$xyz\E        abc$xyz        abc followed by the contents of $xyz
// \Qabc\$xyz\E       abc\$xyz       abc\$xyz
// \Qabc\E\$\Qxyz\E   abc$xyz        abc$xyz
//
// The \Q...\E sequence is recognized both inside and outside character
// classes.
//
// 7) PCRE2 does not support the (?{code}) and (??{code}) constructions.
// However, there is support for recursive patterns.  This is not available in
// Perl 5.8, but it is in Perl 5.10.
//
// 8) Subroutine calls (whether recursive or not) are treated as atomic groups.
// Atomic recursion is like Python, but unlike Perl.  Captured values that are
// set outside a subroutine call can be referenced from inside in PCRE2, but
// not in Perl.
//
// 9) If any of the backtracking control verbs are used in a subpattern that is
// called as a subroutine (whether or not recursively), their effect is
// confined to that subpattern; it does not extend to the surrounding pattern.
// This is not always the case in Perl.  In particular, if (*THEN) is present
// in a group that is called as a subroutine, its action is limited to that
// group, even if the group does not contain any | characters.  Note that such
// subpatterns are processed as anchored at the point where they are tested.
//
// 10) If a pattern contains more than one backtracking control verb, the first
// one that is backtracked onto acts.  For example, in the pattern
// A(*COMMIT)B(*PRUNE)C a failure in B triggers (*COMMIT), but a failure in C
// triggers (*PRUNE).  Perl's behaviour is more complex; in many cases it is
// the same as PCRE2, but there are examples where it differs.
//
// 11) Most backtracking verbs in assertions have their normal actions. They
// are not confined to the assertion.
//
// 12) There are some differences that are concerned with the settings of
// captured strings when part of a pattern is repeated.  For example, matching
// "aba" against the pattern /^(a(b)?)+$/ in Perl leaves $2 unset, but in PCRE2
// it is set to "b".
//
// 13) PCRE2's handling of duplicate subpattern numbers and duplicate
// subpattern names is not as general as Perl's.  This is a consequence of the
// fact the  PCRE2  works  internally just with numbers, using an external
// table to translate between numbers and names.  In particular, a pattern such
// as (?|(?<a>A)|(?<b)B), where the two capturing parentheses have the same
// number but different names, is not supported, and causes an error at compile
// time.  If it were allowed, it would not be possible to distinguish which
// parentheses matched, because both names map to capturing subpattern number
// 1.  To avoid this confusing situation, an error is given at compile time.
//
// 14) Perl recognizes comments in some places that PCRE2 does not, for
// example, between the ( and ? at the start of a subpattern.  If the /x
// modifier is set, Perl allows white space between ( and ? (though current
// Perls warn that this is deprecated) but PCRE2 never does, even if the
// PCRE2_EXTENDED option is set.
//
// 15) Perl, when in warning mode, gives warnings for character classes such as
// [A-\d] or [a-[:digit:]]. It then treats the hyphens as literals.  PCRE2 has
// no warning features, so it gives an error in these cases because they are
// almost certainly user mistakes.
//
// 16) In PCRE2, the upper/lower case character properties Lu and Ll are not
// affected when case-independent matching is specified.  For example, \p{Lu}
// always matches an upper case letter.
//
// 17) PCRE2 provides some extensions to the Perl regular expression
// facilities.  This list is with respect to Perl 5.10:
//
// (a) Although lookbehind assertions in PCRE2 must match fixed length strings,
// each alternative branch of a lookbehind assertion can match a different
// length of string.  Perl requires them all to have the same length.
//
// (b) If PCRE2_DOLLAR_ENDONLY is set and PCRE2_MULTILINE is not set, the $
// meta-character matches only at the very end of the string.
//
// (c) A backslash followed by a letter with no special meaning is faulted.
// (Perl can be made to issue a warning.)
//
// (d) If PCRE2_UNGREEDY is set, the greediness of the repetition quantifiers
// is inverted, that is, by default they are not greedy, but if followed by a
// question mark they are.
//
// (e) PCRE2_ANCHORED can be used at matching time to force a pattern to be
// tried only at the first matching position in the subject string.
//
// (f) The PCRE2_NOTBOL, PCRE2_NOTEOL, PCRE2_NOTEMPTY, PCRE2_NOTEMPTY_ATSTART,
// and PCRE2_NO_AUTO_CAPTURE options have no Perl equivalents.
//
// (g) The \R escape sequence can be restricted to match only CR, LF, or CRLF
// by the PCRE2_BSR_ANYCRLF option.
//
// (h) The callout facility is PCRE2-specific.
//
// (i) The partial matching facility is PCRE2-specific.
//
// (j) The alternative matching function (pcre2_dfa_match() matches in a
// different way and is not Perl-compatible.
//
// (k) PCRE2 recognizes some special sequences such as (*CR) at the start of a
// pattern that set overall options that cannot be changed within the pattern.
//
///Additional Copyright Notice
///- - - - - - - - - - - - - -
// Copyright (c) 1997-2015 University of Cambridge
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
//    * Neither the name of the University of Cambridge nor the names of any
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>        // 'bsl::pair'
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef _PCRE2_H
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#endif

namespace BloombergLP {

namespace bdlpcre {

                             // ===========
                             // class RegEx
                             // ===========

class RegEx {
    // This class provides a mechanism for compiling and matching regular
    // expressions.  A regular expression approximately compatible with Perl
    // 5.8 is compiled with the 'prepare' method.  Subsequently, strings are
    // matched against the compiled (prepared) pattern using the overloaded
    // 'match' methods.  Note that the underlying implementation uses the
    // open-source Perl Compatible Regular Expressions (PCRE2) library that was
    // developed at the University of Cambridge ('http://www.pcre.org/').

    // CLASS DATA
    static
    bsls::AtomicOperations::AtomicTypes::Int s_depthLimit;  // process-wide
                                                            // default maximum
                                                            // evaluation
                                                            // recursion depth

    // PRIVATE DATA
    int                    d_flags;          // prepare/match flags

    bsl::string            d_pattern;        // regular expression pattern

    pcre2_general_context *d_pcre2Context_p; // pcre2 general context

    pcre2_match_context   *d_matchContext_p; // pcre2 match context

    pcre2_code            *d_pcre2Code_p;    // PCRE2 compiled pattern

    int                    d_depthLimit;     // max evaluation recursion depth

    bslma::Allocator      *d_allocator_p;    // allocator to supply memory

  private:
    // NOT IMPLEMENTED
    RegEx(const RegEx&);
    RegEx& operator=(const RegEx&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(RegEx, bslma::UsesBslmaAllocator);

    // PUBLIC TYPES
    enum {
        k_FLAG_CASELESS      = PCRE2_CASELESS,  // case-insensitive matching

        k_FLAG_DOTMATCHESALL = PCRE2_DOTALL,    // dot metacharacter matches
                                                // all chars (including
                                                // newlines)

        k_FLAG_MULTILINE     = PCRE2_MULTILINE, // multi-line matching

        k_FLAG_UTF8          = PCRE2_UTF        // UTF-8 support
    };
        // This enumeration defines the flags that may be supplied to the
        // 'prepare' method to effect specific pattern matching behavior.

    // CLASS METHODS
    static int defaultDepthLimit();
        // Returns the process-wide default evaluation recursion depth limit.

    static int setDefaultDepthLimit(int depthLimit);
        // Set the process-wide default evaluation recursion depth limit to the
        // specified 'depthLimit'.  Returns the previous depth limit.

    // CREATORS
    RegEx(bslma::Allocator *basicAllocator = 0);                    // IMPLICIT
        // Create a regular-expression object in the "unprepared" state.
        // Optionally specify a 'basicAllocator' used to supply memory.  The
        // alignment strategy of the allocator must be "maximum" or "natural".
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    ~RegEx();
        // Destroy this regular-expression object.

    // MANIPULATORS
    void clear();
        // Free resources used by this regular-expression object and put this
        // object into the "unprepared" state.  This method has no effect if
        // this object is already in the "unprepared" state.

    int prepare(bsl::string *errorMessage,
                size_t      *errorOffset,
                const char  *pattern,
                int          flags = 0);
        // Prepare this regular-expression object with the specified 'pattern'
        // and the optionally specified 'flags'.  On success, put this object
        // into the "prepared" state and return 0, with no effect on the
        // specified 'errorMessage' and 'errorOffset'.  Otherwise, (1) put this
        // object into the "unprepared" state, (2) load 'errorMessage' (if
        // non-null) with a string describing the error detected, (3) load
        // 'errorOffset' (if non-null) with the offset in 'pattern' at which
        // the error was detected, and (4) return a non-zero value.  The
        // behavior is undefined unless 'flags' is the bit-wise inclusive-or of
        // 0 or more of the following values:
        //..
        //  k_FLAG_CASELESS
        //  k_FLAG_DOTMATCHESALL
        //  k_FLAG_MULTILINE
        //  k_FLAG_UTF8
        //..

    int setDepthLimit(int depthLimit);
        // Set the evaluation recursion depth limit for this regular-expression
        // object to the specified 'depthLimit'.  Return the previous depth
        // limit.

    // ACCESSORS
    int depthLimit() const;
        // Returns the evaluation recursion depth limit for this
        // regular-expression object.

    int flags() const;
        // Return the flags that were supplied to the most recent successful
        // call to the 'prepare' method of this regular-expression object.  The
        // behavior is undefined unless 'isPrepared() == true'.  Note that the
        // returned value will be the bit-wise inclusive-or of 0 or more of the
        // following values:
        //..
        //  k_FLAG_CASELESS
        //  k_FLAG_DOTMATCHESALL
        //  k_FLAG_MULTILINE
        //  k_FLAG_UTF8
        //..

    bool isPrepared() const;
        // Return 'true' if this regular-expression object is in the "prepared"
        // state, and 'false' otherwise.

    int match(const char *subject,
              size_t      subjectLength,
              size_t      subjectOffset = 0) const;
        // Match the specified 'subject', having the specified 'subjectLength',
        // against the pattern held by this regular-expression object
        // ('pattern()').  Begin matching at the optionally specified
        // 'subjectOffset' in 'subject'.  If 'subjectOffset' is not specified,
        // then begin matching from the start of 'subject'.  Return 0 on
        // success, 1 if the depth limit was exceeded, and another non-zero
        // value otherwise.  The behavior is undefined unless 'isPrepared() ==
        // true', '0 <= subjectLength', '0 <= subjectOffset', and
        // 'subjectOffset <= subjectLength'.  The behavior is also undefined if
        // 'pattern()' was prepared with 'k_FLAG_UTF8', but 'subject' is not
        // valid UTF-8.  Note that 'subject' need not be null-terminated and
        // may contain embedded null characters.  Also note that 'subject' may
        // be null if '0 == subjectLength' (denoting the empty string).

    int match(bsl::pair<size_t, size_t> *result,
              const char                *subject,
              size_t                     subjectLength,
              size_t                     subjectOffset = 0) const;
        // Match the specified 'subject', having the specified 'subjectLength',
        // against the pattern held by this regular-expression object
        // ('pattern()').  Begin matching at the optionally specified
        // 'subjectOffset' in 'subject'.  If 'subjectOffset' is not specified,
        // then begin matching from the start of 'subject'.  On success, load
        // the specified 'result' with the '(offset, length)' pair indicating
        // the leftmost match of 'pattern()', and return 0.  Otherwise, return
        // a non-zero value with no effect on 'result'.  The return value is 1
        // if the failure is caused by exceeding the depth limit.  The behavior
        // is undefined unless 'isPrepared() == true', '0 <= subjectLength', '0
        // <= subjectOffset', and 'subjectOffset <= subjectLength'.  The
        // behavior is also undefined if 'pattern()' was prepared with
        // 'k_FLAG_UTF8', but 'subject' is not valid UTF-8.  Note that
        // 'subject' need not be null-terminated and may contain embedded null
        // characters.  Also note that 'subject' may be null if '0 ==
        // subjectLength' (denoting the empty string).

    int match(bsl::vector<bsl::pair<size_t, size_t> > *result,
              const char                              *subject,
              size_t                                   subjectLength,
              size_t                                   subjectOffset = 0)
                                                                         const;
        // Match the specified 'subject', having the specified 'subjectLength',
        // against the pattern held by this regular-expression object
        // ('pattern()').  Begin matching at the optionally specified
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
        // 'result'.  The return value is 1 if the failure is caused by
        // exceeding the depth limit.  The behavior is undefined unless
        // 'isPrepared() == true', '0 <= subjectLength', '0 <= subjectOffset',
        // and 'subjectOffset <= subjectLength'.  The behavior is also
        // undefined if 'pattern()' was prepared with 'k_FLAG_UTF8', but
        // 'subject' is not valid UTF-8.  Note that 'subject' need not be
        // null-terminated and may contain embedded null characters.  Also note
        // that 'subject' may be null if '0 == subjectLength' (denoting the
        // empty string).  Also note that after a successful call, 'result'
        // will contain exactly 'numSubpatterns() + 1' elements.

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
//                              INLINE DEFINITIONS
// ============================================================================

                             // -----------
                             // class RegEx
                             // -----------

// CLASS METHODS
inline
int RegEx::defaultDepthLimit()
{
    return bsls::AtomicOperations::getIntRelaxed(&s_depthLimit);
}

inline
int RegEx::setDefaultDepthLimit(int depthLimit)
{
    int previous = defaultDepthLimit();

    bsls::AtomicOperations::setIntRelaxed(&s_depthLimit, depthLimit);

    return previous;
}

// CREATORS
inline
RegEx::~RegEx()
{
    clear();
    pcre2_match_context_free(d_matchContext_p);
    pcre2_general_context_free(d_pcre2Context_p);
}

// MANIPULATORS
inline
int RegEx::setDepthLimit(int depthLimit)
{
    int previous = d_depthLimit;

    d_depthLimit = depthLimit;

    pcre2_set_match_limit(d_matchContext_p, d_depthLimit);

    return previous;
}

// ACCESSORS
inline
int RegEx::depthLimit() const
{
    return d_depthLimit;
}

inline
int RegEx::flags() const
{
    return d_flags;
}

inline
bool RegEx::isPrepared() const
{
    return 0 != d_pcre2Code_p;
}

inline
const bsl::string& RegEx::pattern() const
{
    return d_pattern;
}

}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
