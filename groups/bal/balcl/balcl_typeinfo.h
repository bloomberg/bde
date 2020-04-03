// balcl_typeinfo.h                                                   -*-C++-*-
#ifndef INCLUDED_BALCL_TYPEINFO
#define INCLUDED_BALCL_TYPEINFO

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class describing an option's type and other attributes.
//
//@CLASSES:
//   balcl::TypeInfo: attribute type describing command-line option features
//   balcl::TypeInfoUtil: utility functions for 'balcl::TypeInfo'
//
//@SEE_ALSO: balcl_commandline, balcl_optiontype
//
//@DESCRIPTION: This component provides a single (value-semantic) attribute
// class, 'balcl::TypeInfo', that is used to describe several features of a
// command-line option.  Specifically:
//: o The type of an option's value (see {'balcl_optiontype'}).
//: o Optional: The address of a linked variable to hold an option's value.
//: o Optional: The address of a function (see {'balcl_constraint'}) that
//:   imposes a user-defined constraint on an option's value.
//
// For further details see {'balcl_commandline'|Type-and-Constraint Field} and
// {'balcl_commandline'|Example: Type-and-Constraint Field}.
//
///Usage
///-----
// The intended use of this component is illustrated in
// {'balcl_commandline'|Usage}.

#include <balscm_version.h>

#include <balcl_constraint.h>
#include <balcl_optiontype.h>

#include <bdlb_printmethods.h>  // 'bdlb::HasPrintMethod'

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_types.h>     // 'bsls::Types::Int64'

