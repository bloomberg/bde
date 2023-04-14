// bdlpcre_regex.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLPCRE_REGEX
#define INCLUDED_BDLPCRE_REGEX

#include <bsls_ident.h>
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
// subject strings against a prepared expression and replacing the matching
// parts with the replacement string.  The regular expressions supported by
// this component correspond approximately with Perl 5.10.  See the appendix
// entitled "Perl Compatibility" below for more information.
//
// Upon construction, a 'bdlpcre::RegEx' object is initially not associated
// with a regular expression.  A regular expression pattern is compiled for use
// by the object using the 'prepare' method.  Subject strings may then be
// matched against the prepared pattern using the set of overloaded 'match'
// methods.
//
// The component provides the following groups of 'match' overloads (and
// similarly for 'matchRaw'):
//
//: 1 The first group of 'match' overloads simply returns 0 if a given subject
//:   string matches the prepared regular expression, and returns a non-zero
//:   value otherwise.
//:
//: 2 The second group of 'match' overloads returns the substring of the
//:   subject that was matched, either as a 'bsl::string_view', or as a
//:   'bsl::pair<size_t, size_t>' holding the (offset, length) pair.
//:
//: 3 The third group of 'match' overloads returns a vector of either
//:   'bsl::string_view' or 'bsl::pair<size_t, size_t>' holding the matched
//:   substrings.  The first element of the vector indicate the substring of
//:   the subject that matched the entire pattern.  Subsequent elements
//:   indicate the substrings of the subject that matched respective
//:   sub-patterns.
//
// The matched parts of subjects strings can be replaced with the replacement
// string using the set of overloaded 'replace' and 'replaceRaw' methods.
//
///"Prepared" State
///----------------
// A 'bdlpcre::RegEx' object must first be prepared with a valid regular
// expression before attempting to match subject strings or replace the matched
// parts.  We say that an instance of 'bdlpcre::RegEx' is in the "prepared"
// state if the object holds a valid regular expression, in which case calls to
// the overloaded 'match' or 'replace' methods of that instance are valid.
// Otherwise, the object is in the "unprepared" state.  Upon construction, an
// 'bdlpcre::RegEx' object is in the "unprepared" state.  A successful call to
// the 'prepare' method puts the object into the "prepared" state.  The 'clear'
// method, as well as an unsuccessful call to 'prepare', puts the object into
// the "unprepared" state.  The 'isPrepared' accessor may be used to determine
// whether an object is prepared.
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
// By default, a subject string supplied to 'match' or 'replace' is treated as
// consisting of a single line of characters (even if it actually contains '\n'
// characters).  The start-of-line meta-character '^' matches only at the
// beginning of the string, and the end-of-line meta-character '$' matches only
// at the end of the string (or before a terminating '\n', if present).  This
// matches the behavior of Perl.
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
// the regular expression pattern supplied to 'prepare', the subject strings
// subsequently supplied to 'match', 'matchRaw', 'replace', and 'replaceRaw' as
// well as the replacement string supplied to 'replace' and 'replaceRaw' are
// interpreted as strings of UTF-8 characters instead of strings of ASCII
// characters.  'match' and 'replace' return a non-zero value if 'pattern()'
// was prepared with 'k_FLAG_UTF8', but the subject or the replacement are not
// a valid UTF-8 string.  The behavior of 'matchRaw' is undefined if
// 'pattern()' was prepared with 'k_FLAG_UTF8', but the subject is not a valid
// UTF-8 string.  Note that JIT optimization (see below) is disabled for
// 'match' if 'pattern()' was prepared with 'k_FLAG_UTF8'.
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
///Allow Duplicate Named Groups (sub-patterns)
///- - - - - - - - - - - - - - - - - - - - - -
// If 'RegEx::k_FLAG_DUPNAMES' is included in the flags supplied to 'prepare',
// then sub-pattern names can be used more than once.  Alternatively this
// feature can be turned on within a pattern by a '(?J)' option setting
// (see https://www.pcre.org/current/doc/html/pcre2syntax.html#SEC16).  The
// 'subpatternIndex(name)' call will fail if 'name' is used more than once - in
// that case, the 'namedSubpatterns()' call should be used.
// 'namedSubpatterns()' returns a set of (name, index) pairs used in the
// pattern.
//
/// Creating a New String with Replacement
///---------------------------------------
// A new string can be created by applying the regular expression pattern to
// the subject string in which the matching parts are replaced with the
// replacement string supplied to the 'replace' and 'replaceRaw' methods.
//
///Group Insertion Forms
///- - - - - - - - - - -
// By default, a dollar character ('$') is an escape character that can specify
// the insertion of characters from capture groups and names from '(*MARK)' or
// other control verbs in the pattern (see
// https://perldoc.perl.org/perlre#Special-Backtracking-Control-Verbs for
// details).  The following forms are always recognized:
//..
//  $$                  insert a dollar character
//  $<n> or ${<n>}      insert the contents of group <n>
//  $*MARK or ${*MARK}  insert a control verb name
//..
// Either a group number or a group name can be given for '<n>'.  Curly braces
// are required only if the following character would be interpreted as part of
// the number or name.  The number may be zero to include the entire matched
// string.  For example, if the pattern 'a(b)c' is matched with '=abc=' and the
// replacement string '+$1$0$1+', the result is '=+babcb+='.
//
///Replacement Flags
///- - - - - - - - -
// A set of flags may be optionally supplied to the 'replace' and 'replaceRaw'
// method to affect specific substitution behavior.  The flags recognized by
// 'replace' and 'replaceRaw' are defined in an enumeration declared within the
// 'bdlpcre::RegEx'.  The flags are passed as a bitwise combination of OR bits
// in the 'options' argument to 'replace' and 'replaceRaw' (e.g.,
// 'k_REPLACE_GLOBAL | k_REPLACE_LITERAL).  The flags reflect
// 'PCRE_SUBSTITUTE_*' flags and are propagated to the underlying PCRE2 library
// substitute function.  See
// {https://www.pcre.org/current/doc/html/pcre2api.html#SEC36} for details. The
// following describes these flags and their effects.
//
///Global Replacement
///  -  -  -  -  -  -
// The default action of 'replace' and 'replaceRaw'  is to perform just one
// replacement if the pattern matches.  The 'RegEx::k_REPLACE_GLOBAL' flag
// requests multiple replacements in the subject string.
//
///The Replacement String is Literal
///- - - - - - - - - - - - - - - - -
// If 'RegEx::k_REPLACE_LITERAL' is set, the replacement string is not
// interpreted in any way.
//
///Extended Replacement Processing
///-  -  -  -  -  -  -  -  -  -  -
// If 'RegEx::k_REPLACE_EXTENDED' is set, extra processing is applied to the
// replacement string.  Without this option, only the dollar character ('$') is
// special, and only the group insertion forms listed above (see
// {Group Insertion Forms}) are valid.  When this flag is set, two things
// change:
//
//: o Firstly, backslash in a replacement string is interpreted as an escape
//:   character.  The usual forms such as '\n' or '\x{ddd}' can be used to
//:   specify particular character codes, and backslash followed by any
//:   non-alphanumeric character quotes that character.  Extended quoting can
//:   be coded using '\Q...\E', exactly as in the pattern string.
//:
//: o The second effect is to add more flexibility to capture group
//:   substitution.  The syntax is similar to that used by Bash:
//:..
//:   ${<n>:-<string>}
//:   ${<n>:+<string1>:<string2>}
//:..
//:   As before, '<n>' may be a group number or a name.  The first form
//:   specifies a default value.  If group '<n>' is set, its value is inserted;
//:   if not, '<string>' is expanded and the result inserted.  The second form
//:   specifies strings that are expanded and inserted when group '<n>' is set
//:   or unset, respectively.  The first form is just a convenient shorthand
//:   for '${<n>:+${<n>}:<string>}'.
//
///Treat Unknown Group As Unset
///-  -  -  -  -  -  -  -  -  -
// The 'RegEx::k_REPLACE_UNKNOWN_UNSET' causes references to capture groups
// that do not appear in the pattern to be treated as unset groups.
//
///Insert An Empty String For Unset Group
/// -  -  -  -  -  -  -  -  -  -  -  -  -
// The 'RegEx::k_REPLACE_UNSET_EMPTY' causes unset capture groups (including
// unknown groups when 'RegEx::k_REPLACE_UNKNOWN_UNSET' is set) to be treated
// as empty strings when inserted as described in {Group Insertion Forms}.  If
// this option is not set, an attempt to insert an unset group causes 'replace'
// and 'replaceRaw' to return an error.  This option does not influence the
// extended substitution syntax described in {Extended Replacement Processing}.
//
///JIT Compiling Optimization
///--------------------------
// Just-in-time compiling is a heavyweight optimization that can greatly speed
// up pattern matching on supported platforms.  However, it comes at the cost
// of extra processing before the match is performed, so it is of most benefit
// when the same pattern is going to be matched many times.  This does not
// necessarily mean many calls of a matching function; if the pattern is not
// anchored, matching attempts may take place many times at various positions
// in the subject, even for a single call.  Therefore, if the subject string is
// very long, it may still pay to use JIT even for one-off matches.
//
// If 'RegEx::k_FLAG_JIT' is included in the flags supplied to 'prepare', then
// all following matches performed by 'matchRaw' will be JIT optimized.
// Matches performed by 'match' will also be JIT optimized provided that
// 'RegEx::k_FLAG_UTF8' was not supplied to 'prepare' (since UTF-8 string
// validity checking is not done during JIT compilation).  To disable JIT
// optimization for all matches, prepare the regular expression again omitting
// the 'k_FLAG_JIT' flag.
//
// JIT is supported on the following platforms:
//..
//  ARM 32-bit (v5, v7, and Thumb2)
//  ARM 64-bit
//  Intel x86 32-bit and 64-bit
//  MIPS 32-bit and 64-bit
//  Power PC 32-bit and 64-bit
//  SPARC 32-bit
//..
//
// The tables below demonstrate the benefit of the 'match' method with JIT
// optimizations, as well as the increased cost for 'prepare' when enabling JIT
// optimizations:
//..
//  Legend
//  ------
//  'SIMPLE_PATTERN':
//      Pattern - X(abc)*Z
//      Subject - XXXabcabcZZZ
//
//  'EMAIL_PATTERN':
//      Pattern - [A-Za-z0-9._-]+@[[A-Za-z0-9.-]+
//      Subject - john.dow@bloomberg.net
//
//  'IP_ADDRESS_PATTERN':
//      Pattern - (?:[0-9]{1,3}\.){3}[0-9]{1,3}
//      Subject - 255.255.255.255
//
//   Each pattern/subject returns 1 match.
//..
// In this first table, for each pattern, 'prepare' was called once, and match
// was called 100000 times (measurements are in seconds):
//..
//   Table 1: Performance Improvement for 'match' using k_JIT_FLAG
//  +--------------------+---------------------+---------------------+
//  | Pattern            | 'match' without-JIT |  'match' using-JIT  |
//  +====================+=====================+=====================+
//  | SIMPLE_PATTERN     |    0.0559 (~5.1x)   |        0.0108       |
//  +--------------------+---------------------+---------------------+
//  | EMAIL_PATTERN      |    0.0222 (~2.6x)   |        0.0086       |
//  +--------------------+---------------------+---------------------+
//  | IP_ADDRESS_PATTERN |    0.0331 (~5.3x)   |        0.0062       |
//  +--------------------+---------------------+---------------------+
//..
// In this second table, for each pattern, we measured 10000 iterations, where
// 'prepare' was called once, and 'match' was called once (measurements are in
// seconds):
//..
//   Table 2: Performance Cost for 'prepare' using k_JIT_FLAG
//  +--------------------+-----------------------+-----------------------+
//  | Pattern            | 'prepare' without-JIT |  'prepare' using-JIT  |
//  +====================+=======================+=======================+
//  | SIMPLE_PATTERN     |         0.2514        |     2.1426 (~8.5x)   |
//  +--------------------+-----------------------+-----------------------+
//  | EMAIL_PATTERN      |         0.3386        |     2.5758 (~7.6x)    |
//  +--------------------+-----------------------+-----------------------+
//  | IP_ADDRESS_PATTERN |         0.3016        |     2.4433 (~8.1x)    |
//  +--------------------+-----------------------+-----------------------+
//..
// Note that the tests were run on Linux / Intel Xeon CPU (3.47GHz, 64-bit),
// compiled with gcc-4.8.2 in optimized mode.
//
///Thread Safety
///-------------
// 'bdlpcre::RegEx' is *const* *thread-safe*, meaning that accessors may be
// invoked concurrently from different threads, but it is not safe to access or
// modify a 'bdlpcre::RegEx' in one thread while another thread modifies the
// same object.  Specifically, the 'match' method can be called from multiple
// threads after the pattern has been prepared.
//
// Note that 'bdlpcre::RegEx' incurs some overhead in order to provide
// thread-safe pattern matching functionality.  To perform the pattern match,
// the underlying PCRE2 library requires a set of buffers that cannot be shared
// between threads.
//
// The table below demonstrate the difference of invoking the 'match' method
// from main (thread that invokes 'prepare') and other threads:
//..
//   Table 3: Performance cost for 'match' in multi-threaded application
//  +--------------------+-----------------------+----------------------------+
//  | Pattern            | 'match' (main thread) |  'match' (other thread(s)) |
//  +====================+=======================+============================+
//  | SIMPLE_PATTERN     |    0.0549 (~1.4x)     |           0.0759           |
//  +--------------------+-----------------------+----------------------------+
//  | EMAIL_PATTERN      |    0.0259 (~1.8x)     |           0.0464           |
//  +--------------------+-----------------------+----------------------------+
//  | IP_ADDRESS_PATTERN |    0.0377 (~1.5x)     |           0.0560           |
//  +--------------------+-----------------------+----------------------------+
//..
// Note that JIT stack is functionally part of the match context. Using large
// JIT stack can incur additional performance penalty in the multi-threaded
// applications.
//
///Note on Memory Allocation Exceptions
///------------------------------------
// PCRE2 library supports memory allocation/deallocation functions supplied by
// the client.  'bdlpcre_regex' provides wrappers around 'bslma' allocators
// that are called from the context of the PCRE2 library (C linkage).  Any
// exceptions thrown during memory allocation are caught by the wrapper
// functions and are not propagated to the PCRE2 library.
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
//      bsl::vector<bsl::pair<size_t, size_t> > matchVector;
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
//      const bsl::pair<size_t, size_t> capturedSubject =
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
///Appendix: Perl Compatibility
/// - - - - - - - - - - - - - -
// This section describes the differences in the ways that PCRE2 and Perl
// handle regular expressions.  The differences described here are with respect
// to Perl versions 5.10 and above.
//
// 1) PCRE2 has only a subset of Perl's Unicode support.
//
// 2) PCRE2 allows repeat quantifiers only on parenthesized assertions, but
// they do not mean what you might think.  For example, '(?!a){3}' does not
// assert that the next three characters are not '"a"'.  It just asserts that
// the next character is not '"a"' three times (in principle: PCRE2 optimizes
// this to run the assertion just once).  Perl allows repeat quantifiers on
// other assertions such as '\b', but these do not seem to have any use.
//
// 3) Capturing subpatterns that occur inside negative lookahead assertions are
// counted, but their entries in the offsets vector are never set.  Perl
// sometimes (but not always) sets its numerical variables from inside negative
// assertions.
//
// 4) The following Perl escape sequences are not supported: '\l', '\u', '\L',
// '\U', and '\N' when followed by a character name or Unicode value.  ('\N' on
// its own, matching a non-newline character, is supported.)  In fact these are
// implemented by Perl's general string-handling and are not part of its
// pattern matching engine.  If any of these are encountered by PCRE2, an error
// is generated by default.
//
// 5) The Perl escape sequences '\p,' '\P,' and '\X' are supported only if
// PCRE2 is built with Unicode support.  The properties that can be tested with
// '\p' and '\P' are limited to the general category properties such as 'Lu'
// and 'Nd', script names such as Greek or Han, and the derived properties
// 'Any' and 'L&'.  PCRE2 does support the 'Cs' (surrogate) property, which
// Perl does not; the Perl documentation says "Because Perl hides the need for
// the user to understand the internal representation of Unicode characters,
// there is no need to implement the somewhat messy concept of surrogates."
//
// 6) PCRE2 does support the '\Q...\E' escape for quoting substrings.
// Characters in between are treated as literals.  This is slightly different
// from Perl in that '$' and '@' are also handled as literals inside the
// quotes.  In Perl, they cause variable interpolation (but of course PCRE2
// does not have variables).  Note the following examples:
//..
// Pattern            PCRE2 matches  Perl matches
// ----------------   -------------  ------------------------------------
// \Qabc$xyz\E        abc$xyz        abc followed by the contents of $xyz
// \Qabc\$xyz\E       abc\$xyz       abc\$xyz
// \Qabc\E\$\Qxyz\E   abc$xyz        abc$xyz
//..
// The '\Q...\E' sequence is recognized both inside and outside character
// classes.
//
// 7) PCRE2 does not support the '(?{code})' and '(??{code})' constructions.
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
// This is not always the case in Perl.  In particular, if '(*THEN)' is present
// in a group that is called as a subroutine, its action is limited to that
// group, even if the group does not contain any '|' characters.  Note that
// such subpatterns are processed as anchored at the point where they are
// tested.
//
// 10) If a pattern contains more than one backtracking control verb, the first
// one that is backtracked onto acts.  For example, in the pattern
// 'A(*COMMIT)B(*PRUNE)C' a failure in 'B' triggers '(*COMMIT),' but a failure
// in 'C' triggers '(*PRUNE)'.  Perl's behaviour is more complex; in many cases
// it is the same as PCRE2, but there are examples where it differs.
//
// 11) Most backtracking verbs in assertions have their normal actions.  They
// are not confined to the assertion.
//
// 12) There are some differences that are concerned with the settings of
// captured strings when part of a pattern is repeated.  For example, matching
// '"aba"' against the pattern '/^(a(b)?)+$/' in Perl leaves '$2' unset, but in
// PCRE2 it is set to '"b"'.
//
// 13) PCRE2's handling of duplicate subpattern numbers and duplicate
// subpattern names is not as general as Perl's.  This is a consequence of the
// fact the PCRE2 works internally just with numbers, using an external table
// to translate between numbers and names.  In particular, a pattern such as
// '(?|(?<a>A)|(?<b)B)', where the two capturing parentheses have the same
// number but different names, is not supported, and causes an error at compile
// time.  If it were allowed, it would not be possible to distinguish which
// parentheses matched, because both names map to capturing subpattern number
// 1.  To avoid this confusing situation, an error is given at compile time.
//
// 14) Perl recognizes comments in some places that PCRE2 does not, for
// example, between the '(' and '?' at the start of a subpattern.  If the '/x'
// modifier is set, Perl allows white space between '(' and '?' (though current
// Perls warn that this is deprecated) but PCRE2 never does, even if the
// 'PCRE2_EXTENDED' option is set.
//
// 15) Perl, when in warning mode, gives warnings for character classes such as
// '[A-\d]' or '[a-[:digit:]]'.  It then treats the hyphens as literals.  PCRE2
// has no warning features, so it gives an error in these cases because they
// are almost certainly user mistakes.
//
// 16) In PCRE2, the upper/lower case character properties 'Lu' and 'Ll' are
// not affected when case-independent matching is specified.  For example,
// '\p{Lu}' always matches an upper case letter.
//
// 17) PCRE2 provides some extensions to the Perl regular expression
// facilities.  This list is with respect to Perl 5.10:
//
// (a) Although lookbehind assertions in PCRE2 must match fixed length strings,
// each alternative branch of a lookbehind assertion can match a different
// length of string.  Perl requires them all to have the same length.
//
// (b) If 'PCRE2_DOLLAR_ENDONLY' is set and 'PCRE2_MULTILINE' is not set, the
// '$' meta-character matches only at the very end of the string.
//
// (c) A backslash followed by a letter with no special meaning is faulted.
// (Perl can be made to issue a warning.)
//
// (d) If 'PCRE2_UNGREEDY' is set, the greediness of the repetition quantifiers
// is inverted, that is, by default they are not greedy, but if followed by a
// question mark they are.
//
// (e) 'PCRE2_ANCHORED' can be used at matching time to force a pattern to be
// tried only at the first matching position in the subject string.
//
// (f) The 'PCRE2_NOTBOL', 'PCRE2_NOTEOL', 'PCRE2_NOTEMPTY',
// 'PCRE2_NOTEMPTY_ATSTART', and 'PCRE2_NO_AUTO_CAPTURE' options have no Perl
// equivalents.
//
// (g) The '\R' escape sequence can be restricted to match only 'CR,' 'LF,' or
// 'CRLF' by the 'PCRE2_BSR_ANYCRLF' option.
//
// (h) The callout facility is PCRE2-specific.
//
// (i) The partial matching facility is PCRE2-specific.
//
// (j) The alternative matching function ('pcre2_dfa_match()' matches in a
// different way and is not Perl-compatible.
//
// (k) PCRE2 recognizes some special sequences such as '(*CR)' at the start of
// a pattern that set overall options that cannot be changed within the
// pattern.
//
///Additional Copyright Notice
///- - - - - - - - - - - - - -
//..
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
//
// Copyright (c) 1997-2015 University of Cambridge
//..

