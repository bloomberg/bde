// balcl_occurrenceinfo.h                                             -*-C++-*-
#ifndef INCLUDED_BALCL_OCCURRENCEINFO
#define INCLUDED_BALCL_OCCURRENCEINFO

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type describing requirement and default value of option.
//
//@CLASSES:
//   balcl::OccurrenceInfo: specify option necessity and other attributes
//
//@SEE_ALSO: 'balcl_commandline'
//
//@DESCRIPTION: This component provides a single complex-constrained
// (value-semantic) attribute class, 'balcl::OccurrenceInfo', that describes
// whether a command-line argument must be supplied by the user or whether that
// option is truly optional.  If a command-line option is optional, it may have
// the additional attribute of being "hidden" -- i.e., omitted by the
// 'printUsage' method of 'balcl::CommandLine' (see '{balcl_commandline}').
// Additionally, default values can be provided for non-required options.
//
// For further details see {'balcl_commandline'|Occurrence Information Field}
// and {'balcl_commandline'|Example: Occurrence Information Field}.
//
///Attributes
///----------
//..
//  Attribute      Type               Default Constraints
//  -------------- ------------------ ------- -------------------------------
//  isHiddenFlag   bool               false   isHiddenFlag && !isRequiredFlag
//
//  isRequiredFlag bool               false   none
//
//  defaultValue   balcl::OptionValue e_VOID  e_VOID != type() &&
//                                               (e_BOOL != type()
//                                                false  == isNull()
//                                                false  == isRequiredFlag)
//..
//
///Usage
///-----
// The intended use of this component is illustrated in
// {'balcl_commandline'|Usage}.

#include <balscm_version.h>

#include <balcl_optionvalue.h>

#include <bdlb_printmethods.h> // 'bdlb::HasPrintMethod'

#include <bslmf_nestedtraitdeclaration.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_types.h> // 'bsls::Types::Int64'

#include <bsl_iosfwd.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP  {

namespace bdlt { class Date; }
namespace bdlt { class Time; }
namespace bdlt { class Datetime; }

namespace balcl {
                        // ====================
                        // class OccurrenceInfo
                        // ====================

class OccurrenceInfo {
    // This 'class' is a simple attribute class that describes a command-line
    // option occurrence requirement (i.e., required, optional, or optional but
    // hidden) and default value, if any.

    // DATA
    bool        d_isRequired;    // 'true' if option is required
    bool        d_isHidden;      // 'true' if option is hidden
    OptionValue d_defaultValue;  // default value (if any), or null

