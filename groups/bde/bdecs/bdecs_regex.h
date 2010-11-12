// bdecs_regex.h             -*-C++-*-
#ifndef INCLUDED_BDECS_REGEX
#define INCLUDED_BDECS_REGEX

//@PURPOSE: Supply a regular expression matcher.
//
//@DEPRECATED: This component has been replaced with 'bdepcre_regex'.
//
//@CLASSES:
//   bdecs_Regex: efficient implementation of a regular expression matcher
//
//@AUTHOR: Jeffrey Mendelsohn (jmendels)
//
//@DESCRIPTION:
// This component provides a regular expression (RE) matcher.  Clients of
// 'bdecs_Regex' can apply these methods as needed.
//
// The following recursive definition specifies the form of regular
// expressions implemented by 'bdecs_Regex'.
//
// Basic Regular Expressions
//
//   null  The null string is an RE that matches the null string.
//
//   c     Any single character c which is not one of the special characters
//         defined below is an RE that trivially matches c.
//
//   \c    Any of the special characters defined below, or '\' itself, may be
//         \-escaped to negate their special meaning.  Such a \-escaped
//         character is an RE that matches c.
//
//   .     A period (.) is an RE that matches any character except a
//         new-line (\n).
//
//   []    A non-empty string of characters enclosed in square brackets ([])
//         is an RE that matches any one character in that string under the
//         following conditions.  If the first character of the string is a
//         circumflex (^), the RE matches any character excluding a new-line
//         and any of the remaining characters in the string.  The ^ has this
//         special meaning only if it occurs first in the string.  The minus
//         sign (-) may be used to indicate a range of consecutive characters;
//         for example, [0-9] is a short-hand for [0123456789].  The - loses
//         this special meaning if it occurs first (after an initial ^, if any)
//         or last in the string.  The right square bracket (]) does not
//         terminate such a string if it occurs as the first character within
//         it (after an initial ^, if any); for example, []a-f-] matches either
//         a right square bracket (]), one of the ASCII letters 'a' through 'f'
//         inclusive, or the minus sign (-).
//
// Postfix Operators
//
//   *     If R is an RE that matches string r, then R* is an RE that matches
//         zero or more occurrences of r.
//
//   +     If R is an RE that matches string r, then R+ is an RE that matches
//         one or more occurrences of r.
//
//   ?     If R is an RE that matches string r, then R? is an RE that matches
//         zero or one occurrence of r.
//
//   {m} {m,} {m,u}
//         If R is an RE that matches string r, then is an RE that R{m}, R{m,}
//         and R{m,u} are REs that match some number of repetitions of r as
//         follows.  m and u are non-negative integer values less than 256
//         which indicate the number of times the preceding regular expression
//         is to be applied.  The value m is the minimum number and u is a the
//         maximum number (m <= u).  If only m is present (i.e., {m}), it
//         indicates the exact number of times the regular expression is to be
//         applied.  The form {m,} is analogous to {m,infinity}.  Note that R*
//         and R+, respectively, are equivalent to R{0,} and R{1,},
//         respectively.
//
// Concatenation
//
//         If R and S are REs that match strings r and s, respectively,
//         then RS is an RE which matches the string rs.
//
// Alternation
//
//   |     If R and S are REs that match strings r and s, respectively,
//         then R|S is an RE that matches strings r and s.
//
// Grouping
//
//   ()    Parentheses are used for grouping.  If R is an RE that matches
//         string r, then (R) is an RE that matches string r.
//
// Anchors
//
//   ^     Matches the beginning of the string.  A ^ that appears in a context
//         in which it can't match the beginning of the string is ignored
//         (e.g., "a|b^c").
//
//   $     Matches the end of the string; \n matches a newline.  A $ that
//         appears in a context in which it can't match the end of the string
//         is ignored (e.g., "a|b$c").
//
// Precedence and Associativity
//
//         The postfix operators have the highest precedence followed by
//         concatenation (juxtaposition).  Alternation (|) has the lowest
//         precedence.  The postfix operators associate from left-to-right.
//
///USAGE
///-----
// The following snippets of code illustrate how to create and use a
// 'bdecs_Regex'.  First create a 'bdecs_Regex' 're' initialized with a
// regular expression that describes North American-style telephone numbers.
// Let the area code be optional and explicitly disallow any leading or
// trailing characters (i.e., that are not part of the telephone number).
//..
//
//      bdecs_Regex re("^([0-9]{3}-|\\([0-9]{3}\\) )?[0-9]{3}-[0-9]{4}$");
//..
// Now test some character strings to determine if they ascribe to the
// format expected by 're'.
//..
//      assert(1 == re.isMatch("111-222-3333"));
//      assert(0 == re.isMatch("111-222 3333"));
//      assert(0 == re.isMatch("111 222-3333"));
//      assert(0 == re.isMatch(" 111-222-3333"));
//      assert(0 == re.isMatch("111-222-3333 "));
//
//      assert(1 == re.isMatch("(111) 222-3333"));
//      assert(0 == re.isMatch("(111)-222-3333"));
//      assert(0 == re.isMatch("(111) 222 3333"));
//      assert(0 == re.isMatch("(111)-222 3333"));
//      assert(0 == re.isMatch("(111 222-3333"));
//      assert(0 == re.isMatch("111) 222-3333"));
//      assert(0 == re.isMatch(" (111) 222-3333"));
//      assert(0 == re.isMatch("(111) 222-3333 "));
//
//      assert(1 == re.isMatch("222-3333"));
//      assert(0 == re.isMatch("222 3333"));
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_STRING
#include <bdet_string.h>
#endif