#include <bdlscm_version.h>

#include <bslma_allocator.h>
#include <bslma_managedptr.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_enableif.h>
#include <bslmf_issame.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_atomicoperations.h>
#include <bsls_libraryfeatures.h>

#include <bsl_cstddef.h>
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_utility.h>        // 'bsl::pair'
#include <bsl_vector.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
#include <memory_resource>
#endif
#include <string>
#include <vector>

#ifndef _PCRE2_H
#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC
#include <pcre2/pcre2.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bdlpcre {

class RegEx_MatchContext;

                             // ===========
                             // class RegEx
                             // ===========

class RegEx {
    // This class provides a mechanism for compiling and matching regular
    // expressions.  A regular expression approximately compatible with Perl
    // 5.10 is compiled with the 'prepare' method.  Subsequently, strings are
    // matched against the compiled (prepared) pattern using the overloaded
    // 'match' and 'matchRaw' methods.  Note that the underlying implementation
    // uses the open-source Perl Compatible Regular Expressions (PCRE2) library
    // that was developed at the University of Cambridge
    // ('http://www.pcre.org/').

    // CLASS DATA
    static
    bsls::AtomicOperations::AtomicTypes::Int s_depthLimit;  // process-wide
                                                            // default maximum
                                                            // evaluation
                                                            // recursion depth

