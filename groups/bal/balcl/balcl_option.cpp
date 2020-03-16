// balcl_option.cpp                                                   -*-C++-*-
#include <balcl_option.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balcl_option_cpp,"$Id$ $CSID$")

#include <balcl_occurrenceinfo.h>
#include <balcl_optioninfo.h>
#include <balcl_optiontype.h>
#include <balcl_optionvalue.h>  // for testing only
#include <balcl_typeinfo.h>

#include <bdlb_print.h>

#include <bslma_default.h>

#include <bslalg_scalarprimitives.h>
#include <bslalg_autoscalardestructor.h>

#include <bsls_assert.h>

#include <bsl_cstring.h> // 'bsl::strlen', 'bsl::strchr'
#include <bsl_ostream.h> // 'bsl::flush'

namespace BloombergLP {
namespace balcl {

                          // ------------
                          // class Option
                          // ------------

// PRIVATE MANIPULATORS
void Option::init()
{
    bslalg::ScalarPrimitives::defaultConstruct(&d_optionInfo.object().d_tag,
                                               d_allocator_p);
    bslalg::AutoScalarDestructor<bsl::string> tagGuard(
                                                 &d_optionInfo.object().d_tag);

    bslalg::ScalarPrimitives::defaultConstruct(&d_optionInfo.object().d_name,
                                               d_allocator_p);
    bslalg::AutoScalarDestructor<bsl::string> nameGuard(
                                                &d_optionInfo.object().d_name);

    bslalg::ScalarPrimitives::defaultConstruct(
                                          &d_optionInfo.object().d_description,
                                          d_allocator_p);
    bslalg::AutoScalarDestructor<bsl::string> descGuard(
                                         &d_optionInfo.object().d_description);

    bslalg::ScalarPrimitives::defaultConstruct(
                                            &d_optionInfo.object().d_typeInfo,
                                            d_allocator_p);
    bslalg::AutoScalarDestructor<TypeInfo> typeGuard(
                                            &d_optionInfo.object().d_typeInfo);

    bslalg::ScalarPrimitives::defaultConstruct(
                                          &d_optionInfo.object().d_defaultInfo,
                                          d_allocator_p);
    typeGuard.release();
    descGuard.release();
    nameGuard.release();
    tagGuard.release();
}

void Option::init(const OptionInfo& optionInfo)
{
    bslalg::ScalarPrimitives::copyConstruct(&d_optionInfo.object().d_tag,
                                            optionInfo.d_tag,
                                            d_allocator_p);
    bslalg::AutoScalarDestructor<bsl::string> tagGuard(
                                                 &d_optionInfo.object().d_tag);

    bslalg::ScalarPrimitives::copyConstruct(&d_optionInfo.object().d_name,
                                            optionInfo.d_name,
                                            d_allocator_p);
    bslalg::AutoScalarDestructor<bsl::string> nameGuard(
                                                &d_optionInfo.object().d_name);

    bslalg::ScalarPrimitives::copyConstruct(
                                          &d_optionInfo.object().d_description,
                                          optionInfo.d_description,
                                          d_allocator_p);
    bslalg::AutoScalarDestructor<bsl::string> descGuard(
                                         &d_optionInfo.object().d_description);

    bslalg::ScalarPrimitives::copyConstruct(&d_optionInfo.object().d_typeInfo,
                                            optionInfo.d_typeInfo,
                                            d_allocator_p);
    bslalg::AutoScalarDestructor<TypeInfo> typeGuard(
                                            &d_optionInfo.object().d_typeInfo);

    bslalg::ScalarPrimitives::copyConstruct(
                                          &d_optionInfo.object().d_defaultInfo,
                                          optionInfo.d_defaultInfo,
                                          d_allocator_p);
    typeGuard.release();
    descGuard.release();
    nameGuard.release();
    tagGuard.release();

    // Erase trailing "=...." from the name.

    bsl::string::size_type posEqual = d_optionInfo.object().d_name.find('=');

    if (posEqual != bsl::string::npos) {
        d_optionInfo.object().d_name.erase(posEqual, bsl::string::npos);
    }
}

// CREATORS
Option::Option()
: d_allocator_p(bslma::Default::allocator(0))
{
    init();
}

Option::Option(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    init();
}

Option::Option(const Option&     original,
               bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    init(original);
}

Option::Option(const OptionInfo&  optionInfo,
               bslma::Allocator  *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    init(optionInfo);
}

Option::~Option()
{
    bslalg::ScalarDestructionPrimitives::destroy(&d_optionInfo.object());
}

// MANIPULATORS
Option& Option::operator=(const Option& rhs)
{
    const OptionInfo& optionInfo = rhs;
    return *this = optionInfo;
}

Option& Option::operator=(const OptionInfo& rhs)
{
    if (&d_optionInfo.object() != &rhs) {
        d_optionInfo.object().d_tag         = rhs.d_tag;
        d_optionInfo.object().d_name        = rhs.d_name;
        d_optionInfo.object().d_description = rhs.d_description;
        d_optionInfo.object().d_typeInfo    = rhs.d_typeInfo;
        d_optionInfo.object().d_defaultInfo = rhs.d_defaultInfo;
    }
    return *this;
}

// ACCESSORS
Option::operator const OptionInfo&() const
{
    return d_optionInfo.object();
}

OptionInfo::ArgType Option::argType() const
{
    // An empty tag is the only indicator of a non-option so that must be
    // tested first.  An 'OptionType::e_BOOL' option should not have an empty
    // tag but the class is allowed to contain tags that are considered invalid
    // (i.e., 'isTagValid' returns 'false').

    if (d_optionInfo.object().d_tag.empty()) {
        return OptionInfo::e_NON_OPTION;                              // RETURN
    }

    if (OptionType::e_BOOL == d_optionInfo.object().d_typeInfo.type()) {
        return OptionInfo::e_FLAG;                                    // RETURN
    }

    return OptionInfo::e_OPTION;
}

const bsl::string& Option::description() const
{
    return d_optionInfo.object().d_description;
}

bool Option::isArray() const
{
    return OptionType::isArrayType(d_optionInfo.object().d_typeInfo.type());
}

const char *Option::longTag() const
{
    BSLS_ASSERT(argType() != OptionInfo::e_NON_OPTION);

    const char *tagString = d_optionInfo.object().d_tag.c_str();
    return '|' == tagString[1] ? tagString + 2 : tagString;
}

const bsl::string& Option::name() const
{
    return d_optionInfo.object().d_name;
}

const OccurrenceInfo& Option::occurrenceInfo() const
{
    return d_optionInfo.object().d_defaultInfo;
}

char Option::shortTag() const
{
    BSLS_ASSERT(argType() != OptionInfo::e_NON_OPTION);

    const char *tagString = d_optionInfo.object().d_tag.c_str();
    return '|' == tagString[1] ? *tagString : 0;
}

const bsl::string& Option::tagString() const
{
    return d_optionInfo.object().d_tag;
}

const TypeInfo& Option::typeInfo() const
{
    return d_optionInfo.object().d_typeInfo;
}

