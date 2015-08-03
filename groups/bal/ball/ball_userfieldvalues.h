// ball_userfieldvalues.h                                             -*-C++-*-
#ifndef INCLUDED_BALL_USERFIELDVALUES
#define INCLUDED_BALL_USERFIELDVALUES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container of 'bael_Record' user-field valuees.
//
//@CLASSES:
//  ball::UserFieldValues: a container of 'bael_Record' user-field values
//
//@AUTHOR: Henry Verschell (hversche)
//
//@SEE_ALSO: 
//
//@DESCRIPTION: This component provides a *value* *semantic* container-type,
// 'ball::UserFieldValues', that represents a (randomly accessible) sequence of
// user field values (typically associated with a bael log record).
//

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_USERFIELDVALUE
#include <ball_userfieldvalue.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace ball {


                        // =====================
                        // class UserFieldValues
                        // =====================

class UserFieldValues {

  private:
    // DATA
    bsl::vector<ball::UserFieldValue>  d_values;      

    // FRIENDS
    friend bool operator==(const UserFieldValues&, const UserFieldValues&);

  public:
    // TYPES
    typedef bsl::vector<ball::UserFieldValue>::const_iterator ConstIterator;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(UserFieldValues,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit UserFieldValues(bslma::Allocator *basicAllocator = 0);

    UserFieldValues(const UserFieldValues&  original,
                    bslma::Allocator       *basicAllocator = 0);

    // MANIPULATORS
    UserFieldValues& operator=(const UserFieldValues& rhs);

    void removeAll();

    void appendInt64(int64_t value);
    void appendDouble(double value);
    void appendString(bslstl::StringRef value);
    void appendDatetimeTz(const bdlt::DatetimeTz& value);

    void swap(UserFieldValues& other);

    // ACCESSORS
    bslma::Allocator *allocator() const;

    ConstIterator begin() const;
    ConstIterator end() const;

    int length () const;

    const ball::UserFieldValue& operator[](int index) const;
    const ball::UserFieldValue& value(int index) const;
        

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in
        // a human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute
        // value indicates the number of spaces per indentation level for this
        // and all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the
        // format is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const UserFieldValues& lhs, const UserFieldValues& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'UserFieldValues' objects have the
    // same value if the corresponding value of their 'identifier' attribute is
    // the same and if both store the same sequence of transitions, ordered by
    // time.

bool operator!=(const UserFieldValues& lhs, const UserFieldValues& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'UserFieldValues' objects do not
    // have the same value if their corresponding 'identifier' attribute does
    // not have the same value, or if both do *not* store the same sequence of
    // transitions, ordered by time.

bsl::ostream& operator<<(bsl::ostream&          stream,
                         const UserFieldValues& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// FREE FUNCTIONS
void swap(ball::UserFieldValues& a, ball::UserFieldValues& b);
    // Swap the value of the specified 'a' object with the value of the
    // specified 'b' object.  This method provides the no-throw guarantee.  The
    // behavior is undefined if the two objects being swapped have non-equal
    // allocators.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class UserFieldValues
                        // ---------------------

inline
UserFieldValues::UserFieldValues(bslma::Allocator *basicAllocator)
: d_values(basicAllocator)
{
}

inline
UserFieldValues::UserFieldValues(const UserFieldValues&  original,
                                 bslma::Allocator       *basicAllocator)
: d_values(original.d_values, basicAllocator)
{
}

// MANIPULATORS
inline
UserFieldValues& UserFieldValues::operator=(const UserFieldValues& rhs)
{
    d_values = rhs.d_values;
    return *this;
}

inline
void UserFieldValues::removeAll()
{
    d_values.clear();
}
        
inline
void UserFieldValues::appendInt64(int64_t value)
{
    d_values.push_back(UserFieldValue(value));
}

inline
void UserFieldValues::appendDouble(double value)
{
    d_values.push_back(UserFieldValue(value));
}

inline
void UserFieldValues::appendString(bslstl::StringRef value)
{
    d_values.push_back(UserFieldValue(value));
}

inline
void UserFieldValues::appendDatetimeTz(const bdlt::DatetimeTz& value)
{
    d_values.push_back(UserFieldValue(value));
}

inline
void UserFieldValues::swap(UserFieldValues& other)
{
    d_values.swap(other.d_values);
}

// ACCESSORS
inline
bslma::Allocator *UserFieldValues::allocator() const
{
    return d_values.get_allocator().mechanism();
}

inline
UserFieldValues::ConstIterator UserFieldValues::begin() const
{
    return d_values.begin();
}

inline
UserFieldValues::ConstIterator UserFieldValues::end() const
{
    return d_values.end();
}

inline
int UserFieldValues::length() const
{
    return d_values.size();
}

inline
const UserFieldValue& UserFieldValues::operator[](int index) const
{
    return d_values[index];
}

inline
const UserFieldValue& UserFieldValues::value(int index) const
{
    return d_values[index];
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const UserFieldValues& lhs, const UserFieldValues& rhs)
{
    return lhs.d_values == rhs.d_values;
}

inline
bool ball::operator!=(const UserFieldValues& lhs, const UserFieldValues& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& ball::operator<<(bsl::ostream&          stream, 
                               const UserFieldValues& rhs)
{
    return rhs.print(stream, 0, -1);
}


// FREE FUNCTIONS
inline
void swap(ball::UserFieldValues& a, ball::UserFieldValues& b)
{
    a.swap(b);
}

}  // close namespace BloombergLP


#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