    // PRIVATE DATA
    int                    d_flags;             // prepare/match flags

    bsl::string            d_pattern;           // regular expression pattern

    pcre2_general_context *d_pcre2Context_p;    // PCRE2 general context

    pcre2_compile_context *d_compileContext_p;  // PCRE2 compile context

    pcre2_code            *d_patternCode_p;     // PCRE2 compiled pattern

    int                    d_depthLimit;        // evaluation recursion depth

    size_t                 d_jitStackSize;      // PCRE JIT stack size

    bslma::ManagedPtr<RegEx_MatchContext>
                           d_matchContext;      // match context helper

    bslma::Allocator      *d_allocator_p;       // allocator to supply memory

  private:
    // NOT IMPLEMENTED
    RegEx(const RegEx&);
    RegEx& operator=(const RegEx&);

    // PRIVATE MANIPULATORS
    int prepareImp(char        *errorBuffer,
                   size_t       errorBufferLength,
                   size_t      *errorOffset,
                   const char  *pattern,
                   int          flags,
                   size_t       jitStackSize);
        // Prepare this regular-expression object with the specified 'pattern',
        // 'flags', and 'jitStackSize' that indicates the size of the allocated
        // JIT stack to be used for 'pattern'.  On success, put this object
        // into the "prepared" state and return 0, with no effect on the
        // specified 'errorBuffer' and 'errorOffset'.  Otherwise, (1) put this
        // object into the "unprepared" state, (2) load 'errorBuffer' with a
        // message describing the error detected truncated to the specified
        // 'errorBufferLength' (including a null terminator), (3) load
        // 'errorOffset' with the offset in 'pattern' at which the error was
        // detected, and (4) return a non-zero value.  The behavior is
        // undefined unless 'flags' is the bit-wise inclusive-or of 0 or more
        // of the following values:
        //..
        //  k_FLAG_CASELESS
        //  k_FLAG_DOTMATCHESALL
        //  k_FLAG_MULTILINE
        //  k_FLAG_UTF8
        //  k_FLAG_JIT
        //  k_FLAG_DUPNAMES
        //..
        // Note that the flag 'k_FLAG_JIT' is ignored if 'isJitAvailable()' is
        // 'false'.