#include <bsl_iosfwd.h>
#include <bsl_memory.h>     // 'bsl::shared_ptr'
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {

namespace bdlt { class Date; }
namespace bdlt { class Time; }
namespace bdlt { class Datetime; }

namespace balcl {

class OptionValue;
class TypeInfoConstraint;

                        // ==============
                        // class TypeInfo
                        // ==============

class TypeInfo {
    // This 'class' is a attribute class that describes the type, the variable
    // to be linked, and the constraint on an option.  Note that the constraint
    // type is opaque, but it is possible to apply the constraint to an element
    // of the same type as the option and see whether it is valid (using the
    // 'satisfiesConstraint' methods of 'TypeInfoUtil').

    // DATA
    OptionType::Enum     d_elemType;          // type of the option value

    void                *d_linkedVariable_p;  // variable to be linked (held)

    bsl::shared_ptr<TypeInfoConstraint>
                         d_constraint_p;      // constraint on the option value

    bslma::Allocator    *d_allocator_p;       // memory allocator (held)

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TypeInfo, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(TypeInfo, bdlb::HasPrintMethod);

    // CREATORS
    TypeInfo();
        // Construct an object having 'string' type for the associated option
        // that uses the currently installed default allocator to supply
        // memory.  No variable is linked and no constraint is put on the
        // value.

    explicit
    TypeInfo(bslma::Allocator *basicAllocator);
        // Construct an object having 'string' type for the associated option,
        // and the specified 'basicAllocator' to supply memory.  No variable is
        // linked and no constraint is put on the value.  Note that,
        // atypically, 0 is disallowed for 'basicAllocator'.

    explicit
    TypeInfo(bool             *variable,
             bslma::Allocator *basicAllocator = 0);
        // Construct a flag accepting 'bool' as the type for the associated
        // option (i.e., for the linked variable).  If the specified 'variable'
        // is not 0, then link it with the option.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  No constraint
        // can be put on the option.

    explicit
    TypeInfo(char             *variable,
             bslma::Allocator *basicAllocator = 0);
    TypeInfo(char                              *variable,
             const Constraint::CharConstraint&  constraint,
             bslma::Allocator                  *basicAllocator = 0);
        // Construct an object having 'char' type for the associated option.
        // If the specified 'variable' is not 0, then link it with the option.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Optionally specify a 'constraint' to put on the option.  If
        // 'constraint' is not specified, the option has no constraint.

    explicit
    TypeInfo(int              *variable,
             bslma::Allocator *basicAllocator = 0);
    TypeInfo(int                              *variable,
             const Constraint::IntConstraint&  constraint,
             bslma::Allocator                 *basicAllocator = 0);
        // Construct an object having 'int' type for the associated option.  If
        // the specified 'variable' is not 0, then link it with the option.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Optionally specify a 'constraint' to put on the option.  If
        // 'constraint' is not specified, the option has no constraint.

    explicit
    TypeInfo(bsls::Types::Int64 *variable,
             bslma::Allocator   *basicAllocator = 0);
    TypeInfo(bsls::Types::Int64                 *variable,
             const Constraint::Int64Constraint&  constraint,
             bslma::Allocator                   *basicAllocator = 0);
        // Construct an object having 'bsls::Types::Int64' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option has no
        // constraint.

    explicit
    TypeInfo(double           *variable,
             bslma::Allocator *basicAllocator = 0);
    TypeInfo(double                              *variable,
             const Constraint::DoubleConstraint&  constraint,
             bslma::Allocator                    *basicAllocator = 0);
        // Construct an object having 'double' type for the associated option.
        // If the specified 'variable' is not 0, then link it with the option.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Optionally specify a 'constraint' to put on the option.  If
        // 'constraint' is not specified, the option has no constraint.

    explicit
    TypeInfo(bsl::string      *variable,
             bslma::Allocator *basicAllocator = 0);
    TypeInfo(bsl::string                         *variable,
             const Constraint::StringConstraint&  constraint,
             bslma::Allocator                    *basicAllocator = 0);
        // Construct an object having 'bsl::string' type for the associated
        // option.  If the specified 'variable' is not 0, then link it with the
        // option.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Optionally specify a 'constraint' to put on the
        // option.  If 'constraint' is not specified, the option has no
        // constraint.

    explicit
    TypeInfo(bdlt::Datetime   *variable,
             bslma::Allocator *basicAllocator = 0);
    TypeInfo(bdlt::Datetime                        *variable,
             const Constraint::DatetimeConstraint&  constraint,
             bslma::Allocator                      *basicAllocator = 0);
        // Construct an object having 'bdlt::Datetime' type for the associated
        // option.  If the specified 'variable' is not 0, then link it with the
        // option.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Optionally specify a 'constraint' to put on the
        // option.  If 'constraint' is not specified, the option has no
        // constraint.

    explicit
    TypeInfo(bdlt::Date       *variable,
             bslma::Allocator *basicAllocator = 0);
    TypeInfo(bdlt::Date                        *variable,
             const Constraint::DateConstraint&  constraint,
             bslma::Allocator                  *basicAllocator = 0);
        // Construct an object having 'bdlt::Date' type for the associated
        // option.  If the specified 'variable' is not 0, then link it with the
        // option.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Optionally specify a 'constraint' to put on the
        // option.  If 'constraint' is not specified, the option has no
        // constraint.

    explicit
    TypeInfo(bdlt::Time       *variable,
             bslma::Allocator *basicAllocator = 0);
    TypeInfo(bdlt::Time                        *variable,
             const Constraint::TimeConstraint&  constraint,
             bslma::Allocator                  *basicAllocator = 0);
        // Construct an object having 'bdlt::Time' type for the associated
        // option.  If the specified 'variable' is not 0, then link it with the
        // option.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Optionally specify a 'constraint' to put on the
        // option.  If 'constraint' is not specified, the option has no
        // constraint.

    explicit
    TypeInfo(bsl::vector<char> *variable,
             bslma::Allocator  *basicAllocator = 0);
    TypeInfo(bsl::vector<char>                 *variable,
             const Constraint::CharConstraint&  constraint,
             bslma::Allocator                  *basicAllocator = 0);
        // Construct an object having 'bsl::vector<char>' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option has no
        // constraint.

    explicit
    TypeInfo(bsl::vector<int> *variable,
             bslma::Allocator *basicAllocator = 0);
    TypeInfo(bsl::vector<int>                 *variable,
             const Constraint::IntConstraint&  constraint,
             bslma::Allocator                 *basicAllocator = 0);
        // Construct an object having 'bsl::vector<int>' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option has no
        // constraint.

    explicit
    TypeInfo(bsl::vector<bsls::Types::Int64> *variable,
             bslma::Allocator                *basicAllocator = 0);
    TypeInfo(bsl::vector<bsls::Types::Int64>    *variable,
             const Constraint::Int64Constraint&  constraint,
             bslma::Allocator                   *basicAllocator = 0);
        // Construct an object having 'bsl::vector<bsls::Types::Int64>' type
        // for the associated option.  If the specified 'variable' is not 0,
        // then link it with the option.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  Optionally specify a
        // 'constraint' to put on the option.  If 'constraint' is not
        // specified, the option has no constraint.

    explicit
    TypeInfo(bsl::vector<double> *variable,
             bslma::Allocator    *basicAllocator = 0);
    TypeInfo(bsl::vector<double>                 *variable,
             const Constraint::DoubleConstraint&  constraint,
             bslma::Allocator                    *basicAllocator = 0);
        // Construct an object having 'bsl::vector<double>' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option has no
        // constraint.

    explicit
    TypeInfo(bsl::vector<bsl::string> *variable,
             bslma::Allocator         *basicAllocator = 0);
    TypeInfo(bsl::vector<bsl::string>            *variable,
             const Constraint::StringConstraint&  constraint,
             bslma::Allocator                    *basicAllocator = 0);
        // Construct an object having 'bsl::vector<bsl::string>' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option has no
        // constraint.

    explicit
    TypeInfo(bsl::vector<bdlt::Datetime> *variable,
             bslma::Allocator            *basicAllocator = 0);
    TypeInfo(bsl::vector<bdlt::Datetime>           *variable,
             const Constraint::DatetimeConstraint&  constraint,
             bslma::Allocator                      *basicAllocator = 0);
        // Construct an object having 'bsl::vector<bdlt::Datetime>' type for
        // the associated option.  If the specified 'variable' is not 0, then
        // link it with the option.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option has no
        // constraint.

    explicit
    TypeInfo(bsl::vector<bdlt::Date> *variable,
             bslma::Allocator        *basicAllocator = 0);
    TypeInfo(bsl::vector<bdlt::Date>           *variable,
             const Constraint::DateConstraint&  constraint,
             bslma::Allocator                  *basicAllocator = 0);
        // Construct an object having 'bsl::vector<bdlt::Date>' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option has no
        // constraint.

    explicit
    TypeInfo(bsl::vector<bdlt::Time> *variable,
             bslma::Allocator        *basicAllocator = 0);
    TypeInfo(bsl::vector<bdlt::Time>           *variable,
             const Constraint::TimeConstraint&  constraint,
             bslma::Allocator                  *basicAllocator = 0);
        // Construct an object having 'bsl::vector<bdlt::Time>' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option has no
        // constraint.

    TypeInfo(const TypeInfo&   original,
             bslma::Allocator *basicAllocator = 0);
        // Create an object having the value of the specified 'original'
        // object.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Note that this object shares the same constraint
        // as the 'original' object.

    ~TypeInfo();
        // Destroy this object.

    // MANIPULATORS
    TypeInfo& operator=(const TypeInfo& rhs);
        // Assign to this object the value of the specified 'rhs' object and
        // return a reference providing modifiable access to this object.

    void resetConstraint();
        // Reset this object so that it no longer has a constraint.  This
        // method has no effect if the object has no constraint.  Note that
        // neither the type attribute nor the linked variable, if any, are
        // changed.

    void resetLinkedVariableAndConstraint();
        // Reset this object so that it has neither a linked variable nor a
        // constraint.  This method has no effect if the object has no linked
        // variable or constraint.  Note that the type attribute is not
        // changed.

    void setConstraint(const Constraint::CharConstraint&     constraint);
    void setConstraint(const Constraint::IntConstraint&      constraint);
    void setConstraint(const Constraint::Int64Constraint&    constraint);
    void setConstraint(const Constraint::DoubleConstraint&   constraint);
    void setConstraint(const Constraint::StringConstraint&   constraint);
    void setConstraint(const Constraint::DatetimeConstraint& constraint);
    void setConstraint(const Constraint::DateConstraint&     constraint);
    void setConstraint(const Constraint::TimeConstraint&     constraint);
        // Put the specified 'constraint' on the value of the described option,
        // replacing any constraint that had been in effect (if any).  The
        // behavior is undefined unless the described option is not a flag and
        // the 'balcl::OptionValue' type of the described option corresponds to
        // the type of 'constraint'.  Note that two distinct objects that have
        // the same 'constraint' put on them will compare *unequal* unless the
        // constraint is shared among them, which can be done by:
        //..
        //  aTypeInfo.setConstraint(anotherTypeInfo.constraint());
        //..

    void
    setConstraint(const bsl::shared_ptr<TypeInfoConstraint>& constraint);
        // Set the constraint of the described option to the specified
        // 'constraint'.  The behavior is undefined unless the option
        // associated with 'constraint' has the same type as the option
        // associated with this object.  Note that the linked variable, if any,
        // is unchanged by this method.

    void setLinkedVariable(bool                            *variable);
    void setLinkedVariable(char                            *variable);
    void setLinkedVariable(int                             *variable);
    void setLinkedVariable(bsls::Types::Int64              *variable);
    void setLinkedVariable(double                          *variable);
    void setLinkedVariable(bsl::string                     *variable);
    void setLinkedVariable(bdlt::Datetime                  *variable);
    void setLinkedVariable(bdlt::Date                      *variable);
    void setLinkedVariable(bdlt::Time                      *variable);
    void setLinkedVariable(bsl::vector<char>               *variable);
    void setLinkedVariable(bsl::vector<int>                *variable);
    void setLinkedVariable(bsl::vector<bsls::Types::Int64> *variable);
    void setLinkedVariable(bsl::vector<double>             *variable);
    void setLinkedVariable(bsl::vector<bsl::string>        *variable);
    void setLinkedVariable(bsl::vector<bdlt::Datetime>     *variable);
    void setLinkedVariable(bsl::vector<bdlt::Date>         *variable);
    void setLinkedVariable(bsl::vector<bdlt::Time>         *variable);
        // Set this object to have the type indicated by the specified
        // 'variable', and reset this object so that it no longer has a
        // constraint associated with it.  If 'variable' is not 0, then link it
        // with the described option.

    // ACCESSORS
    bsl::shared_ptr<TypeInfoConstraint> constraint() const;
        // Return a shared pointer to the (opaque) object storing the
        // constraint associated with the described option.  If this object has
        // no constraint an empty shared pointer is returned.  The lifetime of
        // any shared reference to this object's constraint must not exceed
        // that of this object's allocator.

    void *linkedVariable() const;
        // Return the address of the modifiable variable linked to the
        // described option, or 0 if no variable is linked.

    OptionType::Enum type() const;
        // Return the type of the described option.  Note that the option is a
        // flag if it is of type 'OptionType::e_BOOL'.

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
        // negative.  Note that the precondition on 'spacesPerLevel' is
        // atypical for the 'print' aspect; a negative 'spacesPerLevel'
        // typically indicates that the entire output should be formatted on
        // one line.
};

// FREE OPERATORS
bool operator==(const TypeInfo& lhs, const TypeInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'TypeInfo' objects have the same value if their
    // associated options have the same type, both objects do not have a linked
    // variable or both refer to the same variable, and both do not have a
    // constraint or both refer to the same constraint.  Note that two objects
    // constructed from copies of the same constraint will *not* be identical.
    // (Use 'constraint'/'setConstraint' to ensure that both constraints are
    // the same.)

bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs);
    // Return 'true' if the specified 'lhs' command-line option info has a
    // different value from the specified 'rhs' command-line option info, and
    // 'false' otherwise.  Two 'TypeInfo' objects do not have the same value if
    // the associated options have different types, or one object has a linked
    // variable and the other either does not or refers to a different
    // variable, and one has a constraint and the other either does not or
    // refers to a different constraint.  Note that two objects constructed
    // from copies of the same constraint will *not* be identical.  (Use
    // 'constraint'/'setConstraint' to ensure that both constraints are the
    // same.)

bsl::ostream& operator<<(bsl::ostream& stream, const TypeInfo& rhs);
    // Write the value of the specified 'rhs' object to the specified 'stream'
    // in a (multi-line) human readable format and return a reference to
    // 'stream'.  Note that the last line is *not* terminated by a newline
    // character.

                        // ==================
                        // class TypeInfoUtil
                        // ==================

struct TypeInfoUtil {
    // This utility 'struct' provides a namespace for functions that perform
    // non-primitive operations using 'TypeInfo' objects.

    // CLASS METHODS
    static bool satisfiesConstraint(const OptionValue& element,
                                    const TypeInfo&    typeInfo);
    static bool satisfiesConstraint(const OptionValue& element,
                                    const TypeInfo&    typeInfo,
                                    bsl::ostream&      stream);
        // Return 'true' if the specified 'element' satisfies the constraint of
        // the specified 'typeInfo' object (if any), and 'false' otherwise.
        // Optionally specify a 'stream'; if 'stream' is specified and
        // validation fails, a descriptive error message indicating the reason
        // for the failure is written to 'stream'.  If 'typeInfo' holds no
        // constraint, this method returns 'true'.  The behavior is undefined
        // unless 'element.type() == typeInfo.type()'.

    static bool satisfiesConstraint(const void      *variable,
                                    const TypeInfo&  typeInfo);
    static bool satisfiesConstraint(const void      *variable,
                                    const TypeInfo&  typeInfo,
                                    bsl::ostream&    stream);
        // Return 'true' if the value at the specified 'variable' satisfies the
        // constraint of the specified 'typeInfo' object (if any), and 'false'
        // otherwise.  Optionally specify a 'stream'; if 'stream' is specified
        // and validation fails, a descriptive error message indicating the
        // reason for the failure is written to 'stream'.  If 'typeInfo' holds
        // no constraint, this method returns 'true'.  The behavior is
        // undefined unless 'variable' can be (validly) cast to
        // 'OptionType<ENUM>::EnumToType::type *' where 'ENUM' matches
        // 'typeInfo.type()'.

    static bool parseAndValidate(OptionValue        *element,
                                 const bsl::string&  input,
                                 const TypeInfo&     typeInfo,
                                 bsl::ostream&       stream);
        // Load into the specified 'element' the result of parsing the
        // specified 'input' as a value of the 'element->type()'.  Return
        // 'true' if 'input' is parsed without error and the value satisfies
        // the constraint of the specified 'typeInfo' object (if any), and
        // 'false' with no effect on 'element' otherwise.  If the operation
        // fails a descriptive error message indicating the reason for the
        // failure is written to the specified 'stream.  If 'typeInfo' holds no
        // constraint that validation is considered 'true'; nevertheless, the
        // parse might still fail due to problems with the input format.  The
        // behavior is undefined unless 'element->type() == typeInfo.type()'.
};

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