                                  // Validators

bool Option::isDescriptionValid(bsl::ostream& stream) const
{
    if (d_optionInfo.object().d_description.empty()) {
        stream << "Description cannot be an empty string." << '\n'
               << bsl::flush;
        return false;                                                 // RETURN
    }
    return true;
}

bool Option::isLongTagValid(const char    *longTag,
                            bsl::ostream&  stream) const
{
    bool result = true;  // attempt to give all possible error messages

    if ('-' == *longTag) {
        stream << "Long tag cannot begin with '-'." << '\n';
        result = false;
    }

    if (0 != bsl::strchr(longTag, '|')) {
        stream << "Long tag cannot contain '|'." << '\n';
        result = false;
    }

    if (0 != bsl::strchr(longTag, ' ')) {
        stream << "Long tag cannot contain spaces." << '\n';
        result = false;
    }

    if (bsl::strlen(longTag) < 2) {
        stream << "Long tag must be 2 or more characters." << '\n';
        result = false;
    }

    stream << bsl::flush;

    return result;
}

bool Option::isNameValid(bsl::ostream& stream) const
{
    if (d_optionInfo.object().d_name.empty()) {
        stream << "Name cannot be an empty string." << '\n' << bsl::flush;
        return false;                                                 // RETURN
    }
    return true;
}

bool Option::isTagValid(bsl::ostream& stream) const
{
    bool        result = true;   // attempt to give all possible error messages
    const char *str    = d_optionInfo.object().d_tag.c_str();

    if (0 == *str) {  // non-option argument
        if (OptionType::e_BOOL == d_optionInfo.object().d_typeInfo.type()) {
            stream << "Flags cannot have an empty tag." << '\n' << bsl::flush;
            return false;                                             // RETURN
        }
        return true;                                                  // RETURN
    }

    if (0 != bsl::strchr(str, ' ')) {  // for all tag types
        stream << "Tag cannot contain spaces." << '\n' << bsl::flush;
        result = false;
    }

    if (0 == bsl::strchr(str, '|')) {
        // Long-tag only: can have multiple '-' (except in first position), and
        // no '|' in the name.

                                                           // no short-circuit!
        return isLongTagValid(str, stream) && result;                 // RETURN
    }

    // Short tag: must be specified as "s|long", where 's' is not '|'.

    if ('|' == *str) {
        if (0 != bsl::strchr(str + 1, '|')) {
            stream << "Too many '|' in tag string." << '\n' << bsl::flush;
            result = false;
        } else {
            stream << "Short tag cannot be empty if '|' present." << '\n'
                   << bsl::flush;
            result = false;
        }
                                                           // no short-circuit!
        return isLongTagValid(str + 1, stream) && result;             // RETURN
    }

    if ('-' == *str) {
        stream << "Short tag cannot be '-'." << '\n' << bsl::flush;
        result = false;
    }

    if ('|' != str[1]) {
        stream << "Short tag must be exactly one character, followed by '|'."
               << '\n' << bsl::flush;
        return false;                                                 // RETURN
    }

    if (0 == str[1]) {
        stream << "Short tag must have associated long tag." << '\n'
               << bsl::flush;
        return false;                                                 // RETURN
    }

    return result && isLongTagValid(str + 2, stream);
}