    // PRIVATE ACCESSORS
    template <class RESULT_EXTRACTOR>
    int matchImp(const RESULT_EXTRACTOR&  extractor,
                 const char              *subject,
                 size_t                   subjectLength,
                 size_t                   subjectOffset,
                 bool                     skipUTF8Validation) const;
        // Match the specified 'subject', having the specified 'subjectLength',
        // against the pattern held by this regular-expression object
        // ('pattern()').  'subject' need not be null-terminated and may
        // contain embedded null characters.  The specified
        // 'skipUTF8Validation' flag indicates whether UTF-8 string validity
        // checking is skipped.  Begin matching at the specified
        // 'subjectOffset' in 'subject'.  Return:
        //
        //: o 0 on success and invoke the specified 'extractor' to extract the
        //:   result of the match
        //:
        //: o 1 if the 'depthLimit()' was exceeded
        //:
        //: o 2 if memory available for the JIT stack is not large enough
        //:   (applicable only if 'pattern()' was prepared with 'k_FLAG_JIT')
        //:
        //: o another non-zero value, otherwise
        //
        // The behavior is undefined unless 'true == isPrepared()',
        // 'subject || 0 == subjectLength', 'subjectOffset <= subjectLength',
        // and 'subject' is valid UTF-8 if 'pattern()' was prepared with
        // 'k_FLAG_UTF8' but 'false == skipUTF8Validation'.

    template <class Vector>
    void namedSubpatternsImp(Vector *result) const;
        // 'namedSubpatterns()' implementation.

    template <class STRING>
    int replaceImp(STRING                  *result,
                   int                     *errorOffset,
                   const bsl::string_view&  subject,
                   const bsl::string_view&  replacement,
                   size_t                   options,
                   bool                     skipUTF8Validation) const;
        // Replace parts of the specified 'subject' that are matched with the
        // specified 'replacement'.  The specified bit mask of 'options' flags
        // is used to configure the behavior of the replacement.  'options'
        // should contain a bit-wise OR of the 'k_REPLACE_*' constants defined
        // by this class, which indicate additional configuration parameters
        // for the replacement.  If 'options' has 'k_REPLACE_GLOBAL' flag then
        // this function iterates over 'subject', replacing every matching
        // substring.  If 'k_REPLACE_GLOBAL' flag is not set, only the first
        // matching substring is replaced.  The specified 'skipUTF8Validation'
        // flag indicates whether UTF-8 'replacment' validity checking is
        // skipped.  Return the number of substitutions that were carried out,
        // and load the specified 'result' with the result of the replacement.
        // Otherwise, if an error occurs, return a negative value.  If that
        // error is a syntax error in 'replacement', load the specified
        // 'errorOffset' (if non-null) with the offset in'replacement' where
        // the error was detected; for other errors, such as invalid 'subject'
        // or 'replacement' UTF-8 string, load 'errorOffset' with a negative
        // value.  The behavior is undefined unless 'true == isPrepared()'.
        // Note that if the size of 'result' is too small to fit the resultant
        // string then this method computes the size of 'result' and adjusts it
        // to the size that is needed.  To avoid automatic calculation and
        // adjustment which may introduce a performace penalty, it is
        // recommended that the size of 'result' has enough room to fit the
        // zero-terminating character.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(RegEx, bslma::UsesBslmaAllocator);

    // CONSTANTS
    enum {
        // This enumeration defines the flags that may be supplied to 'prepare'
        // to affect specific pattern matching behavior.

        k_FLAG_CASELESS      = 1 << 0,  // case-insensitive matching

        k_FLAG_DOTMATCHESALL = 1 << 1,  // dot metacharacter matches all chars
                                        // (including newlines)

        k_FLAG_MULTILINE     = 1 << 2,  // multi-line matching

        k_FLAG_UTF8          = 1 << 3,  // UTF-8 support

        k_FLAG_JIT           = 1 << 4,  // just-in-time compiling optimization
                                        // requested

        k_FLAG_DUPNAMES      = 1 << 5   // allow duplicate named groups
                                        // (sub-patterns)
    };

    enum {
        // This enumeration defines the flags that may be supplied to 'replace'
        // to affect specific replacement behavior.

        k_REPLACE_LITERAL       = 1 << 0,  // the replacement string is literal

        k_REPLACE_GLOBAL        = 1 << 1,  // replace all occurrences in the
                                           // subject

        k_REPLACE_EXTENDED      = 1 << 2,  // do extended replacement
                                           // processing

        k_REPLACE_UNKNOWN_UNSET = 1 << 3,  // treat unknown group as unset

        k_REPLACE_UNSET_EMPTY   = 1 << 4   // simple unset insert = empty
                                           // string
    };

    static const size_t k_INVALID_OFFSET;
        // Value used to denote an invalid offset for match methods returning
        // pairs.

    // CLASS METHODS
    static int defaultDepthLimit();
        // Return the process-wide default evaluation recursion depth limit.