  public:
    // TYPES
    enum OccurrenceType {
        e_REQUIRED = 0,  // option is required and not hidden
        e_OPTIONAL = 1,  // option is optional and not hidden
        e_HIDDEN   = 2   // option is optional and     hidden
    };

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(OccurrenceInfo, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(OccurrenceInfo, bdlb::HasPrintMethod);

    // CREATORS
    OccurrenceInfo();
    explicit
    OccurrenceInfo(bslma::Allocator *basicAllocator);
        // Construct an 'OccurrenceInfo' object that describes a command-line
        // option that is optional but not hidden (i.e.,
        // 'e_OPTIONAL == occurrenceType()') and has no default value.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    OccurrenceInfo(OccurrenceType    type,
                   bslma::Allocator *basicAllocator = 0); // IMPLICIT
        // Construct an 'OccurrenceInfo' object that describes a command-line
        // option of the specified 'type' and has no default value.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit
    OccurrenceInfo(char              defaultValue,
                   bslma::Allocator *basicAllocator = 0);
    explicit
    OccurrenceInfo(int               defaultValue,
                   bslma::Allocator *basicAllocator = 0);
    explicit
    OccurrenceInfo(bsls::Types::Int64  defaultValue,
                   bslma::Allocator   *basicAllocator = 0);
    explicit
    OccurrenceInfo(double            defaultValue,
                   bslma::Allocator *basicAllocator = 0);
    explicit
    OccurrenceInfo(const bsl::string&  defaultValue,
                   bslma::Allocator   *basicAllocator = 0);
    explicit
    OccurrenceInfo(const bdlt::Datetime&  defaultValue,
                   bslma::Allocator      *basicAllocator = 0);
    explicit
    OccurrenceInfo(const bdlt::Date&  defaultValue,
                   bslma::Allocator  *basicAllocator = 0);
    explicit
    OccurrenceInfo(const bdlt::Time&  defaultValue,
                   bslma::Allocator  *basicAllocator = 0);
    explicit
    OccurrenceInfo(const bsl::vector<char>&  defaultValue,
                   bslma::Allocator         *basicAllocator = 0);
    explicit
    OccurrenceInfo(const bsl::vector<int>&  defaultValue,
                   bslma::Allocator        *basicAllocator = 0);
    explicit
    OccurrenceInfo(const bsl::vector<bsls::Types::Int64>&  defaultValue,
                   bslma::Allocator                       *basicAllocator = 0);
    explicit
    OccurrenceInfo(const bsl::vector<double>&  defaultValue,
                   bslma::Allocator           *basicAllocator = 0);
    explicit
    OccurrenceInfo(const bsl::vector<bsl::string>&  defaultValue,
                   bslma::Allocator                *basicAllocator = 0);
    explicit
    OccurrenceInfo(const bsl::vector<bdlt::Datetime>&  defaultValue,
                   bslma::Allocator                   *basicAllocator = 0);
    explicit
    OccurrenceInfo(const bsl::vector<bdlt::Date>&  defaultValue,
                   bslma::Allocator               *basicAllocator = 0);
    explicit
    OccurrenceInfo(const bsl::vector<bdlt::Time>&  defaultValue,
                   bslma::Allocator               *basicAllocator = 0);
        // Construct an 'OccurrenceInfo' object that describes a command-line
        // option that is not required or hidden, and that has the specified
        // 'defaultValue'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    OccurrenceInfo(const OccurrenceInfo&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Construct a 'OccurrenceInfo' object having the value of the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~OccurrenceInfo();
        // Destroy this object.

    // MANIPULATORS
    OccurrenceInfo& operator=(const OccurrenceInfo& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setDefaultValue(const OptionValue& defaultValue);
        // Set the type and default value of the associated option to the
        // specified 'defaultValue'.  The behavior is undefined unless the this
        // object describes an optional argument
        // ('e_REQUIRED != occurrenceType()') and 'defaultValue.type()' is
        // neither 'OptionType::e_BOOL' nor 'OptionType::e_VOID nor a null
        // value ('false == defaultValue.isNull()').

    void setHidden();
        // Set the associated option to be hidden.  The behavior is undefined
        // unless the option is optional ('e_REQUIRED != occurrenceType()').

    // ACCESSORS
    const OptionValue& defaultValue() const;
        // Return a 'const' reference to the default value of this object.

    bool hasDefaultValue() const;
        // Return 'true' if this object has a default value, and 'false'
        // otherwise.

    bool isHidden() const;
        // Return 'true' if the described option is hidden (i.e., not printed
        // in the usage string), and 'false' otherwise.

    bool isRequired() const;
        // Return 'true' if the described option is required, and 'false'
        // otherwise.

    OccurrenceType occurrenceType() const;
        // Return the occurrence type of the described option (i.e., required,
        // optional, or hidden).

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator at construction is used.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this object.  If 'level' is negative, suppress indentation of the
        // first line.  If 'stream' is not valid on entry, this operation has
        // no effect.  The behavior is undefined if 'spacesPerLevel' is
        // negative.
};

// FREE OPERATORS
bool operator==(const OccurrenceInfo& lhs, const OccurrenceInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two objects of type 'OccurrenceInfo' have the same
    // value if and only if they have the same occurrence type and either both
    // do not have a default value, or their respective default values have the
    // same type and value.

bool operator!=(const OccurrenceInfo& lhs, const OccurrenceInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do not have the same
    // value, and 'false' otherwise.  Two objects of type 'OccurrenceInfo' do
    // not have the same value if and only if they have different occurrence
    // types, or exactly one has a default value, or else both have a default
    // value but their respective default values have either different types or
    // different values.

bsl::ostream& operator<<(bsl::ostream& stream, const OccurrenceInfo& rhs);
    // Write the value of the specified 'rhs' object to the specified 'stream'
    // in a (multi-line) human readable format and return a reference to the
    // 'stream'.  Note that the last line is *not* terminated by a newline
    // character.

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
