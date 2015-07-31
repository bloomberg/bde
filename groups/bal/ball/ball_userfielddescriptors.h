// ball_userfielddescriptors.h                                             -*-C++-*-
#ifndef INCLUDED_BALL_USERFIELDDESCRIPTORS
#define INCLUDED_BALL_USERFIELDDESCRIPTORS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container of 'bael_Record' user-field valuees.
//
//@CLASSES:
//  ball::UserFieldDescriptors: a container of 'bael_Record' user-field descriptors
//
//@AUTHOR: Henry Verschell (hversche)
//
//@SEE_ALSO: 
//
//@DESCRIPTION: This component provides a *value* *semantic* container-type,
// 'ball::UserFieldDescriptors', that represents a (randomly accessible) sequence of
// user field descriptors (typically associated with a bael log record).
//

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_USERFIELDTYPE
#include <ball_userfieldtype.h>
#endif

#ifndef INCLUDED_BSL_UNORDERED_MAP
#include <bsl_unordered_map.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace ball {

                        // =====================
                        // class UserFieldDescriptors
                        // =====================

class UserFieldDescriptors {

    typedef bsl::unordered_map<bsl::string, int> NameToIndex;

    NameToIndex                            d_nameToIndex;
    bsl::vector<bslstl::StringRef>         d_names;
    bsl::vector<ball::UserFieldType::Enum> d_types;
    
    // FRIENDS
    friend bool operator==(const UserFieldDescriptors&, 
                           const UserFieldDescriptors&);

  public:
    // TYPES
    BSLALG_DECLARE_NESTED_TRAITS(UserFieldDescriptors,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit UserFieldDescriptors(bslma::Allocator *basicAllocator = 0);

    UserFieldDescriptors(const UserFieldDescriptors&  original,
                         bslma::Allocator       *basicAllocator = 0);

    // MANIPULATORS
    UserFieldDescriptors& operator=(const UserFieldDescriptors& rhs);

    int appendDescriptor(bslstl::StringRef name, 
                          ball::UserFieldType::Enum type);
    void removeAll();

    void swap(UserFieldDescriptors& other);

    // ACCESSORS
    bslma::Allocator *allocator() const;


    int length () const;

    int indexOf(bslstl::StringRef name) const;
    bslstl::StringRef   name(int index) const;
    ball::UserFieldType::Enum type(int index) const;   


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
bool operator==(const UserFieldDescriptors& lhs, 
                const UserFieldDescriptors& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'UserFieldDescriptors' objects have the
    // same value if the corresponding value of their 'identifier' attribute is
    // the same and if both store the same sequence of transitions, ordered by
    // time.

bool operator!=(const UserFieldDescriptors& lhs, 
                const UserFieldDescriptors& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'UserFieldDescriptors' objects do not
    // have the same value if their corresponding 'identifier' attribute does
    // not have the same value, or if both do *not* store the same sequence of
    // transitions, ordered by time.

bsl::ostream& operator<<(bsl::ostream&              stream,
                         const UserFieldDescriptors& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// FREE FUNCTIONS
void swap(ball::UserFieldDescriptors& a, ball::UserFieldDescriptors& b);
    // Swap the value of the specified 'a' object with the value of the
    // specified 'b' object.  This method provides the no-throw guarantee.  The
    // behavior is undefined if the two objects being swapped have non-equal
    // allocators.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class UserFieldDescriptors
                        // ---------------------

inline
UserFieldDescriptors::UserFieldDescriptors(bslma::Allocator *basicAllocator)
: d_nameToIndex(basicAllocator)
, d_names(basicAllocator)
, d_types(basicAllocator)
{
}

inline
UserFieldDescriptors::UserFieldDescriptors(
                                       const UserFieldDescriptors&  original,
                                       bslma::Allocator       *basicAllocator)
: d_nameToIndex(original.d_nameToIndex, basicAllocator)
, d_names(basicAllocator)
, d_types(original.d_types, basicAllocator)
{
    d_names.resize(d_types.size());
    NameToIndex::const_iterator it = d_nameToIndex.begin();
    for (; it != d_nameToIndex.end(); ++it) {
        d_names[it->second] = bslstl::StringRef(it->first.data(),
                                                it->first.size());
    }
}

// MANIPULATORS
inline
UserFieldDescriptors& UserFieldDescriptors::operator=(
                                       const UserFieldDescriptors& rhs)
{
    UserFieldDescriptors tmp(rhs, allocator());
    swap(tmp);
    return *this;
}

inline
int UserFieldDescriptors::appendDescriptor(bslstl::StringRef   name,
                                           ball::UserFieldType::Enum type)
{
    if (d_nameToIndex.end() != d_nameToIndex.find(name)) {
        return -1;                                                    // RETURN
    }    
    bsl::pair<NameToIndex::iterator, bool> result = 
        d_nameToIndex.insert(NameToIndex::value_type(name, d_names.size()));

    BSLS_ASSERT(true == result.second);    
                         
    d_names.push_back(bslstl::StringRef(result.first->first.data(),
                                        result.first->first.size()));
    d_types.push_back(type);
}

inline
void UserFieldDescriptors::removeAll()
{
    d_nameToIndex.clear();
    d_names.clear();
    d_types.clear();
}


inline
void UserFieldDescriptors::swap(UserFieldDescriptors& other)
{
    d_nameToIndex.swap(other.d_nameToIndex);
    d_names.swap(other.d_names);
    d_types.swap(other.d_types);
}

// ACCESSORS
inline
bslma::Allocator *UserFieldDescriptors::allocator() const
{
    return d_names.get_allocator().mechanism();
}

inline
int UserFieldDescriptors::indexOf(bslstl::StringRef name) const
{
    NameToIndex::const_iterator it = d_nameToIndex.find(name);
    return it == d_nameToIndex.end() ? -1 : it->second;
}

inline
bslstl::StringRef UserFieldDescriptors::name(int index) const
{
    return d_names[index];
}

inline
ball::UserFieldType::Enum UserFieldDescriptors::type(int index) const
{
    return d_types[index];
}

inline
int UserFieldDescriptors::length() const
{
    return d_names.size();
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const UserFieldDescriptors& lhs,
                      const UserFieldDescriptors& rhs)
{
    return lhs.d_nameToIndex == rhs.d_nameToIndex 
        && lhs.d_names       == rhs.d_names
        && lhs.d_types       == rhs.d_types;
}

inline
bool ball::operator!=(const UserFieldDescriptors& lhs,
                      const UserFieldDescriptors& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& ball::operator<<(bsl::ostream&               stream, 
                               const UserFieldDescriptors& rhs)
{
    return rhs.print(stream, 0, -1);
}


// FREE FUNCTIONS
inline
void swap(ball::UserFieldDescriptors& a, ball::UserFieldDescriptors& b)
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