    static bool isJitAvailable();
        // Return 'true' if just-in-time compiling optimization is supported by
        // current hardware platform and 'false' otherwise.  Note that JIT
        // support is limited to the following hardware platforms:
        //..
        //  ARM 32-bit (v5, v7, and Thumb2)
        //  ARM 64-bit
        //  Intel x86 32-bit and 64-bit
        //  MIPS 32-bit and 64-bit
        //  Power PC 32-bit and 64-bit
        //  SPARC 32-bit
        //..

    static int setDefaultDepthLimit(int depthLimit);
        // Set the process-wide default evaluation recursion depth limit to the
        // specified 'depthLimit'.  Return the previous depth limit.

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

    int prepare(bsl::nullptr_t         errorMessage,
                size_t                *errorOffset,
                const char            *pattern,
                int                    flags = 0,
                size_t                 jitStackSize = 0);

    template <class STRING>
    typename bsl::enable_if<   bsl::is_same<STRING, bsl::string>::value
                            || bsl::is_same<STRING, std::string>::value
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                            || bsl::is_same<STRING, std::pmr::string>::value
#endif
                            ,  int>::type
    prepare(STRING                    *errorMessage,
            size_t                    *errorOffset,
            const char                *pattern,
            int                        flags = 0,
            size_t                     jitStackSize = 0);
        // Prepare this regular-expression object with the specified 'pattern'
        // and the optionally specified 'flags'.  'flags', if supplied, should
        // contain a bit-wise or of the 'k_FLAG_*' constants defined by this
        // class, which indicate additional configuration parameters for the
        // regular expression.  Optionally specify 'jitStackSize'.  If 'flags'
        // has the 'k_FLAG_JIT' flag set, 'jitStackSize' indicates the size of
        // the allocated JIT stack to be used for this pattern.  If 'flags'
        // has the 'k_FLAG_JIT' bit set and 'jitStackSize' is 0 (or not
        // supplied), no memory will be allocated for the JIT stack and the
        // program stack will be used as the JIT stack.  If 'flags' does not
        // have 'k_FLAG_JIT' set, or 'isJitAvailable()' is 'false', the
        // 'jitStackSize' parameter, if supplied, is ignored.  On success, put
        // this object into the "prepared" state and return 0, with no effect
        // on the specified 'errorMessage' and 'errorOffset'.  Otherwise, (1)
        // put this object into the "unprepared" state, (2) load 'errorMessage'
        // (if non-null) with a string describing the error detected, (3) load
        // 'errorOffset' (if non-null) with the offset in 'pattern' at which
        // the error was detected, and (4) return a non-zero value.  The
        // behavior is undefined unless 'flags' is the bit-wise inclusive-or of
        // 0 or more of the following values:
        //..
        //  k_FLAG_CASELESS
        //  k_FLAG_DOTMATCHESALL
        //  k_FLAG_MULTILINE
        //  k_FLAG_UTF8
        //  k_FLAG_JIT
        //  k_FLAG_DUPNAMES
        //..
        // Note that the flag 'k_FLAG_JIT' is ignored if 'isJitAvailable()' is
        // 'false'.

    int setDepthLimit(int depthLimit);
        // Set the evaluation recursion depth limit for this regular-expression
        // object to the specified 'depthLimit'.  Return the previous depth
        // limit.

    // ACCESSORS
    int depthLimit() const;
        // Return the evaluation recursion depth limit for this
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
        //  k_FLAG_JIT
        //  k_FLAG_DUPNAMES
        //..
        // Also note that 'k_FLAG_JIT' is ignored, but still returned by this
        // method, if 'isJitAvailable()' is 'false'.

    bool isPrepared() const;
        // Return 'true' if this regular-expression object is in the "prepared"
        // state, and 'false' otherwise.

    size_t jitStackSize() const;
        // Return the size of the dynamically allocated JIT stack if it has
        // been specified explicitly with the 'prepare' method.  Return 0 if a
        // zero 'jitStackSize' value was passed to the 'prepare' method (or not
        // supplied at all) or if 'isPrepared()' is 'false'.

    int match(const bsl::string_view& subject,
              size_t                  subjectOffset = 0) const;
        // Match the specified 'subject' against 'pattern()'.  Begin matching
        // at the optionally specified 'subjectOffset' in 'subject'.  If
        // 'subjectOffset' is not specified, matching begins at the start of
        // 'subject'.  UTF-8 validity checking is performed on 'subject' if
        // 'pattern()' was prepared with 'k_FLAG_UTF8'.  Return:
        //
        //: o 0 on success
        //:
        //: o 1 if 'depthLimit()' was exceeded
        //:
        //: o 2 if memory available for the JIT stack is not large enough
        //:     (applicable only if 'pattern()' was prepared with 'k_FLAG_JIT')
        //:
        //: o another non-zero value, for example, if 'pattern()' was prepared
        //:   with 'k_FLAG_UTF8', but 'subject' is not valid UTF-8
        //
        // The behavior is undefined unless 'true == isPrepared()' and
        // 'subjectOffset <= subject.length()'.  Note that JIT optimization is
        // disabled if 'pattern()' was prepared with 'k_FLAG_UTF8'; use
        // 'matchRaw' if JIT is preferred and UTF-8 validation of 'subject' is
        // not required.

    int match(const char *subject,
              size_t      subjectLength,
              size_t      subjectOffset = 0) const;
        // Match the specified 'subject' having the specified 'subjectLength'
        // against 'pattern()'.  Begin matching at the optionally specified
        // 'subjectOffset' in 'subject'.  If 'subjectOffset' is not specified,
        // matching begins at the start of 'subject'.  'subject' may contain
        // embedded null characters.  UTF-8 validity checking is performed on
        // 'subject' if 'pattern()' was prepared with 'k_FLAG_UTF8'.  Return:
        //
        //: o 0 on success
        //:
        //: o 1 if 'depthLimit()' was exceeded
        //:
        //: o 2 if memory available for the JIT stack is not large enough
        //:     (applicable only if 'pattern()' was prepared with 'k_FLAG_JIT')
        //:
        //: o another non-zero value, for example, if 'pattern()' was prepared
        //:   with 'k_FLAG_UTF8', but 'subject' is not valid UTF-8
        //
        // The behavior is undefined unless 'true == isPrepared()',
        // 'subject || 0 == subjectLength', and
        // 'subjectOffset <= subjectLength'.  Note that JIT optimization is
        // disabled if 'pattern()' was prepared with 'k_FLAG_UTF8'; use
        // 'matchRaw' if JIT is preferred and UTF-8 validation of 'subject' is
        // not required.

    int match(bsl::pair<size_t, size_t> *result,
              const char                *subject,
              size_t                     subjectLength,
              size_t                     subjectOffset = 0) const;
    int match(bsl::string_view          *result,
              const char                *subject,
              size_t                     subjectLength,
              size_t                     subjectOffset = 0) const;
        // Match the specified 'subject' having the specified 'subjectLength'
        // against 'pattern()'.  Begin matching at the optionally specified
        // 'subjectOffset' in 'subject'.  If 'subjectOffset' is not specified,
        // matching begins at the start of 'subject'.  'subject' may contain
        // embedded null characters.  UTF-8 validity checking is performed on
        // 'subject' if 'pattern()' was prepared with 'k_FLAG_UTF8'.  Return:
        //
        //: o 0 on success and load the specified 'result' with, respectively,
        //:   a '(offset, length)' pair or a 'bsl::string_view' indicating the
        //:   leftmost match of 'pattern()'
        //:
        //: o 1 if 'depthLimit()' was exceeded
        //:
        //: o 2 if memory available for the JIT stack is not large enough
        //:     (applicable only if 'pattern()' was prepared with 'k_FLAG_JIT')
        //:
        //: o another non-zero value, for example, if 'pattern()' was prepared
        //:   with 'k_FLAG_UTF8', but 'subject' is not valid UTF-8
        //
        // 'result' is unchanged if a non-zero value is returned.  The behavior
        // is undefined unless 'true == isPrepared()',
        // 'subject || 0 == subjectLength', and
        // 'subjectOffset <= subjectLength'.  Note that JIT optimization is
        // disabled if 'pattern()' was prepared with 'k_FLAG_UTF8'; use
        // 'matchRaw' if JIT is preferred and UTF-8 validation of 'subject' is
        // not required.