#ifndef INCLUDED_BDECS_FINITEAUTOMATON
#include <bdecs_finiteautomaton.h>
#endif

#ifndef INCLUDED_BDECS_FINITEAUTOMATONUTIL
#include <bdecs_finiteautomatonutil.h>
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>
#define INCLUDED_CSTRING
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

namespace BloombergLP {

                           // =================
                           // class bdecs_Regex
                           // =================

class bdecs_Regex {
    // This class represents a regular expression and provides an accessor to
    // determine if a specified input is matched by this regular expression.

    bdecs_FiniteAutomaton  d_finiteAutomaton;
    bdema_Allocator       *d_allocator_p;

public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: replaced by 'maxSupportedBdexVersion()'

    // CREATORS
    bdecs_Regex(bdema_Allocator *basicAllocator = 0);
        // Create a regular expression that matches everything.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    bdecs_Regex(const bdecs_Regex&  original,
                bdema_Allocator    *basicAllocator = 0);
        // Create a regular expression having the identical matching
        // requirements of the specified 'original' regular expression.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    bdecs_Regex(const char      *strExpression,
                bdema_Allocator *basicAllocator = 0);
        // Create a regular expression having the matching requirements of the
        // specified C-style string 'strExpression'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    bdecs_Regex(const char      *expression,
                int              length,
                bdema_Allocator *basicAllocator = 0);
        // Create a regular expression having the matching requirements of the
        // specified 'expression' of specified 'length'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~bdecs_Regex();
        // Destroy this object.

    // MANIPULATORS
    bdecs_Regex& operator=(const bdecs_Regex& rhs);
        // Assign to this regular expression the exact matching requirements of
        // the specified 'rhs' regular expression and return a reference to
        // this modifiable regular expression.

    bdecs_Regex& operator+=(const bdecs_Regex& rhs);
        // Append to this regular expression the matching requirements of the
        // specified 'rhs' regular expression and return a reference to this
        // modifiable regular expression.

    bdecs_Regex& operator|=(const bdecs_Regex& rhs);
        // Assign to this regular expression the logical 'or' of this regular
        // expression with the specified 'rhs' regular expression and return a
        // reference to this modifiable regular expression.

    void repeat(int numMatch);
        // Append to this regular expression the requirement that the current
        // requirements of the regular expression must be matched exactly the
        // specified 'numMatch' times.

    void repeat(int minNumMatch, int maxNumMatch);
        // Append to this regular expression the requirement that the current
        // requirements of the regular expression must be matched at least the
        // specified 'minNumMatch' times but no more than the specified
        // 'maxNumMatch'.

    void repeatMin(int numMatch);
        // Append to this regular expression the requirement that the current
        // requirements of the regular expression must be matched at least the
        // specified 'numMatch' times.

    void removeAll();
        // Assign to this regular expression one that accepts no input (an
        // empty input sequence).

    void invalidate();
        // Mark this regular expression as invalid.  This function has no
        // effect if this regular expression is already invalid.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    template <class STREAM>
    STREAM& streamInRaw(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
        //
        // DEPRECATED: replaced by 'bdexStreamIn(stream, version)'

    // ACCESSORS
    int isMatch(const char *data, int length) const;
        // Return 1 if the specified 'data' consisting of the specified
        // 'length' characters is matched by this regular expression, and 0
        // otherwise.

    int isMatch(const char *string) const;
        // Return 1 if the '\0' terminated specified 'string' is matched by
        // this regular expression, and 0 otherwise.

    int isMatch(const bdet_String& string) const;
        // Return 1 if the specified 'string' is matched by this regular
        // expression, and 0 otherwise.

    int isValid() const;
        // Return non-zero if this regular expression is valid, and 0
        // otherwise.

    void print() const;
        // Write this regular expression to 'stdout'.

    std::ostream& streamOut(std::ostream& stream) const;
        // TBD doc

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template <class STREAM>
    STREAM& streamOutRaw(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.
        //
        // DEPRECATED: replaced by 'bdexStreamOut(stream, version)'
};

// FREE OPERATORS
inline
std::ostream& operator<<(std::ostream& stream, const bdecs_Regex& regex);
    // Write to the specified output 'stream' the most current version number
    // of the specified 'regex' followed by the regex value formatted
    // according to the written version, and return a reference to the
    // modifiable 'stream'.  See the package group level documentation for
    // more information on 'bdex' streaming of container types.

inline
bdecs_Regex operator+(const bdecs_Regex& lhs, const bdecs_Regex& rhs);
    // Return a regular expression with the value of the concatenation of the
    // specified 'lhs' and 'rhs' regular expressions.

inline
bdecs_Regex operator|(const bdecs_Regex& lhs, const bdecs_Regex& rhs);
    // Return a regular expression with the value of the logical 'or' of the
    // specified 'lhs' and 'rhs' regular expression.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                                // -------------
                                // CLASS METHODS
                                // -------------

inline
int bdecs_Regex::maxSupportedBdexVersion()
{
    return 1;
}

inline
int bdecs_Regex::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

                                 // --------
                                 // CREATORS
                                 // --------

inline
bdecs_Regex::bdecs_Regex(bdema_Allocator *basicAllocator)
: d_finiteAutomaton(basicAllocator)
, d_allocator_p(basicAllocator)
{
}

inline
bdecs_Regex::bdecs_Regex(const bdecs_Regex&  original,
                         bdema_Allocator    *basicAllocator)
: d_finiteAutomaton(original.d_finiteAutomaton, basicAllocator)
, d_allocator_p(basicAllocator)
{
}

inline
bdecs_Regex::~bdecs_Regex()
{
}

                               // ------------
                               // MANIPULATORS
                               // ------------

inline
bdecs_Regex& bdecs_Regex::operator=(const bdecs_Regex& rhs)
{
    d_finiteAutomaton = rhs.d_finiteAutomaton;
    return *this;
}

inline
bdecs_Regex& bdecs_Regex::operator+=(const bdecs_Regex& rhs)
{
    d_finiteAutomaton += rhs.d_finiteAutomaton;
    return *this;
}

inline
bdecs_Regex& bdecs_Regex::operator|=(const bdecs_Regex& rhs)
{
    d_finiteAutomaton |= rhs.d_finiteAutomaton;
    return *this;
}

inline
void bdecs_Regex::repeat(int numMatch)
{
    bdecs_FiniteAutomatonUtil::repeat(&d_finiteAutomaton, numMatch);
}

inline
void bdecs_Regex::repeat(int minNumMatch, int maxNumMatch)
{
    bdecs_FiniteAutomatonUtil::repeat(&d_finiteAutomaton,
                                      minNumMatch,
                                      maxNumMatch);
}

inline
void bdecs_Regex::repeatMin(int numMatch)
{
    bdecs_FiniteAutomatonUtil::repeatMin(&d_finiteAutomaton, numMatch);
}

inline
void bdecs_Regex::removeAll()
{
    d_finiteAutomaton.removeAll();
}

inline
void bdecs_Regex::invalidate()
{
    d_finiteAutomaton.invalidate();
}

template <class STREAM>
inline
STREAM& bdecs_Regex::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            d_finiteAutomaton.bdexStreamIn(stream, 1);
            if (!stream) {
                return stream;                                   // RETURN
            }
          } break;
          default: {
            stream.invalidate();
          } break;
        }
    }

    return stream;
}

