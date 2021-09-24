// balcl_occurrenceinfo.cpp                                           -*-C++-*-
#include <balcl_occurrenceinfo.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balcl_occurrenceinfo_cpp,"$Id$ $CSID$")

#include <balcl_optiontype.h>

#include <bdlb_print.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace balcl {

                    // --------------------
                    // class OccurrenceInfo
                    // --------------------

// CREATORS
OccurrenceInfo::OccurrenceInfo()
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue()
{
}

OccurrenceInfo::OccurrenceInfo(bslma::Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(OccurrenceType    type,
                               bslma::Allocator *basicAllocator)
: d_isRequired(e_REQUIRED == type)
, d_isHidden(  e_HIDDEN   == type)
, d_defaultValue(basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(char              defaultValue,
                               bslma::Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(int               defaultValue,
                               bslma::Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(bsls::Types::Int64  defaultValue,
                               bslma::Allocator   *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(double            defaultValue,
                               bslma::Allocator *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(const bsl::string&  defaultValue,
                               bslma::Allocator   *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
    // Implementation note: Changing the type of 'defaultValue' to
    // 'bsl::string_view' would lead to the need for creating a temporary
    // 'bsl::string' when initializing 'd_defaultValue', so turning
    // 'defaultValue' into a string view will, in the case where a
    // 'const bsl::string&' is passed, result in a conversion to a string view
    // and then back to a string.  'const char *'s, 'std::string's and
    // 'std::pmr::string's can be passed, and will coerce to a 'bsl::string',
    // which would've happened anyway if 'defaultValue' were a string view.
    //
    // So changing 'defaultValue' to a string view would accomplish no gain in
    // the cases of 'const char *', 'std::string', and 'std::pmr::string', and
    // results in a loss in the case of 'bsl::string'.
}

OccurrenceInfo::OccurrenceInfo(const bdlt::Datetime&  defaultValue,
                               bslma::Allocator      *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(const bdlt::Date&  defaultValue,
                               bslma::Allocator  *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(const bdlt::Time&  defaultValue,
                               bslma::Allocator  *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(const bsl::vector<char>&  defaultValue,
                               bslma::Allocator         *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(const bsl::vector<int>&  defaultValue,
                               bslma::Allocator        *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(
                        const bsl::vector<bsls::Types::Int64>&  defaultValue,
                        bslma::Allocator                       *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(const bsl::vector<double>&  defaultValue,
                               bslma::Allocator           *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(const bsl::vector<bsl::string>&  defaultValue,
                               bslma::Allocator                *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(
                            const bsl::vector<bdlt::Datetime>&  defaultValue,
                            bslma::Allocator                   *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(const bsl::vector<bdlt::Date>&  defaultValue,
                               bslma::Allocator               *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(const bsl::vector<bdlt::Time>&  defaultValue,
                               bslma::Allocator               *basicAllocator)
: d_isRequired(false)
, d_isHidden(false)
, d_defaultValue(defaultValue, basicAllocator)
{
}

OccurrenceInfo::OccurrenceInfo(const OccurrenceInfo&  original,
                               bslma::Allocator      *basicAllocator)
: d_isRequired(original.d_isRequired)
, d_isHidden(original.d_isHidden)
, d_defaultValue(original.d_defaultValue, basicAllocator)
{
}

OccurrenceInfo::~OccurrenceInfo()
{
}

// MANIPULATORS
OccurrenceInfo& OccurrenceInfo::operator=(const OccurrenceInfo& rhs)
{
    if (&rhs != this) {
        d_defaultValue = rhs.d_defaultValue;  // strong exception-safety
        d_isRequired   = rhs.d_isRequired;
        d_isHidden     = rhs.d_isHidden;
    }
    return *this;
}

void OccurrenceInfo::setDefaultValue(const OptionValue& defaultValue)
{
    BSLS_ASSERT(!d_isRequired);
    BSLS_ASSERT(OptionType::e_VOID != defaultValue.type());
    BSLS_ASSERT(OptionType::e_BOOL != defaultValue.type());
    BSLS_ASSERT(false              == defaultValue.isNull());

    d_defaultValue = defaultValue;
}

void OccurrenceInfo::setHidden()
{
    BSLS_ASSERT(!d_isRequired);

    d_isHidden = true;
}

// ACCESSORS
const OptionValue& OccurrenceInfo::defaultValue() const
{
    return d_defaultValue;
}

bool OccurrenceInfo::hasDefaultValue() const
{
    return  d_defaultValue.hasNonVoidType();
}

bool OccurrenceInfo::isHidden() const
{
    return d_isHidden;
}

bool OccurrenceInfo::isRequired() const
{
    return d_isRequired;
}

OccurrenceInfo::OccurrenceType OccurrenceInfo::occurrenceType() const
{
    return d_isRequired ? e_REQUIRED : (d_isHidden ? e_HIDDEN
                                                   : e_OPTIONAL);
}

                                  // Aspects

bslma::Allocator *OccurrenceInfo::allocator() const
{
    return d_defaultValue.allocator();
}

bsl::ostream& OccurrenceInfo::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    if (level < 0) {
        level = -level;
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    if (hasDefaultValue()) {
        stream << "{";
        bdlb::Print::newlineAndIndent(stream, ++level, spacesPerLevel);
    }
    const char *typeString;
    switch (occurrenceType()) {
      case OccurrenceInfo::e_REQUIRED: {
        typeString = "REQUIRED";
      } break;
      case OccurrenceInfo::e_HIDDEN: {
        typeString = "HIDDEN";
      } break;
      case OccurrenceInfo::e_OPTIONAL: {
        typeString = "OPTIONAL";
      } break;
      default: {
        typeString = "***UNKNOWN***";
      } break;
    }
    stream << typeString;
    if (hasDefaultValue()) {
        bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "DEFAULT_TYPE  " << defaultValue().type();
        bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "DEFAULT_VALUE ";
        defaultValue().print(stream, -level, -spacesPerLevel);
        bdlb::Print::newlineAndIndent(stream, --level, spacesPerLevel);
        stream << "}";
    }

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bool balcl::operator==(const OccurrenceInfo& lhs, const OccurrenceInfo& rhs)
{
    return lhs.occurrenceType()  == rhs.occurrenceType()
        && lhs.hasDefaultValue() == rhs.hasDefaultValue()
        && (!lhs.hasDefaultValue() ||
                                     lhs.defaultValue() == rhs.defaultValue());
}

bool balcl::operator!=(const OccurrenceInfo& lhs, const OccurrenceInfo& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& balcl::operator<<(bsl::ostream&         stream,
                                const OccurrenceInfo& rhs)
{
    return rhs.print(stream);
}

}  // close enterprise namespace

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