    int match(bsl::string_view        *result,
              const bsl::string_view&  subject,
              size_t                   subjectOffset = 0) const;
        // Match the specified 'subject' against 'pattern()'.  Begin matching
        // at the optionally specified 'subjectOffset' in 'subject'.  If
        // 'subjectOffset' is not specified, matching begins at the start of
        // 'subject'.  UTF-8 validity checking is performed on 'subject' if
        // 'pattern()' was prepared with 'k_FLAG_UTF8'.  Return:
        //
        //: o 0 on success and load the specified 'result' with a
        //:   'bsl::string_view' indicating the leftmost match of 'pattern()'
        //:
        //: o 1 if 'depthLimit()' was exceeded
        //:
        //: o 2 if memory available for the JIT stack is not large enough
        //:     (applicable only if 'pattern()' was prepared with 'k_FLAG_JIT')
        //:
        //: o another non-zero value, for example, if 'pattern()' was prepared
        //:   with 'k_FLAG_UTF8', but 'subject' is not valid UTF-8
        //
        // 'result' is unchanged if a non-zero value is returned.  The behavior
        // is undefined unless 'true == isPrepared()' and
        // 'subjectOffset <= subject.length()'.  Note that JIT optimization is
        // disabled if 'pattern()' was prepared with 'k_FLAG_UTF8'; use
        // 'matchRaw' if JIT is preferred and UTF-8 validation of 'subject' is
        // not required.

    int match(bsl::vector<bsl::pair<size_t, size_t> > *result,
              const char                              *subject,
              size_t                                   subjectLength,
              size_t                                   subjectOffset = 0)
                                                                         const;
    int match(bsl::vector<bslstl::StringRef>          *result,
              const char                              *subject,
              size_t                                   subjectLength,
              size_t                                   subjectOffset = 0)
                                                                         const;
        // Match the specified 'subject' having the specified 'subjectLength'
        // against 'pattern()'.  Begin matching at the optionally specified
        // 'subjectOffset' in 'subject'.  If 'subjectOffset' is not specified,
        // matching begins at the start of 'subject'.  'subject' may contain
        // embedded null characters.  UTF-8 validity checking is performed on
        // 'subject' if 'pattern()' was prepared with 'k_FLAG_UTF8'.  On
        // success:
        //
        //: 1 Load the first element of the specified 'result' with,
        //:   respectively, a '(offset, length)' pair or a 'bslstl::StringRef'
        //:   indicating the leftmost match of 'pattern()'.
        //:
        //: 2 Load elements of 'result' in the range '[1 .. numSubpatterns()]'
        //:   with, respectively, a '(offset, length)' pair or a
        //:   'bslstl::StringRef' indicating the respective matches of
        //:   sub-patterns (unmatched sub-patterns have their respective
        //:   'result' elements loaded with either the '(k_INVALID_OFFSET, 0)'
        //:   pair or an empty 'bslstl::StringRef'); sub-patterns matching
        //:   multiple times have their respective 'result' elements loaded
        //:   with the pairs or 'bslstl::StringRef' indicating the rightmost
        //:   match, and return 0.
        //
        // Otherwise, return:
        //
        //: o 1 if 'depthLimit()' was exceeded
        //:
        //: o 2 if memory available for the JIT stack is not large enough
        //:     (applicable only if 'pattern()' was prepared with 'k_FLAG_JIT')
        //:
        //: o another non-zero value, for example, if 'pattern()' was prepared
        //:   with 'k_FLAG_UTF8', but 'subject' is not valid UTF-8
        //
        // 'result' is unchanged if a non-zero value is returned.  The behavior
        // is undefined unless 'true == isPrepared()',
        // 'subject || 0 == subjectLength', and
        // 'subjectOffset <= subjectLength'.  Note that JIT optimization is
        // disabled if 'pattern()' was prepared with 'k_FLAG_UTF8'; use
        // 'matchRaw' if JIT is preferred and UTF-8 validation of 'subject' is
        // not required.  Also note that after a successful call, 'result' will
        // contain exactly 'numSubpatterns() + 1' elements.

    int match(bsl::vector<bsl::string_view>      *result,
              const bsl::string_view&             subject,
              size_t                              subjectOffset = 0) const;
    int match(std::vector<bsl::string_view>      *result,
              const bsl::string_view&             subject,
              size_t                              subjectOffset = 0) const;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    int match(std::pmr::vector<bsl::string_view> *result,
              const bsl::string_view&             subject,
              size_t                              subjectOffset = 0) const;
#endif
        // Match the specified 'subject' against 'pattern()'.  Begin matching
        // at the optionally specified 'subjectOffset' in 'subject'.  If
        // 'subjectOffset' is not specified, matching begins at the start of
        // 'subject'.  UTF-8 validity checking is performed on 'subject' if
        // 'pattern()' was prepared with 'k_FLAG_UTF8'.  On success:
        //
        //: 1 Load the first element of the specified 'result' with a
        //:   'bsl::string_view' indicating the leftmost match of 'pattern()'.
        //:
        //: 2 Load elements of 'result' in the range '[1 .. numSubpatterns()]'
        //:   with a 'bsl::string_view' indicating the respective matches of
        //:   sub-patterns (unmatched sub-patterns have their respective
        //:   'result' elements loaded with an empty 'bsl::string_view');
        //:   sub-patterns matching multiple times have their respective
        //:   'result' elements loaded with a 'bsl::string_view' indicating the
        //:   rightmost match, and return 0.
        //
        // Otherwise, return:
        //
        //: o 1 if 'depthLimit()' was exceeded
        //:
        //: o 2 if memory available for the JIT stack is not large enough
        //:     (applicable only if 'pattern()' was prepared with 'k_FLAG_JIT')
        //:
        //: o another non-zero value, for example, if 'pattern()' was prepared
        //:   with 'k_FLAG_UTF8', but 'subject' is not valid UTF-8
        //
        // 'result' is unchanged if a non-zero value is returned.  The behavior
        // is undefined unless 'true == isPrepared()' and
        // 'subjectOffset <= subject.length()'.  Note that JIT optimization is
        // disabled if 'pattern()' was prepared with 'k_FLAG_UTF8'; use
        // 'matchRaw' if JIT is preferred and UTF-8 validation of 'subject' is
        // not required.  Also note that after a successful call, 'result'
        // will contain exactly 'numSubpatterns() + 1' elements.

    int matchRaw(const bsl::string_view& subject,
                 size_t                  subjectOffset = 0) const;
        // Match the specified 'subject' against 'pattern()'.  Begin matching
        // at the optionally specified 'subjectOffset' in 'subject'.  If
        // 'subjectOffset' is not specified, matching begins at the start of
        // 'subject'.  Return:
        //
        //: o 0 on success
        //:
        //: o 1 if 'depthLimit()' was exceeded
        //:
        //: o 2 if memory available for the JIT stack is not large enough
        //:     (applicable only if 'pattern()' was prepared with 'k_FLAG_JIT')
        //:
        //: o another non-zero value, otherwise
        //
        // The behavior is undefined unless 'true == isPrepared()',
        // 'subjectOffset <= subject.length()', and 'subject' is valid UTF-8 if
        // 'pattern()' was prepared with 'k_FLAG_UTF8'.

