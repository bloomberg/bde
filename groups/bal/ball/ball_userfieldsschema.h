// ball_userfieldsschema.h                                            -*-C++-*-
#ifndef INCLUDED_BALL_USERFIELDSSCHEMA
#define INCLUDED_BALL_USERFIELDSSCHEMA

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a description for a sequence user field values
//
//@CLASSES:
//  ball::UserFieldsSchema: describe a sequence of user field values
//
//@AUTHOR: Henry Verschell (hversche)
//
//@SEE_ALSO: 
//
//@DESCRIPTION: This component provides a value-semantic container-type,
// 'ball::UserFieldsSchema', that describes a sequence of user supplied
// field values.
//

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_USERFIELDTYPE
#include <ball_userfieldtype.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
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

                        // ======================
                        // class UserFieldsSchema
                        // ======================

class UserFieldsSchema {

    // PRIVATE TYPES
    typedef bsl::unordered_map<bsl::string, int> NameToIndex;

    // DATA
    NameToIndex                            d_nameToIndex;
    bsl::vector<bslstl::StringRef>         d_names;
    bsl::vector<ball::UserFieldType::Enum> d_types;
    
    // FRIENDS
    friend bool operator==(const UserFieldsSchema&, 
                           const UserFieldsSchema&);

  public:
    // TYPES
    BSLMF_NESTED_TRAIT_DECLARATION(UserFieldsSchema,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit UserFieldsSchema(bslma::Allocator *basicAllocator = 0);

    UserFieldsSchema(const UserFieldsSchema&  original,
                         bslma::Allocator    *basicAllocator = 0);

    // MANIPULATORS
    UserFieldsSchema& operator=(const UserFieldsSchema& rhs);

    int appendDescriptor(bslstl::StringRef         name, 
                         ball::UserFieldType::Enum type);
        // Append to the end of this description of user fields, a description
        // for a field having the specified 'name' and specified data 'type'.

    void removeAll();
        // Remove all of the descriptinos of user fields managed by this
        // object. 

    void swap(UserFieldsSchema& other);

    // ACCESSORS
    bslma::Allocator *allocator() const;


    int length () const;
        // Return the number of fields described by this object.

    int indexOf(bslstl::StringRef name) const;
        // Return the index of the field having the specified 'name', if such
        // a field exists, and -1 otherwise.

    bslstl::StringRef name(int index) const;
        // Return the name of the field at the specified 'index'.

    ball::UserFieldType::Enum type(int index) const;   
        // Return the data type of the field at the specified 'index'.

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
bool operator==(const UserFieldsSchema& lhs, 
                const UserFieldsSchema& rhs);

bool operator!=(const UserFieldsSchema& lhs, 
                const UserFieldsSchema& rhs);

bsl::ostream& operator<<(bsl::ostream&           stream,
                         const UserFieldsSchema& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// FREE FUNCTIONS
void swap(ball::UserFieldsSchema& a, ball::UserFieldsSchema& b);
    // Swap the value of the specified 'a' object with the value of the
    // specified 'b' object.  This method provides the no-throw guarantee.  The
    // behavior is undefined if the two objects being swapped have non-equal
    // allocators.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class UserFieldsSchema
                        // ---------------------

inline
UserFieldsSchema::UserFieldsSchema(bslma::Allocator *basicAllocator)
: d_nameToIndex(basicAllocator)
, d_names(basicAllocator)
, d_types(basicAllocator)
{
}

inline
UserFieldsSchema::UserFieldsSchema(const UserFieldsSchema&  original,
                                   bslma::Allocator        *basicAllocator)
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
UserFieldsSchema& UserFieldsSchema::operator=(const UserFieldsSchema& rhs)
{
    UserFieldsSchema tmp(rhs, allocator());
    swap(tmp);
    return *this;
}

inline
int UserFieldsSchema::appendDescriptor(bslstl::StringRef         name,
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
void UserFieldsSchema::removeAll()
{
    d_nameToIndex.clear();
    d_names.clear();
    d_types.clear();
}


inline
void UserFieldsSchema::swap(UserFieldsSchema& other)
{
    d_nameToIndex.swap(other.d_nameToIndex);
    d_names.swap(other.d_names);
    d_types.swap(other.d_types);
}

// ACCESSORS
inline
bslma::Allocator *UserFieldsSchema::allocator() const
{
    return d_names.get_allocator().mechanism();
}

inline
int UserFieldsSchema::indexOf(bslstl::StringRef name) const
{
    NameToIndex::const_iterator it = d_nameToIndex.find(name);
    return it == d_nameToIndex.end() ? -1 : it->second;
}

inline
bslstl::StringRef UserFieldsSchema::name(int index) const
{
    return d_names[index];
}

inline
ball::UserFieldType::Enum UserFieldsSchema::type(int index) const
{
    return d_types[index];
}

inline
int UserFieldsSchema::length() const
{
    return d_names.size();
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const UserFieldsSchema& lhs,
                      const UserFieldsSchema& rhs)
{
    return lhs.d_nameToIndex == rhs.d_nameToIndex 
        && lhs.d_names       == rhs.d_names
        && lhs.d_types       == rhs.d_types;
}

inline
bool ball::operator!=(const UserFieldsSchema& lhs,
                      const UserFieldsSchema& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& ball::operator<<(bsl::ostream&               stream, 
                               const UserFieldsSchema& rhs)
{
    return rhs.print(stream, 0, -1);
}


// FREE FUNCTIONS
inline
void swap(ball::UserFieldsSchema& a, ball::UserFieldsSchema& b)
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