                                  // Aspects

bslma::Allocator *Option::allocator() const
{
    return d_allocator_p;
}

bsl::ostream& Option::print(bsl::ostream& stream,
                            int           level,
                            int           spacesPerLevel) const
{
    BSLS_ASSERT(0 <= spacesPerLevel);

    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    if (level < 0) {
        level = -level;
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    stream << "{";
    bdlb::Print::newlineAndIndent(stream, ++level, spacesPerLevel);
    if (tagString().empty()) {
        stream << "NON_OPTION";
    } else {
        stream << "TAG            \"" << tagString() << "\"";
    }
    bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "NAME           \"" << name() << "\"";
    bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "DESCRIPTION    \"" << description() << "\"";
    bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "TYPE_INFO      ";
    typeInfo().print(stream, -2, spacesPerLevel);
    bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "OCCURRENCE_INFO ";
    occurrenceInfo().print(stream, -2, spacesPerLevel);
    bdlb::Print::newlineAndIndent(stream, --level, spacesPerLevel);
    stream << "}";

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bool balcl::operator==(const Option& lhs, const Option& rhs)
{
    return static_cast<const OptionInfo&>(lhs)
        == static_cast<const OptionInfo&>(rhs);
}

bool balcl::operator!=(const Option& lhs, const Option& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& balcl::operator<<(bsl::ostream& stream, const Option& rhs)
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