    int matchRaw(const char             *subject,
                 size_t                  subjectLength,
                 size_t                  subjectOffset = 0) const;
        // Match the specified 'subject' having the specified 'subjectLength'
        // against 'pattern()'.  Begin matching at the optionally specified
        // 'subjectOffset' in 'subject'.  If 'subjectOffset' is not specified,
        // matching begins at the start of 'subject'.  'subject' may contain
        // embedded null characters.  Return:
        //
        //: o 0 on success
        //:
        //: o 1 if 'depthLimit()' was exceeded
        //:
        //: o 2 if memory available for the JIT stack is not large enough
        //:     (applicable only if 'pattern()' was prepared with 'k_FLAG_JIT')
        //:
        //: o another non-zero value, otherwise
        //
        // The behavior is undefined unless 'true == isPrepared()',
        // 'subject || 0 == subjectLength', 'subjectOffset <= subjectLength',
        //  and 'subject' is valid UTF-8 if 'pattern()' was prepared with
        // 'k_FLAG_UTF8'.

    int matchRaw(bsl::pair<size_t, size_t> *result,
                 const char                *subject,
                 size_t                     subjectLength,
                 size_t                     subjectOffset = 0) const;
    int matchRaw(bsl::string_view          *result,
                 const char                *subject,
                 size_t                     subjectLength,
                 size_t                     subjectOffset = 0) const;
        // Match the specified 'subject' having the specified 'subjectLength'
        // against 'pattern()'.  Begin matching at the optionally specified
        // 'subjectOffset' in 'subject'.  If 'subjectOffset' is not specified,
        // matching begins at the start of 'subject'.  'subject' may contain
        // embedded null characters.  Return:
        //
        //: o 0 on success and load the specified 'result' with, respectively,
        //:   a '(offset, length)' pair or a 'bsl::string_view' indicating the
        //:   leftmost match of 'pattern()'
        //:
        //: o 1 if 'depthLimit()' was exceeded
        //:
        //: o 2 if memory available for the JIT stack is not large enough
        //:     (applicable only if 'pattern()' was prepared with 'k_FLAG_JIT')
        //:
        //: o another non-zero value, otherwise
        //
        // 'result' is unchanged if a non-zero value is returned.  The behavior
        // is undefined unless 'true == isPrepared()',
        // 'subject || 0 == subjectLength', 'subjectOffset <= subjectLength',
        // and 'subject' is valid UTF-8 if 'pattern()' was prepared with
        // 'k_FLAG_UTF8'.

    int matchRaw(bsl::string_view        *result,
                 const bsl::string_view&  subject,
                 size_t                   subjectOffset = 0) const;
        // Match the specified 'subject' against 'pattern()'.  Begin matching
        // at the optionally specified 'subjectOffset' in 'subject'.  If
        // 'subjectOffset' is not specified, matching begins at the start of
        // 'subject'.  Return:
        //
        //: o 0 on success and load the specified 'result' with a
        //:   'bsl::string_view' indicating the leftmost match of 'pattern()'
        //:
        //: o 1 if 'depthLimit()' was exceeded
        //:
        //: o 2 if memory available for the JIT stack is not large enough
        //:     (applicable only if 'pattern()' was prepared with 'k_FLAG_JIT')
        //:
        //: o another non-zero value, otherwise
        //
        // 'result' is unchanged if a non-zero value is returned.  The behavior
        // is undefined unless 'true == isPrepared()',
        // 'subjectOffset <= subject.length()', and 'subject' is valid UTF-8 if
        // 'pattern()' was prepared with 'k_FLAG_UTF8'.

    int matchRaw(bsl::vector<bsl::pair<size_t, size_t> > *result,
                 const char                              *subject,
                 size_t                                   subjectLength,
                 size_t                                   subjectOffset = 0)
                                                                         const;
    int matchRaw(bsl::vector<bslstl::StringRef>          *result,
                 const char                              *subject,
                 size_t                                   subjectLength,
                 size_t                                   subjectOffset = 0)
                                                                         const;
        // Match the specified 'subject' having the specified 'subjectLength'
        // against 'pattern()'.  Begin matching at the optionally specified
        // 'subjectOffset' in 'subject'.  If 'subjectOffset' is not specified,
        // matching begins at the start of 'subject'.  'subject' may contain
        // embedded null characters.  On success:
        //
        //: 1 Load the first element of the specified 'result' with,
        //:   respectively, a '(offset, length)' pair or a 'bslstl::StringRef'
        //:   indicating the leftmost match of 'pattern()'.
        //:
        //: 2 Load elements of 'result' in the range '[1 .. numSubpatterns()]'
        //:   with, respectively, a '(offset, length)' pair or a
        //:   'bslstl::StringRef' indicating the respective matches of
        //:   sub-patterns (unmatched sub-patterns have their respective
        //:   'result' elements loaded with either the '(k_INVALID_OFFSET, 0)'
        //:   pair or an empty 'bslstl::StringRef'); sub-patterns matching
        //:   multiple times have their respective 'result' elements loaded
        //:   with the pairs or 'bslstl::StringRef' indicating the rightmost
        //:   match, and return 0.
        //
        // Otherwise, return:
        //
        //: o 1 if 'depthLimit()' was exceeded
        //:
        //: o 2 if memory available for the JIT stack is not large enough
        //:     (applicable only if 'pattern()' was prepared with 'k_FLAG_JIT')
        //:
        //: o another non-zero value
        //
        // 'result' is unchanged if a non-zero value is returned.  The behavior
        // is undefined unless 'true == isPrepared()',
        // 'subject || 0 == subjectLength', 'subjectOffset <= subjectLength',
        // and 'subject' is valid UTF-8 if 'pattern()' was prepared with
        // 'k_FLAG_UTF8'.  Note that after a successful call, 'result' will
        // contain exactly 'numSubpatterns() + 1' elements.

    int matchRaw(bsl::vector<bsl::string_view>           *result,
                 const bsl::string_view&                  subject,
                 size_t                                   subjectOffset = 0)
                                                                         const;
    int matchRaw(std::vector<bsl::string_view>           *result,
                 const bsl::string_view&                  subject,
                 size_t                                   subjectOffset = 0)
                                                                         const;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    int matchRaw(std::pmr::vector<bsl::string_view>      *result,
                 const bsl::string_view&                  subject,
                 size_t                                   subjectOffset = 0)
                                                                        const;
#endif
        // Match the specified 'subject' against 'pattern()'.  Begin matching
        // at the optionally specified 'subjectOffset' in 'subject'.  If
        // 'subjectOffset' is not specified, matching begins at the start of
        // 'subject'.  On success:
        //
        //: 1 Load the first element of the specified 'result' with a
        //:   'bsl::string_view' indicating the leftmost match of 'pattern()'.
        //:
        //: 2 Load elements of 'result' in the range '[1 .. numSubpatterns()]'
        //:   with a 'bsl::string_view' indicating the respective matches of
        //:   sub-patterns (unmatched sub-patterns have their respective
        //:   'result' elements loaded with an empty 'bsl::string_view');
        //:   sub-patterns matching multiple times have their respective
        //:   'result' elements loaded with a 'bsl::string_view' indicating the
        //:   rightmost match, and return 0.
        //
        // Otherwise, return:
        //
        //: o 1 if 'depthLimit()' was exceeded
        //:
        //: o 2 if memory available for the JIT stack is not large enough
        //:     (applicable only if 'pattern()' was prepared with 'k_FLAG_JIT')
        //:
        //: o another non-zero value
        //
        // 'result' is unchanged if a non-zero value is returned.  The behavior
        // is undefined unless 'true == isPrepared()',
        // 'subjectOffset <= subject.length()', and 'subject' is valid UTF-8 if
        // 'pattern()' was prepared with 'k_FLAG_UTF8'.  Also note that after a
        // successful call, 'result' will contain exactly
        // 'numSubpatterns() + 1' elements.