template <class STREAM>
inline
STREAM& bdecs_Regex::streamInRaw(STREAM& stream, int version)
{
    return bdexStreamIn(stream, version);
}

                                 // ---------
                                 // ACCESSORS
                                 // ---------

inline
int bdecs_Regex::isMatch(const bdet_String& string) const
{
    return isMatch(string.cstr(), string.length());
}

inline
int bdecs_Regex::isValid() const
{
    return d_finiteAutomaton.isValid();
}

inline
void bdecs_Regex::print() const
{
    d_finiteAutomaton.print();
}

inline
std::ostream& bdecs_Regex::streamOut(std::ostream& stream) const
{
    return d_finiteAutomaton.streamOut(stream);
}

template <class STREAM>
inline
STREAM& bdecs_Regex::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        d_finiteAutomaton.streamOut(stream, 1);
      } break;
    }
    return stream;
}

template <class STREAM>
inline
STREAM& bdecs_Regex::streamOutRaw(STREAM& stream, int version) const
{
    return bdexStreamOut(stream, version);
}

                                // --------------
                                // FREE OPERATORS
                                // --------------

inline
std::ostream& operator<<(std::ostream& stream, const bdecs_Regex& regex)
{
    return regex.streamOut(stream);
}

inline
bdecs_Regex operator+(const bdecs_Regex& lhs, const bdecs_Regex& rhs)
{
    bdecs_Regex tmp(lhs);
    tmp += rhs;
    return tmp;
}

inline
bdecs_Regex operator|(const bdecs_Regex& lhs, const bdecs_Regex& rhs)
{
    bdecs_Regex tmp(lhs);
    tmp |= rhs;
    return tmp;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