    void namedSubpatterns(
                 bsl::vector<bsl::pair<bsl::string_view, int> > *result) const;
    void namedSubpatterns(
                 std::vector<std::pair<bsl::string_view, int> > *result) const;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    void namedSubpatterns(
            std::pmr::vector<std::pair<bsl::string_view, int> > *result) const;
#endif
        // Load into the specified 'result' the mapping between the sub-pattern
        // names and their indices.  The names are in alphabetical order.  If
        // duplicate named groups were enabled for this regular expression (see
        // component documentation for {Allow Duplicate Named Groups
        // (sub-patterns)} then a sub-pattern name may appear multiple times.
        // The behavior is undefined unless 'isPrepared()' is 'true'.

    int numSubpatterns() const;
        // Return the number of sub-patterns in the pattern held by this
        // regular-expression object ('pattern()').  The behavior is undefined
        // unless 'isPrepared() == true'.

    const bsl::string& pattern() const;
        // Return a reference to the non-modifiable pattern held by this
        // regular-expression object.  The behavior is undefined unless
        // 'isPrepared() == true'.

    int replace(bsl::string             *result,
                int                     *errorOffset,
                const bsl::string_view&  subject,
                const bsl::string_view&  replacement,
                size_t                   options = 0) const;
    int replace(std::string             *result,
                int                     *errorOffset,
                const bsl::string_view&  subject,
                const bsl::string_view&  replacement,
                size_t                   options = 0) const;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    int replace(std::pmr::string        *result,
                int                     *errorOffset,
                const bsl::string_view&  subject,
                const bsl::string_view&  replacement,
                size_t                   options = 0) const;
#endif
        // Replace parts of the specified 'subject' that are matched with the
        // specified 'replacement'.  Optionally specify a bit mask of 'options'
        // flags that configure the behavior of the replacement.  'options'
        // should contain a bit-wise OR of the 'k_REPLACE_*' constants defined
        // by this class, which indicate additional configuration parameters
        // for the replacement.  If 'options' has 'k_REPLACE_GLOBAL' flag then
        // this function iterates over 'subject', replacing every matching
        // substring.  If 'k_REPLACE_GLOBAL' flag is not set, only the first
        // matching substring is replaced.  UTF-8 validity checking is
        // performed on 'subject' and 'replacement' if 'pattern()' was prepared
        // with 'k_FLAG_UTF8'.  Return the number of substitutions that were
        // carried out on success, and load the specified 'result' with the
        // result of the replacement.  Otherwise, if an error occurs, return a
        // negative value.  If that error is a syntax error in 'replacement',
        // load the specified 'errorOffset' (if non-null) with the offset in
        // 'replacement' where the error was detected;  for other errors, such
        // as invalid 'subject' or 'replacement' UTF-8 string, load
        // 'errorOffset' with a negative value.  The behavior is undefined
        // unless 'true == isPrepared()'.  Note that if the size of 'result' is
        // too small to fit the resultant string then this method computes the
        // size of 'result' and adjusts it to the size that is needed.  To
        // avoid automatic calculation and adjustment which may introduce a
        // performance penalty, it is recommended that the size of 'result' has
        // enough room to fit the resulting string including a zero-terminating
        // character.

    int replaceRaw(bsl::string             *result,
                   int                     *errorOffset,
                   const bsl::string_view&  subject,
                   const bsl::string_view&  replacement,
                   size_t                   options = 0) const;
    int replaceRaw(std::string             *result,
                   int                     *errorOffset,
                   const bsl::string_view&  subject,
                   const bsl::string_view&  replacement,
                   size_t                   options = 0) const;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    int replaceRaw(std::pmr::string        *result,
                   int                     *errorOffset,
                   const bsl::string_view&  subject,
                   const bsl::string_view&  replacement,
                   size_t                   options = 0) const;
#endif
        // Replace parts of the specified 'subject' that are matched with the
        // specified 'replacement'.  Optionally specify a bit mask of 'options'
        // flags that configure the behavior of the replacement.  'options'
        // should contain a bit-wise OR of the 'k_REPLACE_*' constants defined
        // by this class, which indicate additional configuration parameters
        // for the replacement.  If 'options' has 'k_REPLACE_GLOBAL' flag then
        // this function iterates over 'subject', replacing every matching
        // substring.  If 'k_REPLACE_GLOBAL' flag is not set, only the first
        // matching substring is replaced.  UTF-8 validity checking is
        // performed on 'subject' if 'pattern()' was prepared with
        // 'k_FLAG_UTF8'.  Return the number of substitutions that were carried
        // out on success, and load the specified 'result' with the result of
        // the replacement.  Otherwise, if an error occurs, return a negative
        // value.  If that error is a syntax error in 'replacement', load the
        // specified 'errorOffset' (if non-null) with the offset in
        // 'replacement' where the error was detected;  for other errors, such
        // as invalid 'subject' UTF-8 string, load 'errorOffset' with a
        // negative value.  The behavior is undefined unless
        // 'true == isPrepared()'.  Note that if the size of 'result' is too
        // small to fit the resultant string then this method computes the size
        // of 'result' and adjusts it to the size that is needed.  To avoid
        // automatic calculation and adjustment which may introduce a
        // performance penalty, it is recommended that the size of 'result' has
        // enough room to fit the resulting string including a zero-terminating
        // character.

    int subpatternIndex(const char *name) const;
        // Return the 1-based index of the sub-pattern having the specified
        // 'name' in the pattern held by this regular-expression object
        // ('pattern()'); return -1 if 'pattern()' does not contain a
        // sub-pattern identified by 'name' or 'name' is not unique.  The
        // behavior is undefined unless 'isPrepared() == true'.  Note that the
        // returned value is intended to be used as an index into the
        // 'bsl::vector<bsl::pair<int, int> >' returned by 'match'.  Also note
        // that the function 'namedSubpatterns' can be used to find the
        // sub-pattern index when there are duplicate named sub-patterns.
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
    pcre2_compile_context_free(d_compileContext_p);
    pcre2_general_context_free(d_pcre2Context_p);
}

// MANIPULATORS
template <class STRING>
typename bsl::enable_if<   bsl::is_same<STRING, bsl::string>::value
                        || bsl::is_same<STRING, std::string>::value
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                        || bsl::is_same<STRING, std::pmr::string>::value
#endif
                        ,  int>::type
RegEx::prepare(STRING     *errorMessage,
               size_t     *errorOffset,
               const char *pattern,
               int         flags,
               size_t      jitStackSize)
{
    const int  k_BUFFER_LEN = 256;
    char       buffer[k_BUFFER_LEN] = {0};
    size_t     offset;

    int ret = prepareImp(&buffer[0],
                         k_BUFFER_LEN - 1,
                         &offset,
                         pattern,
                         flags,
                         jitStackSize);

    if (ret) {
        if (errorMessage) {
            errorMessage->assign(&buffer[0]);
        }
        if (errorOffset) {
            *errorOffset = offset;
        }
    }

    return ret;
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
    return (0 != d_patternCode_p);
}

inline
size_t RegEx::jitStackSize() const
{
    return d_jitStackSize;
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
